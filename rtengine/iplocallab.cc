/*
 *  This file is part of RawTherapee.
 *
 *  Copyright (c) 2004-2010 Gabor Horvath <hgabor@rawtherapee.com>
 *
 *  RawTherapee is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RawTherapee is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RawTherapee.  If not, see <http://www.gnu.org/licenses/>.
 *  2016 Jacques Desmis <jdesmis@gmail.com>
 *  2016 Ingo Weyrich <heckflosse@i-weyrich.de>

 */
#include <cmath>
#include <fftw3.h>

#include "improcfun.h"
#include "curves.h"
#include "gauss.h"
#include "iccmatrices.h"
#include "color.h"
#include "rt_math.h"
#include "jaggedarray.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#include "../rtgui/thresholdselector.h"

#include "cplx_wavelet_dec.h"
#include "ciecam02.h"

#define BENCHMARK
#include "StopWatch.h"
#include "guidedfilter.h"

#define TS 64       // Tile size
#define offset 25   // shift between tiles
#define fTS ((TS/2+1))  // second dimension of Fourier tiles
#define blkrad 1    // radius of block averaging

#define epsilon 0.001f/(TS*TS) //tolerance
#define MAXSCOPE 1.25f
#define MINSCOPE 0.025f

#define CLIPC(a) LIM(a, -42000.f, 42000.f)  // limit a and b  to 130 probably enough ?
#define CLIPL(x) LIM(x,0.f,40000.f) // limit L to about L=120 probably enough ?
#define CLIPLOC(x) LIM(x,0.f,32767.f)
#define CLIPLIG(x) LIM(x,-99.5f, 99.5f)
#define CLIPCHRO(x) LIM(x,0.f, 140.f)
#define CLIPRET(x) LIM(x,-99.5f, 99.5f)
#define CLIP1(x) LIM(x, 0.f, 1.f)
//define to prevent crash with old pp3 with integer range 100 instead of double range 1.
#define CLIP24(x) LIM(x, -2., 4.)
#define CLIP04(x) LIM(x, 0.f, 4.f)
#define CLIP42_35(x) LIM(x, 0.42, 3.5)
#define CLIP2_30(x) LIM(x, 0.2, 3.)

#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"


namespace
{

void calcGammaLut(double gamma, double ts, LUTf &gammaLut) {
    double pwr = 1.0 / gamma;
    double gamm = gamma;
    const double gamm2 = gamma;
    rtengine::GammaValues g_a;

    if (gamm2 < 1.0) {
        std::swap(pwr, gamm);
    }

    rtengine::Color::calcGamma(pwr, ts, 0, g_a); // call to calcGamma with selected gamma and slope

    const double start = gamm2 < 1. ? g_a[2] : g_a[3];
    const double add = g_a[4];
    const double mul = 1.0 + g_a[4];

    if (gamm2 < 1.) {
        #pragma omp parallel for schedule(dynamic, 1024)
        for (int i = 0; i < 65536; i++) {
            const double x = rtengine::Color::igammareti(i / 65535.0, gamm, start, ts, mul, add);
            gammaLut[i] = 0.5 * rtengine::CLIP(x * 65535.0);  // CLIP avoid in some case extra values
        }
    } else {
        #pragma omp parallel for schedule(dynamic, 1024)
        for (int i = 0; i < 65536; i++) {
            const double x = rtengine::Color::gammareti(i / 65535.0, gamm, start, ts, mul, add);
            gammaLut[i] = 0.5 * rtengine::CLIP(x * 65535.0);  // CLIP avoid in some case extra values
        }
    }
}

float calcLocalFactor(const float lox, const float loy, const float lcx, const float dx, const float lcy, const float dy, const float ach)
{
//elipse x2/a2 + y2/b2=1
//transition elipsoidal
//x==>lox y==>loy
// a==> dx  b==>dy

    float kelip = dx / dy;
    float belip = sqrt((rtengine::SQR((lox - lcx) / kelip) + rtengine::SQR(loy - lcy)));    //determine position ellipse ==> a and b
    float aelip = belip * kelip;
    float degrad = aelip / dx;
    float ap = rtengine::RT_PI_F / (1.f - ach);
    float bp = rtengine::RT_PI_F - ap;
    return 0.5f * (1.f + xcosf(degrad * ap + bp));  //trigo cos transition

}
float calcLocalFactorrect(const float lox, const float loy, const float lcx, const float dx, const float lcy, const float dy, const float ach)
{
    float eps = 0.0001f;
    float krap = fabs(dx / dy);
    float kx = (lox - lcx);
    float ky = (loy - lcy);
    float ref = 0.f;

    if (fabs(kx / (ky + eps)) < krap) {
        ref = sqrt(rtengine::SQR(dy) * (1.f + rtengine::SQR(kx / (ky + eps))));
    } else {
        ref = sqrt(rtengine::SQR(dx) * (1.f + rtengine::SQR(ky / (kx + eps))));
    }

    float rad = sqrt(rtengine::SQR(kx) + rtengine::SQR(ky));
    float coef = rad / ref;
    float ac = 1.f / (ach - 1.f);
    float fact = ac * (coef - 1.f);
    return fact;

}

}

namespace rtengine

{
extern MyMutex *fftwMutex;

using namespace procparams;

extern const Settings* settings;


struct local_params {
    float yc, xc;
    float ycbuf, xcbuf;
    float lx, ly;
    float lxL, lyT;
    float dxx, dyy;
    float iterat;
    float balance;
    int cir;
    float thr;
    float stru;
    int chro, cont, sens, sensh, senscb, sensbn, senstm, sensex, sensexclu, sensden, senslc, senssf, senshs;
    float clarityml;
    float contresid;
    float blurcbdl;
    float struco;
    float strengrid;
    float struexc;
    float blendmacol;
    float radmacol;
    float chromacol;
    float gammacol;
    float slomacol;
    float radmaexp;
    float chromaexp;
    float gammaexp;
    float slomaexp;
    float softradiusexp;
    float softradiuscol;
    float softradiuscb;
    float softradiusret;
    float softradiustm;
    float blendmaexp;
    float radmaSH;
    float blendmaSH;
    float chromaSH;
    float gammaSH;
    float slomaSH;
    float radmacb;
    float blendmacb;
    float chromacb;
    float gammacb;
    float slomacb;
    float struexp;
    float blurexp;
    float blurcol;
    float blurSH;
    float ligh;
    float lowA, lowB, highA, highB;
    int shamo, shdamp, shiter, senssha, sensv;
    float neig;
    float strng;
    float lcamount;
    double shrad;
    double shblurr;
    double rad;
    double stren;
    int trans;
    float transweak;
    int dehaze;
    bool inv;
    bool invex;
    bool invsh;
    bool curvact;
    bool invrad;
    bool invret;
    bool invshar;
    bool actsp;
    float str;
    int qualmet;
    int qualcurvemet;
    int gridmet;
    int showmaskcolmet;
    int showmaskexpmet;
    int showmaskSHmet;
    int showmaskcbmet;
    int blurmet;
    float noiself;
    float noiself0;
    float noiself2;
    float noiseldetail;
    int noiselequal;
    float noisechrodetail;
    float bilat;
    float noiselc;
    float noisecf;
    float noisecc;
    float mulloc[5];
    float threshol;
//    float chromacb;
    float strengt;
    float gamm;
    float esto;
    float scalt;
    float rewe;
    bool colorena;
    bool blurena;
    bool tonemapena;
    bool retiena;
    bool sharpena;
    bool lcena;
    bool sfena;
    bool cbdlena;
    bool denoiena;
    bool expvib;
    bool exposena;
    bool hsena;
    bool cut_past;
    float past;
    float satur;
    int blac;
    int shcomp;
    int hlcomp;
    int hlcompthr;
    double expcomp;
    float expchroma;
    int excmet;
    int war;
    float adjch;
    int shapmet;
    bool enaColorMask;
    bool enaExpMask;
    bool enaSHMask;
    bool enacbMask;
    int highlihs;
    int shadowhs;
    int radiushs;
    int hltonalhs;
    int shtonalhs;
};

static void SobelCannyLuma(float **sobelL, float **luma, int bfw, int bfh, float radius, bool multiThread = false)
{
    // base of the process to detect shape in complement of deltaE
    // use for calculate Spot reference
    // and for structure of the shape
    // actually , as the program don't use these function, I just create a simple "Canny" near of Sobel. This can be completed after with teta, etc.
    array2D<float> tmL(bfw, bfh);

    //inspired from Chen Guanghua Zhang Xiaolong
    //Sobel Horizontal
    constexpr float GX[3][3] = {
        {1.f, 0.f, -1.f},
        {2.f, 0.f, -2.f},
        {1.f, 0.f, -1.f}
    };

    //Sobel Vertical
    constexpr float GY[3][3] = {
        {1.f, 2.f, 1.f},
        {0.f, 0.f, 0.f},
        {-1.f, -2.f, -1.f}
    };

    if (radius > 0.f) {
        radius = rtengine::max(radius / 2.f, 0.5f);

#ifdef _OPENMP
        #pragma omp parallel if (multiThread)
#endif
        {
            gaussianBlur(luma, tmL, bfw, bfh, radius);
        }
    } else {
        for (int y = 0; y < bfh ; y++) {
            for (int x = 0; x < bfw ; x++) {
                sobelL[y][x] = 0.f;
                tmL[y][x] = luma[y][x];
            }
        }
    }

#ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic, 16) if (multiThread)
#endif

    for (int y = 0; y < bfh ; y++) {
        for (int x = 0; x < bfw ; x++) {
            float sumXL = 0.f;
            float sumYL = 0.f;
            float SUML;

            if (y == 0 || y == bfh - 1) {
                SUML = 0.f;
            } else if (x == 0 || x == bfw - 1) {
                SUML = 0.f;
            } else {
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j += 2) {
                        sumXL += GX[j + 1][i + 1] * tmL[y + i][x + j];
                    }
                }

                for (int i = -1; i < 2; i += 2) {
                    for (int j = -1; j < 2; j++) {
                        sumYL += GY[j + 1][i + 1] * tmL[y + i][x + j];
                    }
                }

                //Edge strength
                SUML = sqrt(SQR(sumXL) + SQR(sumYL));
                //we can add if need teta = atan2 (sumYr, sumXr)
            }

            sobelL[y][x] = CLIPLOC(SUML);
        }
    }
}



static void calcLocalParams(int sp, int oW, int oH, const LocallabParams& locallab, struct local_params& lp, int llColorMask, int llExpMask, int llSHMask, int llcbMask)
{
    int w = oW;
    int h = oH;
    int circr = locallab.spots.at(sp).circrad;
    float streng = ((float)locallab.spots.at(sp).stren) / 100.f;
    float gam = ((float)locallab.spots.at(sp).gamma) / 100.f;
    float est = ((float)locallab.spots.at(sp).estop) / 100.f;
    float scal_tm = ((float)locallab.spots.at(sp).scaltm) / 10.f;
    float rewe = ((float)locallab.spots.at(sp).rewei);
    float strlight = ((float)locallab.spots.at(sp).streng);
    float strucc = locallab.spots.at(sp).struc;

    float thre = locallab.spots.at(sp).thresh;

    if (thre > 8.f || thre < 0.f) {//to avoid artifacts if user does not clear cache with new settings. Can be suppressed after
        thre = 2.f;
    }

    double local_x = locallab.spots.at(sp).locX / 2000.0;
    double local_y = locallab.spots.at(sp).locY / 2000.0;
    double local_xL = locallab.spots.at(sp).locXL / 2000.0;
    double local_yT = locallab.spots.at(sp).locYT / 2000.0;
    double local_center_x = locallab.spots.at(sp).centerX / 2000.0 + 0.5;
    double local_center_y = locallab.spots.at(sp).centerY / 2000.0 + 0.5;
    double local_center_xbuf = 0.0; // Provision
    double local_center_ybuf = 0.0; // Provision
    double local_dxy = locallab.spots.at(sp).iter / 8000.0; //for proxi = 2==> # 1 pixel
    float iterati = (float) locallab.spots.at(sp).iter;
    float balanc = (float) locallab.spots.at(sp).balan;

    if (iterati > 4.f || iterati < 0.2f) {//to avoid artifacts if user does not clear cache with new settings Can be suppressed after
        iterati = 2.f;
    }

    float neigh = float (locallab.spots.at(sp).neigh);
    float chromaPastel = float (locallab.spots.at(sp).pastels)   / 100.0f;
    float chromaSatur  = float (locallab.spots.at(sp).saturated) / 100.0f;
    int local_sensiv = locallab.spots.at(sp).sensiv;
    int local_sensiex = locallab.spots.at(sp).sensiex;

    if (locallab.spots.at(sp).qualityMethod == "enh") {
        lp.qualmet = 1;
    } else if (locallab.spots.at(sp).qualityMethod == "enhden") {
        lp.qualmet = 2;
    }

    if (locallab.spots.at(sp).qualitycurveMethod == "none") {
        lp.qualcurvemet = 0;
    } else if (locallab.spots.at(sp).qualitycurveMethod == "std") {
        lp.qualcurvemet = 1;
    }

    if (locallab.spots.at(sp).gridMethod == "one") {
        lp.gridmet = 0;
    } else if (locallab.spots.at(sp).gridMethod == "two") {
        lp.gridmet = 1;
    }

    lp.showmaskcolmet = llColorMask;
    lp.showmaskexpmet = llExpMask;
    lp.showmaskSHmet = llSHMask;
    lp.showmaskcbmet = llcbMask;
    lp.enaColorMask = locallab.spots.at(sp).enaColorMask && llColorMask == 0 && llExpMask == 0 && llSHMask == 0 && llcbMask == 0;; // Color & Light mask is deactivated if Exposure mask is visible
    lp.enaExpMask = locallab.spots.at(sp).enaExpMask && llExpMask == 0 && llColorMask == 0 && llSHMask == 0 && llcbMask == 0;; // Exposure mask is deactivated if Color & Light mask is visible
    lp.enaSHMask = locallab.spots.at(sp).enaSHMask && llSHMask == 0 && llColorMask == 0 && llExpMask == 0 && llcbMask == 0;; //
    lp.enacbMask = locallab.spots.at(sp).enacbMask && llcbMask == 0 && llColorMask == 0 && llExpMask == 0 && llSHMask == 0;; 


    if (locallab.spots.at(sp).blurMethod == "norm") {
        lp.blurmet = 0;
    } else if (locallab.spots.at(sp).blurMethod == "inv") {
        lp.blurmet = 1;
    } 

    if (locallab.spots.at(sp).spotMethod == "norm") {
        lp.excmet = 0;
    } else if (locallab.spots.at(sp).spotMethod == "exc") {
        lp.excmet = 1;
    }

    if (locallab.spots.at(sp).shape == "ELI") {
        lp.shapmet = 0;
    } else if (locallab.spots.at(sp).shape == "RECT") {
        lp.shapmet = 1;
    }

    float local_noiself = (float)locallab.spots.at(sp).noiselumf;
    float local_noiself0 = (float)locallab.spots.at(sp).noiselumf0;
    float local_noiself2 = (float)locallab.spots.at(sp).noiselumf2;
    float local_noiselc = (float)locallab.spots.at(sp).noiselumc;
    float local_noiseldetail = (float)locallab.spots.at(sp).noiselumdetail;
    int local_noiselequal = locallab.spots.at(sp).noiselequal;
    float local_noisechrodetail = (float)locallab.spots.at(sp).noisechrodetail;
    int local_sensiden = locallab.spots.at(sp).sensiden;

    float local_noisecf = ((float)locallab.spots.at(sp).noisechrof) / 10.f;
    float local_noisecc = ((float)locallab.spots.at(sp).noisechroc) / 10.f;
    float multi[5];

    for (int y = 0; y < 5; y++) {
        multi[y] = ((float) locallab.spots.at(sp).mult[y]);
    }

    float thresho = ((float)locallab.spots.at(sp).threshold);
    float chromcbdl = (float)locallab.spots.at(sp).chromacbdl;

    int local_chroma = locallab.spots.at(sp).chroma;
    int local_sensi = locallab.spots.at(sp).sensi;
    int local_sensibn = locallab.spots.at(sp).sensibn;
    int local_sensitm = locallab.spots.at(sp).sensitm;
    int local_sensiexclu = locallab.spots.at(sp).sensiexclu;
    float structexclude = (float) locallab.spots.at(sp).structexclu;
    int local_sensilc = locallab.spots.at(sp).sensilc;
//    int local_struc = locallab.spots.at(sp).struc;
    int local_warm = locallab.spots.at(sp).warm;
    int local_sensih = locallab.spots.at(sp).sensih;
    int local_dehaze = locallab.spots.at(sp).dehaz;
    int local_sensicb = locallab.spots.at(sp).sensicb;
    float local_clarityml = (float) locallab.spots.at(sp).clarityml;
    float local_contresid = (float) locallab.spots.at(sp).contresid;
    int local_blurcbdl = (float) locallab.spots.at(sp).blurcbdl;
    int local_contrast = locallab.spots.at(sp).contrast;
    float local_lightness = (float) locallab.spots.at(sp).lightness;
    float labgridALowloc = locallab.spots.at(sp).labgridALow;
    float labgridBLowloc = locallab.spots.at(sp).labgridBLow;
    float labgridBHighloc = locallab.spots.at(sp).labgridBHigh;
    float labgridAHighloc = locallab.spots.at(sp).labgridAHigh;
    float strengthgrid = (float) locallab.spots.at(sp).strengthgrid;

    float structcolor = (float) locallab.spots.at(sp).structcol;
    float blendmaskcolor = ((float) locallab.spots.at(sp).blendmaskcol) / 100.f ;
    float radmaskcolor = ((float) locallab.spots.at(sp).radmaskcol);
    float chromaskcolor = ((float) locallab.spots.at(sp).chromaskcol);
    float gammaskcolor = ((float) locallab.spots.at(sp).gammaskcol);
    float slomaskcolor = ((float) locallab.spots.at(sp).slomaskcol);
    float blendmaskexpo = ((float) locallab.spots.at(sp).blendmaskexp) / 100.f ;
    float radmaskexpo = ((float) locallab.spots.at(sp).radmaskexp);
    float chromaskexpo = ((float) locallab.spots.at(sp).chromaskexp);
    float gammaskexpo = ((float) locallab.spots.at(sp).gammaskexp);
    float slomaskexpo = ((float) locallab.spots.at(sp).slomaskexp);
    float softradiusexpo = ((float) locallab.spots.at(sp).softradiusexp);
    float softradiuscolor = ((float) locallab.spots.at(sp).softradiuscol);
    float softradiusreti = ((float) locallab.spots.at(sp).softradiusret);
    float softradiustma = ((float) locallab.spots.at(sp).softradiustm);
    float softradiuscbdl = ((float) locallab.spots.at(sp).softradiuscb);
    float blendmaskSH = ((float) locallab.spots.at(sp).blendmaskSH) / 100.f ;
    float radmaskSH = ((float) locallab.spots.at(sp).radmaskSH);
    float chromaskSH = ((float) locallab.spots.at(sp).chromaskSH);
    float gammaskSH = ((float) locallab.spots.at(sp).gammaskSH);
    float slomaskSH = ((float) locallab.spots.at(sp).slomaskSH);
    float structexpo = (float) locallab.spots.at(sp).structexp;
    float blurexpo = (float) locallab.spots.at(sp).blurexpde;
    float blurcolor = (float) locallab.spots.at(sp).blurcolde;
    float blurSH = (float) locallab.spots.at(sp).blurSHde;
    int local_transit = locallab.spots.at(sp).transit;
    float local_transitweak = (float)locallab.spots.at(sp).transitweak;
    float radius = (float) locallab.spots.at(sp).radius;
    double sharradius = ((double) locallab.spots.at(sp).sharradius);
    sharradius = CLIP42_35(sharradius);
    float lcamount = ((float) locallab.spots.at(sp).lcamount);
    lcamount = CLIP1(lcamount); //to prevent crash with old pp3 integer
    double sharblurr = ((double) locallab.spots.at(sp).sharblur);
    sharblurr = CLIP2_30(sharblurr);//to prevent crash with old pp3 integer
    int local_sensisha = locallab.spots.at(sp).sensisha;
    int local_sharamount = locallab.spots.at(sp).sharamount;
    int local_shardamping = locallab.spots.at(sp).shardamping;
    int local_shariter = locallab.spots.at(sp).shariter;
    bool inverse = locallab.spots.at(sp).invers;
    bool curvacti = locallab.spots.at(sp).curvactiv;
    bool acti = locallab.spots.at(sp).activlum;
    bool cupas = false; // Provision
    int local_sensisf = locallab.spots.at(sp).sensisf;
    bool inverseex = locallab.spots.at(sp).inversex;
    bool inversesh = locallab.spots.at(sp).inverssh;

    bool inverserad = false; // Provision
    bool inverseret = locallab.spots.at(sp).inversret;
    bool inversesha = locallab.spots.at(sp).inverssha;
    double strength = (double) locallab.spots.at(sp).strength;
    float str = (float)locallab.spots.at(sp).str;

    int local_sensihs = locallab.spots.at(sp).sensihs;
    int highhs = locallab.spots.at(sp).highlights;
    int hltonahs = locallab.spots.at(sp).h_tonalwidth;
    int shadhs = locallab.spots.at(sp).shadows;
    int shtonals = locallab.spots.at(sp).s_tonalwidth;
    int radhs = locallab.spots.at(sp).sh_radius;
    float blendmaskcb = ((float) locallab.spots.at(sp).blendmaskcb) / 100.f ;
    float radmaskcb = ((float) locallab.spots.at(sp).radmaskcb);
    float chromaskcb = ((float) locallab.spots.at(sp).chromaskcb);
    float gammaskcb = ((float) locallab.spots.at(sp).gammaskcb);
    float slomaskcb = ((float) locallab.spots.at(sp).slomaskcb);

    lp.cir = circr;
    lp.actsp = acti;
    lp.xc = w * local_center_x;
    lp.yc = h * local_center_y;
    lp.xcbuf = w * local_center_xbuf;
    lp.ycbuf = h * local_center_ybuf;
    lp.lx = w * local_x;
    lp.ly = h * local_y;
    lp.lxL = w * local_xL;
    lp.lyT = h * local_yT;
    lp.chro = local_chroma;
    lp.struco = structcolor;
    lp.strengrid = strengthgrid;
    lp.blendmacol = blendmaskcolor;
    lp.radmacol = radmaskcolor;
    lp.chromacol = chromaskcolor;
    lp.gammacol = gammaskcolor;
    lp.slomacol = slomaskcolor;
    lp.radmaexp = radmaskexpo;
    lp.chromaexp = chromaskexpo;
    lp.gammaexp = gammaskexpo;
    lp.slomaexp = slomaskexpo;
    lp.softradiusexp = softradiusexpo;
    lp.softradiuscol = softradiuscolor;
    lp.softradiusret = softradiusreti;
    lp.softradiuscb = softradiuscbdl;
    lp.softradiustm = softradiustma;
    lp.struexc = structexclude;
    lp.blendmaexp = blendmaskexpo;
    lp.blendmaSH = blendmaskSH;
    lp.radmaSH = radmaskSH;
    lp.chromaSH = chromaskSH;
    lp.gammaSH = gammaskSH;
    lp.slomaSH = slomaskSH;
    lp.blendmacb = blendmaskcb;
    lp.radmacb = radmaskcb;
    lp.chromacb = chromaskcb;
    lp.gammacb = gammaskcb;
    lp.slomacb = slomaskcb;

    lp.struexp = structexpo;
    lp.blurexp = blurexpo;
    lp.blurcol = blurcolor;
    lp.blurSH = blurSH;
    lp.sens = local_sensi;
    lp.sensh = local_sensih;
    lp.dehaze = local_dehaze;
    lp.senscb = local_sensicb;
    lp.clarityml = local_clarityml;
    //printf("lpclari=%f \n", lp.clarityml); 
    lp.contresid = local_contresid;
    lp.blurcbdl = local_blurcbdl;
    lp.cont = local_contrast;
    lp.ligh = local_lightness;
    lp.lowA = labgridALowloc;
    lp.lowB = labgridBLowloc;
    lp.highB = labgridBHighloc;
    lp.highA = labgridAHighloc;

    lp.senssf = local_sensisf;
    lp.strng = strlight;
    lp.neig = neigh;

    if (lp.ligh >= -2.f && lp.ligh <= 2.f) {
        lp.ligh /= 5.f;
    }

    lp.trans = local_transit;
    lp.transweak = local_transitweak;
    lp.rad = radius;
    lp.stren = strength;
    lp.sensbn = local_sensibn;
    lp.sensexclu = local_sensiexclu;
    lp.senslc = local_sensilc;
    lp.lcamount = lcamount;
    lp.inv = inverse;
    lp.invex = inverseex;
    lp.invsh = inversesh;
    lp.curvact = curvacti;
    lp.invrad = inverserad;
    lp.invret = inverseret;
    lp.invshar = inversesha;
    lp.str = str;
    lp.shrad = sharradius;
    lp.shblurr = sharblurr;
    lp.senssha = local_sensisha;
    lp.shamo = local_sharamount;
    lp.shdamp = local_shardamping;
    lp.shiter = local_shariter;
    lp.iterat = iterati;
    lp.balance = balanc;
    lp.dxx = w * local_dxy;
    lp.dyy = h * local_dxy;
    lp.thr = thre;
    lp.stru = strucc;
    lp.noiself = local_noiself;
    lp.noiself0 = local_noiself0;
    lp.noiself2 = local_noiself2;
    lp.noiseldetail = local_noiseldetail;
    lp.noiselequal = local_noiselequal;
    lp.noisechrodetail = local_noisechrodetail;
    lp.noiselc = local_noiselc;
    lp.noisecf = local_noisecf;
    lp.noisecc = local_noisecc;
    lp.sensden = local_sensiden;
    lp.bilat = locallab.spots.at(sp).bilateral;
    lp.adjch = (float) locallab.spots.at(sp).adjblur;
    lp.strengt = streng;
    lp.gamm = gam;
    lp.esto = est;
    lp.scalt = scal_tm;
    lp.rewe = rewe;
    lp.senstm = local_sensitm;

    for (int y = 0; y < 5; y++) {
        lp.mulloc[y] = CLIP04(multi[y]);//to prevent crash with old pp3 integer
    }

    lp.threshol = thresho;
    lp.chromacb = chromcbdl;
    lp.colorena = locallab.spots.at(sp).expcolor && llExpMask == 0 && llSHMask == 0; // Color & Light tool is deactivated if Exposure mask is visible or SHMask
    lp.blurena = locallab.spots.at(sp).expblur;
    lp.tonemapena = locallab.spots.at(sp).exptonemap;
    lp.retiena = locallab.spots.at(sp).expreti;
    lp.sharpena = locallab.spots.at(sp).expsharp;
    lp.lcena = locallab.spots.at(sp).expcontrast;
    lp.sfena = locallab.spots.at(sp).expsoft;
    lp.cbdlena = locallab.spots.at(sp).expcbdl;
    lp.denoiena = locallab.spots.at(sp).expdenoi;
    lp.expvib = locallab.spots.at(sp).expvibrance;
    lp.sensv = local_sensiv;
    lp.past =  chromaPastel;
    lp.satur = chromaSatur;

    lp.exposena = locallab.spots.at(sp).expexpose && llColorMask == 0 && llSHMask == 0; // Exposure tool is deactivated if Color & Light mask SHmask is visible
    lp.cut_past = cupas;
    lp.blac = locallab.spots.at(sp).black;
    lp.shcomp = locallab.spots.at(sp).shcompr;
    lp.hlcomp = locallab.spots.at(sp).hlcompr;
    lp.hlcompthr = locallab.spots.at(sp).hlcomprthresh;
    lp.expcomp = locallab.spots.at(sp).expcomp;
    lp.expcomp = CLIP24(lp.expcomp); //to prevent crash with Old pp3 with integer
    lp.expchroma = locallab.spots.at(sp).expchroma / 100.;
    lp.sensex = local_sensiex;
//    lp.strucc = local_struc;
    lp.war = local_warm;
    lp.hsena = locallab.spots.at(sp).expshadhigh && llColorMask == 0 && llExpMask == 0;// Shadow Highlight tool is deactivated if Color & Light mask or SHmask is visible
    lp.highlihs = highhs;
    lp.shadowhs = shadhs;
    lp.radiushs = radhs;
    lp.hltonalhs = hltonahs;
    lp.shtonalhs = shtonals;
    lp.senshs = local_sensihs;
}



static void calcTransitionrect(const float lox, const float loy, const float ach, const local_params& lp, int &zone, float &localFactor)
{
    zone = 0;

    if (lox >= lp.xc && lox < (lp.xc + lp.lx) && loy >= lp.yc && loy < lp.yc + lp.ly) {
        if (lox < (lp.xc + lp.lx * ach)  && loy < (lp.yc + lp.ly * ach)) {
            zone = 2;
        } else {
            zone = 1;
            localFactor = calcLocalFactorrect(lox, loy, lp.xc, lp.lx, lp.yc, lp.ly, ach);
            localFactor = pow(localFactor, lp.transweak);
        }

    } else if (lox >= lp.xc && lox < lp.xc + lp.lx && loy < lp.yc && loy > lp.yc - lp.lyT) {
        if (lox < (lp.xc + lp.lx * ach) && loy > (lp.yc - lp.lyT * ach)) {
            zone = 2;
        } else {
            zone = 1;
            localFactor = calcLocalFactorrect(lox, loy, lp.xc, lp.lx, lp.yc, lp.lyT, ach);
            localFactor = pow(localFactor, lp.transweak);
        }


    } else if (lox < lp.xc && lox > lp.xc - lp.lxL && loy <= lp.yc && loy > lp.yc - lp.lyT) {
        if (lox > (lp.xc - lp.lxL * ach) && loy > (lp.yc - lp.lyT * ach)) {
            zone = 2;
        } else {
            zone = 1;
            localFactor = calcLocalFactorrect(lox, loy, lp.xc, lp.lxL, lp.yc, lp.lyT, ach);
            localFactor = pow(localFactor, lp.transweak);
        }

    } else if (lox < lp.xc && lox > lp.xc - lp.lxL && loy > lp.yc && loy < lp.yc + lp.ly) {
        if (lox > (lp.xc - lp.lxL * ach) && loy < (lp.yc + lp.ly * ach)) {
            zone = 2;
        } else {
            zone = 1;
            localFactor = calcLocalFactorrect(lox, loy, lp.xc, lp.lxL, lp.yc, lp.ly, ach);
            localFactor = pow(localFactor, lp.transweak);
        }

    }

}



static void calcTransition(const float lox, const float loy, const float ach, const local_params& lp, int &zone, float &localFactor)
{
    // returns the zone (0 = outside selection, 1 = transition zone between outside and inside selection, 2 = inside selection)
    // and a factor to calculate the transition in case zone == 1

    zone = 0;

    if (lox >= lp.xc && lox < (lp.xc + lp.lx) && loy >= lp.yc && loy < lp.yc + lp.ly) {
        float zoneVal = SQR((lox - lp.xc) / (ach * lp.lx)) + SQR((loy - lp.yc) / (ach * lp.ly));
        zone = zoneVal < 1.f ? 2 : 0;

        if (!zone) {
            zone = (zoneVal > 1.f && ((SQR((lox - lp.xc) / (lp.lx)) + SQR((loy - lp.yc) / (lp.ly))) < 1.f)) ? 1 : 0;

            if (zone == 1) {
                localFactor = pow(calcLocalFactor(lox, loy, lp.xc, lp.lx, lp.yc, lp.ly, ach), lp.transweak);
            }
        }
    } else if (lox >= lp.xc && lox < lp.xc + lp.lx && loy < lp.yc && loy > lp.yc - lp.lyT) {
        float zoneVal = SQR((lox - lp.xc) / (ach * lp.lx)) + SQR((loy - lp.yc) / (ach * lp.lyT));
        zone = zoneVal < 1.f ? 2 : 0;

        if (!zone) {
            zone = (zoneVal > 1.f && ((SQR((lox - lp.xc) / (lp.lx)) + SQR((loy - lp.yc) / (lp.lyT))) < 1.f)) ? 1 : 0;

            if (zone == 1) {
                localFactor = pow(calcLocalFactor(lox, loy, lp.xc, lp.lx, lp.yc, lp.lyT, ach), lp.transweak);
            }
        }
    } else if (lox < lp.xc && lox > lp.xc - lp.lxL && loy <= lp.yc && loy > lp.yc - lp.lyT) {
        float zoneVal = SQR((lox - lp.xc) / (ach * lp.lxL)) + SQR((loy - lp.yc) / (ach * lp.lyT));
        zone = zoneVal < 1.f ? 2 : 0;

        if (!zone) {
            zone = (zoneVal > 1.f && ((SQR((lox - lp.xc) / (lp.lxL)) + SQR((loy - lp.yc) / (lp.lyT))) < 1.f)) ? 1 : 0;

            if (zone == 1) {
                localFactor = pow(calcLocalFactor(lox, loy, lp.xc, lp.lxL, lp.yc, lp.lyT, ach), lp.transweak);
            }
        }
    } else if (lox < lp.xc && lox > lp.xc - lp.lxL && loy > lp.yc && loy < lp.yc + lp.ly) {
        float zoneVal = SQR((lox - lp.xc) / (ach * lp.lxL)) + SQR((loy - lp.yc) / (ach * lp.ly));
        zone = zoneVal < 1.f ? 2 : 0;

        if (!zone) {
            zone = (zoneVal > 1.f && ((SQR((lox - lp.xc) / (lp.lxL)) + SQR((loy - lp.yc) / (lp.ly))) < 1.f)) ? 1 : 0;

            if (zone == 1) {
                localFactor = pow(calcLocalFactor(lox, loy, lp.xc, lp.lxL, lp.yc, lp.ly, ach), lp.transweak);
            }
        }
    }
}


void ImProcFunctions::ciecamloc_02float(int sp, LabImage* lab)
{
    //be carefull quasi duplicate with branch cat02wb
    BENCHFUN

    int width = lab->W, height = lab->H;
    float Yw;
    Yw = 1.0f;
    double Xw, Zw;
    float f = 0.f, nc = 0.f, la, c = 0.f, xw, yw, zw, f2 = 1.f, c2 = 1.f, nc2 = 1.f, yb2;
    float fl, n, nbb, ncb, aw; //d
    float xwd, ywd, zwd, xws, yws, zws;
    //  int alg = 0;
    double Xwout, Zwout;
    double Xwsc, Zwsc;

    int tempo;

    if (params->locallab.spots.at(sp).warm > 0) {
        tempo = 5000 - 30 * params->locallab.spots.at(sp).warm;
    } else {
        tempo = 5000 - 49 * params->locallab.spots.at(sp).warm;
    }

    ColorTemp::temp2mulxyz(params->wb.temperature, params->wb.method, Xw, Zw);  //compute white Xw Yw Zw  : white current WB
    ColorTemp::temp2mulxyz(tempo, "Custom", Xwout, Zwout);
    ColorTemp::temp2mulxyz(5000, "Custom", Xwsc, Zwsc);

    //viewing condition for surrsrc
    f  = 1.00f;
    c  = 0.69f;
    nc = 1.00f;
    //viewing condition for surround
    f2 = 1.0f, c2 = 0.69f, nc2 = 1.0f;
    //with which algorithm
    //  alg = 0;


    xwd = 100.f * Xwout;
    zwd = 100.f * Zwout;
    ywd = 100.f;

    xws = 100.f * Xwsc;
    zws = 100.f * Zwsc;
    yws = 100.f;


    yb2 = 18;
    //La and la2 = ambiant luminosity scene and viewing
    la = 400.f;
    const float la2 = 400.f;
    const float pilot = 2.f;
    const float pilotout = 2.f;

    //algoritm's params
    // const float rstprotection = 100. ;//- params->colorappearance.rstprotection;
    LUTu hist16J;
    LUTu hist16Q;
    float yb = 18.f;
    float d, dj;

    // const int gamu = 0; //(params->colorappearance.gamut) ? 1 : 0;
    xw = 100.0f * Xw;
    yw = 100.0f * Yw;
    zw = 100.0f * Zw;
    float xw1 = xws, yw1 = yws, zw1 = zws, xw2 = xwd, yw2 = ywd, zw2 = zwd;

    float cz, wh, pfl;
    Ciecam02::initcam1float(yb, pilot, f, la, xw, yw, zw, n, d, nbb, ncb, cz, aw, wh, pfl, fl, c);
//   const float chr = 0.f;
    const float pow1 = pow_F(1.64f - pow_F(0.29f, n), 0.73f);
    float nj, nbbj, ncbj, czj, awj, flj;
    Ciecam02::initcam2float(yb2, pilotout, f2,  la2,  xw2,  yw2,  zw2, nj, dj, nbbj, ncbj, czj, awj, flj);
#ifdef __SSE2__
    const float reccmcz = 1.f / (c2 * czj);
#endif
    const float pow1n = pow_F(1.64f - pow_F(0.29f, nj), 0.73f);
//    const float QproFactor = (0.4f / c) * (aw + 4.0f) ;
    const bool LabPassOne = true;

#ifdef __SSE2__
    int bufferLength = ((width + 3) / 4) * 4; // bufferLength has to be a multiple of 4
#endif
#ifndef _DEBUG
    #pragma omp parallel
#endif
    {
#ifdef __SSE2__
        // one line buffer per channel and thread
        float Jbuffer[bufferLength] ALIGNED16;
        float Cbuffer[bufferLength] ALIGNED16;
        float hbuffer[bufferLength] ALIGNED16;
        float Qbuffer[bufferLength] ALIGNED16;
        float Mbuffer[bufferLength] ALIGNED16;
        float sbuffer[bufferLength] ALIGNED16;
#endif
#ifndef _DEBUG
        #pragma omp for schedule(dynamic, 16)
#endif

        for (int i = 0; i < height; i++) {
#ifdef __SSE2__
            // vectorized conversion from Lab to jchqms
            int k;
            vfloat x, y, z;
            vfloat J, C, h, Q, M, s;

            vfloat c655d35 = F2V(655.35f);

            for (k = 0; k < width - 3; k += 4) {
                Color::Lab2XYZ(LVFU(lab->L[i][k]), LVFU(lab->a[i][k]), LVFU(lab->b[i][k]), x, y, z);
                x = x / c655d35;
                y = y / c655d35;
                z = z / c655d35;
                Ciecam02::xyz2jchqms_ciecam02float(J, C,  h,
                                                   Q,  M,  s, F2V(aw), F2V(fl), F2V(wh),
                                                   x,  y,  z,
                                                   F2V(xw1), F2V(yw1),  F2V(zw1),
                                                   F2V(c),  F2V(nc), F2V(pow1), F2V(nbb), F2V(ncb), F2V(pfl), F2V(cz), F2V(d));
                STVF(Jbuffer[k], J);
                STVF(Cbuffer[k], C);
                STVF(hbuffer[k], h);
                STVF(Qbuffer[k], Q);
                STVF(Mbuffer[k], M);
                STVF(sbuffer[k], s);
            }

            for (; k < width; k++) {
                float L = lab->L[i][k];
                float a = lab->a[i][k];
                float b = lab->b[i][k];
                float x, y, z;
                //convert Lab => XYZ
                Color::Lab2XYZ(L, a, b, x, y, z);
                x = x / 655.35f;
                y = y / 655.35f;
                z = z / 655.35f;
                float J, C, h, Q, M, s;
                Ciecam02::xyz2jchqms_ciecam02float(J, C,  h,
                                                   Q,  M,  s, aw, fl, wh,
                                                   x,  y,  z,
                                                   xw1, yw1,  zw1,
                                                   c,  nc, pow1, nbb, ncb, pfl, cz, d);
                Jbuffer[k] = J;
                Cbuffer[k] = C;
                hbuffer[k] = h;
                Qbuffer[k] = Q;
                Mbuffer[k] = M;
                sbuffer[k] = s;
            }

#endif // __SSE2__

            for (int j = 0; j < width; j++) {
                float J, C, h, Q, M, s;

#ifdef __SSE2__
                // use precomputed values from above
                J = Jbuffer[j];
                C = Cbuffer[j];
                h = hbuffer[j];
                Q = Qbuffer[j];
                M = Mbuffer[j];
                s = sbuffer[j];
#else
                float x, y, z;
                float L = lab->L[i][j];
                float a = lab->a[i][j];
                float b = lab->b[i][j];
                float x1, y1, z1;
                //convert Lab => XYZ
                Color::Lab2XYZ(L, a, b, x1, y1, z1);
                x = (float)x1 / 655.35f;
                y = (float)y1 / 655.35f;
                z = (float)z1 / 655.35f;
                //process source==> normal
                Ciecam02::xyz2jchqms_ciecam02float(J, C,  h,
                                                   Q,  M,  s, aw, fl, wh,
                                                   x,  y,  z,
                                                   xw1, yw1,  zw1,
                                                   c,  nc, pow1, nbb, ncb, pfl, cz, d);
#endif
                float Jpro, Cpro, hpro, Qpro, Mpro, spro;
                Jpro = J;
                Cpro = C;
                hpro = h;
                Qpro = Q;
                Mpro = M;
                spro = s;
                /*
                */


                //retrieve values C,J...s
                C = Cpro;
                J = Jpro;
                Q = Qpro;
                M = Mpro;
                h = hpro;
                s = spro;

                if (LabPassOne) {
#ifdef __SSE2__
                    // write to line buffers
                    Jbuffer[j] = J;
                    Cbuffer[j] = C;
                    hbuffer[j] = h;
#else
                    float xx, yy, zz;
                    //process normal==> viewing

                    Ciecam02::jch2xyz_ciecam02float(xx, yy, zz,
                                                    J,  C, h,
                                                    xw2, yw2,  zw2,
                                                    c2, nc2,  pow1n, nbbj, ncbj, flj, czj, dj, awj);
                    float x, y, z;
                    x = xx * 655.35f;
                    y = yy * 655.35f;
                    z = zz * 655.35f;
                    float Ll, aa, bb;
                    //convert xyz=>lab
                    Color::XYZ2Lab(x,  y,  z, Ll, aa, bb);
                    lab->L[i][j] = Ll;
                    lab->a[i][j] = aa;
                    lab->b[i][j] = bb;

#endif
                }

                //    }
            }

#ifdef __SSE2__
            // process line buffers
            float *xbuffer = Qbuffer;
            float *ybuffer = Mbuffer;
            float *zbuffer = sbuffer;

            for (k = 0; k < bufferLength; k += 4) {
                Ciecam02::jch2xyz_ciecam02float(x, y, z,
                                                LVF(Jbuffer[k]), LVF(Cbuffer[k]), LVF(hbuffer[k]),
                                                F2V(xw2), F2V(yw2), F2V(zw2),
                                                F2V(nc2), F2V(pow1n), F2V(nbbj), F2V(ncbj), F2V(flj), F2V(dj), F2V(awj), F2V(reccmcz));
                STVF(xbuffer[k], x * c655d35);
                STVF(ybuffer[k], y * c655d35);
                STVF(zbuffer[k], z * c655d35);
            }

            // XYZ2Lab uses a lookup table. The function behind that lut is a cube root.
            // SSE can't beat the speed of that lut, so it doesn't make sense to use SSE
            for (int j = 0; j < width; j++) {
                float Ll, aa, bb;
                //convert xyz=>lab
                Color::XYZ2Lab(xbuffer[j], ybuffer[j], zbuffer[j], Ll, aa, bb);

                lab->L[i][j] = Ll;
                lab->a[i][j] = aa;
                lab->b[i][j] = bb;
            }

#endif
        }

    }
}

void ImProcFunctions::softprocess(const LabImage* bufcolorig, array2D<float> &buflight, float rad, int bfh, int bfw, int sk, bool multiThread)
{
    float minlig = buflight[0][0];

#ifdef _OPENMP
    #pragma omp parallel for reduction(min:minlig) schedule(dynamic,16)
#endif

    for (int ir = 0; ir < bfh; ir++) {
        for (int jr = 0; jr < bfw; jr++) {
            minlig = rtengine::min(buflight[ir][jr], minlig);
        }
    }

    array2D<float> guidsoft(bfw, bfh);

#ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic,16)
#endif

    for (int ir = 0; ir < bfh; ir++) {
        for (int jr = 0; jr < bfw; jr++) {
            buflight[ir][jr] = LIM01((buflight[ir][jr] - minlig) / (100.f - minlig));
            guidsoft[ir][jr] = bufcolorig->L[ir][jr] / 32768.f;
        }
    }

    guidedFilter(guidsoft, buflight, buflight, rad * 10.f / sk, 0.04, multiThread, 4); //lp.softradiuscol

#ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic,16)
#endif

    for (int ir = 0; ir < bfh; ir++) {
        for (int jr = 0; jr < bfw; jr++) {
            buflight[ir][jr] = (100.f - minlig) * buflight[ir][jr] + minlig;
        }
    }
}

void ImProcFunctions::exlabLocal(const local_params& lp, int bfh, int bfw, LabImage* bufexporig, LabImage* lab,  LUTf & hltonecurve, LUTf & shtonecurve, LUTf & tonecurve)
{
    BENCHFUN
    //exposure local

    constexpr float maxran = 65536.f;
    const float exp_scale = pow(2.0, lp.expcomp);
    const float comp = (max(0.0, lp.expcomp) + 1.0) * lp.hlcomp / 100.0;
    const float shoulder = ((maxran / max(1.0f, exp_scale)) * (lp.hlcompthr / 200.0)) + 0.1;
    const float hlrange = maxran - shoulder;


#ifdef _OPENMP
    #pragma omp parallel for
#endif

    for (int ir = 0; ir < bfh; ir++) {
        for (int jr = 0; jr < bfw; jr++) {
            float L = bufexporig->L[ir][jr];
            //highlight
            const float hlfactor = (2 * L < MAXVALF ? hltonecurve[2 * L] : CurveFactory::hlcurve(exp_scale, comp, hlrange, 2 * L));
            L *= hlfactor;
            //shadow tone curve
            const float shfactor = shtonecurve[2 * L];
            //tonecurve
            L *= shfactor;
            lab->L[ir][jr] = 0.5f * tonecurve[2 * L];
        }
 
    }
}


void ImProcFunctions::addGaNoise(LabImage *lab, LabImage *dst, const float mean, const float variance, const int sk)
{
//   BENCHFUN
//Box-Muller method.
// add luma noise to image

    srand(1);

    const float variaFactor = SQR(variance) / sk;
    constexpr float randFactor1 = 1.f / RAND_MAX;
    constexpr float randFactor2 = (2.f * rtengine::RT_PI_F) / RAND_MAX;
#ifdef _OPENMP
    #pragma omp parallel
#endif
    {
        float z0, z1;
        bool generate = false;
#ifdef _OPENMP
        #pragma omp for schedule(static) // static scheduling is important to avoid artefacts
#endif

        for (int y = 0; y < lab->H; y++) {
            for (int x = 0; x < lab->W; x++) {
                generate = !generate;
                float kvar = 1.f;

                if (lab->L[y][x] < 12000.f) {
                    constexpr float ah = -0.5f / 12000.f;
                    constexpr float bh = 1.5f;
                    kvar = ah * lab->L[y][x] + bh;    //increase effect for low lights < 12000.f
                } else if (lab->L[y][x] > 20000.f) {
                    constexpr float ah = -0.5f / 12768.f;
                    constexpr float bh = 1.f - 20000.f * ah;
                    kvar = ah * lab->L[y][x] + bh;    //decrease effect for high lights > 20000.f
                    kvar = kvar < 0.5f ? 0.5f : kvar;
                }

                float varia = SQR(kvar) * variaFactor;

                if (!generate) {
                    dst->L[y][x] = LIM(lab->L[y][x] + mean + varia * z1, 0.f, 32768.f);
                    continue;
                }

                int u1 = 0;
                int u2;

                while (u1 == 0) {
                    u1 = rand();
                    u2 = rand();
                }

                float u1f = u1 * randFactor1;
                float u2f = u2 * randFactor2;

                float2 sincosval = xsincosf(2.f * rtengine::RT_PI_F * u2f);
                float factor = sqrtf(-2.f * xlogf(u1f));
                z0 = factor * sincosval.y;
                z1 = factor * sincosval.x;

                dst->L[y][x] = LIM(lab->L[y][x] + mean + varia * z0, 0.f, 32768.f);

            }
        }
    }
}

static void balancedeltaE(float kL, float &kab)
{
    float mincurs = 0.3f;//minimum slider balan_
    float maxcurs = 1.7f;//maximum slider balan_
    float maxkab = 1.35;//0.5 * (3 - 0.3)
    float minkab = 0.65;//0.5 * (3 - 1.7)
    float abal = (maxkab - minkab) / (mincurs - maxcurs);
    float bbal = maxkab - mincurs * abal;
    kab = abal * kL + bbal;
}

static void calcreducdE(float dE, float maxdE, float mindE, float maxdElim,  float mindElim, float iterat, float limscope, int scope, float &reducdE)
{
    if (dE > maxdE) {
        reducdE = 0.f;
    } else if (dE > mindE && dE <= maxdE) {
        const float ar = 1.f / (mindE - maxdE);
        const float br = - ar * maxdE;
        reducdE = pow(ar * dE + br, iterat);
    } else {
        reducdE = 1.f;
    }

    if (scope > limscope) {//80 arbitrary value, if we change we must change limscope 
        if (dE > maxdElim) {
            reducdE = 0.f;
        } else if (dE > mindElim && dE <= maxdElim) {
            const float arlim = 1.f / (mindElim - maxdElim);
            const float brlim = - arlim * maxdElim;
            const float reducdElim = pow(arlim * dE + brlim, iterat);
            const float aalim = (1.f - reducdElim) / 20.f;
            const float bblim = 1.f - 100.f * aalim;
            reducdE = aalim * scope  +  bblim;
        } else {
          reducdE = 1.f;
        }
    }
}
void ImProcFunctions::DeNoise_Local(int call,  const struct local_params& lp, int levred, float hueref, float lumaref, float chromaref,  LabImage* original, LabImage* transformed, LabImage &tmp1, int cx, int cy, int sk)
{
    //warning, but I hope used it next
    // local denoise and impulse
    //simple algo , perhaps we can improve as the others, but noise is here and not good for hue detection
    // BENCHFUN
    const float ach = (float)lp.trans / 100.f;

    const float factnoise1 = 1.f + (lp.noisecf) / 500.f;
    const float factnoise2 = 1.f + (lp.noisecc) / 500.f;
    const float factnoise = factnoise1 * factnoise2;

    const int GW = transformed->W;
    const int GH = transformed->H;
    const float refa = chromaref * cos(hueref);
    const float refb = chromaref * sin(hueref);

    std::unique_ptr<LabImage> origblur(new LabImage(GW, GH));

    const float radius = 3.f / sk;
#ifdef _OPENMP
    #pragma omp parallel
#endif
    {
        gaussianBlur(original->L, origblur->L, GW, GH, radius);
        gaussianBlur(original->a, origblur->a, GW, GH, radius);
        gaussianBlur(original->b, origblur->b, GW, GH, radius);
    }

    const int begx = int (lp.xc - lp.lxL);
    const int begy = int (lp.yc - lp.lyT);

#ifdef _OPENMP
    #pragma omp parallel if (multiThread)
#endif
    {
        const int limscope = 80;
        const float mindE = 2.f + MINSCOPE * lp.sensden * lp.thr;
        const float maxdE = 5.f + MAXSCOPE * lp.sensden * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

#ifdef _OPENMP
        #pragma omp for schedule(dynamic,16)
#endif

        for (int y = 0; y < transformed->H; y++) {
            const int loy = cy + y;

            const bool isZone0 = loy > lp.yc + lp.ly || loy < lp.yc - lp.lyT; // whole line is zone 0 => we can skip a lot of processing

            if (isZone0) { // outside selection and outside transition zone => no effect, keep original values
                continue;
            }

            for (int x = 0, lox = cx + x; x < transformed->W; x++, lox++) {
                int zone = 0;

                float localFactor = 1.f;

                if (lp.shapmet == 0) {
                    calcTransition(lox, loy, ach, lp, zone, localFactor);
                } else if (lp.shapmet == 1) {
                    calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
                }

                if (zone == 0) { // outside selection and outside transition zone => no effect, keep original values
                    continue;
                }

                float rL = original->L[y][x] / 327.6f;
                float dEL = sqrt(0.9f * SQR(refa - origblur->a[y][x] / 327.6f) + 0.9f * SQR(refb - origblur->b[y][x] / 327.8f) + 1.2f * SQR(lumaref - rL));
                float dEa = sqrt(1.2f * SQR(refa - origblur->a[y][x] / 327.6f) + 1.f * SQR(refb - origblur->b[y][x] / 327.8f) + 0.8f * SQR(lumaref - rL));
                float dEb = sqrt(1.f * SQR(refa - origblur->a[y][x] / 327.6f) + 1.2f * SQR(refb - origblur->b[y][x] / 327.8f) + 0.8f * SQR(lumaref - rL));

                float reducdEL = 1.f;
                float reducdEa = 1.f;
                float reducdEb = 1.f;
                if (levred == 7){
                    calcreducdE(dEL, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, lp.sensden , reducdEL);
                    calcreducdE(dEa, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, lp.sensden , reducdEa);
                    calcreducdE(dEb, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, lp.sensden , reducdEb);
                    reducdEL = SQR(reducdEL);
                    reducdEa = SQR(reducdEa);
                    reducdEb = SQR(reducdEb);

                }

                switch (zone) {
                    case 1: { // inside transition zone
                        float difL, difa, difb;

                        if (call == 2  /*|| call == 1  || call == 3 */) { //simpleprocess
                            difL = tmp1.L[loy - begy][lox - begx] - original->L[y][x];
                            difa = tmp1.a[loy - begy][lox - begx] - original->a[y][x];
                            difb = tmp1.b[loy - begy][lox - begx] - original->b[y][x];
                        } else  { //dcrop
                            difL = tmp1.L[y][x] - original->L[y][x];
                            difa = tmp1.a[y][x] - original->a[y][x];
                            difb = tmp1.b[y][x] - original->b[y][x];

                        }

                        difL *= localFactor * reducdEL;
                        difa *= localFactor * reducdEa;
                        difb *= localFactor * reducdEb;
                        transformed->L[y][x] = CLIP(original->L[y][x] + difL);
                        transformed->a[y][x] = CLIPC((original->a[y][x] + difa) * factnoise);
                        transformed->b[y][x] = CLIPC((original->b[y][x] + difb) * factnoise) ;
                        break;
                    }

                    case 2: { // inside selection => full effect, no transition
                        float difL, difa, difb;

                        if (call == 2 /*|| call == 1 || call == 3 */) { //simpleprocess
                            difL = tmp1.L[loy - begy][lox - begx] - original->L[y][x];
                            difa = tmp1.a[loy - begy][lox - begx] - original->a[y][x];
                            difb = tmp1.b[loy - begy][lox - begx] - original->b[y][x];
                        } else  { //dcrop
                            difL = tmp1.L[y][x] - original->L[y][x];
                            difa = tmp1.a[y][x] - original->a[y][x];
                            difb = tmp1.b[y][x] - original->b[y][x];

                        }

                        difL *= reducdEL;
                        difa *= reducdEa;
                        difb *= reducdEb;

                        transformed->L[y][x] = CLIP(original->L[y][x] + difL);
                        transformed->a[y][x] = CLIPC((original->a[y][x] + difa) * factnoise);
                        transformed->b[y][x] = CLIPC((original->b[y][x] + difb) * factnoise);
                    }
                }

            }
        }
    }
}

void ImProcFunctions::BlurNoise_Local(LabImage *tmp1, const float hueref, const float chromaref, const float lumaref, const local_params & lp, LabImage * original, LabImage * transformed, int cx, int cy, int sk)
{
//local BLUR
    BENCHFUN

    const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
    const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
    const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
    const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);

    const float ach = lp.trans / 100.f;
    const int GW = transformed->W;
    const int GH = transformed->H;
    const float refa = chromaref * cos(hueref) * 327.68f;
    const float refb = chromaref * sin(hueref) * 327.68f;
    const float refL = lumaref * 327.68f;

    //balance deltaE
    float kL = lp.balance;
    float kab = 1.f;
    balancedeltaE(kL, kab);
    kab /= SQR(327.68f);
    kL /= SQR(327.68f);

    std::unique_ptr<LabImage> origblur(new LabImage(GW, GH));

    const float radius = 3.f / sk;
#ifdef _OPENMP
    #pragma omp parallel
#endif
    {
        gaussianBlur(original->L, origblur->L, GW, GH, radius);
        gaussianBlur(original->a, origblur->a, GW, GH, radius);
        gaussianBlur(original->b, origblur->b, GW, GH, radius);
    }

#ifdef _OPENMP
    #pragma omp parallel if (multiThread)
#endif
    {
        const int limscope = 80;
        const float mindE = 4.f + MINSCOPE * lp.sensbn * lp.thr;//best usage ?? with blurnoise
        const float maxdE = 5.f + MAXSCOPE * lp.sensbn * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

#ifdef _OPENMP
        #pragma omp for schedule(dynamic,16)
#endif

        for (int y = ystart; y < yend; y++) {
            const int loy = cy + y;
            for (int x = xstart, lox = cx + x; x < xend; x++, lox++) {
                int zone = 0;
                float localFactor = 1.f;

                if (lp.shapmet == 0) {
                    calcTransition(lox, loy, ach, lp, zone, localFactor);
                } else if (lp.shapmet == 1) {
                    calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
                }

                if (zone == 0) { // outside selection and outside transition zone => no effect, keep original values
                    continue;
                }

                const float dE = sqrt(kab * (SQR(refa - origblur->a[y][x]) + SQR(refb - origblur->b[y][x])) + kL * SQR(refL - origblur->L[y][x]));

                float reducdE;
                calcreducdE(dE, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, lp.sensbn , reducdE);

                const float difL = (tmp1->L[y - ystart][x - xstart] - original->L[y][x]) * localFactor * reducdE;
                transformed->L[y][x] = CLIP(original->L[y][x] + difL);

                if (!lp.actsp) {
                    const float difa = (tmp1->a[y - ystart][x - xstart] - original->a[y][x]) * localFactor * reducdE;;
                    const float difb = (tmp1->b[y - ystart][x - xstart] - original->b[y][x]) * localFactor * reducdE;;
                    transformed->a[y][x] = CLIPC(original->a[y][x] + difa);
                    transformed->b[y][x] = CLIPC(original->b[y][x] + difb);
                }
            }
        }
    }
}

void ImProcFunctions::InverseReti_Local(const struct local_params & lp, const float hueref, const float chromaref,  const float lumaref, LabImage * original, LabImage * transformed, const LabImage * const tmp1, int cx, int cy, int chro, int sk)
{
    // BENCHFUN
//inverse local retinex
    float ach = (float)lp.trans / 100.f;
    int GW = transformed->W;
    int GH = transformed->H;
    float refa = chromaref * cos(hueref);
    float refb = chromaref * sin(hueref);

    //balance deltaE
    float kL = lp.balance;
    float kab = 1.f;
    balancedeltaE(kL, kab);

    LabImage *origblur = new LabImage(GW, GH);

    float radius = 3.f / sk;
#ifdef _OPENMP
    #pragma omp parallel
#endif
    {
        gaussianBlur(original->L, origblur->L, GW, GH, radius);
        gaussianBlur(original->a, origblur->a, GW, GH, radius);
        gaussianBlur(original->b, origblur->b, GW, GH, radius);

    }
#ifdef _OPENMP
    #pragma omp parallel if (multiThread)
#endif
    {
        const int limscope = 80;
        const float mindE = 2.f + MINSCOPE * lp.sensh * lp.thr;
        const float maxdE = 5.f + MAXSCOPE * lp.sensh * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

#ifdef _OPENMP
        #pragma omp for schedule(dynamic,16)
#endif

        for (int y = 0; y < transformed->H; y++) {
            int loy = cy + y;

            for (int x = 0; x < transformed->W; x++) {
                int lox = cx + x;

                int zone;
                float localFactor;

                if (lp.shapmet == 0) {
                    calcTransition(lox, loy, ach, lp, zone, localFactor);
                } else if (lp.shapmet == 1) {
                    calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
                }

                float rL = origblur->L[y][x] / 327.68f;
                float reducdE = 0.f;
                float dE = sqrt(kab * SQR(refa - origblur->a[y][x] / 327.68f) + kab * SQR(refb - origblur->b[y][x] / 327.68f) + kL * SQR(lumaref - rL));
                calcreducdE(dE, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, lp.sensh , reducdE);

                switch (zone) {
                    case 0: { // outside selection and outside transition zone => full effect, no transition
                        if (chro == 0) {
                            float difL = tmp1->L[y][x] - original->L[y][x];
                            transformed->L[y][x] = CLIP(original->L[y][x] + difL * reducdE);

                        }

                        if (chro == 1) {

                            float difa = tmp1->a[y][x] - original->a[y][x];
                            float difb = tmp1->b[y][x] - original->b[y][x];


                            transformed->a[y][x] = CLIPC(original->a[y][x] + difa * reducdE);
                            transformed->b[y][x] = CLIPC(original->b[y][x] + difb * reducdE);
                        }

                        break;
                    }

                    case 1: { // inside transition zone
                        float factorx = 1.f - localFactor;

                        if (chro == 0) {
                            float difL = tmp1->L[y][x] - original->L[y][x];
                            difL *= factorx;
                            transformed->L[y][x] = CLIP(original->L[y][x] + difL * reducdE);
                        }

                        if (chro == 1) {
                            float difa = tmp1->a[y][x] - original->a[y][x];
                            float difb = tmp1->b[y][x] - original->b[y][x];

                            difa *= factorx;
                            difb *= factorx;

                            transformed->a[y][x] = CLIPC(original->a[y][x] + difa * reducdE);
                            transformed->b[y][x] = CLIPC(original->b[y][x] + difb * reducdE);
                        }

                        break;
                    }

                    case 2: { // inside selection => no effect, keep original values
                        if (chro == 0) {
                            transformed->L[y][x] = original->L[y][x];
                        }

                        if (chro == 1) {
                            transformed->a[y][x] = original->a[y][x];
                            transformed->b[y][x] = original->b[y][x];
                        }
                    }
                }
            }
        }
    }
}




void ImProcFunctions::InverseBlurNoise_Local(const struct local_params & lp,  const float hueref, const float chromaref, const float lumaref, LabImage * original, LabImage * transformed, const LabImage * const tmp1, int cx, int cy, int sk)
{
    // BENCHFUN
//inverse local blur and noise
    float ach = (float)lp.trans / 100.f;
    int GW = transformed->W;
    int GH = transformed->H;
    float refa = chromaref * cos(hueref);
    float refb = chromaref * sin(hueref);

    //balance deltaE
    float kL = lp.balance;
    float kab = 1.f;
    balancedeltaE(kL, kab);

    LabImage *origblur = new LabImage(GW, GH);

    float radius = 3.f / sk;
#ifdef _OPENMP
    #pragma omp parallel
#endif
    {
        gaussianBlur(original->L, origblur->L, GW, GH, radius);
        gaussianBlur(original->a, origblur->a, GW, GH, radius);
        gaussianBlur(original->b, origblur->b, GW, GH, radius);

    }
#ifdef _OPENMP
    #pragma omp parallel if (multiThread)
#endif
    {
        const int limscope = 80;
        const float mindE = 2.f + MINSCOPE * lp.sensbn * lp.thr;
        const float maxdE = 5.f + MAXSCOPE * lp.sensbn * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

#ifdef _OPENMP
        #pragma omp for schedule(dynamic,16)
#endif

    for (int y = 0; y < transformed->H; y++) {
        int loy = cy + y;

        for (int x = 0; x < transformed->W; x++) {
            int lox = cx + x;

            int zone;
            float localFactor;

            if (lp.shapmet == 0) {
                calcTransition(lox, loy, ach, lp, zone, localFactor);
            } else if (lp.shapmet == 1) {
                calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
            }

                float rL = origblur->L[y][x] / 327.68f;
                float dE = sqrt(kab * SQR(refa - origblur->a[y][x] / 327.68f) + kab * SQR(refb - origblur->b[y][x] / 327.68f) + kL * SQR(lumaref - rL));
                float reducdE;
                calcreducdE(dE, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, lp.sensbn , reducdE);
            switch (zone) {
                case 0: { // outside selection and outside transition zone => full effect, no transition
                    float difL = tmp1->L[y][x] - original->L[y][x];
                    transformed->L[y][x] = CLIP(original->L[y][x] + difL * reducdE);

                    if (!lp.actsp) {
                        transformed->a[y][x] = CLIPC(tmp1->a[y][x]);
                        transformed->b[y][x] = CLIPC(tmp1->b[y][x]);
                    }

                    break;
                }

                case 1: { // inside transition zone
                    float difL = tmp1->L[y][x] - original->L[y][x];
                    float difa = tmp1->a[y][x] - original->a[y][x];
                    float difb = tmp1->b[y][x] - original->b[y][x];

                    float factorx = 1.f - localFactor;
                    difL *= factorx;
                    difa *= factorx;
                    difb *= factorx;

                    transformed->L[y][x] = CLIP(original->L[y][x] + difL * reducdE);

                    if (!lp.actsp) {

                        transformed->a[y][x] = CLIPC(original->a[y][x] + difa);
                        transformed->b[y][x] = CLIPC(original->b[y][x] + difb);
                    }

                    break;
                }

                case 2: { // inside selection => no effect, keep original values
                    transformed->L[y][x] = original->L[y][x];

                    if (!lp.actsp) {

                        transformed->a[y][x] = original->a[y][x];
                        transformed->b[y][x] = original->b[y][x];
                    }
                }
            }
        }
    }
}
}

static void calclight(float lum, float koef, float &lumnew, const LUTf &lightCurveloc)
//replace L-curve that does not work in local or bad
{
    lumnew = koef != -100.f ? CLIPLOC(lightCurveloc[lum]) : 0.f;
}


static void mean_fab(int xstart, int ystart, int bfw, int bfh, LabImage* bufexporig, const LabImage* original, float &fab, float &meanfab, float chrom)
{
    const int nbfab = bfw * bfh;

    meanfab = 0.f;
    fab = 50.f;

    if (nbfab > 0) {
        double sumab = 0.0;

#ifdef _OPENMP
        #pragma omp parallel for reduction(+:sumab)
#endif

        for (int y = 0; y < bfh; y++) {
            for (int x = 0; x < bfw; x++) {
                bufexporig->a[y][x] = original->a[y + ystart][x + xstart];
                bufexporig->b[y][x] = original->b[y + ystart][x + xstart];
                sumab += fabs(bufexporig->a[y][x]);
                sumab += fabs(bufexporig->b[y][x]);
            }
        }

        meanfab = sumab / (2.f * nbfab);

        double som = 0.0;

#ifdef _OPENMP
        #pragma omp parallel for reduction(+:som)
#endif

        for (int y = 0; y < bfh; y++) {
            for (int x = 0; x < bfw; x++) {
                som += SQR(fabs(bufexporig->a[y][x]) - meanfab) + SQR(fabs(bufexporig->b[y][x]) - meanfab);
            }
        }

        const float multsigma = (chrom >= 0.f ? 0.035f : 0.018f) * chrom + 1.f;

        const float stddv = sqrt(som / nbfab);
        fab = meanfab + multsigma * stddv;

        if (fab <= 0.f) {
            fab = 50.f;
        }
    }
}


void ImProcFunctions::blendstruc(int bfw, int bfh, LabImage* bufcolorig, float radius, float stru, array2D<float> & blend2, int sk, bool multiThread)
{
    SobelCannyLuma(blend2, bufcolorig->L, bfw, bfh, radius, multiThread);

    array2D<float> ble(bfw, bfh);
    array2D<float> guid(bfw, bfh);
#ifdef _OPENMP
    #pragma omp parallel for if (multiThread)
#endif

    for (int ir = 0; ir < bfh; ir++) {
        for (int jr = 0; jr < bfw; jr++) {
            blend2[ir][jr] /= 32768.f;
            guid[ir][jr] = bufcolorig->L[ir][jr] / 32768.f;
        }
    }

    const float blur = 25 / sk * (10.f + 1.2f * stru);

    rtengine::guidedFilter(guid, blend2, ble, blur, 0.001, multiThread);

#ifdef _OPENMP
    #pragma omp parallel for if (multiThread)
#endif

    for (int ir = 0; ir < bfh; ir++) {
        for (int jr = 0; jr < bfw; jr++) {
            ble[ir][jr] *= 32768.f;
        }
    }

    Median_Denoise(ble, blend2, bfw, bfh, Median::TYPE_3X3_STRONG, 1, multiThread);
}


static void blendmask(const local_params& lp, int xstart, int ystart, int cx, int cy, int bfw, int bfh, LabImage* bufexporig, LabImage* original, LabImage* bufmaskor, LabImage* originalmas, float bl)
{
#ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic,16)
#endif

    for (int y = 0; y < bfh ; y++) {
        const int loy = y + ystart + cy;

        for (int x = 0; x < bfw; x++) {
            const int lox = x + xstart + cx;
            int zone = 0;

            float localFactor = 1.f;
            const float achm = (float)lp.trans / 100.f;

            if (lp.shapmet == 0) {
                calcTransition(lox, loy, achm, lp, zone, localFactor);
            } else if (lp.shapmet == 1) {
                calcTransitionrect(lox, loy, achm, lp, zone, localFactor);
            }

            if (zone > 0) {
                bufexporig->L[y][x] += (bl * bufmaskor->L[y][x]);
                bufexporig->a[y][x] *= (1.f + bl * bufmaskor->a[y][x]);
                bufexporig->b[y][x] *= (1.f + bl * bufmaskor->b[y][x]);

                bufexporig->L[y][x] = CLIP(bufexporig->L[y][x]);
                bufexporig->a[y][x] = CLIPC(bufexporig->a[y][x]);
                bufexporig->b[y][x] = CLIPC(bufexporig->b[y][x]);

                originalmas->L[y][x] = CLIP(bufexporig->L[y][x] - bufmaskor->L[y][x]);
                originalmas->a[y][x] = CLIPC(bufexporig->a[y][x] * (1.f - bufmaskor->a[y][x]));
                originalmas->b[y][x] = CLIPC(bufexporig->b[y][x] * (1.f - bufmaskor->b[y][x]));

                switch (zone) {

                    case 1: {
                        original->L[y + ystart][x + xstart] += (bl * localFactor * bufmaskor->L[y][x]);
                        original->a[y + ystart][x + xstart] *= (1.f + bl * localFactor * bufmaskor->a[y][x]);
                        original->b[y + ystart][x + xstart] *= (1.f + bl * localFactor * bufmaskor->b[y][x]);
                        original->L[y + ystart][x + xstart] = CLIP(original->L[y + ystart][x + xstart]);
                        original->a[y + ystart][x + xstart] = CLIPC(original->a[y + ystart][x + xstart]);
                        original->b[y + ystart][x + xstart] = CLIPC(original->b[y + ystart][x + xstart]);
                        break;
                    }

                    case 2: {

                        original->L[y + ystart][x + xstart] = bufexporig->L[y][x];
                        original->a[y + ystart][x + xstart] = bufexporig->a[y][x];
                        original->b[y + ystart][x + xstart] = bufexporig->b[y][x];

                    }
                }
            }
        }
    }
}

static void showmask(const local_params& lp, int xstart, int ystart, int cx, int cy, int bfw, int bfh, LabImage* bufexporig, LabImage* transformed, LabImage* bufmaskorigSH)
{
#ifdef _OPENMP
    #pragma omp parallel for schedule(dynamic,16)
#endif

    for (int y = 0; y < bfh; y++) {
        const int loy = y + ystart + cy;

        for (int x = 0; x < bfw; x++) {
            const int lox = x + xstart + cx;
            int zone = 0;
            float localFactor = 1.f;
            const float achm = (float)lp.trans / 100.f;

            if (lp.shapmet == 0) {
                calcTransition(lox, loy, achm, lp, zone, localFactor);
            } else if (lp.shapmet == 1) {
                calcTransitionrect(lox, loy, achm, lp, zone, localFactor);
            }

            if (zone > 0) {
                transformed->L[y + ystart][x + xstart] = 6000.f + CLIPLOC(bufmaskorigSH->L[y][x]);
                transformed->a[y + ystart][x + xstart] = bufexporig->a[y][x] * bufmaskorigSH->a[y][x];
                transformed->b[y + ystart][x + xstart] = bufexporig->b[y][x] * bufmaskorigSH->b[y][x];
            }
        }
    }
}


void ImProcFunctions::InverseSharp_Local(float **loctemp, const float hueref, const float lumaref, const float chromaref, const local_params & lp, LabImage * original, LabImage * transformed, int cx, int cy, int sk)
{
//local sharp
    //  BENCHFUN
    const float ach = (float)lp.trans / 100.f;
    int GW = transformed->W;
    int GH = transformed->H;
    float refa = chromaref * cos(hueref);
    float refb = chromaref * sin(hueref);
    //balance deltaE
    float kL = lp.balance;
    float kab = 1.f;
    balancedeltaE(kL, kab);

    LabImage *origblur = new LabImage(GW, GH);

    float radius = 3.f / sk;
#ifdef _OPENMP
    #pragma omp parallel
#endif
    {
        gaussianBlur(original->L, origblur->L, GW, GH, radius);
        gaussianBlur(original->a, origblur->a, GW, GH, radius);
        gaussianBlur(original->b, origblur->b, GW, GH, radius);

    }

#ifdef _OPENMP
    #pragma omp parallel if (multiThread)
#endif
    {
        const int limscope = 80;
        const float mindE = 2.f + MINSCOPE * lp.senssha * lp.thr;
        const float maxdE = 5.f + MAXSCOPE * lp.senssha * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

#ifdef _OPENMP
        #pragma omp for schedule(dynamic,16)
#endif

        for (int y = 0; y < transformed->H; y++) {
            int loy = cy + y;

            for (int x = 0; x < transformed->W; x++) {
                int lox = cx + x;
                int zone;
                float localFactor = 1.f;

                if (lp.shapmet == 0) {
                    calcTransition(lox, loy, ach, lp, zone, localFactor);
                } else if (lp.shapmet == 1) {
                    calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
                }

                float rL = origblur->L[y][x] / 327.68f;
                float reducdE = 0.f;
                float dE = sqrt(kab * SQR(refa - origblur->a[y][x] / 327.68f) + kab * SQR(refb - origblur->b[y][x] / 327.68f) + kL * SQR(lumaref - rL));
                calcreducdE(dE, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, lp.senssha , reducdE);

                switch (zone) {
                    case 0: { // outside selection and outside transition zone => full effect, no transition
                        float difL = loctemp[y][x] - original->L[y][x];
                        transformed->L[y][x] = CLIP(original->L[y][x] + difL * reducdE);

                        break;
                    }

                    case 1: { // inside transition zone
                        float difL = loctemp[y][x] - original->L[y][x];

                        float factorx = 1.f - localFactor;
                        difL *= factorx;

                        transformed->L[y][x] = CLIP(original->L[y][x] + difL * reducdE);
                        break;
                    }

                    case 2: { // inside selection => no effect, keep original values
                        transformed->L[y][x] = original->L[y][x];
                    }
                }
            }
        }
    }
    delete origblur;
}


void ImProcFunctions::Sharp_Local(int call, float **loctemp, int senstype, const float hueref, const float chromaref, const float lumaref, const local_params &lp, LabImage *original, LabImage *transformed, int cx, int cy, int sk)
{
    BENCHFUN
    const float ach = lp.trans / 100.f;
    const float varsens = senstype == 1 ? lp.senslc : lp.senssha;

    //balance deltaE
    float kL = lp.balance;
    float kab = 1.f;
    balancedeltaE(kL, kab);
    kab /= SQR(327.68f);
    kL /= SQR(327.68f);

    const int GW = transformed->W;
    const int GH = transformed->H;

    std::unique_ptr<LabImage> origblur(new LabImage(GW, GH));
    const float refa = chromaref * cos(hueref) * 327.68f;
    const float refb = chromaref * sin(hueref) * 327.68f;
    const float refL = lumaref * 327.68f;
    const float radius = 3.f / sk;

#ifdef _OPENMP
    #pragma omp parallel if (multiThread)
#endif
    {
        gaussianBlur(original->L, origblur->L, GW, GH, radius);
        gaussianBlur(original->a, origblur->a, GW, GH, radius);
        gaussianBlur(original->b, origblur->b, GW, GH, radius);
    }

#ifdef _OPENMP
    #pragma omp parallel if (multiThread)
#endif
    {
        const int begy = int (lp.yc - lp.lyT);
        const int begx = int (lp.xc - lp.lxL);
        const int limscope = 80;
        const float mindE = 2.f + MINSCOPE * varsens * lp.thr;
        const float maxdE = 5.f + MAXSCOPE * varsens * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

#ifdef _OPENMP
        #pragma omp for schedule(dynamic,16)
#endif

        for (int y = 0; y < transformed->H; y++) {
            const int loy = cy + y;
            const bool isZone0 = loy > lp.yc + lp.ly || loy < lp.yc - lp.lyT; // whole line is zone 0 => we can skip a lot of processing

            if (isZone0) { // outside selection and outside transition zone => no effect, keep original values
                continue;
            }

            for (int x = 0; x < transformed->W; x++) {
                const int lox = cx + x;
                int zone = 0;
                float localFactor = 1.f;

                if (lp.shapmet == 0) {
                    calcTransition(lox, loy, ach, lp, zone, localFactor);
                } else if (lp.shapmet == 1) {
                    calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
                }

                if (zone == 0) { // outside selection and outside transition zone => no effect, keep original values
                    continue;
                }

                const float dE = sqrt(kab * (SQR(refa - origblur->a[y][x]) + SQR(refb - origblur->b[y][x])) + kL * SQR(refL - origblur->L[y][x]));

                float reducdE = 0.f;
                calcreducdE(dE, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, varsens , reducdE);
                reducdE *= localFactor;

                float difL;

                if (call == 2) {
                    difL = loctemp[loy - begy][lox - begx] - original->L[y][x];
                } else {
                    difL = loctemp[y][x] - original->L[y][x];
                }
                transformed->L[y][x] = CLIP(original->L[y][x] + difL * reducdE);
            }
        }
    }
}

void ImProcFunctions::Exclude_Local(float **deltaso, float hueref, float chromaref, float lumaref, float sobelref, float meansobel, const struct local_params & lp, const LabImage * original, LabImage * transformed, const LabImage * rsv, const LabImage * reserv, int cx, int cy, int sk)
{

    BENCHFUN {
        const float ach = (float)lp.trans / 100.f;
        const float varsens =  lp.sensexclu;

        const int limscope = 80;
        const float mindE = 2.f + MINSCOPE * varsens * lp.thr;
        const float maxdE = 5.f + MAXSCOPE * varsens * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

        const int GW = transformed->W;
        const int GH = transformed->H;

        const float refa = chromaref * cos(hueref) * 327.68f;
        const float refb = chromaref * sin(hueref) * 327.68f;
        lumaref *= 327.68f;
        //balance deltaE
        float kL = lp.balance;
        float kab = 1.f;
        balancedeltaE(kL, kab);
        kL /= SQR(327.68f);
        kab /= SQR(327.68f);
        //sobel
        sobelref = rtengine::min(sobelref / 100.f, 60.f);

        const bool recip = sobelref <  meansobel && sobelref < lp.stru;

        sobelref = log1p(sobelref);

        LabImage *origblur = new LabImage(GW, GH);

        const float radius = 3.f / sk;

#ifdef _OPENMP
        #pragma omp parallel if (multiThread)
#endif
        {
            gaussianBlur(reserv->L, origblur->L, GW, GH, radius);
            gaussianBlur(reserv->a, origblur->a, GW, GH, radius);
            gaussianBlur(reserv->b, origblur->b, GW, GH, radius);


#ifdef _OPENMP
            #pragma omp barrier
            #pragma omp for schedule(dynamic,16)
#endif

            for (int y = 0; y < transformed->H; y++)
            {
                const int loy = cy + y;
                const bool isZone0 = loy > lp.yc + lp.ly || loy < lp.yc - lp.lyT; // whole line is zone 0 => we can skip a lot of processing

                if (isZone0) { // outside selection and outside transition zone => no effect, keep original values
                    for (int x = 0; x < transformed->W; x++) {
                        transformed->L[y][x] = original->L[y][x];
                    }

                    continue;
                }

                for (int x = 0; x < transformed->W; x++) {
                    const int lox = cx + x;
                    const int begx = int (lp.xc - lp.lxL);
                    const int begy = int (lp.yc - lp.lyT);

                    int zone = 0;
                    float localFactor = 1.f;

                    if (lp.shapmet == 0) {
                        calcTransition(lox, loy, ach, lp, zone, localFactor);
                    } else if (lp.shapmet == 1) {
                        calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
                    }


                    if (zone == 0) { // outside selection and outside transition zone => no effect, keep original values
                        transformed->L[y][x] = original->L[y][x];
                        continue;
                    }

                    float rs = 0.f;

                    const float csob = xlogf(1.f + rtengine::min(deltaso[loy - begy][lox - begx] / 100.f, 60.f) + 0.001f);

                    if (!recip) {
                        rs = sobelref / csob;
                    } else {
                        rs = csob / sobelref;
                    }

                    float affsob = 1.f;

                    if (lp.struexc > 0.f && rs > 0.f) {
                        const float rsob =  0.002f *  lp.struexc * rs;
                        const float minrs = 1.3f + 0.05f * lp.stru;

                        if (rs < minrs) {
                            affsob = 1.f;
                        } else {
                            affsob = 1.f / pow_F((1.f + rsob), SQR(SQR(rs - minrs)));
                        }
                    }

                    const float rL = origblur->L[y][x];
                    const float dE = sqrt(kab * SQR(refa - origblur->a[y][x]) + kab * SQR(refb - origblur->b[y][x]) + kL * SQR(lumaref - rL));

                    float reducdE;
                    calcreducdE(dE, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, varsens , reducdE);

                    const float affde = reducdE;

                    if (rL > 32.768f) { //to avoid crash with very low gamut in rare cases ex : L=0.01 a=0.5 b=-0.9
                        switch (zone) {

                            case 1: { // inside transition zone
                                const float difL = (rsv->L[loy - begy][lox - begx] - original->L[y][x]) * localFactor;
                                transformed->L[y][x] = CLIP(original->L[y][x] + difL * affsob * affde);

                                const float difa = (rsv->a[loy - begy][lox - begx] - original->a[y][x]) * localFactor;
                                transformed->a[y][x] = CLIPC(original->a[y][x] + difa * affsob * affde);

                                const float difb = (rsv->b[loy - begy][lox - begx] - original->b[y][x]) * localFactor;
                                transformed->b[y][x] = CLIPC(original->b[y][x] + difb * affsob * affde);

                                break;

                            }

                            case 2: { // inside selection => full effect, no transition
                                const float difL = rsv->L[loy - begy][lox - begx] - original->L[y][x];
                                transformed->L[y][x] = CLIP(original->L[y][x] + difL * affsob * affde);

                                const float difa = rsv->a[loy - begy][lox - begx] - original->a[y][x];;
                                transformed->a[y][x] = CLIPC(original->a[y][x] + difa * affsob * affde);

                                const float difb = rsv->b[loy - begy][lox - begx] - original->b[y][x];
                                transformed->b[y][x] = CLIPC(original->b[y][x] + difb * affsob * affde);
                            }
                        }
                    }
                }
            }
        }
        delete origblur;
    }
}

void ImProcFunctions::transit_shapedetect_retinex(int senstype, LabImage * bufexporig, float **buflight, float **bufchro, const float hueref, const float chromaref, const float lumaref, const struct local_params & lp, LabImage * original, LabImage * transformed, int cx, int cy, int sk)
{

    BENCHFUN {
        const float ach = (float)lp.trans / 100.f;
        const float varsens = lp.sensh;

        int GW = transformed->W;
        int GH = transformed->H;

        const float refa = chromaref * cos(hueref);
        const float refb = chromaref * sin(hueref);

        //balance deltaE
        float kL = lp.balance;
        float kab = 1.f;
        balancedeltaE(kL, kab);

        std::unique_ptr<LabImage> origblur(new LabImage(GW, GH));
        const float radius = 3.f / sk;

#ifdef _OPENMP
        #pragma omp parallel
#endif
        {
            gaussianBlur(original->L, origblur->L, GW, GH, radius);
            gaussianBlur(original->a, origblur->a, GW, GH, radius);
            gaussianBlur(original->b, origblur->b, GW, GH, radius);

        }


#ifdef _OPENMP
        #pragma omp parallel if (multiThread)
#endif
        {
        const int limscope = 80;
        const float mindE = 2.f + MINSCOPE * varsens * lp.thr;
        const float maxdE = 5.f + MAXSCOPE * varsens * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

#ifdef _OPENMP
            #pragma omp for schedule(dynamic,16)
#endif

            for (int y = 0; y < transformed->H; y++)
            {
                const int loy = cy + y;
                const bool isZone0 = loy > lp.yc + lp.ly || loy < lp.yc - lp.lyT; // whole line is zone 0 => we can skip a lot of processing

                if (isZone0) { // outside selection and outside transition zone => no effect, keep original values
                    continue;
                }

                for (int x = 0; x < transformed->W; x++) {
                    const int lox = cx + x;
                    const int begx = int (lp.xc - lp.lxL);
                    const int begy = int (lp.yc - lp.lyT);

                    int zone = 0;
                    float localFactor = 1.f;

                    if (lp.shapmet == 0) {
                        calcTransition(lox, loy, ach, lp, zone, localFactor);
                    } else if (lp.shapmet == 1) {
                        calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
                    }


                    if (zone == 0) { // outside selection and outside transition zone => no effect, keep original values
                        continue;
                    }

                    float rL = origblur->L[y][x] / 327.68f;

                    const float dE = sqrt(kab * SQR(refa - origblur->a[y][x] / 327.68f) + kab * SQR(refb - origblur->b[y][x] / 327.68f) + kL * SQR(lumaref - rL));

                    float cli = buflight[loy - begy][lox - begx];
                    float clc = bufchro[loy - begy][lox - begx];

                    float reducdE;

                    calcreducdE(dE, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, varsens , reducdE);

                    reducdE /= 100.f;
                    cli *= reducdE;
                    clc *= reducdE;

                    if (rL > 0.1f) { //to avoid crash with very low gamut in rare cases ex : L=0.01 a=0.5 b=-0.9
                        if (senstype == 4) {//all except color and light (TODO) and exposure
                            float lightc = bufexporig->L[loy - begy][lox - begx];
                            float fli = 1.f + cli;
                            float diflc = lightc * fli - original->L[y][x];
                            diflc *= localFactor;
                            transformed->L[y][x] = CLIP(original->L[y][x] + diflc);
                        }

                        float fliab = 1.f;
                        const float chra = bufexporig->a[loy - begy][lox - begx];
                        const float chrb = bufexporig->b[loy - begy][lox - begx];

                        if (senstype == 4) {
                            fliab = 1.f + clc;
                        }

                        const float difa = (chra * fliab - original->a[y][x]) * localFactor;
                        const float difb = (chrb * fliab - original->b[y][x]) * localFactor;

                        transformed->a[y][x] = CLIPC(original->a[y][x] + difa);
                        transformed->b[y][x] = CLIPC(original->b[y][x] + difb);
                    }
                }
            }
        }
    }
}


void ImProcFunctions::transit_shapedetect(int senstype, const LabImage *bufexporig, LabImage * originalmask, float **buflight, float **bufchro, float **buf_a_cat, float ** buf_b_cat, float ** bufhh, bool HHutili, const float hueref, const float chromaref, const float lumaref, float sobelref, float meansobel, float ** blend2, const struct local_params & lp, LabImage * original, LabImage * transformed, int cx, int cy, int sk)
{

    BENCHFUN {
        const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
        const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
        const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
        const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);
        const int bfw = xend - xstart;
        const int bfh = yend - ystart;

        const float ach = lp.trans / 100.f;
        float varsens = lp.sensex;

        if (senstype == 0)   //Color and Light
        {
            varsens =  lp.sens;
        } else if (senstype == 1)   //exposure
        {
            varsens =  lp.sensex;
        } else if (senstype == 2)   //vibrance
        {
            varsens =  lp.sensv;
        } else if (senstype == 3)   //soft light
        {
            varsens =  lp.senssf;
        } else if (senstype == 6 || senstype == 7)   //cbdl
        {
            varsens =  lp.senscb;
        } else if (senstype == 8)   //TM
        {
            varsens =  lp.senstm;
        } else if (senstype == 9)   //Shadow highlight
        {
            varsens =  lp.senshs;
        }

        //sobel
        sobelref /= 100.f;
        meansobel /= 100.f;

        sobelref = rtengine::min(sobelref, 60.f);

        const bool k = !(sobelref < meansobel && sobelref < lp.stru); //does not always work with noisy images

        sobelref = log1p(sobelref);

        const float refa = chromaref * cos(hueref) * 327.68f;
        const float refb = chromaref * sin(hueref) * 327.68f;
        const float refL = lumaref * 327.68f;

        const bool expshow = ((lp.showmaskexpmet == 1 || lp.showmaskexpmet == 2)  &&  senstype == 1);
        const bool colshow = ((lp.showmaskcolmet == 1 || lp.showmaskcolmet == 2)  &&  senstype == 0);
        const bool SHshow = ((lp.showmaskSHmet == 1 || lp.showmaskSHmet == 2)  &&  senstype == 9);
        const bool cbshow = ((lp.showmaskcbmet == 1 || lp.showmaskcbmet == 2)  &&  senstype == 6);
        const bool previewcol = ((lp.showmaskcolmet == 5)  &&  senstype == 0);
        const bool previewexp = ((lp.showmaskexpmet == 5)  &&  senstype == 1);
        const bool previewSH = ((lp.showmaskSHmet == 4)  &&  senstype == 9);
        const bool previewcb = ((lp.showmaskcbmet == 4)  &&  senstype == 6);

        std::unique_ptr<LabImage> origblur(new LabImage(bfw, bfh));
        std::unique_ptr<LabImage> origblurmask;

        float radius = 3.f / sk;

        if (senstype == 1)
        {
            radius = (2.f + 0.2f * lp.blurexp) / sk;
        } else if (senstype == 0)
        {
            radius = (2.f + 0.2f * lp.blurcol) / sk;
        } else if (senstype == 9)
        {
            radius = (2.f + 0.2f * lp.blurSH) / sk;
        }

        //balance deltaE
        float kL = lp.balance;
        float kab = 1.f;
        balancedeltaE(kL, kab);
        kab /= SQR(327.68f);
        kL /= SQR(327.68f);
        const bool usemaskexp = (lp.showmaskexpmet == 2 || lp.enaExpMask || lp.showmaskexpmet == 5) && senstype == 1;
        const bool usemaskcol = (lp.showmaskcolmet == 2 || lp.enaColorMask || lp.showmaskcolmet == 5) && senstype == 0;
        const bool usemaskSH = (lp.showmaskSHmet == 2 || lp.enaSHMask || lp.showmaskSHmet == 4) && senstype == 9;
        const bool usemaskcb = (lp.showmaskcbmet == 2 || lp.enacbMask || lp.showmaskcbmet == 4) && senstype == 6;
        const bool usemaskall = (usemaskSH || usemaskcol || usemaskexp || usemaskcb);

        if (usemaskall)
        {
            origblurmask.reset(new LabImage(bfw, bfh));

#ifdef _OPENMP
            #pragma omp parallel if (multiThread)
#endif
            {
                gaussianBlur(originalmask->L, origblurmask->L, bfw, bfh, radius);
                gaussianBlur(originalmask->a, origblurmask->a, bfw, bfh, radius);
                gaussianBlur(originalmask->b, origblurmask->b, bfw, bfh, radius);
            }
        }

#ifdef _OPENMP
        #pragma omp parallel if (multiThread)
#endif
    {
#ifdef _OPENMP
            #pragma omp for schedule(dynamic,16)
#endif

            for (int y = 0; y < bfh; y++)
            {
                for (int x = 0; x < bfw; x++) {
                    origblur->L[y][x] = original->L[y + ystart][x + xstart];
                    origblur->a[y][x] = original->a[y + ystart][x + xstart];
                    origblur->b[y][x] = original->b[y + ystart][x + xstart];
                }
            }

            gaussianBlur(origblur->L, origblur->L, bfw, bfh, radius);
            gaussianBlur(origblur->a, origblur->a, bfw, bfh, radius);
            gaussianBlur(origblur->b, origblur->b, bfw, bfh, radius);

        }

        const LabImage *maskptr = usemaskall ? origblurmask.get() : origblur.get();
        const int limscope = 80;
        const float mindE = 2.f + MINSCOPE * varsens * lp.thr;
        const float maxdE = 5.f + MAXSCOPE * varsens * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

#ifdef _OPENMP
        #pragma omp parallel if (multiThread)
#endif
        {
#ifdef __SSE2__
            float atan2Buffer[transformed->W] ALIGNED16;
#endif

#ifdef _OPENMP
            #pragma omp for schedule(dynamic,16)
#endif

            for (int y = ystart; y < yend; y++)
            {
                const int loy = cy + y;

#ifdef __SSE2__

                if (HHutili || senstype == 7) {
                    int i = xstart;

                    for (; i < xend - 3; i += 4) {
                        vfloat av = LVFU(origblur->a[y - ystart][i - xstart]);
                        vfloat bv = LVFU(origblur->b[y - ystart][i - xstart]);
                        STVFU(atan2Buffer[i], xatan2f(bv, av));
                    }

                    for (; i < xend; i++) {
                        atan2Buffer[i] = xatan2f(origblur->b[y - ystart][i - xstart], origblur->a[y - ystart][i - xstart]);
                    }
                }

#endif

                for (int x = xstart; x < xend; x++) {
                    const int lox = cx + x;

                    int zone = 0;
                    float localFactor = 1.f;

                    if (lp.shapmet == 0) {
                        calcTransition(lox, loy, ach, lp, zone, localFactor);
                    } else if (lp.shapmet == 1) {
                        calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
                    }


                    if (zone == 0) { // outside selection and outside transition zone => no effect, keep original values
                        continue;
                    }

                    float rhue = 0;

                    if (HHutili || senstype == 7) {
#ifdef __SSE2__
                        rhue = atan2Buffer[x];
#else
                        rhue = xatan2f(origblur->b[y - ystart][x - xstart], origblur->a[y - ystart][x - xstart]);
#endif
                    }

                    const float rL = origblur->L[y - ystart][x - xstart] / 327.68f;
                    float rsob = 0.f;

                    if (blend2 && ((senstype == 1 && lp.struexp > 0.f) || (senstype == 0 && lp.struco > 0.f))) {
                        const float csob = xlogf(1.f + std::min(blend2[y - ystart][x - xstart] / 100.f, 60.f) + 0.001f);

                        float rs;

                        if (k) {
                            rs = sobelref / csob;
                        } else {
                            rs = csob / sobelref;
                        }

                        if (rs > 0.f && senstype == 1) {
                            rsob =  1.1f * lp.struexp * rs;
                        } else if (rs > 0.f && senstype == 0) {
                            rsob =  1.1f * lp.struco * rs;
                        }
                    }

                    const float dE = rsob + sqrt(kab * (SQR(refa - maskptr->a[y - ystart][x - xstart]) + SQR(refb - maskptr->b[y - ystart][x - xstart])) + kL * SQR(refL - maskptr->L[y - ystart][x - xstart]));

                    float cla = 0.f;
                    float clb = 0.f;

                    const float cli = buflight[y - ystart][x - xstart];
                    const float clc = (previewcol || previewexp || previewSH || previewcb) ? settings->previewselection * 100.f : bufchro[y - ystart][x - xstart];


                    if (senstype <= 1) {
                        cla = buf_a_cat[y - ystart][x - xstart];
                        clb = buf_b_cat[y - ystart][x - xstart];
                    }

                    float reducdE;
                    calcreducdE(dE, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, varsens , reducdE);

                    const float realstrdE = reducdE * cli;
                    const float realstradE = reducdE * cla;
                    const float realstrbdE = reducdE * clb;
                    const float realstrchdE = reducdE * clc;


                    if (rL > 0.1f) { //to avoid crash with very low gamut in rare cases ex : L=0.01 a=0.5 b=-0.9
                        switch (zone) {
                            case 1: { // inside transition zone
                                float factorx = localFactor;
                                float diflc = 0.f;
                                float newhr = 0.f;
                                float difL = 0.f;

                                if (senstype == 2  || senstype == 8) {
                                    const float lightc = bufexporig->L[y - ystart][x - xstart];
                                    const float fli = (100.f + realstrdE) / 100.f;
                                    transformed->L[y][x] = CLIP(original->L[y][x] + (lightc * fli - original->L[y][x]) * factorx);
                                } else if (senstype == 6) {
                                    difL = (bufexporig->L[y - ystart][x - xstart] - original->L[y][x]) * localFactor * reducdE;
                                    transformed->L[y][x] = CLIP(original->L[y][x] + difL);
                                } else if (senstype == 1 || senstype == 0 || senstype == 9 || senstype == 3) {
                                    if (HHutili) {
                                        const float hhro = bufhh[y - ystart][x - xstart];

                                        if (hhro != 0) {
                                            const float realhhdE = reducdE * hhro;
                                            const float addh = 0.01f * realhhdE * factorx;
                                            newhr = rhue + addh;

                                            if (newhr > rtengine::RT_PI_F) {
                                                newhr -= 2 * rtengine::RT_PI_F;
                                            } else if (newhr < -rtengine::RT_PI_F) {
                                                newhr += 2 * rtengine::RT_PI_F;
                                            }
                                        }
                                    }

                                    transformed->L[y][x] = CLIP(original->L[y][x] + 328.f * factorx * realstrdE);
                                    diflc = 328.f * factorx * realstrdE;
                                }

                                if (senstype == 7) {
                                    float difab = bufexporig->L[y - ystart][x - xstart] - sqrt(SQR(original->a[y][x]) + SQR(original->b[y][x]));
                                    float2 sincosval = xsincosf(rhue);
                                    float difa = difab * sincosval.y;
                                    float difb = difab * sincosval.x;
                                    difa *= factorx * (100.f + realstrchdE) / 100.f;
                                    difb *= factorx * (100.f + realstrchdE) / 100.f;
                                    transformed->a[y][x] = CLIPC(original->a[y][x] + difa);
                                    transformed->b[y][x] = CLIPC(original->b[y][x] + difb);
                                } else {
                                    float flia = 1.f;
                                    float flib = 1.f;
                                    const float chra = bufexporig->a[y - ystart][x - xstart];
                                    const float chrb = bufexporig->b[y - ystart][x - xstart];

                                    if (senstype == 2 || senstype == 3 || senstype == 8 || senstype == 9 || senstype == 6) {
                                        
                                        flia = flib = ((100.f + realstrchdE) / 100.f);
                                   } else if (senstype == 1) {
                                        // printf("rdE=%f chdE=%f", realstradE, realstrchdE);
                                        flia = (100.f + realstradE + 100.f * realstrchdE) / 100.f;
                                        flib = (100.f + realstrbdE + 100.f * realstrchdE) / 100.f;

                                        if (previewcol || previewexp || previewSH) {
                                            flia = (100.f + realstradE + realstrchdE) / 100.f;
                                            flib = (100.f + realstrbdE + realstrchdE) / 100.f;
                                        }
                                    } else if (senstype == 0) {
                                        flia = (100.f + 0.3f * lp.strengrid * realstradE + realstrchdE) / 100.f;
                                        flib = (100.f + 0.3f * lp.strengrid * realstrbdE + realstrchdE) / 100.f;

                                        if (previewcol || previewexp || previewSH  || previewcb) {
                                            flia = (100.f + realstradE + realstrchdE) / 100.f;
                                            flib = (100.f + realstrbdE + realstrchdE) / 100.f;
                                        }
                                    }

                                    float difa = chra * flia - original->a[y][x];
                                    float difb = chrb * flib - original->b[y][x];
                                    difa *= factorx;
                                    difb *= factorx;

                                    transformed->a[y][x] = CLIPC(original->a[y][x] + difa);
                                    transformed->b[y][x] = CLIPC(original->b[y][x] + difb);

                                    if (senstype == 0 && HHutili) {
                                        const float tempa = transformed->a[y][x];
                                        const float tempb = transformed->b[y][x];
                                        const float hhro = bufhh[y - ystart][x - xstart];

                                        if (hhro != 0.f) {
                                            const float chromhr = sqrt(SQR(original->a[y][x] + difa) + SQR(original->b[y][x] + difb));
                                            float epsia = 0.f;
                                            float epsib = 0.f;

                                            if (original->a[y][x] == 0.f) {
                                                epsia = 0.001f;
                                            }

                                            if (original->b[y][x] == 0.f) {
                                                epsib = 0.001f;
                                            }

                                            const float faca = (original->a[y][x] + difa) / (original->a[y][x] + epsia);
                                            const float facb = (original->b[y][x] + difb) / (original->b[y][x] + epsib);
                                            const float2 sincosval = xsincosf(newhr);

                                            transformed->a[y][x] = CLIPC(chromhr * sincosval.y * faca) ;
                                            transformed->b[y][x] = CLIPC(chromhr * sincosval.x * facb);
                                            difa = transformed->a[y][x] - tempa;
                                            difb = transformed->b[y][x] - tempb;
                                        }
                                    }

                                    if (expshow || colshow || SHshow) {
                                        transformed->L[y][x] = CLIP(12000.f + diflc);
                                        transformed->a[y][x] = CLIPC(difa);
                                        transformed->b[y][x] = CLIPC(difb);
                                    } else if (cbshow) {
                                        transformed->L[y][x] = CLIP(12000.f + difL);
                                        transformed->a[y][x] = CLIPC(difa);
                                        transformed->b[y][x] = CLIPC(difb);
                                    } else if (previewcol || previewexp || previewSH  || previewcb) {
                                        transformed->a[y][x] = 0.f;
                                        transformed->b[y][x] = (difb);
                                    }
                                }

                                break;

                            }

                            case 2: { // inside selection => full effect, no transition
                                float diflc = 0.f;
                                float newhr = 0.f;
                                float difL = 0.f;
                                if (senstype == 2  || senstype == 8) { //retinex & cbdl
                                    const float lightc = bufexporig->L[y - ystart][x - xstart];
                                    const float fli = (100.f + realstrdE) / 100.f;
                                    transformed->L[y][x] = CLIP(original->L[y][x] + lightc * fli - original->L[y][x]);
                                } else if (senstype == 6) {
                                    difL = (bufexporig->L[y - ystart][x - xstart] - original->L[y][x]) * reducdE;
                                    transformed->L[y][x] = CLIP(original->L[y][x] + difL);
                                } else if (senstype == 1 || senstype == 0 || senstype == 9 || senstype == 3) {
                                    if (HHutili) {
                                        const float hhro = bufhh[y - ystart][x - xstart];

                                        if (hhro != 0) {
                                            const float realhhdE = reducdE * hhro;
                                            const float addh = 0.01f * realhhdE;
                                            newhr = rhue + addh;

                                            if (newhr > rtengine::RT_PI_F) {
                                                newhr -= 2 * rtengine::RT_PI_F;
                                            } else if (newhr < -rtengine::RT_PI_F) {
                                                newhr += 2 * rtengine::RT_PI_F;
                                            }
                                        }
                                    }

                                    transformed->L[y][x] = CLIP(original->L[y][x] + 328.f * realstrdE);//kch fach
                                    diflc = 328.f * realstrdE;
                                }

                                if (senstype == 7) {//cbdl chroma
                                    float difab = bufexporig->L[y - ystart][x - xstart] - sqrt(SQR(original->a[y][x]) + SQR(original->b[y][x]));
                                    float2 sincosval = xsincosf(rhue);
                                    float difa = difab * sincosval.y;
                                    float difb = difab * sincosval.x;
                                    difa *= (100.f + realstrchdE) / 100.f;
                                    difb *= (100.f + realstrchdE) / 100.f;
                                    transformed->a[y][x] = CLIPC(original->a[y][x] + difa);
                                    transformed->b[y][x] = CLIPC(original->b[y][x] + difb);
                                } else {
                                    float flia = 1.f;
                                    float flib = 1.f;
                                    const float chra = bufexporig->a[y - ystart][x - xstart];
                                    const float chrb = bufexporig->b[y - ystart][x - xstart];

                                    if (senstype == 2 || senstype == 3 || senstype == 8 || senstype == 9 || senstype == 6) {
                                        flia = flib = (100.f + realstrchdE) / 100.f;
                                    } else if (senstype == 1) {
                                        flia = (100.f + realstradE + 100.f * realstrchdE) / 100.f;
                                        flib = (100.f + realstrbdE + 100.f * realstrchdE) / 100.f;

                                        if (previewcol || previewexp || previewSH) {
                                            flia = (100.f + realstradE + realstrchdE) / 100.f;
                                            flib = (100.f + realstrbdE + realstrchdE) / 100.f;
                                        }
                                    } else if (senstype == 0) {
                                        flia = (100.f + 0.3f * lp.strengrid * realstradE + realstrchdE) / 100.f;
                                        flib = (100.f + 0.3f * lp.strengrid * realstrbdE + realstrchdE) / 100.f;

                                        if (previewcol || previewexp || previewSH) {
                                            flia = (100.f + realstradE + realstrchdE) / 100.f;
                                            flib = (100.f + realstrbdE + realstrchdE) / 100.f;
                                        }
                                    }

                                    float difa = chra * flia - original->a[y][x];
                                    float difb = chrb * flib - original->b[y][x];

                                    transformed->a[y][x] = CLIPC(original->a[y][x] + difa);
                                    transformed->b[y][x] = CLIPC(original->b[y][x] + difb);

                                    if (senstype == 0 && HHutili) {
                                        const float tempa = transformed->a[y][x];
                                        const float tempb = transformed->b[y][x];
                                        const float hhro = bufhh[y - ystart][x - xstart];

                                        if (hhro != 0.f) {
                                            const float chromhr = sqrt(SQR(original->a[y][x] + difa) + SQR(original->b[y][x] + difb));
                                            float epsia = 0.f;
                                            float epsib = 0.f;

                                            if (original->a[y][x] == 0.f) {
                                                epsia = 0.001f;
                                            }

                                            if (original->b[y][x] == 0.f) {
                                                epsib = 0.001f;
                                            }

                                            const float faca = (original->a[y][x] + difa) / (original->a[y][x] + epsia);
                                            const float facb = (original->b[y][x] + difb) / (original->b[y][x] + epsib);

                                            const float2 sincosval = xsincosf(newhr);
                                            transformed->a[y][x] = CLIPC(chromhr * sincosval.y * faca) ;
                                            transformed->b[y][x] = CLIPC(chromhr * sincosval.x * facb);
                                            difa = transformed->a[y][x] - tempa;
                                            difb = transformed->b[y][x] - tempb;
                                        }
                                    }

                                    if (expshow || colshow || SHshow) {
                                        transformed->L[y][x] = CLIP(12000.f + diflc);
                                        transformed->a[y][x] = CLIPC(difa);
                                        transformed->b[y][x] = CLIPC(difb);
                                    } else if (cbshow) {
                                        transformed->L[y][x] = CLIP(12000.f + difL);
                                        transformed->a[y][x] = CLIPC(difa);
                                        transformed->b[y][x] = CLIPC(difb);
                                    } else if (previewcol || previewexp || previewSH || previewcb) {
                                        transformed->a[y][x] = 0.f;
                                        transformed->b[y][x] = difb;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void ImProcFunctions::InverseColorLight_Local(int sp, int senstype, const struct local_params & lp, LUTf & lightCurveloc, LUTf & hltonecurveloc, LUTf & shtonecurveloc, LUTf & tonecurveloc, LUTf & exlocalcurve, LUTf & cclocalcurve, float adjustr, bool localcutili, LUTf & lllocalcurve, bool locallutili, LabImage * original, LabImage * transformed, int cx, int cy, const float hueref, const float chromaref, const float lumaref, int sk)
{
    // BENCHFUN
    float ach = (float)lp.trans / 100.f;
    const float facc = (100.f + lp.chro) / 100.f; //chroma factor transition
    float varsens = lp.sens;

    if (senstype == 0) { //Color and Light
        varsens =  lp.sens;
    }

    if (senstype == 1) { //exposure
        varsens =  lp.sensex;
    }

    if (senstype == 2) { //shadows highlight
        varsens =  lp.senshs;
    }

    LabImage *temp = nullptr;
    LabImage *tempCL = nullptr;

    int GW = transformed->W;
    int GH = transformed->H;
    float refa = chromaref * cos(hueref);
    float refb = chromaref * sin(hueref);

    if (senstype == 2) { // Shadows highlight
        temp = new LabImage(GW, GH);
#ifdef _OPENMP
        #pragma omp parallel for schedule(dynamic,16)
#endif

        for (int y = 0; y < transformed->H; y++) {
            for (int x = 0; x < transformed->W; x++) {
                temp->L[y][x] = original->L[y][x];
                temp->a[y][x] = original->a[y][x];
                temp->b[y][x] = original->b[y][x];
            }
        }

        ImProcFunctions::shadowsHighlights(temp, lp.hsena, 1, lp.highlihs, lp.shadowhs, lp.radiushs, sk, lp.hltonalhs, lp.shtonalhs);
    }

    if (senstype == 1) { //exposure
        temp = new LabImage(GW, GH);

        ImProcFunctions::exlabLocal(lp, GH, GW, original, temp, hltonecurveloc, shtonecurveloc, tonecurveloc);

        if (exlocalcurve) {
#ifdef _OPENMP
            #pragma omp parallel for schedule(dynamic,16)
#endif

            for (int y = 0; y < temp->H; y++) {
                for (int x = 0; x < temp->W; x++) {
                    float lighn =  temp->L[y][x];
                    float lh = 0.5f * exlocalcurve[2.f * lighn]; // / ((lighn) / 1.9f) / 3.61f; //lh between 0 and 0 50 or more
                    temp->L[y][x] = lh;
                }
            }
        }

        if (lp.expchroma != 0.f) {
            float ch;
            ch = (1.f + 0.02f * lp.expchroma) ;
            float chprosl;

            if (ch <= 1.f) {//convert data curve near values of slider -100 + 100, to be used after to detection shape
                chprosl = 99.f * ch - 99.f;
            } else {
                float ampli = 70.f;
                chprosl = CLIPCHRO(ampli * ch - ampli);  //ampli = 25.f arbitrary empirical coefficient between 5 and 50
            }

#ifdef _OPENMP
            #pragma omp parallel for schedule(dynamic,16)
#endif

            for (int y = 0; y < transformed->H; y++) {
                for (int x = 0; x < transformed->W; x++) {
                    float epsi = 0.f;

                    if (original->L[y][x] == 0.f) {
                        epsi = 0.001f;
                    }

                    float rapexp = temp->L[y][x] / (original->L[y][x] + epsi);
                    temp->a[y][x] *= 0.01f * (100.f + 100.f * chprosl * rapexp);
                    temp->b[y][x] *= 0.01f * (100.f + 100.f * chprosl * rapexp);
                }
            }
        }

        if (lp.war != 0) {
            ImProcFunctions::ciecamloc_02float(sp, temp);
        }
    }

    if (senstype == 0) { //Color and Light curves L C
        tempCL = new LabImage(GW, GH);
#ifdef _OPENMP
        #pragma omp parallel for schedule(dynamic,16)
#endif

        for (int y = 0; y < tempCL->H; y++) {
            for (int x = 0; x < tempCL->W; x++) {
                tempCL->a[y][x] = original->a[y][x];
                tempCL->b[y][x] = original->b[y][x];
                tempCL->L[y][x] = original->L[y][x];
            }
        }

        if (cclocalcurve  && localcutili) { // C=f(C) curve
#ifdef _OPENMP
            #pragma omp parallel for schedule(dynamic,16)
#endif

            for (int y = 0; y < transformed->H; y++) {
                for (int x = 0; x < transformed->W; x++) {
                    //same as in "normal"
                    float chromat = sqrt(SQR(original->a[y][x]) +  SQR(original->b[y][x]));
                    float ch;
                    float ampli = 25.f;
                    ch = (cclocalcurve[chromat * adjustr ])  / ((chromat + 0.00001f) * adjustr); //ch between 0 and 0 50 or more
                    float chprocu = CLIPCHRO(ampli * ch - ampli);  //ampli = 25.f arbitrary empirical coefficient between 5 and 50
                    tempCL->a[y][x] = original->a[y][x] * (1.f + 0.01f * (chprocu));
                    tempCL->b[y][x] = original->b[y][x] * (1.f + 0.01f * (chprocu));

                }
            }

        }

        if (lllocalcurve && locallutili) {
#ifdef _OPENMP
            #pragma omp parallel for schedule(dynamic,16)
#endif

            for (int y = 0; y < transformed->H; y++) {
                for (int x = 0; x < transformed->W; x++) {
                    float lighn =  original->L[y][x];
                    float lh = 0.5f * lllocalcurve[2.f * lighn];
                    tempCL->L[y][x] = lh;
                }
            }
        }

    }

    //balance deltaE
    float kL = lp.balance;
    float kab = 1.f;
    balancedeltaE(kL, kab);

    LabImage *origblur = new LabImage(GW, GH);

    float radius = 3.f / sk;


    if (senstype == 1) {
        radius = (2.f + 0.2f * lp.blurexp) / sk;
    }

    if (senstype == 0) {
        radius = (2.f + 0.2f * lp.blurcol) / sk;
    }

    if (senstype == 2) {
        radius = (2.f + 0.2f * lp.blurSH) / sk;
    }

#ifdef _OPENMP
    #pragma omp parallel
#endif
    {
        gaussianBlur(original->L, origblur->L, GW, GH, radius);
        gaussianBlur(original->a, origblur->a, GW, GH, radius);
        gaussianBlur(original->b, origblur->b, GW, GH, radius);

    }

#ifdef _OPENMP
    #pragma omp parallel if (multiThread)
#endif
    {
        const int limscope = 80;
        const float mindE = 2.f + MINSCOPE * varsens * lp.thr;
        const float maxdE = 5.f + MAXSCOPE * varsens * (1 + 0.1f * lp.thr);
        const float mindElim = 2.f + MINSCOPE * limscope * lp.thr;
        const float maxdElim = 5.f + MAXSCOPE * limscope * (1 + 0.1f * lp.thr);

#ifdef _OPENMP
        #pragma omp for schedule(dynamic,16)
#endif

        for (int y = 0; y < transformed->H; y++) {
            const int loy = cy + y;

            for (int x = 0; x < transformed->W; x++) {
                const int lox = cx + x;
                int zone = 0;

                float localFactor = 1.f;

                if (lp.shapmet == 0) {
                    calcTransition(lox, loy, ach, lp, zone, localFactor);
                } else if (lp.shapmet == 1) {
                    calcTransitionrect(lox, loy, ach, lp, zone, localFactor);//rect not good
                }

                float rL = origblur->L[y][x] / 327.68f;

                if (fabs(origblur->b[y][x]) < 0.01f) {
                    origblur->b[y][x] = 0.01f;
                }

                float dE = sqrt(kab * SQR(refa - origblur->a[y][x] / 327.68f) + kab * SQR(refb - origblur->b[y][x] / 327.68f) + kL * SQR(lumaref - rL));

                float reducdE = 0.f;
                calcreducdE(dE, maxdE, mindE, maxdElim, mindElim, lp.iterat, limscope, varsens, reducdE);
                float th_r = 0.01f;

                if (rL > th_r) { //to avoid crash with very low gamut in rare cases ex : L=0.01 a=0.5 b=-0.9

                    switch (zone) {
                        case 2: { // outside selection and outside transition zone => no effect, keep original values
                            transformed->L[y][x] = original->L[y][x];
                            transformed->a[y][x] = original->a[y][x];
                            transformed->b[y][x] = original->b[y][x];
                            break;
                        }

                        case 1: { // inside transition zone
                            float difa = 0.f;
                            float difb = 0.f;
                            float factorx = 1.f - localFactor;

                            if (senstype == 0) {
                                float epsia = 0.f;
                                float epsib = 0.f;
                                float lumnew = original->L[y][x];
                                float difL = (tempCL->L[y][x] - original->L[y][x]) * reducdE;
                                difa = (tempCL->a[y][x] - original->a[y][x]) * reducdE;
                                difb = (tempCL->b[y][x] - original->b[y][x]) * reducdE;
                                difL *= factorx;
                                difa *= factorx;
                                difb *= factorx;

                                if (original->a[y][x] == 0.f) {
                                    epsia = 0.0001f;
                                }

                                if (original->b[y][x] == 0.f) {
                                    epsib = 0.0001f;
                                }

                                float facCa = 1.f + (difa / (original->a[y][x] + epsia));
                                float facCb = 1.f + (difb / (original->b[y][x] + epsib));

                                if (lp.sens < 75.f) {
                                    float lightcont;

                                    if ((lp.ligh != 0.f || lp.cont != 0)) {
                                        calclight(lumnew, lp.ligh, lumnew, lightCurveloc);  //replace L-curve
                                        lightcont = lumnew;

                                    } else {
                                        lightcont = lumnew;
                                    }

                                    float fac = (100.f + factorx * lp.chro * reducdE) / 100.f; //chroma factor transition
                                    float diflc = (lightcont - original->L[y][x]) * reducdE;

                                    diflc *= factorx; //transition lightness
                                    transformed->L[y][x] = CLIP(1.f * (original->L[y][x] + diflc + difL));

                                    transformed->a[y][x] = CLIPC(original->a[y][x] * fac * facCa) ;
                                    transformed->b[y][x] = CLIPC(original->b[y][x] * fac * facCb);
                                } else {
                                    float fac = (100.f + factorx * lp.chro) / 100.f; //chroma factor transition

                                    if ((lp.ligh != 0.f || lp.cont != 0)) {
                                        calclight(original->L[y][x], lp.ligh, lumnew, lightCurveloc);
                                    }

                                    float lightcont = lumnew ; //apply lightness

                                    float diflc = lightcont - original->L[y][x];
                                    diflc *= factorx;
                                    transformed->L[y][x] = CLIP(original->L[y][x] + diflc + difL);
                                    transformed->a[y][x] = CLIPC(original->a[y][x] * fac * facCa);
                                    transformed->b[y][x] = CLIPC(original->b[y][x] * fac * facCb);


                                }
                            } else if (senstype == 1 || senstype == 2) {
                                float diflc = (temp->L[y][x] - original->L[y][x]) * reducdE;
                                diflc *= factorx;
                                difa = (temp->a[y][x] - original->a[y][x]) * reducdE;
                                difb = (temp->b[y][x] - original->b[y][x]) * reducdE;
                                difa *= factorx;
                                difb *= factorx;
                                transformed->L[y][x] = CLIP(original->L[y][x] + diflc);
                                transformed->a[y][x] = CLIPC(original->a[y][x] + difa) ;
                                transformed->b[y][x] = CLIPC(original->b[y][x] + difb);

                            }

                            break;
                        }

                        case 0: { // inside selection => full effect, no transition
                            float diflc = 0.f;
                            float difa = 0.f;
                            float difb = 0.f;

                            if (senstype == 0) {
                                float epsia = 0.f;
                                float epsib = 0.f;
                                float lumnew = original->L[y][x];
                                float difL = (tempCL->L[y][x] - original->L[y][x]) * reducdE;
                                difa = (tempCL->a[y][x] - original->a[y][x]) * reducdE;
                                difb = (tempCL->b[y][x] - original->b[y][x]) * reducdE;

                                if (original->a[y][x] == 0.f) {
                                    epsia = 0.0001f;
                                }

                                if (original->b[y][x] == 0.f) {
                                    epsib = 0.0001f;
                                }

                                float facCa = 1.f + (difa / (original->a[y][x] + epsia));
                                float facCb = 1.f + (difb / (original->b[y][x] + epsib));

                                if (lp.sens < 75.f) {

                                    float lightcont;

                                    if ((lp.ligh != 0.f || lp.cont != 0)) {
                                        calclight(lumnew, lp.ligh, lumnew, lightCurveloc);  //replace L-curve
                                        lightcont = lumnew;

                                    } else {
                                        lightcont = lumnew;
                                    }

                                    float fac = (100.f + lp.chro * reducdE) / 100.f; //chroma factor transition
                                    diflc = (lightcont - original->L[y][x]) * reducdE;

                                    transformed->L[y][x] = CLIP(1.f * (original->L[y][x] + diflc + difL));

                                    transformed->a[y][x] = CLIPC(original->a[y][x] * fac * facCa) ;
                                    transformed->b[y][x] = CLIPC(original->b[y][x] * fac * facCb);


                                } else {
                                    if ((lp.ligh != 0.f || lp.cont != 0)) {
                                        calclight(original->L[y][x], lp.ligh, lumnew, lightCurveloc);
                                    }

                                    float lightcont = lumnew ;
                                    transformed->L[y][x] = CLIP(lightcont + difL) ;
                                    transformed->a[y][x] = CLIPC(original->a[y][x] * facc * facCa);
                                    transformed->b[y][x] = CLIPC(original->b[y][x] * facc * facCb);

                                }
                            } else if (senstype == 1  || senstype == 2) {
                                diflc = (temp->L[y][x] - original->L[y][x]) * reducdE;
                                difa = (temp->a[y][x] - original->a[y][x]) * reducdE;
                                difb = (temp->b[y][x] - original->b[y][x]) * reducdE;
                                transformed->L[y][x] = CLIP(original->L[y][x] + diflc);
                                transformed->a[y][x] = CLIPC(original->a[y][x] + difa) ;
                                transformed->b[y][x] = CLIPC(original->b[y][x] + difb);
                            }

                        }
                    }

                }

            }
        }
    }
    delete origblur;

    if (senstype == 1 || senstype == 2) {
        delete temp;
    }

    if (senstype == 0) {
        delete tempCL;
    }

}

void ImProcFunctions::calc_ref(int sp, LabImage * original, LabImage * transformed, int cx, int cy, int oW, int oH, int sk, double & huerefblur, double & chromarefblur, double & lumarefblur, double & hueref, double & chromaref, double & lumaref, double & sobelref, float &avg)
{
    if (params->locallab.enabled) {
        //always calculate hueref, chromaref, lumaref  before others operations use in normal mode for all modules exceprt denoise
        struct local_params lp;
        calcLocalParams(sp, oW, oH, params->locallab, lp, 0, 0, 0, 0);
        int begy = lp.yc - lp.lyT;
        int begx = lp.xc - lp.lxL;
        int yEn = lp.yc + lp.ly;
        int xEn = lp.xc + lp.lx;
        float avg2 = 0.f;
        int nc2 = 0;

        for (int y = 0; y < transformed->H ; y++) //{
            for (int x = 0; x < transformed->W; x++) {
                int lox = cx + x;
                int loy = cy + y;

                if (lox >= begx && lox < xEn && loy >= begy && loy < yEn) {
                    avg2 += original->L[y][x];
                    nc2++;
                }
            }

        avg2 /= 32768.f;
        avg = avg2 / nc2;
//        printf("calc avg=%f \n", avg);
// double precision for large summations
        double aveA = 0.;
        double aveB = 0.;
        double aveL = 0.;
        double aveChro = 0.;
        double aveAblur = 0.;
        double aveBblur = 0.;
        double aveLblur = 0.;
        double aveChroblur = 0.;

        double avesobel = 0.;
// int precision for the counters
        int nab = 0;
        int nso = 0;
        int nsb = 0;
// single precision for the result
        float avA, avB, avL;
        int spotSize = 0.88623f * max(1,  lp.cir / sk);  //18
        //O.88623 = sqrt(PI / 4) ==> sqare equal to circle
        int spotSise2; // = 0.88623f * max (1,  lp.cir / sk); //18

        // very small region, don't use omp here
//      printf("cy=%i cx=%i yc=%f xc=%f circ=%i spot=%i tH=%i tW=%i sk=%i\n", cy, cx, lp.yc, lp.xc, lp.cir, spotSize, transformed->H, transformed->W, sk);
//      printf("ymin=%i ymax=%i\n", max (cy, (int) (lp.yc - spotSize)),min (transformed->H + cy, (int) (lp.yc + spotSize + 1)) );
//      printf("xmin=%i xmax=%i\n", max (cx, (int) (lp.xc - spotSize)),min (transformed->W + cx, (int) (lp.xc + spotSize + 1)) );
        LabImage *sobelL;
        LabImage *deltasobelL;
        LabImage *origsob;
        LabImage *origblur = nullptr;
        LabImage *blurorig = nullptr;

        int spotSi = 1 + 2 * max(1,  lp.cir / sk);

        if (spotSi < 5) {
            spotSi = 5;
        }

        spotSise2 = (spotSi - 1) / 2;

        JaggedArray<float> blend3(spotSi, spotSi);

        origsob = new LabImage(spotSi, spotSi);
        sobelL = new LabImage(spotSi, spotSi);
        deltasobelL = new LabImage(spotSi, spotSi);
        bool isdenoise = false;

        if ((lp.noiself > 0.f || lp.noiself0 > 0.f || lp.noiself2 > 0.f || lp.noiselc > 0.f || lp.noisecf > 0.f || lp.noisecc > 0.f) && lp.denoiena) {
            isdenoise = true;
        }

        if (isdenoise) {
            origblur = new LabImage(spotSi, spotSi);
            blurorig = new LabImage(spotSi, spotSi);

            for (int y = max(cy, (int)(lp.yc - spotSise2)); y < min(transformed->H + cy, (int)(lp.yc + spotSise2 + 1)); y++) {
                for (int x = max(cx, (int)(lp.xc - spotSise2)); x < min(transformed->W + cx, (int)(lp.xc + spotSise2 + 1)); x++) {
                    int yb = max(cy, (int)(lp.yc - spotSise2));

                    int xb = max(cx, (int)(lp.xc - spotSise2));

                    int z = y - yb;
                    int u = x - xb;
                    origblur->L[z][u] = original->L[y - cy][x - cx];
                    origblur->a[z][u] = original->a[y - cy][x - cx];
                    origblur->b[z][u] = original->b[y - cy][x - cx];

                }
            }

            float radius = 3.f / sk;
            {
                //No omp
                gaussianBlur(origblur->L, blurorig->L, spotSi, spotSi, radius);
                gaussianBlur(origblur->a, blurorig->a, spotSi, spotSi, radius);
                gaussianBlur(origblur->b, blurorig->b, spotSi, spotSi, radius);

            }

            for (int y = 0; y < spotSi; y++) {
                for (int x = 0; x < spotSi; x++) {
                    aveLblur += blurorig->L[y][x];
                    aveAblur += blurorig->a[y][x];
                    aveBblur += blurorig->b[y][x];
                    aveChroblur += sqrtf(SQR(blurorig->b[y - cy][x - cx]) + SQR(blurorig->a[y - cy][x - cx]));
                    nsb++;

                }
            }
        }

        //ref for luma, chroma, hue
        for (int y = max(cy, (int)(lp.yc - spotSize)); y < min(transformed->H + cy, (int)(lp.yc + spotSize + 1)); y++) {
            for (int x = max(cx, (int)(lp.xc - spotSize)); x < min(transformed->W + cx, (int)(lp.xc + spotSize + 1)); x++) {
                aveL += original->L[y - cy][x - cx];
                aveA += original->a[y - cy][x - cx];
                aveB += original->b[y - cy][x - cx];
                //    aveblend += 100.f * blend2[y - cy][x - cx];
                aveChro += sqrtf(SQR(original->b[y - cy][x - cx]) + SQR(original->a[y - cy][x - cx]));
                nab++;
            }
        }

        //ref for sobel
        for (int y = max(cy, (int)(lp.yc - spotSise2)); y < min(transformed->H + cy, (int)(lp.yc + spotSise2 + 1)); y++) {
            for (int x = max(cx, (int)(lp.xc - spotSise2)); x < min(transformed->W + cx, (int)(lp.xc + spotSise2 + 1)); x++) {
                int yb = max(cy, (int)(lp.yc - spotSise2));

                int xb = max(cx, (int)(lp.xc - spotSise2));

                int z = y - yb;
                int u = x - xb;
                origsob->L[z][u] = original->L[y - cy][x - cx];
                nso++;
            }
        }

        const float radius = 3.f / (sk * 1.4f); //0 to 70 ==> see skip

        SobelCannyLuma(sobelL->L, origsob->L, spotSi, spotSi, radius);
        int nbs = 0;

        for (int y = 0; y < spotSi ; y ++)
            for (int x = 0; x < spotSi ; x ++) {
                avesobel += sobelL->L[y][x];
                nbs++;
            }

        sobelref = avesobel / nbs;

        delete sobelL;

        delete deltasobelL;
        delete origsob;
        aveL = aveL / nab;
        aveA = aveA / nab;
        aveB = aveB / nab;
        aveChro = aveChro / nab;
        aveChro /= 327.68f;
        avA = aveA / 327.68f;
        avB = aveB / 327.68f;
        avL = aveL / 327.68f;
        hueref = xatan2f(avB, avA);    //mean hue

        if (isdenoise) {
            aveLblur = aveLblur / nsb;
            aveChroblur = aveChroblur / nsb;
            aveChroblur /= 327.68f;
            aveAblur = aveAblur / nsb;
            aveBblur = aveBblur / nsb;
            float avAblur = aveAblur / 327.68f;
            float avBblur = aveBblur / 327.68f;
            float avLblur = aveLblur / 327.68f;
            huerefblur = xatan2f(avBblur, avAblur);
            chromarefblur = aveChroblur;
            lumarefblur = avLblur;
        } else {
            huerefblur = 0.f;
            chromarefblur = 0.f;
            lumarefblur = 0.f;
        }

        //    printf("hueblur=%f hue=%f\n", huerefblur, hueref);
        chromaref = aveChro;
        lumaref = avL;

        //  printf("Calcref => sp=%i befend=%i huere=%2.1f chromare=%2.1f lumare=%2.1f sobelref=%2.1f\n", sp, befend, hueref, chromaref, lumaref, sobelref / 100.f);

        if (isdenoise) {
            delete origblur;
            delete blurorig;
        }

        if (lumaref > 95.f) {//to avoid crash
            lumaref = 95.f;
        }
    }
}

void ImProcFunctions::fftw_denoise(int GW, int GH, int max_numblox_W, int min_numblox_W, float **tmp1, array2D<float> *Lin, int numThreads, const struct local_params & lp, int chrom)
{

    fftwf_plan plan_forward_blox[2];
    fftwf_plan plan_backward_blox[2];

    array2D<float> tilemask_in(TS, TS);
    array2D<float> tilemask_out(TS, TS);

    float *Lbloxtmp  = reinterpret_cast<float*>(fftwf_malloc(max_numblox_W * TS * TS * sizeof(float)));
    float *fLbloxtmp = reinterpret_cast<float*>(fftwf_malloc(max_numblox_W * TS * TS * sizeof(float)));

    int nfwd[2] = {TS, TS};

    //for DCT:
    fftw_r2r_kind fwdkind[2] = {FFTW_REDFT10, FFTW_REDFT10};
    fftw_r2r_kind bwdkind[2] = {FFTW_REDFT01, FFTW_REDFT01};

    // Creating the plans with FFTW_MEASURE instead of FFTW_ESTIMATE speeds up the execute a bit
    plan_forward_blox[0]  = fftwf_plan_many_r2r(2, nfwd, max_numblox_W, Lbloxtmp, nullptr, 1, TS * TS, fLbloxtmp, nullptr, 1, TS * TS, fwdkind, FFTW_MEASURE | FFTW_DESTROY_INPUT);
    plan_backward_blox[0] = fftwf_plan_many_r2r(2, nfwd, max_numblox_W, fLbloxtmp, nullptr, 1, TS * TS, Lbloxtmp, nullptr, 1, TS * TS, bwdkind, FFTW_MEASURE | FFTW_DESTROY_INPUT);
    plan_forward_blox[1]  = fftwf_plan_many_r2r(2, nfwd, min_numblox_W, Lbloxtmp, nullptr, 1, TS * TS, fLbloxtmp, nullptr, 1, TS * TS, fwdkind, FFTW_MEASURE | FFTW_DESTROY_INPUT);
    plan_backward_blox[1] = fftwf_plan_many_r2r(2, nfwd, min_numblox_W, fLbloxtmp, nullptr, 1, TS * TS, Lbloxtmp, nullptr, 1, TS * TS, bwdkind, FFTW_MEASURE | FFTW_DESTROY_INPUT);
    fftwf_free(Lbloxtmp);
    fftwf_free(fLbloxtmp);
    const int border = MAX(2, TS / 16);

    for (int i = 0; i < TS; ++i) {
        float i1 = abs((i > TS / 2 ? i - TS + 1 : i));
        float vmask = (i1 < border ? SQR(sin((rtengine::RT_PI_F * i1) / (2 * border))) : 1.0f);
        float vmask2 = (i1 < 2 * border ? SQR(sin((rtengine::RT_PI_F * i1) / (2 * border))) : 1.0f);

        for (int j = 0; j < TS; ++j) {
            float j1 = abs((j > TS / 2 ? j - TS + 1 : j));
            tilemask_in[i][j] = (vmask * (j1 < border ? SQR(sin((rtengine::RT_PI_F * j1) / (2 * border))) : 1.0f)) + epsilon;
            tilemask_out[i][j] = (vmask2 * (j1 < 2 * border ? SQR(sin((rtengine::RT_PI_F * j1) / (2 * border))) : 1.0f)) + epsilon;

        }
    }


    float *LbloxArray[numThreads];
    float *fLbloxArray[numThreads];



    const int numblox_W = ceil((static_cast<float>(GW)) / (offset)) + 2 * blkrad;
    const int numblox_H = ceil((static_cast<float>(GH)) / (offset)) + 2 * blkrad;


    //residual between input and denoised L channel
    array2D<float> Ldetail(GW, GH, ARRAY2D_CLEAR_DATA);
    array2D<float> totwt(GW, GH, ARRAY2D_CLEAR_DATA); //weight for combining DCT blocks

    for (int i = 0; i < numThreads; ++i) {
        LbloxArray[i]  = reinterpret_cast<float*>(fftwf_malloc(max_numblox_W * TS * TS * sizeof(float)));
        fLbloxArray[i] = reinterpret_cast<float*>(fftwf_malloc(max_numblox_W * TS * TS * sizeof(float)));
    }

#ifdef _OPENMP
    int masterThread = omp_get_thread_num();
#endif
#ifdef _OPENMP
    #pragma omp parallel
#endif
    {
#ifdef _OPENMP
        int subThread = masterThread * 1 + omp_get_thread_num();
#else
        int subThread = 0;
#endif
        float blurbuffer[TS * TS] ALIGNED64;
        float *Lblox = LbloxArray[subThread];
        float *fLblox = fLbloxArray[subThread];
        float pBuf[GW + TS + 2 * blkrad * offset] ALIGNED16;
        float nbrwt[TS * TS] ALIGNED64;
#ifdef _OPENMP
        #pragma omp for
#endif

        for (int vblk = 0; vblk < numblox_H; ++vblk) {

            int top = (vblk - blkrad) * offset;
            float * datarow = pBuf + blkrad * offset;

            for (int i = 0; i < TS; ++i) {
                int row = top + i;
                int rr = row;

                if (row < 0) {
                    rr = MIN(-row, GH - 1);
                } else if (row >= GH) {
                    rr = MAX(0, 2 * GH - 2 - row);
                }

                for (int j = 0; j < GW; ++j) {
                    datarow[j] = ((*Lin)[rr][j] - tmp1[rr][j]);
                }

                for (int j = -blkrad * offset; j < 0; ++j) {
                    datarow[j] = datarow[MIN(-j, GW - 1)];
                }

                for (int j = GW; j < GW + TS + blkrad * offset; ++j) {
                    datarow[j] = datarow[MAX(0, 2 * GW - 2 - j)];
                }//now we have a padded data row

                //now fill this row of the blocks with Lab high pass data
                for (int hblk = 0; hblk < numblox_W; ++hblk) {
                    int left = (hblk - blkrad) * offset;
                    int indx = (hblk) * TS; //index of block in malloc

                    if (top + i >= 0 && top + i < GH) {
                        int j;

                        for (j = 0; j < min((-left), TS); ++j) {
                            Lblox[(indx + i)*TS + j] = tilemask_in[i][j] * datarow[left + j]; // luma data
                        }

                        for (; j < min(TS, GW - left); ++j) {
                            Lblox[(indx + i)*TS + j] = tilemask_in[i][j] * datarow[left + j]; // luma data
                            totwt[top + i][left + j] += tilemask_in[i][j] * tilemask_out[i][j];
                        }

                        for (; j < TS; ++j) {
                            Lblox[(indx + i)*TS + j] = tilemask_in[i][j] * datarow[left + j]; // luma data
                        }
                    } else {
                        for (int j = 0; j < TS; ++j) {
                            Lblox[(indx + i)*TS + j] = tilemask_in[i][j] * datarow[left + j]; // luma data
                        }
                    }

                }

            }//end of filling block row

            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            //fftwf_print_plan (plan_forward_blox);
            if (numblox_W == max_numblox_W) {
                fftwf_execute_r2r(plan_forward_blox[0], Lblox, fLblox);    // DCT an entire row of tiles
            } else {
                fftwf_execute_r2r(plan_forward_blox[1], Lblox, fLblox);    // DCT an entire row of tiles
            }

            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            // now process the vblk row of blocks for noise reduction

            float params_Ldetail = 0.f;
            float noisevar_Ldetail = 1.f;

            if (chrom == 0) {
                params_Ldetail = min(float(lp.noiseldetail), 99.9f);    // max out to avoid div by zero when using noisevar_Ldetail as divisor
                noisevar_Ldetail = SQR(static_cast<float>(SQR(100. - params_Ldetail) + 50.*(100. - params_Ldetail)) * TS * 0.5f);
            } else if (chrom == 1) {
                params_Ldetail = min(float(lp.noisechrodetail), 99.9f);
                noisevar_Ldetail = 100.f * pow((static_cast<float>(SQR(100. - params_Ldetail) + 50.*(100. - params_Ldetail)) * TS * 0.5f), 2);//to test ???
            }

            //   float noisevar_Ldetail = SQR(static_cast<float>(SQR(100. - params_Ldetail) + 50.*(100. - params_Ldetail)) * TS * 0.5f);



            for (int hblk = 0; hblk < numblox_W; ++hblk) {
                ImProcFunctions::RGBtile_denoise(fLblox, hblk, noisevar_Ldetail, nbrwt, blurbuffer);
            }//end of horizontal block loop

            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

            //now perform inverse FT of an entire row of blocks
            if (numblox_W == max_numblox_W) {
                fftwf_execute_r2r(plan_backward_blox[0], fLblox, Lblox);    //for DCT
            } else {
                fftwf_execute_r2r(plan_backward_blox[1], fLblox, Lblox);    //for DCT
            }

            int topproc = (vblk - blkrad) * offset;

            //add row of blocks to output image tile
            ImProcFunctions::RGBoutput_tile_row(Lblox, Ldetail, tilemask_out, GH, GW, topproc);

            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        }//end of vertical block loop

        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    }
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#ifdef _OPENMP

    #pragma omp parallel for
#endif

    for (int i = 0; i < GH; ++i) {
        for (int j = 0; j < GW; ++j) {
            //may want to include masking threshold for large hipass data to preserve edges/detail
            tmp1[i][j] += Ldetail[i][j] / totwt[i][j]; //note that labdn initially stores the denoised hipass data
        }
    }


    delete Lin;


    for (int i = 0; i < numThreads; ++i) {
        fftwf_free(LbloxArray[i]);
        fftwf_free(fLbloxArray[i]);
    }

    fftwf_destroy_plan(plan_forward_blox[0]);
    fftwf_destroy_plan(plan_backward_blox[0]);
    fftwf_destroy_plan(plan_forward_blox[1]);
    fftwf_destroy_plan(plan_backward_blox[1]);
    fftwf_cleanup();


}

void ImProcFunctions::Lab_Local(int call, int sp, float** shbuffer, LabImage * original, LabImage * transformed, LabImage * reserved, int cx, int cy, int oW, int oH, int sk,
                                const LocretigainCurve & locRETgainCcurve, LUTf & lllocalcurve, bool & locallutili, const LocLHCurve & loclhCurve,  const LocHHCurve & lochhCurve, const LocCCmaskCurve & locccmasCurve, bool & lcmasutili, const  LocLLmaskCurve & locllmasCurve, bool & llmasutili, const  LocHHmaskCurve & lochhmasCurve, bool &lhmasutili, const LocCCmaskexpCurve & locccmasexpCurve, bool &lcmasexputili, const  LocLLmaskexpCurve & locllmasexpCurve, bool &llmasexputili, const  LocHHmaskexpCurve & lochhmasexpCurve, bool & lhmasexputili,
                                const LocCCmaskSHCurve & locccmasSHCurve, bool &lcmasSHutili, const  LocLLmaskSHCurve & locllmasSHCurve, bool &llmasSHutili, const  LocHHmaskSHCurve & lochhmasSHCurve, bool & lhmasSHutili,
                                const LocCCmaskcbCurve & locccmascbCurve, bool &lcmascbutili, const  LocLLmaskcbCurve & locllmascbCurve, bool &llmascbutili, const  LocHHmaskcbCurve & lochhmascbCurve, bool & lhmascbutili,
                                bool & LHutili, bool & HHutili, LUTf & cclocalcurve, bool & localcutili, bool & localexutili, LUTf & exlocalcurve, LUTf & hltonecurveloc, LUTf & shtonecurveloc, LUTf & tonecurveloc, LUTf & lightCurveloc, double & huerefblur, double &chromarefblur, double & lumarefblur, double & hueref, double & chromaref, double & lumaref, double & sobelref, int llColorMask, int llExpMask, int llSHMask, int llcbMask)
{
    //general call of others functions : important return hueref, chromaref, lumaref
    if (params->locallab.enabled) {
        BENCHFUN
#ifdef _DEBUG
// init variables to display Munsell corrections
        MunsellDebugInfo* MunsDebugInfo = new MunsellDebugInfo();
#endif

        int del = 3; // to avoid crash with [loy - begy] and [lox - begx] and bfh bfw  // with gtk2 [loy - begy-1] [lox - begx -1 ] and del = 1

        struct local_params lp;
        calcLocalParams(sp, oW, oH, params->locallab, lp, llColorMask, llExpMask, llSHMask, llcbMask);

        const float radius = lp.rad / (sk * 1.4f); //0 to 70 ==> see skip
        int strred = 1;//(lp.strucc - 1);

        float radiussob = strred / (sk * 1.4f);
        int levred;
        bool noiscfactiv = false;

        if (lp.qualmet == 2) { //suppress artifacts with quality enhanced
            levred = 4;
            noiscfactiv = true;
        }    else {
            levred = 7;
            noiscfactiv = false;
        }


// we must here detect : general case, skin, sky,...foliages ???




        if (lp.excmet == 1  && call <= 3) {//exclude
            const int bfh = int (lp.ly + lp.lyT) + del; //bfw bfh real size of square zone
            const int bfw = int (lp.lx + lp.lxL) + del;
            const int begy = lp.yc - lp.lyT;
            const int begx = lp.xc - lp.lxL;
            const int yEn = lp.yc + lp.ly;
            const int xEn = lp.xc + lp.lx;
            LabImage bufreserv(bfw, bfh);
            array2D<float> bufsob(bfw, bfh);

#ifdef _OPENMP
            #pragma omp parallel for schedule(dynamic,16)
#endif

            for (int y = std::max(begy - cy, 0); y < std::min(yEn - cy, original->H); y++) {
                const int loy = cy + y;

                for (int x = std::max(begx - cx, 0); x < std::min(xEn - cx, original->W); x++) {
                    const int lox = cx + x;

                    bufsob[loy - begy][lox - begx] = bufreserv.L[loy - begy][lox - begx] = reserved->L[y][x];
                    bufreserv.a[loy - begy][lox - begx] = reserved->a[y][x];
                    bufreserv.b[loy - begy][lox - begx] = reserved->b[y][x];
                }
            }

            array2D<float> ble(bfw, bfh);
            SobelCannyLuma(ble, bufsob, bfw, bfh, radiussob, true);
            array2D<float> &guid = bufsob;

#ifdef _OPENMP
            #pragma omp parallel for
#endif

            for (int ir = 0; ir < bfh; ir++)
                for (int jr = 0; jr < bfw; jr++) {
                    ble[ir][jr] /= 32768.f;
                    guid[ir][jr] /= 32768.f;
                }

            const float blur = 25 / sk * (10.f + 1.2f * lp.struexp);

            rtengine::guidedFilter(guid, ble, ble, blur, 0.001, multiThread);

            double sombel = 0.f;
            const int ncsobel = bfh * bfw;
            float maxsob = -1.f;
            float minsob = 100000.f;

            array2D<float> &deltasobelL = guid;

#ifdef _OPENMP
            #pragma omp parallel for reduction(+:sombel) reduction(min:minsob) reduction(max:maxsob)
#endif

            for (int ir = 0; ir < bfh; ir++) {
                for (int jr = 0; jr < bfw; jr++) {
                    const float val = ble[ir][jr] * 32768.f;
                    sombel += val;
                    minsob = rtengine::min(maxsob, val);
                    maxsob = rtengine::max(minsob, val);
                    deltasobelL[ir][jr] = val;
                }
            }

            const float meansob = sombel / ncsobel;

            Exclude_Local(deltasobelL, hueref, chromaref, lumaref, sobelref, meansob, lp, original, transformed, &bufreserv, reserved, cx, cy, sk);

        }

//Blur and noise

        if (((radius >= 1.5 * GAUSS_SKIP && lp.rad > 1.) || lp.stren > 0.1) && lp.blurena) { // radius < GAUSS_SKIP means no gauss, just copy of original image
            std::unique_ptr<LabImage> tmp1;

            if (call <= 3 && lp.blurmet == 0) {
                const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
                const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
                const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
                const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);
                const int bfh = yend - ystart;
                const int bfw = xend - xstart;

                if (bfw > 0 && bfh > 0) {
                    tmp1.reset(new LabImage(bfw, bfh));
#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = ystart; y < yend ; y++) {
                        for (int x = xstart; x < xend; x++) {
                            tmp1->L[y - ystart][x - xstart] = original->L[y][x];
                            tmp1->a[y - ystart][x - xstart] = original->a[y][x];
                            tmp1->b[y - ystart][x - xstart] = original->b[y][x];
                        }
                    }

#ifdef _OPENMP
                    #pragma omp parallel
#endif

                    {
                        gaussianBlur(tmp1->L, tmp1->L, bfw, bfh, radius);
                        gaussianBlur(tmp1->a, tmp1->a, bfw, bfh, radius);
                        gaussianBlur(tmp1->b, tmp1->b, bfw, bfh, radius);
                    }
                }
            } else {
                const int GW = transformed->W;
                const int GH = transformed->H;
                tmp1.reset(new LabImage(transformed->W, transformed->H));

#ifdef _OPENMP
                #pragma omp parallel
#endif
                {
                    gaussianBlur(original->L, tmp1->L, GW, GH, radius);
                    gaussianBlur(original->a, tmp1->a, GW, GH, radius);
                    gaussianBlur(original->b, tmp1->b, GW, GH, radius);
                }
            }

            if (tmp1.get() && lp.stren > 0.1f) {
                float mean = 0.f;//0 best result
                float variance = lp.stren ; //(double) SQR(lp.stren)/sk;
                addGaNoise(tmp1.get(), tmp1.get(), mean, variance, sk) ;
            }

            if (lp.blurmet == 0) { //blur and noise (center)
                if (tmp1.get()) {
                    BlurNoise_Local(tmp1.get(), hueref, chromaref, lumaref, lp, original, transformed, cx, cy, sk);
                }
            } else {
               InverseBlurNoise_Local(lp, hueref, chromaref, lumaref, original, transformed, tmp1.get(), cx, cy, sk);
            }
        }


        //local impulse
        if ((lp.bilat > 0.f) && lp.denoiena) {
            const int bfh = int (lp.ly + lp.lyT) + del; //bfw bfh real size of square zone
            const int bfw = int (lp.lx + lp.lxL) + del;

            std::unique_ptr<LabImage> bufwv;

            if (call == 2) {//simpleprocess
                bufwv.reset(new LabImage(bfw, bfh)); //buffer for data in zone limit

                const int begy = lp.yc - lp.lyT;
                const int begx = lp.xc - lp.lxL;
                const int yEn = lp.yc + lp.ly;
                const int xEn = lp.xc + lp.lx;

#ifdef _OPENMP
                #pragma omp parallel for schedule(dynamic,16)
#endif

                for (int y = rtengine::max(0, begy - cy); y < rtengine::min(transformed->H, yEn - cy); y++) {
                    const int loy = cy + y;
                    for (int x = rtengine::max(0, begx - cx); x < rtengine::min(transformed->W, xEn - cx); x++) {
                        const int lox = cx + x;
                        bufwv->L[loy - begy][lox - begx] = original->L[y][x];
                        bufwv->a[loy - begy][lox - begx] = original->a[y][x];
                        bufwv->b[loy - begy][lox - begx] = original->b[y][x];
                    }
                }
            } else {//dcrop.cc
                bufwv.reset(new LabImage(transformed->W, transformed->H));
                bufwv->CopyFrom(original);
            } //end dcrop

            const double threshold = lp.bilat / 20.0;

            if (bfh > 8 && bfw > 8) {
                ImProcFunctions::impulse_nr(bufwv.get(), threshold);
            }

            DeNoise_Local(call, lp, levred, huerefblur, lumarefblur, chromarefblur, original, transformed, *(bufwv.get()), cx, cy, sk);
        }

//local denoise
        //all these variables are to prevent use of denoise when non necessary
        // but with qualmet = 2 (default for best quality) we must denoise chroma with little values to prevent artifacts due to variations of Hue
        // but if user select volontary denoise, it is that choice the good (prioritary)
        bool execcolor = (lp.chro != 0.f || lp.ligh != 0.f || lp.cont != 0); // only if one slider ore more is engaged
        bool execbdl = (lp.mulloc[0] != 1.f || lp.mulloc[1] != 1.f || lp.mulloc[2] != 1.f || lp.mulloc[3] != 1.f || lp.mulloc[4] != 1.f) ;//only if user want cbdl
        bool execdenoi = noiscfactiv && ((lp.colorena && execcolor) || (lp.tonemapena && lp.strengt != 0.f) || (lp.cbdlena && execbdl) || (lp.sfena && lp.strng > 0.f) || (lp.lcena && lp.lcamount > 0.f) || (lp.sharpena && lp.shrad > 0.42) || (lp.retiena  && lp.str > 0.f)  || (lp.exposena && lp.expcomp != 0.f)  || (lp.expvib  && lp.past != 0.f));

        if (((lp.noiself > 0.f || lp.noiself0 > 0.f || lp.noiself2 > 0.f || lp.noiselc > 0.f || lp.noisecf > 0.f || lp.noisecc > 0.f) && lp.denoiena) || execdenoi) {  // sk == 1 ??
            StopWatch Stop1("locallab Denoise called");
            MyMutex::MyLock lock(*fftwMutex);

            if (lp.noisecf >= 0.1f || lp.noisecc >= 0.1f) {
                noiscfactiv = false;
                levred = 7;
            }


#ifdef _OPENMP
            const int numThreads = omp_get_max_threads();
#else
            const int numThreads = 1;

#endif

            if (call == 1) {


                LabImage tmp1(transformed->W, transformed->H);
                LabImage tmp2(transformed->W, transformed->H);
                tmp2.clear();

                array2D<float> *Lin = nullptr;
                array2D<float> *Ain = nullptr;
                array2D<float> *Bin = nullptr;


                int GW = transformed->W;
                int GH = transformed->H;
                int max_numblox_W = ceil((static_cast<float>(GW)) / (offset)) + 2 * blkrad;
                // calculate min size of numblox_W.
                int min_numblox_W = ceil((static_cast<float>(GW)) / (offset)) + 2 * blkrad;


                for (int ir = 0; ir < GH; ir++)
                    for (int jr = 0; jr < GW; jr++) {
                        tmp1.L[ir][jr] = original->L[ir][jr];
                        tmp1.a[ir][jr] = original->a[ir][jr];
                        tmp1.b[ir][jr] = original->b[ir][jr];
                    }

                int DaubLen = 6;

                int levwavL = levred;
                int skip = 1;

                wavelet_decomposition Ldecomp(tmp1.L[0], tmp1.W, tmp1.H, levwavL, 1, skip, numThreads, DaubLen);
                wavelet_decomposition adecomp(tmp1.a[0], tmp1.W, tmp1.H, levwavL, 1, skip, numThreads, DaubLen);
                wavelet_decomposition bdecomp(tmp1.b[0], tmp1.W, tmp1.H, levwavL, 1, skip, numThreads, DaubLen);

                float madL[8][3];
                int edge = 2;

                if (!Ldecomp.memoryAllocationFailed) {
                    #pragma omp parallel for collapse(2) schedule(dynamic,1)

                    for (int lvl = 0; lvl < levred; lvl++) {
                        for (int dir = 1; dir < 4; dir++) {
                            int Wlvl_L = Ldecomp.level_W(lvl);
                            int Hlvl_L = Ldecomp.level_H(lvl);

                            float ** WavCoeffs_L = Ldecomp.level_coeffs(lvl);

                            madL[lvl][dir - 1] = SQR(Mad(WavCoeffs_L[dir], Wlvl_L * Hlvl_L));
                        }
                    }

                    float vari[levred];

                    if (levred == 7) {
                        edge = 2;
                        vari[0] = 8.f * SQR((lp.noiself0 / 125.0) * (1.0 + lp.noiself0 / 25.0));
                        vari[1] = 8.f * SQR((lp.noiself / 125.0) * (1.0 + lp.noiself / 25.0));
                        vari[2] = 8.f * SQR((lp.noiself2 / 125.0) * (1.0 + lp.noiself2 / 25.0));

                        vari[3] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                        vari[4] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                        vari[5] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                        vari[6] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                    } else if (levred == 4) {
                        edge = 3;
                        vari[0] = 8.f * SQR((lp.noiself0 / 125.0) * (1.0 + lp.noiself0 / 25.0));
                        vari[1] = 8.f * SQR((lp.noiself / 125.0) * (1.0 + lp.noiself / 25.0));
                        vari[2] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                        vari[3] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));

                    }

                    if ((lp.noiself >= 0.1f ||  lp.noiself0 >= 0.1f ||  lp.noiself2 >= 0.1f || lp.noiselc >= 0.1f)) {
                        float kr3 = 0.f;
                        float kr4 = 0.f;
                        float kr5 = 0.f;

                        if (lp.noiselc < 30.f) {
                            kr3 = 0.f;
                            kr4 = 0.f;
                            kr5 = 0.f;
                        } else if (lp.noiselc < 50.f) {
                            kr3 = 0.5f;
                            kr4 = 0.3f;
                            kr5 = 0.2f;
                        } else if (lp.noiselc < 70.f) {
                            kr3 = 0.7f;
                            kr4 = 0.5f;
                            kr5 = 0.3f;
                        } else {
                            kr3 = 1.f;
                            kr4 = 1.f;
                            kr5 = 1.f;
                        }

                        vari[0] = max(0.0001f, vari[0]);
                        vari[1] = max(0.0001f, vari[1]);
                        vari[2] = max(0.0001f, vari[2]);
                        vari[3] = max(0.0001f, kr3 * vari[3]);

                        if (levred == 7) {
                            vari[4] = max(0.0001f, kr4 * vari[4]);
                            vari[5] = max(0.0001f, kr5 * vari[5]);
                            vari[6] = max(0.0001f, kr5 * vari[6]);
                        }

                        float* noisevarlum = new float[GH * GW];
                        int GW2 = (GW + 1) / 2;

                        float nvlh[13] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0.7f, 0.5f}; //high value
                        float nvll[13] = {0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.7f, 0.8f, 1.f, 1.f, 1.f}; //low value

                        float seuillow = 3000.f;//low
                        float seuilhigh = 18000.f;//high
                        int i = 10 - lp.noiselequal;
                        float ac = (nvlh[i] - nvll[i]) / (seuillow - seuilhigh);
                        float bc = nvlh[i] - seuillow * ac;
                        //ac and bc for transition
#ifdef _OPENMP
                        #pragma omp parallel for

#endif

                        for (int ir = 0; ir < GH; ir++)
                            for (int jr = 0; jr < GW; jr++) {
                                float lN = tmp1.L[ir][jr];

                                if (lN < seuillow) {
                                    noisevarlum[(ir >> 1)*GW2 + (jr >> 1)] =  nvlh[i];
                                } else if (lN < seuilhigh) {
                                    noisevarlum[(ir >> 1)*GW2 + (jr >> 1)] = ac * lN + bc;
                                } else {
                                    noisevarlum[(ir >> 1)*GW2 + (jr >> 1)] =  nvll[i];
                                }
                            }


                        if (lp.noiselc < 1.f) {
                            WaveletDenoiseAllL(Ldecomp, noisevarlum, madL, vari, edge, numThreads);
                        } else {
                            WaveletDenoiseAll_BiShrinkL(Ldecomp, noisevarlum, madL, vari, edge, numThreads);
                            WaveletDenoiseAllL(Ldecomp, noisevarlum, madL, vari, edge, numThreads);
                        }

                        delete[] noisevarlum;

                    }
                }

                float variC[levred];
                float variCb[levred];

                float noisecfr = lp.noisecf;
                float noiseccr = lp.noisecc;

                if (lp.adjch > 0.f) {
                    noisecfr = lp.noisecf * ((100.f + lp.adjch) / 10.f);
                    noiseccr = lp.noisecc + ((100.f + lp.adjch) / 10.f);
                }

                float noisecfb = lp.noisecf;
                float noiseccb = lp.noisecc;

                if (lp.adjch < 0.f) {
                    noisecfb = lp.noisecf * ((100.f - lp.adjch) / 10.f);
                    noiseccb = lp.noisecc * ((100.f - lp.adjch) / 10.f);
                }


                if (noisecfr < 0.f) {
                    noisecfr = 0.0001f;
                }

                if (noiseccr < 0.f) {
                    noiseccr = 0.0001f;
                }

                if (noisecfb < 0.f) {
                    noisecfb = 0.0001f;
                }

                if (noiseccb < 0.f) {
                    noiseccb = 0.0001f;
                }

                if (!adecomp.memoryAllocationFailed && !bdecomp.memoryAllocationFailed) {

                    if (levred == 7) {
                        edge = 2;
                        variC[0] = SQR(noisecfr);
                        variC[1] = SQR(noisecfr);
                        variC[2] = SQR(noisecfr);

                        variC[3] = SQR(noisecfr);
                        variC[4] = SQR(noisecfr);
                        variC[5] = SQR(noiseccr);
                        variC[6] = SQR(noiseccr);

                        variCb[0] = SQR(noisecfb);
                        variCb[1] = SQR(noisecfb);
                        variCb[2] = SQR(noisecfb);

                        variCb[3] = SQR(noisecfb);
                        variCb[4] = SQR(noisecfb);
                        variCb[5] = SQR(noiseccb);
                        variCb[6] = SQR(noiseccb);

                    } else if (levred == 4) {
                        edge = 3;
                        variC[0] = SQR(lp.noisecf / 10.0);
                        variC[1] = SQR(lp.noisecf / 10.0);
                        variC[2] = SQR(lp.noisecf / 10.0);
                        variC[3] = SQR(lp.noisecf / 10.0);

                        variCb[0] = SQR(lp.noisecf / 10.0);
                        variCb[1] = SQR(lp.noisecf / 10.0);
                        variCb[2] = SQR(lp.noisecf / 10.0);
                        variCb[3] = SQR(lp.noisecf / 10.0);


                    }

                    if ((lp.noisecf >= 0.1f ||  lp.noisecc >= 0.1f  || noiscfactiv)) {
                        float minic = 0.0001f;

                        if (noiscfactiv) {
                            minic = 0.1f;//only for artifact shape detection
                        }

                        float k1 = 0.f;
                        float k2 = 0.f;
                        float k3 = 0.f;

                        if (lp.noisecf < 0.2f) {
                            k1 = 0.f;
                            k2 = 0.f;
                            k3 = 0.f;
                        } else if (lp.noisecf < 0.3f) {
                            k1 = 0.1f;
                            k2 = 0.0f;
                            k3 = 0.f;
                        } else if (lp.noisecf < 0.5f) {
                            k1 = 0.2f;
                            k2 = 0.1f;
                            k3 = 0.f;
                        } else if (lp.noisecf < 0.8f) {
                            k1 = 0.3f;
                            k2 = 0.25f;
                            k3 = 0.f;
                        } else if (lp.noisecf < 1.f) {
                            k1 = 0.4f;
                            k2 = 0.25f;
                            k3 = 0.1f;
                        } else if (lp.noisecf < 2.f) {
                            k1 = 0.5f;
                            k2 = 0.3f;
                            k3 = 0.15f;
                        } else if (lp.noisecf < 3.f) {
                            k1 = 0.6f;
                            k2 = 0.45f;
                            k3 = 0.3f;
                        } else if (lp.noisecf < 4.f) {
                            k1 = 0.7f;
                            k2 = 0.5f;
                            k3 = 0.4f;
                        } else if (lp.noisecf < 5.f) {
                            k1 = 0.8f;
                            k2 = 0.6f;
                            k3 = 0.5f;
                        } else if (lp.noisecf < 10.f) {
                            k1 = 0.85f;
                            k2 = 0.7f;
                            k3 = 0.6f;
                        } else if (lp.noisecf < 20.f) {
                            k1 = 0.9f;
                            k2 = 0.8f;
                            k3 = 0.7f;
                        } else if (lp.noisecf < 50.f) {
                            k1 = 1.f;
                            k2 = 1.f;
                            k3 = 0.9f;

                        } else {
                            k1 = 1.f;
                            k2 = 1.f;
                            k3 = 1.f;
                        }

                        variC[0] = max(minic, variC[0]);
                        variC[1] = max(minic, k1 * variC[1]);
                        variC[2] = max(minic, k2 * variC[2]);
                        variC[3] = max(minic, k3 * variC[3]);

                        variCb[0] = max(minic, variCb[0]);
                        variCb[1] = max(minic, k1 * variCb[1]);
                        variCb[2] = max(minic, k2 * variCb[2]);
                        variCb[3] = max(minic, k3 * variCb[3]);

                        if (levred == 7) {
                            float k4 = 0.f;
                            float k5 = 0.f;
                            float k6 = 0.f;

                            if (lp.noisecc == 0.1f) {
                                k4 = 0.f;
                                k5 = 0.0f;
                            } else if (lp.noisecc < 0.2f) {
                                k4 = 0.1f;
                                k5 = 0.0f;
                            } else if (lp.noisecc < 0.5f) {
                                k4 = 0.15f;
                                k5 = 0.0f;
                            } else if (lp.noisecc < 1.f) {
                                k4 = 0.15f;
                                k5 = 0.1f;
                            } else if (lp.noisecc < 3.f) {
                                k4 = 0.3f;
                                k5 = 0.15f;
                            } else if (lp.noisecc < 4.f) {
                                k4 = 0.6f;
                                k5 = 0.4f;
                            } else if (lp.noisecc < 6.f) {
                                k4 = 0.8f;
                                k5 = 0.6f;
                            } else {
                                k4 = 1.f;
                                k5 = 1.f;
                            }


                            variC[4] = max(0.0001f, k4 * variC[4]);
                            variC[5] = max(0.0001f, k5 * variC[5]);
                            variCb[4] = max(0.0001f, k4 * variCb[4]);
                            variCb[5] = max(0.0001f, k5 * variCb[5]);

                            if (lp.noisecc < 4.f) {
                                k6 = 0.f;
                            } else if (lp.noisecc < 5.f) {
                                k6 = 0.4f;
                            } else if (lp.noisecc < 6.f) {
                                k6 = 0.7f;
                            } else {
                                k6 = 1.f;
                            }

                            variC[6] = max(0.0001f, k6 * variC[6]);
                            variCb[6] = max(0.0001f, k6 * variCb[6]);

                        }

                        float* noisevarchrom = new float[GH * GW];
                        //noisevarchrom in function chroma
                        int GW2 = (GW + 1) / 2;
                        float nvch = 0.6f;//high value
                        float nvcl = 0.1f;//low value

                        if (lp.noisecf > 100.f) {
                            nvch = 0.8f;
                            nvcl = 0.4f;
                        }

                        float seuil = 4000.f;//low
                        float seuil2 = 15000.f;//high
                        //ac and bc for transition
                        float ac = (nvch - nvcl) / (seuil - seuil2);
                        float bc = nvch - seuil * ac;
#ifdef _OPENMP
                        #pragma omp parallel for

#endif

                        for (int ir = 0; ir < GH; ir++)
                            for (int jr = 0; jr < GW; jr++) {
                                float cN = sqrt(SQR(tmp1.a[ir][jr]) + SQR(tmp1.b[ir][jr]));

                                if (cN < seuil) {
                                    noisevarchrom[(ir >> 1)*GW2 + (jr >> 1)] =  nvch;
                                } else if (cN < seuil2) {
                                    noisevarchrom[(ir >> 1)*GW2 + (jr >> 1)] = ac * cN + bc;
                                } else {
                                    noisevarchrom[(ir >> 1)*GW2 + (jr >> 1)] =  nvcl;
                                }
                            }


                        float noisevarab_r = 100.f; //SQR(lp.noisecc / 10.0);

                        if (lp.noisecc < 0.1f)  {
                            WaveletDenoiseAllAB(Ldecomp, adecomp, noisevarchrom, madL, variC, edge, noisevarab_r, true, false, false, numThreads);
                            WaveletDenoiseAllAB(Ldecomp, bdecomp, noisevarchrom, madL, variCb, edge, noisevarab_r, true, false, false, numThreads);
                        } else {
                            WaveletDenoiseAll_BiShrinkAB(Ldecomp, adecomp, noisevarchrom, madL, variC, edge, noisevarab_r, true, false, false, numThreads);
                            WaveletDenoiseAllAB(Ldecomp, adecomp, noisevarchrom, madL, variC, edge, noisevarab_r, true, false, false, numThreads);

                            WaveletDenoiseAll_BiShrinkAB(Ldecomp, bdecomp, noisevarchrom, madL, variCb, edge, noisevarab_r, true, false, false, numThreads);
                            WaveletDenoiseAllAB(Ldecomp, bdecomp, noisevarchrom, madL, variCb, edge, noisevarab_r, true, false, false, numThreads);
                        }

                        delete[] noisevarchrom;

                    }
                }

                if (!Ldecomp.memoryAllocationFailed) {
                    Lin = new array2D<float>(GW, GH);
#ifdef _OPENMP
                    #pragma omp parallel for

#endif

                    for (int i = 0; i < GH; ++i) {
                        for (int j = 0; j < GW; ++j) {
                            (*Lin)[i][j] = tmp1.L[i][j];
                        }
                    }

                    Ldecomp.reconstruct(tmp1.L[0]);
                }

                if (!Ldecomp.memoryAllocationFailed) {
                    if ((lp.noiself >= 0.1f ||  lp.noiself0 >= 0.1f ||  lp.noiself2 >= 0.1f || lp.noiselc >= 0.1f)  && levred == 7) {
                        fftw_denoise(GW, GH, max_numblox_W, min_numblox_W, tmp1.L, Lin,  numThreads, lp, 0);
                    }
                } 

                if (!adecomp.memoryAllocationFailed) {
                    Ain = new array2D<float>(GW, GH);
#ifdef _OPENMP
                    #pragma omp parallel for

#endif

                    for (int i = 0; i < GH; ++i) {
                        for (int j = 0; j < GW; ++j) {
                            (*Ain)[i][j] = tmp1.a[i][j];
                        }
                    }

                    adecomp.reconstruct(tmp1.a[0]);
                }


                if (!adecomp.memoryAllocationFailed) {
                    if ((lp.noisecf >= 0.1f ||  lp.noisecc >= 0.1f)) {
                        if (lp.noisechrodetail > 1000) { //to avoid all utilisation
                            fftw_denoise(GW, GH, max_numblox_W, min_numblox_W, tmp1.a, Ain,  numThreads, lp, 1);
                        }
                    }




                }


                if (!bdecomp.memoryAllocationFailed) {

                    Bin = new array2D<float>(GW, GH);
#ifdef _OPENMP
                    #pragma omp parallel for

#endif

                    for (int i = 0; i < GH; ++i) {
                        for (int j = 0; j < GW; ++j) {
                            (*Bin)[i][j] = tmp1.b[i][j];
                        }
                    }

                    bdecomp.reconstruct(tmp1.b[0]);
                }


                if (!bdecomp.memoryAllocationFailed) {
                    if ((lp.noisecf >= 0.1f ||  lp.noisecc >= 0.1f)) {
                        if (lp.noisechrodetail > 1000) {//to avoid all utilisation

                            fftw_denoise(GW, GH, max_numblox_W, min_numblox_W, tmp1.b, Bin,  numThreads, lp, 1);
                        }
                    }

                }

                DeNoise_Local(call, lp, levred, huerefblur, lumarefblur, chromarefblur, original, transformed, tmp1, cx, cy, sk);

            } else if (call == 2 /* || call == 1 || call == 3 */) { //simpleprocess

                int bfh = int (lp.ly + lp.lyT) + del; //bfw bfh real size of square zone
                int bfw = int (lp.lx + lp.lxL) + del;
                LabImage bufwv(bfw, bfh);
                bufwv.clear(true);
                array2D<float> *Lin = nullptr;
                //  array2D<float> *Ain = nullptr;
                //  array2D<float> *Bin = nullptr;

                int max_numblox_W = ceil((static_cast<float>(bfw)) / (offset)) + 2 * blkrad;
                // calculate min size of numblox_W.
                int min_numblox_W = ceil((static_cast<float>(bfw)) / (offset)) + 2 * blkrad;
                // these are needed only for creation of the plans and will be freed before entering the parallel loop


                int begy = lp.yc - lp.lyT;
                int begx = lp.xc - lp.lxL;
                int yEn = lp.yc + lp.ly;
                int xEn = lp.xc + lp.lx;

#ifdef _OPENMP
                #pragma omp parallel for schedule(dynamic,16)
#endif

                for (int y = 0; y < transformed->H ; y++) //{
                    for (int x = 0; x < transformed->W; x++) {
                        int lox = cx + x;
                        int loy = cy + y;

                        if (lox >= begx && lox < xEn && loy >= begy && loy < yEn) {
                            bufwv.L[loy - begy][lox - begx] = original->L[y][x];
                            bufwv.a[loy - begy][lox - begx] = original->a[y][x];
                            bufwv.b[loy - begy][lox - begx] = original->b[y][x];
                        }

                    }

                int DaubLen = 6;

                int levwavL = levred;
                int skip = 1;
                wavelet_decomposition Ldecomp(bufwv.L[0], bufwv.W, bufwv.H, levwavL, 1, skip, numThreads, DaubLen);
                wavelet_decomposition adecomp(bufwv.a[0], bufwv.W, bufwv.H, levwavL, 1, skip, numThreads, DaubLen);
                wavelet_decomposition bdecomp(bufwv.b[0], bufwv.W, bufwv.H, levwavL, 1, skip, numThreads, DaubLen);

                float madL[8][3];
                int edge = 2;

                if (!Ldecomp.memoryAllocationFailed) {
                    #pragma omp parallel for collapse(2) schedule(dynamic,1)

                    for (int lvl = 0; lvl < levred; lvl++) {
                        for (int dir = 1; dir < 4; dir++) {
                            int Wlvl_L = Ldecomp.level_W(lvl);
                            int Hlvl_L = Ldecomp.level_H(lvl);

                            float ** WavCoeffs_L = Ldecomp.level_coeffs(lvl);

                            madL[lvl][dir - 1] = SQR(Mad(WavCoeffs_L[dir], Wlvl_L * Hlvl_L));
                        }
                    }

                    float vari[levred];

                    if (levred == 7) {
                        edge = 2;
                        vari[0] = 8.f * SQR((lp.noiself0 / 125.0) * (1.0 + lp.noiself0 / 25.0));
                        vari[1] = 8.f * SQR((lp.noiself / 125.0) * (1.0 + lp.noiself / 25.0));
                        vari[2] = 8.f * SQR((lp.noiself2 / 125.0) * (1.0 + lp.noiself2 / 25.0));

                        vari[3] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                        vari[4] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                        vari[5] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                        vari[6] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                    } else if (levred == 4) {
                        edge = 3;
                        vari[0] = 8.f * SQR((lp.noiself0 / 125.0) * (1.0 + lp.noiself0 / 25.0));
                        vari[1] = 8.f * SQR((lp.noiself / 125.0) * (1.0 + lp.noiself / 25.0));
                        vari[2] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));
                        vari[3] = 8.f * SQR((lp.noiselc / 125.0) * (1.0 + lp.noiselc / 25.0));

                    }


                    if ((lp.noiself >= 0.1f ||  lp.noiself0 >= 0.1f ||  lp.noiself2 >= 0.1f || lp.noiselc >= 0.1f)) {
                        float kr3 = 0.f;
                        float kr4 = 0.f;
                        float kr5 = 0.f;

                        if (lp.noiselc < 30.f) {
                            kr3 = 0.f;
                            kr4 = 0.f;
                            kr5 = 0.f;
                        } else if (lp.noiselc < 50.f) {
                            kr3 = 0.5f;
                            kr4 = 0.3f;
                            kr5 = 0.2f;
                        } else if (lp.noiselc < 70.f) {
                            kr3 = 0.7f;
                            kr4 = 0.5f;
                            kr5 = 0.3f;
                        } else {
                            kr3 = 1.f;
                            kr4 = 1.f;
                            kr5 = 1.f;
                        }

                        vari[0] = max(0.0001f, vari[0]);
                        vari[1] = max(0.0001f, vari[1]);
                        vari[2] = max(0.0001f, vari[2]);
                        vari[3] = max(0.0001f, kr3 * vari[3]);

                        if (levred == 7) {
                            vari[4] = max(0.0001f, kr4 * vari[4]);
                            vari[5] = max(0.0001f, kr5 * vari[5]);
                            vari[6] = max(0.0001f, kr5 * vari[6]);
                        }

                        //    float* noisevarlum = nullptr;  // we need a dummy to pass it to WaveletDenoiseAllL
                        float* noisevarlum = new float[bfh * bfw];
                        int bfw2 = (bfw + 1) / 2;

                        float nvlh[13] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0.7f, 0.5f}; //high value
                        float nvll[13] = {0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.35f, 0.4f, 0.45f, 0.7f, 0.8f, 1.f, 1.f, 1.f}; //low value

                        float seuillow = 3000.f;//low
                        float seuilhigh = 18000.f;//high
                        int i = 10 - lp.noiselequal;
                        float ac = (nvlh[i] - nvll[i]) / (seuillow - seuilhigh);
                        float bc = nvlh[i] - seuillow * ac;
                        //ac and bc for transition
#ifdef _OPENMP
                        #pragma omp parallel for

#endif

                        for (int ir = 0; ir < bfh; ir++)
                            for (int jr = 0; jr < bfw; jr++) {
                                float lN = bufwv.L[ir][jr];

                                if (lN < seuillow) {
                                    noisevarlum[(ir >> 1)*bfw2 + (jr >> 1)] =  nvlh[i];
                                } else if (lN < seuilhigh) {
                                    noisevarlum[(ir >> 1)*bfw2 + (jr >> 1)] = ac * lN + bc;
                                } else {
                                    noisevarlum[(ir >> 1)*bfw2 + (jr >> 1)] =  nvll[i];
                                }
                            }

                        if (lp.noiselc < 1.f) {
                            WaveletDenoiseAllL(Ldecomp, noisevarlum, madL, vari, edge, numThreads);
                        } else {
                            WaveletDenoiseAll_BiShrinkL(Ldecomp, noisevarlum, madL, vari, edge, numThreads);
                            WaveletDenoiseAllL(Ldecomp, noisevarlum, madL, vari, edge, numThreads);
                        }

                        delete [] noisevarlum;

                    }
                }


                float variC[levred];
                float variCb[levred];

                float noisecfr = lp.noisecf;
                float noiseccr = lp.noisecc;

                if (lp.adjch > 0.f) {
                    noisecfr = lp.noisecf * ((100.f + lp.adjch) / 10.f);
                    noiseccr = lp.noisecc + ((100.f + lp.adjch) / 10.f);
                }

                float noisecfb = lp.noisecf;
                float noiseccb = lp.noisecc;

                if (lp.adjch < 0.f) {
                    noisecfb = lp.noisecf * ((100.f - lp.adjch) / 10.f);
                    noiseccb = lp.noisecc * ((100.f - lp.adjch) / 10.f);
                }


                if (noisecfr < 0.f) {
                    noisecfr = 0.0001f;
                }

                if (noiseccr < 0.f) {
                    noiseccr = 0.0001f;
                }

                if (noisecfb < 0.f) {
                    noisecfb = 0.0001f;
                }

                if (noiseccb < 0.f) {
                    noiseccb = 0.0001f;
                }


                if (!adecomp.memoryAllocationFailed && !bdecomp.memoryAllocationFailed) {

                    if (levred == 7) {
                        edge = 2;
                        variC[0] = SQR(noisecfr);
                        variC[1] = SQR(noisecfr);
                        variC[2] = SQR(noisecfr);

                        variC[3] = SQR(noisecfr);
                        variC[4] = SQR(noisecfr);
                        variC[5] = SQR(noiseccr);
                        variC[6] = SQR(noiseccr);

                        variCb[0] = SQR(noisecfb);
                        variCb[1] = SQR(noisecfb);
                        variCb[2] = SQR(noisecfb);

                        variCb[3] = SQR(noisecfb);
                        variCb[4] = SQR(noisecfb);
                        variCb[5] = SQR(noiseccb);
                        variCb[6] = SQR(noiseccb);

                    } else if (levred == 4) {
                        edge = 3;
                        variC[0] = SQR(lp.noisecf / 10.0);
                        variC[1] = SQR(lp.noisecf / 10.0);
                        variC[2] = SQR(lp.noisecf / 10.0);
                        variC[3] = SQR(lp.noisecf / 10.0);

                        variCb[0] = SQR(lp.noisecf / 10.0);
                        variCb[1] = SQR(lp.noisecf / 10.0);
                        variCb[2] = SQR(lp.noisecf / 10.0);
                        variCb[3] = SQR(lp.noisecf / 10.0);


                    }


                    if ((lp.noisecf >= 0.1f ||  lp.noisecc >= 0.1f  || noiscfactiv)) {
                        float minic = 0.0001f;

                        if (noiscfactiv) {
                            minic = 0.1f;//only for artifact shape detection
                        }

                        float k1 = 0.f;
                        float k2 = 0.f;
                        float k3 = 0.f;

                        if (lp.noisecf < 0.2f) {
                            k1 = 0.f;
                            k2 = 0.f;
                            k3 = 0.f;
                        } else if (lp.noisecf < 0.3f) {
                            k1 = 0.1f;
                            k2 = 0.0f;
                            k3 = 0.f;
                        } else if (lp.noisecf < 0.5f) {
                            k1 = 0.2f;
                            k2 = 0.1f;
                            k3 = 0.f;
                        } else if (lp.noisecf < 0.8f) {
                            k1 = 0.3f;
                            k2 = 0.25f;
                            k3 = 0.f;
                        } else if (lp.noisecf < 1.f) {
                            k1 = 0.4f;
                            k2 = 0.25f;
                            k3 = 0.1f;
                        } else if (lp.noisecf < 2.f) {
                            k1 = 0.5f;
                            k2 = 0.3f;
                            k3 = 0.15f;
                        } else if (lp.noisecf < 3.f) {
                            k1 = 0.6f;
                            k2 = 0.45f;
                            k3 = 0.3f;
                        } else if (lp.noisecf < 4.f) {
                            k1 = 0.7f;
                            k2 = 0.5f;
                            k3 = 0.4f;
                        } else if (lp.noisecf < 5.f) {
                            k1 = 0.8f;
                            k2 = 0.6f;
                            k3 = 0.5f;
                        } else if (lp.noisecf < 10.f) {
                            k1 = 0.85f;
                            k2 = 0.7f;
                            k3 = 0.6f;
                        } else if (lp.noisecf < 20.f) {
                            k1 = 0.9f;
                            k2 = 0.8f;
                            k3 = 0.7f;
                        } else if (lp.noisecf < 50.f) {
                            k1 = 1.f;
                            k2 = 1.f;
                            k3 = 0.9f;

                        } else {
                            k1 = 1.f;
                            k2 = 1.f;
                            k3 = 1.f;
                        }

                        variC[0] = max(minic, variC[0]);
                        variC[1] = max(minic, k1 * variC[1]);
                        variC[2] = max(minic, k2 * variC[2]);
                        variC[3] = max(minic, k3 * variC[3]);

                        variCb[0] = max(minic, variCb[0]);
                        variCb[1] = max(minic, k1 * variCb[1]);
                        variCb[2] = max(minic, k2 * variCb[2]);
                        variCb[3] = max(minic, k3 * variCb[3]);

                        if (levred == 7) {
                            float k4 = 0.f;
                            float k5 = 0.f;
                            float k6 = 0.f;

                            if (lp.noisecc == 0.1f) {
                                k4 = 0.f;
                                k5 = 0.0f;
                            } else if (lp.noisecc < 0.2f) {
                                k4 = 0.1f;
                                k5 = 0.0f;
                            } else if (lp.noisecc < 0.5f) {
                                k4 = 0.15f;
                                k5 = 0.0f;
                            } else if (lp.noisecc < 1.f) {
                                k4 = 0.15f;
                                k5 = 0.1f;
                            } else if (lp.noisecc < 3.f) {
                                k4 = 0.3f;
                                k5 = 0.15f;
                            } else if (lp.noisecc < 4.f) {
                                k4 = 0.6f;
                                k5 = 0.4f;
                            } else if (lp.noisecc < 6.f) {
                                k4 = 0.8f;
                                k5 = 0.6f;
                            } else {
                                k4 = 1.f;
                                k5 = 1.f;
                            }


                            variC[4] = max(0.0001f, k4 * variC[4]);
                            variC[5] = max(0.0001f, k5 * variC[5]);
                            variCb[4] = max(0.0001f, k4 * variCb[4]);
                            variCb[5] = max(0.0001f, k5 * variCb[5]);

                            if (lp.noisecc < 4.f) {
                                k6 = 0.f;
                            } else if (lp.noisecc < 5.f) {
                                k6 = 0.4f;
                            } else if (lp.noisecc < 6.f) {
                                k6 = 0.7f;
                            } else {
                                k6 = 1.f;
                            }

                            variC[6] = max(0.0001f, k6 * variC[6]);
                            variCb[6] = max(0.0001f, k6 * variCb[6]);
                        }

                        float* noisevarchrom = new float[bfh * bfw];
                        int bfw2 = (bfw + 1) / 2;
                        float nvch = 0.6f;//high value
                        float nvcl = 0.1f;//low value

                        if (lp.noisecf > 100.f) {
                            nvch = 0.8f;
                            nvcl = 0.4f;
                        }

                        float seuil = 4000.f;//low
                        float seuil2 = 15000.f;//high
                        //ac and bc for transition
                        float ac = (nvch - nvcl) / (seuil - seuil2);
                        float bc = nvch - seuil * ac;
#ifdef _OPENMP
                        #pragma omp parallel for

#endif

                        for (int ir = 0; ir < bfh; ir++)
                            for (int jr = 0; jr < bfw; jr++) {
                                float cN = sqrt(SQR(bufwv.a[ir][jr]) + SQR(bufwv.b[ir][jr]));

                                if (cN < seuil) {
                                    noisevarchrom[(ir >> 1)*bfw2 + (jr >> 1)] = nvch;
                                } else if (cN < seuil2) {
                                    noisevarchrom[(ir >> 1)*bfw2 + (jr >> 1)] = ac * cN + bc;
                                } else {
                                    noisevarchrom[(ir >> 1)*bfw2 + (jr >> 1)] = nvcl;
                                }
                            }

                        float noisevarab_r = 100.f; //SQR(lp.noisecc / 10.0);


                        if (lp.noisecc < 0.1f)  {
                            WaveletDenoiseAllAB(Ldecomp, adecomp, noisevarchrom, madL, variC, edge, noisevarab_r, true, false, false, numThreads);
                            WaveletDenoiseAllAB(Ldecomp, bdecomp, noisevarchrom, madL, variCb, edge, noisevarab_r, true, false, false, numThreads);
                        } else {
                            WaveletDenoiseAll_BiShrinkAB(Ldecomp, adecomp, noisevarchrom, madL, variC, edge, noisevarab_r, true, false, false, numThreads);
                            WaveletDenoiseAllAB(Ldecomp, adecomp, noisevarchrom, madL, variC, edge, noisevarab_r, true, false, false, numThreads);

                            WaveletDenoiseAll_BiShrinkAB(Ldecomp, bdecomp, noisevarchrom, madL, variCb, edge, noisevarab_r, true, false, false, numThreads);
                            WaveletDenoiseAllAB(Ldecomp, bdecomp, noisevarchrom, madL, variCb, edge, noisevarab_r, true, false, false, numThreads);
                        }

                        delete[] noisevarchrom;
                    }
                }

                if (!Ldecomp.memoryAllocationFailed) {
                    Lin = new array2D<float>(bfw, bfh);

#ifdef _OPENMP
                    #pragma omp parallel for

#endif

                    for (int i = 0; i < bfh; ++i) {
                        for (int j = 0; j < bfw; ++j) {
                            (*Lin)[i][j] = bufwv.L[i][j];
                        }
                    }

                    Ldecomp.reconstruct(bufwv.L[0]);
                }


                if (!Ldecomp.memoryAllocationFailed) {


                    if ((lp.noiself >= 0.1f ||  lp.noiself0 >= 0.1f ||  lp.noiself2 >= 0.1f || lp.noiselc >= 0.1f) && levred == 7) {
                        fftw_denoise(bfw, bfh, max_numblox_W, min_numblox_W, bufwv.L, Lin,  numThreads, lp, 0);
                    }
                }


                if (!adecomp.memoryAllocationFailed) {
                    adecomp.reconstruct(bufwv.a[0]);
                }


                if (!bdecomp.memoryAllocationFailed) {
                    bdecomp.reconstruct(bufwv.b[0]);
                }

                DeNoise_Local(call, lp, levred, huerefblur, lumarefblur, chromarefblur, original, transformed, bufwv, cx, cy, sk);
            }

        }


//vibrance

        if (lp.expvib && (lp.past != 0.f  || lp.satur != 0.f)) { //interior ellipse renforced lightness and chroma  //locallutili
            if (call <= 3) { //simpleprocess, dcrop, improccoordinator
                const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
                const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
                const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
                const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);
                const int bfh = yend - ystart;
                const int bfw = xend - xstart;

                if (bfw > 0 && bfh > 0) {
                    JaggedArray<float> buflight(bfw, bfh);
                    JaggedArray<float> bufl_ab(bfw, bfh);
                    std::unique_ptr<LabImage> bufexporig(new LabImage(bfw, bfh)); //buffer for data in zone limit
                    std::unique_ptr<LabImage> bufexpfin(new LabImage(bfw, bfh)); //buffer for data in zone limit

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = ystart; y < yend; y++) {
                        for (int x = xstart; x < xend; x++) {
                            bufexporig->L[y - ystart][x - xstart] = original->L[y][x];
                            bufexporig->a[y - ystart][x - xstart] = original->a[y][x];
                            bufexporig->b[y - ystart][x - xstart] = original->b[y][x];
                        }
                    }

                    VibranceParams vibranceParams;
                    vibranceParams.enabled = params->locallab.spots.at(sp).expvibrance;
                    vibranceParams.pastels = params->locallab.spots.at(sp).pastels;
                    vibranceParams.saturated = params->locallab.spots.at(sp).saturated;
                    vibranceParams.psthreshold = params->locallab.spots.at(sp).psthreshold;
                    vibranceParams.protectskins = params->locallab.spots.at(sp).protectskins;
                    vibranceParams.avoidcolorshift = params->locallab.spots.at(sp).avoidcolorshift;
                    vibranceParams.pastsattog = params->locallab.spots.at(sp).pastsattog;
                    vibranceParams.skintonescurve = params->locallab.spots.at(sp).skintonescurve;

                    bufexpfin->CopyFrom(bufexporig.get());
                    ImProcFunctions::vibrance(bufexpfin.get(), vibranceParams, params->toneCurve.hrenabled, params->icm.workingProfile);

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = 0; y < bfh; y++) {
                        for (int x = 0; x < bfw; x++) {
                            buflight[y][x] = CLIPRET((bufexpfin->L[y][x] - bufexporig->L[y][x]) / 328.f);
                            bufl_ab[y][x] = CLIPRET((sqrt(SQR(bufexpfin->a[y][x]) + SQR(bufexpfin->b[y][x])) - sqrt(SQR(bufexporig->a[y][x]) + SQR(bufexporig->b[y][x]))) / 250.f);
                        }
                    }

                    bufexpfin.reset();
                    transit_shapedetect(2, bufexporig.get(), nullptr, buflight, bufl_ab, nullptr, nullptr, nullptr, false, hueref, chromaref, lumaref, sobelref, 0.f, nullptr, lp, original, transformed, cx, cy, sk);
                }
            }
        }


//Tone mapping

        if (lp.strengt != 0.f  && lp.tonemapena) {
            if (call <= 3) { //simpleprocess dcrop improcc
                const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
                const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
                const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
                const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);
                const int bfh = yend - ystart;
                const int bfw = xend - xstart;

                if (bfw > 0 && bfh > 0) {
                    array2D<float> buflight(bfw, bfh);
                    JaggedArray<float> bufchro(bfw, bfh);
                    std::unique_ptr<LabImage> bufgb(new LabImage(bfw, bfh)); //buffer for data in zone limit
                    std::unique_ptr<LabImage> tmp1(new LabImage(bfw, bfh)); //buffer for data in zone limit

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = ystart; y < yend; y++) {
                        for (int x = xstart; x < xend; x++) {
                            bufgb->L[y - ystart][x - xstart] = original->L[y][x];
                            bufgb->a[y - ystart][x - xstart] = original->a[y][x];
                            bufgb->b[y - ystart][x - xstart] = original->b[y][x];
                        }
                    }

                    ImProcFunctions::EPDToneMaplocal(sp, bufgb.get(), tmp1.get(), 5, sk);
                    float minL = tmp1->L[0][0] - bufgb->L[0][0];
                    float maxL = minL;
                    float minC = sqrt(SQR(tmp1->a[0][0]) + SQR(tmp1->b[0][0])) - sqrt(SQR(bufgb->a[0][0]) + SQR(bufgb->b[0][0]));
                    float maxC = minC;

#ifdef _OPENMP
                    #pragma omp parallel for reduction(max:maxL) reduction(min:minL) schedule(dynamic,16)
#endif
                    for (int ir = 0; ir < bfh; ir++) {
                        for (int jr = 0; jr < bfw; jr++) {
                            buflight[ir][jr] = tmp1->L[ir][jr] - bufgb->L[ir][jr];
                            minL = rtengine::min(minL, buflight[ir][jr]);
                            maxL = rtengine::max(maxL, buflight[ir][jr]);
                            bufchro[ir][jr] = sqrt(SQR(tmp1->a[ir][jr]) + SQR(tmp1->b[ir][jr])) - sqrt(SQR(bufgb->a[ir][jr]) + SQR(bufgb->b[ir][jr]));
                            minC = rtengine::min(minC, bufchro[ir][jr]);
                            maxC = rtengine::max(maxC, bufchro[ir][jr]);
                        }
                    }
                    float coef = 0.01f * (max(fabs(minL), fabs(maxL)));
                    float coefC = 0.01f * (max(fabs(minC), fabs(maxC)));

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = 0; y < bfh; y++) {
                        for (int x = 0; x < bfw; x++) {
                            buflight[y][x] /= coef;
                            bufchro[y][x] /= coefC;
                        }
                    }

                    if (lp.softradiustm > 0.f) {
                        softprocess(bufgb.get(), buflight, lp.softradiustm, bfh, bfw, sk, multiThread);
                    }
                    
                    bufgb.reset();
                    transit_shapedetect(8, tmp1.get(), nullptr, buflight, bufchro, nullptr, nullptr, nullptr, false, hueref, chromaref, lumaref, sobelref, 0.f, nullptr, lp, original, transformed, cx, cy, sk);
                }
            }
        }
//end TM

//begin cbdl
        if ((lp.mulloc[0] != 1.f || lp.mulloc[1] != 1.f || lp.mulloc[2] != 1.f || lp.mulloc[3] != 1.f || lp.mulloc[4] != 1.f  || lp.clarityml != 0.f || lp.contresid != 0.f  || lp.enacbMask || lp.showmaskcbmet == 2 || lp.showmaskcbmet == 3 || lp.showmaskcbmet == 4) && lp.cbdlena) {
            if (call <= 3) { //call from simpleprocess dcrop improcc
                const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
                const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
                const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
                const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);
                int bfh = yend - ystart;
                int bfw = xend - xstart;
                 //   int bfh = int (lp.ly + lp.lyT) + del; //bfw bfh real size of square zone
                 //   int bfw = int (lp.lx + lp.lxL) + del;
                        printf("mascb0=%i \n", lp.showmaskcbmet);

                if (bfw > 32 && bfh > 32) {
                    array2D<float> bufsh(bfw, bfh);
                    array2D<float> &buflight = bufsh;
                    JaggedArray<float> bufchrom(bfw, bfh, true);
                    std::unique_ptr<LabImage> loctemp(new LabImage(bfw, bfh));
                    std::unique_ptr<LabImage> origcbdl(new LabImage(bfw, bfh));
                    std::unique_ptr<LabImage> bufmaskorigcb;
                    std::unique_ptr<LabImage> bufmaskblurcb;
                    std::unique_ptr<LabImage> originalmaskcb;
                    if (lp.showmaskcbmet == 2  || lp.enacbMask || lp.showmaskcbmet == 3 || lp.showmaskcbmet == 4) {
                        bufmaskorigcb.reset(new LabImage(bfw, bfh));
                        bufmaskblurcb.reset(new LabImage(bfw, bfh));
                        originalmaskcb.reset(new LabImage(bfw, bfh));
                    }

                    array2D<float> ble(bfw, bfh);
                    array2D<float> guid(bfw, bfh);
                    float meanfab, fab;

                    mean_fab(xstart, ystart, bfw, bfh, loctemp.get(), original, fab, meanfab, lp.chromaSH);

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = 0; y < bfh; y++) {
                        for (int x = 0; x < bfw; x++) {
                            loctemp->L[y][x] = original->L[y + ystart][x + xstart];
                        }
                    }

                    if (lp.showmaskcbmet == 2  || lp.enacbMask || lp.showmaskcbmet == 3 || lp.showmaskcbmet == 4) {

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++) {
                            for (int jr = 0; jr < bfw; jr++) {
                                float kmaskLexp = 0;
                                float kmaskCH = 0;

                                if (locllmascbCurve  && llmascbutili) {
                                    float ligh = loctemp->L[ir][jr] / 32768.f;
                                    kmaskLexp = 32768.f * LIM01(1.f - locllmascbCurve[500.f * ligh]);
                                }

                                if (lp.showmaskcbmet != 4) {
                                    if (locccmascbCurve && lcmascbutili) {
                                        float chromask = 0.0001f + sqrt(SQR((loctemp->a[ir][jr]) / fab) + SQR((loctemp->b[ir][jr]) / fab));
                                        kmaskCH = LIM01(1.f - locccmascbCurve[500.f *  chromask]);
                                    }
                                }

                                if (lochhmascbCurve && lhmascbutili) {
                                    float huema = xatan2f(loctemp->b[ir][jr], loctemp->a[ir][jr]);
                                    float h = Color::huelab_to_huehsv2(huema);
                                    h += 1.f / 6.f;

                                    if (h > 1.f) {
                                        h -= 1.f;
                                    }

                                    float valHH = LIM01(1.f - lochhmascbCurve[500.f *  h]);

                                    if (lp.showmaskcbmet != 4) {
                                        kmaskCH += valHH;
                                    }

                                    kmaskLexp += 32768.f * valHH;
                                }

                                bufmaskblurcb->L[ir][jr] = CLIPLOC(kmaskLexp);
                                bufmaskblurcb->a[ir][jr] = kmaskCH;
                                bufmaskblurcb->b[ir][jr] = kmaskCH;
                                ble[ir][jr] = bufmaskblurcb->L[ir][jr] / 32768.f;
                                guid[ir][jr] = loctemp->L[ir][jr] / 32768.f;

                            }
                        }

                        if (lp.radmacb > 0.f) {
                            guidedFilter(guid, ble, ble, lp.radmacb * 10.f / sk, 0.001, multiThread, 4);
                        }

                        LUTf lutTonemaskcb(65536);
                        calcGammaLut(lp.gammacb, lp.slomacb, lutTonemaskcb);

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++)
                            for (int jr = 0; jr < bfw; jr++) {
                                float L_;
                                bufmaskblurcb->L[ir][jr] = LIM01(ble[ir][jr]) * 32768.f;
                                L_ = 2.f * bufmaskblurcb->L[ir][jr];
                                bufmaskblurcb->L[ir][jr] = lutTonemaskcb[L_];
                            }

                    }

                    float radiusb = 1.f / sk;

                    if (lp.showmaskcbmet == 2 || lp.enacbMask || lp.showmaskcbmet == 3 || lp.showmaskcbmet == 4) {

#ifdef _OPENMP
                        #pragma omp parallel
#endif
                        {
                            gaussianBlur(bufmaskblurcb->L, bufmaskorigcb->L, bfw, bfh, radiusb);
                            gaussianBlur(bufmaskblurcb->a, bufmaskorigcb->a, bfw, bfh, 1.f + (0.5f * lp.radmacb) / sk);
                            gaussianBlur(bufmaskblurcb->b, bufmaskorigcb->b, bfw, bfh, 1.f + (0.5f * lp.radmacb) / sk);
                        }

                        if (lp.showmaskcbmet == 0 || lp.showmaskcbmet == 1 || lp.showmaskcbmet == 2 || lp.showmaskcbmet == 4 || lp.enacbMask) {

                            blendmask(lp, xstart, ystart, cx, cy, bfw, bfh, loctemp.get(), original, bufmaskorigcb.get(), originalmaskcb.get(), lp.blendmacb);

                        } else if (lp.showmaskcbmet == 3) {
                            showmask(lp, xstart, ystart, cx, cy, bfw, bfh, loctemp.get(), transformed, bufmaskorigcb.get());
                            return;
                        }
                    }

                    constexpr float b_l = -5.f;
                    constexpr float t_l = 25.f;
                    constexpr float t_r = 120.f;
                    constexpr float b_r = 170.f;
                    constexpr double skinprot = 0.;
                    constexpr int choice = 0;
                    if (lp.showmaskcbmet == 0 || lp.showmaskcbmet == 1  || lp.showmaskcbmet == 2 || lp.showmaskcbmet == 4 || lp.enacbMask) {

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = ystart; y < yend; y++) {
                        for (int x = xstart; x < xend; x++) {
                            bufsh[y - ystart][x - xstart] = origcbdl->L[y - ystart][x - xstart] = original->L[y][x];
                            loctemp->a[y - ystart][x - xstart] = origcbdl->a[y - ystart][x - xstart] = original->a[y][x];
                            loctemp->b[y - ystart][x - xstart] = origcbdl->b[y - ystart][x - xstart] = original->b[y][x];
                        }
                    }

                    if(lp.clarityml != 0.f && lp.mulloc[4] == 1.0) {//enabled last level to retrieve level 5 and residual image in case user not select level 5
                        lp.mulloc[4]= 1.001f;
                    }

                    if(lp.contresid != 0.f && lp.mulloc[4] == 1.0) {//enabled last level to retrieve level 5 and residual image in case user not select level 5
                        lp.mulloc[4]= 1.001f;
                    }

                    ImProcFunctions::cbdl_local_temp(bufsh, loctemp->L, bfw, bfh, lp.mulloc, 1.f, lp.threshol, lp.clarityml, lp.contresid, lp.blurcbdl, skinprot, false, b_l, t_l, t_r, b_r, choice, sk, multiThread);

                    if (lp.softradiuscb > 0.f) {
                        array2D<float> ble(bfw, bfh);
                        array2D<float> guid(bfw, bfh);
#ifdef _OPENMP
        #pragma omp parallel for
#endif
                        for (int ir = 0; ir < bfh; ir++)
                            for (int jr = 0; jr < bfw; jr++) {
                                ble[ir][jr] = (loctemp->L[ir][jr]  - origcbdl->L[ir][jr]) / 32768.f;
                                guid[ir][jr] = origcbdl->L[ir][jr] / 32768.f;
                            }

                        rtengine::guidedFilter(guid, ble, ble, (lp.softradiuscb * 2.f / sk), 0.001, multiThread);
#ifdef _OPENMP
        #pragma omp parallel for
#endif
                        for (int ir = 0; ir < bfh; ir++)
                            for (int jr = 0; jr < bfw; jr++) {
                                loctemp->L[ir][jr] =  origcbdl->L[ir][jr] + 32768.f * ble[ir][jr];
                            }
                    }

                }
                    transit_shapedetect(6, loctemp.get(), originalmaskcb.get(), buflight, bufchrom, nullptr, nullptr, nullptr, false, hueref, chromaref, lumaref, sobelref, 0.f, nullptr, lp, original, transformed, cx, cy, sk);

                    //chroma CBDL begin here
                    if (lp.chromacb > 0.f) {
#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++) {
                            for (int jr = 0; jr < bfw; jr++) {
                                bufsh[ir][jr] = sqrt(SQR(loctemp->a[ir][jr]) + SQR(loctemp->b[ir][jr]));
                            }
                        }

                        float multc[5];
                        float clarich = 0.5f * lp.clarityml;

                        if(clarich > 0.f && lp.mulloc[0] == 1.f) { //to enabled in case of user select only clarity
                            lp.mulloc[0] = 1.01f;
                        }

                        if(lp.contresid != 0.f && lp.mulloc[0] == 1.f) { //to enabled in case of user select only clarity
                            lp.mulloc[0] = 1.01f;
                        }

                        for (int lv = 0; lv < 5; lv++) {
                            multc[lv] = rtengine::max((lp.chromacb * ((float) lp.mulloc[lv] - 1.f) / 100.f) + 1.f, 0.f);
                        }
                        
                        ImProcFunctions::cbdl_local_temp(bufsh, loctemp->L, bfw, bfh, multc, rtengine::max(lp.chromacb, 1.f), lp.threshol, clarich, 0.f, lp.blurcbdl, skinprot, false,  b_l, t_l, t_r, b_r, choice, sk, multiThread);


                        float minC = loctemp->L[0][0] - sqrt(SQR(loctemp->a[0][0]) + SQR(loctemp->b[0][0]));
                        float maxC = minC;
#ifdef _OPENMP
                        #pragma omp parallel for reduction(max:maxC) reduction(min:minC) schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++) {
                            for (int jr = 0; jr < bfw; jr++) {
                                bufchrom[ir][jr] = (loctemp->L[ir][jr] - sqrt(SQR(loctemp->a[ir][jr]) + SQR(loctemp->b[ir][jr])));
                                minC = rtengine::min(minC, bufchrom[ir][jr]);
                                maxC = rtengine::max(maxC, bufchrom[ir][jr]);
                            }
                        }

                        float coefC = 0.01f * (max(fabs(minC), fabs(maxC)));




#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++) {
                            for (int jr = 0; jr < bfw; jr++) {
                                bufchrom[ir][jr] /= coefC;
                            }
                        }

                        transit_shapedetect(7, loctemp.get(), nullptr, buflight, bufchrom, nullptr, nullptr, nullptr, false, hueref, chromaref, lumaref, sobelref, 0.f, nullptr, lp, original, transformed, cx, cy, sk);
                    }
                }
            }
        }


//end cbdl_Local

//shadow highlight

        if (! lp.invsh && (lp.highlihs > 0.f || lp.shadowhs > 0.f || lp.showmaskSHmet == 2 || lp.enaSHMask || lp.showmaskSHmet == 3 || lp.showmaskSHmet == 4) && call < 3  && lp.hsena) {
            const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
            const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
            const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
            const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);
            const int bfh = yend - ystart;
            const int bfw = xend - xstart;

            if (bfw > 0 && bfh > 0) {

                std::unique_ptr<LabImage> bufexporig(new LabImage(bfw, bfh));
                std::unique_ptr<LabImage> bufexpfin(new LabImage(bfw, bfh));
                std::unique_ptr<LabImage> bufmaskorigSH;
                std::unique_ptr<LabImage> bufmaskblurSH;
                std::unique_ptr<LabImage> originalmaskSH;

                JaggedArray<float> buflight(bfw, bfh);
                JaggedArray<float> bufl_ab(bfw, bfh);

                if (call <= 3) { //simpleprocess, dcrop, improccoordinator
                    if (lp.showmaskSHmet == 2  || lp.enaSHMask || lp.showmaskSHmet == 3 || lp.showmaskSHmet == 4) {
                        bufmaskorigSH.reset(new LabImage(bfw, bfh));
                        bufmaskblurSH.reset(new LabImage(bfw, bfh));
                        originalmaskSH.reset(new LabImage(bfw, bfh));
                    }

                    array2D<float> ble(bfw, bfh);
                    array2D<float> guid(bfw, bfh);
                    float meanfab, fab;

                    mean_fab(xstart, ystart, bfw, bfh, bufexporig.get(), original, fab, meanfab, lp.chromaSH);

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = 0; y < bfh; y++) {
                        for (int x = 0; x < bfw; x++) {
                            bufexporig->L[y][x] = original->L[y + ystart][x + xstart];
                        }
                    }

                    if (lp.showmaskSHmet == 2  || lp.enaSHMask || lp.showmaskSHmet == 3 || lp.showmaskSHmet == 4) {
#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++) {
                            for (int jr = 0; jr < bfw; jr++) {
                                float kmaskLexp = 0;
                                float kmaskCH = 0;

                                if (locllmasSHCurve  && llmasSHutili) {
                                    float ligh = bufexporig->L[ir][jr] / 32768.f;
                                    kmaskLexp = 32768.f * LIM01(1.f - locllmasSHCurve[500.f * ligh]);
                                }

                                if (lp.showmaskSHmet != 4) {
                                    if (locccmasSHCurve && lcmasSHutili) {
                                        float chromask = 0.0001f + sqrt(SQR((bufexporig->a[ir][jr]) / fab) + SQR((bufexporig->b[ir][jr]) / fab));
                                        kmaskCH = LIM01(1.f - locccmasSHCurve[500.f *  chromask]);
                                    }
                                }

                                if (lochhmasSHCurve && lhmasSHutili) {
                                    float huema = xatan2f(bufexporig->b[ir][jr], bufexporig->a[ir][jr]);
                                    float h = Color::huelab_to_huehsv2(huema);
                                    h += 1.f / 6.f;

                                    if (h > 1.f) {
                                        h -= 1.f;
                                    }

                                    float valHH = LIM01(1.f - lochhmasSHCurve[500.f *  h]);

                                    if (lp.showmaskSHmet != 4) {
                                        kmaskCH += valHH;
                                    }

                                    kmaskLexp += 32768.f * valHH;
                                }

                                bufmaskblurSH->L[ir][jr] = CLIPLOC(kmaskLexp);
                                bufmaskblurSH->a[ir][jr] = kmaskCH;
                                bufmaskblurSH->b[ir][jr] = kmaskCH;
                                ble[ir][jr] = bufmaskblurSH->L[ir][jr] / 32768.f;
                                guid[ir][jr] = bufexporig->L[ir][jr] / 32768.f;

                            }
                        }

                        if (lp.radmaSH > 0.f) {
                            guidedFilter(guid, ble, ble, lp.radmaSH * 10.f / sk, 0.001, multiThread, 4);
                        }

                        LUTf lutTonemaskSH(65536);
                        calcGammaLut(lp.gammaSH, lp.slomaSH, lutTonemaskSH);

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++)
                            for (int jr = 0; jr < bfw; jr++) {
                                float L_;
                                bufmaskblurSH->L[ir][jr] = LIM01(ble[ir][jr]) * 32768.f;
                                L_ = 2.f * bufmaskblurSH->L[ir][jr];
                                bufmaskblurSH->L[ir][jr] = lutTonemaskSH[L_];
                            }

                    }

                    float radiusb = 1.f / sk;

                    if (lp.showmaskSHmet == 2 || lp.enaSHMask || lp.showmaskSHmet == 3 || lp.showmaskSHmet == 4) {


#ifdef _OPENMP
                        #pragma omp parallel
#endif
                        {
                            gaussianBlur(bufmaskblurSH->L, bufmaskorigSH->L, bfw, bfh, radiusb);
                            gaussianBlur(bufmaskblurSH->a, bufmaskorigSH->a, bfw, bfh, 1.f + (0.5f * lp.radmaSH) / sk);
                            gaussianBlur(bufmaskblurSH->b, bufmaskorigSH->b, bfw, bfh, 1.f + (0.5f * lp.radmaSH) / sk);
                        }

                        if (lp.showmaskSHmet == 0 || lp.showmaskSHmet == 1 || lp.showmaskSHmet == 2 || lp.showmaskSHmet == 4 || lp.enaSHMask) {
                            blendmask(lp, xstart, ystart, cx, cy, bfw, bfh, bufexporig.get(), original, bufmaskorigSH.get(), originalmaskSH.get(), lp.blendmaSH);

                        } else if (lp.showmaskSHmet == 3) {
                            showmask(lp, xstart, ystart, cx, cy, bfw, bfh, bufexporig.get(), transformed, bufmaskorigSH.get());
                            return;
                        }
                    }


                    if (lp.showmaskSHmet == 0 || lp.showmaskSHmet == 1  || lp.showmaskSHmet == 2 || lp.showmaskSHmet == 4 || lp.enaSHMask) {

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int y = 0; y < bfh ; y++) {
                            for (int x = 0; x < bfw; x++) {
                                bufexporig->L[y][x] = original->L[y + ystart][x + xstart];
                                bufexporig->a[y][x] = original->a[y + ystart][x + xstart];
                                bufexporig->b[y][x] = original->b[y + ystart][x + xstart];
                                bufexpfin->L[y][x] = original->L[y + ystart][x + xstart];
                                bufexpfin->a[y][x] = original->a[y + ystart][x + xstart];
                                bufexpfin->b[y][x] = original->b[y + ystart][x + xstart];
                            }
                        }

                        ImProcFunctions::shadowsHighlights(bufexpfin.get(), lp.hsena, 1, lp.highlihs, lp.shadowhs, lp.radiushs, sk, lp.hltonalhs, lp.shtonalhs);

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++) {
                            for (int jr = 0; jr < bfw; jr++) {
                                buflight[ir][jr] = CLIPRET((bufexpfin->L[ir][jr] - bufexporig->L[ir][jr]) / 328.f);
                                bufl_ab[ir][jr] = CLIPRET((sqrt(SQR(bufexpfin->a[ir][jr]) + SQR(bufexpfin->b[ir][jr])) - sqrt(SQR(bufexporig->a[ir][jr]) + SQR(bufexporig->b[ir][jr]))) / 250.f);
                            }
                        }
                    }

                    transit_shapedetect(9, bufexpfin.get(), originalmaskSH.get(), buflight, bufl_ab, nullptr, nullptr, nullptr, false, hueref, chromaref, lumaref, sobelref, 0.f,  nullptr, lp, original, transformed, cx, cy, sk);
                }
            }
        } else  if (lp.invsh && (lp.highlihs > 0.f || lp.shadowhs > 0.f) && call < 3  && lp.hsena) {

            float adjustr = 2.f;
            InverseColorLight_Local(sp, 2, lp, lightCurveloc, hltonecurveloc, shtonecurveloc, tonecurveloc, exlocalcurve, cclocalcurve, adjustr, localcutili, lllocalcurve, locallutili, original, transformed, cx, cy, hueref, chromaref, lumaref, sk);
        }





// soft light
        if (lp.strng > 0.f && call <= 3 && lp.sfena) {
            const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
            const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
            const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
            const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);
            const int bfh = yend - ystart;
            const int bfw = xend - xstart;

            if (bfw > 0 && bfh > 0) {
                std::unique_ptr<LabImage> bufexporig(new LabImage(bfw, bfh)); //buffer for data in zone limit
                std::unique_ptr<LabImage> bufexpfin(new LabImage(bfw, bfh)); //buffer for data in zone limit
                JaggedArray<float> buflight(bfw, bfh);
                JaggedArray<float> bufl_ab(bfw, bfh);

#ifdef _OPENMP
                #pragma omp parallel for schedule(dynamic,16)
#endif

                for (int y = ystart; y < yend; y++) {
                    for (int x = xstart; x < xend; x++) {
                        bufexporig->L[y - ystart][x - xstart] = original->L[y][x];
                        bufexporig->a[y - ystart][x - xstart] = original->a[y][x];
                        bufexporig->b[y - ystart][x - xstart] = original->b[y][x];
                    }
                }
                bufexpfin->CopyFrom(bufexporig.get());
                SoftLightParams softLightParams;
                softLightParams.enabled = true;
                softLightParams.strength = lp.strng;
                ImProcFunctions::softLight(bufexpfin.get(), softLightParams);

#ifdef _OPENMP
                #pragma omp parallel for schedule(dynamic,16)
#endif

                for (int y = 0; y < bfh; y++) {
                    for (int x = 0; x < bfw; x++) {
                        buflight[y][x] = CLIPRET((bufexpfin->L[y][x] - bufexporig->L[y][x]) / 328.f);
                        bufl_ab[y][x] = CLIPRET((sqrt(SQR(bufexpfin->a[y][x]) + SQR(bufexpfin->b[y][x])) - sqrt(SQR(bufexporig->a[y][x]) + SQR(bufexporig->b[y][x]))) / 250.f);
                    }
                }

                bufexpfin.reset();
                transit_shapedetect(3, bufexporig.get(), nullptr, buflight, bufl_ab, nullptr, nullptr, nullptr, false, hueref, chromaref, lumaref, sobelref, 0.f,  nullptr, lp, original, transformed, cx, cy, sk);
            }
        }


//local contrast
        if (lp.lcamount > 0.f && call < 3  && lp.lcena) { //interior ellipse for sharpening, call = 1 and 2 only with Dcrop and simpleprocess
            int bfh = call == 2 ? int (lp.ly + lp.lyT) + del : original->H; //bfw bfh real size of square zone
            int bfw = call == 2 ? int (lp.lx + lp.lxL) + del : original->W;
            JaggedArray<float> loctemp(bfw, bfh);
            std::unique_ptr<LabImage> bufloca;

            LabImage *localContrastSource;
            if (call == 2) { //call from simpleprocess
                bufloca.reset(new LabImage(bfw, bfh));

                //  JaggedArray<float> hbuffer(bfw, bfh);
                int begy = lp.yc - lp.lyT;
                int begx = lp.xc - lp.lxL;
                int yEn = lp.yc + lp.ly;
                int xEn = lp.xc + lp.lx;

#ifdef _OPENMP
                #pragma omp parallel for schedule(dynamic,16)
#endif

                for (int y = 0; y < transformed->H ; y++) {
                    const int loy = cy + y;
                    if (loy >= begy && loy < yEn) {
                        for (int x = 0; x < transformed->W; x++) {
                            const int lox = cx + x;
                            if (lox >= begx && lox < xEn) {
                                bufloca->L[loy - begy][lox - begx] = original->L[y][x];
                            }
                        }
                    }
                }

                localContrastSource = bufloca.get();
            } else { //call from dcrop.cc
                localContrastSource = original;
            }
            LocalContrastParams localContrastParams;
            localContrastParams.enabled = true;
            localContrastParams.radius = params->locallab.spots.at(sp).lcradius;
            localContrastParams.amount = params->locallab.spots.at(sp).lcamount;
            localContrastParams.darkness = params->locallab.spots.at(sp).lcdarkness;
            localContrastParams.lightness = params->locallab.spots.at(sp).lightness;
            ImProcFunctions::localContrast(localContrastSource, loctemp, localContrastParams, sk);

            //sharpen ellipse and transition
            Sharp_Local(call, loctemp, 1,  hueref, chromaref, lumaref, lp, original, transformed, cx, cy, sk);
        }


        if (!lp.invshar && lp.shrad > 0.42 && call < 3 && lp.sharpena && sk == 1) { //interior ellipse for sharpening, call = 1 and 2 only with Dcrop and simpleprocess
            int bfh = call == 2 ? int (lp.ly + lp.lyT) + del : original->H; //bfw bfh real size of square zone
            int bfw = call == 2 ? int (lp.lx + lp.lxL) + del : original->W;
            JaggedArray<float> loctemp(bfw, bfh);

            if (call == 2) { //call from simpleprocess
                JaggedArray<float> bufsh(bfw, bfh, true);
                JaggedArray<float> hbuffer(bfw, bfh);
                int begy = lp.yc - lp.lyT;
                int begx = lp.xc - lp.lxL;
                int yEn = lp.yc + lp.ly;
                int xEn = lp.xc + lp.lx;

#ifdef _OPENMP
                #pragma omp parallel for schedule(dynamic,16)
#endif

                for (int y = 0; y < transformed->H ; y++) {
                    for (int x = 0; x < transformed->W; x++) {
                        int lox = cx + x;
                        int loy = cy + y;

                        if (lox >= begx && lox < xEn && loy >= begy && loy < yEn) {
                            bufsh[loy - begy][lox - begx] = original->L[y][x];
                        }
                    }
                }

                //sharpen only square area instaed of all image
                ImProcFunctions::deconvsharpeningloc(bufsh, hbuffer, bfw, bfh, loctemp, params->locallab.spots.at(sp).shardamping, (double)params->locallab.spots.at(sp).sharradius, params->locallab.spots.at(sp).shariter, params->locallab.spots.at(sp).sharamount, params->locallab.spots.at(sp).sharcontrast, (double)params->locallab.spots.at(sp).sharblur);
            } else { //call from dcrop.cc
                ImProcFunctions::deconvsharpeningloc(original->L, shbuffer, bfw, bfh, loctemp, params->locallab.spots.at(sp).shardamping, (double)params->locallab.spots.at(sp).sharradius, params->locallab.spots.at(sp).shariter, params->locallab.spots.at(sp).sharamount, params->locallab.spots.at(sp).sharcontrast, (double)params->locallab.spots.at(sp).sharblur);
            }

            //sharpen ellipse and transition
            Sharp_Local(call, loctemp, 0, hueref, chromaref, lumaref, lp, original, transformed, cx, cy, sk);

        } else if (lp.invshar && lp.shrad > 0.42 && call < 3 && lp.sharpena && sk == 1) {
            int GW = original->W;
            int GH = original->H;
            JaggedArray<float> loctemp(GW, GH);

            ImProcFunctions::deconvsharpeningloc(original->L, shbuffer, GW, GH, loctemp, params->locallab.spots.at(sp).shardamping, (double)params->locallab.spots.at(sp).sharradius, params->locallab.spots.at(sp).shariter, params->locallab.spots.at(sp).sharamount, params->locallab.spots.at(sp).sharcontrast, (double)params->locallab.spots.at(sp).sharblur);


            InverseSharp_Local(loctemp, hueref, lumaref, chromaref, lp, original, transformed, cx, cy, sk);
        }

        //      }
//&& lp.retiena
        if (lp.str > 0.f  && lp.retiena) {
            int GW = transformed->W;
            int GH = transformed->H;

            LabImage *bufreti = nullptr;
            int bfh = int (lp.ly + lp.lyT) + del; //bfw bfh real size of square zone
            int bfw = int (lp.lx + lp.lxL) + del;
            array2D<float> buflight(bfw, bfh);
            JaggedArray<float> bufchro(bfw, bfh);

            int Hd, Wd;
            Hd = GH;
            Wd = GW;

            if (!lp.invret && call <= 3) {

                Hd = bfh;
                Wd = bfw;
                bufreti = new LabImage(bfw, bfh);

#ifdef _OPENMP
                #pragma omp parallel for
#endif

                for (int ir = 0; ir < bfh; ir++) //fill with 0
                    for (int jr = 0; jr < bfw; jr++) {
                        bufreti->L[ir][jr] = 0.f;
                        bufreti->a[ir][jr] = 0.f;
                        bufreti->b[ir][jr] = 0.f;
                        buflight[ir][jr] = 0.f;
                        bufchro[ir][jr] = 0.f;
                    }

                int begy = lp.yc - lp.lyT;
                int begx = lp.xc - lp.lxL;
                int yEn = lp.yc + lp.ly;
                int xEn = lp.xc + lp.lx;

#ifdef _OPENMP
                #pragma omp parallel for schedule(dynamic,16)
#endif

                for (int y = 0; y < transformed->H ; y++) //{
                    for (int x = 0; x < transformed->W; x++) {
                        int lox = cx + x;
                        int loy = cy + y;

                        if (lox >= begx && lox < xEn && loy >= begy && loy < yEn) {
                            bufreti->L[loy - begy][lox - begx] = original->L[y][x];
                            bufreti->a[loy - begy][lox - begx] = original->a[y][x];
                            bufreti->b[loy - begy][lox - begx] = original->b[y][x];
                        }
                    }

                //calc dehaze
                Imagefloat *tmpImage = nullptr;

                if (lp.dehaze > 0) {
                    const float depthcombi = 0.3f * params->locallab.spots.at(sp).neigh + 0.15f * (500.f - params->locallab.spots.at(sp).vart);
                    DehazeParams dehazeParams;
                    dehazeParams.enabled = true;
                    dehazeParams.strength = 0.9f * lp.dehaze + 0.3f * lp.str;
                    dehazeParams.showDepthMap = false;
                    dehazeParams.depth = LIM(depthcombi, 0.f, 100.f);

                    tmpImage = new Imagefloat(bfw, bfh);
                    lab2rgb(*bufreti, *tmpImage, params->icm.workingProfile);
                    dehaze(tmpImage, dehazeParams);
                    rgb2lab(*tmpImage, *bufreti, params->icm.workingProfile);

                    delete tmpImage;
                }
            }

            float *orig[Hd] ALIGNED16;
            float *origBuffer = new float[Hd * Wd];

            for (int i = 0; i < Hd; i++) {
                orig[i] = &origBuffer[i * Wd];
            }

            float *orig1[Hd] ALIGNED16;
            float *origBuffer1 = new float[Hd * Wd];

            for (int i = 0; i < Hd; i++) {
                orig1[i] = &origBuffer1[i * Wd];
            }


            LabImage *tmpl = nullptr;

            if (!lp.invret && call <= 3) {


#ifdef _OPENMP
                #pragma omp parallel for schedule(dynamic,16)
#endif

                for (int ir = 0; ir < Hd; ir += 1)
                    for (int jr = 0; jr < Wd; jr += 1) {
                        orig[ir][jr] = bufreti->L[ir][jr];
                        orig1[ir][jr] = bufreti->L[ir][jr];
                    }

                tmpl = new LabImage(Wd, Hd);

            }  else {

                Imagefloat *tmpImage = nullptr;
                bufreti = new LabImage(Wd, Hd);

                if (lp.dehaze > 0) {
                    const float depthcombi = 0.3f * params->locallab.spots.at(sp).neigh + 0.15f * (500.f - params->locallab.spots.at(sp).vart);
                    DehazeParams dehazeParams;
                    dehazeParams.enabled = true;
                    dehazeParams.strength = 0.9f * lp.dehaze + 0.3f * lp.str;
                    dehazeParams.showDepthMap = false;
                    dehazeParams.depth = LIM(depthcombi, 0.f, 100.f);

                    tmpImage = new Imagefloat(Wd, Hd);
                    lab2rgb(*original, *tmpImage, params->icm.workingProfile);
                    dehaze(tmpImage, dehazeParams);
                    rgb2lab(*tmpImage, *bufreti, params->icm.workingProfile);

                    delete tmpImage;
#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int ir = 0; ir < Hd; ir += 1) {
                        for (int jr = 0; jr < Wd; jr += 1) {
                            orig[ir][jr] = original->L[ir][jr];
                            orig1[ir][jr] = bufreti->L[ir][jr];
                        }
                    }

                    delete bufreti;
                    bufreti = nullptr;
                } else {

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int ir = 0; ir < Hd; ir += 1) {
                        for (int jr = 0; jr < Wd; jr += 1) {
                            orig[ir][jr] = original->L[ir][jr];
                            orig1[ir][jr] = transformed->L[ir][jr];
                        }
                    }
                }
                tmpl = new LabImage(transformed->W, transformed->H);
            }

            float minCD, maxCD, mini, maxi, Tmean, Tsigma, Tmin, Tmax;
            ImProcFunctions::MSRLocal(sp, orig, tmpl->L, orig1, Wd, Hd, params->locallab, sk, locRETgainCcurve, 0, 4, 0.8f, minCD, maxCD, mini, maxi, Tmean, Tsigma, Tmin, Tmax);
#ifdef _OPENMP
            #pragma omp parallel for
#endif

            for (int ir = 0; ir < Hd; ir += 1)
                for (int jr = 0; jr < Wd; jr += 1) {
                    tmpl->L[ir][jr] = orig[ir][jr];
                }

            if (!lp.invret) {
                float minL = tmpl->L[0][0] - bufreti->L[0][0];
                float maxL = minL;
#ifdef _OPENMP
                #pragma omp parallel for reduction(min:minL) reduction(max:maxL) schedule(dynamic,16)
#endif

                for (int ir = 0; ir < Hd; ir++) {
                    for (int jr = 0; jr < Wd; jr++) {
                        buflight[ir][jr] = tmpl->L[ir][jr] - bufreti->L[ir][jr];
                        minL = rtengine::min(minL, buflight[ir][jr]);
                        maxL = rtengine::max(maxL, buflight[ir][jr]);
                    }
                }

                float coef = 0.01f * (max(fabs(minL), fabs(maxL)));

                //printf("minL=%f maxL=%f coef=%f\n",  minL, maxL, coef);

                for (int ir = 0; ir < Hd; ir++) {
                    for (int jr = 0; jr < Wd; jr++) {
                        buflight[ir][jr] /= coef;
                    }
                }

                if (lp.softradiusret > 0.f) {
                    softprocess(bufreti, buflight, lp.softradiusret, Hd, Wd, sk, multiThread);
                }

                transit_shapedetect_retinex(4, bufreti, buflight, bufchro, hueref, chromaref, lumaref, lp, original, transformed, cx, cy, sk);
            } else {
                InverseReti_Local(lp, hueref, chromaref, lumaref, original, transformed, tmpl, cx, cy, 0, sk);
            }

            if (params->locallab.spots.at(sp).chrrt > 0) {

                if (!lp.invret && call <= 3) {

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int ir = 0; ir < Hd; ir += 1)
                        for (int jr = 0; jr < Wd; jr += 1) {

                            orig[ir][jr] = sqrt(SQR(bufreti->a[ir][jr]) + SQR(bufreti->b[ir][jr]));
                            orig1[ir][jr] = sqrt(SQR(bufreti->a[ir][jr]) + SQR(bufreti->b[ir][jr]));
                        }

                }  else {

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int ir = 0; ir < GH; ir += 1)
                        for (int jr = 0; jr < GW; jr += 1) {
                            orig[ir][jr] = sqrt(SQR(original->a[ir][jr]) + SQR(original->b[ir][jr]));
                            orig1[ir][jr] = sqrt(SQR(transformed->a[ir][jr]) + SQR(transformed->b[ir][jr]));
                        }
                }

                ImProcFunctions::MSRLocal(sp, orig, tmpl->L, orig1, Wd, Hd, params->locallab, sk, locRETgainCcurve, 1, 4, 0.8f, minCD, maxCD, mini, maxi, Tmean, Tsigma, Tmin, Tmax);

                if (!lp.invret && call <= 3) {

#ifdef _OPENMP
                    #pragma omp parallel for
#endif

                    for (int ir = 0; ir < Hd; ir += 1)
                        for (int jr = 0; jr < Wd; jr += 1) {
                            const float Chprov = orig1[ir][jr];
                            float2 sincosval;
                            sincosval.y = Chprov == 0.0f ? 1.f : bufreti->a[ir][jr] / Chprov;
                            sincosval.x = Chprov == 0.0f ? 0.f : bufreti->b[ir][jr] / Chprov;
                            tmpl->a[ir][jr] = orig[ir][jr] * sincosval.y;
                            tmpl->b[ir][jr] = orig[ir][jr] * sincosval.x;
                        }

                    float minC = sqrt(SQR(tmpl->a[0][0]) + SQR(tmpl->b[0][0]));
                    float maxC = minC;
#ifdef _OPENMP
                    #pragma omp parallel for reduction(min:minC) reduction(max:maxC) schedule(dynamic,16)
#endif

                    for (int ir = 0; ir < Hd; ir++) {
                        for (int jr = 0; jr < Wd; jr++) {
                            bufchro[ir][jr] = sqrt(SQR(tmpl->a[ir][jr]) + SQR(tmpl->b[ir][jr]));
                            minC = rtengine::min(minC, bufchro[ir][jr]);
                            maxC = rtengine::max(maxC, bufchro[ir][jr]);
                        }
                    }

                    const float coefC = 0.01f * (max(fabs(minC), fabs(maxC)));

                    for (int ir = 0; ir < Hd; ir++) {
                        for (int jr = 0; jr < Wd; jr++) {
                            bufchro[ir][jr] /= coefC;
                        }
                    }
                } else {

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int ir = 0; ir < Hd; ir += 1)
                        for (int jr = 0; jr < Wd; jr += 1) {
                            float Chprov = orig1[ir][jr];
                            float2 sincosval;
                            sincosval.y = Chprov == 0.0f ? 1.f : transformed->a[ir][jr] / Chprov;
                            sincosval.x = Chprov == 0.0f ? 0.f : transformed->b[ir][jr] / Chprov;
                            tmpl->a[ir][jr] = orig[ir][jr] * sincosval.y;
                            tmpl->b[ir][jr] = orig[ir][jr] * sincosval.x;

                        }
                }


                if (!lp.invret) {
                    transit_shapedetect_retinex(5, tmpl, buflight, bufchro, hueref, chromaref, lumaref, lp, original, transformed, cx, cy, sk);

                } else {
                    InverseReti_Local(lp, hueref, chromaref, lumaref, original, transformed, tmpl, cx, cy, 1, sk);
                }

            }

            delete tmpl;
            delete [] origBuffer;
            delete [] origBuffer1;

            if (bufreti) {
                delete  bufreti;
            }
        }


        if (!lp.invex  && (lp.exposena && (lp.expcomp != 0.f || lp.war != 0 || lp.showmaskexpmet == 2 || lp.enaExpMask || lp.showmaskexpmet == 3 || lp.showmaskexpmet == 4 || lp.showmaskexpmet == 5 || (exlocalcurve  && localexutili)))) { //interior ellipse renforced lightness and chroma  //locallutili
            const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
            const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
            const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
            const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);
            const int bfh = yend - ystart;
            const int bfw = xend - xstart;

            if (bfw > 0 && bfh > 0) {
                std::unique_ptr<LabImage> bufexporig(new LabImage(bfw, bfh));
                std::unique_ptr<LabImage> bufexpfin(new LabImage(bfw, bfh));
                std::unique_ptr<LabImage> bufmaskblurexp;
                std::unique_ptr<LabImage> originalmaskexp;


                array2D<float> buflight(bfw, bfh);
                JaggedArray<float> bufl_ab(bfw, bfh);
                JaggedArray<float> buf_a_cat(bfw, bfh);
                JaggedArray<float> buf_b_cat(bfw, bfh);
                array2D<float> blend2;

                if (call <= 3) { //simpleprocess, dcrop, improccoordinator
                    float meansob = 0.f;

                    if (lp.showmaskexpmet == 2  || lp.enaExpMask || lp.showmaskexpmet == 3 || lp.showmaskexpmet == 5) {
                        bufmaskblurexp.reset(new LabImage(bfw, bfh));
                        originalmaskexp.reset(new LabImage(bfw, bfh));
                    }

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = ystart; y < yend; y++) {
                        for (int x = xstart; x < xend; x++) {
                            bufexporig->L[y - ystart][x - xstart] = original->L[y][x];
                        }
                    }

                    const int spotSi = rtengine::max(1 + 2 * max(1, lp.cir / sk), 5);

                    if (bfw > 2 * spotSi && bfh > 2 * spotSi && lp.struexp > 0.f) {
                        blend2(bfw, bfh);
                        ImProcFunctions::blendstruc(bfw, bfh, bufexporig.get(), 3.f / (sk * 1.4f), lp.struexp, blend2, sk, multiThread);

                        if (lp.showmaskexpmet == 4) {
#ifdef _OPENMP
                            #pragma omp parallel for schedule(dynamic,16)
#endif

                            for (int y = ystart; y < yend ; y++) {
                                for (int x = xstart; x < xend; x++) {
                                    const int lox = cx + x;
                                    const int loy = cy + y;
                                    int zone = 0;
                                    float localFactor = 1.f;
                                    const float achm = lp.trans / 100.f;

                                    if (lp.shapmet == 0) {
                                        calcTransition(lox, loy, achm, lp, zone, localFactor);
                                    } else if (lp.shapmet == 1) {
                                        calcTransitionrect(lox, loy, achm, lp, zone, localFactor);
                                    }

                                    if (zone > 0) {
                                        transformed->L[y][x] = CLIP(blend2[y - ystart][x - xstart]);
                                        transformed->a[y][x] = 0.f;
                                        transformed->b[y][x] = 0.f;
                                    }
                                }
                            }

                            return;
                        }
                    }

                    std::unique_ptr<array2D<float>> ble;
                    std::unique_ptr<array2D<float>> guid;

                    if (lp.showmaskexpmet == 2 || lp.enaExpMask || lp.showmaskexpmet == 3 || lp.showmaskexpmet == 5) {
                        ble.reset(new array2D<float>(bfw, bfh));
                        guid.reset(new array2D<float>(bfw, bfh));
                    }

                    float meanfab, fab;

                    mean_fab(xstart, ystart, bfw, bfh, bufexporig.get(), original, fab, meanfab, lp.chromaexp);

                    if (lp.showmaskexpmet == 2  || lp.enaExpMask || lp.showmaskexpmet == 3 || lp.showmaskexpmet == 5) {
#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++)
                            for (int jr = 0; jr < bfw; jr++) {
                                float kmaskLexp = 0.f;
                                float kmaskC = 0.f;
                                float kmaskHL = 0.f;
                                float kmaskH = 0.f;


                                if (locllmasexpCurve  && llmasexputili) {
                                    const float ligh = bufexporig->L[ir][jr] / 32768.f;
                                    kmaskLexp = 32768.f * LIM01(1.f - locllmasexpCurve[500.f * ligh]);
                                }

                                if (lp.showmaskexpmet != 5) {
                                    if (locccmasexpCurve && lcmasexputili) {
                                        const float chromaskr = 0.0001f + sqrt(SQR((bufexporig->a[ir][jr])) + SQR((bufexporig->b[ir][jr]))) / fab;
                                        kmaskC = LIM01(1.f - locccmasexpCurve[500.f *  chromaskr]);
                                    }
                                }

                                if (lochhmasexpCurve && lhmasexputili) {
                                    const float huema = xatan2f(bufexporig->b[ir][jr], bufexporig->a[ir][jr]);
                                    float h = Color::huelab_to_huehsv2(huema);
                                    h += 1.f / 6.f;

                                    if (h > 1.f) {
                                        h -= 1.f;
                                    }

                                    const float valHH = LIM01(1.f - lochhmasexpCurve[500.f * h]);

                                    if (lp.showmaskexpmet != 5) {
                                        kmaskH = valHH;
                                    }

                                    kmaskHL = 32768.f * valHH;
                                }

                                bufmaskblurexp->a[ir][jr] = kmaskC + kmaskH;
                                bufmaskblurexp->b[ir][jr] = kmaskC + kmaskH;
                                (*ble)[ir][jr] = LIM01(CLIPLOC(kmaskLexp + kmaskHL) / 32768.f);
                                (*guid)[ir][jr] = LIM01(bufexporig->L[ir][jr] / 32768.f);
                            }

                        if (lp.radmaexp > 0.f) {
                            guidedFilter(*guid, *ble, *ble, lp.radmaexp * 10.f / sk, 0.001, multiThread, 4);
                        }

                        LUTf lutTonemask(65536);
                        calcGammaLut(lp.gammaexp, lp.slomaexp, lutTonemask);

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++) {
                            for (int jr = 0; jr < bfw; jr++) {
                                const float L_ = 2.f * LIM01((*ble)[ir][jr]) * 32768.f;
                                bufmaskblurexp->L[ir][jr] = lutTonemask[L_];
                            }
                        }

                        const float radiusb = 1.f / sk;

#ifdef _OPENMP
                        #pragma omp parallel
#endif
                        {
                            gaussianBlur(bufmaskblurexp->L, bufmaskblurexp->L, bfw, bfh, radiusb);
                            gaussianBlur(bufmaskblurexp->a, bufmaskblurexp->a, bfw, bfh, 1.f + (0.5f * lp.radmaexp) / sk);
                            gaussianBlur(bufmaskblurexp->b, bufmaskblurexp->b, bfw, bfh, 1.f + (0.5f * lp.radmaexp) / sk);
                        }


                        if (lp.showmaskexpmet == 0 || lp.showmaskexpmet == 1 || lp.showmaskexpmet == 2 || lp.showmaskexpmet == 4 || lp.showmaskexpmet == 5 || lp.enaExpMask) {
                            blendmask(lp, xstart, ystart, cx, cy, bfw, bfh, bufexporig.get(), original, bufmaskblurexp.get(), originalmaskexp.get(), lp.blendmaexp);

                        } else if (lp.showmaskexpmet == 3) {
                            showmask(lp, xstart, ystart, cx, cy, bfw, bfh, bufexporig.get(), transformed, bufmaskblurexp.get());
                            return;
                        }
                    }


                    if (lp.showmaskexpmet == 0 || lp.showmaskexpmet == 1  || lp.showmaskexpmet == 2 || lp.showmaskexpmet == 5 || lp.enaExpMask) {
#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int y = 0; y < bfh; y++) {
                            for (int x = 0; x < bfw; x++) {
                                bufexpfin->L[y][x] = original->L[y + ystart][x + xstart];
                                bufexpfin->a[y][x] = original->a[y + ystart][x + xstart];
                                bufexpfin->b[y][x] = original->b[y + ystart][x + xstart];
                            }
                        }

                        if (exlocalcurve && localexutili) {// L=f(L) curve enhanced
#ifdef _OPENMP
                            #pragma omp parallel for schedule(dynamic,16)
#endif

                            for (int ir = 0; ir < bfh; ir++)
                                for (int jr = 0; jr < bfw; jr++) {
                                    bufexpfin->L[ir][jr] = 0.5f * exlocalcurve[2.f * bufexporig->L[ir][jr]];
                                }

                            if (lp.expcomp == 0.f) {
                                lp.expcomp = 0.1f;    // to enabled
                            }

                            ImProcFunctions::exlabLocal(lp, bfh, bfw, bufexpfin.get(), bufexpfin.get(), hltonecurveloc, shtonecurveloc, tonecurveloc);


                        } else {

                            ImProcFunctions::exlabLocal(lp, bfh, bfw, bufexporig.get(), bufexpfin.get(), hltonecurveloc, shtonecurveloc, tonecurveloc);
                        }

                        //cat02
                        if (params->locallab.spots.at(sp).warm != 0) {
                            ImProcFunctions::ciecamloc_02float(sp, bufexpfin.get());
                        }


                        if (lp.expchroma != 0.f) {
                            constexpr float ampli = 70.f;
                            const float ch = (1.f + 0.02f * lp.expchroma);
                            //convert data curve near values of slider -100 + 100, to be used after to detection shape
                            const float chprosl = ch <= 1.f ? 99.f * ch - 99.f : CLIPCHRO(ampli * ch - ampli);

#ifdef _OPENMP
                            #pragma omp parallel for schedule(dynamic,16)
#endif

                            for (int ir = 0; ir < bfh; ir++) {
                                for (int jr = 0; jr < bfw; jr++) {
                                    const float epsi = bufexporig->L[ir][jr] == 0.f ? 0.001f : 0.f;
                                    const float rapexp = bufexpfin->L[ir][jr] / (bufexporig->L[ir][jr] + epsi);

                                    bufl_ab[ir][jr] = chprosl * rapexp;
                                }
                            }
                        }

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++)
                            for (int jr = 0; jr < bfw; jr++) {
                                buflight[ir][jr] = CLIPRET((bufexpfin->L[ir][jr] - bufexporig->L[ir][jr]) / 328.f);
                                buf_a_cat[ir][jr] = CLIPRET((bufexpfin->a[ir][jr] - bufexporig->a[ir][jr]) / 328.f);
                                buf_b_cat[ir][jr] = CLIPRET((bufexpfin->b[ir][jr] - bufexporig->b[ir][jr]) / 328.f);
                            }

                        if (lp.softradiusexp > 0.f) {
                            softprocess(bufexporig.get(), buflight, lp.softradiusexp, bfh, bfw, sk, multiThread);
                        }
                    }

                    transit_shapedetect(1, bufexporig.get(), originalmaskexp.get(), buflight, bufl_ab, buf_a_cat, buf_b_cat, nullptr, false, hueref, chromaref, lumaref, sobelref, meansob, blend2, lp, original, transformed, cx, cy, sk);

                }
            }
        }

//inverse
        else if (lp.invex  && (lp.expcomp != 0.0 || lp.war != 0 || (exlocalcurve  && localexutili)) && lp.exposena) {
            float adjustr = 2.f;
            InverseColorLight_Local(sp, 1, lp, lightCurveloc, hltonecurveloc, shtonecurveloc, tonecurveloc, exlocalcurve, cclocalcurve, adjustr, localcutili, lllocalcurve, locallutili, original, transformed, cx, cy, hueref, chromaref, lumaref, sk);
        }


//local color and light
        const float factor = LocallabParams::LABGRIDL_CORR_MAX * 3.276f;
        const float scaling = LocallabParams::LABGRIDL_CORR_SCALE;
        const float scaledirect = LocallabParams::LABGRIDL_DIRECT_SCALE;
        float a_scale = (lp.highA - lp.lowA) / factor / scaling;
        float a_base = lp.lowA / scaling;
        float b_scale = (lp.highB - lp.lowB) / factor / scaling;
        float b_base = lp.lowB / scaling;
        bool ctoning = (a_scale != 0.f || b_scale != 0.f || a_base != 0.f || b_base != 0.f);

        if (!lp.inv  && (lp.chro != 0 || lp.ligh != 0.f || lp.cont != 0 || ctoning || lp.qualcurvemet != 0 || lp.showmaskcolmet == 2 || lp.enaColorMask || lp.showmaskcolmet == 3  || lp.showmaskcolmet == 4 || lp.showmaskcolmet == 5) && lp.colorena) { // || lllocalcurve)) { //interior ellipse renforced lightness and chroma  //locallutili
            const int ystart = std::max(static_cast<int>(lp.yc - lp.lyT) - cy, 0);
            const int yend = std::min(static_cast<int>(lp.yc + lp.ly) - cy, original->H);
            const int xstart = std::max(static_cast<int>(lp.xc - lp.lxL) - cx, 0);
            const int xend = std::min(static_cast<int>(lp.xc + lp.lx) - cx, original->W);
            const int bfh = yend - ystart;
            const int bfw = xend - xstart;

            if (bfw > 0 && bfh > 0) {
                std::unique_ptr<LabImage> bufcolorig;
                std::unique_ptr<LabImage> bufmaskblurcol;
                std::unique_ptr<LabImage> originalmaskcol;

                array2D<float> buflight(bfw, bfh, true);
                JaggedArray<float> bufchro(bfw, bfh, true);
                JaggedArray<float> bufhh(bfw, bfh, true);

                array2D<float> blend2;
                JaggedArray<float> buf_a(bfw, bfh, true);
                JaggedArray<float> buf_b(bfw, bfh, true);

                float adjustr = 1.0f;

                //adapt chroma to working profile
                if (params->icm.workingProfile == "ProPhoto")   {
                    adjustr = 1.2f;   // 1.2 instead 1.0 because it's very rare to have C>170..
                } else if (params->icm.workingProfile == "Adobe RGB")  {
                    adjustr = 1.8f;
                } else if (params->icm.workingProfile == "sRGB")       {
                    adjustr = 2.0f;
                } else if (params->icm.workingProfile == "WideGamut")  {
                    adjustr = 1.2f;
                } else if (params->icm.workingProfile == "Beta RGB")   {
                    adjustr = 1.4f;
                } else if (params->icm.workingProfile == "BestRGB")    {
                    adjustr = 1.4f;
                } else if (params->icm.workingProfile == "BruceRGB")   {
                    adjustr = 1.8f;
                }

                if (call <= 3) { //simpleprocess, dcrop, improccoordinator
                    float meansob = 0.f;
                    bufcolorig.reset(new LabImage(bfw, bfh));

                    if (lp.showmaskcolmet == 2  || lp.enaColorMask || lp.showmaskcolmet == 3 || lp.showmaskcolmet == 5) {
                        bufmaskblurcol.reset(new LabImage(bfw, bfh, true));
                        originalmaskcol.reset(new LabImage(bfw, bfh));
                    }

#ifdef _OPENMP
                    #pragma omp parallel for schedule(dynamic,16)
#endif

                    for (int y = 0; y < bfh ; y++) {
                        for (int x = 0; x < bfw; x++) {
                            bufcolorig->L[y][x] = original->L[y + ystart][x + xstart];
                        }
                    }

                    const int spotSi = std::max(1 + 2 * max(1,  lp.cir / sk), 5);
                    const bool blend = bfw > 2 * spotSi && bfh > 2 * spotSi && lp.struco > 0.f;

                    if (blend) {
                        blend2(bfw, bfh);
                        ImProcFunctions::blendstruc(bfw, bfh, bufcolorig.get(), 3.f / (sk * 1.4f), lp.struco, blend2, sk, multiThread);

                        if (lp.showmaskcolmet == 4) {
#ifdef _OPENMP
                            #pragma omp parallel for schedule(dynamic,16)
#endif

                            for (int y = ystart; y < yend ; y++) {
                                for (int x = xstart; x < xend; x++) {
                                    transformed->L[y][x] = blend2[y - ystart][x - xstart];
                                    transformed->a[y][x] = 0.f;
                                    transformed->b[y][x] = 0.f;
                                }
                            }

                            return;
                        }
                    }

                    array2D<float> ble(bfw, bfh);
                    array2D<float> guid(bfw, bfh);
                    float meanfab, fab;

                    mean_fab(xstart, ystart, bfw, bfh, bufcolorig.get(), original, fab, meanfab, lp.chromacol);

                    if (lp.showmaskcolmet == 2  || lp.enaColorMask || lp.showmaskcolmet == 3 || lp.showmaskcolmet == 5) {

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int y = 0; y < bfh; y++) {
                            for (int x = 0; x < bfw; x++) {
                                bufmaskblurcol->L[y][x] = original->L[y + ystart][x + xstart];
                                bufmaskblurcol->a[y][x] = original->a[y + ystart][x + xstart];
                                bufmaskblurcol->b[y][x] = original->b[y + ystart][x + xstart];
                            }
                        }

#ifdef _OPENMP
                        #pragma omp parallel
#endif
                        {
#ifdef __SSE2__
                            float atan2Buffer[bfw] ALIGNED64;
#endif
#ifdef _OPENMP
                            #pragma omp for schedule(dynamic, 16)
#endif

                            for (int ir = 0; ir < bfh; ir++) {
#ifdef __SSE2__

                                if (lochhmasCurve && lhmasutili) {
                                    int i = 0;

                                    for (; i < bfw - 3; i += 4) {
                                        STVF(atan2Buffer[i], xatan2f(LVFU(bufcolorig->b[ir][i]), LVFU(bufcolorig->a[ir][i])));
                                    }

                                    for (; i < bfw; i++) {
                                        atan2Buffer[i] = xatan2f(bufcolorig->b[ir][i], bufcolorig->a[ir][i]);
                                    }
                                }

#endif

                                for (int jr = 0; jr < bfw; jr++) {
                                    float kmaskL = 0.f;
                                    float kmaskC = 0.f;
                                    float kmaskHL = 0.f;
                                    float kmaskH = 0.f;

                                    if (locllmasCurve && llmasutili) {
                                        kmaskL = 32768.f * LIM01(1.f - locllmasCurve[(500.f / 32768.f) * bufcolorig->L[ir][jr]]);
                                    }

                                    if (lp.showmaskcolmet != 5 && locccmasCurve && lcmasutili) {
                                        kmaskC = LIM01(1.f - locccmasCurve[500.f * (0.0001f + sqrt(SQR(bufcolorig->a[ir][jr]) + SQR(bufcolorig->b[ir][jr])) / fab)]);
                                    }

                                    if (lochhmasCurve && lhmasutili) {
#ifdef __SSE2__
                                        const float huema = atan2Buffer[jr];
#else
                                        const float huema = xatan2f(bufcolorig->b[ir][jr], bufcolorig->a[ir][jr]);
#endif
                                        float h = Color::huelab_to_huehsv2(huema);
                                        h += 1.f / 6.f;

                                        if (h > 1.f) {
                                            h -= 1.f;
                                        }

                                        const float valHH = LIM01(1.f - lochhmasCurve[500.f *  h]);

                                        if (lp.showmaskcolmet != 5) {
                                            kmaskH = valHH;
                                        }

                                        kmaskHL = 32768.f * valHH;
                                    }

                                    bufmaskblurcol->L[ir][jr] = CLIPLOC(kmaskL + kmaskHL);
                                    bufmaskblurcol->a[ir][jr] = CLIPC(kmaskC + kmaskH);
                                    bufmaskblurcol->b[ir][jr] = CLIPC(kmaskC + kmaskH);
                                    ble[ir][jr] = bufmaskblurcol->L[ir][jr] / 32768.f;
                                    guid[ir][jr] = bufcolorig->L[ir][jr] / 32768.f;
                                }
                            }
                        }

                        if (lp.radmacol > 0.f) {
                            guidedFilter(guid, ble, ble, lp.radmacol * 10.f / sk, 0.001, multiThread, 4);
                        }

                        LUTf lutTonemaskexp(65536);
                        calcGammaLut(lp.gammacol, lp.slomacol, lutTonemaskexp);

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++) {
                            for (int jr = 0; jr < bfw; jr++) {
                                bufmaskblurcol->L[ir][jr] = lutTonemaskexp[LIM01(ble[ir][jr]) * 65536.f];
                            }
                        }
                    }

                    const float radiusb = 1.f / sk;

                    if (lp.showmaskcolmet == 2  || lp.enaColorMask || lp.showmaskcolmet == 3 || lp.showmaskcolmet == 5) {
#ifdef _OPENMP
                        #pragma omp parallel
#endif
                        {
                            gaussianBlur(bufmaskblurcol->L, bufmaskblurcol->L, bfw, bfh, radiusb);
                            gaussianBlur(bufmaskblurcol->a, bufmaskblurcol->a, bfw, bfh, 1.f + (0.5f * lp.radmacol) / sk);
                            gaussianBlur(bufmaskblurcol->b, bufmaskblurcol->b, bfw, bfh, 1.f + (0.5f * lp.radmacol) / sk);
                        }

                        if (lp.showmaskcolmet == 0 || lp.showmaskcolmet == 1 || lp.showmaskcolmet == 2 || lp.showmaskcolmet == 4 || lp.showmaskcolmet == 5 || lp.enaColorMask) {
                            originalmaskcol->CopyFrom(transformed);
                            blendmask(lp, xstart, ystart, cx, cy, bfw, bfh, bufcolorig.get(), original, bufmaskblurcol.get(), originalmaskcol.get(), lp.blendmacol);
                        } else if (lp.showmaskcolmet == 3) {
                            showmask(lp, xstart, ystart, cx, cy, bfw, bfh, bufcolorig.get(), transformed, bufmaskblurcol.get());

                            return;

                        }
                    }

                    if (lp.showmaskcolmet == 0 || lp.showmaskcolmet == 1 || lp.showmaskcolmet == 2 || lp.showmaskcolmet == 5 || lp.enaColorMask) {

                        float chprosl = 1.f;

                        if (lp.chro != 0.f) {
                            const float ch = (1.f + 0.01f * lp.chro) ; //* (chromat * adjustr)  / ((chromat + 0.00001f) * adjustr); //ch between 0 and 0 50 or more;

                            if (ch <= 1.f) {//convert data curve near values of slider -100 + 100, to be used after to detection shape
                                chprosl = 99.f * ch - 99.f;
                            } else {
                                constexpr float ampli = 70.f;
                                chprosl = CLIPCHRO(ampli * ch - ampli);  //ampli = 25.f arbitrary empirical coefficient between 5 and 50
                            }
                        }

#ifdef _OPENMP
                        #pragma omp parallel for schedule(dynamic,16)
#endif

                        for (int ir = 0; ir < bfh; ir++)
                            for (int jr = 0; jr < bfw; jr++) {
                                float bufcolcalca = bufcolorig->a[ir][jr];
                                float bufcolcalcb = bufcolorig->b[ir][jr];
                                float bufcolcalcL = bufcolorig->L[ir][jr];

                                float chprocu = 1.f;

                                if (cclocalcurve  && lp.qualcurvemet != 0 && localcutili) { // C=f(C) curve
                                    const float chromat = sqrt(SQR(bufcolcalca) +  SQR(bufcolcalcb));
                                    const float ch = cclocalcurve[chromat * adjustr] / ((chromat + 0.00001f) * adjustr); //ch between 0 and 0 50 or more
                                    constexpr float ampli = 25.f;
                                    chprocu = CLIPCHRO(ampli * ch - ampli);  //ampli = 25.f arbitrary empirical coefficient between 5 and 50
                                }

                                bufchro[ir][jr] = chprosl + chprocu;

                                if (lochhCurve && HHutili && lp.qualcurvemet != 0) {
                                    const float hhforcurv = xatan2f(bufcolcalcb, bufcolcalca);
                                    const float valparam = float ((lochhCurve[500.f * Color::huelab_to_huehsv2(hhforcurv)] - 0.5f));  //get H=f(H)  1.7 optimisation !
                                    bufhh[ir][jr] = CLIPRET(200.f * valparam);
                                }


                                if (lp.ligh != 0.f || lp.cont != 0) {
                                    calclight(bufcolcalcL, lp.ligh, bufcolcalcL, lightCurveloc);  //replace L-curve
                                }

                                if (lllocalcurve && locallutili  && lp.qualcurvemet != 0) {// L=f(L) curve enhanced
                                    bufcolcalcL = 0.5f * lllocalcurve[bufcolcalcL * 2.f];// / ((lighn + 0.00001f) * 1.9f) ; // / ((lighn) / 1.9f) / 3.61f; //lh between 0 and 0 50 or more
                                }

                                if (loclhCurve && LHutili && lp.qualcurvemet != 0) {
                                    const float rhue = xatan2f(bufcolcalcb, bufcolcalca);
                                    float l_r = bufcolcalcL / 32768.f; //Luminance Lab in 0..1
                                    const float valparam = loclhCurve[500.f * Color::huelab_to_huehsv2(rhue)] - 0.5f;  //get l_r=f(H)

                                    if (valparam > 0.f) {
                                        l_r = (1.f - valparam) * l_r + valparam * (1.f - SQR(((SQR(1.f - min(l_r, 1.0f))))));
                                    } else {
                                        constexpr float khu = 1.9f; //in reserve in case of!
                                        //for negative
                                        l_r *= (1.f + khu * valparam);
                                    }

                                    bufcolcalcL = l_r * 32768.f;

                                }

                                if (ctoning) {
                                    if (lp.gridmet == 0) {
                                        bufcolcalca += bufcolcalcL * a_scale + a_base;
                                        bufcolcalcb += bufcolcalcL * b_scale + b_base;
                                    } else if (lp.gridmet == 1) {
                                        bufcolcalca += scaledirect * a_scale;
                                        bufcolcalcb += scaledirect * b_scale;
                                    }

                                    bufcolcalca = CLIPC(bufcolcalca);
                                    bufcolcalcb = CLIPC(bufcolcalcb);

                                }

                                buflight[ir][jr] = CLIPRET((bufcolcalcL - bufcolorig->L[ir][jr]) / 328.f);
                                buf_a[ir][jr] = CLIPRET((bufcolcalca - bufcolorig->a[ir][jr]) / 328.f);;
                                buf_b[ir][jr] = CLIPRET((bufcolcalcb - bufcolorig->b[ir][jr]) / 328.f);;


                            }

                        if (lp.softradiuscol > 0.f) {
                            softprocess(bufcolorig.get(), buflight, lp.softradiuscol, bfh, bfw, sk, multiThread);
                        }

                    }

                    float **temp = nullptr;

                    if (blend) {
                        temp = blend2;
                    }

                    transit_shapedetect(0, bufcolorig.get(), originalmaskcol.get(), buflight, bufchro, buf_a, buf_b, bufhh, HHutili, hueref, chromaref, lumaref, sobelref, meansob, temp, lp, original, transformed, cx, cy, sk);

                }
            }
        }

//inverse
        else if (lp.inv  && (lp.chro != 0 || lp.ligh != 0 || exlocalcurve) && lp.colorena) {
            float adjustr = 1.0f;

//adapt chroma to working profile
            if (params->icm.workingProfile == "ProPhoto")   {
                adjustr = 1.2f;   // 1.2 instead 1.0 because it's very rare to have C>170..
            } else if (params->icm.workingProfile == "Adobe RGB")  {
                adjustr = 1.8f;
            } else if (params->icm.workingProfile == "sRGB")       {
                adjustr = 2.0f;
            } else if (params->icm.workingProfile == "WideGamut")  {
                adjustr = 1.2f;
            } else if (params->icm.workingProfile == "Beta RGB")   {
                adjustr = 1.4f;
            } else if (params->icm.workingProfile == "BestRGB")    {
                adjustr = 1.4f;
            } else if (params->icm.workingProfile == "BruceRGB")   {
                adjustr = 1.8f;
            }

            InverseColorLight_Local(sp, 0, lp, lightCurveloc, hltonecurveloc, shtonecurveloc, tonecurveloc, exlocalcurve, cclocalcurve, adjustr, localcutili, lllocalcurve, locallutili, original, transformed, cx, cy, hueref, chromaref, lumaref, sk);
        }

// Gamut and Munsell control - very important do not desactivated to avoid crash
        if (params->locallab.spots.at(sp).avoid) {
            const float ach = (float)lp.trans / 100.f;

            TMatrix wiprof = ICCStore::getInstance()->workingSpaceInverseMatrix(params->icm.workingProfile);
            const float wip[3][3] = {
                {static_cast<float>(wiprof[0][0]), static_cast<float>(wiprof[0][1]), static_cast<float>(wiprof[0][2])},
                {static_cast<float>(wiprof[1][0]), static_cast<float>(wiprof[1][1]), static_cast<float>(wiprof[1][2])},
                {static_cast<float>(wiprof[2][0]), static_cast<float>(wiprof[2][1]), static_cast<float>(wiprof[2][2])}
            };
            const bool highlight = params->toneCurve.hrenabled;
            const bool needHH = (lp.chro != 0.f);
#ifdef _OPENMP
            #pragma omp parallel if (multiThread)
#endif
            {
#ifdef __SSE2__
                float atan2Buffer[transformed->W] ALIGNED16;
                float sqrtBuffer[transformed->W] ALIGNED16;
                float sincosyBuffer[transformed->W] ALIGNED16;
                float sincosxBuffer[transformed->W] ALIGNED16;
                vfloat c327d68v = F2V(327.68f);
                vfloat onev = F2V(1.f);
#endif

#ifdef _OPENMP
#ifdef _DEBUG
                #pragma omp for schedule(dynamic,16) firstprivate(MunsDebugInfo)
#else
                #pragma omp for schedule(dynamic,16)
#endif
#endif

                for (int y = 0; y < transformed->H; y++) {
                    const int loy = cy + y;
                    const bool isZone0 = loy > lp.yc + lp.ly || loy < lp.yc - lp.lyT; // whole line is zone 0 => we can skip a lot of processing

                    if (isZone0) { // outside selection and outside transition zone => no effect, keep original values

                        continue;
                    }

#ifdef __SSE2__
                    int i = 0;

                    for (; i < transformed->W - 3; i += 4) {
                        vfloat av = LVFU(transformed->a[y][i]);
                        vfloat bv = LVFU(transformed->b[y][i]);

                        if (needHH) { // only do expensive atan2 calculation if needed
                            STVF(atan2Buffer[i], xatan2f(bv, av));
                        }

                        vfloat Chprov1v = vsqrtf(SQRV(bv) + SQRV(av));
                        STVF(sqrtBuffer[i], Chprov1v / c327d68v);
                        vfloat sincosyv = av / Chprov1v;
                        vfloat sincosxv = bv / Chprov1v;
                        vmask selmask = vmaskf_eq(Chprov1v, ZEROV);
                        sincosyv = vself(selmask, onev, sincosyv);
                        sincosxv = vselfnotzero(selmask, sincosxv);
                        STVF(sincosyBuffer[i], sincosyv);
                        STVF(sincosxBuffer[i], sincosxv);
                    }

                    for (; i < transformed->W; i++) {
                        float aa = transformed->a[y][i];
                        float bb = transformed->b[y][i];

                        if (needHH) { // only do expensive atan2 calculation if needed
                            atan2Buffer[i] = xatan2f(bb, aa);
                        }

                        float Chprov1 = sqrtf(SQR(bb) + SQR(aa));
                        sqrtBuffer[i] = Chprov1 / 327.68f;

                        if (Chprov1 == 0.0f) {
                            sincosyBuffer[i] = 1.f;
                            sincosxBuffer[i] = 0.0f;
                        } else {
                            sincosyBuffer[i] = aa / Chprov1;
                            sincosxBuffer[i] = bb / Chprov1;
                        }

                    }

#endif

                    for (int x = 0; x < transformed->W; x++) {
                        int lox = cx + x;
                        int zone = 0;
                        float localFactor = 1.f;

                        if (lp.shapmet == 0) {
                            calcTransition(lox, loy, ach, lp, zone, localFactor);
                        } else if (lp.shapmet == 1) {
                            calcTransitionrect(lox, loy, ach, lp, zone, localFactor);
                        }

                        if (zone == 0) { // outside selection and outside transition zone => no effect, keep original values
                            continue;
                        }

                        float Lprov1 = transformed->L[y][x] / 327.68f;
                        float2 sincosval;
#ifdef __SSE2__
                        float HH = atan2Buffer[x]; // reading HH from line buffer even if line buffer is not filled is faster than branching
                        float Chprov1 = sqrtBuffer[x];
                        sincosval.y = sincosyBuffer[x];
                        sincosval.x = sincosxBuffer[x];
                        float chr = 0.f;

#else
                        float aa = transformed->a[y][x];
                        float bb = transformed->b[y][x];
                        float HH = 0.f, chr = 0.f;

                        if (needHH) { // only do expensive atan2 calculation if needed
                            HH = xatan2f(bb, aa);
                        }

                        float Chprov1 = sqrtf(SQR(aa) + SQR(bb)) / 327.68f;

                        if (Chprov1 == 0.0f) {
                            sincosval.y = 1.f;
                            sincosval.x = 0.0f;
                        } else {
                            sincosval.y = aa / (Chprov1 * 327.68f);
                            sincosval.x = bb / (Chprov1 * 327.68f);
                        }

#endif

#ifdef _DEBUG
                        bool neg = false;
                        bool more_rgb = false;
                        Chprov1 = min(Chprov1, chr);

                        Color::gamutLchonly(sincosval, Lprov1, Chprov1, wip, highlight, 0.15f, 0.92f, neg, more_rgb);
#else
                        Color::pregamutlab(Lprov1, HH, chr);
                        Chprov1 = min(Chprov1, chr);
                        Color::gamutLchonly(sincosval, Lprov1, Chprov1, wip, highlight, 0.15f, 0.92f);
#endif

                        transformed->L[y][x] = Lprov1 * 327.68f;
                        transformed->a[y][x] = 327.68f * Chprov1 * sincosval.y;
                        transformed->b[y][x] = 327.68f * Chprov1 * sincosval.x;

                        if (needHH) {
                            float Lprov2 = original->L[y][x] / 327.68f;
                            float correctionHue = 0.f; // Munsell's correction
                            float correctlum = 0.f;
                            float memChprov = sqrtf(SQR(original->a[y][x]) + SQR(original->b[y][x])) / 327.68f;
                            float Chprov = sqrtf(SQR(transformed->a[y][x]) + SQR(transformed->b[y][x])) / 327.68f;
#ifdef _DEBUG
                            Color::AllMunsellLch(true, Lprov1, Lprov2, HH, Chprov, memChprov, correctionHue, correctlum, MunsDebugInfo);
#else
                            Color::AllMunsellLch(true, Lprov1, Lprov2, HH, Chprov, memChprov, correctionHue, correctlum);
#endif

                            if (fabs(correctionHue) < 0.015f) {
                                HH += correctlum;    // correct only if correct Munsell chroma very little.
                            }

                            sincosval = xsincosf(HH + correctionHue);

                            transformed->a[y][x] = 327.68f * Chprov * sincosval.y; // apply Munsell
                            transformed->b[y][x] = 327.68f * Chprov * sincosval.x;
                        }
                    }
                }
            }
        }

#ifdef _DEBUG
        delete MunsDebugInfo;
#endif

    }

}

}
