#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include <climits>

using namespace std;

struct BFSinfo;
struct vertex {
    int nodeid;
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
    return (lhs.nodeid == rhs.nodeid);
}

struct edge {
    vertex* u;
    vertex* v;
    int capacity, flow;
};

bool operator==(const edge& lhs, const edge& rhs) {
    return (*(lhs.u) == *(rhs.u) && *(lhs.v) == *(rhs.v));
}

struct graph {
    vector<vertex*> V;
    vector<edge*> E;
};

void printGraph(graph* G) {
    for (int i = 0; i < G->V.size(); i++) {
        printf("vertexid %d:\nkeep: %s\nremove: %s\n\n", G->V[i]->nodeid, G->V[i]->keep, G->V[i]->remove);
    }

    for (int i = 0; i < G->E.size(); i++) {
        printf("edge %d:\nu: %d\nv: %d\n capacity: %d\n\n", i + 1, G->E[i]->u->nodeid, G->E[i]->v->nodeid, G->E[i]->capacity);
    }
}

void initGraph(graph* G) {
    vertex* source = new vertex;
    source->nodeid = 0;
    strcpy(source->keep, "so");
    strcpy(source->remove,"so");
    source->bfs = new BFSinfo;
    G->V.push_back(source);

    vertex* sink = new vertex;
    sink->nodeid = 1;
    strcpy(sink->keep, "si");
    strcpy(sink->remove, "si");
    sink->bfs = new BFSinfo;
    G->V.push_back(sink);
}

edge* edgeInGraph(graph* G, int unodeid, int vnodeid) {
    edge temp;
    temp.u = new vertex;
    temp.v = new vertex;
    temp.u->nodeid = unodeid;
    temp.v->nodeid = vnodeid;
    for (int i = 0; i < G->E.size(); i++) {
        if (*G->E[i] == temp) {
            delete temp.u;
            delete temp.v;
            return G->E[i];
        }
    }
    delete temp.u;
    delete temp.v;
    return NULL;
}

bool hasConflict(vertex* catlover, vertex* doglover) {
    if (!strcmp(catlover->keep, doglover->remove) || !strcmp(catlover->remove, doglover->keep)) {
        return true;
    } else {
        return false;
    }
}

int residualCapacity(graph* G, vertex* u, vertex* v) {
    edge* temp;
    if (temp = edgeInGraph(G, u->nodeid, v->nodeid)) {
        return temp->capacity - temp->flow;
    } else if (temp = edgeInGraph(G, v->nodeid, u->nodeid)) {
        return temp->flow;
    } else {
        return 0;
    }
}

int capacity(graph* G, vertex* u, vertex* v) {
    edge* temp;
    if (temp = edgeInGraph(G, u->nodeid, v->nodeid)) {
        return temp->capacity;
    }
    return INT_MIN;
}

graph* buildResidualGraph(graph* G) {
    graph* Gf = new graph;
    for (int i = 0; i < G->V.size(); i++) {
        Gf->V.push_back(G->V[i]);
        for (int j = 0; j < G->V.size(); j++) {
            if (i != j) {
                int capacity = residualCapacity(G, G->V[i], G->V[j]);
                if (capacity) {
                    edge* temp = new edge;
                    temp->u = G->V[i];
                    temp->v = G->V[j];
                    temp->capacity = capacity;
                    temp->flow = 0;
                    Gf->E.push_back(temp);
                }
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
                edge* temp = edgeInGraph(G, parent->nodeid, path->V.back()->nodeid);
                if (!temp) return NULL; // there was no path from d to s
                path->E.push_back(temp);
                path->V.push_back(parent);
                parent = parent->bfs->parent;
            }
        }
    }
    return path;
}

int min(graph* path) {
    int min = INT_MAX;
    for (int i = 0; i < path->E.size(); i++) {
        if (path->E[i]->capacity < min) min = path->E[i]->capacity;
    }
    return min;
}

graph* edmonds_karp(graph* G, vertex* s, vertex* t) {
    for (int i = 0; i < G->E.size(); i++) {
        G->E[i]->flow = 0;
    }

    graph* Gf = buildResidualGraph(G);
    graph* path = BFS(Gf, s, t);
    while (path != NULL) {
        int pathMinCapacity = min(path);
        for (int i = 0; i < path->E.size(); i++) {
            edge* temp = edgeInGraph(G, path->E[i]->u->nodeid, path->E[i]->v->nodeid);
            if (temp) {
                temp->flow += pathMinCapacity;
            } else {
                temp = edgeInGraph(G, temp->v->nodeid, temp->u->nodeid);
                temp->flow -= pathMinCapacity;
            }
        }
        Gf = buildResidualGraph(G);
        path = BFS(Gf, s, t);
    }
    // if there is no path from s to t in residual network (Gf) then we have
    // found the maximum flow and its corresponding residual network
    return Gf;
}

bool inVector(vector<vertex*>* data, vertex* u) {
    for (int i = 0; i < data->size(); i++) {
        if ((*data)[i] == u) return true;
    }
    return false;
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
            temp->nodeid = j + 2;  // have to add 2 because source and sink have 0 and 1
            strcpy(temp->keep, keep);
            strcpy(temp->remove, remove);
            temp->bfs = new BFSinfo;
            G.V.push_back(temp);
        }

        // setup edges of graph
        for (int j = 0; j < G.V.size(); j++) {
            for (int k = (j + 1); k < G.V.size(); k++) {
                // add an edge from source to each cat lover
                if (G.V[j]->nodeid == 0 && G.V[k]->keep[0] == 'C') {
                    edge* temp = new edge;
                    temp->u = G.V[j];
                    temp->v = G.V[k];
                    temp->capacity = 1;
                    temp->flow = 0;
                    G.E.push_back(temp);
                    G.V[j]->adjList.push_back(G.V[k]);
                    G.V[k]->adjList.push_back(G.V[j]);
                // add an edge from each dog lover to sink
            } else if (G.V[j]->nodeid == 1 && G.V[k]->keep[0] == 'D') {
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

        graph* Gf = edmonds_karp(&G, G.V[0], G.V[1]);
        vector<vertex*> vertexCoverCats, vertexCoverDogs;

        for (int j = 2; j < Gf->V.size(); j++) {
            edge* temp = edgeInGraph(Gf, Gf->V[0], Gf->V[j]);
            if (temp && Gf->V[j]->keep[0] == 'C') vertexCoverCats.push_back(Gf->V[j]);
        }

        for (int j = 0; j < vertexCoverCats.size(); j++) {
            edge* temp = edgeInGraph(Gf, vertexCoverCats[j], )
        }
        printf("%d\n", vertexCover.size());
    }
}
