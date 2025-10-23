#include "graphwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace
{
constexpr double kDr = 0.01;
}

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(320, 320);
    regenerateData();
}

void GraphWidget::setParameters(double e, double n1, double n2, double n3, double lambda)
{
    if (qFuzzyCompare(m_e, e) && qFuzzyCompare(m_n1, n1) && qFuzzyCompare(m_n2, n2) &&
        qFuzzyCompare(m_n3, n3) && qFuzzyCompare(m_lambda, lambda)) {
        return;
    }

    m_e = e;
    m_n1 = n1;
    m_n2 = n2;
    m_n3 = n3;
    m_lambda = lambda;
    regenerateData();
    update();
}

void GraphWidget::regenerateData()
{
    m_samples.clear();
    for (double r = 0.0; r <= 1.0 + 1e-9; r += kDr) {
        const double phase = m_e + m_n1 + m_n2 + m_n3 + m_lambda + r;
        const double brightness = qCos(phase);
        m_samples.append(QPointF(r, brightness * brightness));
    }
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    const int leftMargin = 50;
    const int rightMargin = 20;
    const int topMargin = 20;
    const int bottomMargin = 40;

    const QRect plotRect = rect().adjusted(leftMargin, topMargin, -rightMargin, -bottomMargin);

    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(plotRect);

    if (m_samples.isEmpty()) {
        return;
    }

    const double xMin = m_samples.first().x();
    const double xMax = m_samples.last().x();
    const double yMin = 0.0;
    const double yMax = 1.0;

    auto toPoint = [&](const QPointF &sample) {
        const double xRatio = (sample.x() - xMin) / (xMax - xMin);
        const double yRatio = (sample.y() - yMin) / (yMax - yMin);
        const double x = plotRect.left() + xRatio * plotRect.width();
        const double y = plotRect.bottom() - yRatio * plotRect.height();
        return QPointF(x, y);
    };

    QPainterPath path;
    path.moveTo(toPoint(m_samples.first()));
    for (int i = 1; i < m_samples.size(); ++i) {
        path.lineTo(toPoint(m_samples.at(i)));
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawPath(path);

    painter.setPen(Qt::black);
    painter.drawText(plotRect.bottomRight() + QPoint(-40, 30), QStringLiteral("r"));
    painter.drawText(plotRect.topLeft() + QPoint(-30, 10), QStringLiteral("Brightness"));
}
