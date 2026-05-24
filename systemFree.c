#include "maps_common.h"


void freeGraph(Graph* graph) {
    if (graph == NULL) return;

    for (int i = ZERO; i < graph->numVertices; i++) {
        Node* current = graph->adjLists[i];
        while (current != NULL) {
            Node* nextNode = current->next;
            free(current); 
            current = nextNode;
        }
    }

    free(graph->adjLists);
    free(graph->vertexCoordinates);

    free(graph);
}

void freeRoute(Route* route) {
    if (route == NULL) return;

    if (route->pathVertices != NULL) {
        free(route->pathVertices);
    }

    free(route);
}

void freeMainGraph(MainGraph* mainGraph) {
    if (mainGraph == NULL) return;

    if (mainGraph->graph != NULL) {
        freeGraph(mainGraph->graph);
    }

    if (mainGraph->infoCoordinates != NULL) {
        free(mainGraph->infoCoordinates);
    }

    free(mainGraph);
}