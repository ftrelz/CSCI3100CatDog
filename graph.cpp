#include <cstdio>
#include "graph.h"

using namespace std;

void addEdge(graph* G, vertex* u, vertex* v, int capacity) {
    edge* newEdge = new edge;
    newEdge->u = u;
    newEdge->v = v;
    newEdge->capacity = capacity;
    newEdge->flow = 0;
    G->E.push_back(newEdge);
    u->adjList.push_back(v);
}

bool operator==(const vertex& lhs, const vertex& rhs) {
    return (lhs.nodeid == rhs.nodeid);
}

bool operator==(const edge& lhs, const edge& rhs) {
    return (*(lhs.u) == *(rhs.u) && *(lhs.v) == *(rhs.v));
}

void printGraph(graph* G) {
    for (int i = 0; i < G->V.size(); i++) {
        printf("vertexid %d:\nkeep: %s\nremove: %s\n\n", G->V[i]->nodeid, G->V[i]->keep, G->V[i]->remove);
    }

    for (int i = 0; i < G->E.size(); i++) {
        printf("edge %d:\nu: %d\nv: %d\n capacity: %d\n\n", i + 1, G->E[i]->u->nodeid, G->E[i]->v->nodeid, G->E[i]->capacity);
    }
}

edge* edgeInGraph(graph* G, int unodeid, int vnodeid) {
    for (int i = 0; i < G->E.size(); i++) {
        if (G->E[i]->u->nodeid == unodeid && G->E[i]->v->nodeid == vnodeid) {
            return G->E[i];
        }
    }
    return NULL;
}

void updateAdjList(graph* G) {
    for (int i = 0; i < G->V.size(); i++) {
        G->V[i]->adjList.clear();
    }
    for (int i = 0; i < G->E.size(); i++) {
        G->E[i]->u->adjList.push_back(G->E[i]->v);
    }
}

