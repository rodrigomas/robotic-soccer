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

	void LuaStrategyProfile::rankTacticalOptions(lua_State *L,
						     TacticalSuggestion *suggestion)
	{
		if(!L || !suggestion || !suggestion->carrier || suggestion->optionCount <= 0) {
			return;
		}

		lua_getglobal(L, "strategy");
		if(!lua_istable(L, -1)) {
			lua_pop(L, 1);
			return;
		}

		lua_getfield(L, -1, "tactical");
		if(!lua_istable(L, -1)) {
			lua_pop(L, 2);
			return;
		}

		lua_getfield(L, -1, "rank_pass_option");
		if(!lua_isfunction(L, -1)) {
			lua_pop(L, 3);
			return;
		}

		for(register int i = 0; i < suggestion->optionCount; i++) {
			TacticalPassOption *option = &suggestion->options[i];

			lua_pushvalue(L, -1);
			lua_newtable(L);

			lua_pushinteger(L, suggestion->carrier->num);
			lua_setfield(L, -2, "carrier_number");
			lua_pushinteger(L, option->target->num);
			lua_setfield(L, -2, "target_number");
			lua_pushnumber(L, option->score);
			lua_setfield(L, -2, "score");
			lua_pushnumber(L, option->passDistance);
			lua_setfield(L, -2, "pass_distance");
			lua_pushnumber(L, option->targetPressure);
			lua_setfield(L, -2, "target_pressure");
			lua_pushnumber(L, option->targetGoalDistance);
			lua_setfield(L, -2, "target_goal_distance");

			if(lua_pcall(L, 1, 1, 0) == 0) {
				if(lua_isnumber(L, -1)) {
					option->score = lua_tonumber(L, -1);
				}
			}
			lua_pop(L, 1);
		}

		lua_pop(L, 3);
		TacticalAdvisor::refreshSuggestion(suggestion);
	}

};
