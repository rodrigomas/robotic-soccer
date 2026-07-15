strategy = {
	tactical = {
		pressure_weight = 0.65,
		pass_distance_weight = 0.35,
		goal_distance_weight = 0.20,

		rank_pass_option = function(option)
			return option.score
		end
	}
}

function think(id,team)
	--io.write("[LUA] Flamengo:" .. id .. "\n")
	return 1,-30,-30 + id*5,2
end

function update_pos( id, x, z )
	--io.write("[LUA] Flamengo:" .. id .. " " .. x .. " " .. z .. "\n")
end

function update_vel( id, x, z )
	--io.write("[LUA] Flamengo:" .. id .. " " .. x .. " " .. z .. "\n")
end

function update_ball_vel( x, y, z )
	--io.write("[LUA] Flamengo:" .. x .. " " .. y .. " " .. z .. "\n")
end

function update_ball_pos( x, y, z )
	--io.write("[LUA] Flamengo:" .. x .. " " .. y .. " " .. z .. "\n")
end

function update_goal_pos( z )
	--io.write("[LUA] Flamengo:" .. z .. "\n")
end

function update_enemy_pos( id, x, z )
	--io.write("[LUA] Flamengo:" .. id .. " " .. x .. " " .. z .. "\n")
end
