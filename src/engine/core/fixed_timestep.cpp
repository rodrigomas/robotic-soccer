#include "fixed_timestep.h"

#include <cmath>

namespace soccer {

	FixedTimestep::FixedTimestep(double step, double maxFrame) :
		stepSeconds(step),
		maxFrameSeconds(maxFrame),
		accumulator(0.0),
		alpha(0.0)
	{
		configure(step, maxFrame);
	}

	void FixedTimestep::reset(void)
	{
		accumulator = 0.0;
		alpha = 0.0;
	}

	void FixedTimestep::configure(double step, double maxFrame)
	{
		stepSeconds = step > 0.0 ? step : 1.0 / 60.0;
		maxFrameSeconds = maxFrame > stepSeconds ? maxFrame : stepSeconds;
	}

	bool FixedTimestep::beginFrame(double frameSeconds)
	{
		if(std::isnan(frameSeconds) || std::isinf(frameSeconds)) {
			frameSeconds = 0.0;
		}

		if(frameSeconds < 0.0) {
			frameSeconds = -frameSeconds;
		}

		if(frameSeconds > maxFrameSeconds) {
			frameSeconds = maxFrameSeconds;
		}

		accumulator += frameSeconds;
		alpha = accumulator / stepSeconds;

		return accumulator >= stepSeconds;
	}

	double FixedTimestep::consumeStep(void)
	{
		if(accumulator < stepSeconds) {
			alpha = accumulator / stepSeconds;
			return 0.0;
		}

		accumulator -= stepSeconds;
		alpha = accumulator / stepSeconds;

		return stepSeconds;
	}

	double FixedTimestep::getStepSeconds(void) const
	{
		return stepSeconds;
	}

	double FixedTimestep::getAlpha(void) const
	{
		return alpha;
	}

	double FixedTimestep::getAccumulator(void) const
	{
		return accumulator;
	}

};
