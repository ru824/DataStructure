#include "GraphFileIO.h"
#include <cstdio>
#include <cctype>
#include <cstdlib>
#define _CRT_SECURE_NO_WARNINGS
// 字符串长度计算
int GraphFileIO::stringLength(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

// 安全字符串拷贝
void GraphFileIO::stringCopy(char* dest, const char* src, int maxLen) {
    int i = 0;
    while (i < maxLen - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// 字符串比较
int GraphFileIO::stringCompare(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// 字符串转整数
int GraphFileIO::stringToInt(const char* str) {
    int result = 0;
    int sign = 1;
    int i = 0;

    if (str[0] == '-') {
        sign = -1;
        i++;
    }
    else if (str[0] == '+') {
        i++;
    }

    while (str[i] != '\0') {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
            i++;
        }
        else {
            break;
        }
    }

    return sign * result;
}

// 去除字符串两端空白
void GraphFileIO::trimString(char* str) {
    if (str == nullptr) return;

    int len = stringLength(str);
    int start = 0, end = len - 1;

    while (start < len && (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r')) {
        start++;
    }

    while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r')) {
        end--;
    }

    if (start > 0) {
        int i = 0;
        for (int j = start; j <= end; j++, i++) {
            str[i] = str[j];
        }
        str[i] = '\0';
    }
    else if (end < len - 1) {
        str[end + 1] = '\0';
    }
}

// 分割字符串
int GraphFileIO::splitString(const char* str, char delimiter, char tokens[][20], int maxTokens) {
    char buffer[256];
    stringCopy(buffer, str, 256);

    int tokenCount = 0;
    int pos = 0;
    int tokenStart = 0;
    int bufferLen = stringLength(buffer);

    while (pos <= bufferLen && tokenCount < maxTokens) {
        if (buffer[pos] == delimiter || buffer[pos] == '\0') {
            int tokenLen = pos - tokenStart;
            if (tokenLen > 0 && tokenLen < 20) {
                // 拷贝token
                for (int i = 0; i < tokenLen; i++) {
                    tokens[tokenCount][i] = buffer[tokenStart + i];
                }
                tokens[tokenCount][tokenLen] = '\0';
                trimString(tokens[tokenCount]);
                tokenCount++;
            }
            tokenStart = pos + 1;
        }
        pos++;
    }

    return tokenCount;
}

// 读取一行文本
bool GraphFileIO::readLine(FILE* file, char* buffer, int maxSize) {
    if (file == nullptr || feof(file)) return false;

    int i = 0;
    int ch;

    while (i < maxSize - 1 && (ch = fgetc(file)) != EOF) {
        if (ch == '\n') break;
        buffer[i++] = (char)ch;
    }

    buffer[i] = '\0';
    return i > 0 || ch != EOF;
}

// 构造函数
GraphFileIO::GraphFileIO(const char* file) {
    stringCopy(filename, file, 256);
}

// 从文件读取图数据（仅支持标准图格式）
Graph* GraphFileIO::readGraphFromFile() {
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0 || file == nullptr) {
        printf("错误：无法打开文件 %s，错误码: %d\n", filename, err);
        return nullptr;
    }
    if (file == nullptr) {
        printf("错误：无法打开文件 %s\n", filename);
        return nullptr;
    }

    char line[256];
    int vertices = 0;
    int edges = 0;
    int lineNumber = 0;
    int edgesAdded = 0;

    printf("开始读取标准图格式文件: %s\n", filename);

    Graph* graph = nullptr;

    // 读取第一行：顶点数和边数
    if (readLine(file, line, 256)) {
        lineNumber++;
        trimString(line);

        if (stringLength(line) == 0) {
            printf("错误：文件为空或第一行为空\n");
            fclose(file);
            return nullptr;
        }

        // 解析顶点数和边数
        char tokens[2][20];
        int tokenCount = splitString(line, ' ', tokens, 2);

        if (tokenCount >= 2) {
            vertices = stringToInt(tokens[0]);
            edges = stringToInt(tokens[1]);

            // 输入验证
            if (vertices <= 0 || vertices > 1000) {
                printf("错误：顶点数超出有效范围: %d（有效范围: 1-1000）\n", vertices);
                fclose(file);
                return nullptr;
            }

            if (edges < 0 || edges > vertices * (vertices - 1) / 2) {
                printf("警告：边数可能不合理: %d（最大可能边数: %d）\n",
                    edges, vertices * (vertices - 1) / 2);
            }

            printf("顶点数: %d, 边数: %d\n", vertices, edges);

            // 创建图对象
            graph = new Graph(vertices);
        }
        else {
            printf("错误：第一行格式不正确，应为：顶点数 边数\n");
            fclose(file);
            return nullptr;
        }
    }
    else {
        printf("错误：无法读取文件第一行\n");
        fclose(file);
        return nullptr;
    }

    // 读取边数据
    while (readLine(file, line, 256)) {
        lineNumber++;
        trimString(line);
        if (stringLength(line) == 0) continue; // 跳过空行

        char tokens[3][20];
        int tokenCount = splitString(line, ' ', tokens, 3);

        if (tokenCount >= 3) {
            try {
                int src = stringToInt(tokens[0]);
                int dest = stringToInt(tokens[1]);
                int weight = stringToInt(tokens[2]);

                // 边界检查
                if (src < 0 || src >= vertices || dest < 0 || dest >= vertices) {
                    printf("警告：第 %d 行顶点索引越界 (%d, %d)，最大索引为 %d，已跳过\n",
                        lineNumber, src, dest, vertices - 1);
                    continue;
                }

                if (src == dest) {
                    printf("警告：第 %d 行忽略自环边 (%d->%d)\n", lineNumber, src, dest);
                    continue;
                }

                graph->addEdge(src, dest, weight);
                edgesAdded++;

                printf("添加边: %d - %d 权重: %d\n", src, dest, weight);

            }
            catch (...) {
                printf("警告：第 %d 行数据格式错误，已跳过\n", lineNumber);
            }
        }
        else {
            printf("警告：第 %d 行格式错误（应有3个参数），已跳过\n", lineNumber);
        }
    }

    fclose(file);

    // 统计信息
    printf("文件读取完成\n");
    printf("预期边数: %d, 成功添加: %d\n", edges, edgesAdded);

    if (edgesAdded < edges) {
        printf("警告：实际添加的边数少于预期（可能因为数据格式错误或重复边）\n");
    }
    else if (edgesAdded > edges) {
        printf("警告：实际添加的边数多于预期（文件可能包含额外数据）\n");
    }

    return graph;
}