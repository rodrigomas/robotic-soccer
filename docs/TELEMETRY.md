# Match Telemetry

The first 2.0 analytics slices record match snapshots, rule events, heatmap summaries, and movement metrics from the existing engine loop. The source lives in `src/analytics/`. When a match scene starts, the game creates CSV files in `config/telemetry/` if the game is launched from `config/`.

The snapshot file is sampled every five active game ticks to keep the first version small enough for quick experiments while still being dense enough for heatmaps.

Each match also writes a `metadata_*.csv` key/value file and a `replay_*.json`
manifest. They record the telemetry format version, run id, creation stamp,
deterministic random seed, team names, sample stride, and the related
snapshot/event/derived-event/heatmap/metrics/pressure file paths. This is the first replay scaffold: a
captured match can now be tied back to the random sequence and files that
produced it.

The JSON manifest is the preferred replay entry point for future tooling because
it keeps the run metadata and file bundle in one structured document.
The engine reader/writer for this file lives in `src/analytics/replay_manifest.*`.

## Metadata CSV Keys

- `format_version`: metadata schema version.
- `run_id`: stable id shared by the generated files for this match capture.
- `created_at`: local timestamp used in telemetry filenames.
- `random_seed`: deterministic gameplay seed. Use `ROBOTIC_SOCCER_SEED=<number>` to replay the same random sequence.
- `team01`, `team02`: configured team names.
- `sample_stride`: active game ticks between snapshot samples.
- `manifest_path`: JSON replay manifest for this match capture.
- `snapshots_path`, `events_path`, `derived_events_path`, `heatmap_path`, `metrics_path`, `pressure_path`: related files for the same match capture.

## Replay Manifest JSON

Manifest files are named `replay_*.json` and include:

- `format`: `robotic-soccer-replay-manifest`.
- `format_version`: manifest schema version.
- `run_id`: stable id shared by the generated files for this match capture.
- `created_at`: local timestamp used in telemetry filenames.
- `random_seed`: deterministic gameplay seed.
- `teams`: `team01` and `team02` names.
- `sample_stride`: active game ticks between snapshot samples.
- `files`: related metadata, snapshot, event, derived event, heatmap, metrics, and pressure paths.

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

## Derived Event CSV Columns

Derived event files are named `derived_events_*.csv`. They are generated from
sampled positions rather than explicit rules. The first detector infers
`pass_completed` when the nearest carrier changes within the possession team,
and `possession_change` when the possession team changes.

The same detector also exposes an in-engine summary for tactical pause and
future replay dashboards: total derived events, completed passes, possession
changes, the longest inferred pass, and the latest inferred event.

- `tick`: scene update tick seen by the telemetry recorder.
- `match_time`: in-game seconds from kickoff.
- `event_type`: `pass_completed` or `possession_change`.
- `from_team`, `from_number`, `from_name`: previous inferred carrier.
- `to_team`, `to_number`, `to_name`: next inferred carrier.
- `pass_distance`: distance between previous and next inferred carriers.
- `ball_x`, `ball_y`, `ball_z`: ball position when the event was inferred.

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

## Pressure CSV Columns

Pressure files are named `pressure_*.csv`. They summarize the distance from the
current inferred ball carrier to the nearest active opponent on each telemetry
sample. Smaller distances mean more pressure; the first high-pressure threshold
is 8 world units.

- `tick`: scene update tick seen by the telemetry recorder.
- `match_time`: in-game seconds from kickoff.
- `possession_team`: team currently considered to have possession.
- `carrier_number`, `carrier_name`: inferred ball carrier.
- `opponent_team`, `opponent_number`, `opponent_name`: nearest active opponent.
- `pressure_distance`: X/Z distance between carrier and nearest opponent.
- `high_pressure`: `1` when `pressure_distance` is inside the high-pressure threshold.
- `carrier_x`, `carrier_z`, `opponent_x`, `opponent_z`: world positions used for the calculation.

## 2.0 Use

These CSV files are intentionally simple. They can drive the first heatmap, possession-zone, distance, average-speed, pressure, match timeline, and restart prototypes without changing Lua strategy scripts yet.

The next slice should infer shots, passes, pressure, collisions, and pass-lane candidates from snapshots plus events.
