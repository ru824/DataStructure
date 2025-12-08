#ifndef GRAPHFILEIO_H
#define GRAPHFILEIO_H

#include "Graph.h"
#include <cstdio>

class GraphFileIO {
private:
    char filename[256];

    // 字符串处理函数
    static int stringLength(const char* str);
    static void stringCopy(char* dest, const char* src, int maxLen);
    static int stringCompare(const char* str1, const char* str2);
    static int stringToInt(const char* str);
    static void trimString(char* str);
    static int splitString(const char* str, char delimiter, char tokens[][20], int maxTokens);

    // 文件读取辅助函数
    static bool readLine(FILE* file, char* buffer, int maxSize);

public:
    GraphFileIO(const char* file);

    // 从文件读取图数据（仅支持标准图格式）
    Graph* readGraphFromFile();
};

#endif