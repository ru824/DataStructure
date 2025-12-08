#ifndef UNIONFIND_H
#define UNIONFIND_H

class UnionFind {
private:
    int* parent;   // 存储每个元素的父节点索引
    int* rank;     // 存储以该元素为根的树的秩（高度上界）
    int size;      // 元素的总数

public:
    // 构造函数：初始化大小为 n 的并查集
    UnionFind(int n);

    // 析构函数：释放动态数组内存
    ~UnionFind();

    // 查找元素 x 所在集合的根（代表元素），带路径压缩优化
    int find(int x);

    // 合并元素 x 和 y 所在的集合，使用按秩合并优化
    void unite(int x, int y);

    // 检查元素 x 和 y 是否属于同一集合
    bool isConnected(int x, int y);
};

#endif // UNIONFIND_H