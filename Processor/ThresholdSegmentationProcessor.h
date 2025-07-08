#pragma once

#include "../Widget/ProcessorBaseWidget.h"
#include "BaseProcessor.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QSlider;
class QLabel;
QT_END_NAMESPACE

class ThresholdSegmentationWidget : public ProcessorBaseWidget
{
    Q_OBJECT

public:
    ThresholdSegmentationWidget(BaseProcessor* processor, QWidget* parent = nullptr);

    void setOTSUThreshold(int threshold);

private slots:
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

    void initUI() override;

public slots:
    void thresholdChanged(int value);

protected:
    void processImage(const GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(const MonoImage* srcImage, MonoImage* dstImage) override;

private:
    int findOtsuThreshold(const uint* grayPixelArray, int pixelNumber) const;

private:
    int _threshold;
};
