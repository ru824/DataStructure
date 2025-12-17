/**
 * @file GraphFileIO.cpp
 * @brief 图文件输入输出工具类实现（非STL版本）
 */

#include "GraphFileIO.h"
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <stdexcept>

 // 计算字符串长度
int GraphFileIO::stringLength(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

// 安全字符串复制
void GraphFileIO::stringCopy(char* dest, const char* src, int maxLen) {
    int i = 0;
    while (i < maxLen - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// 去除字符串两端空白
void GraphFileIO::trimString(char* str) {
    if (str == nullptr) return;

    int len = stringLength(str);
    int start = 0, end = len - 1;

    // 找到第一个非空白字符
    while (start <= end && (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r')) {
        start++;
    }

    // 找到最后一个非空白字符
    while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r')) {
        end--;
    }

    // 移动字符串内容
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

// 字符串转整数
int GraphFileIO::stringToInt(const char* str) {
    if (str == nullptr) return 0;

    int result = 0;
    int sign = 1;
    int i = 0;

    // 处理符号
    if (str[0] == '-') {
        sign = -1;
        i++;
    }
    else if (str[0] == '+') {
        i++;
    }

    // 转换数字
    while (str[i] != '\0') {
        if (str[i] >= '0' && str[i] <= '9') {
            // 检查溢出
            if (result > INT_MAX / 10 || (result == INT_MAX / 10 && (str[i] - '0') > INT_MAX % 10)) {
                return sign == 1 ? INT_MAX : INT_MIN;
            }
            result = result * 10 + (str[i] - '0');
            i++;
        }
        else {
            break;  // 遇到非数字字符停止
        }
    }

    return sign * result;
}

// 字符串转浮点数
double GraphFileIO::stringToDouble(const char* str) {
    if (str == nullptr) return 0.0;

    double result = 0.0;
    double fraction = 1.0;
    int sign = 1;
    int i = 0;
    bool hasDecimal = false;

    // 处理符号
    if (str[0] == '-') {
        sign = -1;
        i++;
    }
    else if (str[0] == '+') {
        i++;
    }

    // 转换整数部分和小数部分
    while (str[i] != '\0') {
        if (str[i] == '.') {
            hasDecimal = true;
            i++;
            continue;
        }

        if (str[i] >= '0' && str[i] <= '9') {
            if (!hasDecimal) {
                result = result * 10.0 + (str[i] - '0');
            }
            else {
                fraction *= 0.1;
                result += (str[i] - '0') * fraction;
            }
            i++;
        }
        else {
            break;  // 遇到非数字字符停止
        }
    }

    return sign * result;
}

// 计算欧几里得距离
double GraphFileIO::calculateDistance(double x1, double y1, double x2, double y2) {
    // 输入验证
    if (std::isnan(x1) || std::isnan(y1) || std::isnan(x2) || std::isnan(y2)) {
        return -1.0;  // 错误标识
    }

    double deltaX = x2 - x1;
    double deltaY = y2 - y1;

    return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}

// 读取顶点数量
int GraphFileIO::readVertexCount(FILE* file, const char* filename) {
    char line[MAX_LINE_LENGTH];

    // 读取第一行
    if (std::fgets(line, MAX_LINE_LENGTH, file) == nullptr) {
        std::fprintf(stderr, "错误：无法读取文件第一行 %s\n", filename);
        return -1;
    }

    trimString(line);
    if (stringLength(line) == 0) {
        std::fprintf(stderr, "错误：文件第一行为空 %s\n", filename);
        return -1;
    }

    int vertexCount = stringToInt(line);

    // 验证顶点数量
    if (vertexCount <= 0) {
        std::fprintf(stderr, "错误：顶点数必须为正整数，得到 %d\n", vertexCount);
        return -1;
    }

    if (vertexCount > MAX_NODES) {
        std::fprintf(stderr, "错误：顶点数超出最大限制 %d > %d\n", vertexCount, MAX_NODES);
        return -1;
    }

    return vertexCount;
}

// 读取顶点坐标
int GraphFileIO::readCoordinates(FILE* file, int vertexCount, double* xCoords, double* yCoords, const char* filename) {
    char line[MAX_LINE_LENGTH];
    int coordinatesRead = 0;

    for (int i = 0; i < vertexCount; i++) {
        if (std::fgets(line, MAX_LINE_LENGTH, file) == nullptr) {
            std::fprintf(stderr, "错误：文件过早结束，期望 %d 行坐标，但只读到第 %d 行\n", vertexCount, i);
            return -1;
        }

        trimString(line);
        if (stringLength(line) == 0) {
            continue;  // 跳过空行
        }

        // 解析两个浮点数
        char* token = line;
        int coordIndex = 0;
        bool success = true;

        while (*token != '\0' && coordIndex < 2) {
            // 跳过空白
            while (*token == ' ' || *token == '\t') token++;

            if (*token == '\0') break;

            // 提取数字
            char number[32];
            int numIndex = 0;

            while ((*token >= '0' && *token <= '9') || *token == '.' || *token == '-' || *token == '+') {
                if (numIndex < 31) {
                    number[numIndex++] = *token;
                }
                token++;
            }
            number[numIndex] = '\0';

            if (numIndex > 0) {
                double value = stringToDouble(number);

                if (coordIndex == 0) {
                    xCoords[coordinatesRead] = value;
                }
                else {
                    yCoords[coordinatesRead] = value;
                }
                coordIndex++;
            }

            // 跳过分隔符
            while (*token == ' ' || *token == '\t') token++;
        }

        if (coordIndex == 2) {
            coordinatesRead++;
        }
        else {
            std::fprintf(stderr, "警告：第 %d 行坐标格式无效: %s\n", i + 2, line);
        }
    }

    return coordinatesRead;
}

// 主功能：读取节点并构建图
bool GraphFileIO::readNodesAndBuildGraph(Graph& graph, const char* filename) {
    std::printf("开始从文件读取节点数据: %s\n", filename);

    // 打开文件
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0 || file == nullptr) {
        std::fprintf(stderr, "错误：无法打开文件 %s，错误码: %d\n", filename, err);
        return false; // 或适当的错误处理
    }

    // 分配坐标数组内存
    double* xCoords = new double[MAX_NODES];
    double* yCoords = new double[MAX_NODES];

    if (xCoords == nullptr || yCoords == nullptr) {
        std::fprintf(stderr, "错误：内存分配失败\n");
        std::fclose(file);
        delete[] xCoords;
        delete[] yCoords;
        return false;
    }

    int vertexCount = 0;
    int coordinatesRead = 0;
    bool success = false;

    try {
        // 读取顶点数量
        vertexCount = readVertexCount(file, filename);
        if (vertexCount <= 0) {
            throw std::runtime_error("无效的顶点数量");
        }

        std::printf("文件声明顶点数量: %d\n", vertexCount);

        // 检查图对象容量
        if (graph.getVertices() < vertexCount) {
            char buffer[100];
            sprintf_s(buffer, sizeof(buffer), "图对象容量不足。需要 %d 个顶点，但当前图仅支持 %d 个顶点",
                vertexCount, graph.getVertices());
            throw std::runtime_error(buffer);
        }

        // 读取坐标
        coordinatesRead = readCoordinates(file, vertexCount, xCoords, yCoords, filename);
        if (coordinatesRead != vertexCount) {
            std::fprintf(stderr, "警告：期望 %d 个坐标，但成功读取 %d 个\n", vertexCount, coordinatesRead);
            vertexCount = coordinatesRead;  // 使用实际读取的数量
        }

        std::printf("成功读取 %d 个节点的坐标\n", vertexCount);

        // 为每对节点创建边
        int edgesAdded = 0;
        int skippedCount = 0;

        for (int i = 0; i < vertexCount; i++) {
            for (int j = i + 1; j < vertexCount; j++) {
                double distance = calculateDistance(xCoords[i], yCoords[i], xCoords[j], yCoords[j]);

                if (distance < 0 || std::isnan(distance) || std::isinf(distance)) {
                    std::fprintf(stderr, "警告：节点 %d 和 %d 的距离计算无效: %.2f，跳过\n", i, j, distance);
                    skippedCount++;
                    continue;
                }

                // 四舍五入为整数权重
                int weight = static_cast<int>(distance + 0.5);

                // 添加边到图中
                graph.addEdge(i, j, weight);
                edgesAdded++;
            }
        }

        std::printf("图构建完成。成功添加 %d 条边，跳过 %d 个无效边\n", edgesAdded, skippedCount);
        success = true;

    }
    catch (const std::exception& e) {
        std::fprintf(stderr, "处理文件时发生错误: %s\n", e.what());
        success = false;
    }

    // 清理资源
    std::fclose(file);
    delete[] xCoords;
    delete[] yCoords;

    return success;
}

// 文件验证
bool GraphFileIO::validateFile(const char* filename) {
    std::printf("验证文件格式: %s\n", filename);

    FILE* file = nullptr;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0 || file == nullptr) {
        std::fprintf(stderr, "错误：无法打开文件 %s，错误码: %d\n", filename, err);
        return false; // 或适当的错误处理
    }

    bool isValid = false;
    double* testX = nullptr;
    double* testY = nullptr;

    try {
        int vertexCount = readVertexCount(file, filename);
        if (vertexCount <= 0) {
            throw std::runtime_error("无效的顶点数量");
        }

        // 分配测试数组
        testX = new double[vertexCount];
        testY = new double[vertexCount];

        // 尝试读取前几个坐标验证格式
        int coordsToRead = (vertexCount < 5) ? vertexCount : 5;
        int coordsRead = readCoordinates(file, coordsToRead, testX, testY, filename);

        if (coordsRead != coordsToRead) {
            throw std::runtime_error("坐标读取数量不符合预期");
        }

        std::printf("文件格式验证通过。顶点数量: %d\n", vertexCount);
        isValid = true;

    }
    catch (const std::exception& e) {
        std::fprintf(stderr, "文件格式验证失败: %s\n", e.what());
        isValid = false;
    }

    std::fclose(file);
    delete[] testX;
    delete[] testY;

    return isValid;
}