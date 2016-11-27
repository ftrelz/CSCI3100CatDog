#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include <climits>

using namespace std;

struct BFSinfo;
struct vertex {
    char keep[3];
    char remove[3];
    vector<vertex*> adjList;
    BFSinfo* bfs;
};

struct BFSinfo {
    char color[6];
    int d;
    vertex* parent;
};

bool operator==(const vertex& lhs, const vertex& rhs) {
    return (!strcmp(lhs.keep, rhs.keep) && !strcmp(lhs.remove, rhs.remove) && lhs.adjList == rhs.adjList);
}

struct edge {
    vertex* u;
    vertex* v;
    int capacity, flow;
};

bool operator==(const edge& lhs, const edge& rhs) {
    return (lhs.u == rhs.u && lhs.v == rhs.v && lhs.capacity == rhs.capacity && lhs.flow == rhs.flow);
}

struct graph {
    vector<vertex*> V;
    vector<edge*> E;
};

void printGraph(graph* G) {
    for (int i = 0; i < G->V.size(); i++) {
        printf("vertex %d:\nkeep: %s\nremove: %s\n\n", i + 1, G->V[i]->keep, G->V[i]->remove);
    }

    for (int i = 0; i < G->E.size(); i++) {
        printf("edge %d:\nu: %s %s\nv: %s %s\n capacity: %d\n\n", i + 1, G->E[i]->u->keep, G->E[i]->u->remove, G->E[i]->v->keep, G->E[i]->v->remove, G->E[i]->capacity);
    }
}

void initGraph(graph* G) {
    vertex* source = new vertex;
    strcpy(source->keep, "so");
    strcpy(source->remove,"so");
    source->bfs = new BFSinfo;
    G->V.push_back(source);

    vertex* sink = new vertex;
    strcpy(sink->keep, "si");
    strcpy(sink->remove, "si");
    sink->bfs = new BFSinfo;
    G->V.push_back(sink);
}

edge* edgeInGraph(graph* G, edge* e) {
    for (int i = 0; i < G->E.size(); i++) {
        if (*G->E[i] == *e) return G->E[i];
        else return NULL;
    }
    return false;
}

bool hasConflict(vertex* catlover, vertex* doglover) {
    if (!strcmp(catlover->keep, doglover->remove) || !strcmp(catlover->remove, doglover->keep)) {
        return true;
    } else {
        return false;
    }
}

int residualCapacity(graph* G, vertex* u, vertex* v) {
    for (int i = 0; i < G->E.size(); i++) {
        if (G->E[i]->u == u && G->E[i]->v == v) {
            return (G->E[i]->capacity - G->E[i]->flow);
        } else if (G->E[i]->u == v && G->E[i]->v == u) {
            return G->E[i]->flow;
        } else {
            return 0;
        }
    }
}

int capacity(graph* G, vertex* u, vertex* v) {
    for (int i = 0; i < G->E.size(); i++) {
        if (*G->E[i]->u == *u && *G->E[i]->v == *v) return G->E[i]->capacity;
    }
}

graph* buildResidualGraph(graph* G) {
    graph* Gf = new graph;
    for (int i = 0; i < G->V.size(); i++) {
        Gf->V.push_back(G->V[i]);
        for (int j = (i + 1); j < G->V.size(); j++) {
            int capacity = residualCapacity(G, G->V[i], G->V[j]);
            if (capacity) {
                edge* temp = new edge;
                temp->u = G->V[i];
                temp->v = G->V[j];
                temp->capacity = capacity;
                temp->flow = 0;
                G->E.push_back(temp);
            }
        }
    }
    return Gf;
}

graph* BFS(graph* G, vertex* s, vertex* d) {
    // vanilla BFS
    for (int i = 0; i < G->V.size(); i++) {
        strcpy(G->V[i]->bfs->color, "WHITE");
        G->V[i]->bfs->d = -1;
        G->V[i]->bfs->parent = NULL;
    }

    strcpy(s->bfs->color, "GRAY");
    s->bfs->d = 0;
    s->bfs->parent = NULL;
    queue<vertex*> Q;
    Q.push(s);
    while (!Q.empty()) {
        vertex* u = Q.front();
        Q.pop();
        for (int i = 0; i < u->adjList.size(); i++) {
            if (!strcmp(u->adjList[i]->bfs->color, "WHITE")) {
                strcpy(u->adjList[i]->bfs->color, "GRAY");
                u->adjList[i]->bfs->d += 1;
                u->adjList[i]->bfs->parent = u;
                Q.push(u->adjList[i]);
            }
        }
        strcpy(u->bfs->color, "BLACK");
    }

    // find and return shortest path from source to destination
    graph* path = new graph;
    for (int i = 0; i < G->V.size(); i++) {
        if (G->V[i] == d) {
            path->V.push_back(d);
            vertex* parent = d->bfs->parent;
            while (parent != NULL) {
                edge* temp = new edge;
                temp->u = parent;
                temp->v = path->V.back();
                path->E.push_back(temp);
                path->V.push_back(parent);
                parent = parent->bfs->parent;
            }
        }
    }

    // there was no path from s to d
    if (path->V.back() != s) {
        delete path;
        return NULL;
    } else {
        return path;
    }
}

int min(graph* path) {
    int min = INT_MAX;
    for (int i = 0; i < path->E.size(); i++) {
        if (path->E[i]->capacity < min) min = path->E[i]->capacity;
    }
    return min;
}

void edmonds_karp(graph* G, vertex* s, vertex* t) {
    for (int i = 0; i < G->E.size(); i++) {
        G->E[i]->flow = 0;
    }

    graph* Gf = buildResidualGraph(G);
    graph* path = BFS(Gf, s, t);
    while (path != NULL) {
        int pathMinCapacity = min(path);
        for (int i = 0; i < path->E.size(); i++) {
            if (edgeInGraph(G, path->E[i])) {
                path->E[i]->flow = path->E[i]->flow + pathMinCapacity;
            } else {
                edge* temp = edgeInGraph(path, )
            }
        }
    }
}

int main() {
    freopen("samples/sample.in", "r", stdin);
    int numgames, numcats, numdogs, numvoters;
    scanf("%d\n", &numgames);

    for (int i = 0; i < numgames; i++) {
        scanf("%d %d %d\n", &numcats, &numdogs, &numvoters);
        printf("numcats = %d, numdogs = %d, numvoters = %d\n", numcats, numdogs, numvoters);

        char keep[3], remove[3];

        graph G;
        initGraph(&G);

        // add vertexes from input to graph
        vertex* temp;
        for (int j = 0; j < numvoters; j++) {
            scanf("%s %s\n", &keep, &remove);
            keep[2] = '\0';
            remove[2] = '\0';
            temp = new vertex;
            strcpy(temp->keep, keep);
            strcpy(temp->remove, remove);
            temp->bfs = new BFSinfo;
            G.V.push_back(temp);
        }

        // setup edges of graph
        for (int j = 0; j < G.V.size(); j++) {
            for (int k = (j + 1); k < G.V.size(); k++) {
                // add an edge from source to each cat lover
                if (!strcmp(G.V[j]->keep, "so") && G.V[k]->keep[0] == 'C') {
                    edge* temp = new edge;
                    temp->u = G.V[j];
                    temp->v = G.V[k];
                    temp->capacity = 1;
                    temp->flow = 0;
                    G.E.push_back(temp);
                    G.V[j]->adjList.push_back(G.V[k]);
                    G.V[k]->adjList.push_back(G.V[j]);
                // add an edge from each dog lover to sink
            } else if (!strcmp(G.V[j]->keep, "si") && G.V[k]->keep[0] == 'D') {
                    edge* temp = new edge;
                    temp->u = G.V[k];
                    temp->v = G.V[j];
                    temp->capacity = 1;
                    temp->flow = 0;
                    G.E.push_back(temp);
                    G.V[j]->adjList.push_back(G.V[k]);
                    G.V[k]->adjList.push_back(G.V[j]);
                // add an edge for each pair of conflicting nodes
                } else if (hasConflict(G.V[j], G.V[k])) {
                    edge* temp = new edge;
                    temp->u = G.V[j];
                    temp->v = G.V[k];
                    temp->capacity = 1;
                    temp->flow = 0;
                    G.E.push_back(temp);
                    G.V[j]->adjList.push_back(G.V[k]);
                    G.V[k]->adjList.push_back(G.V[j]);
                }
            }
        }
        //printGraph(&G);
        edmonds_karp(&G, G.V[0], G.V[1]);
    }
}
