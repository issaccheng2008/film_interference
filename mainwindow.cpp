#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "graphwidget.h"
#include "ringwidget.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <QVector>

namespace
{
constexpr int kSliderScale = 100;
constexpr int kSliderMin = -628; // -6.28
constexpr int kSliderMax = 628;  // 6.28
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUiElements();
    updateVisuals();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiElements()
{
    if (!ui->centralwidget) {
        return;
    }

    auto *mainLayout = new QVBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    auto *displayLayout = new QHBoxLayout;
    displayLayout->setSpacing(12);

    m_ringWidget = new RingWidget(this);
    m_graphWidget = new GraphWidget(this);

    displayLayout->addWidget(m_ringWidget, 2);
    displayLayout->addWidget(m_graphWidget, 3);

    mainLayout->addLayout(displayLayout, 1);

    auto *sliderLayout = new QGridLayout;
    sliderLayout->setHorizontalSpacing(12);
    sliderLayout->setVerticalSpacing(8);

    struct SliderDefinition {
        QString key;
        QString label;
    };

    const QVector<SliderDefinition> sliderDefs = {
        {QStringLiteral("e"), QStringLiteral("e")},
        {QStringLiteral("n1"), QStringLiteral("n1")},
        {QStringLiteral("n2"), QStringLiteral("n2")},
        {QStringLiteral("n3"), QStringLiteral("n3")},
        {QStringLiteral("lambda"), QString::fromUtf8("λ")},
        {QStringLiteral("f"), QStringLiteral("f")}
    };

    int row = 0;
    for (const auto &def : sliderDefs) {
        auto *nameLabel = new QLabel(def.label, this);
        auto *slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(kSliderMin, kSliderMax);
        slider->setValue(0);
        slider->setSingleStep(1);
        slider->setPageStep(10);

        auto *valueLabel = new QLabel(QStringLiteral("0.00"), this);
        valueLabel->setMinimumWidth(60);

        sliderLayout->addWidget(nameLabel, row, 0);
        sliderLayout->addWidget(slider, row, 1);
        sliderLayout->addWidget(valueLabel, row, 2);

        SliderInfo info;
        info.slider = slider;
        info.valueLabel = valueLabel;
        m_sliders.insert(def.key, info);
        m_values.insert(def.key, 0.0);

        connect(slider, &QSlider::valueChanged, this, [this, key = def.key](int value) {
            const double numeric = static_cast<double>(value) / kSliderScale;
            if (!m_values.contains(key)) {
                return;
            }
            m_values[key] = numeric;
            auto info = m_sliders.value(key);
            if (info.valueLabel) {
                info.valueLabel->setText(QString::number(numeric, 'f', 2));
            }
            updateVisuals();
        });

        ++row;
    }

    sliderLayout->setColumnStretch(1, 1);

    mainLayout->addLayout(sliderLayout);
}

void MainWindow::updateVisuals()
{
    const double eValue = m_values.value(QStringLiteral("e"), 0.0);
    const double n1Value = m_values.value(QStringLiteral("n1"), 0.0);
    const double n2Value = m_values.value(QStringLiteral("n2"), 0.0);
    const double n3Value = m_values.value(QStringLiteral("n3"), 0.0);
    const double lambdaValue = m_values.value(QStringLiteral("lambda"), 0.0);
    const double fValue = m_values.value(QStringLiteral("f"), 0.0);

    if (m_ringWidget) {
        m_ringWidget->setParameters(eValue, n1Value, n2Value, n3Value, lambdaValue, fValue);
    }
    if (m_graphWidget) {
        m_graphWidget->setParameters(eValue, n1Value, n2Value, n3Value, lambdaValue, fValue);
    }
}
