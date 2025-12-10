#ifndef POSTOFFICE_H
#define POSTOFFICE_H

#include <QPoint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

/**
 * @brief 居民小区类，表示一个居民小区的位置
 */
class ResidentialArea {
public:
    /**
     * @brief 构造函数
     * @param x 东西向坐标
     * @param y 南北向坐标
     * @param name 小区名称
     */
    ResidentialArea(int x, int y, const QString& name = "") 
        : m_x(x), m_y(y), m_name(name) {}
    
    int x() const { return m_x; }
    int y() const { return m_y; }
    QString name() const { return m_name; }
    
    /**
     * @brief 计算到另一个点的曼哈顿距离
     * @param other 另一个点
     * @return 曼哈顿距离
     */
    int manhattanDistance(const QPoint& other) const {
        return std::abs(m_x - other.x()) + std::abs(m_y - other.y());
    }
    
    /**
     * @brief 转换为QPoint
     */
    QPoint toPoint() const { return QPoint(m_x, m_y); }
    
private:
    int m_x;
    int m_y;
    QString m_name;
};

/**
 * @brief 邮局选址算法类
 */
class PostOfficeLocator {
public:
    /**
     * @brief 添加居民小区
     * @param area 居民小区对象
     */
    void addArea(const ResidentialArea& area) {
        m_areas.push_back(area);
    }
    
    /**
     * @brief 清空所有居民小区
     */
    void clearAreas() {
        m_areas.clear();
    }
    
    /**
     * @brief 获取居民小区数量
     * @return 小区数量
     */
    size_t areaCount() const {
        return m_areas.size();
    }
    
    /**
     * @brief 获取所有居民小区
     * @return 小区列表
     */
    const std::vector<ResidentialArea>& areas() const {
        return m_areas;
    }
    
    /**
     * @brief 使用中位数方法计算最优邮局位置
     * @return 最优邮局位置
     */
    QPoint findOptimalLocation() const {
        if (m_areas.empty()) {
            return QPoint(0, 0);
        }
        
        // 分别计算x坐标和y坐标的中位数
        std::vector<int> xCoords, yCoords;
        for (const auto& area : m_areas) {
            xCoords.push_back(area.x());
            yCoords.push_back(area.y());
        }
        
        std::sort(xCoords.begin(), xCoords.end());
        std::sort(yCoords.begin(), yCoords.end());
        
        int medianX = xCoords[xCoords.size() / 2];
        int medianY = yCoords[yCoords.size() / 2];
        
        return QPoint(medianX, medianY);
    }
    
    /**
     * @brief 计算总费用（所有小区到邮局的距离之和）
     * @param postOffice 邮局位置
     * @return 总费用
     */
    int calculateTotalCost(const QPoint& postOffice) const {
        int totalCost = 0;
        for (const auto& area : m_areas) {
            totalCost += area.manhattanDistance(postOffice);
        }
        return totalCost;
    }
    
    /**
     * @brief 获取坐标范围
     * @return 包含最小和最大坐标的矩形
     */
    QRect getCoordinateRange() const {
        if (m_areas.empty()) {
            return QRect(0, 0, 100, 100);
        }
        
        int minX = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int minY = std::numeric_limits<int>::max();
        int maxY = std::numeric_limits<int>::min();
        
        for (const auto& area : m_areas) {
            minX = std::min(minX, area.x());
            maxX = std::max(maxX, area.x());
            minY = std::min(minY, area.y());
            maxY = std::max(maxY, area.y());
        }
        
        // 添加一些边距
        int margin = 20;
        return QRect(minX - margin, minY - margin, 
                    maxX - minX + 2 * margin, maxY - minY + 2 * margin);
    }
    
private:
    std::vector<ResidentialArea> m_areas;
};

#endif // POSTOFFICE_H