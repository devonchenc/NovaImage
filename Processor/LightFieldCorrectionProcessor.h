#pragma once

#include "../Widget/ProcessorBaseWidget.h"
#include "BaseProcessor.h"

QT_BEGIN_NAMESPACE
class QButtonGroup;
class QCheckBox;
class QSlider;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

class LightFieldCorrectionWidget : public ProcessorBaseWidget
{
    Q_OBJECT

public:
    LightFieldCorrectionWidget(BaseProcessor* processor, QWidget* parent = nullptr);

public slots:
    void laplacianCheckBoxClicked();
    void sobelCheckBoxClicked();

    void laplacianValueChanged(int value);
    void sobelValueChanged(int value);

private slots:
    void browseButtonClicked();

signals:
    void sobelChanged(float value);
    void laplacianChanged(float value);

private:
    QButtonGroup* _radioGroup;
    QLineEdit* _pathEdit;

    QCheckBox* _sobelCheckBox;
    QSlider* _sobelSlider;
    QLabel* _sobelValueLabel;
    QCheckBox* _laplacianCheckBox;
    QSlider* _laplacianSlider;
    QLabel* _laplacianValueLabel;

    float _sobelFactor;
    float _laplacianFactor;
};

class LightFieldCorrectionProcessor : public BaseProcessor
{
    Q_OBJECT

public:
    LightFieldCorrectionProcessor(QObject* parent = nullptr);
    virtual ~LightFieldCorrectionProcessor();

    void initUI() override;

public slots:
    void sobelChanged(float value);

    void laplacianChanged(float value);

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;

private:
    void getGaussianArray(float* kernel, int width, int height, float sigma);

private:
    float _sobelFactor;
    float _laplacianFactor;
};
