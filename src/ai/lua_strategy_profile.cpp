#include "lua_strategy_profile.h"

namespace soccer {

	double LuaStrategyProfile::readNumberField(lua_State *L,
						   const char *name,
						   double fallback)
	{
		double value = fallback;

		lua_getfield(L, -1, name);
		if(lua_isnumber(L, -1)) {
			value = lua_tonumber(L, -1);
		}
		lua_pop(L, 1);

		return value;
	}

	void LuaStrategyProfile::applyTacticalWeights(lua_State *L,
						      TacticalAdvisor *advisor)
	{
		double pressure = 0.65;
		double passDistance = 0.35;
		double goalDistance = 0.20;

		if(!L || !advisor) {
			return;
		}

		lua_getglobal(L, "strategy");
		if(lua_istable(L, -1)) {
			lua_getfield(L, -1, "tactical");
			if(lua_istable(L, -1)) {
				pressure = readNumberField(L, "pressure_weight", pressure);
				passDistance = readNumberField(L, "pass_distance_weight", passDistance);
				goalDistance = readNumberField(L, "goal_distance_weight", goalDistance);
			}
			lua_pop(L, 1);
		}
		lua_pop(L, 1);

		advisor->setWeights(pressure, passDistance, goalDistance);
	}

};
