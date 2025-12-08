#include "HeapSort.h"
#include <iostream>

HeapSort::HeapSort() : tempArray(new Edge[1]), compareCount(0), swapCount(0) {
    // 初始化辅助数组
}

HeapSort::~HeapSort() {
    delete[] tempArray;
}

// 递归堆调整函数
void HeapSort::heapify(Edge* edges[], int n, int i) {
    int largest = i;       // 初始化最大元素为当前节点
    int left = 2 * i + 1;  // 左子节点
    int right = 2 * i + 2; // 右子节点

    compareCount += 2; // 统计比较次数

    // 检查左子节点是否存在且大于当前最大节点
    if (left < n) {
        compareCount++;
        if (edges[left]->getWeight() > edges[largest]->getWeight()) {
            largest = left;
        }
    }

    // 检查右子节点是否存在且大于当前最大节点
    if (right < n) {
        compareCount++;
        if (edges[right]->getWeight() > edges[largest]->getWeight()) {
            largest = right;
        }
    }

    // 如果最大元素不是当前节点，交换并递归调整
    if (largest != i) {
        // 使用第三个数组tempArray作为临时存储进行交换
        *tempArray = *edges[i];        // 暂存edges[i]到tempArray
        *edges[i] = *edges[largest];   // 将较大值移到当前位置
        *edges[largest] = *tempArray;   // 完成交换

        swapCount++; // 统计交换次数

        // 递归调整被影响的子树
        heapify(edges, n, largest);
    }
}

// 构建最大堆
void HeapSort::buildHeap(Edge* edges[], int n) {
    // 从最后一个非叶子节点开始构建堆
    // 最后一个非叶子节点的索引为 n/2 - 1
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(edges, n, i);
    }
}

// 堆排序主函数
void HeapSort::sort(Edge* edges[], int n) {
    if (n <= 1) return; // 边界情况处理

    // 重置统计计数器
    compareCount = 0;
    swapCount = 0;

    std::cout << "开始堆排序，元素数量: " << n << std::endl;

    // 构建最大堆
    buildHeap(edges, n);

    std::cout << "最大堆构建完成，比较次数: " << compareCount << std::endl;

    // 逐个从堆中提取元素
    for (int i = n - 1; i > 0; i--) {
        // 将当前堆顶元素（最大值）与末尾元素交换
        *tempArray = *edges[0];
        *edges[0] = *edges[i];
        *edges[i] = *tempArray;

        swapCount++;

        // 调整剩余元素，堆大小减1
        heapify(edges, i, 0);
    }

    std::cout << "堆排序完成，总比较次数: " << compareCount
        << "，总交换次数: " << swapCount << std::endl;
}