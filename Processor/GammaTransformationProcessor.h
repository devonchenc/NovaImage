#pragma once

#include "../Widget/ProcessorBaseWidget.h"
#include "BaseProcessor.h"

QT_BEGIN_NAMESPACE
class QSlider;
class QLabel;
QT_END_NAMESPACE

class GammaTransformationWidget : public ProcessorBaseWidget
{
    Q_OBJECT

public:
    GammaTransformationWidget(BaseProcessor* processor, QWidget* parent = nullptr);

    void reset() override;

private slots:
    void gammaValueChanged(int value);

signals:
    void parametersChanged(float gamma);

private:
    QLabel* _gammaLabel;
    QSlider* _gammaSlider;
    QLabel* _gammaValueLabel;
};

class GammaTransformationProcessor : public BaseProcessor
{
public:
    GammaTransformationProcessor(QObject* parent = nullptr);
    virtual ~GammaTransformationProcessor();

    void initUI() override;

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;

public:
    void setGamma(float gamma);

private:
    float _gamma;
};
