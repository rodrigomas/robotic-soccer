#include "fixed_timestep.h"

#include <cmath>
#include <iostream>

using soccer::FixedTimestep;

static bool nearValue(double a, double b)
{
	return std::fabs(a - b) < 0.00001;
}

static int fail(const char *message)
{
	std::cerr << "fixed_timestep_test: " << message << "\n";
	return 1;
}

int main(void)
{
	FixedTimestep stepper(0.1, 0.25);

	if(stepper.beginFrame(0.05)) {
		return fail("advanced before one full step accumulated");
	}

	if(!stepper.beginFrame(0.05)) {
		return fail("did not advance after one full step accumulated");
	}

	if(!nearValue(stepper.consumeStep(), 0.1)) {
		return fail("consumed step does not match configured step");
	}

	if(!nearValue(stepper.getAccumulator(), 0.0)) {
		return fail("accumulator did not drain after exact step");
	}

	if(!stepper.beginFrame(1.0)) {
		return fail("large frame did not advance");
	}

	if(!nearValue(stepper.consumeStep(), 0.1)) {
		return fail("large frame consumed wrong step");
	}

	if(stepper.getAccumulator() > 0.151) {
		return fail("large frame was not clamped");
	}

	stepper.reset();

	if(!nearValue(stepper.getAccumulator(), 0.0) ||
	   !nearValue(stepper.getAlpha(), 0.0)) {
		return fail("reset did not clear accumulator and alpha");
	}

	return 0;
}
