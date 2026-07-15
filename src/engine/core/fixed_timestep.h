#ifndef FIXED_TIMESTEP_H
#define FIXED_TIMESTEP_H

namespace soccer {

	class FixedTimestep {

		double stepSeconds;
		double maxFrameSeconds;
		double accumulator;
		double alpha;

	public:
		FixedTimestep(double step = 1.0 / 60.0, double maxFrame = 0.25);

		void reset(void);
		void configure(double step, double maxFrame);
		bool beginFrame(double frameSeconds);
		double consumeStep(void);
		double getStepSeconds(void) const;
		double getAlpha(void) const;
		double getAccumulator(void) const;
	};

};

#endif
