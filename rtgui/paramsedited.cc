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
 */
#include <cstring>

#include "paramsedited.h"

#include "addsetids.h"
#include "options.h"

#include "../rtengine/procparams.h"

ParamsEdited::ParamsEdited(bool value)
{

    set(value);
}

void ParamsEdited::set(bool v)
{

    general.rank         = v;
    general.colorlabel   = v;
    general.intrash      = v;
    toneCurve.curve      = v;
    toneCurve.curve2     = v;
    toneCurve.curveMode  = v;
    toneCurve.curveMode2 = v;
    toneCurve.brightness = v;
    toneCurve.black      = v;
    toneCurve.contrast   = v;
    toneCurve.saturation = v;
    toneCurve.shcompr    = v;
    toneCurve.hlcompr    = v;
    toneCurve.hlcomprthresh = v;
    toneCurve.autoexp    = v;
    toneCurve.clip       = v;
    toneCurve.expcomp    = v;
    toneCurve.hrenabled   = v;
    toneCurve.method    = v;
    toneCurve.histmatching = v;
    toneCurve.fromHistMatching = v;
    toneCurve.clampOOG = v;
    retinex.cdcurve    = v;
    retinex.mapcurve    = v;
    retinex.cdHcurve    = v;
    retinex.lhcurve    = v;
    retinex.retinexMethod    = v;
    retinex.mapMethod    = v;
    retinex.viewMethod    = v;
    retinex.retinexcolorspace    = v;
    retinex.gammaretinex    = v;
    retinex.enabled    = v;
    retinex.str    = v;
    retinex.scal    = v;
    retinex.iter    = v;
    retinex.grad    = v;
    retinex.grads    = v;
    retinex.gam    = v;
    retinex.slope    = v;
    retinex.neigh    = v;
    retinex.offs    = v;
    retinex.vart    = v;
    retinex.limd    = v;
    retinex.highl    = v;
    retinex.skal    = v;
    retinex.medianmap = v;
    retinex.transmissionCurve   = v;
    retinex.gaintransmissionCurve   = v;
    retinex.highlights    = v;
    retinex.htonalwidth   = v;
    retinex.shadows       = v;
    retinex.stonalwidth   = v;
    retinex.radius        = v;

    retinex.retinex = v;
    labCurve.enabled = v;
    labCurve.lcurve      = v;
    labCurve.acurve      = v;
    labCurve.bcurve      = v;
    labCurve.cccurve     = v;
    labCurve.chcurve     = v;
    labCurve.lhcurve     = v;
    labCurve.hhcurve     = v;
    labCurve.lccurve    = v;
    labCurve.clcurve    = v;
    labCurve.brightness  = v;
    labCurve.contrast    = v;
    labCurve.chromaticity    = v;
    labCurve.avoidcolorshift = v;
    labCurve.rstprotection   = v;
    labCurve.lcredsk         = v;
    localContrast.enabled = v;
    localContrast.radius = v;
    localContrast.amount = v;
    localContrast.darkness = v;
    localContrast.lightness = v;
    rgbCurves.enabled = v;
    rgbCurves.lumamode       = v;
    rgbCurves.rcurve         = v;
    rgbCurves.gcurve         = v;
    rgbCurves.bcurve         = v;
    colorToning.enabled      = v;
    colorToning.autosat      = v;
    colorToning.opacityCurve = v;
    colorToning.colorCurve   = v;
    colorToning.satprotectionthreshold = v;
    colorToning.saturatedopacity       = v;
    colorToning.strength               = v;
    colorToning.shadowsColSat          = v;
    colorToning.hlColSat   = v;
    colorToning.balance    = v;
    colorToning.clcurve    = v;
    colorToning.method     = v;
    colorToning.twocolor   = v;
    colorToning.cl2curve   = v;
    colorToning.redlow     = v;
    colorToning.greenlow   = v;
    colorToning.bluelow    = v;
    colorToning.satlow     = v;
    colorToning.sathigh    = v;
    colorToning.redmed     = v;
    colorToning.greenmed   = v;
    colorToning.bluemed    = v;
    colorToning.redhigh    = v;
    colorToning.greenhigh  = v;
    colorToning.bluehigh   = v;
    colorToning.lumamode   = v;
    colorToning.labgridALow = v;
    colorToning.labgridBLow = v;
    colorToning.labgridAHigh = v;
    colorToning.labgridBHigh = v;
    colorToning.labregions = v;
    colorToning.labregionsShowMask = v;

    sharpening.enabled            = v;
    sharpening.contrast           = v;
    sharpening.radius             = v;
    sharpening.blurradius         = v;
    sharpening.amount             = v;
    sharpening.threshold          = v;
    sharpening.edgesonly          = v;
    sharpening.edges_radius       = v;
    sharpening.edges_tolerance    = v;
    sharpening.halocontrol        = v;
    sharpening.halocontrol_amount = v;
    sharpening.method         = v;
    sharpening.deconvamount   = v;
    sharpening.deconvradius   = v;
    sharpening.deconviter     = v;
    sharpening.deconvdamping  = v;
    prsharpening.enabled            = v;
    prsharpening.contrast           = v;
    prsharpening.radius             = v;
    prsharpening.amount             = v;
    prsharpening.threshold          = v;
    prsharpening.edgesonly          = v;
    prsharpening.edges_radius       = v;
    prsharpening.edges_tolerance    = v;
    prsharpening.halocontrol        = v;
    prsharpening.halocontrol_amount = v;
    prsharpening.method         = v;
    prsharpening.deconvamount   = v;
    prsharpening.deconvradius   = v;
    prsharpening.deconviter     = v;
    prsharpening.deconvdamping  = v;
    sharpenEdge.enabled       = v;
    sharpenEdge.passes        = v;
    sharpenEdge.amount        = v;
    sharpenEdge.threechannels = v;
    sharpenMicro.enabled      = v;
    sharpenMicro.matrix       = v;
    sharpenMicro.contrast     = v;
    sharpenMicro.amount       = v;
    sharpenMicro.uniformity   = v;
    vibrance.enabled          = v;
    vibrance.pastels          = v;
    vibrance.saturated        = v;
    vibrance.psthreshold      = v;
    vibrance.protectskins     = v;
    vibrance.avoidcolorshift  = v;
    vibrance.pastsattog       = v;
    vibrance.skintonescurve   = v;
    colorappearance.enabled    = v;
    colorappearance.degree     = v;
    colorappearance.autodegree = v;
    colorappearance.degreeout     = v;
    colorappearance.autodegreeout = v;
    colorappearance.surround     = v;
    colorappearance.surrsrc     = v;
    colorappearance.adapscen    = v;
    colorappearance.autoadapscen = v;
    colorappearance.ybscen    = v;
    colorappearance.autoybscen = v;
    colorappearance.adaplum    = v;
    colorappearance.badpixsl    = v;
    colorappearance.wbmodel    = v;
    colorappearance.algo    = v;

    colorappearance.jlight     = v;
    colorappearance.qbright     = v;
    colorappearance.chroma     = v;
    colorappearance.schroma     = v;
    colorappearance.mchroma     = v;
    colorappearance.contrast     = v;
    colorappearance.qcontrast     = v;
    colorappearance.colorh     = v;
    colorappearance.rstprotection     = v;
    colorappearance.surrsource = v;
    colorappearance.gamut = v;
//  colorappearance.badpix = v;
    colorappearance.datacie = v;
    colorappearance.tonecie = v;
//  colorappearance.sharpcie = v;
    colorappearance.curve      = v;
    colorappearance.curve2     = v;
    colorappearance.curve3     = v;
    colorappearance.curveMode  = v;
    colorappearance.curveMode2 = v;
    colorappearance.curveMode3 = v;
    colorappearance.tempout     = v;
    colorappearance.greenout     = v;
    colorappearance.ybout     = v;
    colorappearance.tempsc     = v;
    colorappearance.greensc     = v;

    //colorBoost.amount         = v;
    //colorBoost.avoidclip      = v;
    //colorBoost.enable_saturationlimiter = v;
    //colorBoost.saturationlimit = v;
    wb.enabled                 = v;
    wb.method                  = v;
    wb.green                   = v;
    wb.temperature             = v;
    wb.equal                   = v;
    wb.tempBias                = v;
    //colorShift.a               = v;
    //colorShift.b               = v;
    //lumaDenoise.enabled        = v;
    //lumaDenoise.radius         = v;
    //lumaDenoise.edgetolerance  = v;
    //colorDenoise.enabled       = v;
    //colorDenoise.amount        = v;
    defringe.enabled           = v;
    defringe.radius            = v;
    defringe.threshold         = v;
    defringe.huecurve          = v;
    impulseDenoise.enabled     = v;
    impulseDenoise.thresh      = v;
    dirpyrDenoise.enabled      = v;
    dirpyrDenoise.enhance      = v;
//  dirpyrDenoise.perform      = v;
    dirpyrDenoise.lcurve      = v;
    dirpyrDenoise.cccurve      = v;
    dirpyrDenoise.median      = v;
    dirpyrDenoise.luma         = v;
    dirpyrDenoise.Ldetail      = v;
    dirpyrDenoise.chroma       = v;
    dirpyrDenoise.redchro      = v;
    dirpyrDenoise.bluechro     = v;
    dirpyrDenoise.gamma        = v;
    dirpyrDenoise.passes        = v;
    dirpyrDenoise.dmethod      = v;
    dirpyrDenoise.Lmethod      = v;
    dirpyrDenoise.Cmethod      = v;
    dirpyrDenoise.C2method      = v;
    dirpyrDenoise.smethod      = v;
    dirpyrDenoise.medmethod      = v;
    dirpyrDenoise.methodmed      = v;
    dirpyrDenoise.rgbmethod      = v;
    epd.enabled                = v;
    epd.strength            = v;
    epd.gamma            = v;
    epd.edgeStopping        = v;
    epd.scale               = v;
    epd.reweightingIterates = v;
    fattal.enabled   = v;
    fattal.threshold = v;
    fattal.amount    = v;
    fattal.anchor    = v;
    sh.enabled       = v;
    sh.highlights    = v;
    sh.htonalwidth   = v;
    sh.shadows       = v;
    sh.stonalwidth   = v;
    sh.radius        = v;
    sh.lab           = v;
    crop.enabled = v;
    crop.x       = v;
    crop.y       = v;
    crop.w       = v;
    crop.h       = v;
    crop.fixratio = v;
    crop.ratio   = v;
    crop.orientation = v;
    crop.guide   = v;
    coarse.rotate = v;
    coarse.hflip = v;
    coarse.vflip = v;
    commonTrans.autofill = v;
    rotate.degree = v;
    distortion.amount = v;
    lensProf.lcMode = v;
    lensProf.lcpFile = v;
    lensProf.useDist = v;
    lensProf.useVign = v;
    lensProf.useCA = v;
    lensProf.useLensfun = v;
    lensProf.lfAutoMatch = v;
    lensProf.lfCameraMake = v;
    lensProf.lfCameraModel = v;
    lensProf.lfLens = v;
    perspective.horizontal = v;
    perspective.vertical = v;
    gradient.enabled = v;
    gradient.degree = v;
    gradient.feather = v;
    gradient.strength = v;
    gradient.centerX = v;
    gradient.centerY = v;

    locallab.enabled = v;
    locallab.nbspot = v;
    locallab.selspot = v;
    locallab.id = v;

    for (size_t i = 0; i < locallab.spots.size(); i++) {
        locallab.spots.at(i).set(v);
    }

    pcvignette.enabled = v;
    pcvignette.strength = v;
    pcvignette.feather = v;
    pcvignette.roundness = v;
    cacorrection.red = v;
    cacorrection.blue = v;
    vignetting.amount = v;
    vignetting.radius = v;
    vignetting.strength = v;
    vignetting.centerX = v;
    vignetting.centerY = v;
    chmixer.enabled = v;
    chmixer.red[0] = v;
    chmixer.red[1] = v;
    chmixer.red[2] = v;
    chmixer.green[0] = v;
    chmixer.green[1] = v;
    chmixer.green[2] = v;
    chmixer.blue[0] = v;
    chmixer.blue[1] = v;
    chmixer.blue[2] = v;
    blackwhite.enabled   = v;
    blackwhite.enabledcc   = v;
    blackwhite.mixerRed   = v;
    blackwhite.mixerOrange   = v;
    blackwhite.mixerYellow   = v;
    blackwhite.mixerGreen   = v;
    blackwhite.mixerCyan   = v;
    blackwhite.mixerBlue   = v;
    blackwhite.mixerMagenta   = v;
    blackwhite.mixerPurple   = v;
    blackwhite.gammaRed   = v;
    blackwhite.gammaGreen   = v;
    blackwhite.gammaBlue   = v;
    blackwhite.filter   = v;
    blackwhite.setting   = v;
    blackwhite.method   = v;
    blackwhite.luminanceCurve = v;
    blackwhite.beforeCurve      = v;
    blackwhite.beforeCurveMode  = v;
    blackwhite.afterCurve      = v;
    blackwhite.afterCurveMode  = v;
    blackwhite.autoc    = v;
    blackwhite.algo    = v;


    resize.scale     = v;
    resize.appliesTo = v;
    resize.method    = v;
    resize.dataspec  = v;
    resize.width     = v;
    resize.height    = v;
    resize.enabled   = v;
    resize.allowUpscaling = v;
    icm.inputProfile = v;
    icm.toneCurve = v;
    icm.applyLookTable = v;
    icm.applyBaselineExposureOffset = v;
    icm.applyHueSatMap = v;
    icm.dcpIlluminant = v;
    icm.workingProfile = v;
    icm.outputProfile = v;
    icm.outputIntent = v;
    icm.outputBPC = v;
    icm.workingTRCGamma = v;
    icm.workingTRCSlope = v;
    icm.workingTRC = v;
    raw.bayersensor.method = v;
    raw.bayersensor.border = v;
    raw.bayersensor.imageNum = v;
    raw.bayersensor.ccSteps = v;
    raw.bayersensor.exBlack0 = v;
    raw.bayersensor.exBlack1 = v;
    raw.bayersensor.exBlack2 = v;
    raw.bayersensor.exBlack3 = v;
    raw.bayersensor.exTwoGreen = v;
    raw.bayersensor.dcbIterations = v;
    raw.bayersensor.dcbEnhance = v;
    //raw.bayersensor.allEnhance = v;
    raw.bayersensor.lmmseIterations = v;
    raw.bayersensor.dualDemosaicAutoContrast = v;
    raw.bayersensor.dualDemosaicContrast = v;
    raw.bayersensor.pixelShiftMotionCorrectionMethod = v;
    raw.bayersensor.pixelShiftEperIso = v;
    raw.bayersensor.pixelShiftSigma = v;
    raw.bayersensor.pixelShiftShowMotion = v;
    raw.bayersensor.pixelShiftShowMotionMaskOnly = v;
    raw.bayersensor.pixelShiftHoleFill = v;
    raw.bayersensor.pixelShiftMedian = v;
    raw.bayersensor.pixelShiftGreen = v;
    raw.bayersensor.pixelShiftBlur = v;
    raw.bayersensor.pixelShiftSmooth = v;
    raw.bayersensor.pixelShiftEqualBright = v;
    raw.bayersensor.pixelShiftEqualBrightChannel = v;
    raw.bayersensor.pixelShiftNonGreenCross = v;
    raw.bayersensor.pixelShiftDemosaicMethod = v;
    raw.bayersensor.greenEq = v;
    raw.bayersensor.linenoise = v;
    raw.bayersensor.linenoiseDirection = v;
    raw.bayersensor.pdafLinesFilter = v;
    raw.xtranssensor.method = v;
    raw.xtranssensor.dualDemosaicAutoContrast = v;
    raw.xtranssensor.dualDemosaicContrast = v;
    raw.xtranssensor.border = v;
    raw.xtranssensor.ccSteps = v;
    raw.xtranssensor.exBlackRed = v;
    raw.xtranssensor.exBlackGreen = v;
    raw.xtranssensor.exBlackBlue = v;
    raw.ca_autocorrect = v;
    raw.ca_avoidcolourshift = v;
    raw.caautoiterations  = v;
    raw.cablue  = v;
    raw.cared   = v;
    raw.hotPixelFilter = v;
    raw.deadPixelFilter = v;
    raw.hotdeadpix_thresh = v;
    raw.darkFrame = v;
    raw.df_autoselect = v;
    raw.ff_file = v;
    raw.ff_AutoSelect = v;
    raw.ff_BlurRadius = v;
    raw.ff_BlurType = v;
    raw.ff_AutoClipControl = v;
    raw.ff_clipControl = v;
    raw.exPos = v;
    wavelet.enabled = v;
    wavelet.strength = v;
    wavelet.balance = v;
    wavelet.iter = v;
    wavelet.median = v;
    wavelet.medianlev = v;
    wavelet.linkedg = v;
    wavelet.cbenab = v;
    wavelet.greenhigh = v;
    wavelet.greenmed = v;
    wavelet.greenlow = v;
    wavelet.bluehigh = v;
    wavelet.bluemed = v;
    wavelet.bluelow = v;
    wavelet.lipst = v;
    wavelet.Medgreinf = v;
    wavelet.avoid = v;
    wavelet.tmr = v;
    wavelet.Lmethod = v;
    wavelet.CLmethod = v;
    wavelet.Backmethod = v;
    wavelet.Tilesmethod = v;
    wavelet.daubcoeffmethod = v;
    wavelet.CHmethod = v;
    wavelet.CHSLmethod = v;
    wavelet.EDmethod = v;
    wavelet.NPmethod = v;
    wavelet.BAmethod = v;
    wavelet.TMmethod = v;
    wavelet.HSmethod = v;
    wavelet.Dirmethod = v;
    wavelet.rescon = v;
    wavelet.resconH = v;
    wavelet.reschro = v;
    wavelet.tmrs = v;
    wavelet.gamma = v;
    wavelet.sup = v;
    wavelet.sky = v;
    wavelet.thres = v;
    wavelet.threshold = v;
    wavelet.threshold2 = v;
    wavelet.edgedetect = v;
    wavelet.edgedetectthr = v;
    wavelet.edgedetectthr2 = v;
    wavelet.edgesensi = v;
    wavelet.edgeampli = v;
    wavelet.chroma = v;
    wavelet.chro = v;
    wavelet.contrast = v;
    wavelet.edgrad = v;
    wavelet.edgval = v;
    wavelet.edgthresh = v;
    wavelet.thr = v;
    wavelet.thrH = v;
    wavelet.skinprotect = v;
    wavelet.hueskin = v;
    wavelet.hueskin2 = v;
    wavelet.hllev = v;
    wavelet.bllev = v;
    wavelet.edgcont = v;
    wavelet.level0noise = v;
    wavelet.level1noise = v;
    wavelet.level2noise = v;
    wavelet.level3noise = v;
    wavelet.ccwcurve = v;
    wavelet.opacityCurveRG   = v;
    wavelet.opacityCurveBY   = v;
    wavelet.opacityCurveW   = v;
    wavelet.opacityCurveWL   = v;
    wavelet.hhcurve     = v;
    wavelet.Chcurve     = v;
    wavelet.wavclCurve     = v;

    wavelet.pastlev = v;
    wavelet.satlev = v;
//  wavelet.enacont = v;
//  wavelet.enachrom = v;
//  wavelet.enaedge = v;
//  wavelet.enares = v;
    wavelet.expfinal = v;
    wavelet.expcontrast = v;
    wavelet.expchroma = v;
    wavelet.expedge = v;
    wavelet.expresid = v;
    wavelet.exptoning = v;
    wavelet.expnoise = v;

    for (int i = 0; i < 9; i++) {
        wavelet.c[i] = v;
    }

    for (int i = 0; i < 9; i++) {
        wavelet.ch[i] = v;
    }

    dirpyrequalizer.enabled = v;
    dirpyrequalizer.gamutlab = v;
    dirpyrequalizer.cbdlMethod = v;


    for (int i = 0; i < 6; i++) {
        dirpyrequalizer.mult[i] = v;
    }

    dirpyrequalizer.threshold = v;
    dirpyrequalizer.skinprotect = v;
    dirpyrequalizer.hueskin = v;
    //dirpyrequalizer.algo = v;
    hsvequalizer.enabled = v;
    hsvequalizer.hcurve = v;
    hsvequalizer.scurve = v;
    hsvequalizer.vcurve = v;
    filmSimulation.enabled = v;
    filmSimulation.clutFilename = v;
    filmSimulation.strength = v;
    softlight.enabled = v;
    softlight.strength = v;
    dehaze.enabled = v;
    dehaze.strength = v;
    dehaze.showDepthMap = v;
    dehaze.depth = v;
    metadata.mode = v;
    filmNegative.enabled = v;
    filmNegative.redRatio = v;
    filmNegative.greenExp = v;
    filmNegative.blueRatio = v;

    exif = v;
    iptc = v;
}

using namespace rtengine;
using namespace rtengine::procparams;

void ParamsEdited::initFrom(const std::vector<rtengine::procparams::ProcParams>& src)
{

    set(true);

    if (src.empty()) {
        return;
    }

    const ProcParams& p = src[0];

    // Resize LocallabSpotEdited according to src[0]
    locallab.spots.clear();
    locallab.spots.resize(p.locallab.nbspot, new LocallabParamsEdited::LocallabSpotEdited(true));

    for (size_t i = 1; i < src.size(); i++) {
        const ProcParams& other = src[i];
        toneCurve.curve = toneCurve.curve && p.toneCurve.curve == other.toneCurve.curve;
        toneCurve.curve2 = toneCurve.curve2 && p.toneCurve.curve2 == other.toneCurve.curve2;
        toneCurve.curveMode = toneCurve.curveMode && p.toneCurve.curveMode == other.toneCurve.curveMode;
        toneCurve.curveMode2 = toneCurve.curveMode2 && p.toneCurve.curveMode2 == other.toneCurve.curveMode2;
        toneCurve.brightness = toneCurve.brightness && p.toneCurve.brightness == other.toneCurve.brightness;
        toneCurve.black = toneCurve.black && p.toneCurve.black == other.toneCurve.black;
        toneCurve.contrast = toneCurve.contrast && p.toneCurve.contrast == other.toneCurve.contrast;
        toneCurve.saturation = toneCurve.saturation && p.toneCurve.saturation == other.toneCurve.saturation;
        toneCurve.shcompr = toneCurve.shcompr && p.toneCurve.shcompr == other.toneCurve.shcompr;
        toneCurve.hlcompr = toneCurve.hlcompr && p.toneCurve.hlcompr == other.toneCurve.hlcompr;
        toneCurve.hlcomprthresh = toneCurve.hlcomprthresh && p.toneCurve.hlcomprthresh == other.toneCurve.hlcomprthresh;
        toneCurve.autoexp = toneCurve.autoexp && p.toneCurve.autoexp == other.toneCurve.autoexp;
        toneCurve.clip = toneCurve.clip && p.toneCurve.clip == other.toneCurve.clip;
        toneCurve.expcomp = toneCurve.expcomp && p.toneCurve.expcomp == other.toneCurve.expcomp;
        toneCurve.hrenabled = toneCurve.hrenabled && p.toneCurve.hrenabled == other.toneCurve.hrenabled;
        toneCurve.method = toneCurve.method && p.toneCurve.method == other.toneCurve.method;
        toneCurve.histmatching = toneCurve.histmatching && p.toneCurve.histmatching == other.toneCurve.histmatching;
        toneCurve.fromHistMatching = toneCurve.fromHistMatching && p.toneCurve.fromHistMatching == other.toneCurve.fromHistMatching;
        toneCurve.clampOOG = toneCurve.clampOOG && p.toneCurve.clampOOG == other.toneCurve.clampOOG;
        retinex.cdcurve = retinex.cdcurve && p.retinex.cdcurve == other.retinex.cdcurve;
        retinex.mapcurve = retinex.mapcurve && p.retinex.mapcurve == other.retinex.mapcurve;
        retinex.cdHcurve = retinex.cdHcurve && p.retinex.cdHcurve == other.retinex.cdHcurve;
        retinex.lhcurve = retinex.lhcurve && p.retinex.lhcurve == other.retinex.lhcurve;
        retinex.transmissionCurve = retinex.transmissionCurve && p.retinex.transmissionCurve == other.retinex.transmissionCurve;
        retinex.gaintransmissionCurve = retinex.gaintransmissionCurve && p.retinex.gaintransmissionCurve == other.retinex.gaintransmissionCurve;
        retinex.retinexMethod = retinex.retinexMethod && p.retinex.retinexMethod == other.retinex.retinexMethod;
        retinex.mapMethod = retinex.mapMethod && p.retinex.mapMethod == other.retinex.mapMethod;
        retinex.viewMethod = retinex.viewMethod && p.retinex.viewMethod == other.retinex.viewMethod;
        retinex.retinexcolorspace = retinex.retinexcolorspace && p.retinex.retinexcolorspace == other.retinex.retinexcolorspace;
        retinex.gammaretinex = retinex.gammaretinex && p.retinex.gammaretinex == other.retinex.gammaretinex;
        retinex.str = retinex.str && p.retinex.str == other.retinex.str;
        retinex.scal = retinex.scal && p.retinex.scal == other.retinex.scal;
        retinex.iter = retinex.iter && p.retinex.iter == other.retinex.iter;
        retinex.grad = retinex.grad && p.retinex.grad == other.retinex.grad;
        retinex.grads = retinex.grads && p.retinex.grads == other.retinex.grads;
        retinex.gam = retinex.gam && p.retinex.gam == other.retinex.gam;
        retinex.slope = retinex.slope && p.retinex.slope == other.retinex.slope;
        retinex.neigh = retinex.neigh && p.retinex.neigh == other.retinex.neigh;
        retinex.offs = retinex.offs && p.retinex.offs == other.retinex.offs;
        retinex.vart = retinex.vart && p.retinex.vart == other.retinex.vart;
        retinex.limd = retinex.limd && p.retinex.limd == other.retinex.limd;
        retinex.highl = retinex.highl && p.retinex.highl == other.retinex.highl;
        retinex.skal = retinex.skal && p.retinex.skal == other.retinex.skal;
        retinex.medianmap = retinex.medianmap && p.retinex.medianmap == other.retinex.medianmap;
        retinex.highlights = retinex.highlights && p.retinex.highlights == other.retinex.highlights;
        retinex.htonalwidth = retinex.htonalwidth && p.retinex.htonalwidth == other.retinex.htonalwidth;
        retinex.shadows = retinex.shadows && p.retinex.shadows == other.retinex.shadows;
        retinex.stonalwidth = retinex.stonalwidth && p.retinex.stonalwidth == other.retinex.stonalwidth;
        retinex.radius = retinex.radius && p.retinex.radius == other.retinex.radius;

        retinex.enabled = retinex.enabled && p.retinex.enabled == other.retinex.enabled;
        labCurve.enabled = labCurve.enabled && p.labCurve.enabled == other.labCurve.enabled;
        labCurve.lcurve = labCurve.lcurve && p.labCurve.lcurve == other.labCurve.lcurve;
        labCurve.acurve = labCurve.acurve && p.labCurve.acurve == other.labCurve.acurve;
        labCurve.bcurve = labCurve.bcurve && p.labCurve.bcurve == other.labCurve.bcurve;
        labCurve.cccurve = labCurve.cccurve && p.labCurve.cccurve == other.labCurve.cccurve;
        labCurve.chcurve = labCurve.chcurve && p.labCurve.chcurve == other.labCurve.chcurve;
        labCurve.lhcurve = labCurve.lhcurve && p.labCurve.lhcurve == other.labCurve.lhcurve;
        labCurve.hhcurve = labCurve.hhcurve && p.labCurve.hhcurve == other.labCurve.hhcurve;
        labCurve.lccurve = labCurve.lccurve && p.labCurve.lccurve == other.labCurve.lccurve;
        labCurve.clcurve = labCurve.clcurve && p.labCurve.clcurve == other.labCurve.clcurve;
        labCurve.brightness = labCurve.brightness && p.labCurve.brightness == other.labCurve.brightness;
        labCurve.contrast = labCurve.contrast && p.labCurve.contrast == other.labCurve.contrast;
        labCurve.chromaticity = labCurve.chromaticity && p.labCurve.chromaticity == other.labCurve.chromaticity;
        labCurve.avoidcolorshift = labCurve.avoidcolorshift && p.labCurve.avoidcolorshift == other.labCurve.avoidcolorshift;
        labCurve.rstprotection = labCurve.rstprotection && p.labCurve.rstprotection == other.labCurve.rstprotection;
        labCurve.lcredsk = labCurve.lcredsk && p.labCurve.lcredsk == other.labCurve.lcredsk;

        localContrast.enabled = localContrast.enabled && p.localContrast.enabled == other.localContrast.enabled;
        localContrast.radius = localContrast.radius && p.localContrast.radius == other.localContrast.radius;
        localContrast.amount = localContrast.amount && p.localContrast.amount == other.localContrast.amount;
        localContrast.darkness = localContrast.darkness && p.localContrast.darkness == other.localContrast.darkness;
        localContrast.lightness = localContrast.lightness && p.localContrast.lightness == other.localContrast.lightness;

        rgbCurves.enabled = rgbCurves.enabled && p.rgbCurves.enabled == other.rgbCurves.enabled;
        rgbCurves.lumamode = rgbCurves.lumamode && p.rgbCurves.lumamode == other.rgbCurves.lumamode;
        rgbCurves.rcurve = rgbCurves.rcurve && p.rgbCurves.rcurve == other.rgbCurves.rcurve;
        rgbCurves.gcurve = rgbCurves.gcurve && p.rgbCurves.gcurve == other.rgbCurves.gcurve;
        rgbCurves.bcurve = rgbCurves.bcurve && p.rgbCurves.bcurve == other.rgbCurves.bcurve;
        colorToning.enabled = colorToning.enabled && p.colorToning.enabled == other.colorToning.enabled;
        colorToning.twocolor = colorToning.twocolor && p.colorToning.twocolor == other.colorToning.twocolor;
        colorToning.opacityCurve = colorToning.opacityCurve && p.colorToning.opacityCurve == other.colorToning.opacityCurve;
        colorToning.colorCurve = colorToning.colorCurve && p.colorToning.colorCurve == other.colorToning.colorCurve;
        colorToning.autosat = colorToning.autosat && p.colorToning.autosat == other.colorToning.autosat;
        colorToning.satprotectionthreshold = colorToning.satprotectionthreshold && p.colorToning.satProtectionThreshold == other.colorToning.satProtectionThreshold;
        colorToning.saturatedopacity = colorToning.saturatedopacity && p.colorToning.saturatedOpacity == other.colorToning.saturatedOpacity;
        colorToning.strength = colorToning.strength && p.colorToning.strength == other.colorToning.strength;
        colorToning.shadowsColSat = colorToning.shadowsColSat && p.colorToning.shadowsColSat == other.colorToning.shadowsColSat;
        colorToning.hlColSat = colorToning.hlColSat && p.colorToning.hlColSat == other.colorToning.hlColSat;
        colorToning.balance = colorToning.balance && p.colorToning.balance == other.colorToning.balance;
        colorToning.clcurve = colorToning.clcurve && p.colorToning.clcurve == other.colorToning.clcurve;
        colorToning.cl2curve = colorToning.cl2curve && p.colorToning.cl2curve == other.colorToning.cl2curve;
        colorToning.method = colorToning.method && p.colorToning.method == other.colorToning.method;
        colorToning.redlow = colorToning.redlow && p.colorToning.redlow == other.colorToning.redlow;
        colorToning.greenlow = colorToning.greenlow && p.colorToning.greenlow == other.colorToning.greenlow;
        colorToning.bluelow = colorToning.bluelow && p.colorToning.bluelow == other.colorToning.bluelow;
        colorToning.satlow = colorToning.satlow && p.colorToning.satlow == other.colorToning.satlow;
        colorToning.sathigh = colorToning.sathigh && p.colorToning.sathigh == other.colorToning.sathigh;
        colorToning.redmed = colorToning.redmed && p.colorToning.redmed == other.colorToning.redmed;
        colorToning.greenmed = colorToning.greenmed && p.colorToning.greenmed == other.colorToning.greenmed;
        colorToning.bluemed = colorToning.bluemed && p.colorToning.bluemed == other.colorToning.bluemed;
        colorToning.redhigh = colorToning.redhigh && p.colorToning.redhigh == other.colorToning.redhigh;
        colorToning.greenhigh = colorToning.greenhigh && p.colorToning.greenhigh == other.colorToning.greenhigh;
        colorToning.bluehigh = colorToning.bluehigh && p.colorToning.bluehigh == other.colorToning.bluehigh;
        colorToning.lumamode = colorToning.lumamode && p.colorToning.lumamode == other.colorToning.lumamode;
        colorToning.labgridALow = colorToning.labgridALow && p.colorToning.labgridALow == other.colorToning.labgridALow;
        colorToning.labgridBLow = colorToning.labgridBLow && p.colorToning.labgridBLow == other.colorToning.labgridBLow;
        colorToning.labgridAHigh = colorToning.labgridAHigh && p.colorToning.labgridAHigh == other.colorToning.labgridAHigh;
        colorToning.labgridBHigh = colorToning.labgridBHigh && p.colorToning.labgridBHigh == other.colorToning.labgridBHigh;
        colorToning.labregions = colorToning.labregions && p.colorToning.labregions == other.colorToning.labregions;
        colorToning.labregionsShowMask = colorToning.labregionsShowMask && p.colorToning.labregionsShowMask == other.colorToning.labregionsShowMask;
        sharpenEdge.enabled = sharpenEdge.enabled && p.sharpenEdge.enabled == other.sharpenEdge.enabled;
        sharpenEdge.passes = sharpenEdge.passes && p.sharpenEdge.passes == other.sharpenEdge.passes;
        sharpenEdge.amount = sharpenEdge.amount && p.sharpenEdge.amount == other.sharpenEdge.amount;
        sharpenEdge.threechannels = sharpenEdge.threechannels && p.sharpenEdge.threechannels == other.sharpenEdge.threechannels;
        sharpenMicro.enabled = sharpenMicro.enabled && p.sharpenMicro.enabled == other.sharpenMicro.enabled;
        sharpenMicro.matrix = sharpenMicro.matrix && p.sharpenMicro.matrix == other.sharpenMicro.matrix;
        sharpenMicro.amount = sharpenMicro.amount && p.sharpenMicro.amount == other.sharpenMicro.amount;
        sharpenMicro.contrast = sharpenMicro.contrast && p.sharpenMicro.contrast == other.sharpenMicro.contrast;
        sharpenMicro.uniformity = sharpenMicro.uniformity && p.sharpenMicro.uniformity == other.sharpenMicro.uniformity;
        sharpening.enabled = sharpening.enabled && p.sharpening.enabled == other.sharpening.enabled;
        sharpening.contrast = sharpening.contrast && p.sharpening.contrast == other.sharpening.contrast;
        sharpening.radius = sharpening.radius && p.sharpening.radius == other.sharpening.radius;
        sharpening.blurradius = sharpening.blurradius && p.sharpening.blurradius == other.sharpening.blurradius;
        sharpening.amount = sharpening.amount && p.sharpening.amount == other.sharpening.amount;
        sharpening.threshold = sharpening.threshold && p.sharpening.threshold == other.sharpening.threshold;
        sharpening.edgesonly = sharpening.edgesonly && p.sharpening.edgesonly == other.sharpening.edgesonly;
        sharpening.edges_radius = sharpening.edges_radius && p.sharpening.edges_radius == other.sharpening.edges_radius;
        sharpening.edges_tolerance = sharpening.edges_tolerance && p.sharpening.edges_tolerance == other.sharpening.edges_tolerance;
        sharpening.halocontrol = sharpening.halocontrol && p.sharpening.halocontrol == other.sharpening.halocontrol;
        sharpening.halocontrol_amount = sharpening.halocontrol_amount && p.sharpening.halocontrol_amount == other.sharpening.halocontrol_amount;
        sharpening.method = sharpening.method && p.sharpening.method == other.sharpening.method;
        sharpening.deconvamount = sharpening.deconvamount && p.sharpening.deconvamount == other.sharpening.deconvamount;
        sharpening.deconvradius = sharpening.deconvradius && p.sharpening.deconvradius == other.sharpening.deconvradius;
        sharpening.deconviter = sharpening.deconviter && p.sharpening.deconviter == other.sharpening.deconviter;
        sharpening.deconvdamping = sharpening.deconvdamping && p.sharpening.deconvdamping == other.sharpening.deconvdamping;
        prsharpening.enabled = prsharpening.enabled && p.prsharpening.enabled == other.prsharpening.enabled;
        prsharpening.contrast = prsharpening.contrast && p.prsharpening.contrast == other.prsharpening.contrast;
        prsharpening.radius = prsharpening.radius && p.prsharpening.radius == other.prsharpening.radius;
        prsharpening.amount = prsharpening.amount && p.prsharpening.amount == other.prsharpening.amount;
        prsharpening.threshold = prsharpening.threshold && p.prsharpening.threshold == other.prsharpening.threshold;
        prsharpening.edgesonly = prsharpening.edgesonly && p.prsharpening.edgesonly == other.prsharpening.edgesonly;
        prsharpening.edges_radius = prsharpening.edges_radius && p.prsharpening.edges_radius == other.prsharpening.edges_radius;
        prsharpening.edges_tolerance = prsharpening.edges_tolerance && p.prsharpening.edges_tolerance == other.prsharpening.edges_tolerance;
        prsharpening.halocontrol = prsharpening.halocontrol && p.prsharpening.halocontrol == other.prsharpening.halocontrol;
        prsharpening.halocontrol_amount = prsharpening.halocontrol_amount && p.prsharpening.halocontrol_amount == other.prsharpening.halocontrol_amount;
        prsharpening.method = prsharpening.method && p.prsharpening.method == other.prsharpening.method;
        prsharpening.deconvamount = prsharpening.deconvamount && p.prsharpening.deconvamount == other.prsharpening.deconvamount;
        prsharpening.deconvradius = prsharpening.deconvradius && p.prsharpening.deconvradius == other.prsharpening.deconvradius;
        prsharpening.deconviter = prsharpening.deconviter && p.prsharpening.deconviter == other.prsharpening.deconviter;
        prsharpening.deconvdamping = prsharpening.deconvdamping && p.prsharpening.deconvdamping == other.prsharpening.deconvdamping;
        vibrance.enabled = vibrance.enabled && p.vibrance.enabled == other.vibrance.enabled;
        vibrance.pastels = vibrance.pastels && p.vibrance.pastels == other.vibrance.pastels;
        vibrance.saturated = vibrance.saturated && p.vibrance.saturated == other.vibrance.saturated;
        vibrance.psthreshold = vibrance.psthreshold && p.vibrance.psthreshold == other.vibrance.psthreshold;
        vibrance.protectskins = vibrance.protectskins && p.vibrance.protectskins == other.vibrance.protectskins;
        vibrance.avoidcolorshift = vibrance.avoidcolorshift && p.vibrance.avoidcolorshift == other.vibrance.avoidcolorshift;
        vibrance.pastsattog = vibrance.pastsattog && p.vibrance.pastsattog == other.vibrance.pastsattog;
        vibrance.skintonescurve = vibrance.skintonescurve && p.vibrance.skintonescurve == other.vibrance.skintonescurve;

        colorappearance.enabled = colorappearance.enabled && p.colorappearance.enabled == other.colorappearance.enabled;
        colorappearance.degree = colorappearance.degree && p.colorappearance.degree == other.colorappearance.degree;
        colorappearance.autodegree = colorappearance.autodegree && p.colorappearance.autodegree == other.colorappearance.autodegree;
        colorappearance.degreeout = colorappearance.degreeout && p.colorappearance.degreeout == other.colorappearance.degreeout;
        colorappearance.autodegreeout = colorappearance.autodegreeout && p.colorappearance.autodegreeout == other.colorappearance.autodegreeout;
        colorappearance.surround = colorappearance.surround && p.colorappearance.surround == other.colorappearance.surround;
        colorappearance.surrsrc = colorappearance.surrsrc && p.colorappearance.surrsrc == other.colorappearance.surrsrc;
        colorappearance.adapscen = colorappearance.adapscen && p.colorappearance.adapscen == other.colorappearance.adapscen;
        colorappearance.autoadapscen = colorappearance.autoadapscen && p.colorappearance.autoadapscen == other.colorappearance.autoadapscen;
        colorappearance.ybscen = colorappearance.ybscen && p.colorappearance.ybscen == other.colorappearance.ybscen;
        colorappearance.autoybscen = colorappearance.autoybscen && p.colorappearance.autoybscen == other.colorappearance.autoybscen;
        colorappearance.adaplum = colorappearance.adaplum && p.colorappearance.adaplum == other.colorappearance.adaplum;
        colorappearance.badpixsl = colorappearance.badpixsl && p.colorappearance.badpixsl == other.colorappearance.badpixsl;
        colorappearance.wbmodel = colorappearance.wbmodel && p.colorappearance.wbmodel == other.colorappearance.wbmodel;
        colorappearance.algo = colorappearance.algo && p.colorappearance.algo == other.colorappearance.algo;
        colorappearance.jlight = colorappearance.jlight && p.colorappearance.jlight == other.colorappearance.jlight;
        colorappearance.qbright = colorappearance.qbright && p.colorappearance.qbright == other.colorappearance.qbright;
        colorappearance.chroma = colorappearance.chroma && p.colorappearance.chroma == other.colorappearance.chroma;
        colorappearance.schroma = colorappearance.schroma && p.colorappearance.schroma == other.colorappearance.schroma;
        colorappearance.mchroma = colorappearance.mchroma && p.colorappearance.mchroma == other.colorappearance.mchroma;
        colorappearance.rstprotection = colorappearance.rstprotection && p.colorappearance.rstprotection == other.colorappearance.rstprotection;
        colorappearance.contrast = colorappearance.contrast && p.colorappearance.contrast == other.colorappearance.contrast;
        colorappearance.qcontrast = colorappearance.qcontrast && p.colorappearance.qcontrast == other.colorappearance.qcontrast;
        colorappearance.colorh = colorappearance.colorh && p.colorappearance.colorh == other.colorappearance.colorh;
        colorappearance.surrsource = colorappearance.surrsource && p.colorappearance.surrsource == other.colorappearance.surrsource;
        colorappearance.gamut = colorappearance.gamut && p.colorappearance.gamut == other.colorappearance.gamut;
//       colorappearance.badpix = colorappearance.badpix && p.colorappearance.badpix == other.colorappearance.badpix;
        colorappearance.datacie = colorappearance.datacie && p.colorappearance.datacie == other.colorappearance.datacie;
        colorappearance.tonecie = colorappearance.tonecie && p.colorappearance.tonecie == other.colorappearance.tonecie;
        //     colorappearance.sharpcie = colorappearance.sharpcie && p.colorappearance.sharpcie == other.colorappearance.sharpcie;
        colorappearance.curve = colorappearance.curve && p.colorappearance.curve == other.colorappearance.curve;
        colorappearance.curve3 = colorappearance.curve3 && p.colorappearance.curve3 == other.colorappearance.curve3;
        colorappearance.curve2 = colorappearance.curve2 && p.colorappearance.curve2 == other.colorappearance.curve2;
        colorappearance.curveMode = colorappearance.curveMode && p.colorappearance.curveMode == other.colorappearance.curveMode;
        colorappearance.curveMode2 = colorappearance.curveMode2 && p.colorappearance.curveMode2 == other.colorappearance.curveMode2;
        colorappearance.curveMode3 = colorappearance.curveMode3 && p.colorappearance.curveMode3 == other.colorappearance.curveMode3;
        colorappearance.tempout = colorappearance.tempout && p.colorappearance.tempout == other.colorappearance.tempout;
        colorappearance.greenout = colorappearance.greenout && p.colorappearance.greenout == other.colorappearance.greenout;
        colorappearance.ybout = colorappearance.ybout && p.colorappearance.ybout == other.colorappearance.ybout;
        colorappearance.tempsc = colorappearance.tempsc && p.colorappearance.tempsc == other.colorappearance.tempsc;
        colorappearance.greensc = colorappearance.greensc && p.colorappearance.greensc == other.colorappearance.greensc;

        //colorBoost.amount = colorBoost.amount && p.colorBoost.amount == other.colorBoost.amount;
        //colorBoost.avoidclip = colorBoost.avoidclip && p.colorBoost.avoidclip == other.colorBoost.avoidclip;
        //colorBoost.enable_saturationlimiter = colorBoost.enable_saturationlimiter && p.colorBoost.enable_saturationlimiter == other.colorBoost.enable_saturationlimiter;
        //colorBoost.saturationlimit = colorBoost.saturationlimit && p.colorBoost.saturationlimit == other.colorBoost.saturationlimit;
        wb.enabled = wb.enabled && p.wb.enabled == other.wb.enabled;
        wb.method = wb.method && p.wb.method == other.wb.method;
        wb.green = wb.green && p.wb.green == other.wb.green;
        wb.equal = wb.equal && p.wb.equal == other.wb.equal;
        wb.temperature = wb.temperature && p.wb.temperature == other.wb.temperature;
        wb.tempBias = wb.tempBias && p.wb.tempBias == other.wb.tempBias;
        //colorShift.a = colorShift.a && p.colorShift.a == other.colorShift.a;
        //colorShift.b = colorShift.b && p.colorShift.b == other.colorShift.b;
        //lumaDenoise.enabled = lumaDenoise.enabled && p.lumaDenoise.enabled == other.lumaDenoise.enabled;
        //lumaDenoise.radius = lumaDenoise.radius && p.lumaDenoise.radius == other.lumaDenoise.radius;
        //lumaDenoise.edgetolerance = lumaDenoise.edgetolerance && p.lumaDenoise.edgetolerance == other.lumaDenoise.edgetolerance;
        //colorDenoise.enabled = colorDenoise.enabled && p.colorDenoise.enabled == other.colorDenoise.enabled;
        //colorDenoise.amount = colorDenoise.amount && p.colorDenoise.amount == other.colorDenoise.amount;
        defringe.enabled = defringe.enabled && p.defringe.enabled == other.defringe.enabled;
        defringe.radius = defringe.radius && p.defringe.radius == other.defringe.radius;
        defringe.threshold = defringe.threshold && p.defringe.threshold == other.defringe.threshold;
        defringe.huecurve = defringe.huecurve && p.defringe.huecurve == other.defringe.huecurve;

        impulseDenoise.enabled = impulseDenoise.enabled && p.impulseDenoise.enabled == other.impulseDenoise.enabled;
        impulseDenoise.thresh = impulseDenoise.thresh && p.impulseDenoise.thresh == other.impulseDenoise.thresh;

        dirpyrDenoise.enabled = dirpyrDenoise.enabled && p.dirpyrDenoise.enabled == other.dirpyrDenoise.enabled;
        dirpyrDenoise.enhance = dirpyrDenoise.enhance && p.dirpyrDenoise.enhance == other.dirpyrDenoise.enhance;
        dirpyrDenoise.median = dirpyrDenoise.median && p.dirpyrDenoise.median == other.dirpyrDenoise.median;
//       dirpyrDenoise.perform = dirpyrDenoise.perform && p.dirpyrDenoise.perform == other.dirpyrDenoise.perform;
        dirpyrDenoise.luma = dirpyrDenoise.luma && p.dirpyrDenoise.luma == other.dirpyrDenoise.luma;
        dirpyrDenoise.lcurve = dirpyrDenoise.lcurve && p.dirpyrDenoise.lcurve == other.dirpyrDenoise.lcurve;
        dirpyrDenoise.cccurve = dirpyrDenoise.cccurve && p.dirpyrDenoise.cccurve == other.dirpyrDenoise.cccurve;
        dirpyrDenoise.Ldetail = dirpyrDenoise.Ldetail && p.dirpyrDenoise.Ldetail == other.dirpyrDenoise.Ldetail;
        dirpyrDenoise.chroma = dirpyrDenoise.chroma && p.dirpyrDenoise.chroma == other.dirpyrDenoise.chroma;
        dirpyrDenoise.redchro = dirpyrDenoise.redchro && p.dirpyrDenoise.redchro == other.dirpyrDenoise.redchro;
        dirpyrDenoise.bluechro = dirpyrDenoise.bluechro && p.dirpyrDenoise.bluechro == other.dirpyrDenoise.bluechro;
        dirpyrDenoise.gamma = dirpyrDenoise.gamma && p.dirpyrDenoise.gamma == other.dirpyrDenoise.gamma;
        dirpyrDenoise.passes = dirpyrDenoise.passes && p.dirpyrDenoise.passes == other.dirpyrDenoise.passes;
        dirpyrDenoise.dmethod = dirpyrDenoise.dmethod && p.dirpyrDenoise.dmethod == other.dirpyrDenoise.dmethod;
        dirpyrDenoise.Lmethod = dirpyrDenoise.Lmethod && p.dirpyrDenoise.Lmethod == other.dirpyrDenoise.Lmethod;
        dirpyrDenoise.Cmethod = dirpyrDenoise.Cmethod && p.dirpyrDenoise.Cmethod == other.dirpyrDenoise.Cmethod;
        dirpyrDenoise.C2method = dirpyrDenoise.C2method && p.dirpyrDenoise.C2method == other.dirpyrDenoise.C2method;
        dirpyrDenoise.smethod = dirpyrDenoise.smethod && p.dirpyrDenoise.smethod == other.dirpyrDenoise.smethod;
        dirpyrDenoise.medmethod = dirpyrDenoise.medmethod && p.dirpyrDenoise.medmethod == other.dirpyrDenoise.medmethod;
        dirpyrDenoise.methodmed = dirpyrDenoise.methodmed && p.dirpyrDenoise.methodmed == other.dirpyrDenoise.methodmed;
        dirpyrDenoise.rgbmethod = dirpyrDenoise.rgbmethod && p.dirpyrDenoise.rgbmethod == other.dirpyrDenoise.rgbmethod;

        epd.enabled = epd.enabled && p.epd.enabled == other.epd.enabled;
        epd.strength = epd.strength && p.epd.strength == other.epd.strength;
        epd.gamma = epd.gamma && p.epd.gamma == other.epd.gamma;
        epd.edgeStopping = epd.edgeStopping && p.epd.edgeStopping == other.epd.edgeStopping;
        epd.scale = epd.scale && p.epd.scale == other.epd.scale;
        epd.reweightingIterates = epd.reweightingIterates && p.epd.reweightingIterates == other.epd.reweightingIterates;

        fattal.enabled = fattal.enabled && p.fattal.enabled == other.fattal.enabled;
        fattal.threshold = fattal.threshold && p.fattal.threshold == other.fattal.threshold;
        fattal.amount = fattal.amount && p.fattal.amount == other.fattal.amount;
        fattal.anchor = fattal.anchor && p.fattal.anchor == other.fattal.anchor;

        sh.enabled = sh.enabled && p.sh.enabled == other.sh.enabled;
        sh.highlights = sh.highlights && p.sh.highlights == other.sh.highlights;
        sh.htonalwidth = sh.htonalwidth && p.sh.htonalwidth == other.sh.htonalwidth;
        sh.shadows = sh.shadows && p.sh.shadows == other.sh.shadows;
        sh.stonalwidth = sh.stonalwidth && p.sh.stonalwidth == other.sh.stonalwidth;
        sh.radius = sh.radius && p.sh.radius == other.sh.radius;
        sh.lab = sh.lab && p.sh.lab == other.sh.lab;
        crop.enabled = crop.enabled && p.crop.enabled == other.crop.enabled;
        crop.x = crop.x && p.crop.x == other.crop.x;
        crop.y = crop.y && p.crop.y == other.crop.y;
        crop.w = crop.w && p.crop.w == other.crop.w;
        crop.h = crop.h && p.crop.h == other.crop.h;
        crop.fixratio = crop.fixratio && p.crop.fixratio == other.crop.fixratio;
        crop.ratio = crop.ratio && p.crop.ratio == other.crop.ratio;
        crop.orientation = crop.orientation && p.crop.orientation == other.crop.orientation;
        crop.guide = crop.guide && p.crop.guide == other.crop.guide;
        coarse.rotate = coarse.rotate && p.coarse.rotate == other.coarse.rotate;
        coarse.hflip = coarse.hflip && p.coarse.hflip == other.coarse.hflip;
        coarse.vflip = coarse.vflip && p.coarse.vflip == other.coarse.vflip;
        commonTrans.autofill = commonTrans.autofill && p.commonTrans.autofill == other.commonTrans.autofill;
        rotate.degree = rotate.degree && p.rotate.degree == other.rotate.degree;
        distortion.amount = distortion.amount && p.distortion.amount == other.distortion.amount;
        lensProf.lcMode = lensProf.lcMode && p.lensProf.lcMode == other.lensProf.lcMode;
        lensProf.lcpFile = lensProf.lcpFile && p.lensProf.lcpFile == other.lensProf.lcpFile;
        lensProf.useDist = lensProf.useDist && p.lensProf.useDist == other.lensProf.useDist;
        lensProf.useVign = lensProf.useVign && p.lensProf.useVign == other.lensProf.useVign;
        lensProf.useCA = lensProf.useCA && p.lensProf.useCA == other.lensProf.useCA;
        lensProf.useLensfun = lensProf.useLensfun && p.lensProf.useLensfun() == other.lensProf.useLensfun();
        lensProf.lfAutoMatch = lensProf.lfAutoMatch && p.lensProf.lfAutoMatch() == other.lensProf.lfAutoMatch();
        lensProf.lfCameraMake = lensProf.lfCameraMake && p.lensProf.lfCameraMake == other.lensProf.lfCameraMake;
        lensProf.lfCameraModel = lensProf.lfCameraModel && p.lensProf.lfCameraModel == other.lensProf.lfCameraModel;
        lensProf.lfLens = lensProf.lfLens && p.lensProf.lfLens == other.lensProf.lfLens;
        perspective.horizontal = perspective.horizontal && p.perspective.horizontal == other.perspective.horizontal;
        perspective.vertical = perspective.vertical && p.perspective.vertical == other.perspective.vertical;
        gradient.enabled = gradient.enabled && p.gradient.enabled == other.gradient.enabled;
        gradient.degree = gradient.degree && p.gradient.degree == other.gradient.degree;
        gradient.feather = gradient.feather && p.gradient.feather == other.gradient.feather;
        gradient.strength = gradient.strength && p.gradient.strength == other.gradient.strength;
        gradient.centerX = gradient.centerX && p.gradient.centerX == other.gradient.centerX;
        gradient.centerY = gradient.centerY && p.gradient.centerY == other.gradient.centerY;

        locallab.enabled = locallab.enabled && p.locallab.enabled == other.locallab.enabled;
        locallab.nbspot = locallab.nbspot && p.locallab.nbspot == other.locallab.nbspot;
        locallab.selspot = locallab.selspot && p.locallab.selspot == other.locallab.selspot;

        if (locallab.nbspot) {
            for (size_t j = 0; j < locallab.spots.size() && j < p.locallab.spots.size() && j < other.locallab.spots.size(); j++) {
                const LocallabParams::LocallabSpot& pSpot = p.locallab.spots.at(j);
                const LocallabParams::LocallabSpot& otherSpot = other.locallab.spots.at(j);
                // Control spot settings
                locallab.id = locallab.id && pSpot.id == otherSpot.id;
                locallab.spots.at(j).name = locallab.spots.at(j).name && pSpot.name == otherSpot.name;
                locallab.spots.at(j).isvisible = locallab.spots.at(j).isvisible && pSpot.isvisible == otherSpot.isvisible;
                locallab.spots.at(j).shape = locallab.spots.at(j).shape && pSpot.shape == otherSpot.shape;
                locallab.spots.at(j).spotMethod = locallab.spots.at(j).spotMethod && pSpot.spotMethod == otherSpot.spotMethod;
                locallab.spots.at(j).sensiexclu = locallab.spots.at(j).sensiexclu && pSpot.sensiexclu == otherSpot.sensiexclu;
                locallab.spots.at(j).structexclu = locallab.spots.at(j).structexclu && pSpot.structexclu == otherSpot.structexclu;
                locallab.spots.at(j).struc = locallab.spots.at(j).struc && pSpot.struc == otherSpot.struc;
                locallab.spots.at(j).shapeMethod = locallab.spots.at(j).shapeMethod && pSpot.shapeMethod == otherSpot.shapeMethod;
                locallab.spots.at(j).locX = locallab.spots.at(j).locX && pSpot.locX == otherSpot.locX;
                locallab.spots.at(j).locXL = locallab.spots.at(j).locXL && pSpot.locXL == otherSpot.locXL;
                locallab.spots.at(j).locY = locallab.spots.at(j).locY && pSpot.locY == otherSpot.locY;
                locallab.spots.at(j).locYT = locallab.spots.at(j).locYT && pSpot.locYT == otherSpot.locYT;
                locallab.spots.at(j).centerX = locallab.spots.at(j).centerX && pSpot.centerX == otherSpot.centerX;
                locallab.spots.at(j).centerY = locallab.spots.at(j).centerY && pSpot.centerY == otherSpot.centerY;
                locallab.spots.at(j).circrad = locallab.spots.at(j).circrad && pSpot.circrad == otherSpot.circrad;
                locallab.spots.at(j).qualityMethod = locallab.spots.at(j).qualityMethod && pSpot.qualityMethod == otherSpot.qualityMethod;
                locallab.spots.at(j).transit = locallab.spots.at(j).transit && pSpot.transit == otherSpot.transit;
                locallab.spots.at(j).thresh = locallab.spots.at(j).thresh && pSpot.thresh == otherSpot.thresh;
                locallab.spots.at(j).iter = locallab.spots.at(j).iter && pSpot.iter == otherSpot.iter;
                locallab.spots.at(j).balan = locallab.spots.at(j).balan && pSpot.balan == otherSpot.balan;
                locallab.spots.at(j).transitweak = locallab.spots.at(j).transitweak && pSpot.transitweak == otherSpot.transitweak;
                locallab.spots.at(j).transitgrad = locallab.spots.at(j).transitgrad && pSpot.transitgrad == otherSpot.transitgrad;
                locallab.spots.at(j).avoid = locallab.spots.at(j).avoid && pSpot.avoid == otherSpot.avoid;
                // Color & Light
                locallab.spots.at(j).expcolor = locallab.spots.at(j).expcolor && pSpot.expcolor == otherSpot.expcolor;
                locallab.spots.at(j).curvactiv = locallab.spots.at(j).curvactiv && pSpot.curvactiv == otherSpot.curvactiv;
                locallab.spots.at(j).lightness = locallab.spots.at(j).lightness && pSpot.lightness == otherSpot.lightness;
                locallab.spots.at(j).contrast = locallab.spots.at(j).contrast && pSpot.contrast == otherSpot.contrast;
                locallab.spots.at(j).chroma = locallab.spots.at(j).chroma && pSpot.chroma == otherSpot.chroma;
                locallab.spots.at(j).labgridALow = locallab.spots.at(j).labgridALow && pSpot.labgridALow == otherSpot.labgridALow;
                locallab.spots.at(j).labgridBLow = locallab.spots.at(j).labgridBLow && pSpot.labgridBLow == otherSpot.labgridBLow;
                locallab.spots.at(j).labgridAHigh = locallab.spots.at(j).labgridAHigh && pSpot.labgridAHigh == otherSpot.labgridAHigh;
                locallab.spots.at(j).labgridBHigh = locallab.spots.at(j).labgridBHigh && pSpot.labgridBHigh == otherSpot.labgridBHigh;
                locallab.spots.at(j).strengthgrid = locallab.spots.at(j).strengthgrid && pSpot.strengthgrid == otherSpot.strengthgrid;
                locallab.spots.at(j).sensi = locallab.spots.at(j).sensi && pSpot.sensi == otherSpot.sensi;
                locallab.spots.at(j).structcol = locallab.spots.at(j).structcol && pSpot.structcol == otherSpot.structcol;
                locallab.spots.at(j).blurcolde = locallab.spots.at(j).blurcolde && pSpot.blurcolde == otherSpot.blurcolde;
                locallab.spots.at(j).blendmaskcol = locallab.spots.at(j).blendmaskcol && pSpot.blendmaskcol == otherSpot.blendmaskcol;
                locallab.spots.at(j).radmaskcol = locallab.spots.at(j).radmaskcol && pSpot.radmaskcol == otherSpot.radmaskcol;
                locallab.spots.at(j).chromaskcol = locallab.spots.at(j).chromaskcol && pSpot.chromaskcol == otherSpot.chromaskcol;
                locallab.spots.at(j).gammaskcol = locallab.spots.at(j).gammaskcol && pSpot.gammaskcol == otherSpot.gammaskcol;
                locallab.spots.at(j).slomaskcol = locallab.spots.at(j).slomaskcol && pSpot.slomaskcol == otherSpot.slomaskcol;
                locallab.spots.at(j).qualitycurveMethod = locallab.spots.at(j).qualitycurveMethod && pSpot.qualitycurveMethod == otherSpot.qualitycurveMethod;
                locallab.spots.at(j).gridMethod = locallab.spots.at(j).gridMethod && pSpot.gridMethod == otherSpot.gridMethod;
                locallab.spots.at(j).llcurve = locallab.spots.at(j).llcurve && pSpot.llcurve == otherSpot.llcurve;
                locallab.spots.at(j).cccurve = locallab.spots.at(j).cccurve && pSpot.cccurve == otherSpot.cccurve;
                locallab.spots.at(j).LHcurve = locallab.spots.at(j).LHcurve && pSpot.LHcurve == otherSpot.LHcurve;
                locallab.spots.at(j).HHcurve = locallab.spots.at(j).HHcurve && pSpot.HHcurve == otherSpot.HHcurve;
                locallab.spots.at(j).invers = locallab.spots.at(j).invers && pSpot.invers == otherSpot.invers;
                locallab.spots.at(j).enaColorMask = locallab.spots.at(j).enaColorMask && pSpot.enaColorMask == otherSpot.enaColorMask;
                locallab.spots.at(j).CCmaskcurve = locallab.spots.at(j).CCmaskcurve && pSpot.CCmaskcurve == otherSpot.CCmaskcurve;
                locallab.spots.at(j).LLmaskcurve = locallab.spots.at(j).LLmaskcurve && pSpot.LLmaskcurve == otherSpot.LLmaskcurve;
                locallab.spots.at(j).HHmaskcurve = locallab.spots.at(j).HHmaskcurve && pSpot.HHmaskcurve == otherSpot.HHmaskcurve;
                locallab.spots.at(j).softradiuscol = locallab.spots.at(j).softradiuscol && pSpot.softradiuscol == otherSpot.softradiuscol;
                // Exposure
                locallab.spots.at(j).expexpose = locallab.spots.at(j).expexpose && pSpot.expexpose == otherSpot.expexpose;
                locallab.spots.at(j).expcomp = locallab.spots.at(j).expcomp && pSpot.expcomp == otherSpot.expcomp;
                locallab.spots.at(j).hlcompr = locallab.spots.at(j).hlcompr && pSpot.hlcompr == otherSpot.hlcompr;
                locallab.spots.at(j).hlcomprthresh = locallab.spots.at(j).hlcomprthresh && pSpot.hlcomprthresh == otherSpot.hlcomprthresh;
                locallab.spots.at(j).black = locallab.spots.at(j).black && pSpot.black == otherSpot.black;
                locallab.spots.at(j).shadex = locallab.spots.at(j).shadex && pSpot.shadex == otherSpot.shadex;
                locallab.spots.at(j).shcompr = locallab.spots.at(j).shcompr && pSpot.shcompr == otherSpot.shcompr;
                locallab.spots.at(j).expchroma = locallab.spots.at(j).expchroma && pSpot.expchroma == otherSpot.expchroma;
                locallab.spots.at(j).warm = locallab.spots.at(j).warm && pSpot.warm == otherSpot.warm;
                locallab.spots.at(j).sensiex = locallab.spots.at(j).sensiex && pSpot.sensiex == otherSpot.sensiex;
                locallab.spots.at(j).structexp = locallab.spots.at(j).structexp && pSpot.structexp == otherSpot.structexp;
                locallab.spots.at(j).blurexpde = locallab.spots.at(j).blurexpde && pSpot.blurexpde == otherSpot.blurexpde;
                locallab.spots.at(j).excurve = locallab.spots.at(j).excurve && pSpot.excurve == otherSpot.excurve;
                locallab.spots.at(j).inversex = locallab.spots.at(j).inversex && pSpot.inversex == otherSpot.inversex;
                locallab.spots.at(j).enaExpMask = locallab.spots.at(j).enaExpMask && pSpot.enaExpMask == otherSpot.enaExpMask;
                locallab.spots.at(j).CCmaskexpcurve = locallab.spots.at(j).CCmaskexpcurve && pSpot.CCmaskexpcurve == otherSpot.CCmaskexpcurve;
                locallab.spots.at(j).LLmaskexpcurve = locallab.spots.at(j).LLmaskexpcurve && pSpot.LLmaskexpcurve == otherSpot.LLmaskexpcurve;
                locallab.spots.at(j).HHmaskexpcurve = locallab.spots.at(j).HHmaskexpcurve && pSpot.HHmaskexpcurve == otherSpot.HHmaskexpcurve;
                locallab.spots.at(j).blendmaskexp = locallab.spots.at(j).blendmaskexp && pSpot.blendmaskexp == otherSpot.blendmaskexp;
                locallab.spots.at(j).radmaskexp = locallab.spots.at(j).radmaskexp && pSpot.radmaskexp == otherSpot.radmaskexp;
                locallab.spots.at(j).chromaskexp = locallab.spots.at(j).chromaskexp && pSpot.chromaskexp == otherSpot.chromaskexp;
                locallab.spots.at(j).gammaskexp = locallab.spots.at(j).gammaskexp && pSpot.gammaskexp == otherSpot.gammaskexp;
                locallab.spots.at(j).slomaskexp = locallab.spots.at(j).slomaskexp && pSpot.slomaskexp == otherSpot.slomaskexp;
                locallab.spots.at(j).softradiusexp = locallab.spots.at(j).softradiusexp && pSpot.softradiusexp == otherSpot.softradiusexp;
                locallab.spots.at(j).expMethod = locallab.spots.at(j).expMethod && pSpot.expMethod == otherSpot.expMethod;
                locallab.spots.at(j).laplacexp = locallab.spots.at(j).laplacexp && pSpot.laplacexp == otherSpot.laplacexp;
                locallab.spots.at(j).balanexp = locallab.spots.at(j).balanexp && pSpot.balanexp == otherSpot.balanexp;
                locallab.spots.at(j).linear = locallab.spots.at(j).linear && pSpot.linear == otherSpot.linear;
                locallab.spots.at(j).gamm = locallab.spots.at(j).gamm && pSpot.gamm == otherSpot.gamm;
                locallab.spots.at(j).fatamount = locallab.spots.at(j).linear && pSpot.fatamount == otherSpot.fatamount;
                locallab.spots.at(j).fatdetail = locallab.spots.at(j).linear && pSpot.fatdetail == otherSpot.fatdetail;
                locallab.spots.at(j).fatanchor = locallab.spots.at(j).linear && pSpot.fatanchor == otherSpot.fatanchor;
                // Shadow highlight
                locallab.spots.at(j).expshadhigh = locallab.spots.at(j).expshadhigh && pSpot.expshadhigh == otherSpot.expshadhigh;
                locallab.spots.at(j).highlights = locallab.spots.at(j).highlights && pSpot.highlights == otherSpot.highlights;
                locallab.spots.at(j).h_tonalwidth = locallab.spots.at(j).h_tonalwidth && pSpot.h_tonalwidth == otherSpot.h_tonalwidth;
                locallab.spots.at(j).shadows = locallab.spots.at(j).shadows && pSpot.shadows == otherSpot.shadows;
                locallab.spots.at(j).s_tonalwidth = locallab.spots.at(j).s_tonalwidth && pSpot.s_tonalwidth == otherSpot.s_tonalwidth;
                locallab.spots.at(j).sh_radius = locallab.spots.at(j).sh_radius && pSpot.sh_radius == otherSpot.sh_radius;
                locallab.spots.at(j).sensihs = locallab.spots.at(j).sensihs && pSpot.sensihs == otherSpot.sensihs;
                locallab.spots.at(j).enaSHMask = locallab.spots.at(j).enaSHMask && pSpot.enaSHMask == otherSpot.enaSHMask;
                locallab.spots.at(j).CCmaskSHcurve = locallab.spots.at(j).CCmaskSHcurve && pSpot.CCmaskSHcurve == otherSpot.CCmaskSHcurve;
                locallab.spots.at(j).LLmaskSHcurve = locallab.spots.at(j).LLmaskSHcurve && pSpot.LLmaskSHcurve == otherSpot.LLmaskSHcurve;
                locallab.spots.at(j).HHmaskSHcurve = locallab.spots.at(j).HHmaskSHcurve && pSpot.HHmaskSHcurve == otherSpot.HHmaskSHcurve;
                locallab.spots.at(j).blendmaskSH = locallab.spots.at(j).blendmaskSH && pSpot.blendmaskSH == otherSpot.blendmaskSH;
                locallab.spots.at(j).radmaskSH = locallab.spots.at(j).radmaskSH && pSpot.radmaskSH == otherSpot.radmaskSH;
                locallab.spots.at(j).blurSHde = locallab.spots.at(j).blurSHde && pSpot.blurSHde == otherSpot.blurSHde;
                locallab.spots.at(j).inverssh = locallab.spots.at(j).inverssh && pSpot.inverssh == otherSpot.inverssh;
                locallab.spots.at(j).chromaskSH = locallab.spots.at(j).chromaskSH && pSpot.chromaskSH == otherSpot.chromaskSH;
                locallab.spots.at(j).gammaskSH = locallab.spots.at(j).gammaskSH && pSpot.gammaskSH == otherSpot.gammaskSH;
                locallab.spots.at(j).slomaskSH = locallab.spots.at(j).slomaskSH && pSpot.slomaskSH == otherSpot.slomaskSH;
                // Vibrance
                locallab.spots.at(j).expvibrance = locallab.spots.at(j).expvibrance && pSpot.expvibrance == otherSpot.expvibrance;
                locallab.spots.at(j).saturated = locallab.spots.at(j).saturated && pSpot.saturated == otherSpot.saturated;
                locallab.spots.at(j).pastels = locallab.spots.at(j).pastels && pSpot.pastels == otherSpot.pastels;
                locallab.spots.at(j).psthreshold = locallab.spots.at(j).psthreshold && pSpot.psthreshold == otherSpot.psthreshold;
                locallab.spots.at(j).protectskins = locallab.spots.at(j).protectskins && pSpot.protectskins == otherSpot.protectskins;
                locallab.spots.at(j).avoidcolorshift = locallab.spots.at(j).avoidcolorshift && pSpot.avoidcolorshift == otherSpot.avoidcolorshift;
                locallab.spots.at(j).pastsattog = locallab.spots.at(j).pastsattog && pSpot.pastsattog == otherSpot.pastsattog;
                locallab.spots.at(j).sensiv = locallab.spots.at(j).sensiv && pSpot.sensiv == otherSpot.sensiv;
                locallab.spots.at(j).skintonescurve = locallab.spots.at(j).skintonescurve && pSpot.skintonescurve == otherSpot.skintonescurve;
                // Soft Light
                locallab.spots.at(j).expsoft = locallab.spots.at(j).expsoft && pSpot.expsoft == otherSpot.expsoft;
                locallab.spots.at(j).streng = locallab.spots.at(j).streng && pSpot.streng == otherSpot.streng;
                locallab.spots.at(j).sensisf = locallab.spots.at(j).sensisf && pSpot.sensisf == otherSpot.sensisf;
                locallab.spots.at(j).laplace = locallab.spots.at(j).laplace && pSpot.laplace == otherSpot.laplace;
                locallab.spots.at(j).softMethod = locallab.spots.at(j).softMethod && pSpot.softMethod == otherSpot.softMethod;
                // Blur & Noise
                locallab.spots.at(j).expblur = locallab.spots.at(j).expblur && pSpot.expblur == otherSpot.expblur;
                locallab.spots.at(j).radius = locallab.spots.at(j).radius && pSpot.radius == otherSpot.radius;
                locallab.spots.at(j).strength = locallab.spots.at(j).strength && pSpot.strength == otherSpot.strength;
                locallab.spots.at(j).sensibn = locallab.spots.at(j).sensibn && pSpot.sensibn == otherSpot.sensibn;
                locallab.spots.at(j).itera = locallab.spots.at(j).itera && pSpot.itera == otherSpot.itera;
                locallab.spots.at(j).guidbl = locallab.spots.at(j).guidbl && pSpot.guidbl == otherSpot.guidbl;
                locallab.spots.at(j).epsbl = locallab.spots.at(j).epsbl && pSpot.epsbl == otherSpot.epsbl;
                locallab.spots.at(j).blMethod = locallab.spots.at(j).blMethod && pSpot.blMethod == otherSpot.blMethod;
                locallab.spots.at(j).blurMethod = locallab.spots.at(j).blurMethod && pSpot.blurMethod == otherSpot.blurMethod;
                locallab.spots.at(j).medMethod = locallab.spots.at(j).medMethod && pSpot.medMethod == otherSpot.medMethod;
                locallab.spots.at(j).activlum = locallab.spots.at(j).activlum && pSpot.activlum == otherSpot.activlum;
                locallab.spots.at(j).CCmaskblcurve = locallab.spots.at(j).CCmaskblcurve && pSpot.CCmaskblcurve == otherSpot.CCmaskblcurve;
                locallab.spots.at(j).LLmaskblcurve = locallab.spots.at(j).LLmaskblcurve && pSpot.LLmaskblcurve == otherSpot.LLmaskblcurve;
                locallab.spots.at(j).HHmaskblcurve = locallab.spots.at(j).HHmaskblcurve && pSpot.HHmaskblcurve == otherSpot.HHmaskblcurve;
                locallab.spots.at(j).enablMask = locallab.spots.at(j).enablMask && pSpot.enablMask == otherSpot.enablMask;
                locallab.spots.at(j).blendmaskbl = locallab.spots.at(j).blendmaskbl && pSpot.blendmaskbl == otherSpot.blendmaskbl;
                locallab.spots.at(j).radmaskbl = locallab.spots.at(j).radmaskbl && pSpot.radmaskbl == otherSpot.radmaskbl;
                locallab.spots.at(j).chromaskbl = locallab.spots.at(j).chromaskbl && pSpot.chromaskbl == otherSpot.chromaskbl;
                locallab.spots.at(j).gammaskbl = locallab.spots.at(j).gammaskbl && pSpot.gammaskbl == otherSpot.gammaskbl;
                locallab.spots.at(j).slomaskbl = locallab.spots.at(j).slomaskbl && pSpot.slomaskbl == otherSpot.slomaskbl;
                // Tone Mapping
                locallab.spots.at(j).exptonemap = locallab.spots.at(j).exptonemap && pSpot.exptonemap == otherSpot.exptonemap;
                locallab.spots.at(j).stren = locallab.spots.at(j).stren && pSpot.stren == otherSpot.stren;
                locallab.spots.at(j).gamma = locallab.spots.at(j).gamma && pSpot.gamma == otherSpot.gamma;
                locallab.spots.at(j).estop = locallab.spots.at(j).estop && pSpot.estop == otherSpot.estop;
                locallab.spots.at(j).scaltm = locallab.spots.at(j).scaltm && pSpot.scaltm == otherSpot.scaltm;
                locallab.spots.at(j).rewei = locallab.spots.at(j).rewei && pSpot.rewei == otherSpot.rewei;
                locallab.spots.at(j).satur = locallab.spots.at(j).satur && pSpot.satur == otherSpot.satur;
                locallab.spots.at(j).sensitm = locallab.spots.at(j).sensitm && pSpot.sensitm == otherSpot.sensitm;
                locallab.spots.at(j).softradiustm = locallab.spots.at(j).softradiustm && pSpot.softradiustm == otherSpot.softradiustm;
                locallab.spots.at(j).amount = locallab.spots.at(j).amount && pSpot.amount == otherSpot.amount;
                locallab.spots.at(j).equiltm = locallab.spots.at(j).equiltm && pSpot.equiltm == otherSpot.equiltm;
                locallab.spots.at(j).CCmasktmcurve = locallab.spots.at(j).CCmasktmcurve && pSpot.CCmasktmcurve == otherSpot.CCmasktmcurve;
                locallab.spots.at(j).LLmasktmcurve = locallab.spots.at(j).LLmasktmcurve && pSpot.LLmasktmcurve == otherSpot.LLmasktmcurve;
                locallab.spots.at(j).HHmasktmcurve = locallab.spots.at(j).HHmasktmcurve && pSpot.HHmasktmcurve == otherSpot.HHmasktmcurve;
                locallab.spots.at(j).enatmMask = locallab.spots.at(j).enatmMask && pSpot.enatmMask == otherSpot.enatmMask;
                locallab.spots.at(j).blendmasktm = locallab.spots.at(j).blendmasktm && pSpot.blendmasktm == otherSpot.blendmasktm;
                locallab.spots.at(j).radmasktm = locallab.spots.at(j).radmasktm && pSpot.radmasktm == otherSpot.radmasktm;
                locallab.spots.at(j).chromasktm = locallab.spots.at(j).chromasktm && pSpot.chromasktm == otherSpot.chromasktm;
                locallab.spots.at(j).gammasktm = locallab.spots.at(j).gammasktm && pSpot.gammasktm == otherSpot.gammasktm;
                locallab.spots.at(j).slomasktm = locallab.spots.at(j).slomasktm && pSpot.slomasktm == otherSpot.slomasktm;
                // Retinex
                locallab.spots.at(j).expreti = locallab.spots.at(j).expreti && pSpot.expreti == otherSpot.expreti;
                locallab.spots.at(j).retinexMethod = locallab.spots.at(j).retinexMethod && pSpot.retinexMethod == otherSpot.retinexMethod;
                locallab.spots.at(j).str = locallab.spots.at(j).str && pSpot.str == otherSpot.str;
                locallab.spots.at(j).chrrt = locallab.spots.at(j).chrrt && pSpot.chrrt == otherSpot.chrrt;
                locallab.spots.at(j).neigh = locallab.spots.at(j).neigh && pSpot.neigh == otherSpot.neigh;
                locallab.spots.at(j).vart = locallab.spots.at(j).vart && pSpot.vart == otherSpot.vart;
                locallab.spots.at(j).dehaz = locallab.spots.at(j).dehaz && pSpot.dehaz == otherSpot.dehaz;
                locallab.spots.at(j).sensih = locallab.spots.at(j).sensih && pSpot.sensih == otherSpot.sensih;
                locallab.spots.at(j).localTgaincurve = locallab.spots.at(j).localTgaincurve && pSpot.localTgaincurve == otherSpot.localTgaincurve;
                locallab.spots.at(j).inversret = locallab.spots.at(j).inversret && pSpot.inversret == otherSpot.inversret;
                locallab.spots.at(j).equilret = locallab.spots.at(j).equilret && pSpot.equilret == otherSpot.equilret;
                locallab.spots.at(j).softradiusret = locallab.spots.at(j).softradiusret && pSpot.softradiusret == otherSpot.softradiusret;
                locallab.spots.at(j).CCmaskreticurve = locallab.spots.at(j).CCmaskreticurve && pSpot.CCmaskreticurve == otherSpot.CCmaskreticurve;
                locallab.spots.at(j).LLmaskreticurve = locallab.spots.at(j).LLmaskreticurve && pSpot.LLmaskreticurve == otherSpot.LLmaskreticurve;
                locallab.spots.at(j).HHmaskreticurve = locallab.spots.at(j).HHmaskreticurve && pSpot.HHmaskreticurve == otherSpot.HHmaskreticurve;
                locallab.spots.at(j).enaretiMask = locallab.spots.at(j).enaretiMask && pSpot.enaretiMask == otherSpot.enaretiMask;
                locallab.spots.at(j).enaretiMasktmap = locallab.spots.at(j).enaretiMasktmap && pSpot.enaretiMasktmap == otherSpot.enaretiMasktmap;
                locallab.spots.at(j).blendmaskreti = locallab.spots.at(j).blendmaskreti && pSpot.blendmaskreti == otherSpot.blendmaskreti;
                locallab.spots.at(j).radmaskreti = locallab.spots.at(j).radmaskreti && pSpot.radmaskreti == otherSpot.radmaskreti;
                locallab.spots.at(j).chromaskreti = locallab.spots.at(j).chromaskreti && pSpot.chromaskreti == otherSpot.chromaskreti;
                locallab.spots.at(j).gammaskreti = locallab.spots.at(j).gammaskreti && pSpot.gammaskreti == otherSpot.gammaskreti;
                locallab.spots.at(j).slomaskreti = locallab.spots.at(j).slomaskreti && pSpot.slomaskreti == otherSpot.slomaskreti;
                locallab.spots.at(j).scalereti = locallab.spots.at(j).scalereti && pSpot.scalereti == otherSpot.scalereti;
                locallab.spots.at(j).darkness = locallab.spots.at(j).darkness && pSpot.darkness == otherSpot.darkness;
                locallab.spots.at(j).lightnessreti = locallab.spots.at(j).lightnessreti && pSpot.lightnessreti == otherSpot.lightnessreti;
                locallab.spots.at(j).limd = locallab.spots.at(j).limd && pSpot.limd == otherSpot.limd;
                locallab.spots.at(j).fftwreti = locallab.spots.at(j).fftwreti && pSpot.fftwreti == otherSpot.fftwreti;
                // Sharpening
                locallab.spots.at(j).expsharp = locallab.spots.at(j).expsharp && pSpot.expsharp == otherSpot.expsharp;
                locallab.spots.at(j).sharcontrast = locallab.spots.at(j).sharcontrast && pSpot.sharcontrast == otherSpot.sharcontrast;
                locallab.spots.at(j).sharradius = locallab.spots.at(j).sharradius && pSpot.sharradius == otherSpot.sharradius;
                locallab.spots.at(j).sharamount = locallab.spots.at(j).sharamount && pSpot.sharamount == otherSpot.sharamount;
                locallab.spots.at(j).shardamping = locallab.spots.at(j).shardamping && pSpot.shardamping == otherSpot.shardamping;
                locallab.spots.at(j).shariter = locallab.spots.at(j).shariter && pSpot.shariter == otherSpot.shariter;
                locallab.spots.at(j).sharblur = locallab.spots.at(j).sharblur && pSpot.sharblur == otherSpot.sharblur;
                locallab.spots.at(j).sensisha = locallab.spots.at(j).sensisha && pSpot.sensisha == otherSpot.sensisha;
                locallab.spots.at(j).inverssha = locallab.spots.at(j).inverssha && pSpot.inverssha == otherSpot.inverssha;
                // Local Contrast
                locallab.spots.at(j).expcontrast = locallab.spots.at(j).expcontrast && pSpot.expcontrast == otherSpot.expcontrast;
                locallab.spots.at(j).lcradius = locallab.spots.at(j).lcradius && pSpot.lcradius == otherSpot.lcradius;
                locallab.spots.at(j).lcamount = locallab.spots.at(j).lcamount && pSpot.lcamount == otherSpot.lcamount;
                locallab.spots.at(j).lcdarkness = locallab.spots.at(j).lcdarkness && pSpot.lcdarkness == otherSpot.lcdarkness;
                locallab.spots.at(j).lclightness = locallab.spots.at(j).lclightness && pSpot.lclightness == otherSpot.lclightness;
                locallab.spots.at(j).levelwav = locallab.spots.at(j).levelwav && pSpot.levelwav == otherSpot.levelwav;
                locallab.spots.at(j).residcont = locallab.spots.at(j).residcont && pSpot.residcont == otherSpot.residcont;
                locallab.spots.at(j).residchro = locallab.spots.at(j).residchro && pSpot.residchro == otherSpot.residchro;
                locallab.spots.at(j).clarilres = locallab.spots.at(j).clarilres && pSpot.clarilres == otherSpot.clarilres;
                locallab.spots.at(j).claricres = locallab.spots.at(j).claricres && pSpot.claricres == otherSpot.claricres;
                locallab.spots.at(j).clarisoft = locallab.spots.at(j).clarisoft && pSpot.clarisoft == otherSpot.clarisoft;
                locallab.spots.at(j).sensilc = locallab.spots.at(j).sensilc && pSpot.sensilc == otherSpot.sensilc;
                locallab.spots.at(j).fftwlc = locallab.spots.at(j).fftwlc && pSpot.fftwlc == otherSpot.fftwlc;
                locallab.spots.at(j).localcontMethod = locallab.spots.at(j).localcontMethod && pSpot.localcontMethod == otherSpot.localcontMethod;
                locallab.spots.at(j).locwavcurve = locallab.spots.at(j).locwavcurve && pSpot.locwavcurve == otherSpot.locwavcurve;
                // Contrast by detail levels
                locallab.spots.at(j).expcbdl = locallab.spots.at(j).expcbdl && pSpot.expcbdl == otherSpot.expcbdl;

                for (int k = 0; k < 6; k++) {
                    locallab.spots.at(j).mult[k] = locallab.spots.at(j).mult[k] && pSpot.mult[k] == otherSpot.mult[k];
                }

                locallab.spots.at(j).chromacbdl = locallab.spots.at(j).chromacbdl && pSpot.chromacbdl == otherSpot.chromacbdl;
                locallab.spots.at(j).threshold = locallab.spots.at(j).threshold && pSpot.threshold == otherSpot.threshold;
                locallab.spots.at(j).sensicb = locallab.spots.at(j).sensicb && pSpot.sensicb == otherSpot.sensicb;
                locallab.spots.at(j).clarityml = locallab.spots.at(j).clarityml && pSpot.clarityml == otherSpot.clarityml;
                locallab.spots.at(j).contresid = locallab.spots.at(j).contresid && pSpot.contresid == otherSpot.contresid;
                locallab.spots.at(j).blurcbdl = locallab.spots.at(j).blurcbdl && pSpot.blurcbdl == otherSpot.blurcbdl;
                locallab.spots.at(j).blendmaskcb = locallab.spots.at(j).blendmaskcb && pSpot.blendmaskcb == otherSpot.blendmaskcb;
                locallab.spots.at(j).radmaskcb = locallab.spots.at(j).radmaskcb && pSpot.radmaskcb == otherSpot.radmaskcb;
                locallab.spots.at(j).chromaskcb = locallab.spots.at(j).chromaskcb && pSpot.chromaskcb == otherSpot.chromaskcb;
                locallab.spots.at(j).gammaskcb = locallab.spots.at(j).gammaskcb && pSpot.gammaskcb == otherSpot.gammaskcb;
                locallab.spots.at(j).slomaskcb = locallab.spots.at(j).slomaskcb && pSpot.slomaskcb == otherSpot.slomaskcb;
                locallab.spots.at(j).softradiuscb = locallab.spots.at(j).softradiuscb && pSpot.softradiuscb == otherSpot.softradiuscb;
                locallab.spots.at(j).enacbMask = locallab.spots.at(j).enacbMask && pSpot.enacbMask == otherSpot.enacbMask;
                locallab.spots.at(j).CCmaskcbcurve = locallab.spots.at(j).CCmaskcbcurve && pSpot.CCmaskcbcurve == otherSpot.CCmaskcbcurve;
                locallab.spots.at(j).LLmaskcbcurve = locallab.spots.at(j).LLmaskcbcurve && pSpot.LLmaskcbcurve == otherSpot.LLmaskcbcurve;
                locallab.spots.at(j).HHmaskcbcurve = locallab.spots.at(j).HHmaskcbcurve && pSpot.HHmaskcbcurve == otherSpot.HHmaskcbcurve;
                // Denoise
                locallab.spots.at(j).expdenoi = locallab.spots.at(j).expdenoi && pSpot.expdenoi == otherSpot.expdenoi;
                locallab.spots.at(j).noiselumf = locallab.spots.at(j).noiselumf && pSpot.noiselumf == otherSpot.noiselumf;
                locallab.spots.at(j).noiselumf0 = locallab.spots.at(j).noiselumf0 && pSpot.noiselumf0 == otherSpot.noiselumf0;
                locallab.spots.at(j).noiselumf2 = locallab.spots.at(j).noiselumf2 && pSpot.noiselumf2 == otherSpot.noiselumf2;
                locallab.spots.at(j).noiselumc = locallab.spots.at(j).noiselumc && pSpot.noiselumc == otherSpot.noiselumc;
                locallab.spots.at(j).noiselumdetail = locallab.spots.at(j).noiselumdetail && pSpot.noiselumdetail == otherSpot.noiselumdetail;
                locallab.spots.at(j).noiselequal = locallab.spots.at(j).noiselequal && pSpot.noiselequal == otherSpot.noiselequal;
                locallab.spots.at(j).noisechrof = locallab.spots.at(j).noisechrof && pSpot.noisechrof == otherSpot.noisechrof;
                locallab.spots.at(j).noisechroc = locallab.spots.at(j).noisechroc && pSpot.noisechroc == otherSpot.noisechroc;
                locallab.spots.at(j).noisechrodetail = locallab.spots.at(j).noisechrodetail && pSpot.noisechrodetail == otherSpot.noisechrodetail;
                locallab.spots.at(j).adjblur = locallab.spots.at(j).adjblur && pSpot.adjblur == otherSpot.adjblur;
                locallab.spots.at(j).bilateral = locallab.spots.at(j).bilateral && pSpot.bilateral == otherSpot.bilateral;
                locallab.spots.at(j).sensiden = locallab.spots.at(j).sensiden && pSpot.sensiden == otherSpot.sensiden;
            }
        }

        if (!locallab.nbspot || !locallab.id) {
            // locallab.id and locallab.spots are set to false because cannot be combined
            locallab.id = false;
            locallab.spots.clear();
            locallab.spots.resize(p.locallab.nbspot, new LocallabParamsEdited::LocallabSpotEdited(false));
        }

        pcvignette.enabled = pcvignette.enabled && p.pcvignette.enabled == other.pcvignette.enabled;
        pcvignette.strength = pcvignette.strength && p.pcvignette.strength == other.pcvignette.strength;
        pcvignette.feather = pcvignette.feather && p.pcvignette.feather == other.pcvignette.feather;
        pcvignette.roundness = pcvignette.roundness && p.pcvignette.roundness == other.pcvignette.roundness;
        cacorrection.red = cacorrection.red && p.cacorrection.red == other.cacorrection.red;
        cacorrection.blue = cacorrection.blue && p.cacorrection.blue == other.cacorrection.blue;
        vignetting.amount = vignetting.amount && p.vignetting.amount == other.vignetting.amount;
        vignetting.radius = vignetting.radius && p.vignetting.radius == other.vignetting.radius;
        vignetting.strength = vignetting.strength && p.vignetting.strength == other.vignetting.strength;
        vignetting.centerX = vignetting.centerX && p.vignetting.centerX == other.vignetting.centerX;
        vignetting.centerY = vignetting.centerY && p.vignetting.centerY == other.vignetting.centerY;
        chmixer.enabled = chmixer.enabled && p.chmixer.enabled == other.chmixer.enabled;
        chmixer.red[0] = chmixer.red[0] && p.chmixer.red[0] == other.chmixer.red[0];
        chmixer.red[1] = chmixer.red[1] && p.chmixer.red[1] == other.chmixer.red[1];
        chmixer.red[2] = chmixer.red[2] && p.chmixer.red[2] == other.chmixer.red[2];
        chmixer.green[0] = chmixer.green[0] && p.chmixer.green[0] == other.chmixer.green[0];
        chmixer.green[1] = chmixer.green[1] && p.chmixer.green[1] == other.chmixer.green[1];
        chmixer.green[2] = chmixer.green[2] && p.chmixer.green[2] == other.chmixer.green[2];
        chmixer.blue[0] = chmixer.blue[0] && p.chmixer.blue[0] == other.chmixer.blue[0];
        chmixer.blue[1] = chmixer.blue[1] && p.chmixer.blue[1] == other.chmixer.blue[1];
        chmixer.blue[2] = chmixer.blue[2] && p.chmixer.blue[2] == other.chmixer.blue[2];
        blackwhite.enabledcc = blackwhite.enabledcc && p.blackwhite.enabledcc == other.blackwhite.enabledcc;
        blackwhite.enabled = blackwhite.enabled && p.blackwhite.enabled == other.blackwhite.enabled;
        blackwhite.mixerRed = blackwhite.mixerRed && p.blackwhite.mixerRed == other.blackwhite.mixerRed;
        blackwhite.mixerOrange = blackwhite.mixerOrange && p.blackwhite.mixerOrange == other.blackwhite.mixerOrange;
        blackwhite.mixerYellow = blackwhite.mixerYellow && p.blackwhite.mixerYellow == other.blackwhite.mixerYellow;
        blackwhite.mixerGreen = blackwhite.mixerGreen && p.blackwhite.mixerGreen == other.blackwhite.mixerGreen;
        blackwhite.mixerCyan = blackwhite.mixerCyan && p.blackwhite.mixerCyan == other.blackwhite.mixerCyan;
        blackwhite.mixerBlue = blackwhite.mixerBlue && p.blackwhite.mixerBlue == other.blackwhite.mixerBlue;
        blackwhite.mixerMagenta = blackwhite.mixerMagenta && p.blackwhite.mixerMagenta == other.blackwhite.mixerMagenta;
        blackwhite.mixerPurple = blackwhite.mixerPurple && p.blackwhite.mixerPurple == other.blackwhite.mixerPurple;
        blackwhite.gammaRed = blackwhite.gammaRed && p.blackwhite.gammaRed == other.blackwhite.gammaRed;
        blackwhite.gammaGreen = blackwhite.gammaGreen && p.blackwhite.gammaGreen == other.blackwhite.gammaGreen;
        blackwhite.gammaBlue = blackwhite.gammaBlue && p.blackwhite.gammaBlue == other.blackwhite.gammaBlue;
        blackwhite.filter = blackwhite.filter && p.blackwhite.filter == other.blackwhite.filter;
        blackwhite.setting = blackwhite.setting && p.blackwhite.setting == other.blackwhite.setting;
        blackwhite.luminanceCurve = blackwhite.luminanceCurve && p.blackwhite.luminanceCurve == other.blackwhite.luminanceCurve;
        blackwhite.method = blackwhite.method && p.blackwhite.method == other.blackwhite.method;
        blackwhite.beforeCurve = blackwhite.beforeCurve && p.blackwhite.beforeCurve == other.blackwhite.beforeCurve;
        blackwhite.beforeCurveMode = blackwhite.beforeCurveMode && p.blackwhite.beforeCurveMode == other.blackwhite.beforeCurveMode;
        blackwhite.afterCurve = blackwhite.afterCurve && p.blackwhite.afterCurve == other.blackwhite.afterCurve;
        blackwhite.afterCurveMode = blackwhite.afterCurveMode && p.blackwhite.afterCurveMode == other.blackwhite.afterCurveMode;
        blackwhite.autoc = blackwhite.autoc && p.blackwhite.autoc == other.blackwhite.autoc;
        blackwhite.algo = blackwhite.algo && p.blackwhite.algo == other.blackwhite.algo;
        resize.scale = resize.scale && p.resize.scale == other.resize.scale;
        resize.appliesTo = resize.appliesTo && p.resize.appliesTo == other.resize.appliesTo;
        resize.method = resize.method && p.resize.method == other.resize.method;
        resize.dataspec = resize.dataspec && p.resize.dataspec == other.resize.dataspec;
        resize.width = resize.width && p.resize.width == other.resize.width;
        resize.height = resize.height && p.resize.height == other.resize.height;
        resize.enabled = resize.enabled && p.resize.enabled == other.resize.enabled;
        resize.allowUpscaling = resize.allowUpscaling && p.resize.allowUpscaling == other.resize.allowUpscaling;
        icm.inputProfile = icm.inputProfile && p.icm.inputProfile == other.icm.inputProfile;
        icm.toneCurve = icm.toneCurve && p.icm.toneCurve == other.icm.toneCurve;
        icm.applyLookTable = icm.applyLookTable && p.icm.applyLookTable == other.icm.applyLookTable;
        icm.applyBaselineExposureOffset = icm.applyBaselineExposureOffset && p.icm.applyBaselineExposureOffset == other.icm.applyBaselineExposureOffset;
        icm.applyHueSatMap = icm.applyHueSatMap && p.icm.applyHueSatMap == other.icm.applyHueSatMap;
        icm.dcpIlluminant = icm.dcpIlluminant && p.icm.dcpIlluminant == other.icm.dcpIlluminant;
        icm.workingProfile = icm.workingProfile && p.icm.workingProfile == other.icm.workingProfile;
        icm.outputProfile = icm.outputProfile && p.icm.outputProfile == other.icm.outputProfile;
        icm.outputIntent = icm.outputIntent && p.icm.outputIntent == other.icm.outputIntent;
        icm.outputBPC = icm.outputBPC && p.icm.outputBPC == other.icm.outputBPC ;
        icm.workingTRCGamma = icm.workingTRCGamma && p.icm.workingTRCGamma == other.icm.workingTRCGamma;
        icm.workingTRCSlope = icm.workingTRCSlope && p.icm.workingTRCSlope == other.icm.workingTRCSlope;
        icm.workingTRC = icm.workingTRC && p.icm.workingTRC == other.icm.workingTRC;
        raw.bayersensor.method = raw.bayersensor.method && p.raw.bayersensor.method == other.raw.bayersensor.method;
        raw.bayersensor.border = raw.bayersensor.border && p.raw.bayersensor.border == other.raw.bayersensor.border;
        raw.bayersensor.imageNum = raw.bayersensor.imageNum && p.raw.bayersensor.imageNum == other.raw.bayersensor.imageNum;
        raw.bayersensor.ccSteps = raw.bayersensor.ccSteps && p.raw.bayersensor.ccSteps == other.raw.bayersensor.ccSteps;
        raw.bayersensor.exBlack0 = raw.bayersensor.exBlack0 && p.raw.bayersensor.black0 == other.raw.bayersensor.black0;
        raw.bayersensor.exBlack1 = raw.bayersensor.exBlack1 && p.raw.bayersensor.black1 == other.raw.bayersensor.black1;
        raw.bayersensor.exBlack2 = raw.bayersensor.exBlack2 && p.raw.bayersensor.black2 == other.raw.bayersensor.black2;
        raw.bayersensor.exBlack3 = raw.bayersensor.exBlack3 && p.raw.bayersensor.black3 == other.raw.bayersensor.black3;
        raw.bayersensor.exTwoGreen = raw.bayersensor.exTwoGreen && p.raw.bayersensor.twogreen == other.raw.bayersensor.twogreen;
        raw.bayersensor.dcbIterations = raw.bayersensor.dcbIterations && p.raw.bayersensor.dcb_iterations == other.raw.bayersensor.dcb_iterations;
        raw.bayersensor.dcbEnhance = raw.bayersensor.dcbEnhance && p.raw.bayersensor.dcb_enhance == other.raw.bayersensor.dcb_enhance;
        //raw.bayersensor.allEnhance = raw.bayersensor.allEnhance && p.raw.bayersensor.all_enhance == other.raw.bayersensor.all_enhance;
        raw.bayersensor.lmmseIterations = raw.bayersensor.lmmseIterations && p.raw.bayersensor.lmmse_iterations == other.raw.bayersensor.lmmse_iterations;
        raw.bayersensor.dualDemosaicAutoContrast = raw.bayersensor.dualDemosaicAutoContrast && p.raw.bayersensor.dualDemosaicAutoContrast == other.raw.bayersensor.dualDemosaicAutoContrast;
        raw.bayersensor.dualDemosaicContrast = raw.bayersensor.dualDemosaicContrast && p.raw.bayersensor.dualDemosaicContrast == other.raw.bayersensor.dualDemosaicContrast;
        raw.bayersensor.pixelShiftMotionCorrectionMethod = raw.bayersensor.pixelShiftMotionCorrectionMethod && p.raw.bayersensor.pixelShiftMotionCorrectionMethod == other.raw.bayersensor.pixelShiftMotionCorrectionMethod;
        raw.bayersensor.pixelShiftEperIso = raw.bayersensor.pixelShiftEperIso && p.raw.bayersensor.pixelShiftEperIso == other.raw.bayersensor.pixelShiftEperIso;
        raw.bayersensor.pixelShiftSigma = raw.bayersensor.pixelShiftSigma && p.raw.bayersensor.pixelShiftSigma == other.raw.bayersensor.pixelShiftSigma;
        raw.bayersensor.pixelShiftShowMotion = raw.bayersensor.pixelShiftShowMotion && p.raw.bayersensor.pixelShiftShowMotion == other.raw.bayersensor.pixelShiftShowMotion;
        raw.bayersensor.pixelShiftShowMotionMaskOnly = raw.bayersensor.pixelShiftShowMotionMaskOnly && p.raw.bayersensor.pixelShiftShowMotionMaskOnly == other.raw.bayersensor.pixelShiftShowMotionMaskOnly;
        raw.bayersensor.pixelShiftHoleFill = raw.bayersensor.pixelShiftHoleFill && p.raw.bayersensor.pixelShiftHoleFill == other.raw.bayersensor.pixelShiftHoleFill;
        raw.bayersensor.pixelShiftMedian = raw.bayersensor.pixelShiftMedian && p.raw.bayersensor.pixelShiftMedian == other.raw.bayersensor.pixelShiftMedian;
        raw.bayersensor.pixelShiftGreen = raw.bayersensor.pixelShiftGreen && p.raw.bayersensor.pixelShiftGreen == other.raw.bayersensor.pixelShiftGreen;
        raw.bayersensor.pixelShiftBlur = raw.bayersensor.pixelShiftBlur && p.raw.bayersensor.pixelShiftBlur == other.raw.bayersensor.pixelShiftBlur;
        raw.bayersensor.pixelShiftSmooth = raw.bayersensor.pixelShiftSmooth && p.raw.bayersensor.pixelShiftSmoothFactor == other.raw.bayersensor.pixelShiftSmoothFactor;
        raw.bayersensor.pixelShiftEqualBright = raw.bayersensor.pixelShiftEqualBright && p.raw.bayersensor.pixelShiftEqualBright == other.raw.bayersensor.pixelShiftEqualBright;
        raw.bayersensor.pixelShiftEqualBrightChannel = raw.bayersensor.pixelShiftEqualBrightChannel && p.raw.bayersensor.pixelShiftEqualBrightChannel == other.raw.bayersensor.pixelShiftEqualBrightChannel;
        raw.bayersensor.pixelShiftNonGreenCross = raw.bayersensor.pixelShiftNonGreenCross && p.raw.bayersensor.pixelShiftNonGreenCross == other.raw.bayersensor.pixelShiftNonGreenCross;
        raw.bayersensor.pixelShiftDemosaicMethod = raw.bayersensor.pixelShiftDemosaicMethod && p.raw.bayersensor.pixelShiftDemosaicMethod == other.raw.bayersensor.pixelShiftDemosaicMethod;
        raw.bayersensor.greenEq = raw.bayersensor.greenEq && p.raw.bayersensor.greenthresh == other.raw.bayersensor.greenthresh;
        raw.bayersensor.linenoise = raw.bayersensor.linenoise && p.raw.bayersensor.linenoise == other.raw.bayersensor.linenoise;
        raw.bayersensor.linenoiseDirection = raw.bayersensor.linenoiseDirection && p.raw.bayersensor.linenoiseDirection == other.raw.bayersensor.linenoiseDirection;
        raw.bayersensor.pdafLinesFilter = raw.bayersensor.pdafLinesFilter && p.raw.bayersensor.pdafLinesFilter == other.raw.bayersensor.pdafLinesFilter;
        raw.xtranssensor.method = raw.xtranssensor.method && p.raw.xtranssensor.method == other.raw.xtranssensor.method;
        raw.xtranssensor.dualDemosaicAutoContrast = raw.xtranssensor.dualDemosaicAutoContrast && p.raw.xtranssensor.dualDemosaicAutoContrast == other.raw.xtranssensor.dualDemosaicAutoContrast;
        raw.xtranssensor.dualDemosaicContrast = raw.xtranssensor.dualDemosaicContrast && p.raw.xtranssensor.dualDemosaicContrast == other.raw.xtranssensor.dualDemosaicContrast;
        raw.xtranssensor.border = raw.xtranssensor.border && p.raw.xtranssensor.border == other.raw.xtranssensor.border;
        raw.xtranssensor.ccSteps = raw.xtranssensor.ccSteps && p.raw.xtranssensor.ccSteps == other.raw.xtranssensor.ccSteps;
        raw.xtranssensor.exBlackRed = raw.xtranssensor.exBlackRed && p.raw.xtranssensor.blackred == other.raw.xtranssensor.blackred;
        raw.xtranssensor.exBlackGreen = raw.xtranssensor.exBlackGreen && p.raw.xtranssensor.blackgreen == other.raw.xtranssensor.blackgreen;
        raw.xtranssensor.exBlackBlue = raw.xtranssensor.exBlackBlue && p.raw.xtranssensor.blackblue == other.raw.xtranssensor.blackblue;
        raw.ca_autocorrect = raw.ca_autocorrect && p.raw.ca_autocorrect == other.raw.ca_autocorrect;
        raw.ca_avoidcolourshift = raw.ca_avoidcolourshift && p.raw.ca_avoidcolourshift == other.raw.ca_avoidcolourshift;
        raw.caautoiterations = raw.caautoiterations && p.raw.caautoiterations == other.raw.caautoiterations;
        raw.cared = raw.cared && p.raw.cared == other.raw.cared;
        raw.cablue = raw.cablue && p.raw.cablue == other.raw.cablue;
        raw.hotPixelFilter = raw.hotPixelFilter && p.raw.hotPixelFilter == other.raw.hotPixelFilter;
        raw.deadPixelFilter = raw.deadPixelFilter && p.raw.deadPixelFilter == other.raw.deadPixelFilter;
        raw.hotdeadpix_thresh = raw.hotdeadpix_thresh && p.raw.hotdeadpix_thresh == other.raw.hotdeadpix_thresh;
        raw.darkFrame = raw.darkFrame && p.raw.dark_frame == other.raw.dark_frame;
        raw.df_autoselect = raw.df_autoselect && p.raw.df_autoselect == other.raw.df_autoselect;
        raw.ff_file = raw.ff_file && p.raw.ff_file == other.raw.ff_file;
        raw.ff_AutoSelect = raw.ff_AutoSelect && p.raw.ff_AutoSelect == other.raw.ff_AutoSelect;
        raw.ff_BlurRadius = raw.ff_BlurRadius && p.raw.ff_BlurRadius == other.raw.ff_BlurRadius;
        raw.ff_BlurType = raw.ff_BlurType && p.raw.ff_BlurType == other.raw.ff_BlurType;
        raw.ff_AutoClipControl = raw.ff_AutoClipControl && p.raw.ff_AutoClipControl == other.raw.ff_AutoClipControl;
        raw.ff_clipControl = raw.ff_clipControl && p.raw.ff_clipControl == other.raw.ff_clipControl;
        raw.exPos = raw.exPos && p.raw.expos == other.raw.expos;
        wavelet.enabled = wavelet.enabled && p.wavelet.enabled == other.wavelet.enabled;
        wavelet.strength = wavelet.strength && p.wavelet.strength == other.wavelet.strength;
        wavelet.balance = wavelet.balance && p.wavelet.balance == other.wavelet.balance;
        wavelet.iter = wavelet.iter && p.wavelet.iter == other.wavelet.iter;
        wavelet.median = wavelet.median && p.wavelet.median == other.wavelet.median;
        wavelet.medianlev = wavelet.medianlev && p.wavelet.medianlev == other.wavelet.medianlev;
        wavelet.linkedg = wavelet.linkedg && p.wavelet.linkedg == other.wavelet.linkedg;
        wavelet.cbenab = wavelet.cbenab && p.wavelet.cbenab == other.wavelet.cbenab;
        wavelet.greenmed = wavelet.greenmed && p.wavelet.greenmed == other.wavelet.greenmed;
        wavelet.bluemed = wavelet.bluemed && p.wavelet.bluemed == other.wavelet.bluemed;
        wavelet.greenhigh = wavelet.greenhigh && p.wavelet.greenhigh == other.wavelet.greenhigh;
        wavelet.bluehigh = wavelet.bluehigh && p.wavelet.bluehigh == other.wavelet.bluehigh;
        wavelet.greenlow = wavelet.greenlow && p.wavelet.greenlow == other.wavelet.greenlow;
        wavelet.bluelow = wavelet.bluelow && p.wavelet.bluelow == other.wavelet.bluelow;
        wavelet.lipst = wavelet.lipst && p.wavelet.lipst == other.wavelet.lipst;
        wavelet.Medgreinf = wavelet.Medgreinf && p.wavelet.Medgreinf == other.wavelet.Medgreinf;
        wavelet.avoid = wavelet.avoid && p.wavelet.avoid == other.wavelet.avoid;
        wavelet.tmr = wavelet.tmr && p.wavelet.tmr == other.wavelet.tmr;
        wavelet.Lmethod = wavelet.Lmethod && p.wavelet.Lmethod == other.wavelet.Lmethod;
        wavelet.CLmethod = wavelet.CLmethod && p.wavelet.CLmethod == other.wavelet.CLmethod;
        wavelet.Backmethod = wavelet.Backmethod && p.wavelet.Backmethod == other.wavelet.Backmethod;
        wavelet.Tilesmethod = wavelet.Tilesmethod && p.wavelet.Tilesmethod == other.wavelet.Tilesmethod;
        wavelet.daubcoeffmethod = wavelet.daubcoeffmethod && p.wavelet.daubcoeffmethod == other.wavelet.daubcoeffmethod;
        wavelet.CHmethod = wavelet.CHmethod && p.wavelet.CHmethod == other.wavelet.CHmethod;
        wavelet.CHSLmethod = wavelet.CHSLmethod && p.wavelet.CHSLmethod == other.wavelet.CHSLmethod;
        wavelet.EDmethod = wavelet.EDmethod && p.wavelet.EDmethod == other.wavelet.EDmethod;
        wavelet.NPmethod = wavelet.NPmethod && p.wavelet.NPmethod == other.wavelet.NPmethod;
        wavelet.BAmethod = wavelet.BAmethod && p.wavelet.BAmethod == other.wavelet.BAmethod;
        wavelet.TMmethod = wavelet.TMmethod && p.wavelet.TMmethod == other.wavelet.TMmethod;
        wavelet.HSmethod = wavelet.HSmethod && p.wavelet.HSmethod == other.wavelet.HSmethod;
        wavelet.Dirmethod = wavelet.Dirmethod && p.wavelet.Dirmethod == other.wavelet.Dirmethod;
        wavelet.rescon = wavelet.rescon && p.wavelet.rescon == other.wavelet.rescon;
        wavelet.resconH = wavelet.resconH && p.wavelet.resconH == other.wavelet.resconH;
        wavelet.reschro = wavelet.reschro && p.wavelet.reschro == other.wavelet.reschro;
        wavelet.tmrs = wavelet.tmrs && p.wavelet.tmrs == other.wavelet.tmrs;
        wavelet.gamma = wavelet.gamma && p.wavelet.gamma == other.wavelet.gamma;
        wavelet.sup = wavelet.sup && p.wavelet.sup == other.wavelet.sup;
        wavelet.sky = wavelet.sky && p.wavelet.sky == other.wavelet.sky;
        wavelet.threshold = wavelet.threshold && p.wavelet.threshold == other.wavelet.threshold;
        wavelet.threshold2 = wavelet.threshold2 && p.wavelet.threshold2 == other.wavelet.threshold2;
        wavelet.edgedetect = wavelet.edgedetect && p.wavelet.edgedetect == other.wavelet.edgedetect;
        wavelet.edgedetectthr = wavelet.edgedetectthr && p.wavelet.edgedetectthr == other.wavelet.edgedetectthr;
        wavelet.edgedetectthr2 = wavelet.edgedetectthr2 && p.wavelet.edgedetectthr2 == other.wavelet.edgedetectthr2;
        wavelet.edgesensi = wavelet.edgesensi && p.wavelet.edgesensi == other.wavelet.edgesensi;
        wavelet.edgeampli = wavelet.edgeampli && p.wavelet.edgeampli == other.wavelet.edgeampli;
        wavelet.thres = wavelet.thres && p.wavelet.thres == other.wavelet.thres;
        wavelet.chroma = wavelet.chroma && p.wavelet.chroma == other.wavelet.chroma;
        wavelet.chro = wavelet.chro && p.wavelet.chro == other.wavelet.chro;
        wavelet.contrast = wavelet.contrast && p.wavelet.contrast == other.wavelet.contrast;
        wavelet.edgrad = wavelet.edgrad && p.wavelet.edgrad == other.wavelet.edgrad;
        wavelet.edgval = wavelet.edgval && p.wavelet.edgval == other.wavelet.edgval;
        wavelet.edgthresh = wavelet.edgthresh && p.wavelet.edgthresh == other.wavelet.edgthresh;
        wavelet.thr = wavelet.thr && p.wavelet.thr == other.wavelet.thr;
        wavelet.thrH = wavelet.thrH && p.wavelet.thrH == other.wavelet.thrH;
        wavelet.hueskin = wavelet.hueskin && p.wavelet.hueskin == other.wavelet.hueskin;
        wavelet.hueskin2 = wavelet.hueskin2 && p.wavelet.hueskin2 == other.wavelet.hueskin2;
        wavelet.hllev = wavelet.hllev && p.wavelet.hllev == other.wavelet.hllev;
        wavelet.bllev = wavelet.bllev && p.wavelet.bllev == other.wavelet.bllev;
        wavelet.edgcont = wavelet.edgcont && p.wavelet.edgcont == other.wavelet.edgcont;
        wavelet.level0noise = wavelet.level0noise && p.wavelet.level0noise == other.wavelet.level0noise;
        wavelet.level1noise = wavelet.level1noise && p.wavelet.level1noise == other.wavelet.level1noise;
        wavelet.level2noise = wavelet.level2noise && p.wavelet.level2noise == other.wavelet.level2noise;
        wavelet.level3noise = wavelet.level3noise && p.wavelet.level3noise == other.wavelet.level3noise;
        wavelet.pastlev = wavelet.pastlev && p.wavelet.pastlev == other.wavelet.pastlev;
        wavelet.satlev = wavelet.satlev && p.wavelet.satlev == other.wavelet.satlev;
        wavelet.ccwcurve = wavelet.ccwcurve && p.wavelet.ccwcurve == other.wavelet.ccwcurve;
        wavelet.opacityCurveRG = wavelet.opacityCurveRG && p.wavelet.opacityCurveRG == other.wavelet.opacityCurveRG;
        wavelet.opacityCurveBY = wavelet.opacityCurveBY && p.wavelet.opacityCurveBY == other.wavelet.opacityCurveBY;
        wavelet.opacityCurveW = wavelet.opacityCurveW && p.wavelet.opacityCurveW == other.wavelet.opacityCurveW;
        wavelet.opacityCurveWL = wavelet.opacityCurveWL && p.wavelet.opacityCurveWL == other.wavelet.opacityCurveWL;
        wavelet.wavclCurve = wavelet.wavclCurve && p.wavelet.wavclCurve == other.wavelet.wavclCurve;
        wavelet.hhcurve = wavelet.hhcurve && p.wavelet.hhcurve == other.wavelet.hhcurve;
        wavelet.Chcurve = wavelet.Chcurve && p.wavelet.Chcurve == other.wavelet.Chcurve;
        wavelet.skinprotect = wavelet.skinprotect && p.wavelet.skinprotect == other.wavelet.skinprotect;
        //    wavelet.enacont = wavelet.enacont && p.wavelet.enacont == other.wavelet.enacont;
        wavelet.expcontrast = wavelet.expcontrast && p.wavelet.expcontrast == other.wavelet.expcontrast;
        wavelet.expchroma = wavelet.expchroma && p.wavelet.expchroma == other.wavelet.expchroma;
        wavelet.expedge = wavelet.expedge && p.wavelet.expedge == other.wavelet.expedge;
        wavelet.expresid = wavelet.expresid && p.wavelet.expresid == other.wavelet.expresid;
        wavelet.expfinal = wavelet.expfinal && p.wavelet.expfinal == other.wavelet.expfinal;
        wavelet.exptoning = wavelet.exptoning && p.wavelet.exptoning == other.wavelet.exptoning;
        wavelet.expnoise = wavelet.expnoise && p.wavelet.expnoise == other.wavelet.expnoise;

        for (int level = 0; level < 9; ++level) {
            wavelet.c[level] = wavelet.c[level] && p.wavelet.c[level] == other.wavelet.c[level];
            wavelet.ch[level] = wavelet.ch[level] && p.wavelet.ch[level] == other.wavelet.ch[level];
        }

        dirpyrequalizer.enabled = dirpyrequalizer.enabled && p.dirpyrequalizer.enabled == other.dirpyrequalizer.enabled;
        dirpyrequalizer.gamutlab = dirpyrequalizer.gamutlab && p.dirpyrequalizer.gamutlab == other.dirpyrequalizer.gamutlab;
        dirpyrequalizer.cbdlMethod = dirpyrequalizer.cbdlMethod && p.dirpyrequalizer.cbdlMethod == other.dirpyrequalizer.cbdlMethod;

        for (int level = 0; level < 6; ++level) {
            dirpyrequalizer.mult[level] = dirpyrequalizer.mult[level] && p.dirpyrequalizer.mult[level] == other.dirpyrequalizer.mult[level];
        }

        dirpyrequalizer.threshold = dirpyrequalizer.threshold && p.dirpyrequalizer.threshold == other.dirpyrequalizer.threshold;
        dirpyrequalizer.skinprotect = dirpyrequalizer.skinprotect && p.dirpyrequalizer.skinprotect == other.dirpyrequalizer.skinprotect;
        //    dirpyrequalizer.algo = dirpyrequalizer.algo && p.dirpyrequalizer.algo == other.dirpyrequalizer.algo;
        dirpyrequalizer.hueskin = dirpyrequalizer.hueskin && p.dirpyrequalizer.hueskin == other.dirpyrequalizer.hueskin;
        hsvequalizer.enabled = hsvequalizer.enabled && p.hsvequalizer.enabled == other.hsvequalizer.enabled;
        hsvequalizer.hcurve = hsvequalizer.hcurve && p.hsvequalizer.hcurve == other.hsvequalizer.hcurve;
        hsvequalizer.scurve = hsvequalizer.scurve && p.hsvequalizer.scurve == other.hsvequalizer.scurve;
        hsvequalizer.vcurve = hsvequalizer.vcurve && p.hsvequalizer.vcurve == other.hsvequalizer.vcurve;
        filmSimulation.enabled = filmSimulation.enabled && p.filmSimulation.enabled == other.filmSimulation.enabled;
        filmSimulation.clutFilename = filmSimulation.clutFilename && p.filmSimulation.clutFilename == other.filmSimulation.clutFilename;
        filmSimulation.strength = filmSimulation.strength && p.filmSimulation.strength == other.filmSimulation.strength;
        softlight.enabled = softlight.enabled && p.softlight.enabled == other.softlight.enabled;
        softlight.strength = softlight.strength && p.softlight.strength == other.softlight.strength;
        dehaze.enabled = dehaze.enabled && p.dehaze.enabled == other.dehaze.enabled;
        dehaze.strength = dehaze.strength && p.dehaze.strength == other.dehaze.strength;
        dehaze.showDepthMap = dehaze.showDepthMap && p.dehaze.showDepthMap == other.dehaze.showDepthMap;
        dehaze.depth = dehaze.depth && p.dehaze.depth == other.dehaze.depth;
        metadata.mode = metadata.mode && p.metadata.mode == other.metadata.mode;
        filmNegative.enabled = filmNegative.enabled && p.filmNegative.enabled == other.filmNegative.enabled;
        filmNegative.redRatio = filmNegative.redRatio && p.filmNegative.redRatio == other.filmNegative.redRatio;
        filmNegative.greenExp = filmNegative.greenExp && p.filmNegative.greenExp == other.filmNegative.greenExp;
        filmNegative.blueRatio = filmNegative.blueRatio && p.filmNegative.blueRatio == other.filmNegative.blueRatio;

//      How the hell can we handle that???
//      exif = exif && p.exif==other.exif
//      iptc = other.iptc;
    }
}

void ParamsEdited::combine(rtengine::procparams::ProcParams& toEdit, const rtengine::procparams::ProcParams& mods, bool forceSet)
{

    bool dontforceSet = !forceSet;

    if (toneCurve.curve) {
        toEdit.toneCurve.curve = mods.toneCurve.curve;
    }

    if (toneCurve.curve2) {
        toEdit.toneCurve.curve2 = mods.toneCurve.curve2;
    }

    if (toneCurve.curveMode) {
        toEdit.toneCurve.curveMode = mods.toneCurve.curveMode;
    }

    if (toneCurve.curveMode2) {
        toEdit.toneCurve.curveMode2 = mods.toneCurve.curveMode2;
    }

    if (toneCurve.brightness) {
        toEdit.toneCurve.brightness = dontforceSet && options.baBehav[ADDSET_TC_BRIGHTNESS] ? toEdit.toneCurve.brightness + mods.toneCurve.brightness : mods.toneCurve.brightness;
    }

    if (toneCurve.black) {
        toEdit.toneCurve.black = dontforceSet && options.baBehav[ADDSET_TC_BLACKLEVEL] ? toEdit.toneCurve.black + mods.toneCurve.black : mods.toneCurve.black;
    }

    if (toneCurve.contrast) {
        toEdit.toneCurve.contrast = dontforceSet && options.baBehav[ADDSET_TC_CONTRAST] ? toEdit.toneCurve.contrast + mods.toneCurve.contrast : mods.toneCurve.contrast;
    }

    if (toneCurve.saturation) {
        toEdit.toneCurve.saturation = dontforceSet && options.baBehav[ADDSET_TC_SATURATION] ? toEdit.toneCurve.saturation + mods.toneCurve.saturation : mods.toneCurve.saturation;
    }

    if (toneCurve.shcompr) {
        toEdit.toneCurve.shcompr = dontforceSet && options.baBehav[ADDSET_TC_SHCOMP] ? toEdit.toneCurve.shcompr + mods.toneCurve.shcompr : mods.toneCurve.shcompr;
    }

    if (toneCurve.autoexp) {
        toEdit.toneCurve.autoexp = mods.toneCurve.autoexp;
    }

    if (toneCurve.clip) {
        toEdit.toneCurve.clip = mods.toneCurve.clip;
    }

    if (toneCurve.expcomp) {
        toEdit.toneCurve.expcomp = dontforceSet && options.baBehav[ADDSET_TC_EXPCOMP] ? toEdit.toneCurve.expcomp + mods.toneCurve.expcomp : mods.toneCurve.expcomp;
    }

    if (toneCurve.hlcompr) {
        toEdit.toneCurve.hlcompr = dontforceSet && options.baBehav[ADDSET_TC_HLCOMPAMOUNT] ? toEdit.toneCurve.hlcompr + mods.toneCurve.hlcompr : mods.toneCurve.hlcompr;
    }

    if (toneCurve.hlcomprthresh) {
        toEdit.toneCurve.hlcomprthresh = dontforceSet && options.baBehav[ADDSET_TC_HLCOMPTHRESH] ? toEdit.toneCurve.hlcomprthresh + mods.toneCurve.hlcomprthresh : mods.toneCurve.hlcomprthresh;
    }

    if (toneCurve.hrenabled) {
        toEdit.toneCurve.hrenabled = mods.toneCurve.hrenabled;
    }

    if (toneCurve.method) {
        toEdit.toneCurve.method = mods.toneCurve.method;
    }

    if (toneCurve.histmatching) {
        toEdit.toneCurve.histmatching = mods.toneCurve.histmatching;
    }

    if (toneCurve.fromHistMatching) {
        toEdit.toneCurve.fromHistMatching = mods.toneCurve.fromHistMatching;
    }

    if (toneCurve.clampOOG) {
        toEdit.toneCurve.clampOOG = mods.toneCurve.clampOOG;
    }

    if (retinex.enabled) {
        toEdit.retinex.enabled = mods.retinex.enabled;
    }

    if (retinex.cdcurve) {
        toEdit.retinex.cdcurve = mods.retinex.cdcurve;
    }

    if (retinex.mapcurve) {
        toEdit.retinex.mapcurve = mods.retinex.mapcurve;
    }

    if (retinex.cdHcurve) {
        toEdit.retinex.cdHcurve = mods.retinex.cdHcurve;
    }

    if (retinex.lhcurve) {
        toEdit.retinex.lhcurve = mods.retinex.lhcurve;
    }

    if (retinex.transmissionCurve) {
        toEdit.retinex.transmissionCurve = mods.retinex.transmissionCurve;
    }

    if (retinex.gaintransmissionCurve) {
        toEdit.retinex.gaintransmissionCurve = mods.retinex.gaintransmissionCurve;
    }

    if (retinex.retinexMethod) {
        toEdit.retinex.retinexMethod = mods.retinex.retinexMethod;
    }

    if (retinex.mapMethod) {
        toEdit.retinex.mapMethod = mods.retinex.mapMethod;
    }

    if (retinex.viewMethod) {
        toEdit.retinex.viewMethod = mods.retinex.viewMethod;
    }

    if (retinex.retinexcolorspace) {
        toEdit.retinex.retinexcolorspace = mods.retinex.retinexcolorspace;
    }

    if (retinex.gammaretinex) {
        toEdit.retinex.gammaretinex = mods.retinex.gammaretinex;
    }

    if (retinex.gam) {
        toEdit.retinex.gam = dontforceSet && options.baBehav[ADDSET_RETI_GAM] ? toEdit.retinex.gam + mods.retinex.gam : mods.retinex.gam;
    }

    if (retinex.slope) {
        toEdit.retinex.slope = dontforceSet && options.baBehav[ADDSET_RETI_SLO] ? toEdit.retinex.slope + mods.retinex.slope : mods.retinex.slope;
    }

    if (retinex.str) {
        toEdit.retinex.str = dontforceSet && options.baBehav[ADDSET_RETI_STR] ? toEdit.retinex.str + mods.retinex.str : mods.retinex.str;
    }

    if (retinex.scal) {
        toEdit.retinex.scal = mods.retinex.scal;
    }

    if (retinex.iter) {
        toEdit.retinex.iter = mods.retinex.iter;
    }

    if (retinex.grad) {
        toEdit.retinex.grad = mods.retinex.grad;
    }

    if (retinex.grads) {
        toEdit.retinex.grads = mods.retinex.grads;
    }

//    if (retinex.scal) {
//        toEdit.retinex.scal = dontforceSet && options.baBehav[ADDSET_RETI_SCAL] ? toEdit.retinex.scal + mods.retinex.scal : mods.retinex.scal;
//    }

    if (retinex.medianmap) {
        toEdit.retinex.medianmap = mods.retinex.medianmap;
    }

    if (retinex.neigh) {
        toEdit.retinex.neigh = dontforceSet && options.baBehav[ADDSET_RETI_NEIGH] ? toEdit.retinex.neigh + mods.retinex.neigh : mods.retinex.neigh;
    }

    if (retinex.limd) {
        toEdit.retinex.limd = dontforceSet && options.baBehav[ADDSET_RETI_LIMD] ? toEdit.retinex.limd + mods.retinex.limd : mods.retinex.limd;
    }

    if (retinex.highl) {
        toEdit.retinex.highl = mods.retinex.highl;
    }

    if (retinex.skal) {
        toEdit.retinex.skal = mods.retinex.skal;
    }

    if (retinex.offs) {
        toEdit.retinex.offs = dontforceSet && options.baBehav[ADDSET_RETI_OFFS] ? toEdit.retinex.offs + mods.retinex.offs : mods.retinex.offs;
    }

    if (retinex.vart) {
        toEdit.retinex.vart = dontforceSet && options.baBehav[ADDSET_RETI_VART] ? toEdit.retinex.vart + mods.retinex.vart : mods.retinex.vart;
    }

    if (retinex.highlights) {
        toEdit.retinex.highlights = mods.retinex.highlights;
    }

    if (retinex.htonalwidth) {
        toEdit.retinex.htonalwidth = mods.retinex.htonalwidth;
    }

    if (retinex.shadows) {
        toEdit.retinex.shadows = mods.retinex.shadows;

    }

    if (retinex.stonalwidth) {
        toEdit.retinex.stonalwidth = mods.retinex.stonalwidth;
    }

    if (retinex.radius) {
        toEdit.retinex.radius = mods.retinex.radius;
    }


    if (labCurve.enabled) {
        toEdit.labCurve.enabled = mods.labCurve.enabled;
    }

    if (labCurve.lcurve) {
        toEdit.labCurve.lcurve = mods.labCurve.lcurve;
    }

    if (labCurve.acurve) {
        toEdit.labCurve.acurve = mods.labCurve.acurve;
    }

    if (labCurve.bcurve) {
        toEdit.labCurve.bcurve = mods.labCurve.bcurve;
    }

    if (labCurve.cccurve) {
        toEdit.labCurve.cccurve = mods.labCurve.cccurve;
    }

    if (labCurve.chcurve) {
        toEdit.labCurve.chcurve = mods.labCurve.chcurve;
    }

    if (labCurve.lhcurve) {
        toEdit.labCurve.lhcurve = mods.labCurve.lhcurve;
    }

    if (labCurve.hhcurve) {
        toEdit.labCurve.hhcurve = mods.labCurve.hhcurve;
    }

    if (labCurve.lccurve) {
        toEdit.labCurve.lccurve = mods.labCurve.lccurve;
    }

    if (labCurve.clcurve) {
        toEdit.labCurve.clcurve = mods.labCurve.clcurve;
    }

    if (labCurve.brightness) {
        toEdit.labCurve.brightness = dontforceSet && options.baBehav[ADDSET_LC_BRIGHTNESS] ? toEdit.labCurve.brightness + mods.labCurve.brightness : mods.labCurve.brightness;
    }

    if (labCurve.contrast) {
        toEdit.labCurve.contrast = dontforceSet && options.baBehav[ADDSET_LC_CONTRAST] ? toEdit.labCurve.contrast + mods.labCurve.contrast : mods.labCurve.contrast;
    }

    if (labCurve.chromaticity) {
        toEdit.labCurve.chromaticity = dontforceSet && options.baBehav[ADDSET_LC_CHROMATICITY] ? toEdit.labCurve.chromaticity + mods.labCurve.chromaticity : mods.labCurve.chromaticity;
    }

    if (labCurve.avoidcolorshift) {
        toEdit.labCurve.avoidcolorshift = mods.labCurve.avoidcolorshift;
    }

    if (labCurve.rstprotection) {
        toEdit.labCurve.rstprotection = mods.labCurve.rstprotection;
    }

    if (labCurve.lcredsk) {
        toEdit.labCurve.lcredsk = mods.labCurve.lcredsk;
    }

    if (localContrast.enabled) {
        toEdit.localContrast.enabled = mods.localContrast.enabled;
    }

#define ADDSETVAL_(v, i)                                                                        \
    do {                                                                                        \
        if ( v ) {                                                                              \
            toEdit. v = dontforceSet && options.baBehav[ i ] ? toEdit. v + mods. v : mods. v ;  \
        }                                                                                       \
    } while (false)

    ADDSETVAL_(localContrast.radius, ADDSET_LOCALCONTRAST_RADIUS);
    ADDSETVAL_(localContrast.amount, ADDSET_LOCALCONTRAST_AMOUNT);
    ADDSETVAL_(localContrast.darkness, ADDSET_LOCALCONTRAST_DARKNESS);
    ADDSETVAL_(localContrast.lightness, ADDSET_LOCALCONTRAST_LIGHTNESS);
#undef ADDSETVAL_

    if (rgbCurves.enabled) {
        toEdit.rgbCurves.enabled = mods.rgbCurves.enabled;
    }

    if (rgbCurves.lumamode) {
        toEdit.rgbCurves.lumamode = mods.rgbCurves.lumamode;
    }

    if (rgbCurves.rcurve) {
        toEdit.rgbCurves.rcurve = mods.rgbCurves.rcurve;
    }

    if (rgbCurves.gcurve) {
        toEdit.rgbCurves.gcurve = mods.rgbCurves.gcurve;
    }

    if (rgbCurves.bcurve) {
        toEdit.rgbCurves.bcurve = mods.rgbCurves.bcurve;
    }

    if (colorToning.enabled) {
        toEdit.colorToning.enabled = mods.colorToning.enabled;
    }

    if (colorToning.twocolor) {
        toEdit.colorToning.twocolor = mods.colorToning.twocolor;
    }

    if (colorToning.opacityCurve) {
        toEdit.colorToning.opacityCurve = mods.colorToning.opacityCurve;
    }

    if (colorToning.colorCurve) {
        toEdit.colorToning.colorCurve = mods.colorToning.colorCurve;
    }

    if (colorToning.enabled) {
        toEdit.colorToning.enabled = mods.colorToning.enabled;
    }

    if (colorToning.opacityCurve) {
        toEdit.colorToning.opacityCurve = mods.colorToning.opacityCurve;
    }

    if (colorToning.satprotectionthreshold) {
        toEdit.colorToning.satProtectionThreshold = dontforceSet && options.baBehav[ADDSET_COLORTONING_SATTHRESHOLD] ? toEdit.colorToning.satProtectionThreshold + mods.colorToning.satProtectionThreshold : mods.colorToning.satProtectionThreshold;
    }

    if (colorToning.autosat) {
        toEdit.colorToning.autosat = mods.colorToning.autosat;
    }

    if (colorToning.saturatedopacity) {
        toEdit.colorToning.saturatedOpacity = dontforceSet && options.baBehav[ADDSET_COLORTONING_SATOPACITY] ? toEdit.colorToning.saturatedOpacity + mods.colorToning.saturatedOpacity : mods.colorToning.saturatedOpacity;
    }

    if (colorToning.strength) {
        toEdit.colorToning.strength = dontforceSet && options.baBehav[ADDSET_COLORTONING_STRENGTH] ? toEdit.colorToning.strength + mods.colorToning.strength : mods.colorToning.strength;
    }

    if (colorToning.shadowsColSat) {
        toEdit.colorToning.shadowsColSat = mods.colorToning.shadowsColSat;
    }

    if (colorToning.hlColSat) {
        toEdit.colorToning.hlColSat = mods.colorToning.hlColSat;
    }

    if (colorToning.balance) {
        toEdit.colorToning.balance = dontforceSet && options.baBehav[ADDSET_COLORTONING_BALANCE] ? toEdit.colorToning.balance + mods.colorToning.balance : mods.colorToning.balance;
    }

    if (colorToning.clcurve) {
        toEdit.colorToning.clcurve = mods.colorToning.clcurve;
    }

    if (colorToning.method) {
        toEdit.colorToning.method = mods.colorToning.method;
    }

    if (colorToning.cl2curve) {
        toEdit.colorToning.cl2curve = mods.colorToning.cl2curve;
    }

    if (colorToning.lumamode) {
        toEdit.colorToning.lumamode = mods.colorToning.lumamode;
    }

    if (colorToning.satlow) {
        toEdit.colorToning.satlow = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.satlow + mods.colorToning.satlow : mods.colorToning.satlow;
    }

    if (colorToning.sathigh) {
        toEdit.colorToning.sathigh = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.sathigh + mods.colorToning.sathigh : mods.colorToning.sathigh;
    }

    if (colorToning.redlow) {
        toEdit.colorToning.redlow = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.redlow + mods.colorToning.redlow : mods.colorToning.redlow;
    }

    if (colorToning.greenlow) {
        toEdit.colorToning.greenlow = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.greenlow + mods.colorToning.greenlow : mods.colorToning.greenlow;
    }

    if (colorToning.bluelow) {
        toEdit.colorToning.bluelow = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.bluelow + mods.colorToning.bluelow : mods.colorToning.bluelow;
    }

    if (colorToning.redmed) {
        toEdit.colorToning.redmed = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.redmed + mods.colorToning.redmed : mods.colorToning.redmed;
    }

    if (colorToning.greenmed) {
        toEdit.colorToning.greenmed = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.greenmed + mods.colorToning.greenmed : mods.colorToning.greenmed;
    }

    if (colorToning.bluemed) {
        toEdit.colorToning.bluemed = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.bluemed + mods.colorToning.bluemed : mods.colorToning.bluemed;
    }

    if (colorToning.redhigh) {
        toEdit.colorToning.redhigh = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.redhigh + mods.colorToning.redhigh : mods.colorToning.redhigh;
    }

    if (colorToning.greenhigh) {
        toEdit.colorToning.greenhigh = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.greenhigh + mods.colorToning.greenhigh : mods.colorToning.greenhigh;
    }

    if (colorToning.bluehigh) {
        toEdit.colorToning.bluehigh = dontforceSet && options.baBehav[ADDSET_COLORTONING_SPLIT] ? toEdit.colorToning.bluehigh + mods.colorToning.bluehigh : mods.colorToning.bluehigh;
    }

    if (colorToning.labgridALow) {
        toEdit.colorToning.labgridALow = mods.colorToning.labgridALow;
    }

    if (colorToning.labgridBLow) {
        toEdit.colorToning.labgridBLow = mods.colorToning.labgridBLow;
    }

    if (colorToning.labgridAHigh) {
        toEdit.colorToning.labgridAHigh = mods.colorToning.labgridAHigh;
    }

    if (colorToning.labgridBHigh) {
        toEdit.colorToning.labgridBHigh = mods.colorToning.labgridBHigh;
    }

    if (colorToning.labregions) {
        toEdit.colorToning.labregions = mods.colorToning.labregions;
    }

    if (colorToning.labregionsShowMask) {
        toEdit.colorToning.labregionsShowMask = mods.colorToning.labregionsShowMask;
    }

    if (sharpenEdge.enabled) {
        toEdit.sharpenEdge.enabled = mods.sharpenEdge.enabled;
    }

    if (sharpenEdge.passes) {
        toEdit.sharpenEdge.passes = dontforceSet && options.baBehav[ADDSET_SHARPENEDGE_PASS] ? toEdit.sharpenEdge.passes + mods.sharpenEdge.passes : mods.sharpenEdge.passes;
    }

    if (sharpenEdge.amount) {
        toEdit.sharpenEdge.amount = dontforceSet && options.baBehav[ADDSET_SHARPENEDGE_AMOUNT] ? toEdit.sharpenEdge.amount + mods.sharpenEdge.amount : mods.sharpenEdge.amount;
    }

    if (sharpenEdge.threechannels) {
        toEdit.sharpenEdge.threechannels = mods.sharpenEdge.threechannels;
    }

    if (sharpenMicro.enabled) {
        toEdit.sharpenMicro.enabled = mods.sharpenMicro.enabled;
    }

    if (sharpenMicro.matrix) {
        toEdit.sharpenMicro.matrix = mods.sharpenMicro.matrix;
    }

    if (sharpenMicro.amount) {
        toEdit.sharpenMicro.amount = dontforceSet && options.baBehav[ADDSET_SHARPENMICRO_AMOUNT] ? toEdit.sharpenMicro.amount + mods.sharpenMicro.amount : mods.sharpenMicro.amount;
    }

    if (sharpenMicro.contrast) {
        toEdit.sharpenMicro.contrast = dontforceSet && options.baBehav[ADDSET_SHARPENMICRO_CONTRAST] ? toEdit.sharpenMicro.contrast + mods.sharpenMicro.contrast : mods.sharpenMicro.contrast;
    }

    if (sharpenMicro.uniformity) {
        toEdit.sharpenMicro.uniformity = dontforceSet && options.baBehav[ADDSET_SHARPENMICRO_UNIFORMITY] ? toEdit.sharpenMicro.uniformity + mods.sharpenMicro.uniformity : mods.sharpenMicro.uniformity;
    }

    if (sharpening.enabled) {
        toEdit.sharpening.enabled = mods.sharpening.enabled;
    }

    if (sharpening.contrast) {
        toEdit.sharpening.contrast = dontforceSet && options.baBehav[ADDSET_SHARP_CONTRAST] ? toEdit.sharpening.contrast + mods.sharpening.contrast : mods.sharpening.contrast;
    }

    if (sharpening.radius) {
        toEdit.sharpening.radius = dontforceSet && options.baBehav[ADDSET_SHARP_RADIUS] ? toEdit.sharpening.radius + mods.sharpening.radius : mods.sharpening.radius;
    }

    if (sharpening.blurradius) {
        toEdit.sharpening.blurradius = dontforceSet && options.baBehav[ADDSET_SHARP_RADIUS] ? toEdit.sharpening.blurradius + mods.sharpening.blurradius : mods.sharpening.blurradius;
    }

    if (sharpening.amount) {
        toEdit.sharpening.amount = dontforceSet && options.baBehav[ADDSET_SHARP_AMOUNT] ? toEdit.sharpening.amount + mods.sharpening.amount : mods.sharpening.amount;
    }

    if (sharpening.threshold) {
        toEdit.sharpening.threshold = mods.sharpening.threshold;
    }

    if (sharpening.edgesonly) {
        toEdit.sharpening.edgesonly = mods.sharpening.edgesonly;
    }

    if (sharpening.edges_radius) {
        toEdit.sharpening.edges_radius = dontforceSet && options.baBehav[ADDSET_SHARP_RADIUS] ? toEdit.sharpening.edges_radius + mods.sharpening.edges_radius : mods.sharpening.edges_radius;
    }

    if (sharpening.edges_tolerance) {
        toEdit.sharpening.edges_tolerance = dontforceSet && options.baBehav[ADDSET_SHARP_EDGETOL] ? toEdit.sharpening.edges_tolerance + mods.sharpening.edges_tolerance : mods.sharpening.edges_tolerance;
    }

    if (sharpening.halocontrol) {
        toEdit.sharpening.halocontrol = mods.sharpening.halocontrol;
    }

    if (sharpening.halocontrol_amount) {
        toEdit.sharpening.halocontrol_amount = dontforceSet && options.baBehav[ADDSET_SHARP_HALOCTRL] ? toEdit.sharpening.halocontrol_amount + mods.sharpening.halocontrol_amount : mods.sharpening.halocontrol_amount;
    }

    if (sharpening.method) {
        toEdit.sharpening.method = mods.sharpening.method;
    }

    if (sharpening.deconvamount) {
        toEdit.sharpening.deconvamount = dontforceSet && options.baBehav[ADDSET_SHARP_AMOUNT] ? toEdit.sharpening.deconvamount + mods.sharpening.deconvamount : mods.sharpening.deconvamount;
    }

    if (sharpening.deconvradius) {
        toEdit.sharpening.deconvradius = dontforceSet && options.baBehav[ADDSET_SHARP_RADIUS] ? toEdit.sharpening.deconvradius + mods.sharpening.deconvradius : mods.sharpening.deconvradius;
    }

    if (sharpening.deconviter) {
        toEdit.sharpening.deconviter = dontforceSet && options.baBehav[ADDSET_SHARP_ITER] ? toEdit.sharpening.deconviter + mods.sharpening.deconviter : mods.sharpening.deconviter;
    }

    if (sharpening.deconvdamping) {
        toEdit.sharpening.deconvdamping = dontforceSet && options.baBehav[ADDSET_SHARP_DAMPING] ? toEdit.sharpening.deconvdamping + mods.sharpening.deconvdamping : mods.sharpening.deconvdamping;
    }

    if (prsharpening.enabled) {
        toEdit.prsharpening.enabled = mods.prsharpening.enabled;
    }

    if (prsharpening.contrast) {
        toEdit.prsharpening.contrast = dontforceSet && options.baBehav[ADDSET_SHARP_CONTRAST] ? toEdit.prsharpening.contrast + mods.prsharpening.contrast : mods.prsharpening.contrast;
    }

    if (prsharpening.radius) {
        toEdit.prsharpening.radius = dontforceSet && options.baBehav[ADDSET_SHARP_RADIUS] ? toEdit.prsharpening.radius + mods.prsharpening.radius : mods.prsharpening.radius;
    }

    if (prsharpening.amount) {
        toEdit.prsharpening.amount = dontforceSet && options.baBehav[ADDSET_SHARP_AMOUNT] ? toEdit.prsharpening.amount + mods.prsharpening.amount : mods.prsharpening.amount;
    }

    if (prsharpening.threshold) {
        toEdit.prsharpening.threshold = mods.prsharpening.threshold;
    }

    if (prsharpening.edgesonly) {
        toEdit.prsharpening.edgesonly = mods.prsharpening.edgesonly;
    }

    if (prsharpening.edges_radius) {
        toEdit.prsharpening.edges_radius = dontforceSet && options.baBehav[ADDSET_SHARP_RADIUS] ? toEdit.prsharpening.edges_radius + mods.prsharpening.edges_radius : mods.prsharpening.edges_radius;
    }

    if (prsharpening.edges_tolerance) {
        toEdit.prsharpening.edges_tolerance = dontforceSet && options.baBehav[ADDSET_SHARP_EDGETOL] ? toEdit.prsharpening.edges_tolerance + mods.prsharpening.edges_tolerance : mods.prsharpening.edges_tolerance;
    }

    if (prsharpening.halocontrol) {
        toEdit.prsharpening.halocontrol = mods.prsharpening.halocontrol;
    }

    if (prsharpening.halocontrol_amount) {
        toEdit.prsharpening.halocontrol_amount = dontforceSet && options.baBehav[ADDSET_SHARP_HALOCTRL] ? toEdit.prsharpening.halocontrol_amount + mods.prsharpening.halocontrol_amount : mods.prsharpening.halocontrol_amount;
    }

    if (prsharpening.method) {
        toEdit.prsharpening.method = mods.prsharpening.method;
    }

    if (prsharpening.deconvamount) {
        toEdit.prsharpening.deconvamount = dontforceSet && options.baBehav[ADDSET_SHARP_AMOUNT] ? toEdit.prsharpening.deconvamount + mods.prsharpening.deconvamount : mods.prsharpening.deconvamount;
    }

    if (prsharpening.deconvradius) {
        toEdit.prsharpening.deconvradius = dontforceSet && options.baBehav[ADDSET_SHARP_RADIUS] ? toEdit.prsharpening.deconvradius + mods.prsharpening.deconvradius : mods.prsharpening.deconvradius;
    }

    if (prsharpening.deconviter) {
        toEdit.prsharpening.deconviter = dontforceSet && options.baBehav[ADDSET_SHARP_ITER] ? toEdit.prsharpening.deconviter + mods.prsharpening.deconviter : mods.prsharpening.deconviter;
    }

    if (prsharpening.deconvdamping) {
        toEdit.prsharpening.deconvdamping = dontforceSet && options.baBehav[ADDSET_SHARP_DAMPING] ? toEdit.prsharpening.deconvdamping + mods.prsharpening.deconvdamping : mods.prsharpening.deconvdamping;
    }

    if (vibrance.enabled) {
        toEdit.vibrance.enabled = mods.vibrance.enabled;
    }

    if (vibrance.pastels) {
        toEdit.vibrance.pastels = dontforceSet && options.baBehav[ADDSET_VIBRANCE_PASTELS] ? toEdit.vibrance.pastels + mods.vibrance.pastels : mods.vibrance.pastels;
    }

    if (vibrance.saturated) {
        toEdit.vibrance.saturated = dontforceSet && options.baBehav[ADDSET_VIBRANCE_SATURATED] ? toEdit.vibrance.saturated + mods.vibrance.saturated : mods.vibrance.saturated;
    }

    if (vibrance.psthreshold) {
        toEdit.vibrance.psthreshold = mods.vibrance.psthreshold;
    }

    if (vibrance.protectskins) {
        toEdit.vibrance.protectskins = mods.vibrance.protectskins;
    }

    if (vibrance.avoidcolorshift) {
        toEdit.vibrance.avoidcolorshift = mods.vibrance.avoidcolorshift;
    }

    if (vibrance.pastsattog) {
        toEdit.vibrance.pastsattog = mods.vibrance.pastsattog;
    }

    if (vibrance.skintonescurve) {
        toEdit.vibrance.skintonescurve = mods.vibrance.skintonescurve;
    }

    //if (colorBoost.amount)                    toEdit.colorBoost.amount = dontforceSet && options.baBehav[ADDSET_CBOOST_AMOUNT] ? toEdit.colorBoost.amount + mods.colorBoost.amount : mods.colorBoost.amount;
    //if (colorBoost.avoidclip)             toEdit.colorBoost.avoidclip = mods.colorBoost.avoidclip;
    //if (colorBoost.enable_saturationlimiter)toEdit.colorBoost.enable_saturationlimiter = mods.colorBoost.enable_saturationlimiter;
    //if (colorBoost.saturationlimit)           toEdit.colorBoost.saturationlimit = mods.colorBoost.saturationlimit;
    if (wb.enabled) {
        toEdit.wb.enabled = mods.wb.enabled;
    }

    if (wb.method) {
        toEdit.wb.method = mods.wb.method;
    }

    if (wb.equal) {
        toEdit.wb.equal = dontforceSet && options.baBehav[ADDSET_WB_EQUAL] ? toEdit.wb.equal + mods.wb.equal : mods.wb.equal;
    }

    if (wb.tempBias) {
        toEdit.wb.tempBias = dontforceSet && options.baBehav[ADDSET_WB_TEMPBIAS] ? toEdit.wb.tempBias + mods.wb.tempBias : mods.wb.tempBias;
    }

    if (wb.green) {
        toEdit.wb.green = dontforceSet && options.baBehav[ADDSET_WB_GREEN] ? toEdit.wb.green + mods.wb.green : mods.wb.green;
    }

    if (wb.temperature) {
        toEdit.wb.temperature = dontforceSet && options.baBehav[ADDSET_WB_TEMPERATURE] ? toEdit.wb.temperature + mods.wb.temperature : mods.wb.temperature;
    }

    //if (colorShift.a)                     toEdit.colorShift.a = dontforceSet && options.baBehav[ADDSET_CS_BLUEYELLOW] ? toEdit.colorShift.a + mods.colorShift.a : mods.colorShift.a;
    //if (colorShift.b)                     toEdit.colorShift.b = dontforceSet && options.baBehav[ADDSET_CS_GREENMAGENTA] ? toEdit.colorShift.b + mods.colorShift.b : mods.colorShift.b;
    //if (lumaDenoise.enabled)              toEdit.lumaDenoise.enabled = mods.lumaDenoise.enabled;
    //if (lumaDenoise.radius)                   toEdit.lumaDenoise.radius = mods.lumaDenoise.radius;
    //if (lumaDenoise.edgetolerance)            toEdit.lumaDenoise.edgetolerance = dontforceSet && options.baBehav[ADDSET_LD_EDGETOLERANCE] ? toEdit.lumaDenoise.edgetolerance + mods.lumaDenoise.edgetolerance : mods.lumaDenoise.edgetolerance;
    //if (colorDenoise.enabled)             toEdit.colorDenoise.enabled = mods.colorDenoise.enabled;
    //if (colorDenoise.amount)              toEdit.colorDenoise.amount = mods.colorDenoise.amount;

    if (defringe.enabled) {
        toEdit.defringe.enabled = mods.defringe.enabled;
    }

    if (defringe.radius) {
        toEdit.defringe.radius = mods.defringe.radius;
    }

    if (defringe.threshold) {
        toEdit.defringe.threshold = mods.defringe.threshold;
    }

    if (defringe.huecurve) {
        toEdit.defringe.huecurve = mods.defringe.huecurve;
    }

    if (colorappearance.curve) {
        toEdit.colorappearance.curve = mods.colorappearance.curve;
    }

    if (colorappearance.curve2) {
        toEdit.colorappearance.curve2 = mods.colorappearance.curve2;
    }

    if (colorappearance.curve3) {
        toEdit.colorappearance.curve3 = mods.colorappearance.curve3;
    }

    if (colorappearance.curveMode) {
        toEdit.colorappearance.curveMode = mods.colorappearance.curveMode;
    }

    if (colorappearance.curveMode2) {
        toEdit.colorappearance.curveMode2 = mods.colorappearance.curveMode2;
    }

    if (colorappearance.curveMode3) {
        toEdit.colorappearance.curveMode3 = mods.colorappearance.curveMode3;
    }

    if (colorappearance.enabled) {
        toEdit.colorappearance.enabled = mods.colorappearance.enabled;
    }

    if (colorappearance.degree) {
        toEdit.colorappearance.degree = dontforceSet && options.baBehav[ADDSET_CAT_DEGREE] ? toEdit.colorappearance.degree + mods.colorappearance.degree : mods.colorappearance.degree;
    }

    if (colorappearance.autodegree) {
        toEdit.colorappearance.autodegree = mods.colorappearance.autodegree;
    }

    if (colorappearance.degreeout) {
        toEdit.colorappearance.degreeout = mods.colorappearance.degreeout;
    }

    if (colorappearance.autodegreeout) {
        toEdit.colorappearance.autodegreeout = mods.colorappearance.autodegreeout;
    }

    if (colorappearance.surround) {
        toEdit.colorappearance.surround = mods.colorappearance.surround;
    }

    if (colorappearance.surrsrc) {
        toEdit.colorappearance.surrsrc = mods.colorappearance.surrsrc;
    }

    if (colorappearance.autoadapscen) {
        toEdit.colorappearance.autoadapscen = mods.colorappearance.autoadapscen;
    }

    if (colorappearance.adapscen) {
        toEdit.colorappearance.adapscen = dontforceSet && options.baBehav[ADDSET_CAT_ADAPTSCENE] ? toEdit.colorappearance.adapscen + mods.colorappearance.adapscen : mods.colorappearance.adapscen;
    }

    if (colorappearance.autoybscen) {
        toEdit.colorappearance.autoybscen = mods.colorappearance.autoybscen;
    }

    if (colorappearance.ybscen) {
        toEdit.colorappearance.ybscen = mods.colorappearance.ybscen;
    }

    if (colorappearance.adaplum) {
        toEdit.colorappearance.adaplum = dontforceSet && options.baBehav[ADDSET_CAT_ADAPTVIEWING] ? toEdit.colorappearance.adaplum + mods.colorappearance.adaplum : mods.colorappearance.adaplum;
    }

    if (colorappearance.badpixsl) {
        toEdit.colorappearance.badpixsl = dontforceSet && options.baBehav[ADDSET_CAT_BADPIX] ? toEdit.colorappearance.badpixsl + mods.colorappearance.badpixsl : mods.colorappearance.badpixsl;
    }

    if (colorappearance.wbmodel) {
        toEdit.colorappearance.wbmodel = mods.colorappearance.wbmodel;
    }

    if (colorappearance.algo) {
        toEdit.colorappearance.algo = mods.colorappearance.algo;
    }

    if (colorappearance.tempout) {
        toEdit.colorappearance.tempout = mods.colorappearance.tempout;
    }

    if (colorappearance.greenout) {
        toEdit.colorappearance.greenout = mods.colorappearance.greenout;
    }

    if (colorappearance.tempsc) {
        toEdit.colorappearance.tempsc = mods.colorappearance.tempsc;
    }

    if (colorappearance.greensc) {
        toEdit.colorappearance.greensc = mods.colorappearance.greensc;
    }

    if (colorappearance.ybout) {
        toEdit.colorappearance.ybout = mods.colorappearance.ybout;
    }

    if (colorappearance.jlight) {
        toEdit.colorappearance.jlight = dontforceSet && options.baBehav[ADDSET_CAT_LIGHT] ? toEdit.colorappearance.jlight + mods.colorappearance.jlight : mods.colorappearance.jlight;
    }

    if (colorappearance.qbright) {
        toEdit.colorappearance.qbright = dontforceSet && options.baBehav[ADDSET_CAT_BRIGHT] ? toEdit.colorappearance.qbright + mods.colorappearance.qbright : mods.colorappearance.qbright;
    }

    if (colorappearance.chroma) {
        toEdit.colorappearance.chroma = dontforceSet && options.baBehav[ADDSET_CAT_CHROMA] ? toEdit.colorappearance.chroma + mods.colorappearance.chroma : mods.colorappearance.chroma;
    }

    if (colorappearance.schroma) {
        toEdit.colorappearance.schroma = dontforceSet && options.baBehav[ADDSET_CAT_CHROMA_S] ? toEdit.colorappearance.schroma + mods.colorappearance.schroma : mods.colorappearance.schroma;
    }

    if (colorappearance.mchroma) {
        toEdit.colorappearance.mchroma = dontforceSet && options.baBehav[ADDSET_CAT_CHROMA_M] ? toEdit.colorappearance.mchroma + mods.colorappearance.mchroma : mods.colorappearance.mchroma;
    }

    if (colorappearance.contrast) {
        toEdit.colorappearance.contrast = dontforceSet && options.baBehav[ADDSET_CAT_CONTRAST] ? toEdit.colorappearance.contrast + mods.colorappearance.contrast : mods.colorappearance.contrast;
    }

    if (colorappearance.qcontrast) {
        toEdit.colorappearance.qcontrast = dontforceSet && options.baBehav[ADDSET_CAT_CONTRAST_Q] ? toEdit.colorappearance.qcontrast + mods.colorappearance.qcontrast : mods.colorappearance.qcontrast;
    }

    if (colorappearance.colorh) {
        toEdit.colorappearance.colorh = dontforceSet && options.baBehav[ADDSET_CAT_HUE] ? toEdit.colorappearance.colorh + mods.colorappearance.colorh : mods.colorappearance.colorh;
    }

    if (colorappearance.rstprotection) {
        toEdit.colorappearance.rstprotection = dontforceSet && options.baBehav[ADDSET_CAT_RSTPRO] ? toEdit.colorappearance.rstprotection + mods.colorappearance.rstprotection : mods.colorappearance.rstprotection;
    }

    if (colorappearance.surrsource) {
        toEdit.colorappearance.surrsource = mods.colorappearance.surrsource;
    }

    if (colorappearance.gamut) {
        toEdit.colorappearance.gamut = mods.colorappearance.gamut;
    }

//  if (colorappearance.badpix)             toEdit.colorappearance.badpix = mods.colorappearance.badpix;
    if (colorappearance.datacie) {
        toEdit.colorappearance.datacie = mods.colorappearance.datacie;
    }

    if (colorappearance.tonecie) {
        toEdit.colorappearance.tonecie = mods.colorappearance.tonecie;
    }

//  if (colorappearance.sharpcie)           toEdit.colorappearance.sharpcie = mods.colorappearance.sharpcie;
    if (impulseDenoise.enabled) {
        toEdit.impulseDenoise.enabled = mods.impulseDenoise.enabled;
    }

    if (impulseDenoise.thresh) {
        toEdit.impulseDenoise.thresh = mods.impulseDenoise.thresh;
    }

    if (dirpyrDenoise.enabled) {
        toEdit.dirpyrDenoise.enabled = mods.dirpyrDenoise.enabled;
    }

    if (dirpyrDenoise.enhance) {
        toEdit.dirpyrDenoise.enhance = mods.dirpyrDenoise.enhance;
    }

    if (dirpyrDenoise.median) {
        toEdit.dirpyrDenoise.median = mods.dirpyrDenoise.median;
    }

    if (dirpyrDenoise.luma) {
        toEdit.dirpyrDenoise.luma = dontforceSet && options.baBehav[ADDSET_DIRPYRDN_LUMA] ? toEdit.dirpyrDenoise.luma + mods.dirpyrDenoise.luma : mods.dirpyrDenoise.luma;
    }

    if (dirpyrDenoise.lcurve) {
        toEdit.dirpyrDenoise.lcurve = mods.dirpyrDenoise.lcurve;
    }

    if (dirpyrDenoise.cccurve) {
        toEdit.dirpyrDenoise.cccurve = mods.dirpyrDenoise.cccurve;
    }

    if (dirpyrDenoise.Ldetail) {
        toEdit.dirpyrDenoise.Ldetail = dontforceSet && options.baBehav[ADDSET_DIRPYRDN_LUMDET] ? toEdit.dirpyrDenoise.Ldetail + mods.dirpyrDenoise.Ldetail : mods.dirpyrDenoise.Ldetail;
    }

    if (dirpyrDenoise.chroma) {
        toEdit.dirpyrDenoise.chroma = dontforceSet && options.baBehav[ADDSET_DIRPYRDN_CHROMA] ? toEdit.dirpyrDenoise.chroma + mods.dirpyrDenoise.chroma : mods.dirpyrDenoise.chroma;
    }

    if (dirpyrDenoise.redchro) {
        toEdit.dirpyrDenoise.redchro = dontforceSet && options.baBehav[ADDSET_DIRPYRDN_CHROMARED] ? toEdit.dirpyrDenoise.redchro + mods.dirpyrDenoise.redchro : mods.dirpyrDenoise.redchro;
    }

    if (dirpyrDenoise.bluechro) {
        toEdit.dirpyrDenoise.bluechro = dontforceSet && options.baBehav[ADDSET_DIRPYRDN_CHROMABLUE] ? toEdit.dirpyrDenoise.bluechro + mods.dirpyrDenoise.bluechro : mods.dirpyrDenoise.bluechro;
    }

    if (dirpyrDenoise.gamma) {
        toEdit.dirpyrDenoise.gamma = dontforceSet && options.baBehav[ADDSET_DIRPYRDN_GAMMA] ? toEdit.dirpyrDenoise.gamma + mods.dirpyrDenoise.gamma : mods.dirpyrDenoise.gamma;
    }

    if (dirpyrDenoise.passes) {
        toEdit.dirpyrDenoise.passes = dontforceSet && options.baBehav[ADDSET_DIRPYRDN_PASSES] ? toEdit.dirpyrDenoise.passes + mods.dirpyrDenoise.passes : mods.dirpyrDenoise.passes;
    }

//  if (dirpyrDenoise.perform)              toEdit.dirpyrDenoise.perform = mods.dirpyrDenoise.perform;
    if (dirpyrDenoise.dmethod) {
        toEdit.dirpyrDenoise.dmethod = mods.dirpyrDenoise.dmethod;
    }

    if (dirpyrDenoise.Lmethod) {
        toEdit.dirpyrDenoise.Lmethod = mods.dirpyrDenoise.Lmethod;
    }

    if (dirpyrDenoise.Cmethod) {
        toEdit.dirpyrDenoise.Cmethod = mods.dirpyrDenoise.Cmethod;
    }

    if (dirpyrDenoise.C2method) {
        toEdit.dirpyrDenoise.C2method = mods.dirpyrDenoise.C2method;
    }

    if (dirpyrDenoise.smethod) {
        toEdit.dirpyrDenoise.smethod = mods.dirpyrDenoise.smethod;
    }

    if (dirpyrDenoise.medmethod) {
        toEdit.dirpyrDenoise.medmethod = mods.dirpyrDenoise.medmethod;
    }

    if (dirpyrDenoise.methodmed) {
        toEdit.dirpyrDenoise.methodmed = mods.dirpyrDenoise.methodmed;
    }

    if (dirpyrDenoise.rgbmethod) {
        toEdit.dirpyrDenoise.rgbmethod = mods.dirpyrDenoise.rgbmethod;
    }

    if (epd.enabled) {
        toEdit.epd.enabled = mods.epd.enabled;
    }

    if (epd.strength) {
        toEdit.epd.strength = mods.epd.strength;
    }

    if (epd.gamma) {
        toEdit.epd.gamma = mods.epd.gamma;
    }

    if (epd.edgeStopping) {
        toEdit.epd.edgeStopping = mods.epd.edgeStopping;
    }

    if (epd.scale) {
        toEdit.epd.scale = mods.epd.scale;
    }

    if (epd.reweightingIterates) {
        toEdit.epd.reweightingIterates = mods.epd.reweightingIterates;
    }

    if (fattal.enabled) {
        toEdit.fattal.enabled = mods.fattal.enabled;
    }

    if (fattal.threshold) {
        toEdit.fattal.threshold = mods.fattal.threshold;
    }

    if (fattal.amount) {
        toEdit.fattal.amount = mods.fattal.amount;
    }

    if (fattal.anchor) {
        toEdit.fattal.anchor = mods.fattal.anchor;
    }

    if (sh.enabled) {
        toEdit.sh.enabled = mods.sh.enabled;
    }

    if (sh.highlights) {
        toEdit.sh.highlights = dontforceSet && options.baBehav[ADDSET_SH_HIGHLIGHTS] ? toEdit.sh.highlights + mods.sh.highlights : mods.sh.highlights;
    }

    if (sh.htonalwidth) {
        toEdit.sh.htonalwidth = mods.sh.htonalwidth;
    }

    if (sh.shadows) {
        toEdit.sh.shadows = dontforceSet && options.baBehav[ADDSET_SH_SHADOWS] ? toEdit.sh.shadows + mods.sh.shadows : mods.sh.shadows;
    }

    if (sh.stonalwidth) {
        toEdit.sh.stonalwidth = mods.sh.stonalwidth;
    }

    if (sh.radius) {
        toEdit.sh.radius = mods.sh.radius;
    }

    if (sh.lab) {
        toEdit.sh.lab = mods.sh.lab;
    }

    if (crop.enabled) {
        toEdit.crop.enabled = mods.crop.enabled;
    }

    if (crop.x) {
        toEdit.crop.x = mods.crop.x;
    }

    if (crop.y) {
        toEdit.crop.y = mods.crop.y;
    }

    if (crop.w) {
        toEdit.crop.w = mods.crop.w;
    }

    if (crop.h) {
        toEdit.crop.h = mods.crop.h;
    }

    if (crop.fixratio) {
        toEdit.crop.fixratio = mods.crop.fixratio;
    }

    if (crop.ratio) {
        toEdit.crop.ratio = mods.crop.ratio;
    }

    if (crop.orientation) {
        toEdit.crop.orientation = mods.crop.orientation;
    }

    if (crop.guide) {
        toEdit.crop.guide = mods.crop.guide;
    }

    if (coarse.rotate) {
        toEdit.coarse.rotate = mods.coarse.rotate;
    }

    if (coarse.hflip) {
        toEdit.coarse.hflip = mods.coarse.hflip;
    }

    if (coarse.vflip) {
        toEdit.coarse.vflip = mods.coarse.vflip;
    }

    if (commonTrans.autofill) {
        toEdit.commonTrans.autofill = mods.commonTrans.autofill;
    }

    if (rotate.degree) {
        toEdit.rotate.degree = dontforceSet && options.baBehav[ADDSET_ROTATE_DEGREE] ? toEdit.rotate.degree + mods.rotate.degree : mods.rotate.degree;
    }

    if (distortion.amount) {
        toEdit.distortion.amount = dontforceSet && options.baBehav[ADDSET_DIST_AMOUNT] ? toEdit.distortion.amount + mods.distortion.amount : mods.distortion.amount;
    }

    if (lensProf.lcMode) {
        toEdit.lensProf.lcMode = mods.lensProf.lcMode;
    }

    if (lensProf.lcpFile) {
        toEdit.lensProf.lcpFile = mods.lensProf.lcpFile;
    }

    if (lensProf.useDist) {
        toEdit.lensProf.useDist = mods.lensProf.useDist;
    }

    if (lensProf.useVign) {
        toEdit.lensProf.useVign = mods.lensProf.useVign;
    }

    if (lensProf.useCA) {
        toEdit.lensProf.useCA = mods.lensProf.useCA;
    }

    if (lensProf.lfCameraMake) {
        toEdit.lensProf.lfCameraMake = mods.lensProf.lfCameraMake;
    }

    if (lensProf.lfCameraModel) {
        toEdit.lensProf.lfCameraModel = mods.lensProf.lfCameraModel;
    }

    if (lensProf.lfLens) {
        toEdit.lensProf.lfLens = mods.lensProf.lfLens;
    }

    if (perspective.horizontal) {
        toEdit.perspective.horizontal = dontforceSet && options.baBehav[ADDSET_PERSPECTIVE] ? toEdit.perspective.horizontal + mods.perspective.horizontal : mods.perspective.horizontal;
    }

    if (perspective.vertical) {
        toEdit.perspective.vertical = dontforceSet && options.baBehav[ADDSET_PERSPECTIVE] ? toEdit.perspective.vertical + mods.perspective.vertical : mods.perspective.vertical;
    }

    if (gradient.enabled) {
        toEdit.gradient.enabled = mods.gradient.enabled;
    }

    if (gradient.degree) {
        toEdit.gradient.degree = dontforceSet && options.baBehav[ADDSET_GRADIENT_DEGREE] ? toEdit.gradient.degree + mods.gradient.degree : mods.gradient.degree;
    }

    if (gradient.feather) {
        toEdit.gradient.feather = dontforceSet && options.baBehav[ADDSET_GRADIENT_FEATHER] ? toEdit.gradient.feather + mods.gradient.feather : mods.gradient.feather;
    }

    if (gradient.strength) {
        toEdit.gradient.strength = dontforceSet && options.baBehav[ADDSET_GRADIENT_STRENGTH] ? toEdit.gradient.strength + mods.gradient.strength : mods.gradient.strength;
    }

    if (gradient.centerX) {
        toEdit.gradient.centerX = dontforceSet && options.baBehav[ADDSET_GRADIENT_CENTER] ? toEdit.gradient.centerX + mods.gradient.centerX : mods.gradient.centerX;
    }

    if (gradient.centerY) {
        toEdit.gradient.centerY = dontforceSet && options.baBehav[ADDSET_GRADIENT_CENTER] ? toEdit.gradient.centerY + mods.gradient.centerY : mods.gradient.centerY;
    }


    if (locallab.enabled) {
        toEdit.locallab.enabled   = mods.locallab.enabled;
    }

    if (locallab.nbspot || locallab.id) { // For locallab to work, id order needs to be maintained when adding or deleting spots
        // Saving toEdit locallab temporarily
        LocallabParams tmpLocallab = toEdit.locallab;

        // Removing all spots in toEdit and recreating spots based on mods id
        toEdit.locallab.spots.clear();

        for (size_t i = 0; i < mods.locallab.spots.size(); i++) {
            LocallabParams::LocallabSpot *newSpot = new LocallabParams::LocallabSpot();
            newSpot->id = mods.locallab.spots.at(i).id;
            toEdit.locallab.spots.push_back(*newSpot);
        }

        // Common spots in tmpLocallab and mods are restored in toEdit
        for (size_t i = 0; i < toEdit.locallab.spots.size(); i++) {
            for (size_t j = 0; j < tmpLocallab.spots.size(); j++) {
                if (toEdit.locallab.spots.at(i).id == tmpLocallab.spots.at(j).id) {
                    toEdit.locallab.spots.at(i) = tmpLocallab.spots.at(j);
                }
            }
        }

        // Updating nbspot accordingly
        toEdit.locallab.nbspot   = mods.locallab.nbspot;
    }

    if (locallab.selspot) {
        toEdit.locallab.selspot   = mods.locallab.selspot;
    }

    for (size_t i = 0; i < toEdit.locallab.spots.size() && i < mods.locallab.spots.size() && i < locallab.spots.size(); i++) {
        // Control spot settings
        if (locallab.spots.at(i).name) {
            toEdit.locallab.spots.at(i).name = mods.locallab.spots.at(i).name;
        }

        if (locallab.spots.at(i).isvisible) {
            toEdit.locallab.spots.at(i).isvisible = mods.locallab.spots.at(i).isvisible;
        }

        if (locallab.spots.at(i).shape) {
            toEdit.locallab.spots.at(i).shape = mods.locallab.spots.at(i).shape;
        }

        if (locallab.spots.at(i).spotMethod) {
            toEdit.locallab.spots.at(i).spotMethod = mods.locallab.spots.at(i).spotMethod;
        }

        if (locallab.spots.at(i).sensiexclu) {
            toEdit.locallab.spots.at(i).sensiexclu = mods.locallab.spots.at(i).sensiexclu;
        }

        if (locallab.spots.at(i).structexclu) {
            toEdit.locallab.spots.at(i).structexclu = mods.locallab.spots.at(i).structexclu;
        }

        if (locallab.spots.at(i).struc) {
            toEdit.locallab.spots.at(i).struc = mods.locallab.spots.at(i).struc;
        }

        if (locallab.spots.at(i).shapeMethod) {
            toEdit.locallab.spots.at(i).shapeMethod = mods.locallab.spots.at(i).shapeMethod;
        }

        if (locallab.spots.at(i).locX) {
            toEdit.locallab.spots.at(i).locX = mods.locallab.spots.at(i).locX;
        }

        if (locallab.spots.at(i).locXL) {
            toEdit.locallab.spots.at(i).locXL = mods.locallab.spots.at(i).locXL;
        }

        if (locallab.spots.at(i).locY) {
            toEdit.locallab.spots.at(i).locY = mods.locallab.spots.at(i).locY;
        }

        if (locallab.spots.at(i).locYT) {
            toEdit.locallab.spots.at(i).locYT = mods.locallab.spots.at(i).locYT;
        }

        if (locallab.spots.at(i).centerX) {
            toEdit.locallab.spots.at(i).centerX = mods.locallab.spots.at(i).centerX;
        }

        if (locallab.spots.at(i).centerY) {
            toEdit.locallab.spots.at(i).centerY = mods.locallab.spots.at(i).centerY;
        }

        if (locallab.spots.at(i).circrad) {
            toEdit.locallab.spots.at(i).circrad = mods.locallab.spots.at(i).circrad;
        }

        if (locallab.spots.at(i).qualityMethod) {
            toEdit.locallab.spots.at(i).qualityMethod = mods.locallab.spots.at(i).qualityMethod;
        }

        if (locallab.spots.at(i).transit) {
            toEdit.locallab.spots.at(i).transit = mods.locallab.spots.at(i).transit;
        }


        if (locallab.spots.at(i).thresh) {
            toEdit.locallab.spots.at(i).thresh = mods.locallab.spots.at(i).thresh;
        }

        if (locallab.spots.at(i).iter) {
            toEdit.locallab.spots.at(i).iter = mods.locallab.spots.at(i).iter;
        }

        if (locallab.spots.at(i).balan) {
            toEdit.locallab.spots.at(i).balan = mods.locallab.spots.at(i).balan;
        }

        if (locallab.spots.at(i).transitweak) {
            toEdit.locallab.spots.at(i).transitweak = mods.locallab.spots.at(i).transitweak;
        }

        if (locallab.spots.at(i).transitgrad) {
            toEdit.locallab.spots.at(i).transitgrad = mods.locallab.spots.at(i).transitgrad;
        }

        if (locallab.spots.at(i).avoid) {
            toEdit.locallab.spots.at(i).avoid = mods.locallab.spots.at(i).avoid;
        }

        // Color & Light
        if (locallab.spots.at(i).expcolor) {
            toEdit.locallab.spots.at(i).expcolor = mods.locallab.spots.at(i).expcolor;
        }

        if (locallab.spots.at(i).curvactiv) {
            toEdit.locallab.spots.at(i).curvactiv = mods.locallab.spots.at(i).curvactiv;
        }

        if (locallab.spots.at(i).lightness) {
            toEdit.locallab.spots.at(i).lightness = mods.locallab.spots.at(i).lightness;
        }

        if (locallab.spots.at(i).contrast) {
            toEdit.locallab.spots.at(i).contrast = mods.locallab.spots.at(i).contrast;
        }

        if (locallab.spots.at(i).chroma) {
            toEdit.locallab.spots.at(i).chroma = mods.locallab.spots.at(i).chroma;
        }

        if (locallab.spots.at(i).labgridALow) {
            toEdit.locallab.spots.at(i).labgridALow = mods.locallab.spots.at(i).labgridALow;
        }

        if (locallab.spots.at(i).labgridBLow) {
            toEdit.locallab.spots.at(i).labgridBLow = mods.locallab.spots.at(i).labgridBLow;
        }

        if (locallab.spots.at(i).labgridAHigh) {
            toEdit.locallab.spots.at(i).labgridAHigh = mods.locallab.spots.at(i).labgridAHigh;
        }

        if (locallab.spots.at(i).labgridBHigh) {
            toEdit.locallab.spots.at(i).labgridBHigh = mods.locallab.spots.at(i).labgridBHigh;
        }

        if (locallab.spots.at(i).strengthgrid) {
            toEdit.locallab.spots.at(i).strengthgrid = mods.locallab.spots.at(i).strengthgrid;
        }

        if (locallab.spots.at(i).sensi) {
            toEdit.locallab.spots.at(i).sensi = mods.locallab.spots.at(i).sensi;
        }

        if (locallab.spots.at(i).blurcolde) {
            toEdit.locallab.spots.at(i).blurcolde = mods.locallab.spots.at(i).blurcolde;
        }

        if (locallab.spots.at(i).structcol) {
            toEdit.locallab.spots.at(i).structcol = mods.locallab.spots.at(i).structcol;
        }

        if (locallab.spots.at(i).blendmaskcol) {
            toEdit.locallab.spots.at(i).blendmaskcol = mods.locallab.spots.at(i).blendmaskcol;
        }

        if (locallab.spots.at(i).radmaskcol) {
            toEdit.locallab.spots.at(i).radmaskcol = mods.locallab.spots.at(i).radmaskcol;
        }

        if (locallab.spots.at(i).chromaskcol) {
            toEdit.locallab.spots.at(i).chromaskcol = mods.locallab.spots.at(i).chromaskcol;
        }

        if (locallab.spots.at(i).gammaskcol) {
            toEdit.locallab.spots.at(i).gammaskcol = mods.locallab.spots.at(i).gammaskcol;
        }

        if (locallab.spots.at(i).slomaskcol) {
            toEdit.locallab.spots.at(i).slomaskcol = mods.locallab.spots.at(i).slomaskcol;
        }

        if (locallab.spots.at(i).qualitycurveMethod) {
            toEdit.locallab.spots.at(i).qualitycurveMethod = mods.locallab.spots.at(i).qualitycurveMethod;
        }

        if (locallab.spots.at(i).gridMethod) {
            toEdit.locallab.spots.at(i).gridMethod = mods.locallab.spots.at(i).gridMethod;
        }

        if (locallab.spots.at(i).llcurve) {
            toEdit.locallab.spots.at(i).llcurve = mods.locallab.spots.at(i).llcurve;
        }

        if (locallab.spots.at(i).cccurve) {
            toEdit.locallab.spots.at(i).cccurve = mods.locallab.spots.at(i).cccurve;
        }

        if (locallab.spots.at(i).LHcurve) {
            toEdit.locallab.spots.at(i).LHcurve = mods.locallab.spots.at(i).LHcurve;
        }

        if (locallab.spots.at(i).HHcurve) {
            toEdit.locallab.spots.at(i).HHcurve = mods.locallab.spots.at(i).HHcurve;
        }

        if (locallab.spots.at(i).invers) {
            toEdit.locallab.spots.at(i).invers = mods.locallab.spots.at(i).invers;
        }

        if (locallab.spots.at(i).enaColorMask) {
            toEdit.locallab.spots.at(i).enaColorMask = mods.locallab.spots.at(i).enaColorMask;
        }

        if (locallab.spots.at(i).CCmaskcurve) {
            toEdit.locallab.spots.at(i).CCmaskcurve = mods.locallab.spots.at(i).CCmaskcurve;
        }

        if (locallab.spots.at(i).LLmaskcurve) {
            toEdit.locallab.spots.at(i).LLmaskcurve = mods.locallab.spots.at(i).LLmaskcurve;
        }

        if (locallab.spots.at(i).HHmaskcurve) {
            toEdit.locallab.spots.at(i).HHmaskcurve = mods.locallab.spots.at(i).HHmaskcurve;
        }

        if (locallab.spots.at(i).softradiuscol) {
            toEdit.locallab.spots.at(i).softradiuscol = mods.locallab.spots.at(i).softradiuscol;
        }

        // Exposure
        if (locallab.spots.at(i).expexpose) {
            toEdit.locallab.spots.at(i).expexpose = mods.locallab.spots.at(i).expexpose;
        }

        if (locallab.spots.at(i).expcomp) {
            toEdit.locallab.spots.at(i).expcomp = mods.locallab.spots.at(i).expcomp;
        }

        if (locallab.spots.at(i).hlcompr) {
            toEdit.locallab.spots.at(i).hlcompr = mods.locallab.spots.at(i).hlcompr;
        }

        if (locallab.spots.at(i).hlcomprthresh) {
            toEdit.locallab.spots.at(i).hlcomprthresh = mods.locallab.spots.at(i).hlcomprthresh;
        }

        if (locallab.spots.at(i).black) {
            toEdit.locallab.spots.at(i).black = mods.locallab.spots.at(i).black;
        }

        if (locallab.spots.at(i).shadex) {
            toEdit.locallab.spots.at(i).shadex = mods.locallab.spots.at(i).shadex;
        }

        if (locallab.spots.at(i).expchroma) {
            toEdit.locallab.spots.at(i).expchroma = mods.locallab.spots.at(i).expchroma;
        }

        if (locallab.spots.at(i).warm) {
            toEdit.locallab.spots.at(i).warm = mods.locallab.spots.at(i).warm;
        }

        if (locallab.spots.at(i).sensiex) {
            toEdit.locallab.spots.at(i).sensiex = mods.locallab.spots.at(i).sensiex;
        }

        if (locallab.spots.at(i).structexp) {
            toEdit.locallab.spots.at(i).structexp = mods.locallab.spots.at(i).structexp;
        }

        if (locallab.spots.at(i).blurexpde) {
            toEdit.locallab.spots.at(i).blurexpde = mods.locallab.spots.at(i).blurexpde;
        }

        if (locallab.spots.at(i).excurve) {
            toEdit.locallab.spots.at(i).excurve = mods.locallab.spots.at(i).excurve;
        }

        if (locallab.spots.at(i).inversex) {
            toEdit.locallab.spots.at(i).inversex = mods.locallab.spots.at(i).inversex;
        }

        if (locallab.spots.at(i).enaExpMask) {
            toEdit.locallab.spots.at(i).enaExpMask = mods.locallab.spots.at(i).enaExpMask;
        }

        if (locallab.spots.at(i).CCmaskexpcurve) {
            toEdit.locallab.spots.at(i).CCmaskexpcurve = mods.locallab.spots.at(i).CCmaskexpcurve;
        }

        if (locallab.spots.at(i).LLmaskexpcurve) {
            toEdit.locallab.spots.at(i).LLmaskexpcurve = mods.locallab.spots.at(i).LLmaskexpcurve;
        }

        if (locallab.spots.at(i).HHmaskexpcurve) {
            toEdit.locallab.spots.at(i).HHmaskexpcurve = mods.locallab.spots.at(i).HHmaskexpcurve;
        }

        if (locallab.spots.at(i).blendmaskexp) {
            toEdit.locallab.spots.at(i).blendmaskexp = mods.locallab.spots.at(i).blendmaskexp;
        }

        if (locallab.spots.at(i).radmaskexp) {
            toEdit.locallab.spots.at(i).radmaskexp = mods.locallab.spots.at(i).radmaskexp;
        }

        if (locallab.spots.at(i).chromaskexp) {
            toEdit.locallab.spots.at(i).chromaskexp = mods.locallab.spots.at(i).chromaskexp;
        }

        if (locallab.spots.at(i).gammaskexp) {
            toEdit.locallab.spots.at(i).gammaskexp = mods.locallab.spots.at(i).gammaskexp;
        }

        if (locallab.spots.at(i).slomaskexp) {
            toEdit.locallab.spots.at(i).slomaskexp = mods.locallab.spots.at(i).slomaskexp;
        }

        if (locallab.spots.at(i).softradiusexp) {
            toEdit.locallab.spots.at(i).softradiusexp = mods.locallab.spots.at(i).softradiusexp;
        }

        if (locallab.spots.at(i).expMethod) {
            toEdit.locallab.spots.at(i).expMethod = mods.locallab.spots.at(i).expMethod;
        }

        if (locallab.spots.at(i).laplacexp) {
            toEdit.locallab.spots.at(i).laplacexp = mods.locallab.spots.at(i).laplacexp;
        }

        if (locallab.spots.at(i).balanexp) {
            toEdit.locallab.spots.at(i).balanexp = mods.locallab.spots.at(i).balanexp;
        }

        if (locallab.spots.at(i).linear) {
            toEdit.locallab.spots.at(i).linear = mods.locallab.spots.at(i).linear;
        }

        if (locallab.spots.at(i).gamm) {
            toEdit.locallab.spots.at(i).gamm = mods.locallab.spots.at(i).gamm;
        }

        if (locallab.spots.at(i).fatamount) {
            toEdit.locallab.spots.at(i).fatamount = mods.locallab.spots.at(i).fatamount;
        }

        if (locallab.spots.at(i).fatdetail) {
            toEdit.locallab.spots.at(i).fatdetail = mods.locallab.spots.at(i).fatdetail;
        }

        if (locallab.spots.at(i).fatanchor) {
            toEdit.locallab.spots.at(i).fatanchor = mods.locallab.spots.at(i).fatanchor;
        }

        // Shadow highlight
        if (locallab.spots.at(i).expshadhigh) {
            toEdit.locallab.spots.at(i).expshadhigh = mods.locallab.spots.at(i).expshadhigh;
        }

        if (locallab.spots.at(i).highlights) {
            toEdit.locallab.spots.at(i).highlights = mods.locallab.spots.at(i).highlights;
        }

        if (locallab.spots.at(i).h_tonalwidth) {
            toEdit.locallab.spots.at(i).h_tonalwidth = mods.locallab.spots.at(i).h_tonalwidth;
        }

        if (locallab.spots.at(i).shadows) {
            toEdit.locallab.spots.at(i).shadows = mods.locallab.spots.at(i).shadows;
        }

        if (locallab.spots.at(i).s_tonalwidth) {
            toEdit.locallab.spots.at(i).s_tonalwidth = mods.locallab.spots.at(i).s_tonalwidth;
        }

        if (locallab.spots.at(i).sh_radius) {
            toEdit.locallab.spots.at(i).sh_radius = mods.locallab.spots.at(i).sh_radius;
        }

        if (locallab.spots.at(i).sensihs) {
            toEdit.locallab.spots.at(i).sensihs = mods.locallab.spots.at(i).sensihs;
        }

        if (locallab.spots.at(i).enaSHMask) {
            toEdit.locallab.spots.at(i).enaSHMask = mods.locallab.spots.at(i).enaSHMask;
        }

        if (locallab.spots.at(i).CCmaskSHcurve) {
            toEdit.locallab.spots.at(i).CCmaskSHcurve = mods.locallab.spots.at(i).CCmaskSHcurve;
        }

        if (locallab.spots.at(i).LLmaskSHcurve) {
            toEdit.locallab.spots.at(i).LLmaskSHcurve = mods.locallab.spots.at(i).LLmaskSHcurve;
        }

        if (locallab.spots.at(i).HHmaskSHcurve) {
            toEdit.locallab.spots.at(i).HHmaskSHcurve = mods.locallab.spots.at(i).HHmaskSHcurve;
        }

        if (locallab.spots.at(i).blendmaskSH) {
            toEdit.locallab.spots.at(i).blendmaskSH = mods.locallab.spots.at(i).blendmaskSH;
        }

        if (locallab.spots.at(i).radmaskSH) {
            toEdit.locallab.spots.at(i).radmaskSH = mods.locallab.spots.at(i).radmaskSH;
        }

        if (locallab.spots.at(i).blurSHde) {
            toEdit.locallab.spots.at(i).blurSHde = mods.locallab.spots.at(i).blurSHde;
        }

        if (locallab.spots.at(i).inverssh) {
            toEdit.locallab.spots.at(i).inverssh = mods.locallab.spots.at(i).inverssh;
        }

        if (locallab.spots.at(i).chromaskSH) {
            toEdit.locallab.spots.at(i).chromaskSH = mods.locallab.spots.at(i).chromaskSH;
        }

        if (locallab.spots.at(i).gammaskSH) {
            toEdit.locallab.spots.at(i).gammaskSH = mods.locallab.spots.at(i).gammaskSH;
        }

        if (locallab.spots.at(i).slomaskSH) {
            toEdit.locallab.spots.at(i).slomaskSH = mods.locallab.spots.at(i).slomaskSH;
        }

        // Vibrance
        if (locallab.spots.at(i).expvibrance) {
            toEdit.locallab.spots.at(i).expvibrance = mods.locallab.spots.at(i).expvibrance;
        }

        if (locallab.spots.at(i).saturated) {
            toEdit.locallab.spots.at(i).saturated = mods.locallab.spots.at(i).saturated;
        }

        if (locallab.spots.at(i).pastels) {
            toEdit.locallab.spots.at(i).pastels = mods.locallab.spots.at(i).pastels;
        }

        if (locallab.spots.at(i).psthreshold) {
            toEdit.locallab.spots.at(i).psthreshold = mods.locallab.spots.at(i).psthreshold;
        }

        if (locallab.spots.at(i).protectskins) {
            toEdit.locallab.spots.at(i).protectskins = mods.locallab.spots.at(i).protectskins;
        }

        if (locallab.spots.at(i).avoidcolorshift) {
            toEdit.locallab.spots.at(i).avoidcolorshift = mods.locallab.spots.at(i).avoidcolorshift;
        }

        if (locallab.spots.at(i).pastsattog) {
            toEdit.locallab.spots.at(i).pastsattog = mods.locallab.spots.at(i).pastsattog;
        }

        if (locallab.spots.at(i).sensiv) {
            toEdit.locallab.spots.at(i).sensiv = mods.locallab.spots.at(i).sensiv;
        }

        if (locallab.spots.at(i).skintonescurve) {
            toEdit.locallab.spots.at(i).skintonescurve = mods.locallab.spots.at(i).skintonescurve;
        }

        // Soft Light
        if (locallab.spots.at(i).expsoft) {
            toEdit.locallab.spots.at(i).expsoft = mods.locallab.spots.at(i).expsoft;
        }

        if (locallab.spots.at(i).streng) {
            toEdit.locallab.spots.at(i).streng = mods.locallab.spots.at(i).streng;
        }

        if (locallab.spots.at(i).sensisf) {
            toEdit.locallab.spots.at(i).sensisf = mods.locallab.spots.at(i).sensisf;
        }

        if (locallab.spots.at(i).laplace) {
            toEdit.locallab.spots.at(i).laplace = mods.locallab.spots.at(i).laplace;
        }

        if (locallab.spots.at(i).softMethod) {
            toEdit.locallab.spots.at(i).softMethod = mods.locallab.spots.at(i).softMethod;
        }

        // Blur & Noise
        if (locallab.spots.at(i).expblur) {
            toEdit.locallab.spots.at(i).expblur = mods.locallab.spots.at(i).expblur;
        }

        if (locallab.spots.at(i).radius) {
            toEdit.locallab.spots.at(i).radius = mods.locallab.spots.at(i).radius;
        }

        if (locallab.spots.at(i).strength) {
            toEdit.locallab.spots.at(i).strength = mods.locallab.spots.at(i).strength;
        }

        if (locallab.spots.at(i).sensibn) {
            toEdit.locallab.spots.at(i).sensibn = mods.locallab.spots.at(i).sensibn;
        }

        if (locallab.spots.at(i).itera) {
            toEdit.locallab.spots.at(i).itera = mods.locallab.spots.at(i).itera;
        }

        if (locallab.spots.at(i).guidbl) {
            toEdit.locallab.spots.at(i).guidbl = mods.locallab.spots.at(i).guidbl;
        }

        if (locallab.spots.at(i).epsbl) {
            toEdit.locallab.spots.at(i).epsbl = mods.locallab.spots.at(i).epsbl;
        }

        if (locallab.spots.at(i).blMethod) {
            toEdit.locallab.spots.at(i).blMethod = mods.locallab.spots.at(i).blMethod;
        }

        if (locallab.spots.at(i).blurMethod) {
            toEdit.locallab.spots.at(i).blurMethod = mods.locallab.spots.at(i).blurMethod;
        }

        if (locallab.spots.at(i).medMethod) {
            toEdit.locallab.spots.at(i).medMethod = mods.locallab.spots.at(i).medMethod;
        }

        if (locallab.spots.at(i).activlum) {
            toEdit.locallab.spots.at(i).activlum = mods.locallab.spots.at(i).activlum;
        }

        if (locallab.spots.at(i).CCmaskblcurve) {
            toEdit.locallab.spots.at(i).CCmaskblcurve = mods.locallab.spots.at(i).CCmaskblcurve;
        }

        if (locallab.spots.at(i).LLmaskblcurve) {
            toEdit.locallab.spots.at(i).LLmaskblcurve = mods.locallab.spots.at(i).LLmaskblcurve;
        }

        if (locallab.spots.at(i).HHmaskblcurve) {
            toEdit.locallab.spots.at(i).HHmaskblcurve = mods.locallab.spots.at(i).HHmaskblcurve;
        }

        if (locallab.spots.at(i).enablMask) {
            toEdit.locallab.spots.at(i).enablMask = mods.locallab.spots.at(i).enablMask;
        }

        if (locallab.spots.at(i).blendmaskbl) {
            toEdit.locallab.spots.at(i).blendmaskbl = mods.locallab.spots.at(i).blendmaskbl;
        }

        if (locallab.spots.at(i).radmaskbl) {
            toEdit.locallab.spots.at(i).radmaskbl = mods.locallab.spots.at(i).radmaskbl;
        }

        if (locallab.spots.at(i).chromaskbl) {
            toEdit.locallab.spots.at(i).chromaskbl = mods.locallab.spots.at(i).chromaskbl;
        }

        if (locallab.spots.at(i).gammaskbl) {
            toEdit.locallab.spots.at(i).gammaskbl = mods.locallab.spots.at(i).gammaskbl;
        }

        if (locallab.spots.at(i).slomaskbl) {
            toEdit.locallab.spots.at(i).slomaskbl = mods.locallab.spots.at(i).slomaskbl;
        }

        // Tone Mapping
        if (locallab.spots.at(i).exptonemap) {
            toEdit.locallab.spots.at(i).exptonemap = mods.locallab.spots.at(i).exptonemap;
        }

        if (locallab.spots.at(i).stren) {
            toEdit.locallab.spots.at(i).stren = mods.locallab.spots.at(i).stren;
        }

        if (locallab.spots.at(i).gamma) {
            toEdit.locallab.spots.at(i).gamma = mods.locallab.spots.at(i).gamma;
        }

        if (locallab.spots.at(i).equiltm) {
            toEdit.locallab.spots.at(i).equiltm = mods.locallab.spots.at(i).equiltm;
        }

        if (locallab.spots.at(i).estop) {
            toEdit.locallab.spots.at(i).estop = mods.locallab.spots.at(i).estop;
        }

        if (locallab.spots.at(i).scaltm) {
            toEdit.locallab.spots.at(i).scaltm = mods.locallab.spots.at(i).scaltm;
        }

        if (locallab.spots.at(i).rewei) {
            toEdit.locallab.spots.at(i).rewei = mods.locallab.spots.at(i).rewei;
        }

        if (locallab.spots.at(i).satur) {
            toEdit.locallab.spots.at(i).satur = mods.locallab.spots.at(i).satur;
        }

        if (locallab.spots.at(i).sensitm) {
            toEdit.locallab.spots.at(i).sensitm = mods.locallab.spots.at(i).sensitm;
        }

        if (locallab.spots.at(i).softradiustm) {
            toEdit.locallab.spots.at(i).softradiustm = mods.locallab.spots.at(i).softradiustm;
        }

        if (locallab.spots.at(i).amount) {
            toEdit.locallab.spots.at(i).amount = mods.locallab.spots.at(i).amount;
        }

        if (locallab.spots.at(i).CCmasktmcurve) {
            toEdit.locallab.spots.at(i).CCmasktmcurve = mods.locallab.spots.at(i).CCmasktmcurve;
        }

        if (locallab.spots.at(i).LLmasktmcurve) {
            toEdit.locallab.spots.at(i).LLmasktmcurve = mods.locallab.spots.at(i).LLmasktmcurve;
        }

        if (locallab.spots.at(i).HHmasktmcurve) {
            toEdit.locallab.spots.at(i).HHmasktmcurve = mods.locallab.spots.at(i).HHmasktmcurve;
        }

        if (locallab.spots.at(i).enatmMask) {
            toEdit.locallab.spots.at(i).enatmMask = mods.locallab.spots.at(i).enatmMask;
        }

        if (locallab.spots.at(i).blendmasktm) {
            toEdit.locallab.spots.at(i).blendmasktm = mods.locallab.spots.at(i).blendmasktm;
        }

        if (locallab.spots.at(i).radmasktm) {
            toEdit.locallab.spots.at(i).radmasktm = mods.locallab.spots.at(i).radmasktm;
        }

        if (locallab.spots.at(i).chromasktm) {
            toEdit.locallab.spots.at(i).chromasktm = mods.locallab.spots.at(i).chromasktm;
        }

        if (locallab.spots.at(i).gammasktm) {
            toEdit.locallab.spots.at(i).gammasktm = mods.locallab.spots.at(i).gammasktm;
        }

        if (locallab.spots.at(i).slomasktm) {
            toEdit.locallab.spots.at(i).slomasktm = mods.locallab.spots.at(i).slomasktm;
        }

        // Retinex
        if (locallab.spots.at(i).expreti) {
            toEdit.locallab.spots.at(i).expreti = mods.locallab.spots.at(i).expreti;
        }

        if (locallab.spots.at(i).retinexMethod) {
            toEdit.locallab.spots.at(i).retinexMethod = mods.locallab.spots.at(i).retinexMethod;
        }

        if (locallab.spots.at(i).str) {
            toEdit.locallab.spots.at(i).str = mods.locallab.spots.at(i).str;
        }

        if (locallab.spots.at(i).chrrt) {
            toEdit.locallab.spots.at(i).chrrt = mods.locallab.spots.at(i).chrrt;
        }

        if (locallab.spots.at(i).neigh) {
            toEdit.locallab.spots.at(i).neigh = mods.locallab.spots.at(i).neigh;
        }

        if (locallab.spots.at(i).vart) {
            toEdit.locallab.spots.at(i).vart = mods.locallab.spots.at(i).vart;
        }

        if (locallab.spots.at(i).dehaz) {
            toEdit.locallab.spots.at(i).dehaz = mods.locallab.spots.at(i).dehaz;
        }

        if (locallab.spots.at(i).sensih) {
            toEdit.locallab.spots.at(i).sensih = mods.locallab.spots.at(i).sensih;
        }

        if (locallab.spots.at(i).softradiusret) {
            toEdit.locallab.spots.at(i).softradiusret = mods.locallab.spots.at(i).softradiusret;
        }

        if (locallab.spots.at(i).localTgaincurve) {
            toEdit.locallab.spots.at(i).localTgaincurve = mods.locallab.spots.at(i).localTgaincurve;
        }

        if (locallab.spots.at(i).inversret) {
            toEdit.locallab.spots.at(i).inversret = mods.locallab.spots.at(i).inversret;
        }

        if (locallab.spots.at(i).equilret) {
            toEdit.locallab.spots.at(i).equilret = mods.locallab.spots.at(i).equilret;
        }

        if (locallab.spots.at(i).CCmaskreticurve) {
            toEdit.locallab.spots.at(i).CCmaskreticurve = mods.locallab.spots.at(i).CCmaskreticurve;
        }

        if (locallab.spots.at(i).LLmaskreticurve) {
            toEdit.locallab.spots.at(i).LLmaskreticurve = mods.locallab.spots.at(i).LLmaskreticurve;
        }

        if (locallab.spots.at(i).HHmaskreticurve) {
            toEdit.locallab.spots.at(i).HHmaskreticurve = mods.locallab.spots.at(i).HHmaskreticurve;
        }

        if (locallab.spots.at(i).enaretiMask) {
            toEdit.locallab.spots.at(i).enaretiMask = mods.locallab.spots.at(i).enaretiMask;
        }

        if (locallab.spots.at(i).enaretiMasktmap) {
            toEdit.locallab.spots.at(i).enaretiMasktmap = mods.locallab.spots.at(i).enaretiMasktmap;
        }

        if (locallab.spots.at(i).blendmaskreti) {
            toEdit.locallab.spots.at(i).blendmaskreti = mods.locallab.spots.at(i).blendmaskreti;
        }

        if (locallab.spots.at(i).radmaskreti) {
            toEdit.locallab.spots.at(i).radmaskreti = mods.locallab.spots.at(i).radmaskreti;
        }

        if (locallab.spots.at(i).chromaskreti) {
            toEdit.locallab.spots.at(i).chromaskreti = mods.locallab.spots.at(i).chromaskreti;
        }

        if (locallab.spots.at(i).gammaskreti) {
            toEdit.locallab.spots.at(i).gammaskreti = mods.locallab.spots.at(i).gammaskreti;
        }

        if (locallab.spots.at(i).slomaskreti) {
            toEdit.locallab.spots.at(i).slomaskreti = mods.locallab.spots.at(i).slomaskreti;
        }

        if (locallab.spots.at(i).scalereti) {
            toEdit.locallab.spots.at(i).scalereti = mods.locallab.spots.at(i).scalereti;
        }

        if (locallab.spots.at(i).darkness) {
            toEdit.locallab.spots.at(i).darkness = mods.locallab.spots.at(i).darkness;
        }

        if (locallab.spots.at(i).lightnessreti) {
            toEdit.locallab.spots.at(i).lightnessreti = mods.locallab.spots.at(i).lightnessreti;
        }

        if (locallab.spots.at(i).limd) {
            toEdit.locallab.spots.at(i).limd = mods.locallab.spots.at(i).limd;
        }

        if (locallab.spots.at(i).fftwreti) {
            toEdit.locallab.spots.at(i).fftwreti = mods.locallab.spots.at(i).fftwreti;
        }

        // Sharpening
        if (locallab.spots.at(i).expsharp) {
            toEdit.locallab.spots.at(i).expsharp = mods.locallab.spots.at(i).expsharp;
        }

        if (locallab.spots.at(i).sharcontrast) {
            toEdit.locallab.spots.at(i).sharcontrast    = mods.locallab.spots.at(i).sharcontrast;
        }

        if (locallab.spots.at(i).sharradius) {
            toEdit.locallab.spots.at(i).sharradius = mods.locallab.spots.at(i).sharradius;
        }

        if (locallab.spots.at(i).sharamount) {
            toEdit.locallab.spots.at(i).sharamount = mods.locallab.spots.at(i).sharamount;
        }

        if (locallab.spots.at(i).shardamping) {
            toEdit.locallab.spots.at(i).shardamping = mods.locallab.spots.at(i).shardamping;
        }

        if (locallab.spots.at(i).shariter) {
            toEdit.locallab.spots.at(i).shariter = mods.locallab.spots.at(i).shariter;
        }

        if (locallab.spots.at(i).sharblur) {
            toEdit.locallab.spots.at(i).sharblur    = mods.locallab.spots.at(i).sharblur;
        }

        if (locallab.spots.at(i).sensisha) {
            toEdit.locallab.spots.at(i).sensisha = mods.locallab.spots.at(i).sensisha;
        }

        if (locallab.spots.at(i).inverssha) {
            toEdit.locallab.spots.at(i).inverssha = mods.locallab.spots.at(i).inverssha;
        }

        // Local Contrast
        if (locallab.spots.at(i).expcontrast) {
            toEdit.locallab.spots.at(i).expcontrast   = mods.locallab.spots.at(i).expcontrast;
        }

        if (locallab.spots.at(i).lcradius) {
            toEdit.locallab.spots.at(i).lcradius   = mods.locallab.spots.at(i).lcradius;
        }

        if (locallab.spots.at(i).lcamount) {
            toEdit.locallab.spots.at(i).lcamount   = mods.locallab.spots.at(i).lcamount;
        }

        if (locallab.spots.at(i).lcdarkness) {
            toEdit.locallab.spots.at(i).lcdarkness   = mods.locallab.spots.at(i).lcdarkness;
        }

        if (locallab.spots.at(i).lclightness) {
            toEdit.locallab.spots.at(i).lclightness   = mods.locallab.spots.at(i).lclightness;
        }

        if (locallab.spots.at(i).levelwav) {
            toEdit.locallab.spots.at(i).levelwav   = mods.locallab.spots.at(i).levelwav;
        }

        if (locallab.spots.at(i).residcont) {
            toEdit.locallab.spots.at(i).residcont   = mods.locallab.spots.at(i).residcont;
        }

        if (locallab.spots.at(i).residchro) {
            toEdit.locallab.spots.at(i).residchro   = mods.locallab.spots.at(i).residchro;
        }

        if (locallab.spots.at(i).clarilres) {
            toEdit.locallab.spots.at(i).clarilres   = mods.locallab.spots.at(i).clarilres;
        }

        if (locallab.spots.at(i).claricres) {
            toEdit.locallab.spots.at(i).claricres   = mods.locallab.spots.at(i).claricres;
        }

        if (locallab.spots.at(i).clarisoft) {
            toEdit.locallab.spots.at(i).clarisoft   = mods.locallab.spots.at(i).clarisoft;
        }

        if (locallab.spots.at(i).sensilc) {
            toEdit.locallab.spots.at(i).sensilc   = mods.locallab.spots.at(i).sensilc;
        }

        if (locallab.spots.at(i).fftwlc) {
            toEdit.locallab.spots.at(i).fftwlc = mods.locallab.spots.at(i).fftwlc;
        }

        if (locallab.spots.at(i).localcontMethod) {
            toEdit.locallab.spots.at(i).localcontMethod = mods.locallab.spots.at(i).localcontMethod;
        }

        if (locallab.spots.at(i).locwavcurve) {
            toEdit.locallab.spots.at(i).locwavcurve = mods.locallab.spots.at(i).locwavcurve;
        }

        // Contrast by detail levels
        if (locallab.spots.at(i).expcbdl) {
            toEdit.locallab.spots.at(i).expcbdl = mods.locallab.spots.at(i).expcbdl;
        }

        for (int j = 0; j < 6; j++) {
            if (locallab.spots.at(i).mult[j]) {
                toEdit.locallab.spots.at(i).mult[j] = mods.locallab.spots.at(i).mult[j];
            }
        }

        if (locallab.spots.at(i).chromacbdl) {
            toEdit.locallab.spots.at(i).chromacbdl = mods.locallab.spots.at(i).chromacbdl;
        }

        if (locallab.spots.at(i).threshold) {
            toEdit.locallab.spots.at(i).threshold = mods.locallab.spots.at(i).threshold;
        }

        if (locallab.spots.at(i).sensicb) {
            toEdit.locallab.spots.at(i).sensicb = mods.locallab.spots.at(i).sensicb;
        }

        if (locallab.spots.at(i).clarityml) {
            toEdit.locallab.spots.at(i).clarityml = mods.locallab.spots.at(i).clarityml;
        }

        if (locallab.spots.at(i).contresid) {
            toEdit.locallab.spots.at(i).contresid = mods.locallab.spots.at(i).contresid;
        }

        if (locallab.spots.at(i).blurcbdl) {
            toEdit.locallab.spots.at(i).blurcbdl = mods.locallab.spots.at(i).blurcbdl;
        }

        if (locallab.spots.at(i).blendmaskcb) {
            toEdit.locallab.spots.at(i).blendmaskcb = mods.locallab.spots.at(i).blendmaskcb;
        }

        if (locallab.spots.at(i).radmaskcb) {
            toEdit.locallab.spots.at(i).radmaskcb = mods.locallab.spots.at(i).radmaskcb;
        }

        if (locallab.spots.at(i).chromaskcb) {
            toEdit.locallab.spots.at(i).chromaskcb = mods.locallab.spots.at(i).chromaskcb;
        }

        if (locallab.spots.at(i).gammaskcb) {
            toEdit.locallab.spots.at(i).gammaskcb = mods.locallab.spots.at(i).gammaskcb;
        }

        if (locallab.spots.at(i).slomaskcb) {
            toEdit.locallab.spots.at(i).slomaskcb = mods.locallab.spots.at(i).slomaskcb;
        }

        if (locallab.spots.at(i).enacbMask) {
            toEdit.locallab.spots.at(i).enacbMask = mods.locallab.spots.at(i).enacbMask;
        }

        if (locallab.spots.at(i).CCmaskcbcurve) {
            toEdit.locallab.spots.at(i).CCmaskcbcurve = mods.locallab.spots.at(i).CCmaskcbcurve;
        }

        if (locallab.spots.at(i).LLmaskcbcurve) {
            toEdit.locallab.spots.at(i).LLmaskcbcurve = mods.locallab.spots.at(i).LLmaskcbcurve;
        }

        if (locallab.spots.at(i).HHmaskcbcurve) {
            toEdit.locallab.spots.at(i).HHmaskcbcurve = mods.locallab.spots.at(i).HHmaskcbcurve;
        }

        if (locallab.spots.at(i).softradiuscb) {
            toEdit.locallab.spots.at(i).softradiuscb = mods.locallab.spots.at(i).softradiuscb;
        }

        // Denoise
        if (locallab.spots.at(i).expdenoi) {
            toEdit.locallab.spots.at(i).expdenoi = mods.locallab.spots.at(i).expdenoi;
        }

        if (locallab.spots.at(i).noiselumf) {
            toEdit.locallab.spots.at(i).noiselumf = mods.locallab.spots.at(i).noiselumf;
        }

        if (locallab.spots.at(i).noiselumf0) {
            toEdit.locallab.spots.at(i).noiselumf0 = mods.locallab.spots.at(i).noiselumf0;
        }

        if (locallab.spots.at(i).noiselumf2) {
            toEdit.locallab.spots.at(i).noiselumf2 = mods.locallab.spots.at(i).noiselumf2;
        }

        if (locallab.spots.at(i).noiselumc) {
            toEdit.locallab.spots.at(i).noiselumc = mods.locallab.spots.at(i).noiselumc;
        }

        if (locallab.spots.at(i).noiselumdetail) {
            toEdit.locallab.spots.at(i).noiselumdetail = mods.locallab.spots.at(i).noiselumdetail;
        }

        if (locallab.spots.at(i).noiselequal) {
            toEdit.locallab.spots.at(i).noiselequal = mods.locallab.spots.at(i).noiselequal;
        }

        if (locallab.spots.at(i).noisechrof) {
            toEdit.locallab.spots.at(i).noisechrof = mods.locallab.spots.at(i).noisechrof;
        }

        if (locallab.spots.at(i).noisechroc) {
            toEdit.locallab.spots.at(i).noisechroc = mods.locallab.spots.at(i).noisechroc;
        }

        if (locallab.spots.at(i).noisechrodetail) {
            toEdit.locallab.spots.at(i).noisechrodetail = mods.locallab.spots.at(i).noisechrodetail;
        }

        if (locallab.spots.at(i).adjblur) {
            toEdit.locallab.spots.at(i).adjblur = mods.locallab.spots.at(i).adjblur;
        }

        if (locallab.spots.at(i).bilateral) {
            toEdit.locallab.spots.at(i).bilateral = mods.locallab.spots.at(i).bilateral;
        }

        if (locallab.spots.at(i).sensiden) {
            toEdit.locallab.spots.at(i).sensiden = mods.locallab.spots.at(i).sensiden;
        }
    }


    if (pcvignette.enabled) {
        toEdit.pcvignette.enabled = mods.pcvignette.enabled;
    }

    if (pcvignette.strength) {
        toEdit.pcvignette.strength = dontforceSet && options.baBehav[ADDSET_PCVIGNETTE_STRENGTH] ? toEdit.pcvignette.strength + mods.pcvignette.strength : mods.pcvignette.strength;
    }

    if (pcvignette.feather) {
        toEdit.pcvignette.feather = dontforceSet && options.baBehav[ADDSET_PCVIGNETTE_FEATHER] ? toEdit.pcvignette.feather + mods.pcvignette.feather : mods.pcvignette.feather;
    }

    if (pcvignette.roundness) {
        toEdit.pcvignette.roundness = dontforceSet && options.baBehav[ADDSET_PCVIGNETTE_ROUNDNESS] ? toEdit.pcvignette.roundness + mods.pcvignette.roundness : mods.pcvignette.roundness;
    }

    if (cacorrection.red) {
        toEdit.cacorrection.red = dontforceSet && options.baBehav[ADDSET_CA] ? toEdit.cacorrection.red + mods.cacorrection.red : mods.cacorrection.red;
    }

    if (cacorrection.blue) {
        toEdit.cacorrection.blue = dontforceSet && options.baBehav[ADDSET_CA] ? toEdit.cacorrection.blue + mods.cacorrection.blue : mods.cacorrection.blue;
    }

    if (vignetting.amount) {
        toEdit.vignetting.amount = dontforceSet && options.baBehav[ADDSET_VIGN_AMOUNT] ? toEdit.vignetting.amount + mods.vignetting.amount : mods.vignetting.amount;
    }

    if (vignetting.radius) {
        toEdit.vignetting.radius = dontforceSet && options.baBehav[ADDSET_VIGN_RADIUS] ? toEdit.vignetting.radius + mods.vignetting.radius : mods.vignetting.radius;
    }

    if (vignetting.strength) {
        toEdit.vignetting.strength = dontforceSet && options.baBehav[ADDSET_VIGN_STRENGTH] ? toEdit.vignetting.strength + mods.vignetting.strength : mods.vignetting.strength;
    }

    if (vignetting.centerX) {
        toEdit.vignetting.centerX = dontforceSet && options.baBehav[ADDSET_VIGN_CENTER] ? toEdit.vignetting.centerX + mods.vignetting.centerX : mods.vignetting.centerX;
    }

    if (vignetting.centerY) {
        toEdit.vignetting.centerY = dontforceSet && options.baBehav[ADDSET_VIGN_CENTER] ? toEdit.vignetting.centerY + mods.vignetting.centerY : mods.vignetting.centerY;
    }

    if (chmixer.enabled) {
        toEdit.chmixer.enabled = mods.chmixer.enabled;
    }

    for (int i = 0; i < 3; i++) {
        if (chmixer.red[i]) {
            toEdit.chmixer.red[i] = dontforceSet && options.baBehav[ADDSET_CHMIXER] ? toEdit.chmixer.red[i] + mods.chmixer.red[i] : mods.chmixer.red[i];
        }

        if (chmixer.green[i]) {
            toEdit.chmixer.green[i] = dontforceSet && options.baBehav[ADDSET_CHMIXER] ? toEdit.chmixer.green[i] + mods.chmixer.green[i] : mods.chmixer.green[i];
        }

        if (chmixer.blue[i]) {
            toEdit.chmixer.blue[i] = dontforceSet && options.baBehav[ADDSET_CHMIXER] ? toEdit.chmixer.blue[i] + mods.chmixer.blue[i] : mods.chmixer.blue[i];
        }
    }

    if (blackwhite.enabled) {
        toEdit.blackwhite.enabled = mods.blackwhite.enabled;
    }

    if (blackwhite.method) {
        toEdit.blackwhite.method = mods.blackwhite.method;
    }

    if (blackwhite.luminanceCurve) {
        toEdit.blackwhite.luminanceCurve = mods.blackwhite.luminanceCurve;
    }

    if (blackwhite.autoc) {
        toEdit.blackwhite.autoc = mods.blackwhite.autoc;
    }

    if (blackwhite.setting) {
        toEdit.blackwhite.setting = mods.blackwhite.setting;
    }

    if (blackwhite.enabledcc) {
        toEdit.blackwhite.enabledcc = mods.blackwhite.enabledcc;
    }

    if (blackwhite.filter) {
        toEdit.blackwhite.filter = mods.blackwhite.filter;
    }

    if (blackwhite.mixerRed) {
        toEdit.blackwhite.mixerRed = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_HUES] ? toEdit.blackwhite.mixerRed + mods.blackwhite.mixerRed : mods.blackwhite.mixerRed;
    }

    if (blackwhite.mixerOrange) {
        toEdit.blackwhite.mixerOrange = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_HUES] ? toEdit.blackwhite.mixerOrange + mods.blackwhite.mixerOrange : mods.blackwhite.mixerOrange;
    }

    if (blackwhite.mixerYellow) {
        toEdit.blackwhite.mixerYellow = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_HUES] ? toEdit.blackwhite.mixerYellow + mods.blackwhite.mixerYellow : mods.blackwhite.mixerYellow;
    }

    if (blackwhite.mixerGreen) {
        toEdit.blackwhite.mixerGreen = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_HUES] ? toEdit.blackwhite.mixerGreen + mods.blackwhite.mixerGreen : mods.blackwhite.mixerGreen;
    }

    if (blackwhite.mixerCyan) {
        toEdit.blackwhite.mixerCyan = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_HUES] ? toEdit.blackwhite.mixerCyan + mods.blackwhite.mixerCyan : mods.blackwhite.mixerCyan;
    }

    if (blackwhite.mixerBlue) {
        toEdit.blackwhite.mixerBlue = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_HUES] ? toEdit.blackwhite.mixerBlue + mods.blackwhite.mixerBlue : mods.blackwhite.mixerBlue;
    }

    if (blackwhite.mixerMagenta) {
        toEdit.blackwhite.mixerMagenta = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_HUES] ? toEdit.blackwhite.mixerMagenta + mods.blackwhite.mixerMagenta : mods.blackwhite.mixerMagenta;
    }

    if (blackwhite.mixerPurple) {
        toEdit.blackwhite.mixerPurple = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_HUES] ? toEdit.blackwhite.mixerPurple + mods.blackwhite.mixerPurple : mods.blackwhite.mixerPurple;
    }

    if (blackwhite.gammaRed) {
        toEdit.blackwhite.gammaRed = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_GAMMA] ? toEdit.blackwhite.gammaRed + mods.blackwhite.gammaRed : mods.blackwhite.gammaRed;
    }

    if (blackwhite.gammaGreen) {
        toEdit.blackwhite.gammaGreen = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_GAMMA] ? toEdit.blackwhite.gammaGreen + mods.blackwhite.gammaGreen : mods.blackwhite.gammaGreen;
    }

    if (blackwhite.gammaBlue) {
        toEdit.blackwhite.gammaBlue = dontforceSet && options.baBehav[ADDSET_BLACKWHITE_GAMMA] ? toEdit.blackwhite.gammaBlue + mods.blackwhite.gammaBlue : mods.blackwhite.gammaBlue;
    }

    if (blackwhite.beforeCurve) {
        toEdit.blackwhite.beforeCurve = mods.blackwhite.beforeCurve;
    }

    if (blackwhite.beforeCurveMode) {
        toEdit.blackwhite.beforeCurveMode = mods.blackwhite.beforeCurveMode;
    }

    if (blackwhite.afterCurve) {
        toEdit.blackwhite.afterCurve = mods.blackwhite.afterCurve;
    }

    if (blackwhite.afterCurveMode) {
        toEdit.blackwhite.afterCurveMode = mods.blackwhite.afterCurveMode;
    }

    if (blackwhite.algo) {
        toEdit.blackwhite.algo = mods.blackwhite.algo;
    }

    if (resize.scale) {
        toEdit.resize.scale = dontforceSet && options.baBehav[ADDSET_RESIZE_SCALE] ? toEdit.resize.scale + mods.resize.scale : mods.resize.scale;
    }

    if (resize.appliesTo) {
        toEdit.resize.appliesTo = mods.resize.appliesTo;
    }

    if (resize.method) {
        toEdit.resize.method = mods.resize.method;
    }

    if (resize.dataspec) {
        toEdit.resize.dataspec = mods.resize.dataspec;
    }

    if (resize.width) {
        toEdit.resize.width = mods.resize.width;
    }

    if (resize.height) {
        toEdit.resize.height = mods.resize.height;
    }

    if (resize.enabled) {
        toEdit.resize.enabled = mods.resize.enabled;
    }

    if (resize.allowUpscaling) {
        toEdit.resize.allowUpscaling = mods.resize.allowUpscaling;
    }

    if (icm.inputProfile) {
        toEdit.icm.inputProfile = mods.icm.inputProfile;
    }

    if (icm.toneCurve) {
        toEdit.icm.toneCurve = mods.icm.toneCurve;
    }

    if (icm.applyLookTable) {
        toEdit.icm.applyLookTable = mods.icm.applyLookTable;
    }

    if (icm.applyBaselineExposureOffset) {
        toEdit.icm.applyBaselineExposureOffset = mods.icm.applyBaselineExposureOffset;
    }

    if (icm.applyHueSatMap) {
        toEdit.icm.applyHueSatMap = mods.icm.applyHueSatMap;
    }

    if (icm.dcpIlluminant) {
        toEdit.icm.dcpIlluminant = mods.icm.dcpIlluminant;
    }

    if (icm.workingProfile) {
        toEdit.icm.workingProfile = mods.icm.workingProfile;
    }

    if (icm.outputProfile) {
        toEdit.icm.outputProfile = mods.icm.outputProfile;
    }

    if (icm.outputIntent) {
        toEdit.icm.outputIntent = mods.icm.outputIntent;
    }

    if (icm.outputBPC) {
        toEdit.icm.outputBPC = mods.icm.outputBPC;
    }

    if (icm.workingTRCGamma) {
        toEdit.icm.workingTRCGamma = mods.icm.workingTRCGamma;
    }

    if (icm.workingTRCSlope) {
        toEdit.icm.workingTRCSlope = mods.icm.workingTRCSlope;
    }

    if (icm.workingTRC) {
        toEdit.icm.workingTRC = mods.icm.workingTRC;
    }

    if (raw.bayersensor.method) {
        toEdit.raw.bayersensor.method = mods.raw.bayersensor.method;
    }

    if (raw.bayersensor.border) {
        toEdit.raw.bayersensor.border = mods.raw.bayersensor.border;
    }

    if (raw.bayersensor.imageNum) {
        toEdit.raw.bayersensor.imageNum = mods.raw.bayersensor.imageNum;
    }

    if (raw.bayersensor.ccSteps) {
        toEdit.raw.bayersensor.ccSteps = mods.raw.bayersensor.ccSteps;
    }

    if (raw.bayersensor.exBlack0) {
        toEdit.raw.bayersensor.black0 = dontforceSet && options.baBehav[ADDSET_RAWEXPOS_BLACKS] ? toEdit.raw.bayersensor.black0 + mods.raw.bayersensor.black0 : mods.raw.bayersensor.black0;
    }

    if (raw.bayersensor.exBlack1) {
        toEdit.raw.bayersensor.black1 = dontforceSet && options.baBehav[ADDSET_RAWEXPOS_BLACKS] ? toEdit.raw.bayersensor.black1 + mods.raw.bayersensor.black1 : mods.raw.bayersensor.black1;
    }

    if (raw.bayersensor.exBlack2) {
        toEdit.raw.bayersensor.black2 = dontforceSet && options.baBehav[ADDSET_RAWEXPOS_BLACKS] ? toEdit.raw.bayersensor.black2 + mods.raw.bayersensor.black2 : mods.raw.bayersensor.black2;
    }

    if (raw.bayersensor.exBlack3) {
        toEdit.raw.bayersensor.black3 = dontforceSet && options.baBehav[ADDSET_RAWEXPOS_BLACKS] ? toEdit.raw.bayersensor.black3 + mods.raw.bayersensor.black3 : mods.raw.bayersensor.black3;
    }

    if (raw.bayersensor.exTwoGreen) {
        toEdit.raw.bayersensor.twogreen = mods.raw.bayersensor.twogreen;
    }

    if (raw.bayersensor.dcbIterations) {
        toEdit.raw.bayersensor.dcb_iterations = mods.raw.bayersensor.dcb_iterations;
    }

    if (raw.bayersensor.dcbEnhance) {
        toEdit.raw.bayersensor.dcb_enhance = mods.raw.bayersensor.dcb_enhance;
    }

    if (raw.bayersensor.lmmseIterations) {
        toEdit.raw.bayersensor.lmmse_iterations = mods.raw.bayersensor.lmmse_iterations;
    }

    if (raw.bayersensor.dualDemosaicAutoContrast) {
        toEdit.raw.bayersensor.dualDemosaicAutoContrast = mods.raw.bayersensor.dualDemosaicAutoContrast;
    }

    if (raw.bayersensor.dualDemosaicContrast) {
        toEdit.raw.bayersensor.dualDemosaicContrast = mods.raw.bayersensor.dualDemosaicContrast;
    }

    if (raw.bayersensor.pixelShiftMotionCorrectionMethod) {
        toEdit.raw.bayersensor.pixelShiftMotionCorrectionMethod = mods.raw.bayersensor.pixelShiftMotionCorrectionMethod;
    }

    if (raw.bayersensor.pixelShiftEperIso) {
        toEdit.raw.bayersensor.pixelShiftEperIso = mods.raw.bayersensor.pixelShiftEperIso;
    }

    if (raw.bayersensor.pixelShiftSigma) {
        toEdit.raw.bayersensor.pixelShiftSigma = mods.raw.bayersensor.pixelShiftSigma;
    }

    if (raw.bayersensor.pixelShiftShowMotion) {
        toEdit.raw.bayersensor.pixelShiftShowMotion = mods.raw.bayersensor.pixelShiftShowMotion;
    }

    if (raw.bayersensor.pixelShiftShowMotionMaskOnly) {
        toEdit.raw.bayersensor.pixelShiftShowMotionMaskOnly = mods.raw.bayersensor.pixelShiftShowMotionMaskOnly;
    }

    if (raw.bayersensor.pixelShiftHoleFill) {
        toEdit.raw.bayersensor.pixelShiftHoleFill = mods.raw.bayersensor.pixelShiftHoleFill;
    }

    if (raw.bayersensor.pixelShiftMedian) {
        toEdit.raw.bayersensor.pixelShiftMedian = mods.raw.bayersensor.pixelShiftMedian;
    }

    if (raw.bayersensor.pixelShiftGreen) {
        toEdit.raw.bayersensor.pixelShiftGreen = mods.raw.bayersensor.pixelShiftGreen;
    }

    if (raw.bayersensor.pixelShiftBlur) {
        toEdit.raw.bayersensor.pixelShiftBlur = mods.raw.bayersensor.pixelShiftBlur;
    }

    if (raw.bayersensor.pixelShiftSmooth) {
        toEdit.raw.bayersensor.pixelShiftSmoothFactor = mods.raw.bayersensor.pixelShiftSmoothFactor;
    }

    if (raw.bayersensor.pixelShiftEqualBright) {
        toEdit.raw.bayersensor.pixelShiftEqualBright = mods.raw.bayersensor.pixelShiftEqualBright;
    }

    if (raw.bayersensor.pixelShiftEqualBrightChannel) {
        toEdit.raw.bayersensor.pixelShiftEqualBrightChannel = mods.raw.bayersensor.pixelShiftEqualBrightChannel;
    }

    if (raw.bayersensor.pixelShiftNonGreenCross) {
        toEdit.raw.bayersensor.pixelShiftNonGreenCross = mods.raw.bayersensor.pixelShiftNonGreenCross;
    }

    if (raw.bayersensor.pixelShiftDemosaicMethod) {
        toEdit.raw.bayersensor.pixelShiftDemosaicMethod = mods.raw.bayersensor.pixelShiftDemosaicMethod;
    }

    if (raw.bayersensor.greenEq) {
        toEdit.raw.bayersensor.greenthresh = dontforceSet && options.baBehav[ADDSET_PREPROCESS_GREENEQUIL] ? toEdit.raw.bayersensor.greenthresh + mods.raw.bayersensor.greenthresh : mods.raw.bayersensor.greenthresh;
    }

    if (raw.bayersensor.linenoise) {
        toEdit.raw.bayersensor.linenoise = dontforceSet && options.baBehav[ADDSET_PREPROCESS_LINEDENOISE] ? toEdit.raw.bayersensor.linenoise + mods.raw.bayersensor.linenoise : mods.raw.bayersensor.linenoise;
    }

    if (raw.bayersensor.linenoiseDirection) {
        toEdit.raw.bayersensor.linenoiseDirection = mods.raw.bayersensor.linenoiseDirection;
    }

    if (raw.bayersensor.pdafLinesFilter) {
        toEdit.raw.bayersensor.pdafLinesFilter = mods.raw.bayersensor.pdafLinesFilter;
    }

    if (raw.xtranssensor.method) {
        toEdit.raw.xtranssensor.method = mods.raw.xtranssensor.method;
    }

    if (raw.xtranssensor.dualDemosaicAutoContrast) {
        toEdit.raw.xtranssensor.dualDemosaicAutoContrast = mods.raw.xtranssensor.dualDemosaicAutoContrast;
    }

    if (raw.xtranssensor.dualDemosaicContrast) {
        toEdit.raw.xtranssensor.dualDemosaicContrast = mods.raw.xtranssensor.dualDemosaicContrast;
    }

    if (raw.xtranssensor.ccSteps) {
        toEdit.raw.xtranssensor.ccSteps = mods.raw.xtranssensor.ccSteps;
    }

    if (raw.xtranssensor.border) {
        toEdit.raw.xtranssensor.border = mods.raw.xtranssensor.border;
    }

    if (raw.xtranssensor.exBlackRed) {
        toEdit.raw.xtranssensor.blackred = dontforceSet && options.baBehav[ADDSET_RAWEXPOS_BLACKS] ? toEdit.raw.xtranssensor.blackred + mods.raw.xtranssensor.blackred : mods.raw.xtranssensor.blackred;
    }

    if (raw.xtranssensor.exBlackGreen) {
        toEdit.raw.xtranssensor.blackgreen = dontforceSet && options.baBehav[ADDSET_RAWEXPOS_BLACKS] ? toEdit.raw.xtranssensor.blackgreen + mods.raw.xtranssensor.blackgreen : mods.raw.xtranssensor.blackgreen;
    }

    if (raw.xtranssensor.exBlackBlue) {
        toEdit.raw.xtranssensor.blackblue = dontforceSet && options.baBehav[ADDSET_RAWEXPOS_BLACKS] ? toEdit.raw.xtranssensor.blackblue + mods.raw.xtranssensor.blackblue : mods.raw.xtranssensor.blackblue;
    }

    if (raw.ca_autocorrect) {
        toEdit.raw.ca_autocorrect = mods.raw.ca_autocorrect;
    }

    if (raw.ca_avoidcolourshift) {
        toEdit.raw.ca_avoidcolourshift = mods.raw.ca_avoidcolourshift;
    }

    if (raw.caautoiterations) {
        toEdit.raw.caautoiterations = dontforceSet && options.baBehav[ADDSET_RAWCACORR] ? toEdit.raw.caautoiterations + mods.raw.caautoiterations : mods.raw.caautoiterations;
    }

    if (raw.cared) {
        toEdit.raw.cared = dontforceSet && options.baBehav[ADDSET_RAWCACORR] ? toEdit.raw.cared + mods.raw.cared : mods.raw.cared;
    }

    if (raw.cablue) {
        toEdit.raw.cablue = dontforceSet && options.baBehav[ADDSET_RAWCACORR] ? toEdit.raw.cablue + mods.raw.cablue : mods.raw.cablue;
    }

    if (raw.exPos) {
        toEdit.raw.expos = dontforceSet && options.baBehav[ADDSET_RAWEXPOS_LINEAR] ? toEdit.raw.expos + mods.raw.expos : mods.raw.expos;
    }

    if (raw.hotPixelFilter) {
        toEdit.raw.hotPixelFilter = mods.raw.hotPixelFilter;
    }

    if (raw.deadPixelFilter) {
        toEdit.raw.deadPixelFilter = mods.raw.deadPixelFilter;
    }

    if (raw.hotdeadpix_thresh) {
        toEdit.raw.hotdeadpix_thresh = mods.raw.hotdeadpix_thresh;
    }

    if (raw.darkFrame) {
        toEdit.raw.dark_frame = mods.raw.dark_frame;
    }

    if (raw.df_autoselect) {
        toEdit.raw.df_autoselect = mods.raw.df_autoselect;
    }

    if (raw.ff_file) {
        toEdit.raw.ff_file = mods.raw.ff_file;
    }

    if (raw.ff_AutoSelect) {
        toEdit.raw.ff_AutoSelect = mods.raw.ff_AutoSelect;
    }

    if (raw.ff_BlurRadius) {
        toEdit.raw.ff_BlurRadius = mods.raw.ff_BlurRadius;
    }

    if (raw.ff_BlurType) {
        toEdit.raw.ff_BlurType = mods.raw.ff_BlurType;
    }

    if (raw.ff_AutoClipControl) {
        toEdit.raw.ff_AutoClipControl = mods.raw.ff_AutoClipControl;
    }

    if (raw.ff_clipControl) {
        toEdit.raw.ff_clipControl = dontforceSet && options.baBehav[ADDSET_RAWFFCLIPCONTROL] ? toEdit.raw.ff_clipControl + mods.raw.ff_clipControl : mods.raw.ff_clipControl;
    }

    if (wavelet.enabled) {
        toEdit.wavelet.enabled = mods.wavelet.enabled;
    }

    if (wavelet.strength) {
        toEdit.wavelet.strength = mods.wavelet.strength;
    }

    if (wavelet.balance) {
        toEdit.wavelet.balance = mods.wavelet.balance;
    }

    if (wavelet.iter) {
        toEdit.wavelet.iter = mods.wavelet.iter;
    }

    if (wavelet.median) {
        toEdit.wavelet.median = mods.wavelet.median;
    }

    if (wavelet.medianlev) {
        toEdit.wavelet.medianlev = mods.wavelet.medianlev;
    }

    if (wavelet.linkedg) {
        toEdit.wavelet.linkedg = mods.wavelet.linkedg;
    }

    if (wavelet.cbenab) {
        toEdit.wavelet.cbenab = mods.wavelet.cbenab;
    }

    if (wavelet.greenhigh) {
        toEdit.wavelet.greenhigh = mods.wavelet.greenhigh;
    }

    if (wavelet.bluehigh) {
        toEdit.wavelet.bluehigh = mods.wavelet.bluehigh;
    }

    if (wavelet.greenmed) {
        toEdit.wavelet.greenmed = mods.wavelet.greenmed;
    }

    if (wavelet.bluemed) {
        toEdit.wavelet.bluemed = mods.wavelet.bluemed;
    }

    if (wavelet.greenlow) {
        toEdit.wavelet.greenlow = mods.wavelet.greenlow;
    }

    if (wavelet.bluelow) {
        toEdit.wavelet.bluelow = mods.wavelet.bluelow;
    }

    if (wavelet.lipst) {
        toEdit.wavelet.lipst = mods.wavelet.lipst;
    }

    if (wavelet.Medgreinf) {
        toEdit.wavelet.Medgreinf = mods.wavelet.Medgreinf;
    }

    if (wavelet.avoid) {
        toEdit.wavelet.avoid = mods.wavelet.avoid;
    }

    if (wavelet.tmr) {
        toEdit.wavelet.tmr = mods.wavelet.tmr;
    }

    if (wavelet.Lmethod) {
        toEdit.wavelet.Lmethod = mods.wavelet.Lmethod;
    }

    if (wavelet.CLmethod) {
        toEdit.wavelet.CLmethod = mods.wavelet.CLmethod;
    }

    if (wavelet.Backmethod) {
        toEdit.wavelet.Backmethod = mods.wavelet.Backmethod;
    }

    if (wavelet.Tilesmethod) {
        toEdit.wavelet.Tilesmethod = mods.wavelet.Tilesmethod;
    }

    if (wavelet.daubcoeffmethod) {
        toEdit.wavelet.daubcoeffmethod = mods.wavelet.daubcoeffmethod;
    }

    if (wavelet.CHmethod) {
        toEdit.wavelet.CHmethod = mods.wavelet.CHmethod;
    }

    if (wavelet.CHSLmethod) {
        toEdit.wavelet.CHSLmethod = mods.wavelet.CHSLmethod;
    }

    if (wavelet.EDmethod) {
        toEdit.wavelet.EDmethod = mods.wavelet.EDmethod;
    }

    if (wavelet.NPmethod) {
        toEdit.wavelet.NPmethod = mods.wavelet.NPmethod;
    }

    if (wavelet.BAmethod) {
        toEdit.wavelet.BAmethod = mods.wavelet.BAmethod;
    }

    if (wavelet.TMmethod) {
        toEdit.wavelet.TMmethod = mods.wavelet.TMmethod;
    }

    if (wavelet.HSmethod) {
        toEdit.wavelet.HSmethod = mods.wavelet.HSmethod;
    }

    if (wavelet.Dirmethod) {
        toEdit.wavelet.Dirmethod = mods.wavelet.Dirmethod;
    }

    if (wavelet.edgthresh) {
        toEdit.wavelet.edgthresh = mods.wavelet.edgthresh;
    }

    if (wavelet.sky) {
        toEdit.wavelet.sky = dontforceSet && options.baBehav[ADDSET_WA_SKYPROTECT] ? toEdit.wavelet.sky + mods.wavelet.sky : mods.wavelet.sky;
    }

    if (wavelet.thr) {
        toEdit.wavelet.thr = dontforceSet && options.baBehav[ADDSET_WA_THRR] ? toEdit.wavelet.thr + mods.wavelet.thr : mods.wavelet.thr;
    }

    if (wavelet.thrH) {
        toEdit.wavelet.thrH = dontforceSet && options.baBehav[ADDSET_WA_THRRH] ? toEdit.wavelet.thrH + mods.wavelet.thrH : mods.wavelet.thrH;
    }

    if (wavelet.sup) {
        toEdit.wavelet.sup = mods.wavelet.sup;
    }

    if (wavelet.hllev) {
        toEdit.wavelet.hllev = mods.wavelet.hllev;
    }

    if (wavelet.bllev) {
        toEdit.wavelet.bllev = mods.wavelet.bllev;
    }

    if (wavelet.edgcont) {
        toEdit.wavelet.edgcont = mods.wavelet.edgcont;
    }

    if (wavelet.level0noise) {
        toEdit.wavelet.level0noise = mods.wavelet.level0noise;
    }

    if (wavelet.level1noise) {
        toEdit.wavelet.level1noise = mods.wavelet.level1noise;
    }

    if (wavelet.level2noise) {
        toEdit.wavelet.level2noise = mods.wavelet.level2noise;
    }

    if (wavelet.level3noise) {
        toEdit.wavelet.level3noise = mods.wavelet.level3noise;
    }

    if (wavelet.pastlev) {
        toEdit.wavelet.pastlev = mods.wavelet.pastlev;
    }

    if (wavelet.satlev) {
        toEdit.wavelet.satlev = mods.wavelet.satlev;
    }

    if (wavelet.ccwcurve) {
        toEdit.wavelet.ccwcurve = mods.wavelet.ccwcurve;
    }

    if (wavelet.opacityCurveRG) {
        toEdit.wavelet.opacityCurveRG = mods.wavelet.opacityCurveRG;
    }

    if (wavelet.opacityCurveBY) {
        toEdit.wavelet.opacityCurveBY = mods.wavelet.opacityCurveBY;
    }

    if (wavelet.opacityCurveW) {
        toEdit.wavelet.opacityCurveW = mods.wavelet.opacityCurveW;
    }

    if (wavelet.opacityCurveWL) {
        toEdit.wavelet.opacityCurveWL = mods.wavelet.opacityCurveWL;
    }

    if (wavelet.hhcurve) {
        toEdit.wavelet.hhcurve = mods.wavelet.hhcurve;
    }

    if (wavelet.Chcurve) {
        toEdit.wavelet.Chcurve = mods.wavelet.Chcurve;
    }

    if (wavelet.wavclCurve) {
        toEdit.wavelet.wavclCurve = mods.wavelet.wavclCurve;
    }

    //if (wavelet.enacont)  toEdit.wavelet.enacont = mods.wavelet.enacont;
    if (wavelet.expcontrast) {
        toEdit.wavelet.expcontrast = mods.wavelet.expcontrast;
    }

    if (wavelet.expchroma) {
        toEdit.wavelet.expchroma = mods.wavelet.expchroma;
    }

    if (wavelet.expedge) {
        toEdit.wavelet.expedge = mods.wavelet.expedge;
    }

    if (wavelet.expresid) {
        toEdit.wavelet.expresid = mods.wavelet.expresid;
    }

    if (wavelet.expfinal) {
        toEdit.wavelet.expfinal = mods.wavelet.expfinal;
    }

    if (wavelet.exptoning) {
        toEdit.wavelet.exptoning = mods.wavelet.exptoning;
    }

    if (wavelet.expnoise) {
        toEdit.wavelet.expnoise = mods.wavelet.expnoise;
    }

    for (int i = 0; i < 9; i++) {
        if (wavelet.c[i]) {
            toEdit.wavelet.c[i] = dontforceSet && options.baBehav[ADDSET_WA] ? toEdit.wavelet.c[i] + mods.wavelet.c[i] : mods.wavelet.c[i];
        }
    }

    for (int i = 0; i < 9; i++) {
        if (wavelet.ch[i]) {
            toEdit.wavelet.ch[i] = dontforceSet && options.baBehav[ADDSET_WA] ? toEdit.wavelet.ch[i] + mods.wavelet.ch[i] : mods.wavelet.ch[i];
        }
    }

    if (wavelet.skinprotect) {
        toEdit.wavelet.skinprotect = dontforceSet && options.baBehav[ADDSET_WA_SKINPROTECT] ? toEdit.wavelet.skinprotect + mods.wavelet.skinprotect : mods.wavelet.skinprotect;
    }

    if (wavelet.hueskin) {
        toEdit.wavelet.hueskin = mods.wavelet.hueskin;
    }

    if (wavelet.hueskin2) {
        toEdit.wavelet.hueskin2 = mods.wavelet.hueskin2;
    }

    if (wavelet.edgesensi) {
        toEdit.wavelet.edgesensi = mods.wavelet.edgesensi;
    }

    if (wavelet.edgeampli) {
        toEdit.wavelet.edgeampli = mods.wavelet.edgeampli;
    }

    if (wavelet.resconH) {
        toEdit.wavelet.resconH = dontforceSet && options.baBehav[ADDSET_WA_RESCONH] ? toEdit.wavelet.resconH + mods.wavelet.resconH : mods.wavelet.resconH;
    }

    if (wavelet.reschro) {
        toEdit.wavelet.reschro = dontforceSet && options.baBehav[ADDSET_WA_RESCHRO] ? toEdit.wavelet.reschro + mods.wavelet.reschro : mods.wavelet.reschro;
    }

    if (wavelet.tmrs) {
        toEdit.wavelet.tmrs = dontforceSet && options.baBehav[ADDSET_WA_TMRS] ? toEdit.wavelet.tmrs + mods.wavelet.tmrs : mods.wavelet.tmrs;
    }

    if (wavelet.gamma) {
        toEdit.wavelet.gamma = dontforceSet && options.baBehav[ADDSET_WA_GAMMA] ? toEdit.wavelet.gamma + mods.wavelet.gamma : mods.wavelet.gamma;
    }

    if (wavelet.rescon) {
        toEdit.wavelet.rescon = dontforceSet && options.baBehav[ADDSET_WA_RESCON] ? toEdit.wavelet.rescon + mods.wavelet.rescon : mods.wavelet.rescon;
    }

    if (wavelet.thres) {
        toEdit.wavelet.thres = dontforceSet && options.baBehav[ADDSET_WA_THRES] ? toEdit.wavelet.thres + mods.wavelet.thres : mods.wavelet.thres;
    }

    if (wavelet.threshold) {
        toEdit.wavelet.threshold = dontforceSet && options.baBehav[ADDSET_WA_THRESHOLD] ? toEdit.wavelet.threshold + mods.wavelet.threshold : mods.wavelet.threshold;
    }

    if (wavelet.threshold2) {
        toEdit.wavelet.threshold2 = dontforceSet && options.baBehav[ADDSET_WA_THRESHOLD2] ? toEdit.wavelet.threshold2 + mods.wavelet.threshold2 : mods.wavelet.threshold2;
    }

    if (wavelet.edgedetect) {
        toEdit.wavelet.edgedetect = dontforceSet && options.baBehav[ADDSET_WA_EDGEDETECT] ? toEdit.wavelet.edgedetect + mods.wavelet.edgedetect : mods.wavelet.edgedetect;
    }

    if (wavelet.edgedetectthr) {
        toEdit.wavelet.edgedetectthr = dontforceSet && options.baBehav[ADDSET_WA_EDGEDETECTTHR] ? toEdit.wavelet.edgedetectthr + mods.wavelet.edgedetectthr : mods.wavelet.edgedetectthr;
    }

    if (wavelet.edgedetectthr2) {
        toEdit.wavelet.edgedetectthr2 = dontforceSet && options.baBehav[ADDSET_WA_EDGEDETECTTHR2] ? toEdit.wavelet.edgedetectthr2 + mods.wavelet.edgedetectthr2 : mods.wavelet.edgedetectthr2;
    }

    if (wavelet.chro) {
        toEdit.wavelet.chro = dontforceSet && options.baBehav[ADDSET_WA_CHRO] ? toEdit.wavelet.chro + mods.wavelet.chro : mods.wavelet.chro;
    }

    if (wavelet.chroma) {
        toEdit.wavelet.chroma = dontforceSet && options.baBehav[ADDSET_WA_CHROMA] ? toEdit.wavelet.chroma + mods.wavelet.chroma : mods.wavelet.chroma;
    }

    if (wavelet.contrast) {
        toEdit.wavelet.contrast = dontforceSet && options.baBehav[ADDSET_WA_CONTRAST] ? toEdit.wavelet.contrast + mods.wavelet.contrast : mods.wavelet.contrast;
    }

    if (wavelet.edgrad) {
        toEdit.wavelet.edgrad = dontforceSet && options.baBehav[ADDSET_WA_EDGRAD] ? toEdit.wavelet.edgrad + mods.wavelet.edgrad : mods.wavelet.edgrad;
    }

    if (wavelet.edgval) {
        toEdit.wavelet.edgval = dontforceSet && options.baBehav[ADDSET_WA_EDGVAL] ? toEdit.wavelet.edgval + mods.wavelet.edgval : mods.wavelet.edgval;
    }

    if (wavelet.strength) {
        toEdit.wavelet.strength = dontforceSet && options.baBehav[ADDSET_WA_STRENGTH] ? toEdit.wavelet.strength + mods.wavelet.strength : mods.wavelet.strength;
    }


    if (dirpyrequalizer.enabled) {
        toEdit.dirpyrequalizer.enabled = mods.dirpyrequalizer.enabled;
    }

    if (dirpyrequalizer.gamutlab) {
        toEdit.dirpyrequalizer.gamutlab = mods.dirpyrequalizer.gamutlab;
    }

    if (dirpyrequalizer.cbdlMethod) {
        toEdit.dirpyrequalizer.cbdlMethod = mods.dirpyrequalizer.cbdlMethod;
    }

    for (int i = 0; i < 6; i++) {
        if (dirpyrequalizer.mult[i]) {
            toEdit.dirpyrequalizer.mult[i] = dontforceSet && options.baBehav[ADDSET_DIRPYREQ] ? toEdit.dirpyrequalizer.mult[i] + mods.dirpyrequalizer.mult[i] : mods.dirpyrequalizer.mult[i];
        }
    }

    if (dirpyrequalizer.threshold) {
        toEdit.dirpyrequalizer.threshold = dontforceSet && options.baBehav[ADDSET_DIRPYREQ_THRESHOLD] ? toEdit.dirpyrequalizer.threshold + mods.dirpyrequalizer.threshold : mods.dirpyrequalizer.threshold;
    }

    if (dirpyrequalizer.skinprotect) {
        toEdit.dirpyrequalizer.skinprotect = dontforceSet && options.baBehav[ADDSET_DIRPYREQ_SKINPROTECT] ? toEdit.dirpyrequalizer.skinprotect + mods.dirpyrequalizer.skinprotect : mods.dirpyrequalizer.skinprotect;
    }

    if (dirpyrequalizer.hueskin) {
        toEdit.dirpyrequalizer.hueskin = mods.dirpyrequalizer.hueskin;
    }

//  if (dirpyrequalizer.algo)       toEdit.dirpyrequalizer.algo = mods.dirpyrequalizer.algo;
    if (hsvequalizer.enabled) {
        toEdit.hsvequalizer.enabled = mods.hsvequalizer.enabled;
    }

    if (hsvequalizer.hcurve) {
        toEdit.hsvequalizer.hcurve = mods.hsvequalizer.hcurve;
    }

    if (hsvequalizer.scurve) {
        toEdit.hsvequalizer.scurve = mods.hsvequalizer.scurve;
    }

    if (hsvequalizer.vcurve) {
        toEdit.hsvequalizer.vcurve = mods.hsvequalizer.vcurve;
    }

    if (filmSimulation.enabled) {
        toEdit.filmSimulation.enabled = mods.filmSimulation.enabled;
    }

    if (filmSimulation.clutFilename) {
        toEdit.filmSimulation.clutFilename = mods.filmSimulation.clutFilename;
    }

    if (filmSimulation.strength) {
        toEdit.filmSimulation.strength = dontforceSet && options.baBehav[ADDSET_FILMSIMULATION_STRENGTH] ? toEdit.filmSimulation.strength + mods.filmSimulation.strength : mods.filmSimulation.strength;
    }

    if (softlight.enabled) {
        toEdit.softlight.enabled = mods.softlight.enabled;
    }

    if (softlight.strength) {
        toEdit.softlight.strength = dontforceSet && options.baBehav[ADDSET_SOFTLIGHT_STRENGTH] ? toEdit.softlight.strength + mods.softlight.strength : mods.softlight.strength;
    }

    if (dehaze.enabled) {
        toEdit.dehaze.enabled = mods.dehaze.enabled;
    }

    if (dehaze.strength) {
        toEdit.dehaze.strength = dontforceSet && options.baBehav[ADDSET_DEHAZE_STRENGTH] ? toEdit.dehaze.strength + mods.dehaze.strength : mods.dehaze.strength;
    }

    if (dehaze.depth) {
        toEdit.dehaze.depth = mods.dehaze.depth;
    }

    if (dehaze.showDepthMap) {
        toEdit.dehaze.showDepthMap = mods.dehaze.showDepthMap;
    }

    if (metadata.mode) {
        toEdit.metadata.mode = mods.metadata.mode;
    }

    if (filmNegative.enabled) {
        toEdit.filmNegative.enabled = mods.filmNegative.enabled;
    }

    if (filmNegative.redRatio) {
        toEdit.filmNegative.redRatio = mods.filmNegative.redRatio;
    }

    if (filmNegative.greenExp) {
        toEdit.filmNegative.greenExp = mods.filmNegative.greenExp;
    }

    if (filmNegative.blueRatio) {
        toEdit.filmNegative.blueRatio = mods.filmNegative.blueRatio;
    }

    // Exif changes are added to the existing ones
    if (exif) {
        for (procparams::ExifPairs::const_iterator i = mods.exif.begin(); i != mods.exif.end(); ++i) {
            toEdit.exif[i->first] = i->second;
        }
    }

    // IPTC changes are added to the existing ones
    if (iptc) {
        for (procparams::IPTCPairs::const_iterator i = mods.iptc.begin(); i != mods.iptc.end(); ++i) {
            toEdit.iptc[i->first] = i->second;
        }
    }
}

bool RAWParamsEdited::BayerSensor::isUnchanged() const
{
    return  method && border && imageNum && dcbIterations && dcbEnhance && lmmseIterations && dualDemosaicAutoContrast && dualDemosaicContrast /*&& allEnhance*/ &&  greenEq
            && pixelShiftMotionCorrectionMethod && pixelShiftEperIso && pixelShiftSigma && pixelShiftShowMotion && pixelShiftShowMotionMaskOnly
            && pixelShiftHoleFill && pixelShiftMedian && pixelShiftNonGreenCross && pixelShiftDemosaicMethod && pixelShiftGreen && pixelShiftBlur && pixelShiftSmooth && pixelShiftEqualBright && pixelShiftEqualBrightChannel
            && linenoise && linenoiseDirection && pdafLinesFilter && exBlack0 && exBlack1 && exBlack2 && exBlack3 && exTwoGreen;
}

bool RAWParamsEdited::XTransSensor::isUnchanged() const
{
    return method && border && exBlackRed && exBlackGreen && exBlackBlue && dualDemosaicAutoContrast && dualDemosaicContrast;
}

bool RAWParamsEdited::isUnchanged() const
{
    return  bayersensor.isUnchanged() && xtranssensor.isUnchanged() && ca_autocorrect && ca_avoidcolourshift && caautoiterations && cared && cablue && hotPixelFilter && deadPixelFilter && hotdeadpix_thresh && darkFrame
            && df_autoselect && ff_file && ff_AutoSelect && ff_BlurRadius && ff_BlurType && exPos && ff_AutoClipControl && ff_clipControl;
}

bool LensProfParamsEdited::isUnchanged() const
{
    return lcMode && lcpFile && useVign && lfLens;
}

bool RetinexParamsEdited::isUnchanged() const
{
    return enabled && retinexcolorspace && gammaretinex && gam && slope;
}

bool FilmNegativeParamsEdited::isUnchanged() const
{
    return enabled && redRatio && greenExp && blueRatio;
}

LocallabParamsEdited::LocallabSpotEdited::LocallabSpotEdited(bool v) :
    // Control spot settings
    name(v),
    isvisible(v),
    shape(v),
    spotMethod(v),
    sensiexclu(v),
    structexclu(v),
    struc(v),
    shapeMethod(v),
    locX(v),
    locXL(v),
    locY(v),
    locYT(v),
    centerX(v),
    centerY(v),
    circrad(v),
    qualityMethod(v),
    transit(v),
    thresh(v),
    iter(v),
    balan(v),
    transitweak(v),
    transitgrad(v),
    avoid(v),
    // Color & Light
    expcolor(v),
    curvactiv(v),
    lightness(v),
    contrast(v),
    chroma(v),
    labgridALow(v),
    labgridBLow(v),
    labgridAHigh(v),
    labgridBHigh(v),
    strengthgrid(v),
    sensi(v),
    structcol(v),
    blurcolde(v),
    blendmaskcol(v),
    radmaskcol(v),
    chromaskcol(v),
    gammaskcol(v),
    slomaskcol(v),
    qualitycurveMethod(v),
    gridMethod(v),
    llcurve(v),
    cccurve(v),
    LHcurve(v),
    HHcurve(v),
    invers(v),
    enaColorMask(v),
    CCmaskcurve(v),
    LLmaskcurve(v),
    HHmaskcurve(v),
    softradiuscol(v),
    // Exposure
    expexpose(v),
    expcomp(v),
    hlcompr(v),
    hlcomprthresh(v),
    black(v),
    shadex(v),
    shcompr(v),
    expchroma(v),
    warm(v),
    sensiex(v),
    structexp(v),
    blurexpde(v),
    excurve(v),
    inversex(v),
    enaExpMask(v),
    CCmaskexpcurve(v),
    LLmaskexpcurve(v),
    HHmaskexpcurve(v),
    blendmaskexp(v),
    radmaskexp(v),
    chromaskexp(v),
    gammaskexp(v),
    slomaskexp(v),
    softradiusexp(v),
    expMethod(v),
    laplacexp(v),
    balanexp(v),
    linear(v),
    gamm(v),
    fatamount(v),
    fatdetail(v),
    fatanchor(v),
    // Shadow highlight
    expshadhigh(v),
    highlights(v),
    h_tonalwidth(v),
    shadows(v),
    s_tonalwidth(v),
    sh_radius(v),
    sensihs(v),
    enaSHMask(v),
    CCmaskSHcurve(v),
    LLmaskSHcurve(v),
    HHmaskSHcurve(v),
    blendmaskSH(v),
    radmaskSH(v),
    blurSHde(v),
    inverssh(v),
    chromaskSH(v),
    gammaskSH(v),
    slomaskSH(v),
    // Vibrance
    expvibrance(v),
    saturated(v),
    pastels(v),
    psthreshold(v),
    protectskins(v),
    avoidcolorshift(v),
    pastsattog(v),
    sensiv(v),
    skintonescurve(v),
    // Soft Light
    expsoft(v),
    streng(v),
    sensisf(v),
    laplace(v),
    softMethod(v),
    // Blur & Noise
    expblur(v),
    radius(v),
    strength(v),
    sensibn(v),
    itera(v),
    guidbl(v),
    epsbl(v),
    blMethod(v),
    blurMethod(v),
    medMethod(v),
    activlum(v),
    CCmaskblcurve(v),
    LLmaskblcurve(v),
    HHmaskblcurve(v),
    enablMask(v),
    blendmaskbl(v),
    radmaskbl(v),
    chromaskbl(v),
    gammaskbl(v),
    slomaskbl(v),
    // Tone Mapping
    exptonemap(v),
    stren(v),
    gamma(v),
    estop(v),
    scaltm(v),
    rewei(v),
    satur(v),
    sensitm(v),
    softradiustm(v),
    amount(v),
    equiltm(v),
    CCmasktmcurve(v),
    LLmasktmcurve(v),
    HHmasktmcurve(v),
    enatmMask(v),
    blendmasktm(v),
    radmasktm(v),
    chromasktm(v),
    gammasktm(v),
    slomasktm(v),
    // Retinex
    expreti(v),
    retinexMethod(v),
    str(v),
    chrrt(v),
    neigh(v),
    vart(v),
    dehaz(v),
    sensih(v),
    localTgaincurve(v),
    inversret(v),
    equilret(v),
    softradiusret(v),
    CCmaskreticurve(v),
    LLmaskreticurve(v),
    HHmaskreticurve(v),
    enaretiMask(v),
    enaretiMasktmap(v),
    blendmaskreti(v),
    radmaskreti(v),
    chromaskreti(v),
    gammaskreti(v),
    slomaskreti(v),
    scalereti(v),
    darkness(v),
    lightnessreti(v),
    limd(v),
    fftwreti(v),
    // Sharpening
    expsharp(v),
    sharcontrast(v),
    sharradius(v),
    sharamount(v),
    shardamping(v),
    shariter(v),
    sharblur(v),
    sensisha(v),
    inverssha(v),
    // Local Contrast
    expcontrast(v),
    lcradius(v),
    lcamount(v),
    lcdarkness(v),
    lclightness(v),
    levelwav(v),
    residcont(v),
    residchro(v),
    clarilres(v),
    claricres(v),
    clarisoft(v),
    sensilc(v),
    fftwlc(v),
    localcontMethod(v),
    locwavcurve(v),
    // Contrast by detail levels
    expcbdl(v),
    mult{v, v, v, v, v, v},
    chromacbdl(v),
    threshold(v),
    sensicb(v),
    clarityml(v),
    contresid(v),
    blurcbdl(v),
    blendmaskcb(v),
    radmaskcb(v),
    chromaskcb(v),
    gammaskcb(v),
    slomaskcb(v),
    softradiuscb(v),
    enacbMask(v),
    CCmaskcbcurve(v),
    LLmaskcbcurve(v),
    HHmaskcbcurve(v),
    // Denoise
    expdenoi(v),
    noiselumf(v),
    noiselumf0(v),
    noiselumf2(v),
    noiselumc(v),
    noiselumdetail(v),
    noiselequal(v),
    noisechrof(v),
    noisechroc(v),
    noisechrodetail(v),
    adjblur(v),
    bilateral(v),
    sensiden(v)
{
}

void LocallabParamsEdited::LocallabSpotEdited::set(bool v)
{
    name = v;
    isvisible = v;
    shape = v;
    spotMethod = v;
    sensiexclu = v;
    structexclu = v;
    struc = v;
    shapeMethod = v;
    locX = v;
    locXL = v;
    locY = v;
    locYT = v;
    centerX = v;
    centerY = v;
    circrad = v;
    qualityMethod = v;
    transit = v;
    thresh = v;
    iter = v;
    balan = v;
    transitweak = v;
    transitgrad = v;
    avoid = v;
    // Color & Light
    expcolor = v;
    curvactiv = v;
    lightness = v;
    contrast = v;
    chroma = v;
    labgridALow = v;
    labgridBLow = v;
    labgridAHigh = v;
    labgridBHigh = v;
    strengthgrid = v;
    sensi = v;
    structcol = v;
    blurcolde = v;
    blendmaskcol = v;
    radmaskcol = v;
    chromaskcol = v;
    gammaskcol = v;
    slomaskcol = v;
    qualitycurveMethod = v;
    gridMethod = v;
    llcurve = v;
    cccurve = v;
    LHcurve = v;
    HHcurve = v;
    invers = v;
    enaColorMask = v;
    CCmaskcurve = v;
    LLmaskcurve = v;
    HHmaskcurve = v;
    softradiuscol = v;
    // Exposure
    expexpose = v;
    expcomp = v;
    hlcompr = v;
    hlcomprthresh = v;
    black = v;
    shadex = v;
    shcompr = v;
    expchroma = v;
    warm = v;
    sensiex = v;
    structexp = v;
    blurexpde = v;
    excurve = v;
    inversex = v;
    enaExpMask = v;
    CCmaskexpcurve = v;
    LLmaskexpcurve = v;
    HHmaskexpcurve = v;
    blendmaskexp = v;
    radmaskexp = v;
    chromaskexp = v;
    gammaskexp = v;
    slomaskexp = v;
    softradiusexp = v;
    expMethod = v;
    laplacexp = v;
    balanexp = v;
    linear = v;
    gamm = v;
    fatamount = v;
    fatdetail = v;
    fatanchor = v;
    // Shadow highlight
    expshadhigh = v;
    highlights = v;
    h_tonalwidth = v;
    shadows = v;
    s_tonalwidth = v;
    sh_radius = v;
    sensihs = v;
    enaSHMask = v;
    CCmaskSHcurve = v;
    LLmaskSHcurve = v;
    HHmaskSHcurve = v;
    blendmaskSH = v;
    radmaskSH = v;
    blurSHde = v;
    inverssh = v;
    chromaskSH = v;
    gammaskSH = v;
    slomaskSH = v;
    // Vibrance
    expvibrance = v;
    saturated = v;
    pastels = v;
    psthreshold = v;
    protectskins = v;
    avoidcolorshift = v;
    pastsattog = v;
    sensiv = v;
    skintonescurve = v;
    // Soft Light
    expsoft = v;
    streng = v;
    sensisf = v;
    laplace = v;
    softMethod = v;
    // Blur & Noise
    expblur = v;
    radius = v;
    strength = v;
    sensibn = v;
    itera = v;
    guidbl = v;
    epsbl = v;
    blMethod = v;
    blurMethod = v;
    medMethod = v;
    activlum = v;
    CCmaskblcurve = v;
    LLmaskblcurve = v;
    HHmaskblcurve = v;
    enablMask = v;
    blendmaskbl = v;
    radmaskbl = v;
    chromaskbl = v;
    gammaskbl = v;
    slomaskbl = v;
    // Tone Mapping
    exptonemap = v;
    stren = v;
    gamma = v;
    estop = v;
    scaltm = v;
    rewei = v;
    satur = v;
    sensitm = v;
    softradiustm = v;
    amount = v;
    equiltm = v;
    CCmasktmcurve = v;
    LLmasktmcurve = v;
    HHmasktmcurve = v;
    enatmMask = v;
    blendmasktm = v;
    radmasktm = v;
    chromasktm = v;
    gammasktm = v;
    slomasktm = v;
    // Retinex
    expreti = v;
    retinexMethod = v;
    str = v;
    chrrt = v;
    neigh = v;
    vart = v;
    dehaz = v;
    sensih = v;
    localTgaincurve = v;
    inversret = v;
    equilret = v;
    softradiusret = v;
    CCmaskreticurve = v;
    LLmaskreticurve = v;
    HHmaskreticurve = v;
    enaretiMask = v;
    enaretiMasktmap = v;
    blendmaskreti = v;
    radmaskreti = v;
    chromaskreti = v;
    gammaskreti = v;
    slomaskreti = v;
    scalereti = v;
    darkness = v;
    lightnessreti = v;
    limd = v;
    fftwreti = v;
    // Sharpening
    expsharp = v;
    sharcontrast = v;
    sharradius = v;
    sharamount = v;
    shardamping = v;
    shariter = v;
    sharblur = v;
    sensisha = v;
    inverssha = v;
    // Local Contrast
    expcontrast = v;
    lcradius = v;
    lcamount = v;
    lcdarkness = v;
    lclightness = v;
    levelwav = v;
    residcont = v;
    residchro = v;
    clarilres = v;
    claricres = v;
    clarisoft = v;
    sensilc = v;
    fftwlc = v;
    localcontMethod = v;
    locwavcurve = v;
    // Contrast by detail levels
    expcbdl = v;

    for (int i = 0; i < 6; i++) {
        mult[i] = v;
    }

    chromacbdl = v;
    threshold = v;
    sensicb = v;
    clarityml = v;
    contresid = v;
    blurcbdl = v;
    blendmaskcb = v;
    radmaskcb = v;
    chromaskcb = v;
    gammaskcb = v;
    slomaskcb = v;
    softradiuscb = v;
    enacbMask = v;
    CCmaskcbcurve = v;
    LLmaskcbcurve = v;
    HHmaskcbcurve = v;
    // Denoise
    expdenoi = v;
    noiselumf = v;
    noiselumf0 = v;
    noiselumf2 = v;
    noiselumc = v;
    noiselumdetail = v;
    noiselequal = v;
    noisechrof = v;
    noisechroc = v;
    noisechrodetail = v;
    adjblur = v;
    bilateral = v;
    sensiden = v;
}
