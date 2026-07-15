(function(root, factory) {
	if(typeof module === "object" && module.exports) {
		module.exports = factory();
	} else {
		root.ReplayViewer = factory();
	}
}(typeof self !== "undefined" ? self : this, function() {
	"use strict";

	function formatNumber(value, digits) {
		if(!Number.isFinite(value)) {
			return "-";
		}
		return value.toFixed(digits);
	}

	function summaryPathFromManifest(manifestPath) {
		var lastSlash = manifestPath.lastIndexOf("/");
		var base = lastSlash >= 0 ? manifestPath.slice(0, lastSlash + 1) : "";
		return base + "summary" + manifestPath.slice(base.length + "replay".length);
	}

	function buildReplayViewModel(entry, summary) {
		return {
			id: entry.id,
			title: entry.title,
			description: entry.description,
			runId: summary.run_id,
			teams: [summary.teams.team01, summary.teams.team02],
			scoreline: summary.teams.team01 + " " + summary.shots.team01 + " - " +
				summary.shots.team02 + " " + summary.teams.team02,
			matchTime: formatNumber(summary.last_match_time, 2) + "s",
			metrics: [
				{
					label: "Completed Passes",
					value: String(summary.passes.completed),
					detail: summary.passes.possession_changes + " possession changes"
				},
				{
					label: "Pressure",
					value: summary.pressure.high_pressure_samples + "/" + summary.pressure.samples,
					detail: "avg " + formatNumber(summary.pressure.average_distance, 2) +
						", min " + formatNumber(summary.pressure.min_distance, 2)
				},
				{
					label: "Shots",
					value: String(summary.shots.total),
					detail: "last " + summary.shots.last_team + " #" +
						summary.shots.last_shooter_number
				},
				{
					label: "Collisions",
					value: String(summary.collisions.total),
					detail: summary.collisions.player_player + " player, " +
						summary.collisions.player_ball + " ball"
				}
			],
			tactical: [
				["Seed", String(summary.random_seed)],
				["Samples", String(summary.samples)],
				["Ticks", String(summary.ticks)],
				["Longest pass", formatNumber(summary.passes.longest_distance, 2)],
				["Pass options", String(summary.pass_lanes.options)],
				["Best target", "#" + summary.pass_lanes.last_best_target_number],
				["Best score", formatNumber(summary.pass_lanes.last_best_score, 2)],
				["Pass distance", formatNumber(summary.pass_lanes.last_best_pass_distance, 2)]
			],
			passLane: {
				label: "#" + summary.pass_lanes.last_best_target_number +
					" score " + formatNumber(summary.pass_lanes.last_best_score, 2),
				targetNumber: summary.pass_lanes.last_best_target_number,
				distance: summary.pass_lanes.last_best_pass_distance
			}
		};
	}

	function createEl(documentRef, tag, className, text) {
		var el = documentRef.createElement(tag);
		if(className) {
			el.className = className;
		}
		if(text !== undefined) {
			el.textContent = text;
		}
		return el;
	}

	function renderCatalog(documentRef, catalogEl, entries, selectedId, onSelect) {
		catalogEl.innerHTML = "";
		entries.forEach(function(entry) {
			var button = createEl(documentRef, "button", "replay-button", "");
			button.type = "button";
			button.className = "replay-button" + (entry.id === selectedId ? " active" : "");
			button.appendChild(createEl(documentRef, "strong", "", entry.title));
			button.appendChild(createEl(documentRef, "span", "", entry.team01 + " vs " + entry.team02));
			button.appendChild(createEl(documentRef, "span", "", entry.description));
			button.addEventListener("click", function() {
				onSelect(entry.id);
			});
			catalogEl.appendChild(button);
		});
	}

	function renderMetrics(documentRef, metricsEl, metrics) {
		metricsEl.innerHTML = "";
		metrics.forEach(function(metric) {
			var item = createEl(documentRef, "article", "metric", "");
			item.appendChild(createEl(documentRef, "div", "metric-label", metric.label));
			item.appendChild(createEl(documentRef, "div", "metric-value", metric.value));
			item.appendChild(createEl(documentRef, "div", "metric-detail", metric.detail));
			metricsEl.appendChild(item);
		});
	}

	function renderTacticalSummary(documentRef, summaryEl, rows) {
		summaryEl.innerHTML = "";
		rows.forEach(function(row) {
			summaryEl.appendChild(createEl(documentRef, "dt", "", row[0]));
			summaryEl.appendChild(createEl(documentRef, "dd", "", row[1]));
		});
	}

	function svgNode(documentRef, tag, attrs) {
		var node = documentRef.createElementNS("http://www.w3.org/2000/svg", tag);
		Object.keys(attrs).forEach(function(key) {
			node.setAttribute(key, attrs[key]);
		});
		return node;
	}

	function renderField(documentRef, svgEl, viewModel) {
		svgEl.innerHTML = "";
		svgEl.appendChild(svgNode(documentRef, "rect", {
			x: "0", y: "0", width: "540", height: "360", fill: "#2f7d4d"
		}));
		for(var stripe = 0; stripe < 6; stripe++) {
			svgEl.appendChild(svgNode(documentRef, "rect", {
				x: String(stripe * 90), y: "0", width: "45", height: "360",
				fill: "#286f43", opacity: "0.35"
			}));
		}
		svgEl.appendChild(svgNode(documentRef, "rect", {
			x: "22", y: "22", width: "496", height: "316",
			fill: "none", stroke: "#f4f7ee", "stroke-width": "4"
		}));
		svgEl.appendChild(svgNode(documentRef, "line", {
			x1: "270", y1: "22", x2: "270", y2: "338",
			stroke: "#f4f7ee", "stroke-width": "3", opacity: "0.8"
		}));
		svgEl.appendChild(svgNode(documentRef, "circle", {
			cx: "270", cy: "180", r: "54", fill: "none",
			stroke: "#f4f7ee", "stroke-width": "3", opacity: "0.8"
		}));
		svgEl.appendChild(svgNode(documentRef, "line", {
			x1: "190", y1: "226", x2: "350", y2: "126",
			stroke: "#f5d06d", "stroke-width": "6", "stroke-linecap": "round"
		}));
		svgEl.appendChild(svgNode(documentRef, "circle", {
			cx: "190", cy: "226", r: "17", fill: "#1f5f99", stroke: "#ffffff", "stroke-width": "4"
		}));
		svgEl.appendChild(svgNode(documentRef, "circle", {
			cx: "350", cy: "126", r: "20", fill: "#b53f3f", stroke: "#ffffff", "stroke-width": "4"
		}));
		var label = svgNode(documentRef, "text", {
			x: "350", y: "132", fill: "#ffffff", "font-size": "15",
			"font-weight": "800", "text-anchor": "middle"
		});
		label.textContent = String(viewModel.passLane.targetNumber);
		svgEl.appendChild(label);
	}

	function renderReplay(documentRef, viewModel) {
		documentRef.getElementById("run-id").textContent = viewModel.runId;
		documentRef.getElementById("match-title").textContent = viewModel.title;
		documentRef.getElementById("scoreline").textContent = viewModel.scoreline;
		documentRef.getElementById("match-time").textContent = viewModel.matchTime;
		documentRef.getElementById("pass-lane-label").textContent = viewModel.passLane.label;
		renderMetrics(documentRef, documentRef.getElementById("metrics"), viewModel.metrics);
		renderTacticalSummary(documentRef, documentRef.getElementById("tactical-summary"), viewModel.tactical);
		renderField(documentRef, documentRef.getElementById("field-view"), viewModel);
	}

	async function loadJson(path) {
		var response = await fetch(path);
		if(!response.ok) {
			throw new Error("Could not load " + path);
		}
		return response.json();
	}

	async function init(documentRef) {
		var catalogPath = "../../fixtures/replays/index.json";
		var catalog = await loadJson(catalogPath);
		var entries = catalog.replays;
		var summaries = {};

		async function selectReplay(id) {
			var entry = entries.find(function(item) { return item.id === id; });
			if(!entry) {
				return;
			}
			if(!summaries[id]) {
				var summaryPath = "../../fixtures/replays/" + summaryPathFromManifest(entry.manifest);
				summaries[id] = await loadJson(summaryPath);
			}
			var viewModel = buildReplayViewModel(entry, summaries[id]);
			renderCatalog(documentRef, documentRef.getElementById("catalog"), entries, id, selectReplay);
			renderReplay(documentRef, viewModel);
		}

		renderCatalog(documentRef, documentRef.getElementById("catalog"), entries, "", selectReplay);
		if(entries.length > 0) {
			await selectReplay(entries[0].id);
		}
	}

	if(typeof document !== "undefined") {
		document.addEventListener("DOMContentLoaded", function() {
			init(document).catch(function(error) {
				var catalog = document.getElementById("catalog");
				if(catalog) {
					catalog.innerHTML = "";
					catalog.appendChild(createEl(document, "p", "empty-state", error.message));
				}
			});
		});
	}

	return {
		buildReplayViewModel: buildReplayViewModel,
		formatNumber: formatNumber,
		summaryPathFromManifest: summaryPathFromManifest
	};
}));
