#include "Graph.h"
#include "GraphFileIO.h"
#include <cstdio>

int main() {
    printf("=== 从标准图格式文件读取数据 ===\n");

    // 从文件读取图
    GraphFileIO io("graph.txt");
    Graph* graph = io.readGraphFromFile();

    if (graph != nullptr) {
        // 打印图信息
        graph->printGraph();

        // 执行Kruskal算法
        graph->kruskalMST();

        // 释放内存
        delete graph;
    }

    printf("\n=== 手动构建图测试 ===\n");

    // 手动构建测试（可选）
    Graph manualGraph(4);
    manualGraph.addEdge(0, 1, 1);
    manualGraph.addEdge(1, 2, 2);
    manualGraph.addEdge(2, 3, 3);
    manualGraph.addEdge(0, 3, 4);

    manualGraph.printGraph();
    manualGraph.kruskalMST();

    return 0;
}