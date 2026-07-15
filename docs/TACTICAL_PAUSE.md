# Tactical Pause

The first tactical pause slice is intentionally small and engine-owned. Press `P` during a match to pause the simulation and show a live analytics panel.

Current overlay:

- Score context through the existing HUD.
- Team possession percentage.
- Fouls, corners, and throw-ins.
- Ball position and current possession team.
- A deterministic suggested pass lane.
- Lane score, pass distance, goal distance, and opponent pressure.
- A top-down mini field with players, ball, and up to three ranked pass lanes.

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

`rank_pass_option` receives one engine-generated option at a time with
`carrier_number`, `target_number`, `score`, `pass_distance`, `target_pressure`,
and `target_goal_distance`. Returning a number replaces the engine score before
the pass lanes are sorted.

Future 2.0 slices should expose richer context to Lua while keeping geometry queries in the engine.
