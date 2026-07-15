# Tactical Pause

The first tactical pause slice is intentionally small and engine-owned. Press `P` during a match to pause the simulation and show a live analytics panel.

Current overlay:

- Score context through the existing HUD.
- Team possession percentage.
- Fouls, corners, and throw-ins.
- Ball position and current possession team.
- Fixed timestep step, accumulator, and interpolation alpha.
- Completed passes, possession turns, longest inferred pass, replay seed, derived event count, and manifest filename.
- Current carrier pressure, average pressure, and high-pressure sample count.
- Inferred shot count by team and latest shot speed.
- A deterministic suggested pass lane.
- Lane score, pass distance, goal distance, and opponent pressure.
- A top-down mini field with the possession team's live heatmap, the carrier player's live heatmap, players, ball, and up to three ranked pass lanes.

The pass suggestion lives in `src/analytics/tactical_advisor.*` and uses simple
geometry for now:

- Carrier: nearest player on the possession team to the ball.
- Target: teammate with a weighted score for pass distance, distance to goal, and nearby opponent pressure.
- Output: carrier, ranked pass options, score, pass distance, pressure, and goal distance.

Teams can customize the scoring weights from Lua with `strategy.tactical`:

```lua
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
```

`rank_pass_option` receives one engine-generated option at a time. The stable
fields are:

- `carrier_number`, `target_number`: shirt numbers for the current passer and candidate receiver.
- `score`: current engine score before Lua customization.
- `pass_distance`: world-space pass distance.
- `target_pressure`: distance from the candidate receiver to the nearest opponent.
- `target_goal_distance`: candidate receiver distance to the attacking goal.
- `carrier_x`, `carrier_z`, `target_x`, `target_z`: current positions.
- `ball_x`, `ball_z`: current ball position.
- `attack_direction_z`: `1` or `-1`, depending on the current attacking direction.
- `forward_progress`: how much closer the candidate receiver is to goal than the carrier.
- `target_centrality`: absolute distance from the field center line.
- `target_ball_distance`: candidate receiver distance to the ball.
- `pass_lane_angle`: pass angle in degrees relative to the attacking direction.

Returning a number replaces the engine score before the pass lanes are sorted.
Lua can shape strategy, but geometry queries stay inside the engine.
