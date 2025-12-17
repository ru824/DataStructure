#ifndef GRAPH_H
#define GRAPH_H

#include "Edge.h"
class UnionFind; // 前向声明
class Node;
class Graph {
private:
    int vertices;     // 顶点数量
    int** adjMatrix;  // 邻接矩阵
    Edge** edgesArray; // 边数组
    int edgeCount;    // 当前边数
    int maxEdges;     // 最大边数容量
    // 堆排序相关方法
    void heapify(Edge* edges[], int n, int i) const;
    void buildHeap(Edge* edges[], int n)const;
    double calDistance(double x1, double y1, double x2, double y2) const;


public:
    static const int INF = 2147483647; // 无穷大表示

    Graph(int v);
    ~Graph();

    // 图操作
    void NodesToEdges(Node nodes[], int nodes_n);
    void addEdge(int u, int v, int weight);
    void printGraph() const;

    // 算法实现
    void heapSortEdges(Edge* edges[], int n) const;
    void kruskalMST() const;

    // 辅助方法

    void NodesToEdges(Graph& graph, Node* nodes[], int nodes_n);
    int getVertices() const { return vertices; }
    int getEdgeCount() const { return edgeCount; }
};

#endif