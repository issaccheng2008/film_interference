#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QPointF>
#include <QVector>
#include <QWidget>

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr);

    void setParameters(double e, double n1, double n2, double n3, double lambda, double f);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void regenerateData();

    double m_e = 0.0;
    double m_n1 = 0.0;
    double m_n2 = 0.0;
    double m_n3 = 0.0;
    double m_lambda = 0.0;
    double m_f = 0.0;

    QVector<QPointF> m_samples;
};

#endif // GRAPHWIDGET_H
