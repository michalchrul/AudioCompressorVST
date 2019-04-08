#include "AudioCompressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

#include <cstdio>
#include <iostream>

const int kNumPrograms = 1;

enum EParams
{
	kGain = 0,
	k_rms_period_ms = 1,
	k_attack_ms = 2,
	k_release_ms = 3,
	k_threshold_dB = 4,
	k_gain_dB = 5,
	k_ratio = 6,
	kNumParams
};

enum ELayout
{
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,

	kGainX = 15,
	kGainY = 20,

	k_attack_msX = 80,
	k_attack_msY = 20,

	k_release_msX = 140,
	k_release_msY = 20,

	k_threshold_dBX = 80,
	k_threshold_dBY = 90,

	k_gain_dBX = 140,
	k_gain_dBY = 90,

	k_ratioX = 200,
	k_ratioY = 20,

	kKnobFrames = 128,
};

AudioCompressor::AudioCompressor(IPlugInstanceInfo instanceInfo)
	: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.), comp(40.)

{
	//arguments are: name, defaultVal, minVal, maxVal, step, label
	GetParam(kGain)->InitDouble("Preamp", 50., 0., 100.0, 0.01, "%");
	GetParam(kGain)->SetShape(2.);

	GetParam(k_attack_ms)->InitDouble("Attack", 15., 0., 100.0, 0.01, "ms");
	GetParam(k_attack_ms)->SetShape(2.);

	GetParam(k_release_ms)->InitDouble("Release", 60., 0., 250.0, 0.01, "ms");
	GetParam(k_release_ms)->SetShape(2.);

	GetParam(k_threshold_dB)->InitDouble("Threshold", -20., -100., 0.0, 0.01, "dB");
	GetParam(k_threshold_dB)->SetShape(2.);

	GetParam(k_gain_dB)->InitDouble("Gain", 0.0, 0.0, 24.0, 0.01, "dB");
	GetParam(k_gain_dB)->SetShape(2.);

	GetParam(k_ratio)->InitDouble("Ratio", 3.0, 1.0, 100.0, 0.01, "");
	GetParam(k_ratio)->SetShape(2.);

	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
	pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

	IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
	IBitmap knob_large = pGraphics->LoadIBitmap(KNOB_ID2, KNOB_FN2, kKnobFrames);

	pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, kGain, &knob));
	pGraphics->AttachControl(new IKnobMultiControl(this, k_attack_msX, k_attack_msY, k_attack_ms, &knob));
	pGraphics->AttachControl(new IKnobMultiControl(this, k_release_msX, k_release_msY, k_release_ms, &knob));
	pGraphics->AttachControl(new IKnobMultiControl(this, k_threshold_dBX, k_threshold_dBY, k_threshold_dB, &knob));
	pGraphics->AttachControl(new IKnobMultiControl(this, k_gain_dBX, k_gain_dBY, k_gain_dB, &knob));
	pGraphics->AttachControl(new IKnobMultiControl(this, k_ratioX, k_ratioY, k_ratio, &knob_large));

	AttachGraphics(pGraphics);

	//to do
	//MakePreset("preset 1", ... );
	MakeDefaultPreset((char *) "-", kNumPrograms);
}

AudioCompressor::~AudioCompressor() {}

void AudioCompressor::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{

	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* out1 = outputs[0];
	double* out2 = outputs[1];


	double sampleRate = this->GetSampleRate();
	comp.set_attack(sampleRate*0.001*attack_ms.load());
	comp.set_release(sampleRate*0.001*release_ms.load());
	comp.set_threshold_dB(threshold_dB.load());
	comp.set_gain_dB(gain_dB.load());
	comp.set_ratio(ratio.load());


	for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
	{
		*out1 = *in1 * mGain;
		*out2 = *in2 * mGain;

		*out1 = lim(comp(*out1));
		*out2 = lim(comp(*out2));
	}

}


void AudioCompressor::Reset()
{

}

void AudioCompressor::OnParamChange(int paramIdx)
{
	switch (paramIdx)
	{
	case kGain:
		mGain.store(GetParam(kGain)->Value() / 100.);
		break;

	case k_attack_ms:
		attack_ms.store(GetParam(k_attack_ms)->Value());
		break;

	case k_release_ms:
		release_ms.store(GetParam(k_release_ms)->Value());
		break;

	case k_threshold_dB:
		threshold_dB.store(GetParam(k_threshold_dB)->Value());
		break;

	case k_gain_dB:
		gain_dB.store(GetParam(k_gain_dB)->Value());
		break;

	case k_ratio:
		ratio.store(GetParam(k_ratio)->Value());
		break;

	default:
		break;
	}
}
