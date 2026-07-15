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
const collisionText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.collisions)));
const collisionRows = viewer.parseCsv(collisionText);
const timeline = viewer.buildTimelineItems(derivedRows, shotRows, collisionRows);
const selectedShot = viewer.selectedTimelineItem(timeline, "shot-20-0");
const passCompleted = viewer.selectedTimelineItem(timeline, "pass_completed-10-0");
const selectedShotDetails = viewer.selectedEventDetails(selectedShot);
const selectedShotSummary = viewer.selectedShotDetails(selectedShot);
const nonShotSummary = viewer.selectedShotDetails(passCompleted);
const shotEventType = selectedShotDetails.find(([key]) => key === "event_type");
const shotSpeed = selectedShotDetails.find(([key]) => key === "shot_speed");
const shotForwardSpeed = selectedShotDetails.find(([key]) => key === "forward_speed");
const firstTimelineId = timeline[0].id;
const secondTimelineId = timeline[1].id;
const thirdTimelineId = timeline[2].id;
const playerBallCollision = viewer.selectedTimelineItem(timeline, "player_ball-8-0");
const playerPlayerCollision = viewer.selectedTimelineItem(timeline, "player_player-18-1");
const possessionChange = viewer.selectedTimelineItem(timeline, "possession_change-25-1");
const passTimeline = viewer.filterTimelineItems(timeline, {
	pass_completed: true,
	shot: false,
	possession_change: false,
	collision: false
});
const shotTimeline = viewer.filterTimelineItems(timeline, {
	pass_completed: false,
	shot: true,
	possession_change: false,
	collision: false
});
const tacticalTimeline = viewer.filterTimelineItems(timeline, {
	pass_completed: true,
	shot: true,
	possession_change: false,
	collision: false
});
const emptyTimeline = viewer.filterTimelineItems(timeline, {
	pass_completed: false,
	shot: false,
	possession_change: false,
	collision: false
});
const collisionTimeline = viewer.filterTimelineItems(timeline, {
	pass_completed: false,
	shot: false,
	possession_change: false,
	collision: true
});
const botafogoTimeline = viewer.filterTimelineItems(timeline, {
	team: "Botafogo",
	types: {
		pass_completed: true,
		shot: true,
		possession_change: true,
		collision: true
	}
});
const flamengoTimeline = viewer.filterTimelineItems(timeline, {
	team: "Flamengo",
	types: {
		pass_completed: true,
		shot: true,
		possession_change: true,
		collision: true
	}
});
const flamengoShotsTimeline = viewer.filterTimelineItems(timeline, {
	team: "Flamengo",
	types: {
		pass_completed: false,
		shot: true,
		possession_change: false,
		collision: false
	}
});
const strikerSearchTimeline = viewer.filterTimelineItems(timeline, {
	team: "all",
	query: "Striker",
	types: {
		pass_completed: true,
		shot: true,
		possession_change: true,
		collision: true
	}
});
const shirtSearchTimeline = viewer.filterTimelineItems(timeline, {
	team: "all",
	query: "#9",
	types: {
		pass_completed: true,
		shot: true,
		possession_change: true,
		collision: true
	}
});
const markerSearchTimeline = viewer.filterTimelineItems(timeline, {
	team: "Flamengo",
	query: "Marker",
	types: {
		pass_completed: true,
		shot: true,
		possession_change: true,
		collision: true
	}
});
const mismatchedSearchTimeline = viewer.filterTimelineItems(timeline, {
	team: "Flamengo",
	query: "Striker",
	types: {
		pass_completed: true,
		shot: true,
		possession_change: true,
		collision: true
	}
});
const allTimelineCounts = viewer.timelineTeamCounts(timeline, [summary.teams.team01, summary.teams.team02]);
const botafogoTimelineCounts = viewer.timelineTeamCounts(botafogoTimeline, [summary.teams.team01, summary.teams.team02]);
const flamengoTimelineCounts = viewer.timelineTeamCounts(flamengoTimeline, [summary.teams.team01, summary.teams.team02]);
const shotTimelineCounts = viewer.timelineTeamCounts(shotTimeline, [summary.teams.team01, summary.teams.team02]);
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
	collisionRows,
	snapshotRows,
	metricsRows,
	heatmapRows);

if(viewModel.runId !== "basic_match_fixture" ||
   viewModel.scoreline !== "Botafogo 1 - 0 Flamengo" ||
   viewModel.metrics.length !== 4 ||
   viewModel.tactical.length < 8 ||
   viewModel.passLane.targetNumber !== 9 ||
   viewModel.passLane.options.length !== 2 ||
   viewModel.passLane.options[0].targetName !== "Striker" ||
   viewModel.passLane.options[0].score !== 13.5 ||
   viewModel.passLane.options[0].targetPressure !== 12 ||
   viewModel.passLane.options[0].targetGoalDistance !== 42 ||
   viewModel.passLane.options[0].forwardProgress !== 10 ||
   viewModel.passLane.options[1].targetNumber !== 7 ||
   viewModel.passLane.options[1].targetGoalDistance !== 50 ||
   viewModel.passLane.options[1].forwardProgress !== 4 ||
   latestPassLanes[0].target_number !== "9" ||
   latestPressure.carrier_number !== "11" ||
   viewModel.pressure.carrierNumber !== 11 ||
   viewModel.pressure.carrierName !== "Winger" ||
   viewModel.pressure.opponentNumber !== 5 ||
   viewModel.pressure.opponentName !== "Marker" ||
   viewModel.pressure.distance !== 12 ||
   viewModel.pressure.high !== false ||
   viewModel.pressure.carrierX !== 12 ||
   viewModel.pressure.carrierZ !== 1 ||
   viewModel.pressure.opponentX !== 18 ||
   viewModel.pressure.opponentZ !== 4 ||
   timeline.length !== 5 ||
   timeline[0].type !== "player_ball" ||
   timeline[1].type !== "pass_completed" ||
   timeline[2].type !== "player_player" ||
   timeline[3].type !== "shot" ||
   timeline[4].type !== "possession_change" ||
   playerBallCollision.detail !== "Striker #9 with Ball" ||
   playerBallCollision.value !== "6.75" ||
   playerBallCollision.field.x !== -5 ||
   playerPlayerCollision.detail !== "Winger #11 with Marker #5" ||
   playerPlayerCollision.teams.length !== 2 ||
   playerPlayerCollision.teams[1] !== "Flamengo" ||
   passTimeline.length !== 1 ||
   passTimeline[0].type !== "pass_completed" ||
   shotTimeline.length !== 1 ||
   shotTimeline[0].id !== selectedShot.id ||
   tacticalTimeline.length !== 2 ||
   tacticalTimeline[1].type !== "shot" ||
   emptyTimeline.length !== 0 ||
   collisionTimeline.length !== 2 ||
   collisionTimeline[0].type !== "player_ball" ||
   collisionTimeline[1].type !== "player_player" ||
   botafogoTimeline.length !== 4 ||
   botafogoTimeline[0].team !== "Botafogo" ||
   botafogoTimeline[3].type !== "shot" ||
   flamengoTimeline.length !== 2 ||
   flamengoTimeline[0].type !== "player_player" ||
   flamengoTimeline[1].type !== "possession_change" ||
   flamengoShotsTimeline.length !== 0 ||
   strikerSearchTimeline.length !== 3 ||
   strikerSearchTimeline[0].type !== "player_ball" ||
   strikerSearchTimeline[1].type !== "pass_completed" ||
   strikerSearchTimeline[2].type !== "shot" ||
   shirtSearchTimeline.length !== 3 ||
   markerSearchTimeline.length !== 2 ||
   markerSearchTimeline[0].type !== "player_player" ||
   markerSearchTimeline[1].team !== "Flamengo" ||
   mismatchedSearchTimeline.length !== 0 ||
   allTimelineCounts[0].total !== 4 ||
   allTimelineCounts[0].passCompleted !== 1 ||
   allTimelineCounts[0].shots !== 1 ||
   allTimelineCounts[0].collisions !== 2 ||
   allTimelineCounts[1].total !== 2 ||
   allTimelineCounts[1].possessionChanges !== 1 ||
   allTimelineCounts[1].collisions !== 1 ||
   botafogoTimelineCounts[0].total !== 4 ||
   botafogoTimelineCounts[1].total !== 1 ||
   flamengoTimelineCounts[0].total !== 1 ||
   flamengoTimelineCounts[1].total !== 2 ||
   shotTimelineCounts[0].shots !== 1 ||
   shotTimelineCounts[1].shots !== 0 ||
   viewer.repairedTimelineSelection(passTimeline, selectedShot.id) !== passCompleted.id ||
   viewer.repairedTimelineSelection(tacticalTimeline, selectedShot.id) !== selectedShot.id ||
   viewer.repairedTimelineSelection(flamengoTimeline, selectedShot.id) !== playerPlayerCollision.id ||
   viewer.repairedTimelineSelection(flamengoShotsTimeline, selectedShot.id) !== "" ||
   viewer.repairedTimelineSelection(markerSearchTimeline, selectedShot.id) !== playerPlayerCollision.id ||
   viewer.repairedTimelineSelection(mismatchedSearchTimeline, selectedShot.id) !== "" ||
   viewer.repairedTimelineSelection(emptyTimeline, selectedShot.id) !== "" ||
   viewer.timelineNavigationTarget(timeline, firstTimelineId, "ArrowLeft") !== firstTimelineId ||
   viewer.timelineNavigationTarget(timeline, firstTimelineId, "ArrowRight") !== secondTimelineId ||
   viewer.timelineNavigationTarget(timeline, secondTimelineId, "ArrowUp") !== firstTimelineId ||
   viewer.timelineNavigationTarget(timeline, secondTimelineId, "ArrowDown") !== thirdTimelineId ||
   viewer.timelineNavigationTarget(timeline, thirdTimelineId, "Home") !== firstTimelineId ||
   viewer.timelineNavigationTarget(timeline, firstTimelineId, "End") !== possessionChange.id ||
   viewer.timelineNavigationTarget(timeline, firstTimelineId, "Escape") !== "" ||
   viewModel.timeline[3].value !== "16.25" ||
   selectedShot.type !== "shot" ||
   selectedShot.field.x !== -1 ||
   selectedShot.field.fromZ !== 18 ||
   selectedShotDetails.length < 18 ||
   selectedShotSummary.length !== 9 ||
   selectedShotSummary[0][1] !== "#9 Striker" ||
   selectedShotSummary[2][1] !== "16.25" ||
   selectedShotSummary[3][1] !== "15.75" ||
   selectedShotSummary[4][1] !== "42.00" ||
   selectedShotSummary[5][1] !== "60" ||
   selectedShotSummary[6][1] !== "-3.00, 18.00" ||
   selectedShotSummary[7][1] !== "-1.00, 18.00" ||
   selectedShotSummary[8][1] !== "2.00, 0.00, 16.00" ||
   nonShotSummary.length !== 0 ||
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
