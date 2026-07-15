# Match Telemetry

The first 2.0 analytics slices record match snapshots, rule events, heatmap summaries, and movement metrics from the existing engine loop. The source lives in `src/analytics/`. When a match scene starts, the game creates CSV files in `config/telemetry/` if the game is launched from `config/`.

The snapshot file is sampled every five active game ticks to keep the first version small enough for quick experiments while still being dense enough for heatmaps.

Each match also writes a `metadata_*.csv` key/value file and a `replay_*.json`
manifest. They record the telemetry format version, run id, creation stamp,
deterministic random seed, team names, sample stride, and the related
snapshot/event/derived-event/heatmap/metrics/pressure/shot/collision/pass-lane/summary file paths. This is the first replay scaffold: a
captured match can now be tied back to the random sequence and files that
produced it.

The JSON manifest is the preferred replay entry point for future tooling because
it keeps the run metadata and file bundle in one structured document.
The engine reader/writer for this file lives in `src/analytics/replay_manifest.*`.
The first report helper is `build/<platform>/tools/replay_report`; pass it a
`replay_*.json` manifest and it prints the compact summary JSON. Use
`--text` for a human-readable match overview.

Tracked sample replays live in `fixtures/replays/`. They are intentionally
small, stable bundles for command-line tools, future browser dashboards, and
documentation screenshots. Runtime telemetry remains ignored under
`config/telemetry/`.

`fixtures/replays/index.json` is the first replay catalog. It uses
`robotic-soccer-replay-catalog` format version 1 and lists sample replay ids,
titles, manifest paths, teams, and descriptions. The catalog reader lives in
`src/analytics/replay_catalog.*`.

`web/replay_viewer/` is the first browser-facing replay shell. It loads the same
catalog and summary fixture used by native smoke tests, follows the manifest
file paths, parses the pass-lane and pressure CSV streams, merges derived
events, shots, and collisions into a compact timeline, and lets the user switch
focus between
dashboard field overlays, timeline, and heatmap views. The field focus can
toggle pass-lane and pressure layers independently, shows ranked pass-lane and
pressure detail rows, and timeline selections highlight matching field context
with the selected event's raw CSV values. The timeline supports Arrow, Home, and
End keyboard navigation using the same selection state as the field and detail
panels, with event-type filters for passes, shots, possession changes, and
collisions plus team scope, player search, and compact per-team counts. The
heatmap view can
filter the same grid by team, entity type, and individual player, and it shows
movement metrics plus possession-zone summaries beside the field grid.

## Metadata CSV Keys

- `format_version`: metadata schema version.
- `run_id`: stable id shared by the generated files for this match capture.
- `created_at`: local timestamp used in telemetry filenames.
- `random_seed`: deterministic gameplay seed. Use `ROBOTIC_SOCCER_SEED=<number>` to replay the same random sequence.
- `team01`, `team02`: configured team names.
- `sample_stride`: active game ticks between snapshot samples.
- `manifest_path`: JSON replay manifest for this match capture.
- `snapshots_path`, `events_path`, `derived_events_path`, `heatmap_path`, `metrics_path`, `pressure_path`, `shots_path`, `collisions_path`, `pass_lanes_path`, `summary_path`: related files for the same match capture.

## Replay Manifest JSON

Manifest files are named `replay_*.json` and include:

- `format`: `robotic-soccer-replay-manifest`.
- `format_version`: manifest schema version.
- `run_id`: stable id shared by the generated files for this match capture.
- `created_at`: local timestamp used in telemetry filenames.
- `random_seed`: deterministic gameplay seed.
- `teams`: `team01` and `team02` names.
- `sample_stride`: active game ticks between snapshot samples.
- `files`: related metadata, snapshot, event, derived event, heatmap, metrics, pressure, shot, collision, pass-lane, and summary paths.

## Match Summary JSON

Summary files are named `summary_*.json`. They are compact dashboard entry
points generated when telemetry finishes. They do not replace the detailed CSV
files; they collect the first replay cards a browser or post-match screen needs
without parsing every stream.

- `format`: `robotic-soccer-match-summary`.
- `format_version`: summary schema version.
- `run_id`, `random_seed`, `teams`: identity and replay provenance.
- `samples`, `ticks`, `last_match_time`: capture length.
- `passes`: derived event count, completed passes, possession changes, and longest inferred pass.
- `pass_lanes`: recorded sample count, option count, latest option count, and latest best target summary.
- `pressure`: sample count, high-pressure samples, average/min/latest distance.
- `shots`: total shots, team shot counts, and latest inferred shot details.
- `collisions`: total contacts, player-player contacts, player-ball contacts, and latest relative speed.

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

The replay viewer derives possession-zone summaries from ball snapshot rows.
Field length is split into defensive, middle, and attacking thirds, mirrored for
the second team.

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

## Shots CSV Columns

Shot files are named `shots_*.csv`. They are inferred from sampled ball velocity
and current possession. The first detector records a shot when the ball moves
fast enough toward the possession team's attacking goal inside a broad shooting
lane, then debounces that movement until the ball slows down or possession
changes.

- `tick`: scene update tick seen by the telemetry recorder.
- `match_time`: in-game seconds from kickoff.
- `team`: team credited with the inferred shot.
- `shooter_number`, `shooter_name`: inferred carrier nearest the ball.
- `shot_speed`: X/Z ball speed when the shot was inferred.
- `forward_speed`: ball speed toward the attacking goal.
- `goal_distance`: remaining Z distance to the target goal line.
- `target_goal_z`: target goal line used for the inference.
- `shooter_x`, `shooter_z`: inferred shooter position.
- `ball_x`, `ball_y`, `ball_z`: ball position.
- `ball_vx`, `ball_vy`, `ball_vz`: ball velocity.

## Collision CSV Columns

Collision files are named `collisions_*.csv`. They infer contact episodes from
sampled player and ball positions using the same public radii used by the
legacy physics objects. Contacts are debounced, so a resting overlap produces
one collision event until the objects separate.

- `tick`: scene update tick seen by the telemetry recorder.
- `match_time`: in-game seconds from kickoff.
- `event_type`: `player_player` or `player_ball`.
- `team_a`, `number_a`, `name_a`: first participant.
- `team_b`, `number_b`, `name_b`: second participant, or `Ball`.
- `distance`: X/Z distance between the participants.
- `relative_speed`: X/Z relative speed between the participants.
- `a_x`, `a_z`, `b_x`, `b_z`: world positions used for the calculation.

## Pass Lane CSV Columns

Pass lane files are named `pass_lanes_*.csv`. They record the top ranked pass
options for the inferred carrier on each telemetry sample. The first ranking is
the engine baseline used by tactical pause: receiver pressure minus pass
distance and goal distance. Lua strategy customization can be layered onto this
later, but the replay export starts with deterministic engine geometry.

- `tick`: scene update tick seen by the telemetry recorder.
- `match_time`: in-game seconds from kickoff.
- `possession_team`: team currently considered to have possession.
- `carrier_number`, `carrier_name`: inferred ball carrier.
- `rank`: option rank for that sample.
- `target_number`, `target_name`: candidate receiver.
- `score`: baseline engine score.
- `pass_distance`: carrier-to-target distance.
- `target_pressure`: nearest-opponent distance around the receiver.
- `target_goal_distance`: receiver distance to the attacking goal.
- `forward_progress`: how much closer the receiver is to the attacking goal than the carrier.
- `carrier_x`, `carrier_z`, `target_x`, `target_z`, `ball_x`, `ball_z`: world positions used for the calculation.

## 2.0 Use

These CSV files are intentionally simple. They can drive the first heatmap, possession-zone, distance, average-speed, pressure, shots, collisions, pass-lane, match timeline, selected-event inspection, player heatmap review, movement panels, and restart prototypes without changing Lua strategy scripts yet.

The next slice should add shot detail rows beside the field overlay.
