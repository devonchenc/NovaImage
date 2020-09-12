#pragma once

#include "../Widget/ProcessorBaseWidget.h"
#include "BaseProcessor.h"

QT_BEGIN_NAMESPACE
class QSlider;
class QLabel;
QT_END_NAMESPACE

class BrightnessAndContrastWidget : public ProcessorBaseWidget
{
    Q_OBJECT

public:
    BrightnessAndContrastWidget(BaseProcessor* processor, QWidget* parent = nullptr);

public slots:
    void brightnessValueChanged(int value);

    void contrastValueChanged(int value);

signals:
    void parametersChanged(int brightness, int contrast);

private:
    QSlider* _brightnessSlider;
    QSlider* _contrastSlider;
    QLabel* _brightnessValueLabel;
    QLabel* _contrastValueLabel;

    QLabel* _brightnessLabel;
    QLabel* _contrastLabel;
};

class BrightnessAndContrastProcessor : public BaseProcessor
{
public:
    BrightnessAndContrastProcessor(QObject* parent = nullptr);
    virtual ~BrightnessAndContrastProcessor();

    void initUI() override;

protected:
    void processImageImpl(GeneralImage* image, QImage* dstImage) override;

    void processImageImpl(MonoImage* image, QImage* dstImage) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

public:
    void setBrightnessAndContrast(int brightness, int contrast);

private:
    int _brightness;
    int _contrast;
};
