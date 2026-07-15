strategy = {
	tactical = {
		pressure_weight = 0.65,
		pass_distance_weight = 0.35,
		goal_distance_weight = 0.20,

		rank_pass_option = function(option)
			return option.score
				+ option.forward_progress * 0.12
				- option.pass_lane_angle * 0.03
		end
	}
}

function think(id,team)
	--io.write("[LUA] Botafogo:" .. id .. "\n")
	--return 1,0,0,2
	return 1,30,-30 + id*5,2
end

function update_pos( id, x, z )
	--io.write("[LUA] Botafogo:" .. id .. " " .. x .. " " .. z .. "\n")
end

function update_vel( id, x, z )
	--io.write("[LUA] Botafogo:" .. id .. " " .. x .. " " .. z .. "\n")
end

function update_ball_vel( x, y, z )
	--io.write("[LUA] Botafogo:" .. x .. " " .. y .. " " .. z .. "\n")
end

function update_ball_pos( x, y, z )
	--io.write("[LUA] Botafogo:" .. x .. " " .. y .. " " .. z .. "\n")
end

function update_goal_pos( z )
	--io.write("[LUA] Botafogo:" .. z .. "\n")
end

function update_enemy_pos( id, x, z )
	--io.write("[LUA] Botafogo:" .. id .. " " .. x .. " " .. z .. "\n")
end
