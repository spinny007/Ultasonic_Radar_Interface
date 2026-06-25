#ifndef RADAR_INDEX_HTML_H
#define RADAR_INDEX_HTML_H

#include <Arduino.h>

// ============================================================
//  01 — PRIMARY RADAR OPERATOR DECK DASHBOARD (Served at /)
// ============================================================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>360° Realtime Radar Tactical Control Deck</title>
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link href="https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Orbitron:wght@400;700;900&display=swap" rel="stylesheet">
    <style>
        :root {
            --green: #00ff41;
            --green-dim: #00cc33;
            --green-glow: rgba(0, 255, 65, 0.35);
            --green-faint: rgba(0, 255, 65, 0.07);
            --red: #ff2244;
            --amber: #ffaa00;
            --bg: #030704;
            --bg-card: #070d08;
            --bg-deeper: #040904;
            --border: rgba(0, 255, 65, 0.18);
            --border-active: rgba(0, 255, 65, 0.55);
            --font-mono: 'Share Tech Mono', monospace;
            --font-display: 'Orbitron', monospace;
        }

        *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

        body {
            background: var(--bg);
            color: var(--green);
            font-family: var(--font-mono);
            min-height: 100vh;
            overflow-x: hidden;
        }

        body::before {
            content: '';
            position: fixed;
            inset: 0;
            background: repeating-linear-gradient(
                0deg,
                transparent,
                transparent 2px,
                rgba(0,0,0,0.08) 2px,
                rgba(0,0,0,0.08) 4px
            );
            pointer-events: none;
            z-index: 9999;
        }

        body::after {
            content: '';
            position: fixed;
            inset: 0;
            background: radial-gradient(ellipse at center, transparent 60%, rgba(0,0,0,0.7) 100%);
            pointer-events: none;
            z-index: 9998;
        }

        .layout {
            display: grid;
            grid-template-rows: auto 1fr;
            grid-template-columns: 1fr;
            min-height: 100vh;
            padding: 16px;
            gap: 16px;
        }

        header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 1px solid var(--border);
            padding-bottom: 12px;
            animation: fadeIn 0.6s ease both;
        }

        .header-left { display: flex; align-items: center; gap: 14px; }

        .header-icon {
            width: 36px; height: 36px;
            border: 1px solid var(--green-dim);
            border-radius: 50%;
            display: flex; align-items: center; justify-content: center;
            font-size: 18px;
            box-shadow: 0 0 10px var(--green-glow);
            animation: pulse 2s ease-in-out infinite;
        }

        h1 {
            font-family: var(--font-display);
            font-size: clamp(0.65rem, 1.5vw, 0.9rem);
            font-weight: 700;
            letter-spacing: 0.12em;
            text-shadow: 0 0 12px var(--green-glow);
            color: var(--green);
        }

        .header-right { display: flex; align-items: center; gap: 12px; }

        .clock {
            font-family: var(--font-display);
            font-size: 0.75rem;
            color: var(--green-dim);
            letter-spacing: 0.1em;
        }

        .status-badge {
            font-family: var(--font-display);
            font-size: 0.6rem;
            font-weight: 700;
            letter-spacing: 0.15em;
            padding: 4px 10px;
            border: 1px solid currentColor;
            border-radius: 2px;
        }
        .status-badge.offline { color: var(--red); box-shadow: 0 0 6px rgba(255,34,68,0.4); }
        .status-badge.online  { color: var(--green); box-shadow: 0 0 6px var(--green-glow); }

        .main-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 16px;
            align-items: start;
        }

        @media (max-width: 1000px) {
            .main-grid { grid-template-columns: 1fr; }
        }

        .card {
            background: var(--bg-card);
            border: 1px solid var(--border);
            padding: 14px;
            position: relative;
            animation: fadeIn 0.7s ease both;
        }
        .card::before {
            content: '';
            position: absolute;
            top: 0; left: 0; right: 0;
            height: 1px;
            background: linear-gradient(90deg, transparent, var(--green-dim), transparent);
            opacity: 0.4;
        }

        .card-label {
            font-size: 0.65rem;
            letter-spacing: 0.2em;
            color: rgba(0,255,65,0.45);
            margin-bottom: 10px;
            text-transform: uppercase;
        }

        .right-col { display: flex; flex-direction: column; gap: 14px; }

        .radar-wrap {
            position: relative;
            width: 100%;
            max-width: 560px;
            margin: 0 auto;
        }

        canvas {
            width: 100%;
            height: auto;
            display: block;
            border-radius: 50%;
            border: 1px solid rgba(0,255,65,0.25);
            box-shadow:
                0 0 40px rgba(0,255,65,0.12),
                inset 0 0 60px rgba(0,0,0,0.6);
        }

        .controls-grid {
            display: grid;
            grid-template-columns: 1fr auto auto;
            gap: 8px;
            align-items: end;
        }

        label {
            font-size: 0.6rem;
            letter-spacing: 0.15em;
            color: rgba(0,255,65,0.5);
            display: block;
            margin-bottom: 4px;
            text-transform: uppercase;
        }

        select, input[type="number"] {
            width: 100%;
            background: var(--bg-deeper);
            border: 1px solid rgba(0,255,65,0.25);
            color: var(--green);
            font-family: var(--font-mono);
            font-size: 0.78rem;
            padding: 6px 8px;
            outline: none;
            appearance: none;
            -webkit-appearance: none;
            transition: border-color 0.2s;
        }
        select:focus, input:focus {
            border-color: var(--green);
            box-shadow: 0 0 8px var(--green-glow);
        }
        select option { background: #060d07; }

        .btn-row { display: flex; gap: 8px; }

        button {
            flex: 1;
            font-family: var(--font-mono);
            font-size: 0.72rem;
            letter-spacing: 0.1em;
            padding: 8px 12px;
            border: 1px solid;
            background: transparent;
            cursor: pointer;
            transition: all 0.18s ease;
            text-transform: uppercase;
        }

        .btn-sim {
            color: var(--amber);
            border-color: rgba(255,170,0,0.35);
        }
        .btn-sim:hover, .btn-sim.active {
            background: rgba(255,170,0,0.08);
            border-color: var(--amber);
            color: var(--amber);
            box-shadow: 0 0 10px rgba(255,170,0,0.25);
        }

        .btn-clear {
            color: var(--red);
            border-color: rgba(255,34,68,0.35);
        }
        .btn-clear:hover {
            background: rgba(255,34,68,0.07);
            border-color: var(--red);
            color: var(--red);
            box-shadow: 0 0 10px rgba(255,34,68,0.3);
        }

        .sensor-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
        .sensor-panel { background: var(--bg-deeper); border: 1px solid rgba(0,255,65,0.15); padding: 10px; position: relative; }
        .sensor-id { font-family: var(--font-display); font-size: .55rem; letter-spacing: .2em; color: rgba(0,255,65,0.4); margin-bottom: 6px; }
        .sensor-val { font-family: var(--font-display); font-size: 1.4rem; font-weight: 700; line-height: 1; margin-bottom: 4px; }
        .sensor-val.s1 { color: #00ff41; text-shadow: 0 0 12px rgba(0,255,65,0.6); }
        .sensor-val.s2 { color: #00ddc8; text-shadow: 0 0 12px rgba(0,221,200,0.6); }
        .sensor-meta { font-size: .62rem; color: rgba(0,255,65,0.4); }
        .sensor-state { position: absolute; top: 8px; right: 8px; font-size: .52rem; font-family: var(--font-display); letter-spacing: .1em; padding: 2px 6px; border: 1px solid; border-radius: 2px; }
        .sensor-state.clear    { color: var(--green); border-color: rgba(0,255,65,0.4); }
        .sensor-state.warn     { color: var(--amber); border-color: rgba(255,170,0,0.4); }
        .sensor-state.filtered { color: rgba(0,255,65,0.3); border-color: rgba(0,255,65,0.15); }

        .log-table-wrap { overflow: hidden; }
        table { width: 100%; border-collapse: collapse; font-size: .67rem; }
        th { text-align: left; padding: 4px 8px; color: rgba(0,255,65,0.35); font-size: .58rem; letter-spacing: .15em; border-bottom: 1px solid rgba(0,255,65,0.1); }
        td { padding: 5px 8px; border-bottom: 1px solid rgba(0,255,65,0.06); color: rgba(0,255,65,0.7); }
        tr:last-child td { border-bottom: none; }
        tr.warn td { color: var(--amber); }
        tr.alert td { color: var(--red); }

        .console-box { background: var(--bg-deeper); border: 1px solid rgba(0,255,65,0.12); padding: 10px; height: 110px; overflow-y: auto; font-size: .66rem; line-height: 1.6; scrollbar-width: thin; scrollbar-color: rgba(0,255,65,0.2) transparent; }
        .log-time { color: rgba(0,255,65,0.35); }
        .log-ok   { color: rgba(0,255,65,0.6); }
        .log-warn { color: var(--amber); }
        .log-err  { color: var(--red); }
        .log-msg  { color: rgba(0,255,65,0.35); }

        .heatmap-row { display: flex; align-items: flex-end; gap: 1px; height: 40px; }
        .heatmap-bar { flex: 1; min-width: 1px; border-radius: 1px 1px 0 0; transition: height .15s ease, background .15s ease; }

        .info-bar { display: flex; justify-content: center; gap: 24px; padding: 8px 12px; border: 1px solid var(--border); font-size: 0.62rem; color: rgba(0,255,65,0.4); letter-spacing: 0.1em; margin-top: 4px; flex-wrap: wrap; }
        .info-bar span { white-space: nowrap; }
        .info-bar strong { color: rgba(0,255,65,0.65); }

        @keyframes fadeIn { from { opacity: 0; transform: translateY(6px); } to { opacity: 1; transform: translateY(0); } }
        @keyframes pulse  { 0%,100% { box-shadow: 0 0 6px var(--green-glow); } 50% { box-shadow: 0 0 16px var(--green-glow), 0 0 30px rgba(0,255,65,0.15); } }
    </style>
</head>
<body>

<div class="layout">
    <header>
        <div class="header-left">
            <div class="header-icon">&#x1F4E1;</div>
            <div>
                <h1>RUDRA R360&deg; V1 &mdash; DUAL-ARRAY RADAR OPERATOR DECK</h1>
                <div style="font-size:.58rem;color:rgba(0,255,65,.35);letter-spacing:.1em;margin-top:2px">SG90 + HC-SR04 x2 &nbsp;|&nbsp; 360&deg; COVERAGE &nbsp;|&nbsp; S1:0&deg;-179&deg; &nbsp;|&nbsp; S2:180&deg;-359&deg;</div>
            </div>
        </div>
        <div class="header-right">
            <div class="clock" id="clock">--:--:--</div>
            <div class="status-badge offline" id="hwBadge">SIM</div>
        </div>
    </header>

    <div class="main-grid">
        <div>
            <div class="card">
                <div class="card-label">// SENSOR ARRAY MAP - 360&deg; SWEEP</div>
                <div class="radar-wrap"><canvas id="radarCanvas"></canvas></div>
            </div>
            <div class="card" style="margin-top:14px">
                <div class="card-label">// FILTER / SECTOR CONTROL</div>
                <div class="controls-grid" style="margin-bottom:10px">
                    <div>
                        <label for="filterMode">Filter Mode</label>
                        <select id="filterMode" onchange="applyFilter()">
                            <option value="ALL">ALL - Full 360&deg;</option>
                            <option value="FOCUS">FOCUS - Watch Zone</option>
                            <option value="MUTE">MUTE - Dead Zone</option>
                        </select>
                    </div>
                    <div>
                        <label for="arcStart">Arc Start &deg;</label>
                        <input type="number" id="arcStart" min="0" max="359" value="0" style="width:72px">
                    </div>
                    <div>
                        <label for="arcEnd">Arc End &deg;</label>
                        <input type="number" id="arcEnd" min="0" max="359" value="90" style="width:72px">
                    </div>
                </div>
            </div>
        </div>

        <div class="right-col">
            <div class="card">
                <div class="card-label">// SENSOR STATUS</div>
                <div class="sensor-grid">
                    <div class="sensor-panel">
                        <div class="sensor-id">S1 - FORWARD ARRAY (0&deg;-179&deg;)</div>
                        <div class="sensor-val s1" id="dist1">---</div>
                        <div class="sensor-meta" id="meta1">angle: --- &nbsp;|&nbsp; ---</div>
                        <div class="sensor-state clear" id="state1">IDLE</div>
                    </div>
                    <div class="sensor-panel">
                        <div class="sensor-id">S2 - AFT ARRAY (180&deg;-359&deg;)</div>
                        <div class="sensor-val s2" id="dist2">---</div>
                        <div class="sensor-meta" id="meta2">angle: --- &nbsp;|&nbsp; ---</div>
                        <div class="sensor-state clear" id="state2">IDLE</div>
                    </div>
                </div>
            </div>

            <div class="card">
                <div class="card-label">// 360&deg; ACTIVITY HEATMAP</div>
                <div class="heatmap-row" id="heatmapRow"></div>
                <div style="display:flex;justify-content:space-between;margin-top:4px;font-size:.55rem;color:rgba(0,255,65,.3)">
                    <span>0&deg;</span><span>90&deg;</span><span>180&deg;</span><span>270&deg;</span><span>360&deg;</span>
                </div>
            </div>

            <div class="card">
                <div class="card-label">// System Override & Emulation Deck</div>
                <div class="btn-row">
                    <button class="btn-sim active" id="btnSim" onclick="toggleSimulation()">&#x26A1; SIM: ON</button>
                    <button class="btn-clear" onclick="clearRadarTracks()">&#x2715; CLEAR TRACKS</button>
                </div>
            </div>

            <div class="card">
                <div class="card-label">// DETECTION LOG</div>
                <div class="log-table-wrap">
                    <table>
                        <thead><tr><th>TIME</th><th>SENSOR</th><th>ANGLE</th><th>DIST</th><th>CLASS</th></tr></thead>
                        <tbody id="logTableBody"></tbody>
                    </table>
                </div>
            </div>

            <div class="card">
                <div class="card-label">// SYSTEM CONSOLE</div>
                <div class="console-box" id="consoleBox"></div>
                <div class="info-bar">
                    <span><strong>Range Limit:</strong> 150 cm</span>
                    <span><strong>Sweep:</strong> Dual-Array Passive</span>
                    <span><strong>Grid:</strong> 30° / 45° / 60° / 90°</span>
                </div>
            </div>
        </div>
    </div>
</div>

<script>
"use strict";

var canvas  = document.getElementById('radarCanvas');
var ctx     = canvas.getContext('2d');
var LOGICAL = 600;
var dpr     = window.devicePixelRatio || 1;
canvas.width  = LOGICAL * dpr;
canvas.height = LOGICAL * dpr;
canvas.style.width  = LOGICAL + 'px';
canvas.style.height = LOGICAL + 'px';
ctx.scale(dpr, dpr);
var cx = LOGICAL / 2, cy = LOGICAL / 2;
var rMax = LOGICAL / 2 - 22;
var MAX_DIST_CM = 150, MAX_TAIL = 20, LOG_ROWS = 8, LOG_COOLDOWN_MS = 3000;

var radarMap     = new Float32Array(360);
var mapHits      = new Float32Array(360);
var sweepAngles  = [0, 180];
var sweepHistory = [];
var filterMode   = 'ALL';
var arcStart = 0, arcEnd = 90;
var simRunning = true;
var isHardwareConnected = false;
var lastLogTime = [0, 0];
var SENSOR_DOMAIN = [{min:0,max:179},{min:180,max:359}];

function inSensorDomain(si, a) { return a >= SENSOR_DOMAIN[si].min && a <= SENSOR_DOMAIN[si].max; }

var simTargets = [{angle:45,dist:80,sensor:0},{angle:220,dist:55,sensor:1}];

function updateSimTargets() {
    if (filterMode === 'ALL') { simTargets = [{angle:45,dist:80,sensor:0},{angle:220,dist:55,sensor:1}]; return; }
    var mid = Math.round((arcStart + arcEnd) / 2) % 360;
    simTargets = [
        {angle: mid <= 179 ? mid : 45,  dist:80, sensor:0},
        {angle: mid > 179  ? mid : 220, dist:55, sensor:1}
    ];
}

function isAngleAllowed(a) {
    if (filterMode === 'ALL') return true;
    var inZone = arcStart <= arcEnd ? (a >= arcStart && a <= arcEnd) : (a >= arcStart || a <= arcEnd);
    return filterMode === 'FOCUS' ? inZone : !inZone;
}

function applyFilter() {
    filterMode = document.getElementById('filterMode').value;
    arcStart   = parseInt(document.getElementById('arcStart').value) || 0;
    arcEnd     = parseInt(document.getElementById('arcEnd').value)   || 90;
    updateSimTargets();
    logToConsole('Filter: ' + filterMode + ' arc ' + arcStart + 'deg->' + arcEnd + 'deg', 'msg');
}
document.getElementById('arcStart').addEventListener('input', applyFilter);
document.getElementById('arcEnd').addEventListener('input',   applyFilter);

function logToConsole(msg, type) {
    var box  = document.getElementById('consoleBox');
    if(!box) return;
    var line = document.createElement('div');
    line.className   = 'log-' + (type || 'msg');
    line.textContent = '[' + new Date().toLocaleTimeString() + '] ' + msg;
    box.appendChild(line);
    while (box.children.length > 40) box.removeChild(box.firstChild);
    box.scrollTop = box.scrollHeight;
}

function updateSensorUI(idx, angle, dist, active) {
    var n = idx + 1;
    var dEl = document.getElementById('dist' + n);
    var mEl = document.getElementById('meta' + n);
    var sEl = document.getElementById('state' + n);
    if (!dEl) return;
    if (!active) { sEl.textContent = 'FILTERED'; sEl.className = 'sensor-state filtered'; return; }
    dEl.textContent  = dist > 0 ? dist + ' cm' : '---';
    mEl.innerHTML    = 'angle: ' + angle + 'deg &nbsp;|&nbsp; ' + (dist > 0 ? dist + 'cm' : 'no echo');
    if (dist <= 0) { sEl.textContent = 'CLEAR';    sEl.className = 'sensor-state clear'; }
    else if (dist < 40) { sEl.textContent = 'ALERT'; sEl.className = 'sensor-state warn'; }
    else { sEl.textContent = 'SCANNING'; sEl.className = 'sensor-state clear'; }
}

function pushToHistoricalLog(si, angle, dist) {
    var tbody = document.getElementById('logTableBody');
    if(!tbody) return;
    var hits  = mapHits[angle] || 0;
    var cls   = hits > 70 ? 'Static Object' : hits > 30 ? 'Moving Object' : 'Transient Obstacle';
    var row   = document.createElement('tr');
    row.className = dist < 40 ? 'alert' : '';
    row.innerHTML = '<td>' + new Date().toLocaleTimeString() + '</td><td>S' + (si+1) + '</td><td>' + angle + 'deg</td><td>' + dist + 'cm</td><td>' + cls + '</td>';
    tbody.insertBefore(row, tbody.firstChild);
    while (tbody.rows.length > LOG_ROWS) tbody.deleteRow(tbody.rows.length - 1);
}

(function(){
    var row = document.getElementById('heatmapRow');
    if(!row) return;
    for (var i = 0; i < 360; i++) {
        var bar = document.createElement('div');
        bar.className = 'heatmap-bar'; bar.id = 'hb' + i;
        row.appendChild(bar);
    }
})();

setInterval(function() {
    for (var i = 0; i < 360; i++) {
        mapHits[i] = Math.max(0, mapHits[i] - 0.4);
        var bar = document.getElementById('hb' + i);
        if (!bar) continue;
        var h = Math.round((mapHits[i] / 100) * 38);
        var r = Math.floor((mapHits[i] / 100) * 255);
        var g = Math.floor(((100 - mapHits[i]) / 100) * 200) + 55;
        bar.style.height = h + 'px';
        bar.style.background = h > 0 ? 'rgb(' + r + ',' + g + ',20)' : 'transparent';
    }
}, 50);

var motorAngle = 0, motorDir = 1;

function toggleSimulation() {
    simRunning = !simRunning;
    var btn = document.getElementById('btnSim');
    if(btn) {
        btn.textContent = simRunning ? '⚡ SIM: ON' : '⚡ SIM: OFF';
        btn.classList.toggle('active', simRunning);
    }
    logToConsole('Simulation ' + (simRunning ? 'ENABLED' : 'DISABLED'), 'msg');
}

setInterval(function() {
    if (isHardwareConnected || !simRunning) return;
    motorAngle += motorDir;
    if (motorAngle >= 179 || motorAngle <= 0) motorDir *= -1;
    var a1 = motorAngle, a2 = (motorAngle + 180) % 360;
    sweepAngles[0] = a1; sweepAngles[1] = a2;
    simTargets.forEach(function(t) {
        var sa   = t.sensor === 0 ? a1 : a2;
        var diff = Math.abs(sa - t.angle);
        if (diff <= 3) {
            radarMap[t.angle] = t.dist;
            mapHits[t.angle]  = Math.min(mapHits[t.angle] + 20, 100);
            updateSensorUI(t.sensor, t.angle, t.dist, isAngleAllowed(t.angle));
            var now = Date.now();
            if (isAngleAllowed(t.angle) && now - lastLogTime[t.sensor] > LOG_COOLDOWN_MS) {
                pushToHistoricalLog(t.sensor, t.angle, t.dist);
                lastLogTime[t.sensor] = now;
            }
        }
    });
    sweepHistory.push([sweepAngles[0], sweepAngles[1]]);
    if (sweepHistory.length > MAX_TAIL) sweepHistory.shift();
}, 40);

function clearRadarTracks() {
    radarMap.fill(0); mapHits.fill(0); sweepHistory.length = 0;
    lastLogTime[0] = 0; lastLogTime[1] = 0;
    var tbody = document.getElementById('logTableBody');
    if(tbody) tbody.innerHTML = '';
    logToConsole('Tracks cleared.', 'msg');
}

setInterval(function() { 
    var clk = document.getElementById('clock');
    if(clk) clk.textContent = new Date().toLocaleTimeString(); 
}, 1000);

(function() {
    var ws = null;
    function connect() {
        try {
            var host = window.location.host ? window.location.host.split(':')[0] : '192.168.4.1';
            var wsUrl = 'ws://' + host + ':81/';
            ws = new WebSocket(wsUrl);
            ws.onopen = function() {
                isHardwareConnected = true;
                document.getElementById('hwBadge').textContent = 'LIVE';
                document.getElementById('hwBadge').className   = 'status-badge online';
                logToConsole('Hardware link secured: ' + wsUrl, 'ok');
            };
            ws.onclose = ws.onerror = function() {
                isHardwareConnected = false;
                document.getElementById('hwBadge').textContent = 'SIM';
                document.getElementById('hwBadge').className   = 'status-badge offline';
                logToConsole('Hardware stream lost - simulation active.', 'warn');
                setTimeout(connect, 3000);
            };
            ws.onmessage = function(e) {
                var sensors = e.data.split('|');
                sensors.forEach(function(chunk, idx) {
                    if (idx > 1) return;
                    var parts    = chunk.split(',');
                    var rawAngle = parseInt(parts[0]);
                    var distance = parseFloat(parts[1]);
                    if (isNaN(rawAngle) || isNaN(distance)) return;
                    var angle = ((rawAngle % 360) + 360) % 360;
                    if (!inSensorDomain(idx, angle)) return;
                    if (!isAngleAllowed(angle)) { updateSensorUI(idx, angle, -1, false); return; }
                    radarMap[angle] = distance; sweepAngles[idx] = angle;
                    if (distance > 0 && distance <= MAX_DIST_CM) mapHits[angle] = Math.min(mapHits[angle] + 20, 100);
                    updateSensorUI(idx, angle, distance, true);
                    var now = Date.now();
                    if (distance > 0 && now - lastLogTime[idx] > LOG_COOLDOWN_MS) {
                        pushToHistoricalLog(idx, angle, distance);
                        lastLogTime[idx] = now;
                    }
                });
                sweepHistory.push([sweepAngles[0], sweepAngles[1]]);
                if (sweepHistory.length > MAX_TAIL) sweepHistory.shift();
            };
        } catch(err) { logToConsole('WS error: ' + err.message, 'err'); setTimeout(connect, 5000); }
    }
    connect();
    window.addEventListener('beforeunload', function() { if (ws) ws.close(); });
})();

function drawRadar() {
    requestAnimationFrame(drawRadar);
    ctx.clearRect(0, 0, LOGICAL, LOGICAL);

    var bg = ctx.createRadialGradient(cx,cy,0,cx,cy,rMax);
    bg.addColorStop(0,'rgba(0,18,4,.98)'); bg.addColorStop(1,'rgba(0,6,1,.99)');
    ctx.fillStyle = bg; ctx.beginPath(); ctx.arc(cx,cy,rMax+2,0,Math.PI*2); ctx.fill();

    for (var r = 1; r <= 4; r++) {
        var rad = (r/4)*rMax;
        ctx.beginPath(); ctx.arc(cx,cy,rad,0,Math.PI*2);
        ctx.strokeStyle = 'rgba(0,255,65,' + (r===4 ? .22 : .09) + ')';
        ctx.lineWidth = r===4 ? 1.2 : .8; ctx.stroke();
        ctx.fillStyle = 'rgba(0,255,65,.3)'; ctx.font = '10px "Share Tech Mono"';
        ctx.fillText(Math.round((r/4)*MAX_DIST_CM)+'cm', cx+rad-34, cy-5);
    }

    [0,30,45,60,90,120,135,150].forEach(function(deg) {
        var r1 = degToRad(deg), r2 = degToRad(deg+180);
        ctx.strokeStyle = 'rgba(0,255,65,.07)'; ctx.lineWidth = .8;
        ctx.beginPath(); ctx.moveTo(cx+rMax*Math.cos(r1),cy+rMax*Math.sin(r1)); ctx.lineTo(cx+rMax*Math.cos(r2),cy+rMax*Math.sin(r2)); ctx.stroke();
        var tr = rMax+16; ctx.fillStyle='rgba(0,255,65,.28)'; ctx.font='10px "Share Tech Mono"';
        ctx.fillText(deg+'°', cx+tr*Math.cos(r1)-10, cy+tr*Math.sin(r1)+4);
        ctx.fillText((deg+180)+'°', cx+tr*Math.cos(r2)-14, cy+tr*Math.sin(r2)+4);
    });

    if (filterMode !== 'ALL') {
        ctx.fillStyle = filterMode==='MUTE' ? 'rgba(255,34,68,.07)' : 'rgba(0,255,65,.05)';
        var s = degToRad(arcStart), e = degToRad(arcEnd);
        ctx.beginPath(); ctx.moveTo(cx,cy);
        if (arcStart <= arcEnd) { ctx.arc(cx,cy,rMax,s,e); }
        else { ctx.arc(cx,cy,rMax,s,degToRad(359.99)); ctx.lineTo(cx,cy); ctx.arc(cx,cy,rMax,degToRad(0),e); }
        ctx.closePath(); ctx.fill(); ctx.lineWidth = 1;
        ctx.strokeStyle = filterMode==='MUTE' ? 'rgba(255,34,68,.35)' : 'rgba(0,255,65,.3)';
        [arcStart,arcEnd].forEach(function(deg) {
            var ra = degToRad(deg); ctx.beginPath(); ctx.moveTo(cx,cy); ctx.lineTo(cx+rMax*Math.cos(ra),cy+rMax*Math.sin(ra)); ctx.stroke();
        });
    }

    sweepHistory.forEach(function(aa, ti) {
        var alpha = (ti/sweepHistory.length)*0.14; ctx.lineWidth = 1.5;
        aa.forEach(function(angle, si) {
            if (!inSensorDomain(si,angle) || !isAngleAllowed(angle)) return;
            var ra = degToRad(angle);
            ctx.strokeStyle = si===0 ? 'rgba(0,255,65,'+alpha+')' : 'rgba(0,220,200,'+alpha+')';
            ctx.beginPath(); ctx.moveTo(cx,cy); ctx.lineTo(cx+rMax*Math.cos(ra),cy+rMax*Math.sin(ra)); ctx.stroke();
        });
    });

    for (var i = 0; i < 360; i++) {
        var dist = radarMap[i], hits = mapHits[i];
        if (dist<=0||dist>MAX_DIST_CM||hits<=0) continue;
        var ra = degToRad(i), br = (dist/MAX_DIST_CM)*rMax;
        var bx = cx+br*Math.cos(ra), by = cy+br*Math.sin(ra);
        var cr = Math.floor((hits/100)*255), cg = Math.floor(((100-hits)/100)*220)+35; var sz = 4+(hits/35);
        ctx.shadowBlur=hits>50?14:6; ctx.shadowColor='rgb('+cr+','+cg+',20)'; ctx.fillStyle='rgb('+cr+','+cg+',20)';
        ctx.beginPath(); ctx.arc(bx,by,sz,0,Math.PI*2); ctx.fill(); ctx.shadowBlur=0;
    }

    sweepAngles.forEach(function(angle,si) {
        if (!inSensorDomain(si,angle)||!isAngleAllowed(angle)) return;
        var ra  = degToRad(angle); var col = si===0 ? '#00ff41' : '#00ddc8';
        var gr  = ctx.createLinearGradient(cx,cy,cx+rMax*Math.cos(ra),cy+rMax*Math.sin(ra));
        gr.addColorStop(0,'rgba(0,0,0,0)'); gr.addColorStop(.6, si===0 ? 'rgba(0,255,65,.35)' : 'rgba(0,221,200,.35)'); gr.addColorStop(1,  si===0 ? 'rgba(0,255,65,.9)'  : 'rgba(0,221,200,.9)');
        ctx.strokeStyle=gr; ctx.lineWidth=si===0?3:2; ctx.shadowBlur=14; ctx.shadowColor=col;
        ctx.beginPath(); ctx.moveTo(cx,cy); ctx.lineTo(cx+rMax*Math.cos(ra),cy+rMax*Math.sin(ra)); ctx.stroke(); ctx.shadowBlur=0;
    });

    var ng = ctx.createRadialGradient(cx,cy,0,cx,cy,8); ng.addColorStop(0,'#00ff41'); ng.addColorStop(1,'rgba(0,255,65,0)');
    ctx.fillStyle=ng; ctx.shadowBlur=10; ctx.shadowColor='#00ff41'; ctx.beginPath(); ctx.arc(cx,cy,5,0,Math.PI*2); ctx.fill(); ctx.shadowBlur=0;
    ctx.strokeStyle='rgba(0,255,65,.3)'; ctx.lineWidth=1.5; ctx.beginPath(); ctx.arc(cx,cy,rMax,0,Math.PI*2); ctx.stroke();
}

logToConsole('Simulation Framework online.','ok');
logToConsole('S1:0-179deg | S2:180-359deg | Dual back-to-back sensors initialized.','ok');
drawRadar();
</script>
</body>
</html>
)rawliteral";

// ============================================================
//  02 — SYSTEM WIFI PORTAL STORAGE (Served at /config)
// ============================================================
const char config_html[] PROGMEM = R"CFGEOF(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Radar Config</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{background:#030704;color:#00ff41;font-family:'Courier New',monospace;min-height:100vh;display:flex;align-items:flex-start;justify-content:center;padding:28px 16px}
.wrap{width:100%;max-width:540px}
h1{font-size:.95rem;letter-spacing:.2em;text-align:center;margin-bottom:6px;text-shadow:0 0 10px rgba(0,255,65,.5)}
.sub{text-align:center;font-size:.6rem;color:rgba(0,255,65,.35);letter-spacing:.15em;margin-bottom:24px}
.card{background:#070d08;border:1px solid rgba(0,255,65,.2);padding:18px 20px;margin-bottom:16px;position:relative}
.card::before{content:'';position:absolute;top:0;left:0;right:0;height:1px;background:linear-gradient(90deg,transparent,#00cc33,transparent);opacity:.4}
.section-label{font-size:.58rem;letter-spacing:.22em;color:rgba(0,255,65,.4);margin-bottom:14px;text-transform:uppercase}
label{display:block;font-size:.63rem;letter-spacing:.14em;color:rgba(0,255,65,.5);margin-bottom:4px;margin-top:12px}
input[type=text],input[type=password]{width:100%;background:#040904;border:1px solid rgba(0,255,65,.22);color:#00ff41;font-family:'Courier New',monospace;font-size:.8rem;padding:8px 10px;outline:none;transition:border-color .2s,box-shadow .2s}
input:focus{border-color:#00ff41;box-shadow:0 0 6px rgba(0,255,65,.3)}
.btn-row{display:flex;gap:10px;margin-top:18px}
button{font-family:'Courier New',monospace;font-size:.72rem;letter-spacing:.1em;padding:10px 14px;border:1px solid;background:transparent;cursor:pointer;text-transform:uppercase;transition:all .18s;white-space:nowrap}
.btn-save{flex:1;color:#00ff41;border-color:rgba(0,255,65,.4)}
.btn-save:hover{background:rgba(0,255,65,.09);border-color:#00ff41;box-shadow:0 0 10px rgba(0,255,65,.25)}
.btn-reboot{color:#ffaa00;border-color:rgba(255,170,0,.35)}
.btn-reboot:hover{background:rgba(255,170,0,.08);border-color:#ffaa00}
.btn-scan{color:#00ddc8;border-color:rgba(0,221,200,.35);padding:8px 14px;font-size:.68rem}
.btn-scan:hover{background:rgba(0,221,200,.08);border-color:#00ddc8}
.notice{font-size:.62rem;color:rgba(0,255,65,.38);text-align:center;margin-top:10px;line-height:1.5}
.sta-bar{display:flex;align-items:center;gap:10px;font-size:.65rem;padding:8px 10px;border:1px solid rgba(0,255,65,.15);background:#040904;margin-bottom:16px}
.sta-dot{width:8px;height:8px;border-radius:50%;flex-shrink:0}
.sta-dot.on{background:#00ff41;box-shadow:0 0 6px #00ff41}
.sta-dot.off{background:#ff2244;box-shadow:0 0 4px #ff2244}
.net-list{display:flex;flex-direction:column;gap:6px;max-height:280px;overflow-y:auto}
.net-item{display:flex;align-items:center;gap:10px;padding:9px 12px;border:1px solid rgba(0,255,65,.15);cursor:pointer;background:#040904;}
.net-item:hover{border-color:rgba(0,255,65,.5);background:rgba(0,255,65,.06)}
.net-ssid{flex:1;font-size:.78rem;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.net-lock{font-size:.75rem;opacity:.6}
.signal-bars{display:flex;align-items:flex-end;gap:2px;height:14px;}
.signal-bars span{width:4px;background:rgba(0,255,65,.2);border-radius:1px}
.signal-bars span.lit{background:#00ff41}
.scan-msg{color:rgba(0,255,65,.35);font-size:.7rem;text-align:center;padding:20px;}
.modal-bg{display:none;position:fixed;inset:0;background:rgba(0,0,0,.75);z-index:100;align-items:center;justify-content:center}
.modal-bg.open{display:flex}
.modal{background:#070d08;border:1px solid rgba(0,255,65,.35);padding:24px;width:92%;max-width:380px}
.modal h2{font-size:.8rem;letter-spacing:.2em;margin-bottom:4px}
.modal .ssid-name{font-size:.68rem;color:rgba(0,255,65,.5);margin-bottom:16px;}
.modal-btns{display:flex;gap:8px;margin-top:16px}
.btn-connect{flex:1;color:#00ff41;border-color:rgba(0,255,65,.4)}
.btn-connect:hover{background:rgba(0,255,65,.09);border-color:#00ff41}
.btn-cancel{color:rgba(0,255,65,.4);border-color:rgba(0,255,65,.2)}
.result-banner{display:none;padding:12px 14px;font-size:.72rem;line-height:1.6;margin-top:12px;border:1px solid}
.result-banner.ok{color:#00ff41;border-color:rgba(0,255,65,.4);background:rgba(0,255,65,.05)}
.result-banner.err{color:#ff2244;border-color:rgba(255,34,68,.4);background:rgba(255,34,68,.05)}
</style>
</head>
<body>
<div class="wrap">
  <h1>📡 RADAR SYSTEM CONFIGURATION</h1>
  <div class="sub">// SUPERADMIN ACCESS &mdash; CHANGES PERSIST TO FLASH</div>
  <div class="sta-bar" id="staBar"><div class="sta-dot off" id="staDot"></div><span id="staText">Checking network infrastructure loop...</span></div>
  <div class="card">
    <div class="section-label">// 01 &mdash; TARGET ROUTER INTEGRATION</div>
    <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:12px">
      <span style="font-size:.65rem;color:rgba(0,255,65,.45)">Select node gateway channel from list:</span>
      <button class="btn-scan" onclick="scanNetworks()">&#x21BB; DISCOVER</button>
    </div>
    <div class="net-list" id="netList"><div class="scan-msg">Execute scan command to pulse local environment...</div></div>
    <div class="result-banner" id="connectResult"></div>
  </div>
  <div class="card">
    <div class="section-label">// 02 &mdash; LOCAL STATION BEACON AP REGISTRY</div>
    <label for="apssid">SSID Beacon Address</label><input id="apssid" type="text" maxlength="32">
    <label for="appass">Encryption Handshake Key</label><input id="appass" type="password" maxlength="32">
    <div class="btn-row"><button class="btn-save" onclick="saveSection('ap')">Commit Sector 02</button></div>
  </div>
  <div class="card">
    <div class="section-label">// 03 &mdash; PASSIVE DECK PRIVILEGES</div>
    <label for="duser">Operator Handle</label><input id="duser" type="text" maxlength="32">
    <label for="dpass">Passkey Token</label><input id="dpass" type="password" maxlength="32">
    <div class="btn-row"><button class="btn-save" onclick="saveSection('dash')">Commit Sector 03</button></div>
  </div>
  <div class="btn-row" style="justify-content:flex-end"><button class="btn-reboot" onclick="doReboot()">Execute Cold Reboot</button></div>
</div>
<div class="modal-bg" id="modalBg">
  <div class="modal">
    <h2>🔒 INITIALIZE PASSKEY HANDSHAKE</h2><div class="ssid-name" id="modalSSID"></div>
    <input id="modalPass" type="password" maxlength="64" placeholder="Enter WPA2 Key">
    <div class="modal-btns"><button class="btn-connect" onclick="doConnect()">AUTHENTICATE</button><button class="btn-cancel" onclick="closeModal()">ABORT</button></div>
  </div>
</div>
<script>
var selectedSSID = '';
window.addEventListener('load', fetchStatus);
function fetchStatus() {
  fetch('/wifi/status').then(r=>r.json()).then(d=>{
    var dot = document.getElementById('staDot'), txt = document.getElementById('staText');
    if (d.connected) { dot.className='sta-dot on'; txt.innerHTML='Linked to <b>'+esc(d.ssid)+'</b> | IP: <b>'+esc(d.ip)+'</b>'; }
    else { dot.className='sta-dot off'; txt.textContent='System Link Offline.'; }
  });
}
function scanNetworks() {
  var list = document.getElementById('netList'); list.innerHTML = '<div class="scan-msg">PULSARY DISCOVERY MODE CYCLING...</div>';
  fetch('/wifi/scan').then(r=>r.json()).then(nets=>{
    list.innerHTML = '';
    if(!nets.length) { list.innerHTML='<div class="scan-msg">Zero arrays discovered.</div>'; return; }
    nets.forEach(n=>{
      var item = document.createElement('div'); item.className='net-item'; item.onclick=()=>{ openModal(n.ssid); };
      var bars='', lvl=n.rssi>-55?4:n.rssi>-67?3:n.rssi>-78?2:1;
      for(var i=0;i<4;i++) bars+='<span style="height:'+([4,7,10,13][i])+'px" class="'+(i<lvl?'lit':'')+'"></span>';
      item.innerHTML = '<div class="signal-bars">'+bars+'</div><span class="net-ssid">'+esc(n.ssid)+'</span><span>'+(n.enc?'🔒':'🔓')+'</span>';
      list.appendChild(item);
    });
  });
}
function openModal(ssid) { selectedSSID=ssid; document.getElementById('modalSSID').textContent=ssid; document.getElementById('modalBg').className='modal-bg open'; }
function closeModal() { document.getElementById('modalBg').className='modal-bg'; }
function doConnect() {
  var p = document.getElementById('modalPass').value; var res = document.getElementById('connectResult'); closeModal();
  fetch('/wifi/connect',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'ssid='+encodeURIComponent(selectedSSID)+'&pass='+encodeURIComponent(p)})
  .then(r=>r.json()).then(d=>{
    res.style.display='block';
    if(d.connected){ res.className='result-banner ok'; res.innerHTML='Success IP: '+d.ip; fetchStatus(); }
    else { res.className='result-banner err'; res.textContent='Link rejected.'; }
  });
}
function saveSection(sec) {
  var val = sec==='ap'?'apssid='+encodeURIComponent(document.getElementById('apssid').value)+'&appass='+encodeURIComponent(document.getElementById('appass').value)
                     :'duser='+encodeURIComponent(document.getElementById('duser').value)+'&dpass='+encodeURIComponent(document.getElementById('dpass').value);
  fetch('/config/save',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:val}).then(()=>{alert('Sector buffered! Reboot device.');});
}
function doReboot() { if(confirm('Cold cycle node hardware?')) fetch('/reboot',{method:'POST'}); }
function esc(s) { return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;'); }
</script>
</body>
</html>
)CFGEOF";

#endif