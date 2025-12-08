#ifndef GRAPH_H
#define GRAPH_H

#include "Edge.h"
#include "HeapSort.h"
class UnionFind; // 前向声明

class Graph {
private:
    int vertices;     // 顶点数量
    int** adjMatrix;  // 邻接矩阵
    Edge** edgesArray; // 边数组
    int edgeCount;    // 当前边数
    int maxEdges;     // 最大边数容量
    HeapSort heapSorter;
    // 堆排序相关方法
    void heapify(Edge* edges[], int n, int i, Edge* temp) const;
    void buildHeap(Edge* edges[], int n, Edge* temp)const;

public:
    static const int INF = 2147483647; // 无穷大表示

    Graph(int v);
    ~Graph();

    // 图操作
    void addEdge(int u, int v, int weight);
    void printGraph() const;

    // 算法实现
    void heapSortEdges(Edge* edges[], int n) const;
    void kruskalMST() const;

    // 辅助方法
    int getVertices() const { return vertices; }
    int getEdgeCount() const { return edgeCount; }
};

#endif