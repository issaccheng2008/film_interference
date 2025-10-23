#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>

class QSlider;
class QLabel;

class GraphWidget;
class RingWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUiElements();
    void updateVisuals();

    Ui::MainWindow *ui;
    RingWidget *m_ringWidget = nullptr;
    GraphWidget *m_graphWidget = nullptr;

    struct SliderInfo {
        QSlider *slider = nullptr;
        QLabel *valueLabel = nullptr;
    };

    QMap<QString, SliderInfo> m_sliders;
    QMap<QString, double> m_values;
};

#endif // MAINWINDOW_H
