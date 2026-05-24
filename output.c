#include <stdio.h>
#include "maps_common.h"

void outputRoute(Route* resultRoute) {
    if (resultRoute == NULL) {
        printf("Error: Cannot display route. Route pointer is NULL.\n");
        return;
    }

    if (resultRoute->totalPathLength <= ZERO || resultRoute->pathVertices == NULL) {
        printf("Error: Route is empty or has no path data.\n");
        return;
    }

    printf(" Total Route Traversal Cost: %d\n", resultRoute->totalDistance);
    printf(" Total Map Checkpoints Visited: %d\n", resultRoute->totalPathLength);
    printf(" Directions:\n ");

    for (int i = ZERO; i < resultRoute->totalPathLength; i++) {
        printf("%d", resultRoute->pathVertices[i]);

        if (i < resultRoute->totalPathLength - ONE) {
            printf(" -> ");
        }
    }
}

void exportVisualGraph(Graph* map, Route* finalRoute) {
    FILE* htmlFile = fopen("map_view.html", "w");
    if (htmlFile == NULL) {
        printf("[Error] Could not generate map visualizer file.\n");
        return;
    }

    // Write HTML Shell and SVG/Canvas setup scripts
    fprintf(htmlFile, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(htmlFile, "<title>Delivery Route Graph Visualizer</title>\n");
    fprintf(htmlFile, "<style>\n");
    fprintf(htmlFile, "  body { margin: 0; background-color: #1a1a1a; color: white; font-family: sans-serif; display: flex; flex-direction: column; align-items: center; }\n");
    fprintf(htmlFile, "  canvas { border: 2px solid #333; background-color: #111; margin-top: 20px; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }\n");
    fprintf(htmlFile, "  .controls { margin-top: 15px; font-size: 14px; color: #aaa; }\n");
    fprintf(htmlFile, "</style>\n</head>\n<body>\n");
    fprintf(htmlFile, "<h2>Delivery Network Road Map</h2>\n");
    fprintf(htmlFile, "<canvas id='mapCanvas' width='900' height='600'></canvas>\n");
    fprintf(htmlFile, "<div class='controls'><b>Legend:</b> Grey Lines = Roads | Grey Dots = Intersections | <span style='color:#00ffcc'>●</span> Neon = Path Route</div>\n");

    fprintf(htmlFile, "<script>\nconst canvas = document.getElementById('mapCanvas');\nconst ctx = canvas.getContext('2d');\n");

    // 1. Export Vertex Coordinates Array
    fprintf(htmlFile, "const nodes = [\n");
    for (int i = 0; i < map->numVertices; i++) {
        fprintf(htmlFile, "  { id: %d, x: %f, y: %f },\n", i, map->vertexCoordinates[i].x, map->vertexCoordinates[i].y);
    }
    fprintf(htmlFile, "];\n\nconst roads = [\n");

    // 2. Safely traverse your Linked Adjacency Lists to export road lines
    for (int i = 0; i < map->numVertices; i++) {
        Node* current = map->adjLists[i];
        while (current != NULL) {
            // Avoid printing duplicate bi-directional lines if i > neighbor
            if (i < current->vertex) {
                fprintf(htmlFile, "  { from: %d, to: %d },\n", i, current->vertex);
            }
            current = current->next;
        }
    }
    fprintf(htmlFile, "];\n\n");

    // 3. Pass the final route trajectory indexes
    fprintf(htmlFile, "const routePath = [");
    if (finalRoute != NULL) {
        for (int i = 0; i < finalRoute->totalPathLength; i++) {
            fprintf(htmlFile, "%d%s", finalRoute->pathVertices[i], (i == finalRoute->totalPathLength - 1) ? "" : ", ");
        }
    }
    fprintf(htmlFile, "];\n\n");

    // Dynamic Scaling Geometry Logic
    fprintf(htmlFile, "const xs = nodes.map(n => n.x); const ys = nodes.map(n => n.y);\n");
    fprintf(htmlFile, "const minX = Math.min(...xs); const maxX = Math.max(...xs);\n");
    fprintf(htmlFile, "const minY = Math.min(...ys); const maxY = Math.max(...ys);\n");
    fprintf(htmlFile, "function scale(x, y) {\n");
    fprintf(htmlFile, "  const px = 50 + ((x - minX) / (maxX - minX || 1)) * 800;\n");
    fprintf(htmlFile, "  const py = 550 - ((y - minY) / (maxY - minY || 1)) * 500;\n");
    fprintf(htmlFile, "  return { x: px, y: py };\n}\n\n");

    // Draw Background Roads Map Network Layout
    fprintf(htmlFile, "roads.forEach(r => {\n");
    fprintf(htmlFile, "  const p1 = scale(nodes[r.from].x, nodes[r.from].y);\n");
    fprintf(htmlFile, "  const p2 = scale(nodes[r.to].x, nodes[r.to].y);\n");
    fprintf(htmlFile, "  ctx.beginPath(); ctx.moveTo(p1.x, p1.y); ctx.lineTo(p2.x, p2.y);\n");
    fprintf(htmlFile, "  ctx.strokeStyle = '#3a3a3a'; ctx.lineWidth = 1.5; ctx.stroke();\n});\n\n");

    // Draw Intersections Nodes
    fprintf(htmlFile, "nodes.forEach(n => {\n");
    fprintf(htmlFile, "  const p = scale(n.x, n.y);\n");
    fprintf(htmlFile, "  ctx.beginPath(); ctx.arc(p.x, p.y, 3, 0, 2*Math.PI);\n");
    fprintf(htmlFile, "  ctx.fillStyle = '#666'; ctx.fill();\n});\n\n");

    // Overlay the Active Delivery Route Track Sequences
    fprintf(htmlFile, "if (routePath.length > 1) {\n");
    fprintf(htmlFile, "  ctx.beginPath();\n");
    fprintf(htmlFile, "  const startPoint = scale(nodes[routePath[0]].x, nodes[routePath[0]].y);\n");
    fprintf(htmlFile, "  ctx.moveTo(startPoint.x, startPoint.y);\n");
    fprintf(htmlFile, "  for(let i=1; i<routePath.length; i++) {\n");
    fprintf(htmlFile, "    const p = scale(nodes[routePath[i]].x, nodes[routePath[i]].y);\n");
    fprintf(htmlFile, "    ctx.lineTo(p.x, p.y);\n");
    fprintf(htmlFile, "  }\n");
    fprintf(htmlFile, "  ctx.strokeStyle = '#00ffcc'; ctx.lineWidth = 4; ctx.stroke();\n");

    // Color code checkpoints (Start = Pink, Mid-route = Neon, Destination = Yellow)
    fprintf(htmlFile, "  routePath.forEach((nodeId, idx) => {\n");
    fprintf(htmlFile, "    const p = scale(nodes[nodeId].x, nodes[nodeId].y);\n");
    fprintf(htmlFile, "    ctx.beginPath(); ctx.arc(p.x, p.y, idx === 0 || idx === routePath.length-1 ? 7 : 5, 0, 2*Math.PI);\n");
    fprintf(htmlFile, "    ctx.fillStyle = idx === 0 ? '#ff3366' : (idx === routePath.length-1 ? '#ffcc00' : '#00ffcc');\n");
    fprintf(htmlFile, "    ctx.fill();\n  });\n}\n");

    fprintf(htmlFile, "</script>\n</body>\n</html>\n");
    fclose(htmlFile);

    printf("[Map Generated] Open 'map_view.html' in your browser to view the graph network structure and road paths!\n");
}