#pragma once

#include "BaseProcessor.h"

class LevelsProcessor : public BaseProcessor
{
public:
    LevelsProcessor(QObject* parent = nullptr);

protected:
    void processImage(const GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(const MonoImage* srcImage, MonoImage* dstImage) override;

public:
    void setPara(float bottom, float mid, float top);

private:
    // Pointer to levels widget
    float _bottom, _mid, _top;
};
