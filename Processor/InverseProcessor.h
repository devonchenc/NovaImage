#pragma once

#include "BaseProcessor.h"

class InverseProcessor : public BaseProcessor
{
public:
    InverseProcessor(QObject* parent = nullptr);
    virtual ~InverseProcessor();

protected:
    void processImage(GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(MonoImage* srcImage, MonoImage* dstImage) override;
};
