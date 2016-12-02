#include <vector>

struct BFSinfo;
struct vertex {
    int nodeid;
    char keep[3];
    char remove[3];
    std::vector<vertex*> adjList;
    BFSinfo* bfs;
};

struct BFSinfo {
    char color[6];
    int d;
    vertex* parent;
};

struct edge {
    vertex* u;
    vertex* v;
    int capacity, flow;
};

struct graph {
    std::vector<vertex*> V;
    std::vector<edge*> E;
};

void addEdge(graph* G, vertex* u, vertex* v, int capacity);
bool operator==(const vertex& lhs, const vertex& rhs);
bool operator==(const edge& lhs, const edge& rhs);
void printGraph(graph* G);
edge* edgeInGraph(graph* G, int unodeid, int vnodeid);
void updateAdjList(graph* G);

