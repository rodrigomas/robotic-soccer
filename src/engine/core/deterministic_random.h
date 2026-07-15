#ifndef DETERMINISTIC_RANDOM_H
#define DETERMINISTIC_RANDOM_H

#include <stdint.h>

namespace soccer {

	void seedDeterministicRandom(uint32_t seed);
	uint32_t getDeterministicRandomSeed(void);
	uint32_t nextDeterministicRandom(void);
	int randomInt(int exclusiveMax);
	int randomSignedModulo(int modulo);

};

#endif
