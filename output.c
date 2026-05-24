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
    printf("\n");
}
const char* getPriorityLabel(int priority) {
    switch (priority) {
    case HIGH_PRIORITY:
        return "HIGH";
    case MEDIUM_PRIORITY:
        return "MEDIUM";
    case LOW_PRIORITY:
        return "LOW";
    default:
        return "UNKNOWN";
    }
}

const char* getPriorityColor(int priority) {
    switch (priority) {
    case HIGH_PRIORITY:
        return "#ff1111";
    case MEDIUM_PRIORITY:
        return "#ffaa00";
    case LOW_PRIORITY:
        return "#cccccc";
    default:
        return "#ffffff";
    }
}

void exportVisualGraph(Graph* map, Route* finalRoute,
    int restaurantVertex,
    int* orderVertices,
    int numOrders,
    int* orderPriorities)
{
    FILE* htmlFile = fopen("map_view.html", "w");

    if (htmlFile == NULL) {
        printf("[Error] Could not generate map visualizer file.\n");
        return;
    }

    fprintf(htmlFile, "<!DOCTYPE html>\n");
    fprintf(htmlFile, "<html>\n<head>\n");
    fprintf(htmlFile, "<title>Priority-Aware Delivery Route Graph Visualizer</title>\n");

    fprintf(htmlFile, "<style>\n");
    fprintf(htmlFile, "body {\n");
    fprintf(htmlFile, "  margin: 0;\n");
    fprintf(htmlFile, "  background-color: #1a1a1a;\n");
    fprintf(htmlFile, "  color: white;\n");
    fprintf(htmlFile, "  font-family: sans-serif;\n");
    fprintf(htmlFile, "  display: flex;\n");
    fprintf(htmlFile, "  flex-direction: column;\n");
    fprintf(htmlFile, "  align-items: center;\n");
    fprintf(htmlFile, "}\n");

    fprintf(htmlFile, "canvas {\n");
    fprintf(htmlFile, "  border: 2px solid #333;\n");
    fprintf(htmlFile, "  background-color: #111;\n");
    fprintf(htmlFile, "  margin-top: 20px;\n");
    fprintf(htmlFile, "  box-shadow: 0 4px 10px rgba(0,0,0,0.5);\n");
    fprintf(htmlFile, "}\n");

    fprintf(htmlFile, ".controls {\n");
    fprintf(htmlFile, "  margin-top: 15px;\n");
    fprintf(htmlFile, "  font-size: 14px;\n");
    fprintf(htmlFile, "  color: #aaa;\n");
    fprintf(htmlFile, "  margin-bottom: 15px;\n");
    fprintf(htmlFile, "}\n");

    fprintf(htmlFile, ".priority-legend {\n");
    fprintf(htmlFile, "  display: flex;\n");
    fprintf(htmlFile, "  gap: 20px;\n");
    fprintf(htmlFile, "  margin-top: 10px;\n");
    fprintf(htmlFile, "}\n");

    fprintf(htmlFile, ".priority-item {\n");
    fprintf(htmlFile, "  display: flex;\n");
    fprintf(htmlFile, "  align-items: center;\n");
    fprintf(htmlFile, "  gap: 8px;\n");
    fprintf(htmlFile, "}\n");

    fprintf(htmlFile, ".priority-dot {\n");
    fprintf(htmlFile, "  width: 12px;\n");
    fprintf(htmlFile, "  height: 12px;\n");
    fprintf(htmlFile, "  border-radius: 50%%;\n");
    fprintf(htmlFile, "}\n");

    fprintf(htmlFile, "</style>\n");
    fprintf(htmlFile, "</head>\n<body>\n");

    fprintf(htmlFile, "<h2>Priority-Aware Delivery Network Route</h2>\n");

    fprintf(htmlFile,
        "<canvas id='mapCanvas' width='900' height='600'></canvas>\n");

    fprintf(htmlFile, "<div class='controls'>\n");

    fprintf(htmlFile,
        "<div><b>Legend:</b> "
        "Grey Arrows = Roads | "
        "<span style='color:#ff9900'>━━</span> Driver to Restaurant | "
        "<span style='color:#00ffcc'>━━</span> Restaurant to Orders | "
        "<span style='color:#ff3333'>●</span> Delivery Orders</div>\n");

    fprintf(htmlFile, "<div class='priority-legend'>\n");

    fprintf(htmlFile,
        "<div class='priority-item'>"
        "<div class='priority-dot' style='background-color:#ff1111;'></div>"
        "High Priority"
        "</div>\n");

    fprintf(htmlFile,
        "<div class='priority-item'>"
        "<div class='priority-dot' style='background-color:#ffaa00;'></div>"
        "Medium Priority"
        "</div>\n");

    fprintf(htmlFile,
        "<div class='priority-item'>"
        "<div class='priority-dot' style='background-color:#cccccc;'></div>"
        "Low Priority"
        "</div>\n");

    fprintf(htmlFile, "</div>\n");
    fprintf(htmlFile, "</div>\n");

    fprintf(htmlFile, "<script>\n");

    fprintf(htmlFile,
        "const canvas = document.getElementById('mapCanvas');\n");

    fprintf(htmlFile,
        "const ctx = canvas.getContext('2d');\n");

    fprintf(htmlFile,
        "const restVertexId = %d;\n",
        restaurantVertex);

    // =========================
    // EXPORT NODES
    // =========================

    fprintf(htmlFile, "const nodes = [\n");

    for (int i = 0; i < map->numVertices; i++) {
        fprintf(htmlFile,
            "  { id:%d, x:%f, y:%f },\n",
            i,
            map->vertexCoordinates[i].x,
            map->vertexCoordinates[i].y);
    }

    fprintf(htmlFile, "];\n\n");

    // =========================
    // EXPORT ROADS
    // =========================

    fprintf(htmlFile, "const roads = [\n");

    for (int i = 0; i < map->numVertices; i++) {

        Node* current = map->adjLists[i];

        while (current != NULL) {

            fprintf(htmlFile,
                "  { from:%d, to:%d },\n",
                i,
                current->vertex);

            current = current->next;
        }
    }

    fprintf(htmlFile, "];\n\n");

    // =========================
    // EXPORT ORDER NODES
    // =========================

    fprintf(htmlFile, "const orderNodes = [\n");

    for (int i = 0; i < numOrders; i++) {

        const char* priorityColor =
            getPriorityColor(orderPriorities[i]);

        fprintf(htmlFile,
            "  { id:%d, vertex:%d, priority:%d, color:'%s' }%s\n",
            i,
            orderVertices[i],
            orderPriorities[i],
            priorityColor,
            (i == numOrders - 1) ? "" : ",");
    }

    fprintf(htmlFile, "];\n\n");

    // =========================
    // EXPORT ROUTE
    // =========================

    fprintf(htmlFile, "const routePath = [");

    if (finalRoute != NULL) {

        for (int i = 0; i < finalRoute->totalPathLength; i++) {

            fprintf(htmlFile,
                "%d%s",
                finalRoute->pathVertices[i],
                (i == finalRoute->totalPathLength - 1)
                ? ""
                : ", ");
        }
    }

    fprintf(htmlFile, "];\n\n");

    // =========================
    // SCALING
    // =========================

    fprintf(htmlFile,
        "const xs = nodes.map(n => n.x);\n");

    fprintf(htmlFile,
        "const ys = nodes.map(n => n.y);\n");

    fprintf(htmlFile,
        "const minX = Math.min(...xs);\n");

    fprintf(htmlFile,
        "const maxX = Math.max(...xs);\n");

    fprintf(htmlFile,
        "const minY = Math.min(...ys);\n");

    fprintf(htmlFile,
        "const maxY = Math.max(...ys);\n");

    fprintf(htmlFile,
        "function scale(x, y) {\n"
        "  const px = 50 + ((x - minX) / (maxX - minX || 1)) * 800;\n"
        "  const py = 550 - ((y - minY) / (maxY - minY || 1)) * 500;\n"
        "  return { x: px, y: py };\n"
        "}\n\n");

    // =========================
    // DRAW ARROW FUNCTION
    // =========================

    fprintf(htmlFile,
        "function drawArrow(x1, y1, x2, y2, color, width) {\n"

        "  const dx = x2 - x1;\n"
        "  const dy = y2 - y1;\n"

        "  const angle = Math.atan2(dy, dx);\n"

        "  const midX = (x1 + x2) / 2;\n"
        "  const midY = (y1 + y2) / 2;\n"

        "  const arrowSize = 8;\n"

        "  ctx.beginPath();\n"
        "  ctx.moveTo(x1, y1);\n"
        "  ctx.lineTo(x2, y2);\n"
        "  ctx.strokeStyle = color;\n"
        "  ctx.lineWidth = width;\n"
        "  ctx.stroke();\n"

        "  ctx.beginPath();\n"

        "  ctx.moveTo(midX, midY);\n"

        "  ctx.lineTo(\n"
        "    midX - arrowSize * Math.cos(angle - Math.PI / 6),\n"
        "    midY - arrowSize * Math.sin(angle - Math.PI / 6)\n"
        "  );\n"

        "  ctx.lineTo(\n"
        "    midX - arrowSize * Math.cos(angle + Math.PI / 6),\n"
        "    midY - arrowSize * Math.sin(angle + Math.PI / 6)\n"
        "  );\n"

        "  ctx.closePath();\n"

        "  ctx.fillStyle = color;\n"
        "  ctx.fill();\n"

        "}\n\n");

    // =========================
    // DRAW ROAD NETWORK
    // =========================

    fprintf(htmlFile,
        "roads.forEach(r => {\n"

        "  const p1 = scale(nodes[r.from].x, nodes[r.from].y);\n"

        "  const p2 = scale(nodes[r.to].x, nodes[r.to].y);\n"

        "  drawArrow(p1.x, p1.y, p2.x, p2.y, '#3a3a3a', 1.5);\n"

        "});\n\n");

    // =========================
    // DRAW INTERSECTIONS
    // =========================

    fprintf(htmlFile,
        "nodes.forEach(n => {\n"

        "  const p = scale(n.x, n.y);\n"

        "  ctx.beginPath();\n"

        "  ctx.arc(p.x, p.y, 3, 0, 2*Math.PI);\n"

        "  ctx.fillStyle = '#666';\n"

        "  ctx.fill();\n"

        "});\n\n");

    // =========================
    // DRAW ROUTE
    // =========================

    fprintf(htmlFile,
        "if (routePath.length > 1) {\n"

        "  let restIdx = routePath.indexOf(restVertexId);\n"

        "  if (restIdx === -1) restIdx = 0;\n"

        "  if (restIdx > 0) {\n"

        "    for(let i = 0; i < restIdx; i++) {\n"

        "      const p1 = scale(nodes[routePath[i]].x, nodes[routePath[i]].y);\n"

        "      const p2 = scale(nodes[routePath[i+1]].x, nodes[routePath[i+1]].y);\n"

        "      drawArrow(p1.x, p1.y, p2.x, p2.y, '#ff9900', 4);\n"

        "    }\n"

        "  }\n"

        "  for(let i = restIdx; i < routePath.length - 1; i++) {\n"

        "    const p1 = scale(nodes[routePath[i]].x, nodes[routePath[i]].y);\n"

        "    const p2 = scale(nodes[routePath[i+1]].x, nodes[routePath[i+1]].y);\n"

        "    drawArrow(p1.x, p1.y, p2.x, p2.y, '#00ffcc', 4);\n"

        "  }\n");

    // =========================
    // DRAW ROUTE NODES
    // =========================

    fprintf(htmlFile,
        "  routePath.forEach((nodeId, idx) => {\n"

        "    const p = scale(nodes[nodeId].x, nodes[nodeId].y);\n"

        "    let orderInfo = orderNodes.find(o => o.vertex === nodeId);\n"

        "    let isOrder = orderInfo !== undefined;\n"

        "    ctx.beginPath();\n"

        "    ctx.arc(\n"
        "      p.x,\n"
        "      p.y,\n"
        "      (isOrder || nodeId === restVertexId || idx === 0) ? 7 : 4,\n"
        "      0,\n"
        "      2*Math.PI\n"
        "    );\n"

        "    if (idx === 0)\n"
        "      ctx.fillStyle = '#ff3366';\n"

        "    else if (nodeId === restVertexId)\n"
        "      ctx.fillStyle = '#ff9900';\n"

        "    else if (isOrder)\n"
        "      ctx.fillStyle = orderInfo.color;\n"

        "    else\n"
        "      ctx.fillStyle = '#00ffcc';\n"

        "    ctx.fill();\n"

        "  });\n"

        "}\n");

    fprintf(htmlFile, "</script>\n");
    fprintf(htmlFile, "</body>\n");
    fprintf(htmlFile, "</html>\n");

    fclose(htmlFile);

    printf("[Map Generated] Open 'map_view.html' in your browser!\n");
}
