#include "Graph.h"
#include "UnionFind.h"
#include "HeapSort.h"
#include <iostream>
#include <algorithm>
#include <climits>

// 构造函数：初始化图结构
Graph::Graph(int v) : vertices(v), edgeCount(0), maxEdges(v* (v - 1) / 2) {
    // 输入验证
    if (v <= 0) {
        std::cerr << "错误：顶点数必须为正整数" << std::endl;
        vertices = 1;
        maxEdges = 0;
    }

    // 分配邻接矩阵内存
    adjMatrix = new int* [vertices];
    for (int i = 0; i < vertices; ++i) {
        adjMatrix[i] = new int[vertices];
    }

    // 初始化邻接矩阵
    for (int i = 0; i < vertices; ++i) {
        for (int j = 0; j < vertices; ++j) {
            if (i == j) {
                adjMatrix[i][j] = 0;  // 顶点到自身距离为0
            }
            else {
                adjMatrix[i][j] = INF; // 初始化为无穷大
            }
        }
    }

    // 分配边数组内存
    edgesArray = new Edge * [maxEdges];
    for (int i = 0; i < maxEdges; ++i) {
        edgesArray[i] = nullptr;
    }
}

// 析构函数：释放所有内存
Graph::~Graph() {
    // 释放邻接矩阵
    if (adjMatrix != nullptr) {
        for (int i = 0; i < vertices; ++i) {
            delete[] adjMatrix[i];
        }
        delete[] adjMatrix;
    }

    // 释放边数组
    if (edgesArray != nullptr) {
        for (int i = 0; i < maxEdges; ++i) {
            delete edgesArray[i];
        }
        delete[] edgesArray;
    }
}

// 添加边到图中
void Graph::addEdge(int u, int v, int weight) {
    // 边界检查
    if (u < 0 || u >= vertices || v < 0 || v >= vertices) {
        std::cerr << "错误：顶点索引越界 (" << u << ", " << v << ")" << std::endl;
        return;
    }

    if (u == v) {
        std::cerr << "警告：忽略自环边 (" << u << "->" << v << ")" << std::endl;
        return;
    }

    // 设置双向边（无向图）
    adjMatrix[u][v] = weight;
    adjMatrix[v][u] = weight;

    // 添加到边数组（避免重复，只存储u<v的边）
    if (u < v && edgeCount < maxEdges) {
        // 检查边是否已存在
        for (int i = 0; i < edgeCount; ++i) {
            if (edgesArray[i] != nullptr &&
                edgesArray[i]->getSrc() == u && edgesArray[i]->getDest() == v) {
                // 边已存在，更新权重
                edgesArray[i]->setWeight(weight);
                return;
            }
        }

        // 添加新边
        edgesArray[edgeCount] = new Edge(u, v, weight);
        edgeCount++;
    }
    else if (edgeCount >= maxEdges) {
        std::cerr << "错误：边数已达上限，无法添加新边" << std::endl;
    }
}

// 打印图的邻接矩阵
void Graph::printGraph() const {
    std::cout << "图的邻接矩阵 (" << vertices << "×" << vertices << "):" << std::endl;

    // 打印列标题
    std::cout << "    ";
    for (int j = 0; j < vertices; ++j) {
        std::cout << j << "\t";
    }
    std::cout << std::endl;

    // 打印矩阵内容
    for (int i = 0; i < vertices; ++i) {
        std::cout << i << " | ";
        for (int j = 0; j < vertices; ++j) {
            if (adjMatrix[i][j] == INF) {
                std::cout << "INF\t";
            }
            else {
                std::cout << adjMatrix[i][j] << "\t";
            }
        }
        std::cout << std::endl;
    }

    // 打印边信息
    std::cout << "\n边列表 (" << edgeCount << " 条边):" << std::endl;
    for (int i = 0; i < edgeCount; ++i) {
        if (edgesArray[i] != nullptr) {
            std::cout << "边 " << i << ": " << edgesArray[i]->getSrc() << " - "
                << edgesArray[i]->getDest() << " 权重: " << edgesArray[i]->getWeight() << std::endl;
        }
    }
    std::cout << std::endl;
}

// 堆排序相关方法实现
void Graph::heapify(Edge* edges[], int n, int i, Edge* temp) const{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    // 找到左子节点、右子节点和当前节点中的最大值
    if (left < n && edges[left]->getWeight() > edges[largest]->getWeight()) {
        largest = left;
    }

    if (right < n && edges[right]->getWeight() > edges[largest]->getWeight()) {
        largest = right;
    }

    // 如果最大值不是当前节点，交换并递归调整
    if (largest != i) {
        // 使用第三个数组temp作为临时存储进行交换
        *temp = *edges[i];
        *edges[i] = *edges[largest];
        *edges[largest] = *temp;

        heapify(edges, n, largest, temp);
    }
}

void Graph::buildHeap(Edge* edges[], int n, Edge* temp) const{
    // 从最后一个非叶子节点开始构建堆
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(edges, n, i, temp);
    }
}

void Graph::heapSortEdges(Edge* edges[], int n) const {
    if (n <= 1) return;

    // 创建第三个数组作为辅助空间
    Edge* temp = new Edge();

    // 构建最大堆
    buildHeap(edges, n, temp);

    // 逐个从堆中提取元素
    for (int i = n - 1; i > 0; i--) {
        // 将当前堆顶元素（最大值）与末尾元素交换
        *temp = *edges[0];
        *edges[0] = *edges[i];
        *edges[i] = *temp;

        // 调整剩余元素，堆大小减1
        heapify(edges, i, 0, temp);
    }

    delete temp;
}

// Kruskal算法实现（优化版）
void Graph::kruskalMST() const {
    if (edgeCount == 0) {
        std::cout << "图中没有边" << std::endl;
        return;
    }

    if (vertices <= 1) {
        std::cout << "顶点数不足，无法生成最小生成树" << std::endl;
        return;
    }

    // 创建边指针数组用于排序
    Edge** edges = new Edge * [edgeCount];
    for (int i = 0; i < edgeCount; i++) {
        edges[i] = new Edge(*edgesArray[i]); // 深拷贝
    }

    // 使用堆排序对边按权重排序
    heapSortEdges(edges, edgeCount);

    UnionFind uf(vertices);
    Edge** result = new Edge * [vertices - 1];
    int resultCount = 0;
    int totalWeight = 0;

    std::cout << "Kruskal算法执行过程:" << std::endl;

    // 构建最小生成树
    for (int i = 0; i < edgeCount && resultCount < vertices - 1; i++) {
        Edge* currentEdge = edges[i];
        int u = currentEdge->getSrc();
        int v = currentEdge->getDest();

        if (uf.find(u) != uf.find(v)) {
            uf.unite(u, v);
            result[resultCount] = new Edge(*currentEdge);
            totalWeight += currentEdge->getWeight();

            std::cout << "添加边: " << u << " - " << v
                << " \t权重: " << currentEdge->getWeight() << std::endl;
            resultCount++;
        }
    }

    // 输出结果
    if (resultCount == vertices - 1) {
        std::cout << "\n最小生成树构建成功!" << std::endl;
        std::cout << "最小生成树包含 " << resultCount << " 条边:" << std::endl;
        for (int i = 0; i < resultCount; i++) {
            std::cout << result[i]->getSrc() << " - " << result[i]->getDest()
                << " \t权重: " << result[i]->getWeight() << std::endl;
        }
        std::cout << "总权重: " << totalWeight << std::endl;
    }
    else {
        std::cout << "\n图不连通，无法生成完整的最小生成树" << std::endl;
        std::cout << "只找到了 " << resultCount << " 条边，需要 " << (vertices - 1) << " 条边" << std::endl;
    }

    // 释放内存
    for (int i = 0; i < edgeCount; i++) {
        delete edges[i];
    }
    delete[] edges;

    for (int i = 0; i < resultCount; i++) {
        delete result[i];
    }
    delete[] result;
}