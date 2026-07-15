#include "lua_strategy_profile.h"

#include <cmath>

namespace soccer {

	static double tacticalDistanceXZ(const CVector3D &a, const CVector3D &b)
	{
		double dx = a.x - b.x;
		double dz = a.z - b.z;
		return std::sqrt(dx * dx + dz * dz);
	}

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
						     TacticalSuggestion *suggestion,
						     const CBall &ball,
						     bool team01,
						     bool secondHalf)
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

		double goalZ = team01 ? (secondHalf ? 60.0 : -60.0)
				      : (secondHalf ? -60.0 : 60.0);
		double attackDirectionZ = goalZ > suggestion->carrier->pos.z ? 1.0 : -1.0;
		CVector3D goal(0, 0, goalZ);

		for(register int i = 0; i < suggestion->optionCount; i++) {
			TacticalPassOption *option = &suggestion->options[i];
			double passDx = option->target->pos.x - suggestion->carrier->pos.x;
			double passDz = option->target->pos.z - suggestion->carrier->pos.z;
			double forwardPassDistance = attackDirectionZ * passDz;
			double passLaneAngle = std::atan2(std::fabs(passDx), forwardPassDistance) *
				180.0 / 3.14159265358979323846;
			double carrierGoalDistance = tacticalDistanceXZ(suggestion->carrier->pos, goal);

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
			lua_pushnumber(L, suggestion->carrier->pos.x);
			lua_setfield(L, -2, "carrier_x");
			lua_pushnumber(L, suggestion->carrier->pos.z);
			lua_setfield(L, -2, "carrier_z");
			lua_pushnumber(L, option->target->pos.x);
			lua_setfield(L, -2, "target_x");
			lua_pushnumber(L, option->target->pos.z);
			lua_setfield(L, -2, "target_z");
			lua_pushnumber(L, ball.pos.x);
			lua_setfield(L, -2, "ball_x");
			lua_pushnumber(L, ball.pos.z);
			lua_setfield(L, -2, "ball_z");
			lua_pushnumber(L, attackDirectionZ);
			lua_setfield(L, -2, "attack_direction_z");
			lua_pushnumber(L, carrierGoalDistance - option->targetGoalDistance);
			lua_setfield(L, -2, "forward_progress");
			lua_pushnumber(L, std::fabs(option->target->pos.x));
			lua_setfield(L, -2, "target_centrality");
			lua_pushnumber(L, tacticalDistanceXZ(option->target->pos, ball.pos));
			lua_setfield(L, -2, "target_ball_distance");
			lua_pushnumber(L, passLaneAngle);
			lua_setfield(L, -2, "pass_lane_angle");

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
