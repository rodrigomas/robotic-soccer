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
