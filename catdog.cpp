#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include <climits>
#include "graph.h"

using namespace std;

// Initialize source and sink for the given graph
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

// Simple function to return true or false if a given pair of nodes
// meet our definition of "in conflict"
bool hasConflict(vertex* catlover, vertex* doglover) {
    if (!strcmp(catlover->keep, doglover->remove) || !strcmp(catlover->remove, doglover->keep)) {
        return true;
    } else {
        return false;
    }
}

// Calculates the residual capacity for a given edge
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

// Creates a new graph struct with all the nodes of the given graph
// and then generates new edges for the residual graph of the given
// graph.
graph* buildResidualGraph(graph* G) {
    graph* Gf = new graph;
    for (int i = 0; i < G->V.size(); i++) {
        Gf->V.push_back(G->V[i]);
        for (int j = 0; j < G->V.size(); j++) {
            if (i != j) {
                int capacity = residualCapacity(G, G->V[i], G->V[j]);
                if (capacity) {
                    addEdge(Gf, G->V[i], G->V[j], capacity);                    
                }
            }
        }
    }
    return Gf;
}

// classic BFS with two modifications:
// 1) When updating the current node to GRAY, if we notice the
//    destination node in the adjList then we know we are done so stop,
//    we don't care about any other nodes
//
// 2) This implementation has been modified to record the path from s->d
//    that it finds in a new graph struct and then return that struct
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
                if (*(u->adjList[i]) == *d) break;
                Q.push(u->adjList[i]);
            }
        }
        strcpy(u->bfs->color, "BLACK");
    }

    // find and return shortest path from source to destination
    graph* path = new graph;
    path->V.push_back(d);
    while (path->V.back()->bfs->parent != NULL) {
        edge* temp = edgeInGraph(G, path->V.back()->bfs->parent->nodeid, path->V.back()->nodeid);
        path->E.push_back(temp);
        path->V.push_back(path->V.back()->bfs->parent);
    }
    if (path->V.back()->nodeid == s->nodeid) {
        return path;
    } else {
        return NULL;
    }
}

graph* edmonds_karp(graph* G, vertex* s, vertex* t) {
    for (int i = 0; i < G->E.size(); i++) {
        G->E[i]->flow = 0;
    }

    // Generate initial residual network
    graph* Gf = buildResidualGraph(G);
    // Update node adjacecy lists
    updateAdjList(Gf);
    // Find first augmenting path
    graph* path = BFS(Gf, s, t);
    // Begin main algorithm
    while (path != NULL) {
        int pathMinCapacity = 1;
        // adjust flows
        for (int i = 0; i < path->E.size(); i++) {
            edge* temp = edgeInGraph(G, path->E[i]->u->nodeid, path->E[i]->v->nodeid);
            if (temp) {
                temp->flow += pathMinCapacity;
            } else {
                temp = edgeInGraph(G, path->E[i]->v->nodeid, path->E[i]->u->nodeid);
                temp->flow -= pathMinCapacity;
            }
        }

        // repeat
        Gf = buildResidualGraph(G);
        updateAdjList(Gf);
        path = BFS(Gf, s, t);
    }

    // if there is no path from s to t in residual network (Gf) then we have
    // found the maximum flow and its corresponding residual network
    return Gf;
}

// function to check for existance of a vertex in a vector of vertices
bool inVector(vector<vertex*>* data, vertex* u) {
    for (int i = 0; i < data->size(); i++) {
        if ((*data)[i] == u) return true;
    }
    return false;
}

// function to check for existance of a string in a vector of strings
bool inVector(vector<char*>* data, char* string) {
    for (int i = 0; i < data->size(); i++) {
        if (!strcmp((*data)[i], string)) return true;
    }
    return false;
}

int main() {
    freopen("samples/sample.in", "r", stdin);
    int numgames, numcats, numdogs, numvoters;
    scanf("%d\n", &numgames);

    for (int i = 0; i < numgames; i++) {
        scanf("%d %d %d\n", &numcats, &numdogs, &numvoters);

        char keep[3], remove[3];

        graph G;
        // set up sink and source
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
            for (int k = 0; k < G.V.size(); k++) {
                // add an edge from source to each cat lover
                if (G.V[j]->nodeid == 0 && G.V[k]->keep[0] == 'C') {
                    addEdge(&G, G.V[j], G.V[k], 1);
                // add an edge from each dog lover to sink
                } else if (G.V[j]->nodeid == 1 && G.V[k]->keep[0] == 'D') {
                    addEdge(&G, G.V[k], G.V[j], 1); 
                // add an edge for each pair of conflicting nodes
                } else if (G.V[j]->keep[0] == 'C' && G.V[k]->keep[0] == 'D' && hasConflict(G.V[j], G.V[k])) {
                    addEdge(&G, G.V[j], G.V[k], 1);                    
                }
            }
        }
        
        // find the maximum flow and return the residual graph for the found max flow
        graph* Gf = edmonds_karp(&G, G.V[0], G.V[1]);
        updateAdjList(Gf);

        // begin calculation of the vertex cover
        vector<vertex*> vertexCover;
	vector<char*> keepAnimals;
        graph* path;

        // use BFS to check for existance of a path from s to each cat and dog lover
        for (int j = 2; j < Gf->V.size(); j++) {
            path = BFS(Gf, Gf->V[0], Gf->V[j]);
            // if there is no path from s to this cat lover then add them to the vertex cover
            if (Gf->V[j]->keep[0] == 'C' && !path) vertexCover.push_back(Gf->V[j]);
            // if there is a path from s to this dog lover then add them to the cover
            else if (Gf->V[j]->keep[0] == 'D' && path) vertexCover.push_back(Gf->V[j]);
            // otherwise, record the animal this person wants to keep
            else if (!G.V[j]->nodeid != 0 && G.V[j]->nodeid != 1 && inVector(&keepAnimals, G.V[j]->keep)) {
                char* temp = new char[3];
                strcpy(temp, G.V[j]->keep);
                keepAnimals.push_back(temp);
            }
        }

        // print trace info
	fprintf(stderr, "Animals to keep:\n");
	for (int j = 0; j < keepAnimals.size(); j++) {
            fprintf(stderr, "%s\n", keepAnimals[j]);
	}
	fprintf(stderr, "Happy people:\n");
	for (int j = 0; j < G.V.size(); j++) {
            if (!inVector(&vertexCover, G.V[j]) && G.V[j]->nodeid != 0 && G.V[j]->nodeid != 1) {
                fprintf(stderr, "Happy person: %s %s\n", G.V[j]->keep, G.V[j]->remove);
	    }
	}

        printf("%d\n", Gf->V.size() - 2 - vertexCover.size());
    }
}
