import("stdfaust.lib");

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// A very simple subtractive synthesizer with 1 VCO 1 VCF.
// The VCO Waveform is variable between Saw and Square
// The frequency is modulated by an LFO
// The envelope control volum and filter frequency
//
///////////////////////////////////////////////////////////////////////////////////////////////////
// ANALOG IMPLEMENTATION:
//
// ANALOG_0	: waveform (Saw to square)
// ANALOG_1	: Filter Cutoff frequency
// ANALOG_2	: Filter resonance (Q)
// ANALOG_3	: Filter Envelope Modulation
//
// MIDI:
// CC 79	: Filter keyboard tracking (0 to X2, default 1)
//
// Envelope
// CC 73	: Attack
// CC 76	: Decay
// CC 77	: Sustain
// CC 72	: Release
//
// CC 78	: LFO frequency (0.001Hz to 10Hz)
// CC 1		: LFO Amplitude (Modulation)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// HUI //////////////////////////////////////////////////
// Keyboard
midigate        = button ("gate");
midifreq        = nentry("freq[unit:Hz]", 440, 20, 20000, 1);
midigain        = nentry("gain", 0.5, 0, 0.5, 0.01);// MIDI KEYBOARD

// pitchwheel
pitchwheel = hslider("bend [midi:pitchwheel]",1,0.001,10,0.01);

// VCO
wfFade	= hslider("waveform[BELA: ANALOG_0]",0.5,0,1,0.001):si.smoo;

// VCF
res     = hslider("resonnance[BELA: ANALOG_2]",0.5,0,1,0.001):si.smoo;
fr      = hslider("fc[BELA: ANALOG_1]", 10, 15, 12000, 0.001):si.smoo;
track   = hslider("tracking[midi:ctrl 79]", 1, 0, 2, 0.001);
envMod  = hslider("envMod[BELA: ANALOG_3]",50,0,100,0.01):si.smoo; 

// ENV
att     = 0.01 * (hslider ("attack[midi:ctrl 73]",0.1,0.1,400,0.001));
dec     = 0.01 * (hslider ("decay[midi:ctrl 76]",60,0.1,400,0.001));
sust    = hslider ("sustain[midi:ctrl 77]",0.2,0,1,0.001);
rel     = 0.01 * (hslider ("release[midi:ctrl 72]",100,0.1,400,0.001));

// LFO
lfoFreq	= hslider("lfoFreq[midi:ctrl 78]",6,0.001,10,0.001):si.smoo;
modwheel = hslider("modwheel[midi:ctrl 1]",0,0,0.5,0.001):si.smoo;

// PROCESS /////////////////////////////////////////////
allfreq = (midifreq * pitchwheel) + LFO;
// VCF
cutoff	= ((allfreq * track) + fr + (envMod * midigain * env)) : min(ma.SR/8);

// VCO
oscillo(f)= (os.sawtooth(f)*(1-wfFade))+(os.square(f)*wfFade);

// VCA
volume = midigain * env;

// Enveloppe
env	= en.adsre(att,dec,sust,rel,midigate);

// LFO
LFO = os.lf_triangle(lfoFreq)*modwheel*10;

// SYNTH ////////////////////////////////////////////////
synth = (oscillo(allfreq) :ve.moog_vcf(res,cutoff)) * volume;

//#################################################################################################//
//##################################### EFFECT SECTION ############################################//
//#################################################################################################//
//
// Simple FX chaine build for a mono synthesizer.
// It controle general volume and pan.
// FX Chaine is:
//		Drive
//		Flanger
//		Reverberation
//
// This version use ANALOG IN to controle some of the parameters.
// Other parameters continue to be available by MIDI or OSC.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
// ANALOG IMPLEMENTATION:
//
// ANALOG_4	: Distortion Drive
// ANALOG_5	: Flanger Dry/Wet
// ANALOG_6	: Reverberation Dry/Wet
// ANALOG_7	: Reverberation Room size
//
// MIDI:
// CC 7		: Volume
// CC 10	: Pan
//
// CC 13	: Flanger Delay
// CC 13	: Flanger Delay
// CC 94	: Flanger Feedback
//
// CC 95	: Reverberation Damp
// CC 90	: Reverberation Stereo Width
// 
///////////////////////////////////////////////////////////////////////////////////////////////////

// VOLUME:
volFX	= hslider ("volume[midi:ctrl 7]",1,0,1,0.001);// Should be 7 according to MIDI CC norm.

// EFFECTS /////////////////////////////////////////////
drive	= hslider ("drive[BELA: ANALOG_4]",0.3,0,1,0.001);


// Flanger
curdel	= hslider ("flangDel[midi:ctrl 13]",4,0.001,10,0.001);
fb		= hslider ("flangFeedback[midi:ctrl 94]",0.7,0,1,0.001);
fldw	= hslider ("dryWetFlang[BELA: ANALOG_5]",0.5,0,1,0.001);
flanger = efx
	with {
		fldel = (curdel + (os.lf_triangle(1) * 2) ) : min(10);
		efx = _ <: _, pf.flanger_mono(10,fldel,1,fb,0) : dry_wet(fldw);
	};

// Pannoramique:
panno = _ : sp.panner(hslider ("pan[midi:ctrl 10]",0.5,0,1,0.001)) : _,_;

// REVERB (from freeverb_demo)
reverb = _,_ <: (*(g)*fixedgain,*(g)*fixedgain :
	re.stereo_freeverb(combfeed, allpassfeed, damping, spatSpread)),
	*(1-g), *(1-g) :> _,_
    with {
        scaleroom   = 0.28;
        offsetroom  = 0.7;
        allpassfeed = 0.5;
        scaledamp   = 0.4;
        fixedgain   = 0.1;
        origSR = 44100;

        damping = vslider("Damp[midi:ctrl 95]",0.5, 0, 1, 0.025)*scaledamp*origSR/ma.SR;
        combfeed = vslider("RoomSize[BELA: ANALOG_7]", 0.7, 0, 1, 0.025)*scaleroom*origSR/ma.SR + offsetroom;
        spatSpread = vslider("Stereo[midi:ctrl 90]",0.6,0,1,0.01)*46*ma.SR/origSR;
        g = vslider("dryWetReverb[BELA: ANALOG_6]", 0.4, 0, 1, 0.001);
        // (g = Dry/Wet)
    };

// Dry-Wet (from C. LEBRETON)
dry_wet(dw,x,y) = wet*y + dry*x
    with {
        wet = 0.5*(dw+1.0);
        dry = 1.0-wet;
    };

// ALL
effect = _ *(volFX) : ef.cubicnl_nodc(drive, 0.1) : flanger : panno : reverb;

// PROCESS /////////////////////////////////////////////
process = synth;