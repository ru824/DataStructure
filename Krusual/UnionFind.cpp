#include "UnionFind.h"

// 构造函数：初始化并查集，每个元素自成一集合
UnionFind::UnionFind(int n) : size(n) {
    parent = new int[n];
    rank = new int[n];

    // 初始化：每个元素的父节点指向自己，秩初始为0
    for (int i = 0; i < n; ++i) {
        parent[i] = i;
        rank[i] = 0;
    }
}

// 析构函数：释放动态分配的内存
UnionFind::~UnionFind() {
    delete[] parent;
    delete[] rank;
}

// Find操作（带路径压缩）
int UnionFind::find(int x) {
    // 查找根节点
    int root = x;
    while (root != parent[root]) {
        root = parent[root];
    }

    // 路径压缩：将查找路径上所有节点直接指向根节点
    while (x != root) {
        int next = parent[x]; // 暂存原父节点
        parent[x] = root;     // 当前节点直接指向根
        x = next;             // 继续处理原父节点
    }
    return root;
}

// Union/Unite操作（按秩合并）
void UnionFind::unite(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);

    // 如果已在同一集合，无需合并
    if (rootX == rootY) return;

    // 按秩合并：将秩较小的树合并到秩较大的树下
    if (rank[rootX] < rank[rootY]) {
        parent[rootX] = rootY;
    }
    else if (rank[rootX] > rank[rootY]) {
        parent[rootY] = rootX;
    }
    else {
        // 秩相同时，任意合并，但新根的秩需加1
        parent[rootY] = rootX;
        rank[rootX]++;
    }
}

// 检查连通性
bool UnionFind::isConnected(int x, int y) {
    return find(x) == find(y);
}