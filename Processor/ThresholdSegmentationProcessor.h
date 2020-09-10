#pragma once

#include <QWidget>

#include "BaseProcessor.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QSlider;
class QLabel;
QT_END_NAMESPACE

class ThresholdSegmentationWidget : public QWidget
{
    Q_OBJECT

public:
    ThresholdSegmentationWidget(QWidget* parent = nullptr);

    void setOTSUThreshold(int threshold);

public slots:
    void valueChanged(int value);

    void autoCheckBoxClicked();

signals:
    void thresholdChanged(int value);

private:
    QCheckBox* _autoCheckBox;
    QSlider* _thresholdSlider;
    QLabel* _thresholdValueLabel;
    int _OSTUThreshold;
};

class ThresholdSegmentationProcessor : public BaseProcessor
{
    Q_OBJECT

public:
    ThresholdSegmentationProcessor(QObject* parent = nullptr);
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
