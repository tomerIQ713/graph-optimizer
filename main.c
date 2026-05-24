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

void assignOrderPriorities(int* orderPriorities, int numOrders) {
    if (orderPriorities == NULL || numOrders <= ZERO) return;

    printf("\n=== ORDER PRIORITY ASSIGNMENT ===\n");
    printf("Assigning priorities to %d orders...\n\n", numOrders);


    for (int i = ZERO; i < numOrders; i++) {
        if (i == ZERO) {
            orderPriorities[i] = HIGH_PRIORITY;
            printf("Order %d: HIGH PRIORITY (multiplier: %.1f)\n", i, HIGH_PRIORITY_WEIGHT);
        }
        else if (i < numOrders - ONE) {
            orderPriorities[i] = MEDIUM_PRIORITY;
            printf("Order %d: MEDIUM PRIORITY (multiplier: %.1f)\n", i, MEDIUM_PRIORITY_WEIGHT);
        }
        else {
            orderPriorities[i] = LOW_PRIORITY;
            printf("Order %d: LOW PRIORITY (multiplier: %.1f)\n", i, LOW_PRIORITY_WEIGHT);
        }
    }
    printf("\n");
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
    int* orderPriorities = mainGraph->orderPriorities;

    assignOrderPriorities(orderPriorities, numOrders);

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

    Graph* ordersGraph = buildOrdersGraph(map, infoCoordinates + TWO, numOrders, infoCoordinates[ONE], orderPriorities);

    mainRoute = getMostEfficientDeliveryWay(ordersGraph, numOrders, orderPriorities, numOrders);

    resultRoute = combineRoutes(map, restaurantRoute, mainRoute, orderVertices, restaurantVertex);

    exportVisualGraph(map, resultRoute, restaurantVertex, orderVertices, numOrders, orderPriorities);
    outputRoute(resultRoute);

    free(orderVertices);
    freeRoute(restaurantRoute);
    freeRoute(mainRoute);
    freeRoute(resultRoute);
    freeGraph(ordersGraph);
    freeMainGraph(mainGraph);

    return ZERO;
}
