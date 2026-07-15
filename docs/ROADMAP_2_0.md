# Robotic Soccer 2.0 Roadmap

Goal: modernize the game while keeping the original spirit: a small engine built from scratch, with external libraries used only for platform access, audio devices, image decoding, and similar infrastructure.

## Product Direction

Robotic Soccer 2.0 should become a programmable soccer simulator: a native game during development, a web-shareable experience for players and reviewers, and an inspectable tactical lab for AI experiments.

The web target is first-class, but the engine should remain portable C++ rather than becoming a browser-only application. The browser shell can host replays, dashboards, team editors, docs, and sharing, while the simulation, rules, robot model, analytics, and renderer remain engine-owned.

## Tech Stack

- Core engine: C++17 or C++20.
- Scripting: Lua, with a versioned robot and strategy API.
- Native platform layer: SDL3 or GLFW, with SDL3 preferred for input/audio ergonomics.
- Web target: Emscripten/WebAssembly.
- Rendering: renderer abstraction with a WebGPU-oriented design, WebGL2-compatible fallback, and a short-term OpenGL revival bridge.
- Browser shell: TypeScript with a lightweight app layer for replays, analytics dashboards, team/script editing, and documentation.
- Data: deterministic replay files plus JSON/CSV exports for match telemetry.
- Build: keep native and web builds side by side, with generated files outside `src/`.

## 2.0 Architecture

See [`ARCHITECTURE_2_0.md`](ARCHITECTURE_2_0.md) for the first technical shape.

## First Slice In Progress

- Match snapshot telemetry now writes CSV files for ball and player positions from the existing match loop.
- See [`TELEMETRY.md`](TELEMETRY.md) for the first CSV format.
- A first tactical pause overlay is available in-match with possession, restart stats, and a suggested pass lane. See [`TACTICAL_PAUSE.md`](TACTICAL_PAUSE.md).

## Phase 0 - Preservation

- Restore the original runtime assets: `config.xml`, team XML files, Lua AI scripts, textures, materials, sounds, and music.
- Add a `data/` or `assets/` root and make all asset paths relative to it.
- Capture short videos/screenshots of the 2007 behavior before changing gameplay.
- Add smoke checks: config parse, team parse, Lua script load, texture decode, audio device open.
- Keep this phase as the compatibility baseline for future 2.0 changes.

## Phase 1 - Engine Skeleton

- Split platform/windowing from game code behind a small interface.
- Keep a custom game loop with fixed timestep simulation and interpolated rendering.
- Replace global scene state gradually with an `EngineContext` carrying time, input, assets, audio, and analytics.
- Move generated objects and build outputs outside `src/`.
- Keep Lua robot scripts working, then version the robot AI API.
- Introduce module boundaries for `engine/core`, `engine/platform`, `engine/render`, `engine/audio`, `engine/physics`, `engine/ai`, `engine/analytics`, and `game/soccer`.

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
- Add a strategy API so teams can customize formations, pass ranking, risk tolerance, pressing, defensive shape, and target selection without rewriting engine geometry.

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
- Add a tactical pause mode for analytics, replay inspection, heatmaps, pass suggestions, and strategy overlays.

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

## Phase 8 - Web Experience

- Compile the engine to WebAssembly with Emscripten.
- Run the simulation and renderer in a browser canvas.
- Add a TypeScript shell for match setup, replay browser, team/script editor, analytics dashboard, and sharing.
- Store local replays/configs through browser storage.
- Make tactical pause overlays work both native and web.

## Tactical Analytics

The pause mode should freeze simulation and expose tactical state:

- Team and player heatmaps.
- Ball possession zones.
- Pass networks and suggested passing lanes.
- Shot angles and goal probability hints.
- Player speed, distance, stamina/heat, collisions, and fouls.
- Strategy suggestions drawn as circles, arrows, and target zones.

The first strategy assistant can use a field grid and score each option by:

- Distance and angle to goal.
- Opponent pressure.
- Teammate support.
- Pass safety.
- Dribble space.
- Shot quality.
- Team risk preference from Lua.

Lua should be able to customize the scoring weights, but the engine should own geometry queries, field sampling, and replayable analytics.

## Suggested Milestones

1. `1.0-revival`: builds on macOS/Linux, assets restored, original game launches.
2. `1.5-stable`: fixed timestep, asset root, smoke tests, safer audio/texture loading.
3. `2.0-foundation`: module boundaries, platform abstraction, fixed timestep, replay seed, Lua API boundary.
4. `2.0-prototype`: new physics core, modular robots, replay telemetry, first tactical pause.
5. `2.0-web-preview`: engine compiled to WebAssembly with replay viewing in the browser.
6. `2.0-alpha`: modern renderer path, UI layer, analytics dashboard, Lua strategy API.
7. `2.0`: complete match loop, polished visuals/audio, documented robot API, browser-shareable matches.

## Design Principle

Use libraries to talk to the machine; write the game engine yourself. Window creation, GL loading, audio devices, image codecs, and filesystem helpers can be external. Simulation, scene management, rendering architecture, UI widgets, robot modules, rules, replay, and analytics stay in this repo.
