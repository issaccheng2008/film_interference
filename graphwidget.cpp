#include "graphwidget.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QWheelEvent>
#include <QtGlobal>
#include <QtMath>
#include <QString>

namespace
{
constexpr double kDr = 0.01;
constexpr int kLeftMargin = 60;
constexpr int kRightMargin = 20;
constexpr int kTopMargin = 20;
constexpr int kBottomMargin = 60;
constexpr double kMinSpan = 1e-3;
}

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(320, 320);
    setMouseTracking(true);
    regenerateData();
    resetView();
}

void GraphWidget::setParameters(double e, double n1, double n2, double n3, double lambda, double f)
{
    if (qFuzzyCompare(m_e, e) && qFuzzyCompare(m_n1, n1) && qFuzzyCompare(m_n2, n2) &&
        qFuzzyCompare(m_n3, n3) && qFuzzyCompare(m_lambda, lambda) && qFuzzyCompare(m_f, f)) {
        return;
    }

    m_e = e;
    m_n1 = n1;
    m_n2 = n2;
    m_n3 = n3;
    m_lambda = lambda;
    m_f = f;
    regenerateData();
    clampView();
    update();
}

void GraphWidget::regenerateData()
{
    m_samples.clear();
    for (double r = 0.0; r <= 1.0 + 1e-9; r += kDr) {
        const double phase = m_e + m_n1 + m_n2 + m_n3 + m_lambda + m_f + r;
        const double brightness = qCos(phase);
        m_samples.append(QPointF(r, brightness * brightness));
    }
}

void GraphWidget::resetView()
{
    m_dataXMin = 0.0;
    m_dataXMax = 1.0;
    m_dataYMin = 0.0;
    m_dataYMax = 1.0;

    m_viewXMin = m_dataXMin;
    m_viewXMax = m_dataXMax;
    m_viewYMin = m_dataYMin;
    m_viewYMax = m_dataYMax;
}

void GraphWidget::clampView()
{
    double xSpan = m_viewXMax - m_viewXMin;
    double ySpan = m_viewYMax - m_viewYMin;

    const double dataXSpan = m_dataXMax - m_dataXMin;
    const double dataYSpan = m_dataYMax - m_dataYMin;

    if (xSpan > dataXSpan) {
        m_viewXMin = m_dataXMin;
        m_viewXMax = m_dataXMax;
        xSpan = dataXSpan;
    }
    if (ySpan > dataYSpan) {
        m_viewYMin = m_dataYMin;
        m_viewYMax = m_dataYMax;
        ySpan = dataYSpan;
    }

    if (xSpan < kMinSpan) {
        const double mid = 0.5 * (m_viewXMin + m_viewXMax);
        m_viewXMin = mid - kMinSpan * 0.5;
        m_viewXMax = mid + kMinSpan * 0.5;
        xSpan = kMinSpan;
    }
    if (ySpan < kMinSpan) {
        const double mid = 0.5 * (m_viewYMin + m_viewYMax);
        m_viewYMin = mid - kMinSpan * 0.5;
        m_viewYMax = mid + kMinSpan * 0.5;
        ySpan = kMinSpan;
    }

    if (m_viewXMin < m_dataXMin) {
        const double shift = m_dataXMin - m_viewXMin;
        m_viewXMin += shift;
        m_viewXMax += shift;
    }
    if (m_viewXMax > m_dataXMax) {
        const double shift = m_viewXMax - m_dataXMax;
        m_viewXMin -= shift;
        m_viewXMax -= shift;
    }
    if (m_viewYMin < m_dataYMin) {
        const double shift = m_dataYMin - m_viewYMin;
        m_viewYMin += shift;
        m_viewYMax += shift;
    }
    if (m_viewYMax > m_dataYMax) {
        const double shift = m_viewYMax - m_dataYMax;
        m_viewYMin -= shift;
        m_viewYMax -= shift;
    }

    if (m_viewXMin < m_dataXMin) {
        m_viewXMin = m_dataXMin;
        m_viewXMax = m_dataXMin + xSpan;
    }
    if (m_viewXMax > m_dataXMax) {
        m_viewXMax = m_dataXMax;
        m_viewXMin = m_dataXMax - xSpan;
    }
    if (m_viewYMin < m_dataYMin) {
        m_viewYMin = m_dataYMin;
        m_viewYMax = m_dataYMin + ySpan;
    }
    if (m_viewYMax > m_dataYMax) {
        m_viewYMax = m_dataYMax;
        m_viewYMin = m_dataYMax - ySpan;
    }

    if (m_viewXMax <= m_viewXMin || m_viewYMax <= m_viewYMin) {
        resetView();
    }
}

QRect GraphWidget::computePlotRect() const
{
    return rect().adjusted(kLeftMargin, kTopMargin, -kRightMargin, -kBottomMargin);
}

QPointF GraphWidget::dataToScreen(const QPointF &sample, const QRect &plotRect) const
{
    const double xRange = m_viewXMax - m_viewXMin;
    const double yRange = m_viewYMax - m_viewYMin;
    if (xRange <= 0.0 || yRange <= 0.0 || plotRect.width() <= 0 || plotRect.height() <= 0) {
        return QPointF(plotRect.left(), plotRect.bottom());
    }

    const double xRatio = (sample.x() - m_viewXMin) / xRange;
    const double yRatio = (sample.y() - m_viewYMin) / yRange;
    const double x = plotRect.left() + xRatio * plotRect.width();
    const double y = plotRect.bottom() - yRatio * plotRect.height();
    return QPointF(x, y);
}

QPointF GraphWidget::screenToData(const QPointF &point, const QRect &plotRect) const
{
    const double xRange = m_viewXMax - m_viewXMin;
    const double yRange = m_viewYMax - m_viewYMin;
    if (xRange <= 0.0 || yRange <= 0.0 || plotRect.width() <= 0 || plotRect.height() <= 0) {
        return QPointF(m_viewXMin, m_viewYMin);
    }

    const double xRatio = (point.x() - plotRect.left()) / plotRect.width();
    const double yRatio = (plotRect.bottom() - point.y()) / plotRect.height();
    const double x = m_viewXMin + xRatio * xRange;
    const double y = m_viewYMin + yRatio * yRange;
    return QPointF(x, y);
}

void GraphWidget::drawAxes(QPainter &painter, const QRect &plotRect)
{
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(plotRect);

    const int xTickCount = 5;
    const int yTickCount = 5;

    const double xRange = m_viewXMax - m_viewXMin;
    const double yRange = m_viewYMax - m_viewYMin;
    if (xRange <= 0.0 || yRange <= 0.0) {
        return;
    }

    QFontMetrics fm(painter.font());

    for (int i = 0; i <= xTickCount; ++i) {
        const double value = m_viewXMin + (m_viewXMax - m_viewXMin) * i / xTickCount;
        const double ratio = (value - m_viewXMin) / (m_viewXMax - m_viewXMin);
        const int x = plotRect.left() + qRound(ratio * plotRect.width());
        painter.drawLine(x, plotRect.bottom(), x, plotRect.bottom() + 5);
        const QString label = QString::number(value, 'f', 2);
        const int textWidth = fm.horizontalAdvance(label);
        painter.drawText(x - textWidth / 2, plotRect.bottom() + fm.height() + 5, label);
    }

    for (int i = 0; i <= yTickCount; ++i) {
        const double value = m_viewYMin + (m_viewYMax - m_viewYMin) * i / yTickCount;
        const double ratio = (value - m_viewYMin) / (m_viewYMax - m_viewYMin);
        const int y = plotRect.bottom() - qRound(ratio * plotRect.height());
        painter.drawLine(plotRect.left() - 5, y, plotRect.left(), y);
        const QString label = QString::number(value, 'f', 2);
        painter.drawText(plotRect.left() - 10 - fm.horizontalAdvance(label), y + fm.ascent() / 2, label);
    }

    painter.drawText(plotRect.bottomRight() + QPoint(-40, 40), QStringLiteral("r"));
    painter.save();
    painter.translate(plotRect.topLeft() + QPoint(-45, 0));
    painter.rotate(-90);
    painter.drawText(QPoint(0, 0), QStringLiteral("Brightness"));
    painter.restore();
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    const QRect plotRect = computePlotRect();
    drawAxes(painter, plotRect);

    if (m_samples.isEmpty()) {
        return;
    }

    QPainterPath path;
    path.moveTo(dataToScreen(m_samples.first(), plotRect));
    for (int i = 1; i < m_samples.size(); ++i) {
        path.lineTo(dataToScreen(m_samples.at(i), plotRect));
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawPath(path);
}

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    const QRect plotRect = computePlotRect();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPointF posF = event->position();
#else
    const QPointF posF = event->posF();
#endif

    if (!plotRect.contains(posF.toPoint())) {
        QWidget::wheelEvent(event);
        return;
    }

    const double delta = event->angleDelta().y();
    if (qFuzzyIsNull(delta)) {
        return;
    }

    const double factor = std::pow(1.0015, delta);
    const QPointF dataPoint = screenToData(posF, plotRect);

    const double newXSpan = (m_viewXMax - m_viewXMin) / factor;
    const double newYSpan = (m_viewYMax - m_viewYMin) / factor;

    const double xCenter = qBound(m_dataXMin, dataPoint.x(), m_dataXMax);
    const double yCenter = qBound(m_dataYMin, dataPoint.y(), m_dataYMax);

    m_viewXMin = xCenter - (xCenter - m_viewXMin) / factor;
    m_viewXMax = m_viewXMin + newXSpan;
    m_viewYMin = yCenter - (yCenter - m_viewYMin) / factor;
    m_viewYMax = m_viewYMin + newYSpan;

    clampView();
    update();
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    const QRect plotRect = computePlotRect();
    if (event->button() == Qt::LeftButton && plotRect.contains(event->pos())) {
        m_isPanning = true;
        m_lastMousePos = event->pos();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning) {
        const QRect plotRect = computePlotRect();
        const QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        if (!plotRect.isEmpty()) {
            const double dx = -static_cast<double>(delta.x()) / plotRect.width() * (m_viewXMax - m_viewXMin);
            const double dy = static_cast<double>(delta.y()) / plotRect.height() * (m_viewYMax - m_viewYMin);
            translateView(dx, dy);
        }
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isPanning) {
        m_isPanning = false;
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void GraphWidget::translateView(double dx, double dy)
{
    m_viewXMin += dx;
    m_viewXMax += dx;
    m_viewYMin += dy;
    m_viewYMax += dy;

    clampView();
    update();
}
