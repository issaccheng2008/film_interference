#ifndef RINGWIDGET_H
#define RINGWIDGET_H

#include <QWidget>

class RingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RingWidget(QWidget *parent = nullptr);

    void setParameters(double e, double n1, double n2, double n3, double lambda);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    double m_e = 0.0;
    double m_n1 = 0.0;
    double m_n2 = 0.0;
    double m_n3 = 0.0;
    double m_lambda = 0.0;

    double brightnessForRadius(double r) const;
};

#endif // RINGWIDGET_H
