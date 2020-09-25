#pragma once

#include <fftw3.h>

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

private slots:
    void autoButtonToggled();

    void fileButtonToggled();

    void browseButtonClicked();

signals:
    void typeChanged(int type, QString fileName);

private:
    QButtonGroup* _radioGroup;
    QLineEdit* _pathEdit;

    QString _fileName;
};

class LightFieldCorrectionProcessor : public BaseProcessor
{
    Q_OBJECT

public:
    LightFieldCorrectionProcessor(QObject* parent = nullptr);
    virtual ~LightFieldCorrectionProcessor();

    void initUI() override;

public slots:
    void typeChanged(int type, QString fileName);

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;

private:
    void generateKernel(fftw_complex* kernelOut, int kernelWidth, int kernelHeight, float sigma);

    void getGaussianArray(float* kernel, int width, int height, float sigma);

private:
    int _type;
    QString _fileName;
};
