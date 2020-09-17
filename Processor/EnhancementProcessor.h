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

class EnhancementProcessor : public BaseProcessor
{
    Q_OBJECT

public:
    EnhancementProcessor(QObject* parent = nullptr);
    virtual ~EnhancementProcessor();

    void initUI() override;

public slots:
    void thresholdChanged(int value);

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;

private:


private:
    int _threshold;
};
