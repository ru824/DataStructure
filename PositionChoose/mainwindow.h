#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWidget>
#include "postoffice.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief 主窗口类，负责邮局选址问题的可视化界面
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    /**
     * @brief 重绘事件处理函数
     * @param event 重绘事件
     */
    void paintEvent(QPaintEvent *event) override;
    
    /**
     * @brief 鼠标点击事件处理函数
     * @param event 鼠标事件
     */
    void mousePressEvent(QMouseEvent *event) override;
    
    /**
     * @brief 窗口大小改变事件处理函数
     * @param event 大小改变事件
     */
    void resizeEvent(QResizeEvent *event) override;

private slots:
    /**
     * @brief 计算最优邮局位置
     */
    void calculateOptimalLocation();
    
    /**
     * @brief 清空所有居民小区
     */
    void clearAllAreas();
    
    /**
     * @brief 随机生成居民小区
     */
    void generateRandomAreas();
    
    /**
     * @brief 更新状态栏信息
     */
    void updateStatusBar();

private:
    /**
     * @brief 初始化界面
     */
    void initializeUI();
    
    /**
     * @brief 绘制坐标网格
     * @param painter 绘图对象
     * @param contentRect 内容区域
     */
    void drawGrid(QPainter &painter, const QRect &contentRect);
    
    /**
     * @brief 绘制简单网格（网格图模式）
     * @param painter 绘图对象
     * @param contentRect 内容区域
     */
    void drawSimpleGrid(QPainter &painter, const QRect &contentRect);
    
    /**
     * @brief 绘制居民小区
     * @param painter 绘图对象
     */
    void drawResidentialAreas(QPainter &painter);
    
    /**
     * @brief 绘制邮局位置
     * @param painter 绘图对象
     */
    void drawPostOffice(QPainter &painter);
    
    /**
     * @brief 绘制连接线
     * @param painter 绘图对象
     */
    void drawConnections(QPainter &painter);
    
    /**
     * @brief 坐标转换：逻辑坐标到屏幕坐标
     * @param logicalPoint 逻辑坐标点
     * @return 屏幕坐标点
     */
    QPoint logicalToScreen(const QPoint &logicalPoint) const;
    
    /**
     * @brief 坐标转换：屏幕坐标到逻辑坐标
     * @param screenPoint 屏幕坐标点
     * @return 逻辑坐标点
     */
    QPoint screenToLogical(const QPoint &screenPoint) const;

private:
    Ui::MainWindow *ui;
    PostOfficeLocator m_locator;          ///< 邮局选址器
    QPoint m_optimalPostOffice;           ///< 最优邮局位置
    bool m_hasCalculated;                 ///< 是否已计算最优位置
    QRect m_drawingArea;                  ///< 绘图区域（现在使用frame_2）
    QRect m_logicalBounds;                ///< 逻辑坐标边界
    double m_scaleX, m_scaleY;            ///< 坐标缩放比例
    int m_gridSize;                       ///< 网格大小
    bool m_showGridMode;                  ///< 是否显示网格模式（true: 网格图, false: XY坐标图）
};

#endif // MAINWINDOW_H
