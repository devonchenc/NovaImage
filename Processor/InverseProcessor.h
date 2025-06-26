#pragma once

#include "BaseProcessor.h"

class InverseProcessor : public BaseProcessor
{
public:
    InverseProcessor(QObject* parent = nullptr);

protected:
    void processImage(const GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(const MonoImage* srcImage, MonoImage* dstImage) override;
};
