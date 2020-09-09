#pragma once

#include <QWidget>

#include "BaseProcessor.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class EqualizationWidget : public QWidget
{
    Q_OBJECT

public:
    EqualizationWidget(QWidget* parent = nullptr);

public slots:
    void editChanged();

signals:
    void valueChanged(int gridSize, int clipLimit);

private:
    QLineEdit* _gridSizeEdit;
    QLineEdit* _clipLimitEdit;
};

class EqualizationProcessor : public BaseProcessor
{
    Q_OBJECT

public:
    EqualizationProcessor();
    virtual ~EqualizationProcessor();

    void initUI() override;

public slots:
    void valueChanged(int gridSize, int clipLimit);

protected:
    void processGeneralImage(GeneralImage* image) override;

    void processMonoImage(MonoImage* image) override;

    // Process float array
    void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

private:
    void RGB2HSV(uchar R, uchar G, uchar B, int& H, float& S, uchar& V);
    void HSV2RGB(int H, float S, uchar V, uchar& R, uchar& G, uchar& B);

private:
    EqualizationWidget* _widget;
    int _gridSize;
    int _clipLimit;
};
