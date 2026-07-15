# Robotic Soccer 2.0 Roadmap

Goal: modernize the game while keeping the original spirit: a small engine built from scratch, with external libraries used only for platform access, audio devices, image decoding, and similar infrastructure.

## Phase 0 - Preservation

- Restore the original runtime assets: `config.xml`, team XML files, Lua AI scripts, textures, materials, sounds, and music.
- Add a `data/` or `assets/` root and make all asset paths relative to it.
- Capture short videos/screenshots of the 2007 behavior before changing gameplay.
- Add smoke checks: config parse, team parse, Lua script load, texture decode, audio device open.

## Phase 1 - Engine Skeleton

- Split platform/windowing from game code behind a small interface.
- Keep a custom game loop with fixed timestep simulation and interpolated rendering.
- Replace global scene state gradually with an `EngineContext` carrying time, input, assets, audio, and analytics.
- Move generated objects and build outputs outside `src/`.
- Keep Lua robot scripts working, then version the robot AI API.

## Phase 2 - Physics

- Build a deterministic 2D-on-3D soccer physics core from scratch.
- Use fixed timestep integration, broadphase spatial grid, narrowphase circle/capsule/segment tests, and impulse resolution.
- Model ball spin, rolling friction, wall/goal collisions, robot mass, acceleration, turn radius, and kick impulse.
- Add replayable physics tests with seeded input and golden match traces.
- Expose debug overlays for contacts, velocities, forces, possession, and referee state.

## Phase 3 - Robots And Game Dynamics

- Introduce modular robots: chassis, wheel/drive model, kicker, dribbler, sensor range, battery/heat limits.
- Add formations and tactical roles: goalkeeper, defender, midfielder, striker, support.
- Keep simple local multiplayer, but make AI-vs-AI a first-class simulation mode.
- Add rule tuning for fouls, possession, out-of-bounds, restarts, and match pacing.
- Provide robot/team JSON or XML definitions plus Lua behavior modules.

## Phase 4 - Visuals

- Preserve the handmade renderer, but modernize the pipeline incrementally.
- Replace immediate-mode rendering with retained meshes and a small material system.
- Add camera modes: broadcast, tactical top-down, robot follow, replay.
- Add higher-quality field, robot, ball, shadow, and crowd assets.
- Add debug render layers so engine work stays visible and inspectable.

## Phase 5 - UI And UX

- Replace GLUT menus with an in-engine UI layer.
- Add screens for match setup, team selection, controls, graphics/audio settings, pause, replay, and post-match stats.
- Add a developer overlay for FPS, frame time, physics time, Lua time, collisions, and asset counts.
- Make keyboard/gamepad mapping data-driven.

## Phase 6 - Sound

- Keep OpenAL or a thin audio backend, but wrap it in an engine-owned mixer.
- Add sound categories: UI, robot motors, ball impacts, crowd, whistle, music.
- Add positional audio for on-field events and ducking for whistle/referee events.
- Make missing sound assets non-fatal.

## Phase 7 - Analytics

- Write local match telemetry to JSON/CSV: goals, shots, possession, passes, collisions, fouls, heatmaps, robot distance, average speed.
- Add deterministic replay files: config hash, team definitions, input stream, random seed.
- Build a post-match report view with charts rendered in-engine.
- Add AI evaluation mode for batch matches and rankings.

## Suggested Milestones

1. `1.0-revival`: builds on macOS/Linux, assets restored, original game launches.
2. `1.5-stable`: fixed timestep, asset root, smoke tests, safer audio/texture loading.
3. `2.0-prototype`: new physics core, modular robots, replay telemetry.
4. `2.0-alpha`: modern renderer path, UI layer, analytics dashboard.
5. `2.0`: complete match loop, polished visuals/audio, documented robot API.

## Design Principle

Use libraries to talk to the machine; write the game engine yourself. Window creation, GL loading, audio devices, image codecs, and filesystem helpers can be external. Simulation, scene management, rendering architecture, UI widgets, robot modules, rules, replay, and analytics stay in this repo.
