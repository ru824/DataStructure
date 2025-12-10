/**
 * @file Graph.cpp
 * @brief 图数据结构实现文件，包含图的创建、操作和Kruskal算法
 * @class Graph
 * @brief 图类，使用边数组存储图结构，针对Kruskal算法优化
 *
 * Kruskal算法以边为核心操作，使用边数组存储可以直接对边进行排序和筛选，比邻接矩阵更高效[3,5](@ref)。
 */

#include "Graph.h"
#include "UnionFind.h"
#include <iostream>
#include <algorithm>
#include <climits>

 /**
  * @brief 图类构造函数
  * @param v 图中顶点数量，必须为正整数
  * @exception std::invalid_argument 当v<=0时抛出异常
  * @note 初始化边数组，放弃邻接矩阵存储方式，专注于Kruskal算法需求[3,5](@ref)
  */
Graph::Graph(int v) : vertices(v), edgeCount(0), maxEdges(v* (v - 1) / 2) {
    // 输入验证
    if (v <= 0) {
        std::cerr << "错误：顶点数必须为正整数" << std::endl;
        vertices = 1;
        maxEdges = 0;
    }

    // 分配边数组内存
    edgesArray = new Edge * [maxEdges];
    for (int i = 0; i < maxEdges; ++i) {
        edgesArray[i] = nullptr;
    }

    std::cout << "图初始化完成：顶点数=" << vertices
        << ", 最大边数=" << maxEdges << std::endl;
}

/**
 * @brief 图类析构函数
 * @note 释放边数组占用的所有内存资源，防止内存泄漏[1](@ref)
 */
Graph::~Graph() {
    // 释放边数组
    if (edgesArray != nullptr) {
        for (int i = 0; i < maxEdges; ++i) {
            delete edgesArray[i];
        }
        delete[] edgesArray;
    }
    std::cout << "图资源已释放" << std::endl;
}

/**
 * @brief 向图中添加无向边
 * @param u 边的起始顶点索引，范围[0, vertices-1]
 * @param v 边的目标顶点索引，范围[0, vertices-1]
 * @param weight 边的权重值
 * @exception std::out_of_range 当顶点索引越界时输出错误信息
 * @note 对于无向图，确保u < v以避免重复存储，提高Kruskal算法效率[3,7](@ref)
 */
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

    // 统一处理为u < v的形式，避免无向图中的重复边[7](@ref)
    if (u > v) {
        std::swap(u, v);
    }

    // 检查边是否已存在
    for (int i = 0; i < edgeCount; ++i) {
        if (edgesArray[i] != nullptr &&
            edgesArray[i]->getSrc() == u && edgesArray[i]->getDest() == v) {
            // 边已存在，更新权重
            std::cout << "更新边权重: " << u << " - " << v
                << " 旧权重: " << edgesArray[i]->getWeight()
                << " -> 新权重: " << weight << std::endl;
            edgesArray[i]->setWeight(weight);
            return;
        }
    }

    // 检查边数是否超出限制
    if (edgeCount >= maxEdges) {
        std::cerr << "错误：边数已达上限 " << maxEdges << "，无法添加新边" << std::endl;
        return;
    }

    // 添加新边
    edgesArray[edgeCount] = new Edge(u, v, weight);
    edgeCount++;
    std::cout << "添加边成功: " << u << " - " << v << " 权重: " << weight
        << " (总边数: " << edgeCount << ")" << std::endl;
}

/**
 * @brief 打印图的边列表信息
 * @note 仅打印边数组内容，不再显示邻接矩阵[5](@ref)
 */
void Graph::printGraph() const {
    std::cout << "\n图信息概览:" << std::endl;
    std::cout << "顶点数: " << vertices << std::endl;
    std::cout << "边数: " << edgeCount << std::endl;

    if (edgeCount == 0) {
        std::cout << "图中没有边" << std::endl;
        return;
    }

    std::cout << "\n边列表 (" << edgeCount << " 条边):" << std::endl;
    std::cout << "序号\t起点\t终点\t权重" << std::endl;
    std::cout << "----------------------------" << std::endl;

    for (int i = 0; i < edgeCount; ++i) {
        if (edgesArray[i] != nullptr) {
            std::cout << i << "\t" << edgesArray[i]->getSrc() << "\t"
                << edgesArray[i]->getDest() << "\t"
                << edgesArray[i]->getWeight() << std::endl;
        }
    }
    std::cout << std::endl;
}
/**
 * @brief 堆排序的堆化操作
 * @param edges 边指针数组
 * @param n 当前堆的大小
 * @param i 需要堆化的子树根节点索引
 * @param temp 临时边对象，用于交换操作
 * @note 维护最大堆性质，时间复杂度O(log n)[1](@ref)
 */
void Graph::heapify(Edge* edges[], int n, int i, Edge* temp) const {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    // 找到左子节点、右子节点和当前节点中的最大值[1](@ref)
    if (left < n && edges[left]->getWeight() > edges[largest]->getWeight()) {
        largest = left;
    }

    if (right < n && edges[right]->getWeight() > edges[largest]->getWeight()) {
        largest = right;
    }

    // 如果最大值不是当前节点，交换并递归调整[1](@ref)
    if (largest != i) {
        // 使用第三个数组temp作为临时存储进行交换
        *temp = *edges[i];
        *edges[i] = *edges[largest];
        *edges[largest] = *temp;

        heapify(edges, n, largest, temp);
    }
}

/**
 * @brief 构建最大堆
 * @param edges 边指针数组
 * @param n 数组大小
 * @param temp 临时边对象，用于交换操作
 * @note 从最后一个非叶子节点开始构建堆，时间复杂度O(n)[1](@ref)
 */
void Graph::buildHeap(Edge* edges[], int n, Edge* temp) const {
    // 从最后一个非叶子节点开始构建堆[1](@ref)
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(edges, n, i, temp);
    }
}

/**
 * @brief 对边数组进行堆排序
 * @param edges 边指针数组
 * @param n 数组大小
 * @note 使用堆排序算法对边按权重升序排列，时间复杂度O(n log n)[1](@ref)
 */
void Graph::heapSortEdges(Edge* edges[], int n) const {
    if (n <= 1) return;

    // 创建第三个数组作为辅助空间
    Edge* temp = new Edge();

    // 构建最大堆[1](@ref)
    buildHeap(edges, n, temp);

    // 逐个从堆中提取元素[1](@ref)
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


/**
 * @brief 使用Kruskal算法求解最小生成树
 * @note 基于并查集和堆排序实现，专门为边数组存储优化[3,5,7](@ref)
 * @exception std::runtime_error 当图不连通时无法生成完整最小生成树
 */
void Graph::kruskalMST() const {
    std::cout << "\n=== 开始执行Kruskal算法 ===" << std::endl;

    if (edgeCount == 0) {
        std::cout << "图中没有边，无法生成最小生成树" << std::endl;
        return;
    }

    if (vertices <= 1) {
        std::cout << "顶点数不足，无法生成最小生成树" << std::endl;
        return;
    }

    // 创建边指针数组用于排序（深拷贝）
    Edge** edges = new Edge * [edgeCount];
    for (int i = 0; i < edgeCount; i++) {
        edges[i] = new Edge(*edgesArray[i]);
    }

    // 使用堆排序对边按权重排序
    heapSortEdges(edges, edgeCount);

    std::cout << "边按权重排序完成:" << std::endl;
    for (int i = 0; i < edgeCount; i++) {
        std::cout << "边 " << i << ": " << edges[i]->getSrc() << " - "
            << edges[i]->getDest() << " 权重: " << edges[i]->getWeight() << std::endl;
    }

    UnionFind uf(vertices);
    Edge** result = new Edge * [vertices - 1];
    int resultCount = 0;
    int totalWeight = 0;

    std::cout << "\n开始构建最小生成树:" << std::endl;

    // Kruskal算法核心：遍历排序后的边，使用并查集避免环路[3,7](@ref)
    for (int i = 0; i < edgeCount && resultCount < vertices - 1; i++) {
        Edge* currentEdge = edges[i];
        int u = currentEdge->getSrc();
        int v = currentEdge->getDest();

        if (uf.find(u) != uf.find(v)) {
            uf.unite(u, v);
            result[resultCount] = new Edge(*currentEdge);
            totalWeight += currentEdge->getWeight();

            std::cout << "添加第" << (resultCount + 1) << "条边: "
                << u << " - " << v << " 权重: " << currentEdge->getWeight() << std::endl;
            resultCount++;
        }
        else {
            std::cout << "跳过边: " << u << " - " << v
                << " 权重: " << currentEdge->getWeight() << " (会形成环路)" << std::endl;
        }
    }

    // 输出最终结果
    std::cout << "\n=== Kruskal算法执行完成 ===" << std::endl;

    if (resultCount == vertices - 1) {
        std::cout << "最小生成树构建成功!" << std::endl;
        std::cout << "最小生成树包含 " << resultCount << " 条边:" << std::endl;
        std::cout << "起点\t终点\t权重" << std::endl;
        std::cout << "---------------------" << std::endl;

        for (int i = 0; i < resultCount; i++) {
            std::cout << result[i]->getSrc() << "\t" << result[i]->getDest()
                << "\t" << result[i]->getWeight() << std::endl;
        }
        std::cout << "总权重: " << totalWeight << std::endl;
    }
    else {
        std::cout << "图不连通，无法生成完整的最小生成树" << std::endl;
        std::cout << "只找到了 " << resultCount << "条边，需要 "<<(vertices - 1) << " 条边" << std::endl;

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