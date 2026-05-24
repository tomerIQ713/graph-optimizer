#include "maps_common.h"  

#define EOD '\0'

int findClosestVertex(Graph* map, Coordinate coord) {
    if (map == NULL) return NEG_ONE;

    int closestVertexIndex = NEG_ONE;
    double minDistance = INFINITY_DIST;

    for (int i = ZERO; i < map->numVertices; i++) {
        double dx = map->vertexCoordinates[i].x - coord.x;
        double dy = map->vertexCoordinates[i].y - coord.y;
        double distance = sqrt((dx * dx) + (dy * dy));

        if (distance < minDistance) {
            minDistance = distance;
            closestVertexIndex = i;
        }
    }

    return closestVertexIndex;
}

int main()
{
    MainGraph* mainGraph = loadAreaGraph("graph.txt");
    if (mainGraph == NULL) {
        return ONE;
    }

    Graph* map = mainGraph->graph;
    Coordinate* infoCoordinates = mainGraph->infoCoordinates;
    int numOrders = mainGraph->numOrders;

    Route* restaurantRoute;
    Route* mainRoute;
    Route* resultRoute;

    restaurantRoute = bestRouteToRestaurant(map, infoCoordinates);
    int restaurantVertex = findClosestVertex(map, infoCoordinates[ONE]);

    int* orderVertices = (int*)malloc(numOrders * sizeof(int));
    if (orderVertices == NULL) {
        freeRoute(restaurantRoute);
        freeMainGraph(mainGraph);
        return ONE;
    }
    for (int i = ZERO; i < numOrders; i++) {
        orderVertices[i] = findClosestVertex(map, infoCoordinates[TWO + i]);
    }

    Graph* ordersGraph = buildOrdersGraph(map, infoCoordinates + TWO, numOrders, infoCoordinates[ONE]);

    int restaurantTSPIndex = numOrders;
    mainRoute = getMostEfficientDeliveryWay(ordersGraph, restaurantTSPIndex);

    resultRoute = combineRoutes(map, restaurantRoute, mainRoute, orderVertices, restaurantVertex);

    exportVisualGraph(map, restaurantRoute);
    outputRoute(resultRoute);

    free(orderVertices);
    freeRoute(restaurantRoute);
    freeRoute(mainRoute);
    freeRoute(resultRoute);
    freeGraph(ordersGraph);
    freeMainGraph(mainGraph);
    
    return ZERO;
}
 