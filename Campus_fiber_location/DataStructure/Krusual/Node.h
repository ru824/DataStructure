#ifndef NODE_H
#define NODE_H

class Node {
private:
    float x;
    float y;

public:
    // 构造函数
    Node(float x = 0, float y = 0);

    // 获取源顶点
    float getX() const;

    // 获取目标顶点
    float getY() const;
};

#endif