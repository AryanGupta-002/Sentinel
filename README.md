# Project Sentinel: Autonomous Path Recovery System



**Developer:** Aryan Gupta  
**Institution:** Graphic Era University, Computer Science  
**Language:** C++17  

## 📌 Overview
Project Sentinel is a highly optimized, multi-algorithmic pathfinding engine designed for autonomous agents operating in hazardous environments. Rather than relying on a single pathfinding solution, the engine intelligently layers multiple classical algorithms to balance local survival constraints with global resource efficiency.

## ⚙️ Core Architecture & Algorithms
The system's "brain" is built on four interlocking mathematical modules:

1. **Multi-Source Backtracking (The Navigator):** - Explores dynamic grids recursively. It features a multi-source loop capable of auto-detecting multiple entry/exit points. If Entry A is blocked by terrain, the agent physically redeploys to Entry B.
   
2. **Modified N-Queens (The Hazard Scanner):** - Evaluates dynamic spatial proximity to hazardous terrain (e.g., toxic gas, fire). It dynamically enforces custom safety perimeters based on the specific mission scenario.

3. **Subset Sum (The Micro-Bouncer):** - Acts as a real-time capability gate. Before stepping onto penalty terrain, the algorithm checks the agent's exact physical inventory to ensure it has the specific discrete resources required to survive the step.

4. **0/1 Knapsack (The Quartermaster):** - Calculates the globally optimal equipment loadout. Once a safe path is charted, it analyzes the warehouse inventory and prints the most mathematically efficient combination of batteries/oxygen packs to survive the trip without over-encumbering the agent.

## 🚀 Features
- **Dynamic Grid Mapping:** Auto-parses `.txt` files to map walls, hazards, and borders.
- **Scenario Profiles:** 5 custom rule sets (Fire, Forest, Mine, Warehouse, Custom) that alter traversal rules and battery drain rates on the fly.
- **Aggressive Pruning:** Calculates the absolute hardware limits of the agent prior to deployment to aggressively prune unviable recursion paths, protecting CPU memory on massive (50x50+) grids.
- **Live Terminal Visualizer:** A cross-platform rendering engine that displays the agent's memory stack, path history, and backtracking attempts in real-time at 500ms intervals.

## 🛠️ Compilation & Execution

Compile the architecture using a modern C++17 compiler:

```bash
g++ -std=c++17 src/main.cpp src/GridMap.cpp src/Agent.cpp src/PathEngine.cpp src/ScenarioManager.cpp src/Visualizer.cpp src/algos/KnapsackLogic.cpp src/algos/NQueensLogic.cpp src/algos/SubsetSumLogic.cpp src/algos/Backtracking.cpp -o sentinel
