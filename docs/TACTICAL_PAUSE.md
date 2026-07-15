# Tactical Pause

The first tactical pause slice is intentionally small and engine-owned. Press `P` during a match to pause the simulation and show a live analytics panel.

Current overlay:

- Score context through the existing HUD.
- Team possession percentage.
- Fouls, corners, and throw-ins.
- Ball position and current possession team.
- A deterministic suggested pass lane.
- A top-down mini field with players, ball, and the suggested target.

The pass suggestion lives in `src/analytics/tactical_advisor.*` and uses simple
geometry for now:

- Carrier: nearest player on the possession team to the ball.
- Target: teammate with a weighted score for pass distance, distance to goal, and nearby opponent pressure.
- Output: carrier, target, score, pass distance, pressure, and goal distance.

Teams can customize the scoring weights from Lua with `strategy.tactical`:

```lua
strategy = {
  tactical = {
    pressure_weight = 0.65,
    pass_distance_weight = 0.35,
    goal_distance_weight = 0.20
  }
}
```

Future 2.0 slices should let Lua rank engine-generated options while keeping geometry queries in the engine.
