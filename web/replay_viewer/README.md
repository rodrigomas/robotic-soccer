# Replay Viewer

This is the first static browser shell for Robotic Soccer 2.0 replay analysis.
It loads `fixtures/replays/index.json`, follows the selected replay manifest,
reads the summary plus pass-lane and pressure CSV streams, and renders dashboard
cards with switchable field, timeline, and heatmap focus views. The field view
can toggle pass-lane and pressure overlays independently, with ranked pass-lane
and pressure detail rows below the field plus selected-shot detail rows beside
the field. Timeline clicks or keyboard navigation highlight matching field
context with raw selected-event and shot-specific detail panels.
Timeline event-type filters can narrow passes, shots, possession changes, and
collisions by team or player search, with compact per-team counts. The heatmap
view can filter samples by team and entity type, then drill into a single player
with movement metrics beside the grid. It also summarizes possession zones from
ball snapshots.

Run it from the repository root with a local static server:

```sh
python3 -m http.server 8080
```

Then open `http://localhost:8080/web/replay_viewer/`.
