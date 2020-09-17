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

    void reset() override;

public slots:
    void brightnessValueChanged(int value);

    void contrastValueChanged(int value);

signals:
    void parametersChanged(int brightness, int contrast);

private:
    QLabel* _brightnessLabel;
    QLabel* _contrastLabel;
    QSlider* _brightnessSlider;
    QSlider* _contrastSlider;
    QLabel* _brightnessValueLabel;
    QLabel* _contrastValueLabel;
};

class BrightnessAndContrastProcessor : public BaseProcessor
{
public:
    BrightnessAndContrastProcessor(QObject* parent = nullptr);
    virtual ~BrightnessAndContrastProcessor();

    void initUI() override;

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;

public:
    void setBrightnessAndContrast(int brightness, int contrast);

private:
    int _brightness;
    int _contrast;
};
