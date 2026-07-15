local option = {
	score = 10.0,
	pass_distance = 18.0,
	target_pressure = 7.0,
	target_goal_distance = 35.0,
	carrier_number = 5,
	target_number = 9,
	carrier_x = -4.0,
	carrier_z = 12.0,
	target_x = 2.0,
	target_z = 30.0,
	ball_x = -3.5,
	ball_z = 11.0,
	attack_direction_z = 1.0,
	forward_progress = 15.0,
	target_centrality = 2.0,
	target_ball_distance = 20.0,
	pass_lane_angle = 18.0,
}

local function check_strategy(path)
	strategy = nil
	dofile(path)

	assert(type(strategy) == "table", path .. " did not define strategy")
	assert(type(strategy.tactical) == "table", path .. " did not define strategy.tactical")
	assert(type(strategy.tactical.rank_pass_option) == "function",
		path .. " did not define rank_pass_option")

	local score = strategy.tactical.rank_pass_option(option)
	assert(type(score) == "number", path .. " rank_pass_option did not return a number")
	assert(score == score, path .. " rank_pass_option returned NaN")
end

check_strategy("scripts/botafogo.lua")
check_strategy("scripts/flamengo.lua")
