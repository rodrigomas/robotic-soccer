# Match Telemetry

The first 2.0 slice records match snapshots from the existing engine loop. When a match scene starts, the game creates a CSV file in `config/telemetry/` if the game is launched from `config/`.

The file is sampled every five active game ticks to keep the first version small enough for quick experiments while still being dense enough for heatmaps.

## CSV Columns

- `sample`: sequential telemetry sample index.
- `tick`: scene update tick seen by the telemetry recorder.
- `match_time`: in-game seconds from kickoff.
- `entity_type`: `ball` or `player`.
- `team`: team name for players; empty for the ball.
- `number`: player shirt number; `0` for the ball.
- `name`: player name or `Ball`.
- `x`, `y`, `z`: world position.
- `vx`, `vy`, `vz`: world velocity.
- `team_in_possession`: team name currently considered to have possession.

## 2.0 Use

This CSV is intentionally simple. It can drive the first heatmap, possession-zone, distance, and average-speed prototypes without changing Lua strategy scripts yet.

The next slice should add event telemetry for goals, shots, passes, collisions, fouls, and restarts.
