#pragma once

#include <QWidget>

#include "BaseProcessor.h"

QT_BEGIN_NAMESPACE
class QSlider;
class QLabel;
QT_END_NAMESPACE

class EqualizationWidget : public QWidget
{
    Q_OBJECT

public:
    EqualizationWidget(QWidget* parent = nullptr);

    void setThreshold(int threshold);

public slots:
    void valueChanged(int value);

signals:
    void thresholdChanged(int value);

private:
    QSlider* _thresholdSlider;
    QLabel* _thresholdValueLabel;
};

class EqualizationProcessor : public BaseProcessor
{
    Q_OBJECT

public:
    EqualizationProcessor();
    virtual ~EqualizationProcessor();

    void initUI() override;

public slots:
    void thresholdChanged(int value);

protected:
    void processGeneralImage(GeneralImage* image) override;

    void processMonoImage(MonoImage* image) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

private:
    void RGB2HSV(uchar R, uchar G, uchar B, int& H, float& S, uchar& V);
    void HSV2RGB(int H, float S, uchar V, uchar& R, uchar& G, uchar& B);

private:
    EqualizationWidget* _widget;
    int _threshold;
};
