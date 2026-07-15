#ifndef LUA_STRATEGY_PROFILE_H
#define LUA_STRATEGY_PROFILE_H

#include "analytics/tactical_advisor.h"

#include <lua.h>

namespace soccer {

	class LuaStrategyProfile {

		static double readNumberField(lua_State *L, const char *name, double fallback);

	public:
		static void applyTacticalWeights(lua_State *L, TacticalAdvisor *advisor);
	};

};

#endif
