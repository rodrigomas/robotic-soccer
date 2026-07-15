# Match Telemetry

The first 2.0 analytics slices record match snapshots, rule events, heatmap summaries, and movement metrics from the existing engine loop. The source lives in `src/analytics/`. When a match scene starts, the game creates CSV files in `config/telemetry/` if the game is launched from `config/`.

The snapshot file is sampled every five active game ticks to keep the first version small enough for quick experiments while still being dense enough for heatmaps.

## Snapshot CSV Columns

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

## Event CSV Columns

Event files are named `events_*.csv` and record discrete match events:

- `tick`: scene update tick seen by the telemetry recorder.
- `match_time`: in-game seconds from kickoff.
- `event_type`: `match_start`, `throw_in`, `corner`, `goal`, `foul`, `penalty`, `halftime`, or `full_time`.
- `team`: team associated with the event, if any.
- `number`: player shirt number, if any.
- `name`: player name, if any.
- `x`, `y`, `z`: event world position.
- `team_in_possession`: team name currently considered to have possession.
- `detail`: extra context such as `own_goal`, `restart_to_team1`, or `free_kick_to_team2`.

## Heatmap CSV Columns

Heatmap files are named `heatmap_*.csv`. They aggregate sampled positions into an 18 by 24 grid over the current field lines, x `-45..45` and z `-60..60`.

- `entity_type`: `ball` or `player`.
- `team`: team name for players; empty for the ball.
- `number`: player shirt number; `0` for the ball.
- `name`: player name or `Ball`.
- `column`: heatmap grid column.
- `row`: heatmap grid row.
- `x_min`, `x_max`, `z_min`, `z_max`: world-space bounds for the cell.
- `samples`: number of sampled positions in the cell.

## Metrics CSV Columns

Metrics files are named `metrics_*.csv`. They summarize sampled movement per ball/player.

- `entity_type`: `ball` or `player`.
- `team`: team name for players; empty for the ball.
- `number`: player shirt number; `0` for the ball.
- `name`: player name or `Ball`.
- `samples`: number of sampled positions used.
- `distance`: accumulated world-space distance between samples.
- `average_speed`: average sampled velocity magnitude.
- `max_speed`: maximum sampled velocity magnitude.

## 2.0 Use

These CSV files are intentionally simple. They can drive the first heatmap, possession-zone, distance, average-speed, match timeline, and restart prototypes without changing Lua strategy scripts yet.

The next slice should infer shots, passes, pressure, collisions, and pass-lane candidates from snapshots plus events.
