#pragma once

#include "BaseProcessor.h"

class HistogramProcessor : public BaseProcessor
{
public:
    HistogramProcessor(QObject* parent = nullptr);

protected:
    void processImage(const GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(const MonoImage* srcImage, MonoImage* dstImage) override;

public:
    void setWindowArray(bool* array, int arrayNum);

private:
    bool* _array;
    int _arrayNum;
};
