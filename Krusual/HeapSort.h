#ifndef HEAPSORT_H
#define HEAPSORT_H

#include "Edge.h" // 包含Edge类定义

class HeapSort {
private:
    Edge* tempArray; // 第三个数组，用于辅助排序

    // 递归调整堆（最大堆）
    void heapify(Edge* edges[], int n, int i);

    // 构建最大堆
    void buildHeap(Edge* edges[], int n);

public:
    HeapSort();
    ~HeapSort();

    // 堆排序主函数
    void sort(Edge* edges[], int n);

    // 获取排序过程中的比较次数（用于性能分析）
    int getCompareCount() const { return compareCount; }

    // 获取交换次数
    int getSwapCount() const { return swapCount; }

private:
    int compareCount; // 比较次数统计
    int swapCount;    // 交换次数统计
};

#endif