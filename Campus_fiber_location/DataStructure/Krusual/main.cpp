#include "Graph.h"
#include "GraphFileIO.h"
#include <cstdio>

int main() {
    // 创建一个足够大的图对象（例如，预计最多100个顶点）
    Graph myGraph(100);

    // 使用GraphFileIO读取文件并构建图
    if (GraphFileIO::readNodesAndBuildGraph(myGraph, "nodes.txt")) {
        std::printf("文件读取成功！图信息如下：\n");
        myGraph.printGraph(); // 打印图信息

        std::printf("\n 计算最小生成树：\n");
        myGraph.kruskalMST(); // 执行Kruskal算法
    }
    else {
        std::fprintf(stderr, "文件读取失败\n");
    }
    return 0;
}