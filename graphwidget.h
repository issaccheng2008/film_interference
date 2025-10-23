#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QPoint>
#include <QPointF>
#include <QVector>
#include <QWidget>

class QMouseEvent;
class QPainter;
class QRect;
class QWheelEvent;

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr);

    void setParameters(double e, double n1, double n2, double n3, double lambda, double f);

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void regenerateData();
    void resetView();
    void clampView();
    QRect computePlotRect() const;
    QPointF dataToScreen(const QPointF &sample, const QRect &plotRect) const;
    QPointF screenToData(const QPointF &point, const QRect &plotRect) const;
    void drawAxes(QPainter &painter, const QRect &plotRect);
    void translateView(double dx, double dy);

    double m_e = 0.0;
    double m_n1 = 0.0;
    double m_n2 = 0.0;
    double m_n3 = 0.0;
    double m_lambda = 0.0;
    double m_f = 0.0;

    QVector<QPointF> m_samples;
    double m_dataXMin = 0.0;
    double m_dataXMax = 1.0;
    double m_dataYMin = 0.0;
    double m_dataYMax = 1.0;
    double m_viewXMin = 0.0;
    double m_viewXMax = 1.0;
    double m_viewYMin = 0.0;
    double m_viewYMax = 1.0;
    bool m_isPanning = false;
    QPoint m_lastMousePos;
};

#endif // GRAPHWIDGET_H
