#include "deterministic_random.h"

namespace soccer {

	static uint32_t randomSeed = 1;
	static uint32_t randomState = 1;

	void seedDeterministicRandom(uint32_t seed)
	{
		randomSeed = seed != 0 ? seed : 1;
		randomState = randomSeed;
	}

	uint32_t getDeterministicRandomSeed(void)
	{
		return randomSeed;
	}

	uint32_t nextDeterministicRandom(void)
	{
		randomState = randomState * 1664525u + 1013904223u;
		return randomState;
	}

	int randomInt(int exclusiveMax)
	{
		if(exclusiveMax <= 0) {
			return 0;
		}

		return static_cast<int>(nextDeterministicRandom() %
					static_cast<uint32_t>(exclusiveMax));
	}

	int randomSignedModulo(int modulo)
	{
		if(modulo <= 0) {
			return 0;
		}

		int a = randomInt(modulo);
		int b = randomInt(modulo);

		return a - b;
	}

};
