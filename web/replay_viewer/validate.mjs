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
   catalog.replays.length < 1) {
	throw new Error("replay catalog shape was not recognized");
}

const entry = catalog.replays[0];
const manifest = await readJson(join(repoRoot, "fixtures", "replays", entry.manifest));
const manifestBase = viewer.directoryName(entry.manifest);
const summary = await readJson(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.summary)));
const passLaneText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.pass_lanes)));
const passLaneRows = viewer.parseCsv(passLaneText);
const latestPassLanes = viewer.latestPassLaneOptions(passLaneRows);
const pressureText = await readText(join(repoRoot, "fixtures", "replays",
	viewer.joinPath(manifestBase, manifest.files.pressure)));
const pressureRows = viewer.parseCsv(pressureText);
const latestPressure = viewer.latestPressureFrame(pressureRows);
const viewModel = viewer.buildReplayViewModel(entry,
	manifest,
	summary,
	passLaneRows,
	pressureRows);

if(viewModel.runId !== "basic_match_fixture" ||
   viewModel.scoreline !== "Botafogo 1 - 0 Flamengo" ||
   viewModel.metrics.length !== 4 ||
   viewModel.tactical.length < 8 ||
   viewModel.passLane.targetNumber !== 9 ||
   viewModel.passLane.options.length !== 2 ||
   latestPassLanes[0].target_number !== "9" ||
   latestPressure.carrier_number !== "11" ||
   viewModel.pressure.opponentNumber !== 5 ||
   viewModel.pressure.distance !== 12) {
	throw new Error("replay viewer view model did not match the fixture");
}

console.log("replay viewer validation passed");
