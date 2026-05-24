#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef MAPS_COMMON_H
#define MAPS_COMMON_H

#define SUCCESS_CODE 0
#define GENERAL_ERROR 501
#define ZERO 0
#define ONE 1
#define TWO 2

#define INFINITY_DIST 9999999
#define NOT_VISITED 0
#define VISITED 1

#define NEG_ONE          -1
#define ESTIMATED_PADDING 1

typedef struct {
    double x;
    double y;
} Coordinate;

typedef struct Node {
    int vertex;
    int weight;
    struct Node* next;
} Node;

typedef struct Graph {
    int numVertices;
    Node** adjLists;
    Coordinate* vertexCoordinates;
} Graph;

typedef struct MainGraph {
    Graph* graph;
    Coordinate* infoCoordinates;
    int numOrders; 
} MainGraph;

typedef struct {
    int* pathVertices;
    int totalPathLength;
    int totalDistance;
} Route;


MainGraph* loadAreaGraph(char* fileName);

Route* bestRouteToRestaurant(Graph* mainGraph, Coordinate* infoCoordinates);

Graph* buildOrdersGraph(Graph* mainGraph, Coordinate* orders,
    int numOrders, Coordinate restaurant);

Route* getMostEfficientDeliveryWay(Graph* ordersGraph, int startVertex);

Route* combineRoutes(Graph* mainGraph, Route* routeToRestaurant,
    Route* deliveryRoute, int* orderVertices, int restaurantVertex);

void outputRoute(Route* resultRoute);
void exportVisualGraph(Graph* map, Route* finalRoute);

void freeGraph(Graph* graph);
void freeRoute(Route* route);
void freeMainGraph(MainGraph* mainGraph);

#endif