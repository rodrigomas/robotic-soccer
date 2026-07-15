#include "deterministic_random.h"

#include <iostream>
#include <stdint.h>

using soccer::getDeterministicRandomSeed;
using soccer::nextDeterministicRandom;
using soccer::randomInt;
using soccer::randomSignedModulo;
using soccer::seedDeterministicRandom;

static int fail(const char *message)
{
	std::cerr << "deterministic_random_test: " << message << "\n";
	return 1;
}

int main(void)
{
	seedDeterministicRandom(1234);
	uint32_t first = nextDeterministicRandom();
	uint32_t second = nextDeterministicRandom();

	seedDeterministicRandom(1234);
	if(first != nextDeterministicRandom() ||
	   second != nextDeterministicRandom()) {
		return fail("same seed did not replay the same sequence");
	}

	seedDeterministicRandom(0);
	if(getDeterministicRandomSeed() == 0) {
		return fail("zero seed was not normalized");
	}

	for(int i = 0; i < 100; i++) {
		int value = randomInt(7);
		if(value < 0 || value >= 7) {
			return fail("randomInt returned value out of range");
		}
	}

	for(int i = 0; i < 100; i++) {
		int value = randomSignedModulo(5);
		if(value <= -5 || value >= 5) {
			return fail("randomSignedModulo returned value out of range");
		}
	}

	return 0;
}
