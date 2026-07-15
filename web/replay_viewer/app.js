(function(root, factory) {
	if(typeof module === "object" && module.exports) {
		module.exports = factory();
	} else {
		root.ReplayViewer = factory();
	}
}(typeof self !== "undefined" ? self : this, function() {
	"use strict";

	var FOCUS_VIEWS = ["field", "timeline", "heatmap"];
	var FIELD_LAYERS = ["passLanes", "pressure", "shotPaths"];
	var FIELD_MIN_X = -45;
	var FIELD_MAX_X = 45;

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
		if(type === "player_ball") {
			return "Player-ball collision";
		}
		if(type === "player_player") {
			return "Player collision";
		}
		return type;
	}

	function rowValue(row, key) {
		return row[key] === undefined || row[key] === "" ? "-" : row[key];
	}

	function eventRawRows(row, keys) {
		return keys.map(function(key) {
			return [key, rowValue(row, key)];
		});
	}

	function latestPressureForShot(pressureRows, item) {
		var latest = null;
		(pressureRows || []).forEach(function(row) {
			var tick = asNumber(row.tick);
			if(row.possession_team !== item.team || tick > item.tick) {
				return;
			}
			if(!latest || tick > latest.tick) {
				latest = {
					tick: tick,
					time: asNumber(row.match_time),
					team: row.possession_team,
					carrierNumber: asNumber(row.carrier_number),
					carrierName: row.carrier_name,
					opponentTeam: row.opponent_team,
					opponentNumber: asNumber(row.opponent_number),
					opponentName: row.opponent_name,
					distance: asNumber(row.pressure_distance),
					high: asNumber(row.high_pressure) === 1
				};
			}
		});
		return latest;
	}

	function shotQualityLabel(score) {
		if(score >= 80) {
			return "Excellent";
		}
		if(score >= 60) {
			return "Promising";
		}
		if(score >= 40) {
			return "Difficult";
		}
		return "Low";
	}

	function buildShotQuality(item) {
		if(!item || item.type !== "shot" || !item.shot) {
			return null;
		}
		var overlay = selectedShotOverlay(item);
		var distanceScore = clampNumber(100 - (item.shot.goalDistance * 1.25), 0, 100);
		var angleDegrees = overlay ?
			Math.atan2(Math.abs(overlay.targetX), Math.max(1, Math.abs(item.shot.goalDistance))) * (180 / Math.PI) :
			90;
		var angleScore = clampNumber(100 - (angleDegrees * 5), 0, 100);
		var speedScore = clampNumber((item.shot.forwardSpeed / 18) * 100, 0, 100);
		var pressureScore = item.shotPressure ?
			clampNumber((item.shotPressure.distance / 16) * 100, 0, 100) :
			50;
		if(item.shotPressure && item.shotPressure.high) {
			pressureScore = clampNumber(pressureScore - 18, 0, 100);
		}
		var score = Math.round(
			(distanceScore * 0.30) +
			(angleScore * 0.25) +
			(speedScore * 0.20) +
			(pressureScore * 0.25));
		return {
			score: score,
			label: shotQualityLabel(score),
			distanceScore: distanceScore,
			angleDegrees: angleDegrees,
			angleScore: angleScore,
			speedScore: speedScore,
			pressureScore: pressureScore
		};
	}

	function buildTimelineItems(derivedRows, shotRows, collisionRows, pressureRows) {
		var items = [];
		(derivedRows || []).forEach(function(row, index) {
			var type = row.event_type;
			var fromLabel = row.from_name + " #" + row.from_number;
			var toLabel = row.to_name + " #" + row.to_number;
			var team = type === "possession_change" ? row.to_team : row.from_team;
			items.push({
				id: type + "-" + row.tick + "-" + index,
				type: type,
				tick: asNumber(row.tick),
				time: asNumber(row.match_time),
				title: eventTitle(type),
				team: team,
				teams: [team],
				detail: fromLabel + " to " + toLabel,
				value: type === "pass_completed" ?
					formatNumber(asNumber(row.pass_distance), 2) :
					row.to_team,
				raw: eventRawRows(row, [
					"event_type", "tick", "match_time", "from_team", "from_number",
					"from_name", "to_team", "to_number", "to_name", "pass_distance",
					"ball_x", "ball_y", "ball_z"
				]),
				field: {
					x: asNumber(row.ball_x),
					z: asNumber(row.ball_z),
					label: type === "pass_completed" ? "pass" : "change"
				}
			});
		});
		(shotRows || []).forEach(function(row, index) {
			var shot = {
				team: row.team,
				shooterNumber: asNumber(row.shooter_number),
				shooterName: row.shooter_name,
				shotSpeed: asNumber(row.shot_speed),
				forwardSpeed: asNumber(row.forward_speed),
				goalDistance: asNumber(row.goal_distance),
				targetGoalZ: asNumber(row.target_goal_z),
				shooterX: asNumber(row.shooter_x),
				shooterZ: asNumber(row.shooter_z),
				ballX: asNumber(row.ball_x),
				ballY: asNumber(row.ball_y),
				ballZ: asNumber(row.ball_z),
				ballVx: asNumber(row.ball_vx),
				ballVy: asNumber(row.ball_vy),
				ballVz: asNumber(row.ball_vz)
			};
			var item = {
				id: "shot-" + row.tick + "-" + index,
				type: "shot",
				tick: asNumber(row.tick),
				time: asNumber(row.match_time),
				title: "Shot",
				team: row.team,
				teams: [row.team],
				detail: row.shooter_name + " #" + row.shooter_number,
				value: formatNumber(asNumber(row.shot_speed), 2),
				raw: [["event_type", "shot"]].concat(eventRawRows(row, [
					"tick", "match_time", "team", "shooter_number",
					"shooter_name", "shot_speed", "forward_speed", "goal_distance",
					"target_goal_z", "shooter_x", "shooter_z", "ball_x", "ball_y",
					"ball_z", "ball_vx", "ball_vy", "ball_vz"
				])),
				shot: shot,
				field: {
					x: shot.ballX,
					z: shot.ballZ,
					fromX: shot.shooterX,
					fromZ: shot.shooterZ,
					label: "shot"
				}
			};
			item.shotPressure = latestPressureForShot(pressureRows || [], item);
			item.shotQuality = buildShotQuality(item);
			items.push(item);
		});
		(collisionRows || []).forEach(function(row, index) {
			var type = row.event_type;
			var teamB = row.team_b || "";
			var participantA = row.name_a + " #" + row.number_a;
			var participantB = row.name_b === "Ball" ? "Ball" : row.name_b + " #" + row.number_b;
			var teams = [row.team_a];
			if(teamB && teams.indexOf(teamB) === -1) {
				teams.push(teamB);
			}
			items.push({
				id: type + "-" + row.tick + "-" + index,
				type: type,
				tick: asNumber(row.tick),
				time: asNumber(row.match_time),
				title: eventTitle(type),
				team: row.team_a,
				teams: teams,
				detail: participantA + " with " + participantB,
				value: formatNumber(asNumber(row.relative_speed), 2),
				raw: eventRawRows(row, [
					"event_type", "tick", "match_time", "team_a", "number_a",
					"name_a", "team_b", "number_b", "name_b", "distance",
					"relative_speed", "a_x", "a_z", "b_x", "b_z"
				]),
				field: {
					x: asNumber(row.a_x),
					z: asNumber(row.a_z),
					fromX: asNumber(row.b_x),
					fromZ: asNumber(row.b_z),
					label: "collision"
				}
			});
		});

		return items.sort(function(a, b) {
			if(a.time === b.time) {
				return a.title.localeCompare(b.title);
			}
			return a.time - b.time;
		});
	}

	function selectedEventDetails(item) {
		if(!item) {
			return [];
		}
		return [
			["title", item.title],
			["team", item.team],
			["detail", item.detail],
			["display_value", item.value]
		].concat(item.raw || []);
	}

	function fieldPosition(x, z) {
		return formatNumber(x, 2) + ", " + formatNumber(z, 2);
	}

	function fieldVelocity(x, y, z) {
		return formatNumber(x, 2) + ", " + formatNumber(y, 2) + ", " + formatNumber(z, 2);
	}

	function selectedShotDetails(item) {
		if(!item || item.type !== "shot" || !item.shot) {
			return [];
		}
		var rows = [
			["Shooter", "#" + item.shot.shooterNumber + " " + item.shot.shooterName],
			["Team", item.shot.team],
			["Shot speed", formatNumber(item.shot.shotSpeed, 2)],
			["Forward speed", formatNumber(item.shot.forwardSpeed, 2)],
			["Goal distance", formatNumber(item.shot.goalDistance, 2)],
			["Target goal", formatNumber(item.shot.targetGoalZ, 0)],
			["Shooter pos", fieldPosition(item.shot.shooterX, item.shot.shooterZ)],
			["Ball pos", fieldPosition(item.shot.ballX, item.shot.ballZ)],
			["Ball velocity", fieldVelocity(item.shot.ballVx, item.shot.ballVy, item.shot.ballVz)]
		];
		if(item.shotQuality) {
			rows.push(["Quality", item.shotQuality.label + " " + item.shotQuality.score]);
			rows.push([
				"Distance hint",
				formatNumber(item.shot.goalDistance, 2) + " units, score " +
					formatNumber(item.shotQuality.distanceScore, 2)
			]);
			rows.push([
				"Angle hint",
				formatNumber(item.shotQuality.angleDegrees, 2) + " deg, score " +
					formatNumber(item.shotQuality.angleScore, 2)
			]);
			rows.push([
				"Speed hint",
				formatNumber(item.shot.forwardSpeed, 2) + " fwd, score " +
					formatNumber(item.shotQuality.speedScore, 2)
			]);
			rows.push([
				"Pressure hint",
				item.shotPressure ?
					"#" + item.shotPressure.carrierNumber + " " + item.shotPressure.carrierName +
						" " + formatNumber(item.shotPressure.distance, 2) +
						(item.shotPressure.high ? " high" : " stable") +
						", score " + formatNumber(item.shotQuality.pressureScore, 2) :
					"No pressure sample, score " + formatNumber(item.shotQuality.pressureScore, 2)
			]);
		}
		return rows;
	}

	function clampNumber(value, minimum, maximum) {
		return Math.max(minimum, Math.min(maximum, value));
	}

	function selectedShotOverlay(item) {
		if(!item || item.type !== "shot" || !item.shot) {
			return null;
		}
		var targetX = item.shot.ballX;
		var targetZ = item.shot.targetGoalZ;
		if(Number.isFinite(item.shot.ballVz) && Math.abs(item.shot.ballVz) > 0.0001) {
			var travelTime = (targetZ - item.shot.ballZ) / item.shot.ballVz;
			if(Number.isFinite(travelTime) && travelTime >= 0) {
				targetX = item.shot.ballX + item.shot.ballVx * travelTime;
			}
		}
		var clampedTargetX = clampNumber(targetX, FIELD_MIN_X, FIELD_MAX_X);
		var dx = clampedTargetX - item.shot.ballX;
		var dz = targetZ - item.shot.ballZ;
		return {
			shooterX: item.shot.shooterX,
			shooterZ: item.shot.shooterZ,
			ballX: item.shot.ballX,
			ballZ: item.shot.ballZ,
			targetX: clampedTargetX,
			projectedTargetX: targetX,
			targetZ: targetZ,
			pathDistance: Math.sqrt((dx * dx) + (dz * dz)),
			label: "Target goal " + formatNumber(targetZ, 0)
		};
	}

	function timelineSearchText(item) {
		var parts = [
			item.title,
			item.team,
			item.detail,
			item.value,
			"#" + item.detail.replace(/[^0-9# ]/g, " ")
		];
		(item.raw || []).forEach(function(row) {
			parts.push(row[0]);
			parts.push(row[1]);
			if(/number$/.test(row[0])) {
				parts.push("#" + row[1]);
			}
		});
		return parts.join(" ").toLowerCase();
	}

	function filterTimelineItems(items, filters) {
		var activeFilters = filters || {};
		var typeState = activeFilters.types || activeFilters;
		var team = activeFilters.team || "all";
		var query = (activeFilters.query || "").trim().toLowerCase();
		var hasEnabledType = Object.keys(typeState).some(function(type) {
			return typeState[type];
		});
		if(!hasEnabledType) {
			return [];
		}
		return (items || []).filter(function(item) {
			var typeMatch = item.type === "player_ball" || item.type === "player_player" ?
				typeState.collision !== false :
				typeState[item.type] !== false;
			var itemTeams = item.teams || [item.team];
			var teamMatch = team === "all" || itemTeams.indexOf(team) !== -1;
			var searchMatch = !query || timelineSearchText(item).indexOf(query) !== -1;
			return typeMatch && teamMatch && searchMatch;
		});
	}

	function repairedTimelineSelection(items, selectedId) {
		if(!items || items.length === 0) {
			return "";
		}
		for(var i = 0; i < items.length; i++) {
			if(items[i].id === selectedId) {
				return selectedId;
			}
		}
		return items[0].id;
	}

	function timelineTeamCounts(items, teams) {
		var counts = {};
		(teams || []).forEach(function(team) {
			counts[team] = {
				team: team,
				total: 0,
				passCompleted: 0,
				shots: 0,
				possessionChanges: 0,
				collisions: 0
			};
		});
		(items || []).forEach(function(item) {
			var itemTeams = item.teams || [item.team];
			itemTeams.forEach(function(team) {
				if(!team) {
					return;
				}
				if(!counts[team]) {
					counts[team] = {
						team: team,
						total: 0,
						passCompleted: 0,
						shots: 0,
						possessionChanges: 0,
						collisions: 0
					};
				}
				counts[team].total += 1;
				if(item.type === "pass_completed") {
					counts[team].passCompleted += 1;
				}
				if(item.type === "shot") {
					counts[team].shots += 1;
				}
				if(item.type === "possession_change") {
					counts[team].possessionChanges += 1;
				}
				if(item.type === "player_ball" || item.type === "player_player") {
					counts[team].collisions += 1;
				}
			});
		});
		return Object.keys(counts).map(function(team) {
			return counts[team];
		});
	}

	function timelineNavigationTarget(items, selectedId, key) {
		if(!items || items.length === 0) {
			return "";
		}
		var index = 0;
		for(var i = 0; i < items.length; i++) {
			if(items[i].id === selectedId) {
				index = i;
				break;
			}
		}
		if(key === "ArrowLeft" || key === "ArrowUp") {
			return items[Math.max(0, index - 1)].id;
		}
		if(key === "ArrowRight" || key === "ArrowDown") {
			return items[Math.min(items.length - 1, index + 1)].id;
		}
		if(key === "Home") {
			return items[0].id;
		}
		if(key === "End") {
			return items[items.length - 1].id;
		}
		return "";
	}

	function selectedTimelineItem(items, selectedId) {
		for(var i = 0; i < items.length; i++) {
			if(items[i].id === selectedId) {
				return items[i];
			}
		}
		return items.length > 0 ? items[0] : null;
	}

	function heatmapColor(cell, teams) {
		if(cell.entityType === "ball") {
			return "#ffffff";
		}
		if(cell.team === teams[0]) {
			return "#1f5f99";
		}
		if(cell.team === teams[1]) {
			return "#b53f3f";
		}
		return "#f5d06d";
	}

	function buildHeatmap(rows, teams) {
		var cells = (rows || []).map(function(row) {
			return {
				entityType: row.entity_type,
				team: row.team,
				number: asNumber(row.number),
				name: row.name,
				column: asNumber(row.column),
				row: asNumber(row.row),
				samples: asNumber(row.samples)
			};
		});
		var maxSamples = cells.reduce(function(best, cell) {
			return Math.max(best, cell.samples);
		}, 0);
		var totalSamples = cells.reduce(function(total, cell) {
			return total + cell.samples;
		}, 0);

		return {
			columns: 18,
			rows: 24,
			cells: cells.map(function(cell) {
				return {
					entityType: cell.entityType,
					team: cell.team,
					number: cell.number,
					name: cell.name,
					column: cell.column,
					row: cell.row,
					samples: cell.samples,
					color: heatmapColor(cell, teams),
					intensity: maxSamples > 0 ? cell.samples / maxSamples : 0
				};
			}),
			maxSamples: maxSamples,
			totalSamples: totalSamples
		};
	}

	function heatmapPlayerKey(cell) {
		if(!cell || cell.entityType !== "player") {
			return "";
		}
		return cell.team + "|" + cell.number + "|" + cell.name;
	}

	function filterHeatmap(heatmap, filters) {
		var activeFilters = filters || {};
		var team = activeFilters.team || "all";
		var entityType = activeFilters.entityType || "all";
		var player = activeFilters.player || "all";
		var cells = heatmap.cells.filter(function(cell) {
			var teamMatch = team === "all" || cell.team === team;
			var entityMatch = entityType === "all" || cell.entityType === entityType;
			var playerMatch = player === "all" || heatmapPlayerKey(cell) === player;
			return teamMatch && entityMatch && playerMatch;
		});
		var maxSamples = cells.reduce(function(best, cell) {
			return Math.max(best, cell.samples);
		}, 0);
		var totalSamples = cells.reduce(function(total, cell) {
			return total + cell.samples;
		}, 0);
		return {
			columns: heatmap.columns,
			rows: heatmap.rows,
			cells: cells.map(function(cell) {
				return {
					entityType: cell.entityType,
					team: cell.team,
					number: cell.number,
					name: cell.name,
					column: cell.column,
					row: cell.row,
					samples: cell.samples,
					color: cell.color,
					intensity: maxSamples > 0 ? cell.samples / maxSamples : 0
				};
			}),
			maxSamples: maxSamples,
			totalSamples: totalSamples
		};
	}

	function heatmapFilterState(documentRef) {
		var team = documentRef.getElementById("heatmap-team-filter");
		var entity = documentRef.getElementById("heatmap-entity-filter");
		var player = documentRef.getElementById("heatmap-player-filter");
		return {
			team: team && team.value ? team.value : "all",
			entityType: entity && entity.value ? entity.value : "all",
			player: player && player.value ? player.value : "all"
		};
	}

	function buildMovementMetrics(rows) {
		return (rows || []).map(function(row) {
			var metric = {
				entityType: row.entity_type,
				team: row.team,
				number: asNumber(row.number),
				name: row.name,
				samples: asNumber(row.samples),
				distance: asNumber(row.distance),
				averageSpeed: asNumber(row.average_speed),
				maxSpeed: asNumber(row.max_speed)
			};
			metric.playerKey = heatmapPlayerKey(metric);
			return metric;
		});
	}

	function movementLabel(filters) {
		if(filters.player && filters.player !== "all") {
			var parts = filters.player.split("|");
			return "#" + parts[1] + " " + parts[2] + " (" + parts[0] + ")";
		}
		if(filters.entityType === "ball") {
			return "Ball";
		}
		if(filters.entityType === "player" && filters.team !== "all") {
			return filters.team + " players";
		}
		if(filters.entityType === "player") {
			return "All players";
		}
		if(filters.team !== "all") {
			return filters.team;
		}
		return "All movement";
	}

	function summarizeMovementMetrics(metrics, filters) {
		var activeFilters = filters || {};
		var team = activeFilters.team || "all";
		var entityType = activeFilters.entityType || "all";
		var player = activeFilters.player || "all";
		var rows = (metrics || []).filter(function(metric) {
			var teamMatch = team === "all" || metric.team === team;
			var entityMatch = entityType === "all" || metric.entityType === entityType;
			var playerMatch = player === "all" || metric.playerKey === player;
			return teamMatch && entityMatch && playerMatch;
		});
		var samples = rows.reduce(function(total, row) {
			return total + row.samples;
		}, 0);
		var distance = rows.reduce(function(total, row) {
			return total + row.distance;
		}, 0);
		var weightedSpeed = rows.reduce(function(total, row) {
			return total + (row.averageSpeed * row.samples);
		}, 0);
		var maxSpeed = rows.reduce(function(best, row) {
			return Math.max(best, row.maxSpeed);
		}, 0);
		return {
			label: movementLabel({ team: team, entityType: entityType, player: player }),
			count: rows.length,
			rows: [
				["Entities", String(rows.length)],
				["Samples", String(samples)],
				["Distance", formatNumber(distance, 2)],
				["Average speed", formatNumber(samples > 0 ? weightedSpeed / samples : 0, 2)],
				["Max speed", formatNumber(maxSpeed, 2)]
			]
		};
	}

	function possessionZoneFor(team, z, teams) {
		var orientedZ = team === teams[1] ? -z : z;
		if(orientedZ < -20) {
			return "Defensive";
		}
		if(orientedZ > 20) {
			return "Attacking";
		}
		return "Middle";
	}

	function buildPossessionZones(rows, teams) {
		var summaries = {};
		(teams || []).forEach(function(team) {
			summaries[team] = {
				team: team,
				total: 0,
				zones: {
					Defensive: 0,
					Middle: 0,
					Attacking: 0
				},
				latestZone: "-"
			};
		});
		(rows || []).forEach(function(row) {
			if(row.entity_type !== "ball" || !row.team_in_possession) {
				return;
			}
			var team = row.team_in_possession;
			if(!summaries[team]) {
				summaries[team] = {
					team: team,
					total: 0,
					zones: {
						Defensive: 0,
						Middle: 0,
						Attacking: 0
					},
					latestZone: "-"
				};
			}
			var zone = possessionZoneFor(team, asNumber(row.z), teams || []);
			summaries[team].total += 1;
			summaries[team].zones[zone] += 1;
			summaries[team].latestZone = zone;
		});
		var teamSummaries = Object.keys(summaries).map(function(team) {
			var summary = summaries[team];
			var dominantZone = "No samples";
			var dominantSamples = 0;
			["Defensive", "Middle", "Attacking"].forEach(function(zone) {
				if(summary.zones[zone] > dominantSamples) {
					dominantZone = zone;
					dominantSamples = summary.zones[zone];
				}
			});
			summary.dominantZone = dominantZone;
			summary.dominantSamples = dominantSamples;
			summary.dominantPercent = summary.total > 0 ?
				Math.round((dominantSamples / summary.total) * 100) :
				0;
			return summary;
		});
		return {
			totalSamples: teamSummaries.reduce(function(total, summary) {
				return total + summary.total;
			}, 0),
			teams: teamSummaries
		};
	}

	function possessionZoneRows(possessionZones) {
		var rows = [
			["Ball samples", String(possessionZones.totalSamples)]
		];
		possessionZones.teams.forEach(function(summary) {
			rows.push([
				summary.team,
				summary.total > 0 ?
					summary.dominantZone + " " + summary.dominantPercent + "%" :
					"No samples"
			]);
			rows.push([
				summary.team + " latest",
				summary.latestZone
			]);
		});
		return rows;
	}

	function buildReplayViewModel(entry,
				      manifest,
				      summary,
				      passLaneRows,
				      pressureRows,
				      derivedRows,
				      shotRows,
				      collisionRows,
				      snapshotRows,
				      metricsRows,
				      heatmapRows) {
		var latestOptions = latestPassLaneOptions(passLaneRows || []);
		var latestPressure = latestPressureFrame(pressureRows || []);
		var timeline = buildTimelineItems(derivedRows || [], shotRows || [],
			collisionRows || [], pressureRows || []);
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
		var teams = [summary.teams.team01, summary.teams.team02];
		var heatmap = buildHeatmap(heatmapRows || [], teams);
		var movementMetrics = buildMovementMetrics(metricsRows || []);
		var possessionZones = buildPossessionZones(snapshotRows || [], teams);

		return {
			id: entry.id,
			title: entry.title,
			description: entry.description,
			runId: summary.run_id,
			manifestPath: entry.manifest,
			sampleStride: manifest.sample_stride,
			teams: teams,
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
						targetGoalDistance: asNumber(row.target_goal_distance),
						forwardProgress: asNumber(row.forward_progress),
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
			timeline: timeline,
			movementMetrics: movementMetrics,
			possessionZones: possessionZones,
			heatmap: heatmap
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

	function renderEventDetails(documentRef, detailEl, item) {
		detailEl.innerHTML = "";
		selectedEventDetails(item).forEach(function(row) {
			detailEl.appendChild(createEl(documentRef, "dt", "", row[0]));
			detailEl.appendChild(createEl(documentRef, "dd", "", row[1]));
		});
	}

	function renderShotDetails(documentRef, detailEl, item) {
		detailEl.innerHTML = "";
		var rows = selectedShotDetails(item);
		if(rows.length === 0) {
			detailEl.appendChild(createEl(documentRef, "dt", "", "Selection"));
			detailEl.appendChild(createEl(documentRef, "dd", "", "No shot selected"));
			return;
		}
		rows.forEach(function(row) {
			detailEl.appendChild(createEl(documentRef, "dt", "", row[0]));
			detailEl.appendChild(createEl(documentRef, "dd", "", row[1]));
		});
	}

	function renderTimeline(documentRef, timelineEl, items, selectedId, onSelect) {
		timelineEl.innerHTML = "";
		if(items.length === 0) {
			var empty = createEl(documentRef, "li", "timeline-empty", "No events match the current filters.");
			timelineEl.appendChild(empty);
			return;
		}
		items.forEach(function(item) {
			var row = createEl(documentRef, "li", "timeline-item " + item.type, "");
			var button = createEl(documentRef, "button", "timeline-button", "");
			button.type = "button";
			button.setAttribute("data-event-id", item.id);
			button.setAttribute("aria-pressed", item.id === selectedId ? "true" : "false");
			button.setAttribute("tabindex", item.id === selectedId ? "0" : "-1");
			if(item.id === selectedId) {
				button.classList.add("active");
				button.setAttribute("aria-current", "true");
			}
			button.appendChild(createEl(documentRef, "span", "timeline-time", formatNumber(item.time, 2) + "s"));
			var body = createEl(documentRef, "span", "timeline-body", "");
			body.appendChild(createEl(documentRef, "strong", "", item.title));
			body.appendChild(createEl(documentRef, "span", "", item.detail));
			button.appendChild(body);
			button.appendChild(createEl(documentRef, "span", "timeline-value", item.value));
			button.addEventListener("click", function() {
				onSelect(item.id);
			});
			button.addEventListener("keydown", function(event) {
				var nextId = timelineNavigationTarget(items, item.id, event.key);
				if(nextId) {
					event.preventDefault();
					if(nextId !== item.id) {
						onSelect(nextId, { focusTimeline: true, stayInView: true });
					}
				}
			});
			row.appendChild(button);
			timelineEl.appendChild(row);
		});
	}

	function renderTimelineSummary(documentRef, summaryEl, counts) {
		summaryEl.innerHTML = "";
		counts.forEach(function(count) {
			var item = createEl(documentRef, "article", "timeline-summary-item", "");
			item.appendChild(createEl(documentRef, "strong", "", count.team));
			item.appendChild(createEl(documentRef, "span", "", count.total + " events"));
			item.appendChild(createEl(documentRef, "small", "",
				count.passCompleted + " passes, " +
				count.shots + " shots, " +
				count.possessionChanges + " changes, " +
				count.collisions + " collisions"));
			summaryEl.appendChild(item);
		});
	}

	function renderPassLaneDetails(documentRef, detailsEl, passLane) {
		detailsEl.innerHTML = "";
		if(!passLane || passLane.options.length === 0) {
			detailsEl.appendChild(createEl(documentRef, "div", "pass-lane-empty", "No pass lanes available."));
			return;
		}
		passLane.options.forEach(function(option) {
			var row = createEl(documentRef, "div", "pass-lane-row", "");
			row.appendChild(createEl(documentRef, "span", "pass-lane-rank", "#" + option.rank));
			row.appendChild(createEl(documentRef, "span", "", "#" + option.targetNumber + " " + option.targetName));
			row.appendChild(createEl(documentRef, "span", "", formatNumber(option.score, 2)));
			row.appendChild(createEl(documentRef, "span", "", formatNumber(option.passDistance, 2)));
			row.appendChild(createEl(documentRef, "span", "", formatNumber(option.targetPressure, 2)));
			row.appendChild(createEl(documentRef, "span", "", formatNumber(option.targetGoalDistance, 2)));
			row.appendChild(createEl(documentRef, "span", "", formatNumber(option.forwardProgress, 2)));
			detailsEl.appendChild(row);
		});
	}

	function renderPressureDetails(documentRef, detailsEl, pressure) {
		detailsEl.innerHTML = "";
		if(!pressure || pressure.opponentNumber === 0) {
			detailsEl.appendChild(createEl(documentRef, "div", "pressure-empty", "No pressure sample available."));
			return;
		}
		var row = createEl(documentRef, "div", "pressure-row", "");
		row.appendChild(createEl(documentRef, "span", "", "#" + pressure.carrierNumber + " " + pressure.carrierName));
		row.appendChild(createEl(documentRef, "span", "", "#" + pressure.opponentNumber + " " + pressure.opponentName));
		row.appendChild(createEl(documentRef, "span", "", formatNumber(pressure.distance, 2)));
		row.appendChild(createEl(documentRef, "span", "", pressure.high ? "High" : "Stable"));
		row.appendChild(createEl(documentRef, "span", "", fieldPosition(pressure.carrierX, pressure.carrierZ)));
		row.appendChild(createEl(documentRef, "span", "", fieldPosition(pressure.opponentX, pressure.opponentZ)));
		detailsEl.appendChild(row);
	}

	function renderHeatmap(documentRef, svgEl, heatmap) {
		svgEl.innerHTML = "";
		svgEl.appendChild(svgNode(documentRef, "rect", {
			x: "0", y: "0", width: "360", height: "240", fill: "#2f7d4d"
		}));
		for(var column = 0; column < heatmap.columns; column++) {
			svgEl.appendChild(svgNode(documentRef, "line", {
				x1: String((column / heatmap.columns) * 360), y1: "0",
				x2: String((column / heatmap.columns) * 360), y2: "240",
				stroke: "#f4f7ee", "stroke-width": "0.6", opacity: "0.22"
			}));
		}
		for(var row = 0; row < heatmap.rows; row++) {
			svgEl.appendChild(svgNode(documentRef, "line", {
				x1: "0", y1: String((row / heatmap.rows) * 240),
				x2: "360", y2: String((row / heatmap.rows) * 240),
				stroke: "#f4f7ee", "stroke-width": "0.6", opacity: "0.22"
			}));
		}
		heatmap.cells.forEach(function(cell) {
			var width = 360 / heatmap.columns;
			var height = 240 / heatmap.rows;
			svgEl.appendChild(svgNode(documentRef, "rect", {
				x: String(cell.column * width),
				y: String(cell.row * height),
				width: String(width),
				height: String(height),
				fill: cell.color,
				opacity: String(0.28 + cell.intensity * 0.58)
			}));
		});
		svgEl.appendChild(svgNode(documentRef, "rect", {
			x: "1", y: "1", width: "358", height: "238",
			fill: "none", stroke: "#f4f7ee", "stroke-width": "2"
		}));
	}

	function selectHasValue(selectEl, value) {
		for(var i = 0; i < selectEl.options.length; i++) {
			if(selectEl.options[i].value === value) {
				return true;
			}
		}
		return false;
	}

	function setSelectOptions(documentRef, selectEl, options, selectedValue) {
		selectEl.innerHTML = "";
		options.forEach(function(option) {
			var item = createEl(documentRef, "option", "", option.label);
			item.value = option.value;
			selectEl.appendChild(item);
		});
		selectEl.value = selectHasValue(selectEl, selectedValue) ? selectedValue : "all";
	}

	function heatmapEntityLabel(entityType) {
		if(entityType === "player") {
			return "Players";
		}
		if(entityType === "ball") {
			return "Ball";
		}
		return entityType;
	}

	function heatmapPlayerLabel(cell, includeTeam) {
		var label = "#" + cell.number + " " + cell.name;
		return includeTeam ? label + " (" + cell.team + ")" : label;
	}

	function renderHeatmapFilters(documentRef, viewModel) {
		var teamSelect = documentRef.getElementById("heatmap-team-filter");
		var entitySelect = documentRef.getElementById("heatmap-entity-filter");
		var playerSelect = documentRef.getElementById("heatmap-player-filter");
		if(!teamSelect || !entitySelect || !playerSelect) {
			return;
		}

		var current = heatmapFilterState(documentRef);
		var teams = [{ value: "all", label: "All teams" }];
		viewModel.teams.forEach(function(team) {
			teams.push({ value: team, label: team });
		});

		var seenEntities = {};
		viewModel.heatmap.cells.forEach(function(cell) {
			if(cell.entityType) {
				seenEntities[cell.entityType] = true;
			}
		});
		var entityOrder = ["player", "ball"];
		var entities = [{ value: "all", label: "All entities" }];
		entityOrder.forEach(function(entityType) {
			if(seenEntities[entityType]) {
				entities.push({ value: entityType, label: heatmapEntityLabel(entityType) });
				delete seenEntities[entityType];
			}
		});
		Object.keys(seenEntities).sort().forEach(function(entityType) {
			entities.push({ value: entityType, label: heatmapEntityLabel(entityType) });
		});

		setSelectOptions(documentRef, teamSelect, teams, current.team);
		setSelectOptions(documentRef, entitySelect, entities, current.entityType);

		var updated = heatmapFilterState(documentRef);
		var includeTeam = updated.team === "all";
		var playerOptions = [{ value: "all", label: "All players" }];
		var seenPlayers = {};
		viewModel.heatmap.cells.forEach(function(cell) {
			var key = heatmapPlayerKey(cell);
			if(!key ||
			   seenPlayers[key] ||
			   (updated.team !== "all" && cell.team !== updated.team) ||
			   (updated.entityType !== "all" && updated.entityType !== "player")) {
				return;
			}
			seenPlayers[key] = true;
			playerOptions.push({
				value: key,
				label: heatmapPlayerLabel(cell, includeTeam)
			});
		});
		setSelectOptions(documentRef, playerSelect, playerOptions, current.player);
		playerSelect.disabled = playerOptions.length === 1;
	}

	function renderHeatmapSection(documentRef, viewModel) {
		renderHeatmapFilters(documentRef, viewModel);
		var filters = heatmapFilterState(documentRef);
		var filteredHeatmap = filterHeatmap(viewModel.heatmap, filters);
		var movement = summarizeMovementMetrics(viewModel.movementMetrics, filters);
		documentRef.getElementById("heatmap-label").textContent =
			filteredHeatmap.totalSamples + " samples, max " + filteredHeatmap.maxSamples;
		renderHeatmap(documentRef, documentRef.getElementById("heatmap-view"), filteredHeatmap);
		documentRef.getElementById("movement-label").textContent = movement.label;
		renderTacticalSummary(documentRef,
			documentRef.getElementById("movement-metrics"),
			movement.rows);
		documentRef.getElementById("possession-zone-label").textContent =
			viewModel.possessionZones.totalSamples + " ball samples";
		renderTacticalSummary(documentRef,
			documentRef.getElementById("possession-zones"),
			possessionZoneRows(viewModel.possessionZones));
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

	function fieldLayerState(documentRef) {
		var passLanes = documentRef.getElementById("toggle-pass-lanes");
		var pressure = documentRef.getElementById("toggle-pressure");
		var shotPaths = documentRef.getElementById("toggle-shot-paths");
		return {
			passLanes: !passLanes || passLanes.checked,
			pressure: !pressure || pressure.checked,
			shotPaths: !shotPaths || shotPaths.checked
		};
	}

	function timelineFilterState(documentRef) {
		var filters = documentRef.querySelectorAll(".timeline-filter");
		var team = documentRef.getElementById("timeline-team-filter");
		var search = documentRef.getElementById("timeline-search");
		var state = {
			team: team && team.value ? team.value : "all",
			query: search && search.value ? search.value : "",
			types: {}
		};
		for(var i = 0; i < filters.length; i++) {
			state.types[filters[i].value] = filters[i].checked;
		}
		return state;
	}

	function renderTimelineFilters(documentRef, viewModel) {
		var teamSelect = documentRef.getElementById("timeline-team-filter");
		if(!teamSelect) {
			return;
		}
		var currentTeam = teamSelect.value || "all";
		var teams = [{ value: "all", label: "All teams" }];
		viewModel.teams.forEach(function(team) {
			teams.push({ value: team, label: team });
		});
		setSelectOptions(documentRef, teamSelect, teams, currentTeam);
	}

	function renderField(documentRef, svgEl, viewModel, layers, selectedEvent) {
		var activeLayers = layers || fieldLayerState(documentRef);
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

		var shotOverlay = activeLayers.shotPaths ? selectedShotOverlay(selectedEvent) : null;
		if(shotOverlay) {
			var defs = svgNode(documentRef, "defs", {});
			var arrowMarker = svgNode(documentRef, "marker", {
				id: "shot-path-arrow",
				viewBox: "0 0 10 10",
				refX: "8",
				refY: "5",
				markerWidth: "7",
				markerHeight: "7",
				orient: "auto-start-reverse"
			});
			arrowMarker.appendChild(svgNode(documentRef, "path", {
				d: "M 0 0 L 10 5 L 0 10 z",
				fill: "#f5d06d"
			}));
			defs.appendChild(arrowMarker);
			svgEl.appendChild(defs);

			var goalLeft = fieldPoint(FIELD_MIN_X, shotOverlay.targetZ);
			var goalRight = fieldPoint(FIELD_MAX_X, shotOverlay.targetZ);
			var ballPoint = fieldPoint(shotOverlay.ballX, shotOverlay.ballZ);
			var targetPoint = fieldPoint(shotOverlay.targetX, shotOverlay.targetZ);
			svgEl.appendChild(svgNode(documentRef, "line", {
				x1: String(goalLeft.x), y1: String(goalLeft.y),
				x2: String(goalRight.x), y2: String(goalRight.y),
				stroke: "#f5d06d", "stroke-width": "5",
				"stroke-dasharray": "12 8", opacity: "0.82"
			}));
			svgEl.appendChild(svgNode(documentRef, "line", {
				x1: String(ballPoint.x), y1: String(ballPoint.y),
				x2: String(targetPoint.x), y2: String(targetPoint.y),
				stroke: "#f5d06d", "stroke-width": "6",
				"stroke-linecap": "round",
				"marker-end": "url(#shot-path-arrow)"
			}));
			svgEl.appendChild(svgNode(documentRef, "circle", {
				cx: String(targetPoint.x), cy: String(targetPoint.y), r: "15",
				fill: "#fff5cf", stroke: "#172019", "stroke-width": "4"
			}));
			var shotLabel = svgNode(documentRef, "text", {
				x: String(targetPoint.x), y: String(targetPoint.y + (shotOverlay.targetZ >= 0 ? 30 : -22)),
				fill: "#172019", "font-size": "13",
				"font-weight": "800", "text-anchor": "middle"
			});
			shotLabel.textContent = "shot path";
			svgEl.appendChild(shotLabel);
		}

		if(activeLayers.pressure && viewModel.pressure.opponentNumber !== 0) {
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

		if(activeLayers.passLanes) {
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
		}

		if(activeLayers.passLanes && viewModel.passLane.options.length > 0) {
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

		if(selectedEvent && selectedEvent.field) {
			var selectedPoint = fieldPoint(selectedEvent.field.x, selectedEvent.field.z);
			if(selectedEvent.field.fromX !== undefined) {
				var fromPoint = fieldPoint(selectedEvent.field.fromX, selectedEvent.field.fromZ);
				svgEl.appendChild(svgNode(documentRef, "line", {
					x1: String(fromPoint.x), y1: String(fromPoint.y),
					x2: String(selectedPoint.x), y2: String(selectedPoint.y),
					stroke: "#172019", "stroke-width": "4",
					"stroke-linecap": "round", opacity: "0.85"
				}));
			}
			svgEl.appendChild(svgNode(documentRef, "circle", {
				cx: String(selectedPoint.x), cy: String(selectedPoint.y), r: "23",
				fill: "none", stroke: "#172019", "stroke-width": "5"
			}));
			svgEl.appendChild(svgNode(documentRef, "circle", {
				cx: String(selectedPoint.x), cy: String(selectedPoint.y), r: "13",
				fill: "#f5d06d", stroke: "#ffffff", "stroke-width": "4"
			}));
			var marker = svgNode(documentRef, "text", {
				x: String(selectedPoint.x), y: String(selectedPoint.y - 30),
				fill: "#172019", "font-size": "14",
				"font-weight": "800", "text-anchor": "middle"
			});
			marker.textContent = selectedEvent.title;
			svgEl.appendChild(marker);
		}
	}

	function renderReplay(documentRef, viewModel, selectedEventId, onTimelineSelect) {
		renderTimelineFilters(documentRef, viewModel);
		var visibleTimeline = filterTimelineItems(viewModel.timeline, timelineFilterState(documentRef));
		var repairedEventId = repairedTimelineSelection(visibleTimeline, selectedEventId);
		var selectedEvent = selectedTimelineItem(visibleTimeline, repairedEventId);
		documentRef.getElementById("run-id").textContent = viewModel.runId;
		documentRef.getElementById("match-title").textContent = viewModel.title;
		documentRef.getElementById("scoreline").textContent = viewModel.scoreline;
		documentRef.getElementById("match-time").textContent = viewModel.matchTime;
		documentRef.getElementById("pass-lane-label").textContent = viewModel.passLane.label;
		documentRef.getElementById("shot-detail-label").textContent =
			selectedEvent && selectedEvent.type === "shot" ?
				selectedEvent.detail :
				"-";
		documentRef.getElementById("event-detail-time").textContent = selectedEvent ?
			formatNumber(selectedEvent.time, 2) + "s" :
			"-";
		documentRef.getElementById("timeline-count").textContent =
			visibleTimeline.length + "/" + viewModel.timeline.length + " events";
		renderMetrics(documentRef, documentRef.getElementById("metrics"), viewModel.metrics);
		renderTacticalSummary(documentRef, documentRef.getElementById("tactical-summary"), viewModel.tactical);
		renderEventDetails(documentRef, documentRef.getElementById("event-detail"), selectedEvent);
		renderShotDetails(documentRef, documentRef.getElementById("shot-detail"), selectedEvent);
		renderPressureDetails(documentRef,
			documentRef.getElementById("pressure-details"),
			viewModel.pressure);
		renderPassLaneDetails(documentRef,
			documentRef.getElementById("pass-lane-details"),
			viewModel.passLane);
		renderTimelineSummary(documentRef,
			documentRef.getElementById("timeline-summary"),
			timelineTeamCounts(visibleTimeline, viewModel.teams));
		renderTimeline(documentRef,
			documentRef.getElementById("match-timeline"),
			visibleTimeline,
			selectedEvent ? selectedEvent.id : "",
			onTimelineSelect);
		renderHeatmapSection(documentRef, viewModel);
		renderField(documentRef,
			documentRef.getElementById("field-view"),
			viewModel,
			fieldLayerState(documentRef),
			selectedEvent);
	}

	function setFocusView(documentRef, view) {
		if(FOCUS_VIEWS.indexOf(view) === -1) {
			return;
		}

		var report = documentRef.getElementById("report");
		if(report) {
			report.setAttribute("data-view", view);
		}
		var buttons = documentRef.querySelectorAll(".view-button");
		for(var i = 0; i < buttons.length; i++) {
			var active = buttons[i].getAttribute("data-view") === view;
			buttons[i].classList.toggle("active", active);
			buttons[i].setAttribute("aria-pressed", active ? "true" : "false");
		}
	}

	function initFocusControls(documentRef) {
		var buttons = documentRef.querySelectorAll(".view-button");
		for(var i = 0; i < buttons.length; i++) {
			buttons[i].addEventListener("click", function(event) {
				setFocusView(documentRef, event.currentTarget.getAttribute("data-view"));
			});
		}
		setFocusView(documentRef, "field");
	}

	function initLayerControls(documentRef, getViewModel, getSelectedEvent) {
		var toggles = documentRef.querySelectorAll(".layer-toggle");
		for(var i = 0; i < toggles.length; i++) {
			toggles[i].addEventListener("change", function() {
				var viewModel = getViewModel();
				if(viewModel) {
					renderField(documentRef,
						documentRef.getElementById("field-view"),
						viewModel,
						fieldLayerState(documentRef),
						getSelectedEvent());
				}
			});
		}
	}

	function initHeatmapControls(documentRef, getViewModel) {
		var controls = [
			documentRef.getElementById("heatmap-team-filter"),
			documentRef.getElementById("heatmap-entity-filter"),
			documentRef.getElementById("heatmap-player-filter")
		];
		controls.forEach(function(control) {
			if(!control) {
				return;
			}
			control.addEventListener("change", function() {
				var viewModel = getViewModel();
				if(viewModel) {
					renderHeatmapSection(documentRef, viewModel);
				}
			});
		});
	}

	function initTimelineControls(documentRef, getViewModel, getSelectedEventId, onSelect) {
		var filters = documentRef.querySelectorAll(".timeline-filter");
		var teamFilter = documentRef.getElementById("timeline-team-filter");
		var search = documentRef.getElementById("timeline-search");
		var controls = [];
		for(var i = 0; i < filters.length; i++) {
			controls.push(filters[i]);
		}
		if(teamFilter) {
			controls.push(teamFilter);
		}
		if(search) {
			controls.push(search);
		}
		controls.forEach(function(control) {
			var eventName = control === search ? "input" : "change";
			control.addEventListener(eventName, function() {
				var viewModel = getViewModel();
				if(!viewModel) {
					return;
				}
				var visibleTimeline = filterTimelineItems(viewModel.timeline, timelineFilterState(documentRef));
				var nextId = repairedTimelineSelection(visibleTimeline, getSelectedEventId());
				onSelect(nextId, { stayInView: true });
			});
		});
	}

	function focusTimelineButton(documentRef, selectedId) {
		var buttons = documentRef.querySelectorAll(".timeline-button");
		for(var i = 0; i < buttons.length; i++) {
			if(buttons[i].getAttribute("data-event-id") === selectedId) {
				buttons[i].focus();
				return;
			}
		}
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
		initFocusControls(documentRef);

		var catalogPath = "../../fixtures/replays/index.json";
		var catalog = await loadJson(catalogPath);
		var entries = catalog.replays;
		var replayData = {};
		var activeViewModel = null;
		var selectedEventId = "";
		initLayerControls(documentRef,
			function() { return activeViewModel; },
			function() {
				if(!activeViewModel) {
					return null;
				}
				var visibleTimeline = filterTimelineItems(activeViewModel.timeline, timelineFilterState(documentRef));
				var repairedEventId = repairedTimelineSelection(visibleTimeline, selectedEventId);
				return repairedEventId ?
					selectedTimelineItem(visibleTimeline, repairedEventId) :
					null;
			});
		initHeatmapControls(documentRef, function() { return activeViewModel; });
		initTimelineControls(documentRef,
			function() { return activeViewModel; },
			function() { return selectedEventId; },
			selectTimelineEvent);

		function selectTimelineEvent(id, options) {
			if(!activeViewModel) {
				return;
			}
			options = options || {};
			selectedEventId = id;
			if(!options.stayInView) {
				setFocusView(documentRef, "field");
			}
			renderReplay(documentRef, activeViewModel, selectedEventId, selectTimelineEvent);
			if(options.focusTimeline) {
				focusTimelineButton(documentRef, selectedEventId);
			}
		}

		async function selectReplay(id) {
			var entry = entries.find(function(item) { return item.id === id; });
			if(!entry) {
				return;
			}
			if(!replayData[id]) {
				var manifest = await loadJson(fixturePath(entry.manifest));
				var manifestBase = directoryName(entry.manifest);
				var summary = await loadJson(fixturePath(joinPath(manifestBase, manifest.files.summary)));
				var snapshotText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.snapshots)));
				var passLaneText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.pass_lanes)));
				var pressureText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.pressure)));
				var derivedText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.derived_events)));
				var shotText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.shots)));
				var collisionText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.collisions)));
				var metricsText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.metrics)));
				var heatmapText = await loadText(fixturePath(joinPath(manifestBase, manifest.files.heatmap)));
				replayData[id] = {
					manifest: manifest,
					summary: summary,
					snapshotRows: parseCsv(snapshotText),
					passLaneRows: parseCsv(passLaneText),
					pressureRows: parseCsv(pressureText),
					derivedRows: parseCsv(derivedText),
					shotRows: parseCsv(shotText),
					collisionRows: parseCsv(collisionText),
					metricsRows: parseCsv(metricsText),
					heatmapRows: parseCsv(heatmapText)
				};
			}
			var data = replayData[id];
			var viewModel = buildReplayViewModel(entry,
				data.manifest,
				data.summary,
				data.passLaneRows,
				data.pressureRows,
				data.derivedRows,
				data.shotRows,
				data.collisionRows,
				data.snapshotRows,
				data.metricsRows,
				data.heatmapRows);
			activeViewModel = viewModel;
			selectedEventId = repairedTimelineSelection(
				filterTimelineItems(viewModel.timeline, timelineFilterState(documentRef)),
				selectedEventId);
			renderCatalog(documentRef, documentRef.getElementById("catalog"), entries, id, selectReplay);
			renderReplay(documentRef, viewModel, selectedEventId, selectTimelineEvent);
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
		focusViews: FOCUS_VIEWS.slice(),
		fieldLayers: FIELD_LAYERS.slice(),
		buildReplayViewModel: buildReplayViewModel,
		directoryName: directoryName,
		fieldPoint: fieldPoint,
		fieldLayerState: fieldLayerState,
		fixturePath: fixturePath,
		formatNumber: formatNumber,
		buildHeatmap: buildHeatmap,
		filterHeatmap: filterHeatmap,
		buildMovementMetrics: buildMovementMetrics,
		summarizeMovementMetrics: summarizeMovementMetrics,
		buildPossessionZones: buildPossessionZones,
		possessionZoneRows: possessionZoneRows,
		buildTimelineItems: buildTimelineItems,
		filterTimelineItems: filterTimelineItems,
		repairedTimelineSelection: repairedTimelineSelection,
		timelineTeamCounts: timelineTeamCounts,
		selectedEventDetails: selectedEventDetails,
		selectedShotDetails: selectedShotDetails,
		selectedShotOverlay: selectedShotOverlay,
		buildShotQuality: buildShotQuality,
		latestPressureForShot: latestPressureForShot,
		timelineNavigationTarget: timelineNavigationTarget,
		selectedTimelineItem: selectedTimelineItem,
		setFocusView: setFocusView,
		joinPath: joinPath,
		latestPassLaneOptions: latestPassLaneOptions,
		latestPressureFrame: latestPressureFrame,
		parseCsv: parseCsv
	};
}));
