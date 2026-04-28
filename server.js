
const express = require('express');
const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');

const app = express();
const PORT = 3000;

app.use(express.json());
// Serve the website files
app.use(express.static(path.join(__dirname, 'public')));
// Allow the website to download the actual text files to draw the grid later
app.use('/maps', express.static(path.join(__dirname, 'maps')));

// --- THE MAP SCANNER ENDPOINT ---
app.get('/api/get-maps', (req, res) => {
    const mapsDir = path.join(__dirname, 'maps');
    console.log(`[SERVER] Website requested maps. Looking in: ${mapsDir}`);
    
    fs.readdir(mapsDir, (err, files) => {
        if (err) {
            console.error("[SERVER ERROR] Could not find the maps folder! Check your folder structure.");
            return res.json([]); 
        }
        
        const txtFiles = files.filter(f => f.endsWith('.txt'));
        console.log(`[SERVER] Success! Found ${txtFiles.length} maps:`, txtFiles);
        res.json(txtFiles);
    });
});

// --- THE C++ ENGINE ENDPOINT ---
app.post('/api/run-simulation', (req, res) => {
    const { scenario, mapFile, batteries, customSettings } = req.body;
    
    // 1. Safely translate string names to C++ Integers
    const scenarioMap = {
        "FIRE": 1, "FOREST": 2, "MINE": 3, "WAREHOUSE": 4, "CUSTOM": 5
    };
    const numericScenario = scenarioMap[scenario] || scenario;
    
    const args = [numericScenario, mapFile, ...batteries];
    const executablePath = path.join(__dirname, 'sentinel.exe');

    // 2. Secretly package the Custom overrides into Environment Variables
    const customEnv = { ...process.env }; // Copy system environment
    if (customSettings) {
        customEnv.CUSTOM_AGENTS = customSettings.agents;
        customEnv.CUSTOM_PROX = customSettings.prox;
        customEnv.CUSTOM_COST = customSettings.cost;
    }

    console.log(`[SERVER] Booting Engine: sentinel.exe ${args.join(' ')}`);

    // 3. Launch the C++ engine with the custom environment injected
    execFile(executablePath, args, { env: customEnv, maxBuffer: 1024 * 1024 * 50 }, (error, stdout, stderr) => {
        if (error) {
            console.error(`[SERVER ERROR] Engine failed: ${stderr || error.message}`);
            return res.status(500).json({ error: 'Engine failed to run', details: stderr || error.message });
        }
        res.json({ output: stdout });
    });
});

app.listen(PORT, () => {
    console.log(`=========================================`);
    console.log(`🚀 Sentinel Server Live: http://localhost:${PORT}`);
    console.log(`=========================================`);
});