#ifndef EDGE_H
#define EDGE_H

class Edge {
private:
    int src;    // 源顶点
    int dest;   // 目标顶点
    int weight; // 边权重

public:
    // 构造函数
    Edge(int s = 0, int d = 0, int w = 0);

    // 获取源顶点
    int getSrc() const;

    // 获取目标顶点
    int getDest() const;

    // 获取边权重
    int getWeight() const;

    // 设置边权重
    void setWeight(int w);
};

#endif