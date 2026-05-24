#include "maps_common.h"

MainGraph* loadAreaGraph(char* fileName) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", fileName);
        return NULL;
    }

    // Allocation of the Master MainGraph wrapper
    MainGraph* mainGraph = (MainGraph*)malloc(sizeof(MainGraph));
    if (mainGraph == NULL) {
        fclose(file);
        return NULL;
    }

    // Safety initialization
    mainGraph->graph = NULL;
    mainGraph->infoCoordinates = NULL;
    mainGraph->numOrders = 0;
    mainGraph->orderPriorities = NULL;

    char line[256];
    int numVertices = 0;
    int numOrders = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        if (sscanf(line, "%d %d", &numVertices, &numOrders) == 2) {
            break;
        }
    }

    mainGraph->numOrders = numOrders;

    mainGraph->graph = (Graph*)malloc(sizeof(Graph));
    if (mainGraph->graph == NULL) {
        freeMainGraph(mainGraph);
        fclose(file);
        return NULL;
    }

    mainGraph->graph->numVertices = numVertices;
    mainGraph->graph->adjLists = (Node**)calloc(numVertices, sizeof(Node*));
    mainGraph->graph->vertexCoordinates = (Coordinate*)malloc(numVertices * sizeof(Coordinate));

    // Size = Driver (1) + Restaurant (1) + Orders (numOrders)
    int totalInfoCoords = TWO + numOrders;
    mainGraph->infoCoordinates = (Coordinate*)malloc(totalInfoCoords * sizeof(Coordinate));

    // Allocate priority array for orders
    mainGraph->orderPriorities = (int*)malloc(numOrders * sizeof(int));

    if (mainGraph->graph->adjLists == NULL ||
        mainGraph->graph->vertexCoordinates == NULL ||
        mainGraph->infoCoordinates == NULL ||
        mainGraph->orderPriorities == NULL) {
        freeMainGraph(mainGraph);
        fclose(file);
        return NULL;
    }

    // Initialize priorities: default all to LOW_PRIORITY
    for (int i = 0; i < numOrders; i++) {
        mainGraph->orderPriorities[i] = LOW_PRIORITY;
    }

    int infoCount = 0;
    while (infoCount < totalInfoCoords && fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        if (sscanf(line, "%lf %lf", &mainGraph->infoCoordinates[infoCount].x,
            &mainGraph->infoCoordinates[infoCount].y) == 2) {
            infoCount++;
        }
    }

    int vertexCount = 0;
    while (vertexCount < numVertices && fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int vId;
        double xVal, yVal;
        if (sscanf(line, "%d %lf %lf", &vId, &xVal, &yVal) == 3) {
            if (vId >= 0 && vId < numVertices) {
                mainGraph->graph->vertexCoordinates[vId].x = xVal;
                mainGraph->graph->vertexCoordinates[vId].y = yVal;
                vertexCount++;
            }
        }
    }

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int src, dest, weight;
        if (sscanf(line, "%d %d %d", &src, &dest, &weight) == 3) {
            if (src >= 0 && src < numVertices && dest >= 0 && dest < numVertices) {

                // Add ONLY the forward directed edge path (src -> dest)
                Node* newNode = (Node*)malloc(sizeof(Node));
                if (newNode != NULL) {
                    newNode->vertex = dest;
                    newNode->weight = weight;
                    newNode->next = mainGraph->graph->adjLists[src];
                    mainGraph->graph->adjLists[src] = newNode;
                }

            }
        }
    }

    fclose(file);
    return mainGraph;
}
