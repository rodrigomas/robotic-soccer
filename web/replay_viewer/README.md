# Replay Viewer

This is the first static browser shell for Robotic Soccer 2.0 replay analysis.
It loads `fixtures/replays/index.json`, follows the selected replay manifest,
reads the summary plus pass-lane and pressure CSV streams, and renders dashboard
cards with switchable field, timeline, and heatmap focus views. The field view
can toggle pass-lane and pressure overlays independently.

Run it from the repository root with a local static server:

```sh
python3 -m http.server 8080
```

Then open `http://localhost:8080/web/replay_viewer/`.
