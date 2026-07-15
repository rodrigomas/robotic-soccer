# Robotic Soccer 2.0 Architecture

This document is the first technical shape for 2.0. The goal is to keep the engine from-scratch while making it portable, testable, scriptable, and web-shareable.

## Core Principle

Use libraries to talk to the machine; write the game yourself.

External libraries can provide window creation, input devices, audio devices, image decoding, OpenGL/WebGPU/WebGL access, and WebAssembly integration. The simulation, rules, robot model, Lua strategy API, renderer structure, UI widgets, analytics, replay, and tactical assistant belong in this repository.

## Target Shape

```text
engine/
  core/        time, math, memory, logging, deterministic random
  platform/    window, input, filesystem, gamepad, browser/native bridge
  assets/      loading, handles, hot reload, asset manifests
  render/      renderer API, debug draw, materials, meshes, overlays
  audio/       mixer, positional sounds, categories
  physics/     deterministic soccer physics
  ai/          Lua bridge, robot API, strategy API
  analytics/   telemetry, heatmaps, replay events, reports

game/
  soccer/      rules, match state, teams, robots, referee, field

tools/
  replay/      replay inspection and conversion tools
  assets/      asset validation and packaging

web/
  shell/       TypeScript app shell for browser workflows
  wasm/        Emscripten build integration

config/
  data/        teams and match config
  scripts/     Lua team behavior
  textures/
  sounds/
  materials/
```

## Engine Loop

The 2.0 loop should be deterministic and replayable:

1. Poll platform input.
2. Convert input and Lua decisions into commands.
3. Step simulation at a fixed timestep.
4. Record replay and analytics events.
5. Render with interpolation.
6. Present debug/tactical overlays.

Variable frame rendering is fine, but simulation should not depend on frame rate.

Current bridge: `src/engine/core/fixed_timestep.*` now provides the first
engine-owned fixed timestep accumulator, and the revived match scene consumes
one deterministic simulation step from it when GLUT idle has accumulated enough
real time.

`src/engine/core/deterministic_random.*` owns gameplay randomness. Native runs
default to a time-based seed, but `ROBOTIC_SOCCER_SEED=<number>` can force a
repeatable random sequence for replay and test work.

## Lua Strategy API

Lua remains a first-class customization layer. The engine should expose stable, versioned APIs:

- `robot_api_v1`: robot movement, kicking, target selection, sensor queries.
- `team_api_v1`: formations, roles, substitutions, risk preferences.
- `strategy_api_v1`: pass ranking, shot ranking, pressure behavior, tactical pause suggestions.

Lua should not own physics or collision detection. Lua should ask the engine for world facts and rank options.

Example direction:

```lua
strategy = {
  risk = 0.35,
  pass_weight = 0.7,
  shot_weight = 0.9,
  support_distance = 14.0,
  prefer_wings = true
}

function rank_options(context, options)
  return score_options(context, options, strategy)
end
```

## Tactical Pause

Tactical pause is a core 2.0 feature, not a debug afterthought.

When paused, the engine should show:

- Team and player heatmaps.
- Possession zones.
- Pass networks.
- Suggested passing lanes.
- Suggested movement targets.
- Shot quality and goal approach hints.
- Player/robot metrics.

The first implementation can use a field grid and score options with simple, explainable heuristics. Later versions can add learning or batch simulation, but the first version should be deterministic and inspectable.

## Web Target

The browser version should run the same core engine through WebAssembly.

Use the browser for:

- Replay viewing.
- Match sharing.
- Team/script editing.
- Analytics dashboards.
- Documentation and tutorials.

Use the engine for:

- Simulation.
- Rendering the field and tactical overlays.
- Lua decisions.
- Replays and telemetry emission.

The web shell can be TypeScript. The engine should remain C++.

Current bridge: `web/replay_viewer/` is the first static browser shell. It reads
the replay catalog and summary fixture directly, renders summary cards and a
field pass-lane view, and validates with `node web/replay_viewer/validate.mjs`.
It is intentionally dependency-free until the TypeScript/WASM boundary needs a
more formal build.

## First Implementation Slice

Start small:

1. Add a documented engine module layout without moving all code at once.
2. Introduce a fixed timestep wrapper around the existing GLUT loop.
3. Add a `MatchTelemetry` collector that records player and ball positions.
4. Add a paused overlay data model for heatmaps and suggestions.
5. Version the current Lua calls as the legacy robot API.
6. Add one smoke test for config/team/Lua loading.

This gives 2.0 a skeleton without breaking the revived 1.0 game.
