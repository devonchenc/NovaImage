#pragma once

#include "../Widget/ProcessorBaseWidget.h"
#include "BaseProcessor.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QSlider;
class QLabel;
QT_END_NAMESPACE

class EnhancementWidget : public ProcessorBaseWidget
{
    Q_OBJECT

public:
    EnhancementWidget(BaseProcessor* processor, QWidget* parent = nullptr);

private slots:
    void laplacianCheckBoxClicked();
    void sobelCheckBoxClicked();

    void laplacianValueChanged(int value);
    void sobelValueChanged(int value);

signals:
    void sobelChanged(float value);
    void laplacianChanged(float value);

private:
    QCheckBox* _sobelCheckBox;
    QSlider* _sobelSlider;
    QLabel* _sobelValueLabel;
    QCheckBox* _laplacianCheckBox;
    QSlider* _laplacianSlider;
    QLabel* _laplacianValueLabel;

    float _sobelFactor;
    float _laplacianFactor;
};

class EnhancementProcessor : public BaseProcessor
{
    Q_OBJECT

public:
    EnhancementProcessor(QObject* parent = nullptr);
    virtual ~EnhancementProcessor();

    void initUI() override;

public slots:
    void sobelChanged(float value);

    void laplacianChanged(float value);

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;

private:
    float _sobelFactor;
    float _laplacianFactor;
};
