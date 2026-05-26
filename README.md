# Graph Path Optimizer

A delivery route optimization system built in C. The project calculates the most efficient route for a delivery driver to pick up an order from a restaurant and deliver it to multiple destinations using a Traveling Salesperson Problem (TSP) approach.  
This setup demonstrates principles of graph theory, adjacency list memory structures, path-finding algorithms, and data parsing.

The application optimizes paths through a multi-stage routing pipeline:
1. **Best Route to Restaurant:** Uses path-finding to navigate the driver from their starting location to the kitchen coordinates.
2. **TSP Order Matrix:** Builds a dynamic graph focusing strictly on delivery order coordinates.
3. **Priority-Aware Delivery Optimization:** Solves the most efficient sequence while dynamically prioritizing urgent deliveries.
4. **Route Splicing:** Combines and outputs a unified, complete trajectory.

## Installation
- Download the repository files.
- Ensure you have a C compiler (like `gcc`) installed on your system.

**Compile the project using this command:**
```bash
gcc main.c -o graph_optimizer -lm
```

## Features
- Directed weighted road graph
- Priority-aware delivery routing
- Traveling Salesperson Problem (TSP) optimization
- Dynamic adjacency list graph structure
- Browser-based graph rendering
- Multi-stage path optimization pipeline

## Delivery Priority System

Orders are assigned delivery priorities:

- HIGH Priority
- MEDIUM Priority
- LOW Priority

The optimizer adjusts routing cost dynamically to prefer urgent deliveries first while still minimizing total travel distance.

## Directed Road System

The graph supports directional roads using adjacency lists.

This allows the simulation of:
- One-way streets
- Restricted traffic directions
- Realistic city routing
- Direction-sensitive shortest paths

Each road edge stores:
- Destination vertex
- Distance weight
- Direction

## Visual Graph Export System
The project has a visual browser-based representation of the road network and the calculated delivery route.

It receives:
- `Graph* map` — the full road graph
- `Route* finalRoute` — the optimized delivery path

and creates:
```html
map_view.html
```

## Priority-Aware Directed Route Visualization
The visualization displays:
- Directed\Undirected roads using inline arrows
- Priority-colored delivery nodes
- Optimized traversal order
- Distinct routing stages

Undirected Graph:
![Graph Screenshot](images/graph.png)

Directed Graph:
![Graph Screenshot](images/graph1.png)
