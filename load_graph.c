
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

    char line[256];
    int numVertices = 0;
    int numOrders = 0;

    // 1. Read Metadata configuration lines
    while (fgets(line, sizeof(line), file)) {
        // Skip comment lines and empty layout rows safely
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        if (sscanf(line, "%d %d", &numVertices, &numOrders) == 2) {
            break;
        }
    }

    mainGraph->numOrders = numOrders;

    // Allocate the underlying sub-graph data framework
    mainGraph->graph = (Graph*)malloc(sizeof(Graph));
    if (mainGraph->graph == NULL) {
        freeMainGraph(mainGraph);
        fclose(file);
        return NULL;
    }

    mainGraph->graph->numVertices = numVertices;
    mainGraph->graph->adjLists = (Node**)calloc(numVertices, sizeof(Node*));
    mainGraph->graph->vertexCoordinates = (Coordinate*)malloc(numVertices * sizeof(Coordinate));

    // Allocate the Special Coordinates tracker array
    // Size = Driver (1) + Restaurant (1) + Orders (numOrders)
    int totalInfoCoords = TWO + numOrders;
    mainGraph->infoCoordinates = (Coordinate*)malloc(totalInfoCoords * sizeof(Coordinate));

    if (mainGraph->graph->adjLists == NULL ||
        mainGraph->graph->vertexCoordinates == NULL ||
        mainGraph->infoCoordinates == NULL) {
        freeMainGraph(mainGraph);
        fclose(file);
        return NULL;
    }

    // 2. Load Special Information Positions array
    int infoCount = 0;
    while (infoCount < totalInfoCoords && fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        if (sscanf(line, "%lf %lf", &mainGraph->infoCoordinates[infoCount].x,
            &mainGraph->infoCoordinates[infoCount].y) == 2) {
            infoCount++;
        }
    }

    // 3. Load Base Map Physical Intersections Positions
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

    // 4. Load Road Network Interconnections Map
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int src, dest, weight;
        if (sscanf(line, "%d %d %d", &src, &dest, &weight) == 3) {
            if (src >= 0 && src < numVertices && dest >= 0 && dest < numVertices) {

                // Add forward edge path (src -> dest)
                Node* newNode1 = (Node*)malloc(sizeof(Node));
                if (newNode1 != NULL) {
                    newNode1->vertex = dest;
                    newNode1->weight = weight;
                    newNode1->next = mainGraph->graph->adjLists[src];
                    mainGraph->graph->adjLists[src] = newNode1;
                }

                // Add reversed edge path for undirected maps (dest -> src)
                Node* newNode2 = (Node*)malloc(sizeof(Node));
                if (newNode2 != NULL) {
                    newNode2->vertex = src;
                    newNode2->weight = weight;
                    newNode2->next = mainGraph->graph->adjLists[dest];
                    mainGraph->graph->adjLists[dest] = newNode2;
                }
            }
        }
    }

    fclose(file);
    return mainGraph;
}