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

	function directoryName(path) {
		var lastSlash = path.lastIndexOf("/");
		return lastSlash >= 0 ? path.slice(0, lastSlash + 1) : "";
	}

	function joinPath(base, leaf) {
		return base + leaf;
	}

	function fixturePath(path) {
		return "../../fixtures/replays/" + path;
	}

	function parseCsv(text) {
		var lines = text.trim().split(/\r?\n/);
		if(lines.length < 2) {
			return [];
		}

		var headers = lines[0].split(",");
		return lines.slice(1).filter(function(line) {
			return line.trim() !== "";
		}).map(function(line) {
			var values = line.split(",");
			var row = {};
			headers.forEach(function(header, index) {
				row[header] = values[index] !== undefined ? values[index] : "";
			});
			return row;
		});
	}

	function asNumber(value) {
		var parsed = Number(value);
		return Number.isFinite(parsed) ? parsed : 0;
	}

	function latestPassLaneOptions(rows) {
		if(rows.length === 0) {
			return [];
		}

		var latestTick = rows.reduce(function(best, row) {
			return Math.max(best, asNumber(row.tick));
		}, 0);

		return rows.filter(function(row) {
			return asNumber(row.tick) === latestTick;
		}).sort(function(a, b) {
			return asNumber(a.rank) - asNumber(b.rank);
		});
	}

	function latestPressureFrame(rows) {
		if(rows.length === 0) {
			return null;
		}

		return rows.reduce(function(best, row) {
			return asNumber(row.tick) >= asNumber(best.tick) ? row : best;
		}, rows[0]);
	}

	function eventTitle(type) {
		if(type === "pass_completed") {
			return "Pass completed";
		}
		if(type === "possession_change") {
			return "Possession change";
		}
		if(type === "shot") {
			return "Shot";
		}
		return type;
	}

	function buildTimelineItems(derivedRows, shotRows) {
		var items = [];
		(derivedRows || []).forEach(function(row) {
			var type = row.event_type;
			var fromLabel = row.from_name + " #" + row.from_number;
			var toLabel = row.to_name + " #" + row.to_number;
			items.push({
				type: type,
				time: asNumber(row.match_time),
				title: eventTitle(type),
				team: type === "possession_change" ? row.to_team : row.from_team,
				detail: fromLabel + " to " + toLabel,
				value: type === "pass_completed" ?
					formatNumber(asNumber(row.pass_distance), 2) :
					row.to_team
			});
		});
		(shotRows || []).forEach(function(row) {
			items.push({
				type: "shot",
				time: asNumber(row.match_time),
				title: "Shot",
				team: row.team,
				detail: row.shooter_name + " #" + row.shooter_number,
				value: formatNumber(asNumber(row.shot_speed), 2)
			});
		});

		return items.sort(function(a, b) {
			if(a.time === b.time) {
				return a.title.localeCompare(b.title);
			}
			return a.time - b.time;
		});
	}

	function buildReplayViewModel(entry,
				      manifest,
				      summary,
				      passLaneRows,
				      pressureRows,
				      derivedRows,
				      shotRows) {
		var latestOptions = latestPassLaneOptions(passLaneRows || []);
		var latestPressure = latestPressureFrame(pressureRows || []);
		var timeline = buildTimelineItems(derivedRows || [], shotRows || []);
		var bestPassLane = latestOptions[0] || null;
		var bestTargetNumber = bestPassLane ?
			asNumber(bestPassLane.target_number) :
			summary.pass_lanes.last_best_target_number;
		var bestScore = bestPassLane ?
			asNumber(bestPassLane.score) :
			summary.pass_lanes.last_best_score;
		var bestDistance = bestPassLane ?
			asNumber(bestPassLane.pass_distance) :
			summary.pass_lanes.last_best_pass_distance;
		var pressureDistance = latestPressure ?
			asNumber(latestPressure.pressure_distance) :
			summary.pressure.last_distance;
		var highPressure = latestPressure ?
			asNumber(latestPressure.high_pressure) === 1 :
			false;

		return {
			id: entry.id,
			title: entry.title,
			description: entry.description,
			runId: summary.run_id,
			manifestPath: entry.manifest,
			sampleStride: manifest.sample_stride,
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
					detail: "latest " + formatNumber(pressureDistance, 2) +
						(highPressure ? ", high" : ", stable")
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
				["Sample stride", String(manifest.sample_stride)],
				["Longest pass", formatNumber(summary.passes.longest_distance, 2)],
				["Latest pressure", formatNumber(pressureDistance, 2)],
				["Pass options", String(summary.pass_lanes.options)],
				["Latest options", String(latestOptions.length)],
				["Best target", "#" + bestTargetNumber],
				["Best score", formatNumber(bestScore, 2)],
				["Pass distance", formatNumber(bestDistance, 2)]
			],
			passLane: {
				label: "#" + bestTargetNumber + " score " + formatNumber(bestScore, 2),
				targetNumber: bestTargetNumber,
				distance: bestDistance,
				options: latestOptions.map(function(row) {
					return {
						rank: asNumber(row.rank),
						carrierNumber: asNumber(row.carrier_number),
						carrierName: row.carrier_name,
						targetNumber: asNumber(row.target_number),
						targetName: row.target_name,
						score: asNumber(row.score),
						passDistance: asNumber(row.pass_distance),
						targetPressure: asNumber(row.target_pressure),
						carrierX: asNumber(row.carrier_x),
						carrierZ: asNumber(row.carrier_z),
						targetX: asNumber(row.target_x),
						targetZ: asNumber(row.target_z),
						ballX: asNumber(row.ball_x),
						ballZ: asNumber(row.ball_z)
					};
				})
			},
			pressure: {
				label: latestPressure ?
					latestPressure.carrier_name + " vs " + latestPressure.opponent_name :
					"No pressure stream",
				distance: pressureDistance,
				high: highPressure,
				carrierNumber: latestPressure ? asNumber(latestPressure.carrier_number) : 0,
				carrierName: latestPressure ? latestPressure.carrier_name : "",
				opponentNumber: latestPressure ? asNumber(latestPressure.opponent_number) : 0,
				opponentName: latestPressure ? latestPressure.opponent_name : "",
				carrierX: latestPressure ? asNumber(latestPressure.carrier_x) : 0,
				carrierZ: latestPressure ? asNumber(latestPressure.carrier_z) : 0,
				opponentX: latestPressure ? asNumber(latestPressure.opponent_x) : 0,
				opponentZ: latestPressure ? asNumber(latestPressure.opponent_z) : 0
			},
			timeline: timeline
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

	function renderTimeline(documentRef, timelineEl, items) {
		timelineEl.innerHTML = "";
		items.forEach(function(item) {
			var row = createEl(documentRef, "li", "timeline-item " + item.type, "");
			row.appendChild(createEl(documentRef, "span", "timeline-time", formatNumber(item.time, 2) + "s"));
			var body = createEl(documentRef, "span", "timeline-body", "");
			body.appendChild(createEl(documentRef, "strong", "", item.title));
			body.appendChild(createEl(documentRef, "span", "", item.detail));
			row.appendChild(body);
			row.appendChild(createEl(documentRef, "span", "timeline-value", item.value));
			timelineEl.appendChild(row);
		});
	}

	function svgNode(documentRef, tag, attrs) {
		var node = documentRef.createElementNS("http://www.w3.org/2000/svg", tag);
		Object.keys(attrs).forEach(function(key) {
			node.setAttribute(key, attrs[key]);
		});
		return node;
	}

	function fieldPoint(x, z) {
		return {
			x: 22 + ((x + 45) / 90) * 496,
			y: 338 - ((z + 60) / 120) * 316
		};
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

		if(viewModel.pressure.opponentNumber !== 0) {
			var pressureCarrier = fieldPoint(viewModel.pressure.carrierX, viewModel.pressure.carrierZ);
			var pressureOpponent = fieldPoint(viewModel.pressure.opponentX, viewModel.pressure.opponentZ);
			svgEl.appendChild(svgNode(documentRef, "line", {
				x1: String(pressureCarrier.x), y1: String(pressureCarrier.y),
				x2: String(pressureOpponent.x), y2: String(pressureOpponent.y),
				stroke: viewModel.pressure.high ? "#d9483b" : "#f08a3e",
				"stroke-width": viewModel.pressure.high ? "5" : "4",
				"stroke-dasharray": "8 7",
				"stroke-linecap": "round",
				opacity: "0.85"
			}));
			svgEl.appendChild(svgNode(documentRef, "circle", {
				cx: String(pressureOpponent.x), cy: String(pressureOpponent.y), r: "14",
				fill: "#b53f3f", stroke: "#f08a3e", "stroke-width": "4"
			}));
		}

		viewModel.passLane.options.forEach(function(option) {
			var carrier = fieldPoint(option.carrierX, option.carrierZ);
			var target = fieldPoint(option.targetX, option.targetZ);
			svgEl.appendChild(svgNode(documentRef, "line", {
				x1: String(carrier.x), y1: String(carrier.y),
				x2: String(target.x), y2: String(target.y),
				stroke: option.rank === 1 ? "#f5d06d" : "#dce6d4",
				"stroke-width": option.rank === 1 ? "6" : "3",
				"stroke-linecap": "round",
				opacity: option.rank === 1 ? "1" : "0.65"
			}));
		});

		if(viewModel.passLane.options.length > 0) {
			var best = viewModel.passLane.options[0];
			var carrierPoint = fieldPoint(best.carrierX, best.carrierZ);
			var targetPoint = fieldPoint(best.targetX, best.targetZ);
			var ballPoint = fieldPoint(best.ballX, best.ballZ);
			svgEl.appendChild(svgNode(documentRef, "circle", {
				cx: String(ballPoint.x), cy: String(ballPoint.y), r: "8",
				fill: "#ffffff", stroke: "#172019", "stroke-width": "3"
			}));
			svgEl.appendChild(svgNode(documentRef, "circle", {
				cx: String(carrierPoint.x), cy: String(carrierPoint.y), r: "17",
				fill: "#1f5f99", stroke: "#ffffff", "stroke-width": "4"
			}));
			svgEl.appendChild(svgNode(documentRef, "circle", {
				cx: String(targetPoint.x), cy: String(targetPoint.y), r: "20",
				fill: "#b53f3f", stroke: "#ffffff", "stroke-width": "4"
			}));
			var label = svgNode(documentRef, "text", {
				x: String(targetPoint.x), y: String(targetPoint.y + 5), fill: "#ffffff",
				"font-size": "15", "font-weight": "800", "text-anchor": "middle"
			});
			label.textContent = String(best.targetNumber);
			svgEl.appendChild(label);
		}
	}

	function renderReplay(documentRef, viewModel) {
		documentRef.getElementById("run-id").textContent = viewModel.runId;
		documentRef.getElementById("match-title").textContent = viewModel.title;
		documentRef.getElementById("scoreline").textContent = viewModel.scoreline;
		documentRef.getElementById("match-time").textContent = viewModel.matchTime;
		documentRef.getElementById("pass-lane-label").textContent = viewModel.passLane.label;
		documentRef.getElementById("timeline-count").textContent = viewModel.timeline.length + " events";
		renderMetrics(documentRef, documentRef.getElementById("metrics"), viewModel.metrics);
		renderTacticalSummary(documentRef, documentRef.getElementById("tactical-summary"), viewModel.tactical);
		renderTimeline(documentRef, documentRef.getElementById("match-timeline"), viewModel.timeline);
		renderField(documentRef, documentRef.getElementById("field-view"), viewModel);
	}

	async function loadJson(path) {
		var response = await fetch(path);
		if(!response.ok) {
			throw new Error("Could not load " + path);
		}
		return response.json();
	}

	async function loadText(path) {
		var response = await fetch(path);
		if(!response.ok) {
			throw new Error("Could not load " + path);
		}
		return response.text();
	}

	async function init(documentRef) {
		var catalogPath = "../../fixtures/replays/index.json";
		var catalog = await loadJson(catalogPath);
		var entries = catalog.replays;
		var replayData = {};

		async function selectReplay(id) {
			var entry = entries.find(function(item) { return item.id === id; });
			if(!entry) {
				return;
			}
			if(!replayData[id]) {
				var manifest = await loadJson(fixturePath(entry.manifest));
				var manifestBase = directoryName(entry.manifest);
				var summary = await loadJson(fixturePath(joinPath(manifestBase, manifest.files.summary)));
				var passLaneText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.pass_lanes)));
				var pressureText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.pressure)));
				var derivedText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.derived_events)));
				var shotText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.shots)));
				replayData[id] = {
					manifest: manifest,
					summary: summary,
					passLaneRows: parseCsv(passLaneText),
					pressureRows: parseCsv(pressureText),
					derivedRows: parseCsv(derivedText),
					shotRows: parseCsv(shotText)
				};
			}
			var data = replayData[id];
			var viewModel = buildReplayViewModel(entry,
				data.manifest,
				data.summary,
				data.passLaneRows,
				data.pressureRows,
				data.derivedRows,
				data.shotRows);
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
		directoryName: directoryName,
		fieldPoint: fieldPoint,
		fixturePath: fixturePath,
		formatNumber: formatNumber,
		buildTimelineItems: buildTimelineItems,
		joinPath: joinPath,
		latestPassLaneOptions: latestPassLaneOptions,
		latestPressureFrame: latestPressureFrame,
		parseCsv: parseCsv
	};
}));
