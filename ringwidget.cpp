#include "ringwidget.h"

#include <QImage>
#include <QPainter>
#include <QtGlobal>
#include <QtMath>
#include <cmath>

namespace
{
constexpr double kDr = 0.01;
}

RingWidget::RingWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(320, 320);
}

void RingWidget::setParameters(double e, double n1, double n2, double n3, double lambda)
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
    update();
}

double RingWidget::brightnessForRadius(double r) const
{
    const double phase = m_e + m_n1 + m_n2 + m_n3 + m_lambda + r;
    const double brightness = qCos(phase);
    return brightness * brightness;
}

void RingWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    const int w = width();
    const int h = height();
    const QPointF center(w / 2.0, h / 2.0);
    const double maxRadius = qMin(w, h) / 2.0;

    QImage image(w, h, QImage::Format_RGB32);
    image.fill(Qt::black);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const double dx = x + 0.5 - center.x();
            const double dy = y + 0.5 - center.y();
            const double distance = std::sqrt(dx * dx + dy * dy);
            if (distance > maxRadius) {
                continue;
            }

            const double normalized = distance / maxRadius;
            const int ringIndex = static_cast<int>(normalized / kDr);
            const double rValue = ringIndex * kDr;
            const double brightness = qBound(0.0, brightnessForRadius(rValue), 1.0);
            const int intensity = static_cast<int>(brightness * 255.0 + 0.5);
            image.setPixel(x, y, qRgb(intensity, intensity, intensity));
        }
    }

    QPainter painter(this);
    painter.drawImage(rect(), image);
}
