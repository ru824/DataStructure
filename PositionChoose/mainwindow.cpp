#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QLinearGradient>

/**
 * @brief 构造函数
 * @param parent 父窗口
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_hasCalculated(false)
    , m_gridSize(50)
    , m_showGridMode(false)  // 默认显示XY坐标图
{
    ui->setupUi(this);
    initializeUI();
}

/**
 * @brief 析构函数
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief 初始化界面
 */
void MainWindow::initializeUI()
{
    // 设置窗口标题
    setWindowTitle("邮局选址问题");
    
    // 设置窗口大小和最小尺寸
    resize(1400, 900);
    setMinimumSize(1200, 700);
    
    // 连接信号和槽
    connect(ui->btnCalculate, &QPushButton::clicked, this, &MainWindow::calculateOptimalLocation);
    connect(ui->btnClear, &QPushButton::clicked, this, &MainWindow::clearAllAreas);
    connect(ui->btnRandom, &QPushButton::clicked, this, &MainWindow::generateRandomAreas);
    
    // 初始化绘图区域 - frame框现在在最左侧，绘图区域在右侧
    int padding = 20;
    int frameWidth = ui->frame->width();
    int availableWidth = this->width() - frameWidth - padding * 2; // 减去控制面板宽度和边距
    int availableHeight = this->height() - 100; // 减去状态栏和边距
    
    // 设置绘图区域尺寸，确保不小于最小尺寸
    int drawingWidth = qMax(800, availableWidth);
    int drawingHeight = qMax(600, availableHeight);
    
    // 绘图区域位于frame框右侧，保持适当间距
    m_drawingArea = QRect(frameWidth + padding, padding, drawingWidth, drawingHeight);
    m_logicalBounds = QRect(-200, -200, 400, 400); // 逻辑坐标范围
    m_gridSize = 50; // 网格尺寸
    
    // 计算缩放比例
    m_scaleX = static_cast<double>(m_drawingArea.width()) / m_logicalBounds.width();
    m_scaleY = static_cast<double>(m_drawingArea.height()) / m_logicalBounds.height();
    
    // 更新状态栏
    updateStatusBar();
}

/**
 * @brief 重绘事件处理函数
 * @param event 重绘事件
 */
void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制控制面板区域背景
    painter.fillRect(ui->frame->geometry(), QColor(248, 249, 250));
    
    // 绘制绘图区域背景 - 渐变背景
    QLinearGradient gradient(m_drawingArea.topLeft(), m_drawingArea.bottomRight());
    gradient.setColorAt(0, QColor(255, 255, 255));
    gradient.setColorAt(1, QColor(245, 245, 245));
    painter.fillRect(m_drawingArea, gradient);
    
    // 绘制绘图区域边框
    painter.setPen(QPen(QColor(222, 226, 230), 2));
    painter.drawRect(m_drawingArea.adjusted(0, 0, -1, -1));
    
    // 保存绘图状态
    painter.save();
    
    // 设置绘图区域为m_drawingArea
    painter.translate(m_drawingArea.topLeft());
    
    // 根据显示模式选择绘制方式
    if (m_showGridMode) {
        // 网格图模式：只绘制简单的网格，不显示坐标轴和标签
        drawSimpleGrid(painter, QRect(0, 0, m_drawingArea.width(), m_drawingArea.height()));
        
        // 在网格图模式下也绘制居民小区和邮局
        drawResidentialAreas(painter);
        drawPostOffice(painter);
        
        // 如果已计算最优位置，绘制连接线
        if (m_hasCalculated) {
            drawConnections(painter);
        }
    } else {
        // XY坐标图模式：绘制完整的坐标系统
        drawGrid(painter, QRect(0, 0, m_drawingArea.width(), m_drawingArea.height()));
        
        // 绘制居民小区
        drawResidentialAreas(painter);
        
        // 绘制邮局位置
        drawPostOffice(painter);
        
        // 如果已计算最优位置，绘制连接线（在小区上方）
        if (m_hasCalculated) {
            drawConnections(painter);
        }
    }
    
    // 恢复绘图状态
    painter.restore();
}

/**
 * @brief 绘制坐标网格
 * @param painter 绘图对象
 * @param contentRect 内容区域
 */
void MainWindow::drawGrid(QPainter &painter, const QRect &contentRect)
{
    painter.save();
    
    // 设置绘图区域为内容区域
    painter.setClipRect(contentRect);
    
    // 绘制主网格线 - 更粗更明显的网格，增强小窗口下的可见性
    QPen mainGridPen(QColor(180, 180, 180, 180)); // 更深的颜色，更高的不透明度
    mainGridPen.setWidth(2); // 增加线宽
    painter.setPen(mainGridPen);
    
    // 绘制主网格线（每隔一个网格绘制更粗的线）
    for (int x = m_logicalBounds.left(); x <= m_logicalBounds.right(); x += m_gridSize * 2) {
        QPoint start = logicalToScreen(QPoint(x, m_logicalBounds.top()));
        QPoint end = logicalToScreen(QPoint(x, m_logicalBounds.bottom()));
        painter.drawLine(start, end);
    }
    
    for (int y = m_logicalBounds.top(); y <= m_logicalBounds.bottom(); y += m_gridSize * 2) {
        QPoint start = logicalToScreen(QPoint(m_logicalBounds.left(), y));
        QPoint end = logicalToScreen(QPoint(m_logicalBounds.right(), y));
        painter.drawLine(start, end);
    }
    
    // 绘制次网格线 - 较细的网格线
    QPen subGridPen(QColor(200, 200, 200, 150));
    subGridPen.setWidth(1);
    painter.setPen(subGridPen);
    
    for (int x = m_logicalBounds.left(); x <= m_logicalBounds.right(); x += m_gridSize) {
        if (x % (m_gridSize * 2) != 0) { // 跳过主网格线
            QPoint start = logicalToScreen(QPoint(x, m_logicalBounds.top()));
            QPoint end = logicalToScreen(QPoint(x, m_logicalBounds.bottom()));
            painter.drawLine(start, end);
        }
    }
    
    for (int y = m_logicalBounds.top(); y <= m_logicalBounds.bottom(); y += m_gridSize) {
        if (y % (m_gridSize * 2) != 0) { // 跳过主网格线
            QPoint start = logicalToScreen(QPoint(m_logicalBounds.left(), y));
            QPoint end = logicalToScreen(QPoint(m_logicalBounds.right(), y));
            painter.drawLine(start, end);
        }
    }
    
    // 绘制坐标轴 - 更粗更明显的坐标轴
    QPen axisPen(QColor(52, 73, 94));
    axisPen.setWidth(3);
    painter.setPen(axisPen);
    
    QPoint origin = logicalToScreen(QPoint(0, 0));
    QPoint xAxisEnd = logicalToScreen(QPoint(m_logicalBounds.right(), 0));
    QPoint yAxisEnd = logicalToScreen(QPoint(0, m_logicalBounds.bottom()));
    
    painter.drawLine(origin, xAxisEnd);
    painter.drawLine(origin, yAxisEnd);
    
    // 绘制网格标签 - 增强小窗口下的可读性
    painter.setPen(QColor(100, 100, 100, 200));
    QFont font = painter.font();
    font.setPointSize(8);
    font.setBold(true);
    painter.setFont(font);
    
    // 绘制X轴网格标签
    for (int x = m_logicalBounds.left(); x <= m_logicalBounds.right(); x += m_gridSize * 2) {
        if (x != 0) { // 跳过原点
            QPoint labelPos = logicalToScreen(QPoint(x, 0));
            painter.drawText(labelPos + QPoint(-10, 15), QString::number(x));
        }
    }
    
    // 绘制Y轴网格标签
    for (int y = m_logicalBounds.top(); y <= m_logicalBounds.bottom(); y += m_gridSize * 2) {
        if (y != 0) { // 跳过原点
            QPoint labelPos = logicalToScreen(QPoint(0, y));
            painter.drawText(labelPos + QPoint(-25, 5), QString::number(y));
        }
    }
    
    // 绘制坐标标签
    painter.setPen(QColor(44, 62, 80));
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(xAxisEnd + QPoint(10, 10), "X轴");
    painter.drawText(yAxisEnd + QPoint(10, 10), "Y轴");
    
    // 绘制原点标签
    font.setPointSize(10);
    painter.setFont(font);
    painter.drawText(origin + QPoint(-15, 15), "O(0,0)");
    
    painter.restore();
}

/**
 * @brief 绘制简单网格（网格图模式）
 * @param painter 绘图对象
 * @param contentRect 内容区域
 */
void MainWindow::drawSimpleGrid(QPainter &painter, const QRect &contentRect)
{
    painter.save();
    
    // 设置绘图区域为内容区域
    painter.setClipRect(contentRect);
    
    // 绘制简单的网格线 - 更细更简洁的网格
    QPen gridPen(QColor(220, 220, 220, 150)); // 浅灰色网格
    gridPen.setWidth(1);
    painter.setPen(gridPen);
    
    // 绘制水平网格线
    for (int y = m_logicalBounds.top(); y <= m_logicalBounds.bottom(); y += m_gridSize) {
        QPoint start = logicalToScreen(QPoint(m_logicalBounds.left(), y));
        QPoint end = logicalToScreen(QPoint(m_logicalBounds.right(), y));
        painter.drawLine(start, end);
    }
    
    // 绘制垂直网格线
    for (int x = m_logicalBounds.left(); x <= m_logicalBounds.right(); x += m_gridSize) {
        QPoint start = logicalToScreen(QPoint(x, m_logicalBounds.top()));
        QPoint end = logicalToScreen(QPoint(x, m_logicalBounds.bottom()));
        painter.drawLine(start, end);
    }
    
    // 绘制更粗的主网格线
    QPen mainGridPen(QColor(180, 180, 180, 200));
    mainGridPen.setWidth(2);
    painter.setPen(mainGridPen);
    
    // 绘制主水平网格线（每隔一个网格）
    for (int y = m_logicalBounds.top(); y <= m_logicalBounds.bottom(); y += m_gridSize * 2) {
        QPoint start = logicalToScreen(QPoint(m_logicalBounds.left(), y));
        QPoint end = logicalToScreen(QPoint(m_logicalBounds.right(), y));
        painter.drawLine(start, end);
    }
    
    // 绘制主垂直网格线（每隔一个网格）
    for (int x = m_logicalBounds.left(); x <= m_logicalBounds.right(); x += m_gridSize * 2) {
        QPoint start = logicalToScreen(QPoint(x, m_logicalBounds.top()));
        QPoint end = logicalToScreen(QPoint(x, m_logicalBounds.bottom()));
        painter.drawLine(start, end);
    }
    
    painter.restore();
}

/**
 * @brief 绘制居民小区
 * @param painter 绘图对象
 */
void MainWindow::drawResidentialAreas(QPainter &painter)
{
    painter.save();
    
    painter.setPen(QPen(QColor(52, 152, 219), 4)); // 更粗的蓝色边框
    painter.setBrush(QBrush(QColor(173, 216, 230, 200))); // 半透明浅蓝色填充
    
    for (const auto& area : m_locator.areas()) {
        QPoint screenPos = logicalToScreen(QPoint(area.x(), area.y()));
        
        // 绘制居民小区（圆形）- 显著增大尺寸
        painter.drawEllipse(screenPos, 25, 25);
        
        // 绘制小区标签
        painter.setPen(QPen(QColor(41, 128, 185), 2));
        QFont font = painter.font();
        font.setPointSize(10);
        font.setBold(true);
        painter.setFont(font);
        QString coordText = QString("(%1,%2)").arg(area.x()).arg(area.y());
        painter.drawText(screenPos + QPoint(30, -8), coordText);
        painter.setPen(QPen(QColor(52, 152, 219), 4));
    }
    
    painter.restore();
}

/**
 * @brief 绘制邮局位置
 * @param painter 绘图对象
 */
void MainWindow::drawPostOffice(QPainter &painter)
{
    if (!m_hasCalculated) return;
    
    painter.save();
    
    QPoint screenPos = logicalToScreen(m_optimalPostOffice);
    
    // 绘制邮局图标（红色圆形）- 显著增大尺寸
    painter.setBrush(QBrush(QColor(231, 76, 60, 220))); // 半透明红色填充
    painter.setPen(QPen(QColor(192, 57, 43), 3)); // 更粗的边框
    painter.drawEllipse(screenPos, 35, 35);
    
    // 绘制邮局标签
    painter.setPen(QColor(192, 57, 43));
    QFont font = painter.font();
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);
    QString labelText = QString("🏤 邮局 (%1,%2)\n💰 总费用: %3")
                        .arg(m_optimalPostOffice.x())
                        .arg(m_optimalPostOffice.y())
                        .arg(m_locator.calculateTotalCost(m_optimalPostOffice));
    painter.drawText(screenPos + QPoint(40, -15), labelText);
    
    painter.restore();
}

/**
 * @brief 绘制连接线
 * @param painter 绘图对象
 */
void MainWindow::drawConnections(QPainter &painter)
{
    if (!m_hasCalculated) return;
    
    painter.save();
    
    QPen connectionPen(QColor(149, 165, 166, 180));
    connectionPen.setWidth(4); // 增大连接线宽度
    connectionPen.setStyle(Qt::DashLine);
    painter.setPen(connectionPen);
    
    QPoint postOfficeScreen = logicalToScreen(m_optimalPostOffice);
    
    for (const auto& area : m_locator.areas()) {
        QPoint areaScreen = logicalToScreen(QPoint(area.x(), area.y()));
        painter.drawLine(postOfficeScreen, areaScreen);
    }
    
    painter.restore();
}

/**
 * @brief 鼠标点击事件处理函数
 * @param event 鼠标事件
 */
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 检查点击是否在绘图区域内
        if (m_drawingArea.contains(event->pos())) {
            QPoint contentPos = event->pos() - m_drawingArea.topLeft();
            
            QPoint logicalPos = screenToLogical(contentPos);
            
            // 调试信息：显示坐标转换结果
            qDebug() << "鼠标点击位置:" << event->pos();
            qDebug() << "内容区域位置:" << contentPos;
            qDebug() << "逻辑坐标位置:" << logicalPos;
            
            // 切换显示模式：点击绘图区域时在XY坐标图和网格图之间切换
            m_showGridMode = !m_showGridMode;
            
            // 添加居民小区
            QString areaName = QString("小区%1").arg(m_locator.areaCount() + 1);
            ResidentialArea newArea(logicalPos.x(), logicalPos.y(), areaName);
            m_locator.addArea(newArea);
            
            m_hasCalculated = false; // 重置计算状态
            
            // 直接更新主窗口显示
            update();
            updateStatusBar();
        }
    }
    
    QMainWindow::mousePressEvent(event);
}

/**
 * @brief 窗口大小改变事件处理函数
 * @param event 大小改变事件
 */
void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    
    // 动态调整绘图区域大小
    QSize windowSize = this->size();
    
    // 计算可用绘图区域大小 - frame框现在在最左侧
    int padding = 20;
    int availableWidth = windowSize.width() - ui->frame->width() - padding * 2; // 减去控制面板宽度和边距
    int availableHeight = windowSize.height() - 100; // 减去状态栏和边距
    
    // 设置绘图区域的尺寸，确保不小于最小尺寸
    int drawingWidth = qMax(800, availableWidth);
    int drawingHeight = qMax(600, availableHeight);
    
    // 更新绘图区域的实际尺寸 - 位于frame框右侧
    m_drawingArea = QRect(ui->frame->width() + padding, padding, drawingWidth, drawingHeight);
    
    // 重新计算缩放比例
    m_scaleX = static_cast<double>(m_drawingArea.width()) / m_logicalBounds.width();
    m_scaleY = static_cast<double>(m_drawingArea.height()) / m_logicalBounds.height();
    
    // 强制重绘
    update();
    updateStatusBar();
}

/**
 * @brief 计算最优邮局位置
 */
void MainWindow::calculateOptimalLocation()
{
    if (m_locator.areaCount() == 0) {
        QMessageBox::warning(this, "警告", "请先添加居民小区！");
        return;
    }
    
    m_optimalPostOffice = m_locator.findOptimalLocation();
    m_hasCalculated = true;
    
    // 更新逻辑坐标范围以包含所有点，并添加边距
    QRect range = m_locator.getCoordinateRange();
    int margin = 20; // 边距
    m_logicalBounds = QRect(range.left() - margin, range.top() - margin,
                           range.width() + 2 * margin, range.height() + 2 * margin);
    
    // 重新计算缩放比例
    m_scaleX = static_cast<double>(m_drawingArea.width()) / m_logicalBounds.width();
    m_scaleY = static_cast<double>(m_drawingArea.height()) / m_logicalBounds.height();
    
    // 更新主窗口显示
    update();
    updateStatusBar();
    
    // 显示计算结果
    int totalCost = m_locator.calculateTotalCost(m_optimalPostOffice);
    QMessageBox::information(this, "计算结果", 
                            QString("最优邮局位置: (%1, %2)\n"
                                   "总费用(距离和): %3\n"
                                   "居民小区数量: %4")
                            .arg(m_optimalPostOffice.x())
                            .arg(m_optimalPostOffice.y())
                            .arg(totalCost)
                            .arg(m_locator.areaCount()));
}

/**
 * @brief 清空所有居民小区
 */
void MainWindow::clearAllAreas()
{
    if (m_locator.areaCount() == 0) {
        QMessageBox::information(this, "提示", "当前没有居民小区可清空！");
        return;
    }
    
    int ret = QMessageBox::question(this, "确认清空", 
                                   "确定要清空所有居民小区吗？",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        m_locator.clearAreas();
        m_hasCalculated = false;
        
        // 重置逻辑坐标范围
        m_logicalBounds = QRect(-200, -200, 400, 400);
        
        // 重新计算缩放比例
        m_scaleX = static_cast<double>(m_drawingArea.width()) / m_logicalBounds.width();
        m_scaleY = static_cast<double>(m_drawingArea.height()) / m_logicalBounds.height();
        
        // 更新主窗口显示
        update();
        updateStatusBar();
    }
}

/**
 * @brief 随机生成居民小区
 */
void MainWindow::generateRandomAreas()
{
    m_locator.clearAreas();
    
    QRandomGenerator *generator = QRandomGenerator::global();
    int areaCount = generator->bounded(5, 15); // 生成5-14个小区
    
    for (int i = 0; i < areaCount; ++i) {
        int x = generator->bounded(-180, 180);
        int y = generator->bounded(-180, 180);
        QString name = QString("随机小区%1").arg(i + 1);
        m_locator.addArea(ResidentialArea(x, y, name));
    }
    
    m_hasCalculated = false;
    
    // 切换显示模式：随机生成小区时切换到网格图模式
    m_showGridMode = true;
    
    // 更新逻辑坐标范围以包含所有点，并添加边距
    QRect range = m_locator.getCoordinateRange();
    int margin = 20; // 边距
    m_logicalBounds = QRect(range.left() - margin, range.top() - margin,
                           range.width() + 2 * margin, range.height() + 2 * margin);
    
    // 重新计算缩放比例
    m_scaleX = static_cast<double>(m_drawingArea.width()) / m_logicalBounds.width();
    m_scaleY = static_cast<double>(m_drawingArea.height()) / m_logicalBounds.height();
    
    // 更新主窗口显示
    update();
    updateStatusBar();
}



/**
 * @brief 更新状态栏信息
 */
void MainWindow::updateStatusBar()
{
    QString statusText = QString("居民小区数量: %1 | ")
                        .arg(m_locator.areaCount());
    
    // 添加显示模式信息
    statusText += QString("显示模式: %1 | ")
                 .arg(m_showGridMode ? "网格图" : "XY坐标图");
    
    if (m_hasCalculated) {
        statusText += QString("最优邮局位置: (%1, %2) | 总费用: %3")
                     .arg(m_optimalPostOffice.x())
                     .arg(m_optimalPostOffice.y())
                     .arg(m_locator.calculateTotalCost(m_optimalPostOffice));
    } else {
        statusText += "请点击'计算最优邮局位置'按钮";
    }
    
    ui->statusbar->showMessage(statusText);
}

/**
 * @brief 坐标转换：逻辑坐标到屏幕坐标
 * @param logicalPoint 逻辑坐标点
 * @return 屏幕坐标点
 */
QPoint MainWindow::logicalToScreen(const QPoint &logicalPoint) const
{
    // 将逻辑坐标映射到屏幕坐标
    double normalizedX = static_cast<double>(logicalPoint.x() - m_logicalBounds.left()) / m_logicalBounds.width();
    double normalizedY = static_cast<double>(logicalPoint.y() - m_logicalBounds.top()) / m_logicalBounds.height();
    
    int screenX = static_cast<int>(normalizedX * m_drawingArea.width());
    int screenY = static_cast<int>(normalizedY * m_drawingArea.height());
    
    // Y轴翻转（屏幕坐标Y轴向下为正）
    screenY = m_drawingArea.height() - screenY;
    
    // 调试信息：显示坐标转换结果
    qDebug() << "主窗口坐标转换 - 逻辑坐标:" << logicalPoint << "-> 屏幕坐标:" << QPoint(screenX, screenY);
    
    return QPoint(screenX, screenY);
}

/**
 * @brief 坐标转换：屏幕坐标到逻辑坐标
 * @param screenPoint 屏幕坐标点
 * @return 逻辑坐标点
 */
QPoint MainWindow::screenToLogical(const QPoint &screenPoint) const
{
    // Y轴翻转（屏幕坐标Y轴向下为正）
    int flippedY = m_drawingArea.height() - screenPoint.y();
    
    // 将屏幕坐标映射到逻辑坐标
    double normalizedX = static_cast<double>(screenPoint.x()) / m_drawingArea.width();
    double normalizedY = static_cast<double>(flippedY) / m_drawingArea.height();
    
    int logicalX = static_cast<int>(m_logicalBounds.left() + normalizedX * m_logicalBounds.width());
    int logicalY = static_cast<int>(m_logicalBounds.top() + normalizedY * m_logicalBounds.height());
    
    return QPoint(logicalX, logicalY);
}
