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

const catalog = await readJson(join(repoRoot, "fixtures", "replays", "index.json"));

if(catalog.format !== "robotic-soccer-replay-catalog" ||
   catalog.format_version !== 1 ||
   !Array.isArray(catalog.replays) ||
   catalog.replays.length < 1) {
	throw new Error("replay catalog shape was not recognized");
}

const entry = catalog.replays[0];
const summaryPath = viewer.summaryPathFromManifest(entry.manifest);
const summary = await readJson(join(repoRoot, "fixtures", "replays", summaryPath));
const viewModel = viewer.buildReplayViewModel(entry, summary);

if(viewModel.runId !== "basic_match_fixture" ||
   viewModel.scoreline !== "Botafogo 1 - 0 Flamengo" ||
   viewModel.metrics.length !== 4 ||
   viewModel.tactical.length < 8 ||
   viewModel.passLane.targetNumber !== 11) {
	throw new Error("replay viewer view model did not match the fixture");
}

console.log("replay viewer validation passed");
