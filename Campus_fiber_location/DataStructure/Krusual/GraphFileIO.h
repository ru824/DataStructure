/**
 * @file GraphFileIO.h
 * @brief 图文件输入输出工具类（非STL版本）
 * @class GraphFileIO
 * @brief 使用原生数组实现，负责读取节点坐标文件并构建图结构
 */

#pragma once
#ifndef GRAPH_FILE_IO_H
#define GRAPH_FILE_IO_H

#include "Graph.h"
#include <cstdio>
#include <cmath>

class GraphFileIO {
private:
    static const int MAX_NODES = 1000;  // 最大节点数
    static const int MAX_LINE_LENGTH = 256;  // 最大行长度

public:
    /**
     * @brief 从文件读取节点坐标并填充到图对象
     * @param graph 要填充的图对象引用
     * @param filename 包含节点坐标的文件路径
     * @return bool 成功返回true，失败返回false
     */
    static bool readNodesAndBuildGraph(Graph& graph, const char* filename);

    /**
     * @brief 验证文件格式是否有效
     * @param filename 要验证的文件路径
     * @return bool 文件格式有效返回true，否则返回false
     */
    static bool validateFile(const char* filename);

private:
    /**
     * @brief 计算两点间的欧几里得距离
     */
    static double calculateDistance(double x1, double y1, double x2, double y2);

    /**
     * @brief 从文件读取顶点数量
     */
    static int readVertexCount(FILE* file, const char* filename);

    /**
     * @brief 读取所有顶点坐标到数组
     */
    static int readCoordinates(FILE* file, int vertexCount, double* xCoords, double* yCoords, const char* filename);

    /**
     * @brief 字符串转换为整数
     */
    static int stringToInt(const char* str);

    /**
     * @brief 字符串转换为浮点数
     */
    static double stringToDouble(const char* str);

    /**
     * @brief 去除字符串两端空白字符
     */
    static void trimString(char* str);

    /**
     * @brief 安全的字符串复制
     */
    static void stringCopy(char* dest, const char* src, int maxLen);

    /**
     * @brief 计算字符串长度
     */
    static int stringLength(const char* str);
};

#endif // GRAPH_FILE_IO_H