import { createRequire } from "node:module";
import { readFile } from "node:fs/promises";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const require = createRequire(import.meta.url);
const viewer = require("./app.js");
const here = dirname(fileURLToPath(import.meta.url));
const repoRoot = join(here, "..", "..");

async function readJson(path) {
	const body = await readFile(path, "utf8");
	return JSON.parse(body);
}

async function readText(path) {
	return readFile(path, "utf8");
}

const catalog = await readJson(join(repoRoot, "fixtures", "replays", "index.json"));

if(catalog.format !== "robotic-soccer-replay-catalog" ||
   catalog.format_version !== 1 ||
   !Array.isArray(catalog.replays) ||
   catalog.replays.length < 1 ||
   viewer.focusViews.join(",") !== "field,timeline,heatmap" ||
   viewer.fieldLayers.join(",") !== "passLanes,pressure") {
	throw new Error("replay catalog shape was not recognized");
}

const entry = catalog.replays[0];
const manifest = await readJson(join(repoRoot, "fixtures", "replays", entry.manifest));
const manifestBase = viewer.directoryName(entry.manifest);
const summary = await readJson(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.summary)));
const snapshotText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.snapshots)));
const snapshotRows = viewer.parseCsv(snapshotText);
const passLaneText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.pass_lanes)));
const passLaneRows = viewer.parseCsv(passLaneText);
const latestPassLanes = viewer.latestPassLaneOptions(passLaneRows);
const pressureText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.pressure)));
const pressureRows = viewer.parseCsv(pressureText);
const latestPressure = viewer.latestPressureFrame(pressureRows);
const derivedText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.derived_events)));
const derivedRows = viewer.parseCsv(derivedText);
const shotText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.shots)));
const shotRows = viewer.parseCsv(shotText);
const timeline = viewer.buildTimelineItems(derivedRows, shotRows);
const selectedShot = viewer.selectedTimelineItem(timeline, "shot-20-0");
const selectedShotDetails = viewer.selectedEventDetails(selectedShot);
const shotEventType = selectedShotDetails.find(([key]) => key === "event_type");
const shotSpeed = selectedShotDetails.find(([key]) => key === "shot_speed");
const shotForwardSpeed = selectedShotDetails.find(([key]) => key === "forward_speed");
const firstTimelineId = timeline[0].id;
const secondTimelineId = timeline[1].id;
const thirdTimelineId = timeline[2].id;
const heatmapText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.heatmap)));
const heatmapRows = viewer.parseCsv(heatmapText);
const heatmap = viewer.buildHeatmap(heatmapRows, [summary.teams.team01, summary.teams.team02]);
const botafogoHeatmap = viewer.filterHeatmap(heatmap, { team: "Botafogo", entityType: "player" });
const flamengoHeatmap = viewer.filterHeatmap(heatmap, { team: "Flamengo", entityType: "player" });
const ballHeatmap = viewer.filterHeatmap(heatmap, { team: "all", entityType: "ball" });
const emptyHeatmap = viewer.filterHeatmap(heatmap, { team: "Botafogo", entityType: "ball" });
const strikerHeatmap = viewer.filterHeatmap(heatmap, {
	team: "Botafogo",
	entityType: "player",
	player: "Botafogo|9|Striker"
});
const wingerHeatmap = viewer.filterHeatmap(heatmap, {
	team: "Botafogo",
	entityType: "player",
	player: "Botafogo|11|Winger"
});
const markerHeatmap = viewer.filterHeatmap(heatmap, {
	team: "all",
	entityType: "player",
	player: "Flamengo|5|Marker"
});
const mismatchedPlayerHeatmap = viewer.filterHeatmap(heatmap, {
	team: "Flamengo",
	entityType: "player",
	player: "Botafogo|9|Striker"
});
const metricsText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.metrics)));
const metricsRows = viewer.parseCsv(metricsText);
const movementMetrics = viewer.buildMovementMetrics(metricsRows);
const allMovement = viewer.summarizeMovementMetrics(movementMetrics, {
	team: "all",
	entityType: "all",
	player: "all"
});
const strikerMovement = viewer.summarizeMovementMetrics(movementMetrics, {
	team: "Botafogo",
	entityType: "player",
	player: "Botafogo|9|Striker"
});
const botafogoMovement = viewer.summarizeMovementMetrics(movementMetrics, {
	team: "Botafogo",
	entityType: "player",
	player: "all"
});
const ballMovement = viewer.summarizeMovementMetrics(movementMetrics, {
	team: "all",
	entityType: "ball",
	player: "all"
});
const possessionZones = viewer.buildPossessionZones(snapshotRows, [summary.teams.team01, summary.teams.team02]);
const possessionRows = viewer.possessionZoneRows(possessionZones);
const viewModel = viewer.buildReplayViewModel(entry,
	manifest,
	summary,
	passLaneRows,
	pressureRows,
	derivedRows,
	shotRows,
	snapshotRows,
	metricsRows,
	heatmapRows);

if(viewModel.runId !== "basic_match_fixture" ||
   viewModel.scoreline !== "Botafogo 1 - 0 Flamengo" ||
   viewModel.metrics.length !== 4 ||
   viewModel.tactical.length < 8 ||
   viewModel.passLane.targetNumber !== 9 ||
   viewModel.passLane.options.length !== 2 ||
   latestPassLanes[0].target_number !== "9" ||
   latestPressure.carrier_number !== "11" ||
   viewModel.pressure.opponentNumber !== 5 ||
   viewModel.pressure.distance !== 12 ||
   timeline.length !== 3 ||
   timeline[0].type !== "pass_completed" ||
   timeline[1].type !== "shot" ||
   timeline[2].type !== "possession_change" ||
   viewer.timelineNavigationTarget(timeline, firstTimelineId, "ArrowLeft") !== firstTimelineId ||
   viewer.timelineNavigationTarget(timeline, firstTimelineId, "ArrowRight") !== secondTimelineId ||
   viewer.timelineNavigationTarget(timeline, secondTimelineId, "ArrowUp") !== firstTimelineId ||
   viewer.timelineNavigationTarget(timeline, secondTimelineId, "ArrowDown") !== thirdTimelineId ||
   viewer.timelineNavigationTarget(timeline, thirdTimelineId, "Home") !== firstTimelineId ||
   viewer.timelineNavigationTarget(timeline, firstTimelineId, "End") !== thirdTimelineId ||
   viewer.timelineNavigationTarget(timeline, firstTimelineId, "Escape") !== "" ||
   viewModel.timeline[1].value !== "16.25" ||
   selectedShot.type !== "shot" ||
   selectedShot.field.x !== -1 ||
   selectedShot.field.fromZ !== 18 ||
   selectedShotDetails.length < 18 ||
   shotEventType?.[1] !== "shot" ||
   shotSpeed?.[1] !== "16.25000" ||
   shotForwardSpeed?.[1] !== "15.75000" ||
   heatmap.cells.length !== 4 ||
   heatmap.maxSamples !== 2 ||
   botafogoHeatmap.cells.length !== 2 ||
   botafogoHeatmap.totalSamples !== 2 ||
   botafogoHeatmap.maxSamples !== 1 ||
   flamengoHeatmap.cells.length !== 1 ||
   flamengoHeatmap.totalSamples !== 2 ||
   ballHeatmap.cells.length !== 1 ||
   ballHeatmap.totalSamples !== 2 ||
   emptyHeatmap.cells.length !== 0 ||
   emptyHeatmap.totalSamples !== 0 ||
   strikerHeatmap.cells.length !== 1 ||
   strikerHeatmap.totalSamples !== 1 ||
   wingerHeatmap.cells.length !== 1 ||
   wingerHeatmap.totalSamples !== 1 ||
   markerHeatmap.cells.length !== 1 ||
   markerHeatmap.totalSamples !== 2 ||
   mismatchedPlayerHeatmap.cells.length !== 0 ||
   mismatchedPlayerHeatmap.totalSamples !== 0 ||
   movementMetrics.length !== 4 ||
   allMovement.rows[1][1] !== "6" ||
   allMovement.rows[2][1] !== "13.73" ||
   allMovement.rows[3][1] !== "7.95" ||
   allMovement.rows[4][1] !== "18.97" ||
   strikerMovement.label !== "#9 Striker (Botafogo)" ||
   strikerMovement.rows[1][1] !== "1" ||
   strikerMovement.rows[2][1] !== "0.00" ||
   strikerMovement.rows[3][1] !== "4.12" ||
   botafogoMovement.label !== "Botafogo players" ||
   botafogoMovement.rows[0][1] !== "2" ||
   botafogoMovement.rows[3][1] !== "3.86" ||
   ballMovement.label !== "Ball" ||
   ballMovement.rows[4][1] !== "18.97" ||
   viewModel.movementMetrics.length !== 4 ||
   possessionZones.totalSamples !== 2 ||
   possessionZones.teams[0].team !== "Botafogo" ||
   possessionZones.teams[0].dominantZone !== "Middle" ||
   possessionZones.teams[0].dominantPercent !== 100 ||
   possessionZones.teams[0].latestZone !== "Middle" ||
   possessionZones.teams[1].team !== "Flamengo" ||
   possessionZones.teams[1].total !== 0 ||
   possessionRows[0][1] !== "2" ||
   possessionRows[1][1] !== "Middle 100%" ||
   possessionRows[3][1] !== "No samples" ||
   viewModel.possessionZones.totalSamples !== 2 ||
   viewModel.heatmap.totalSamples !== 6) {
	throw new Error("replay viewer view model did not match the fixture");
}

console.log("replay viewer validation passed");
