// Global Variables to hold our setup data
let selectedScenario = null;

// Function triggered when a Scenario Card is clicked
function selectScenario(scenarioID) {
    selectedScenario = scenarioID;
    console.log("Scenario Selected: " + selectedScenario);

    // Update the UI Panels based on the scenario
    const customPanel = document.getElementById('customRulesPanel');
    const normalLoadout = document.getElementById('normalLoadoutUI');
    const warehouseLoadout = document.getElementById('warehouseLoadoutUI');

    // 1, 2, 3, 5 use normal packs. 4 uses battery %.
    if (scenarioID === 4) {
        normalLoadout.classList.add('hidden');
        warehouseLoadout.classList.remove('hidden');
    } else {
        normalLoadout.classList.remove('hidden');
        warehouseLoadout.classList.add('hidden');
    }

    // Custom Rules only for 5
    if (scenarioID === 5) {
        customPanel.classList.remove('hidden');
    } else {
        customPanel.classList.add('hidden');
    }

    // Un-hide the next section
    const step2 = document.getElementById('step-2-setup');
    step2.classList.remove('hidden');

    // Smooth scroll down to it
    step2.scrollIntoView({ behavior: 'smooth' });
}

// --- TASK 2 LOGIC ---

// Fetch maps when the page loads
window.addEventListener('DOMContentLoaded', async () => {
    try {
        const res = await fetch('/api/get-maps');
        const files = await res.json();
        const select = document.getElementById('mapSelect');
        select.innerHTML = ''; // Clear loading text
        
        if (files.length === 0) {
            select.innerHTML = '<option>No maps found in folder</option>';
            return;
        }

        files.forEach(f => {
            const opt = document.createElement('option');
            opt.value = `maps/${f}`;
            opt.textContent = f;
            select.appendChild(opt);
        });
    } catch (e) {
        console.error("Could not load maps.", e);
    }
});

// Dynamically generate the input boxes for the batteries
function generatePackInputs() {
    const numPacks = parseInt(document.getElementById('numPacks').value);
    const container = document.getElementById('packInputsContainer');
    
    container.innerHTML = ''; // Clear old inputs

    if (isNaN(numPacks) || numPacks <= 0) {
        container.innerHTML = '<p class="waiting-text">Enter the number of packs above to assign their values.</p>';
        return;
    }

    // Limit to 20 to prevent people from crashing the UI
    const safeNum = Math.min(numPacks, 20);

    for (let i = 1; i <= safeNum; i++) {
        const wrapper = document.createElement('div');
        wrapper.className = 'pack-input-wrapper';
        wrapper.style.display = 'flex';
        wrapper.style.gap = '10px';
        wrapper.style.alignItems = 'center';
        
        const label = document.createElement('label');
        label.textContent = `Pack ${i}`;
        label.style.minWidth = '50px';
        
        const energyInput = document.createElement('input');
        energyInput.type = 'number'; 
        energyInput.className = 'pack-energy-input modern-input'; 
        energyInput.placeholder = 'Energy';
        energyInput.style.width = '95px';
        energyInput.style.fontSize = '0.85rem';
        energyInput.min = '1';

        const weightInput = document.createElement('input');
        weightInput.type = 'number'; 
        weightInput.className = 'pack-weight-input modern-input'; 
        weightInput.placeholder = 'Weight';
        weightInput.style.width = '95px';
        weightInput.style.fontSize = '0.85rem';
        weightInput.min = '1';

        wrapper.appendChild(label);
        wrapper.appendChild(energyInput);
        wrapper.appendChild(weightInput);
        container.appendChild(wrapper);
    }
}

// --- TASK 3: THE LIVE TRACKING LOGIC ---

let mapData = [];
let cellSize = 20;
let isAnimating = false;

// --- NEW: Multi-Agent Swarm Variables ---
let activeRobots = {}; 
const robotColors = {
    "R1": "#00f6ff", // Neon Cyan
    "R2": "#ff003c", // Crimson Red
    "R3": "#ffea00", // Bright Yellow
    "R4": "#b700ff"  // Purple
};

const canvas = document.getElementById('gridCanvas');
const ctx = canvas.getContext('2d');
const logBox = document.getElementById('actionLog');

function writeLog(text, cssClass = '') {
    const div = document.createElement('div');
    div.className = `log-entry ${cssClass}`;
    div.textContent = `> ${text}`;
    logBox.appendChild(div);
    logBox.scrollTop = logBox.scrollHeight; 
}

function drawBaseMap() {
    const rows = mapData.length;
    const cols = mapData[0].length;
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    for (let r = 0; r < rows; r++) {
        for (let c = 0; c < cols; c++) {
            let val = mapData[r][c];
            ctx.fillStyle = (val === 1) ? '#475569' : (val === -1) ? '#ef4444' : '#0f172a';
            ctx.fillRect(c * cellSize, r * cellSize, cellSize, cellSize);
            ctx.strokeStyle = '#1e293b';
            ctx.strokeRect(c * cellSize, r * cellSize, cellSize, cellSize);
        }
    }
}

async function fetchAndDrawMap(mapPath) {
    const res = await fetch('/' + mapPath);
    const text = await res.text();
    
    mapData = [];
    const lines = text.trim().split('\n');
    lines.forEach(line => {
        const row = line.trim().split(/\s+/).map(Number);
        if (row.length > 0 && !isNaN(row[0])) mapData.push(row);
    });

    const rows = mapData.length;
    const cols = mapData[0].length;
    
    cellSize = Math.min(canvas.width / cols, canvas.height / rows);
    drawBaseMap();
}

async function prepareDeployment() {
    const mapFile = document.getElementById('mapSelect').value;
    
    let batteries = [];
    
    if (selectedScenario === 4) {
        // Warehouse: Get from the two robot inputs
        const b1 = parseInt(document.getElementById('robot1Battery').value) || 0;
        const b2 = parseInt(document.getElementById('robot2Battery').value) || 0;
        batteries.push(`${b1} 0`, `${b2} 0`); // Pass them as two packs with 0 weight
    } else {
        // Normal scenarios: Get from dynamic packs
        const energyInputs = document.querySelectorAll('.pack-energy-input');
        const weightInputs = document.querySelectorAll('.pack-weight-input');
        
        for (let i = 0; i < energyInputs.length; i++) {
            const e = parseInt(energyInputs[i].value) || 0;
            const w = parseInt(weightInputs[i].value) || 0;
            batteries.push(`${e} ${w}`);
        }
    }

    if (batteries.length === 0) {
        alert("Please assign at least 1 resource pack/battery before deploying.");
        return;
    }

    let customData = null;
    if (selectedScenario === 5) {
        customData = {
            agents: parseInt(document.getElementById('customAgents').value) || 2,
            prox: parseInt(document.getElementById('customProx').value) || 1,
            cost: parseInt(document.getElementById('customCost').value) || 2
        };
    }

    const btn = document.getElementById('deployBtn');
    btn.disabled = true;
    btn.innerText = "EXECUTING DEPLOYMENT...";

    const step3 = document.getElementById('step-3-tracking');
    step3.classList.remove('hidden');
    step3.scrollIntoView({ behavior: 'smooth' });

    logBox.innerHTML = '';
    writeLog("Establishing connection to C++ Engine...", "log-system");
    
    activeRobots = {};
    await fetchAndDrawMap(mapFile);

    try {
        const response = await fetch('/api/run-simulation', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ 
                scenario: selectedScenario, 
                mapFile, 
                batteries,
                customSettings: customData
            })
        });

        const data = await response.json();
        if (data.error) {
            writeLog(`ENGINE ERROR: ${data.error}`, 'log-error');
            btn.disabled = false;
            btn.innerText = "RE-DEPLOY SENTINEL";
            return;
        }

        parseAndAnimate(data.output);

    } catch (err) {
        writeLog(`Server Connection Failed: ${err.message}`, 'log-error');
        btn.disabled = false;
        btn.innerText = "RE-DEPLOY SENTINEL";
    }
}

function parseAndAnimate(outputText) {
    const lines = outputText.split('\n');
    let queue = [];
    let bestPathCoords = []; 

    lines.forEach(line => {
        line = line.trim();
        if (!line || line.startsWith('[GRID]')) return;
        
        if (line.startsWith('[STEP]')) {
            const parts = line.split(' ');
            // Handle both SingleAgent [STEP] r c AND MultiAgent [STEP] ID r c
            let id = "R1";
            let r, c;
            if (parts.length === 4) {
                id = parts[1];
                r = parseInt(parts[2]);
                c = parseInt(parts[3]);
            } else {
                r = parseInt(parts[1]);
                c = parseInt(parts[2]);
            }
            
            queue.push({ type: 'log', text: `[${id}] Moved to coordinate [${r}, ${c}]`, css: '' });
            queue.push({ type: 'move', id: id, r: r, c: c });
        } 
        else if (line.startsWith('[HAZARD]')) {
            const parts = line.split(' ', 3); 
            let id = (parts.length > 1 && parts[1].startsWith('R')) ? parts[1] : 'SYSTEM';
            let msg = line.replace(`[HAZARD] ${id} `, '').replace('[HAZARD] ', '');
            queue.push({ type: 'log', text: `[${id}] ${msg}`, css: 'log-error' });
        }
        else if (line.startsWith('[ACTION]')) {
            const parts = line.split(' ', 3); 
            let id = (parts.length > 1 && parts[1].startsWith('R')) ? parts[1] : 'SYSTEM';
            let msg = line.replace(`[ACTION] ${id} `, '').replace('[ACTION] ', '');
            queue.push({ type: 'log', text: `[${id}] ${msg}`, css: 'log-action' });
        }
        else if (line.startsWith('[KNAPSACK]')) {
            queue.push({ type: 'log', text: line.replace('[KNAPSACK] ', ''), css: 'log-success' });
        }
        else if (line.startsWith('[BEST_PATH]')) {
            const parts = line.split(' ');
            bestPathCoords.push({ r: parseInt(parts[1]), c: parseInt(parts[2]) });
        }
        else if (line.startsWith('[SUCCESS]') || line.startsWith('[FAILED]')) {
            queue.push({ type: 'log', text: line, css: line.startsWith('[SUCCESS]') ? 'log-success' : 'log-error' });
        } 
        else if (line.startsWith('[LOG]')) {
            queue.push({ type: 'log', text: line.replace('[LOG] ', ''), css: 'log-system' });
        } 
        else {
            queue.push({ type: 'log', text: line, css: 'log-success' }); 
        }
    });

    function nextFrame() {
        if (queue.length === 0) {
            if (bestPathCoords.length > 0) {
                writeLog("Locking in optimal route. Highlighting path in green.", "log-success");
                bestPathCoords.forEach(coord => {
                    ctx.fillStyle = '#22c55e'; 
                    ctx.fillRect(coord.c * cellSize + 2, coord.r * cellSize + 2, cellSize - 4, cellSize - 4);
                });
            }

            writeLog("Simulation Sequence Complete.", 'log-system');
            document.getElementById('deployBtn').disabled = false;
            document.getElementById('deployBtn').innerText = "RE-DEPLOY SENTINEL";
            return;
        }
        
        const frame = queue.shift();
        
        if (frame.type === 'log') {
            writeLog(frame.text, frame.css);
            setTimeout(nextFrame, 20); 
        } else {
            if (!activeRobots[frame.id]) {
                activeRobots[frame.id] = { trail: [] };
            }
            
            activeRobots[frame.id].trail.push({r: frame.r, c: frame.c});
            activeRobots[frame.id].r = frame.r;
            activeRobots[frame.id].c = frame.c;

            drawBaseMap();

            Object.keys(activeRobots).forEach(id => {
                ctx.fillStyle = '#3b82f6'; 
                activeRobots[id].trail.forEach(pos => {
                    ctx.fillRect(pos.c * cellSize + (cellSize/4), pos.r * cellSize + (cellSize/4), cellSize/2, cellSize/2);
                });
            });

            Object.keys(activeRobots).forEach(id => {
                const pos = activeRobots[id];
                ctx.fillStyle = robotColors[id] || "#10b981"; // Fallback to green
                
                ctx.beginPath();
                ctx.arc(pos.c * cellSize + (cellSize/2), pos.r * cellSize + (cellSize/2), cellSize/3, 0, Math.PI * 2);
                ctx.fill();
            });

            setTimeout(() => {
                nextFrame();
            }, 80); 
        }
    }

    nextFrame();
}