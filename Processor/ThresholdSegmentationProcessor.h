#pragma once

#include <QSlider>
#include <QLabel>

#include "BaseProcessor.h"

class ThresholdSegmentationWidget : public QWidget
{
    Q_OBJECT

public:
    ThresholdSegmentationWidget(QWidget* parent = nullptr);

    void setThreshold(int threshold);

public slots:
    void valueChanged(int value);

signals:
    void thresholdChanged(int value);

private:
    QLabel* _thresholdLabel;
    QSlider* _thresholdSlider;
    QLabel* _thresholdValueLabel;
};

class ThresholdSegmentationProcessor : public BaseProcessor
{
    Q_OBJECT

public:
    ThresholdSegmentationProcessor();
    virtual ~ThresholdSegmentationProcessor();

    void initUI() override;

public slots:
    void thresholdChanged(int value);

protected:
    void processGeneralImage(GeneralImage* image) override;

    void processMonoImage(MonoImage* image) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

private:
    int findOtsuThreshold(uint* grayPixelArray, int pixelNumber);

private:
    ThresholdSegmentationWidget* _widget;
    int _threshold;
};
