#include "maps_common.h"

// Helper function to calculate Euclidean distance between two coordinates
static double calculateDistance(Coordinate c1, Coordinate c2) {
    double dx = c1.x - c2.x;
    double dy = c1.y - c2.y;
    return sqrt(dx * dx + dy * dy);
}

// Helper function to find the unvisited vertex with minimum distance
static int findMinDistVertex(int numVertices, int* distances, int* visited) {
    int minDist = INFINITY_DIST;
    int minVertex = NEG_ONE;

    for (int i = ZERO; i < numVertices; i++) {
        if (!visited[i] && distances[i] < minDist) {
            minDist = distances[i];
            minVertex = i;
        }
    }

    return minVertex;
}

// Helper function to reconstruct path from parent array
static int* reconstructPath(int* parent, int targetVertex, int* pathLength) {
    int count = ZERO;
    int current = targetVertex;

    while (current != NEG_ONE) {
        count++;
        current = parent[current];
    }

    *pathLength = count;

    int* path = (int*)malloc(count * sizeof(int));
    if (path == NULL) {
        return NULL;
    }

    // Fill path in reverse order
    current = targetVertex;
    for (int i = count - ONE; i >= ZERO; i--) {
        path[i] = current;
        current = parent[current];
    }

    return path;
}

/* Find best path to the kitchen hub */
Route* bestRouteToRestaurant(Graph* mainGraph, Coordinate* infoCoordinates) {
    if (mainGraph == NULL || infoCoordinates == NULL) {
        return NULL;
    }

    int numVertices = mainGraph->numVertices;

    int startVertex = ZERO;
    double minDistToStart = INFINITY_DIST;
    for (int i = ZERO; i < numVertices; i++) {
        double dist = calculateDistance(infoCoordinates[ZERO], mainGraph->vertexCoordinates[i]);
        if (dist < minDistToStart) {
            minDistToStart = dist;
            startVertex = i;
        }
    }

    int endVertex = ZERO;
    double minDistToEnd = INFINITY_DIST;
    for (int i = ZERO; i < numVertices; i++) {
        double dist = calculateDistance(infoCoordinates[ONE], mainGraph->vertexCoordinates[i]);
        if (dist < minDistToEnd) {
            minDistToEnd = dist;
            endVertex = i;
        }
    }

    // Allocate arrays 
    int* distances = (int*)malloc(numVertices * sizeof(int));
    int* visited = (int*)malloc(numVertices * sizeof(int));
    int* parent = (int*)malloc(numVertices * sizeof(int));

    unsigned char allocFailed = distances == NULL || visited == NULL || parent == NULL;

    if (allocFailed) {
        if (distances) free(distances);
        if (visited) free(visited);
        if (parent) free(parent);
        return NULL;
    }

    // Initialize arrays
    for (int i = ZERO; i < numVertices; i++) {
        distances[i] = INFINITY_DIST;
        visited[i] = NOT_VISITED;
        parent[i] = NEG_ONE;
    }

    distances[startVertex] = ZERO;

    // Dijkstra's algorithm main loop
    for (int count = ZERO; count < numVertices - ONE; count++) {
        int currentVertex = findMinDistVertex(numVertices, distances, visited);

        if (currentVertex == NEG_ONE || distances[currentVertex] == INFINITY_DIST) {
            break;
        }

        visited[currentVertex] = VISITED;

        // Update distances for adjacent vertices
        Node* adjacentNode = mainGraph->adjLists[currentVertex];
        while (adjacentNode != NULL) {
            int adjacentVertex = adjacentNode->vertex;
            int edgeWeight = adjacentNode->weight;

            if (!visited[adjacentVertex] && distances[currentVertex] != INFINITY_DIST) {
                int newDistance = distances[currentVertex] + edgeWeight;

                if (newDistance < distances[adjacentVertex]) {
                    distances[adjacentVertex] = newDistance;
                    parent[adjacentVertex] = currentVertex;
                }
            }
            adjacentNode = adjacentNode->next;
        }
    }

    int pathLength = ZERO;
    int* pathVertices = reconstructPath(parent, endVertex, &pathLength);

    if (pathVertices == NULL) {
        free(distances);
        free(visited);
        free(parent);
        return NULL;
    }

    Route* route = (Route*)malloc(sizeof(Route));
    if (route == NULL) {
        free(pathVertices);
        free(distances);
        free(visited);
        free(parent);
        return NULL;
    }

    route->pathVertices = pathVertices;
    route->totalPathLength = pathLength;
    route->totalDistance = distances[endVertex];

    free(distances);
    free(visited);
    free(parent);

    return route;
}

// Helper: Find closest vertex to a coordinate
static int findClosestVertex(Graph* graph, Coordinate coord) {
    if (graph == NULL || graph->vertexCoordinates == NULL) {
        return NEG_ONE;
    }

    int closestVertex = ZERO;
    double minDistance = calculateDistance(coord, graph->vertexCoordinates[0]);

    for (int i = ONE; i < graph->numVertices; i++) {
        double dist = calculateDistance(coord, graph->vertexCoordinates[i]);
        if (dist < minDistance) {
            minDistance = dist;
            closestVertex = i;
        }
    }

    return closestVertex;
}

// Helper: Single-source Dijkstra from a given vertex
static void singleSourceDijkstra(Graph* graph, int sourceVertex, int* distances) {
    int numVertices = graph->numVertices;

    int* visited = (int*)malloc(numVertices * sizeof(int));
    if (visited == NULL) {
        return;
    }

    for (int i = ZERO; i < numVertices; i++) {
        distances[i] = INFINITY_DIST;
        visited[i] = NOT_VISITED;
    }

    distances[sourceVertex] = ZERO;

    for (int count = ZERO; count < numVertices - ONE; count++) {
        int currentVertex = findMinDistVertex(numVertices, distances, visited);

        if (currentVertex == NEG_ONE || distances[currentVertex] == INFINITY_DIST) {
            break;
        }

        visited[currentVertex] = VISITED;

        Node* adjacentNode = graph->adjLists[currentVertex];
        while (adjacentNode != NULL) {
            int adjacentVertex = adjacentNode->vertex;
            int edgeWeight = adjacentNode->weight;

            if (!visited[adjacentVertex] && distances[currentVertex] != INFINITY_DIST) {
                int newDistance = distances[currentVertex] + edgeWeight;

                if (newDistance < distances[adjacentVertex]) {
                    distances[adjacentVertex] = newDistance;
                }
            }
            adjacentNode = adjacentNode->next;
        }
    }

    free(visited);
}

/* buildOrdersGraph Engine */
Graph* buildOrdersGraph(Graph* mainGraph, Coordinate* orders, int numOrders, Coordinate restaurant) {
    if (mainGraph == NULL || orders == NULL || numOrders <= ZERO) {
        return NULL;
    }

    int restaurantVertex = findClosestVertex(mainGraph, restaurant);
    if (restaurantVertex == NEG_ONE) {
        return NULL;
    }

    Graph* ordersGraph = (Graph*)malloc(sizeof(Graph));
    if (ordersGraph == NULL) {
        return NULL;
    }

    ordersGraph->numVertices = numOrders + ONE;

    ordersGraph->adjLists = (Node**)malloc((numOrders + ONE) * sizeof(Node*));
    if (ordersGraph->adjLists == NULL) {
        free(ordersGraph);
        return NULL;
    }

    for (int i = ZERO; i <= numOrders; i++) {
        ordersGraph->adjLists[i] = NULL;
    }

    ordersGraph->vertexCoordinates = (Coordinate*)malloc((numOrders + ONE) * sizeof(Coordinate));
    if (ordersGraph->vertexCoordinates == NULL) {
        free(ordersGraph->adjLists);
        free(ordersGraph);
        return NULL;
    }

    for (int i = ZERO; i < numOrders; i++) {
        ordersGraph->vertexCoordinates[i] = orders[i];
    }
    ordersGraph->vertexCoordinates[numOrders] = restaurant;

    int* orderVertices = (int*)malloc(numOrders * sizeof(int));
    if (orderVertices == NULL) {
        free(ordersGraph->vertexCoordinates);
        free(ordersGraph->adjLists);
        free(ordersGraph);
        return NULL;
    }

    for (int i = 0; i < numOrders; i++) {
        orderVertices[i] = findClosestVertex(mainGraph, orders[i]);
        if (orderVertices[i] == NEG_ONE) {
            free(orderVertices);
            free(ordersGraph->vertexCoordinates);
            free(ordersGraph->adjLists);
            free(ordersGraph);
            return NULL;
        }
    }

    int** distanceMatrix = (int**)malloc((numOrders + ONE) * sizeof(int*));
    if (distanceMatrix == NULL) {
        free(orderVertices);
        free(ordersGraph->vertexCoordinates);
        free(ordersGraph->adjLists);
        free(ordersGraph);
        return NULL;
    }

    for (int i = ZERO; i <= numOrders; i++) {
        distanceMatrix[i] = (int*)malloc(mainGraph->numVertices * sizeof(int));
        if (distanceMatrix[i] == NULL) {
            for (int j = ZERO; j < i; j++) {
                free(distanceMatrix[j]);
            }
            free(distanceMatrix);
            free(orderVertices);
            free(ordersGraph->vertexCoordinates);
            free(ordersGraph->adjLists);
            free(ordersGraph);
            return NULL;
        }

        int sourceVertex = (i == numOrders) ? restaurantVertex : orderVertices[i];
        singleSourceDijkstra(mainGraph, sourceVertex, distanceMatrix[i]);
    }

    // Connect pairs
    for (int i = ZERO; i <= numOrders; i++) {
        for (int j = ZERO; j <= numOrders; j++) {
            if (i == j) continue;

            int destMainVertex = (j == numOrders) ? restaurantVertex : orderVertices[j];
            int distance = distanceMatrix[i][destMainVertex];

            if (distance != INFINITY_DIST) {
                Node* newNode = (Node*)malloc(sizeof(Node));
                if (newNode == NULL) {
                    for (int k = ZERO; k <= numOrders; k++) {
                        free(distanceMatrix[k]);
                    }
                    free(distanceMatrix);
                    free(orderVertices);
                    free(ordersGraph->vertexCoordinates);

                    for (int k = ZERO; k <= numOrders; k++) {
                        Node* current = ordersGraph->adjLists[k];
                        while (current != NULL) {
                            Node* temp = current;
                            current = current->next;
                            free(temp);
                        }
                    }
                    free(ordersGraph->adjLists);
                    free(ordersGraph);
                    return NULL;
                }

                newNode->vertex = j;
                newNode->weight = distance;
                newNode->next = ordersGraph->adjLists[i];
                ordersGraph->adjLists[i] = newNode;
            }
        }
    }

    for (int i = ZERO; i <= numOrders; i++) {
        free(distanceMatrix[i]);
    }
    free(distanceMatrix);
    free(orderVertices);

    return ordersGraph;
}

// Helper: Calculate total distance of a given path
static int calculatePathDistance(Graph* graph, int* path, int pathLength) {
    if (path == NULL || pathLength < TWO) {
        return ZERO;
    }

    int totalDistance = ZERO;

    for (int i = ZERO; i < pathLength - ONE; i++) {
        int fromVertex = path[i];
        int toVertex = path[i + ONE];

        Node* current = graph->adjLists[fromVertex];
        while (current != NULL) {
            if (current->vertex == toVertex) {
                totalDistance += current->weight;
                break;
            }
            current = current->next;
        }
    }

    return totalDistance;
}

// Helper: Find nearest unvisited neighbor
static int findNearestNeighbor(Graph* graph, int currentVertex, int* visited, int numVertices) {
    int nearestVertex = NEG_ONE;
    int minDistance = INFINITY_DIST;

    Node* current = graph->adjLists[currentVertex];
    while (current != NULL) {
        if (!visited[current->vertex] && current->weight < minDistance) {
            minDistance = current->weight;
            nearestVertex = current->vertex;
        }
        current = current->next;
    }

    return nearestVertex;
}

// Helper: Reverse a portion of the tour (for 2-opt swap)
static void reverseTourSegment(int* tour, int i, int j) {
    while (i < j) {
        int temp = tour[i];
        tour[i] = tour[j];
        tour[j] = temp;
        i++;
        j--;
    }
}

// Nearest Neighbor heuristic: Greedy tour construction
static int* nearestNeighborTour(Graph* graph, int startVertex, int numVertices, int* tourLength) {
    int* tour = (int*)malloc(numVertices * sizeof(int));
    if (tour == NULL) {
        return NULL;
    }

    int* visited = (int*)malloc(numVertices * sizeof(int));
    if (visited == NULL) {
        free(tour);
        return NULL;
    }

    for (int i = ZERO; i < numVertices; i++) {
        visited[i] = NOT_VISITED;
    }

    tour[ZERO] = startVertex;
    visited[startVertex] = VISITED;
    int currentVertex = startVertex;
    int tourIndex = ONE;

    for (int i = ONE; i < numVertices; i++) {
        int nextVertex = findNearestNeighbor(graph, currentVertex, visited, numVertices);

        if (nextVertex == NEG_ONE) {
            for (int j = ZERO; j < numVertices; j++) {
                if (!visited[j]) {
                    nextVertex = j;
                    break;
                }
            }
        }

        if (nextVertex != NEG_ONE) {
            tour[tourIndex++] = nextVertex;
            visited[nextVertex] = VISITED;
            currentVertex = nextVertex;
        }
    }

    free(visited);
    *tourLength = numVertices;
    return tour;
}

// 2-opt local search
static void twoOptImprovement(Graph* graph, int* tour, int tourLength) {
    int improved = ONE;
    int iterations = ZERO;
    int maxIterations = tourLength * tourLength;

    while (improved && iterations < maxIterations) {
        improved = ZERO;
        iterations++;

        for (int i = ZERO; i < tourLength - TWO; i++) {
            for (int j = i + TWO; j < tourLength - ONE; j++) { 

                int v1 = tour[i];
                int v2 = tour[i + ONE];
                int v3 = tour[j];
                int v4 = tour[j + ONE]; 

                int weight_v1_v2 = INFINITY_DIST;
                int weight_v3_v4 = INFINITY_DIST;
                int weight_v1_v3 = INFINITY_DIST;
                int weight_v2_v4 = INFINITY_DIST;

                Node* current = graph->adjLists[v1];
                while (current != NULL) {
                    if (current->vertex == v2) { weight_v1_v2 = current->weight; break; }
                    current = current->next;
                }

                current = graph->adjLists[v3];
                while (current != NULL) {
                    if (current->vertex == v4) { weight_v3_v4 = current->weight; break; }
                    current = current->next;
                }

                current = graph->adjLists[v1];
                while (current != NULL) {
                    if (current->vertex == v3) { weight_v1_v3 = current->weight; break; }
                    current = current->next;
                }

                current = graph->adjLists[v2];
                while (current != NULL) {
                    if (current->vertex == v4) { weight_v2_v4 = current->weight; break; }
                    current = current->next;
                }

                if (weight_v1_v2 != INFINITY_DIST && weight_v3_v4 != INFINITY_DIST &&
                    weight_v1_v3 != INFINITY_DIST && weight_v2_v4 != INFINITY_DIST) {

                    int oldDistance = weight_v1_v2 + weight_v3_v4;
                    int newDistance = weight_v1_v3 + weight_v2_v4;

                    if (newDistance < oldDistance) {
                        reverseTourSegment(tour, i + ONE, j);
                        improved = ONE;
                    }
                }
            }
        }
    }
}

/* TSP Calculation Solver Framework Entrypoint */
Route* getMostEfficientDeliveryWay(Graph* ordersGraph, int startVertex) {
    if (ordersGraph == NULL || ordersGraph->numVertices < TWO) {
        return NULL;
    }

    if (startVertex < ZERO || startVertex >= ordersGraph->numVertices) {
        return NULL;
    }

    int numVertices = ordersGraph->numVertices;

    int tourLength = ZERO;
    int* tour = nearestNeighborTour(ordersGraph, startVertex, numVertices, &tourLength);

    if (tour == NULL || tourLength != numVertices) {
        if (tour) free(tour);
        return NULL;
    }

    twoOptImprovement(ordersGraph, tour, tourLength);

    int totalDistance = calculatePathDistance(ordersGraph, tour, tourLength);

    Route* route = (Route*)malloc(sizeof(Route));
    if (route == NULL) {
        free(tour);
        return NULL;
    }

    route->pathVertices = (int*)malloc(tourLength * sizeof(int));
    if (route->pathVertices == NULL) {
        free(route);
        free(tour);
        return NULL;
    }

    memcpy(route->pathVertices, tour, tourLength * sizeof(int));
    route->totalPathLength = tourLength;
    route->totalDistance = totalDistance;

    free(tour);

    return route;
}

/* Splicing algorithm paths together */
Route* combineRoutes(Graph* mainGraph, Route* routeToRestaurant, Route* deliveryRoute, int* orderVertices, int restaurantVertex) {
    if (routeToRestaurant == NULL || deliveryRoute == NULL || mainGraph == NULL) {
        return NULL;
    }

    int maxEstimatedCapacity = routeToRestaurant->totalPathLength + (deliveryRoute->totalPathLength * mainGraph->numVertices);
    int* combinedPath = (int*)malloc(maxEstimatedCapacity * sizeof(int));
    if (combinedPath == NULL) return NULL;

    int currentLength = ZERO;

    for (int i = ZERO; i < routeToRestaurant->totalPathLength; i++) {
        combinedPath[currentLength++] = routeToRestaurant->pathVertices[i];
    }

    int numOrders = deliveryRoute->totalPathLength - ONE;

    for (int i = ZERO;i < deliveryRoute->totalPathLength - ONE; i++) {
        int localFrom = deliveryRoute->pathVertices[i];
        int localTo = deliveryRoute->pathVertices[i + ONE];

        int globalFrom = (localFrom == numOrders) ? restaurantVertex : orderVertices[localFrom];
        int globalTo = (localTo == numOrders) ? restaurantVertex : orderVertices[localTo];

        // Dijkstra 
        int pathLen = ZERO;
        int* intermediateParent = (int*)malloc(mainGraph->numVertices * sizeof(int));
        int* intermediateDistances = (int*)malloc(mainGraph->numVertices * sizeof(int));
        int* intermediateVisited = (int*)malloc(mainGraph->numVertices * sizeof(int));

        if (!intermediateParent || !intermediateDistances || !intermediateVisited) {
            free(combinedPath);
            if (intermediateParent) free(intermediateParent);
            if (intermediateDistances) free(intermediateDistances);
            if (intermediateVisited) free(intermediateVisited);
            return NULL;
        }

        for (int k = ZERO; k < mainGraph->numVertices; k++) {
            intermediateDistances[k] = INFINITY_DIST;
            intermediateVisited[k] = NOT_VISITED;
            intermediateParent[k] = NEG_ONE;
        }
        intermediateDistances[globalFrom] = ZERO;

        for (int count = ZERO; count < mainGraph->numVertices - ONE; count++) {
            int currentVertex = findMinDistVertex(mainGraph->numVertices, intermediateDistances, intermediateVisited);
            if (currentVertex == NEG_ONE || intermediateDistances[currentVertex] == INFINITY_DIST) break;
            intermediateVisited[currentVertex] = VISITED;

            Node* adj = mainGraph->adjLists[currentVertex];
            while (adj != NULL) {
                if (!intermediateVisited[adj->vertex]) {
                    int newDist = intermediateDistances[currentVertex] + adj->weight;
                    if (newDist < intermediateDistances[adj->vertex]) {
                        intermediateDistances[adj->vertex] = newDist;
                        intermediateParent[adj->vertex] = currentVertex;
                    }
                }
                adj = adj->next;
            }
        }

        int segmentLen = ZERO;
        int* segment = reconstructPath(intermediateParent, globalTo, &segmentLen);

        if (segment != NULL) {
            for (int s = ONE; s < segmentLen; s++) {
                combinedPath[currentLength++] = segment[s];
            }
            free(segment);
        }

        free(intermediateParent);
        free(intermediateDistances);
        free(intermediateVisited);
    }

    int* exactCombinedPath = (int*)realloc(combinedPath, currentLength * sizeof(int));
    if (exactCombinedPath != NULL) {
        combinedPath = exactCombinedPath;
    }

    Route* resultRoute = (Route*)malloc(sizeof(Route));
    if (resultRoute == NULL) {
        free(combinedPath);
        return NULL;
    }

    resultRoute->pathVertices = combinedPath;
    resultRoute->totalPathLength = currentLength;
    resultRoute->totalDistance = routeToRestaurant->totalDistance + deliveryRoute->totalDistance;

    return resultRoute;
}