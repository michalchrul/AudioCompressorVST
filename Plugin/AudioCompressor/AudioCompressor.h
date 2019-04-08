#ifndef __AUDIOCOMPRESSOR__
#define __AUDIOCOMPRESSOR__

#include "IPlug_include_in_plug_hdr.h"
#include <atomic>
#include "dynamics.h"

class AudioCompressor : public IPlug
{
public:
	AudioCompressor(IPlugInstanceInfo instanceInfo);
	~AudioCompressor();

	void Reset();
	void OnParamChange(int paramIdx);
	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);


private:
	dsp::compressor<float> comp;
	dsp::limiter<float> lim;

	std::atomic<float>  mGain;
	std::atomic<float> rms_period_ms;
	std::atomic<float>  attack_ms;
	std::atomic<float>  release_ms;
	std::atomic<float>  threshold_dB;
	std::atomic<float>  gain_dB;
	std::atomic<float>  ratio;
};

#endif
