#pragma once

#include "BaseProcessor.h"

class LookupTableProcessor : public BaseProcessor
{
public:
    LookupTableProcessor(const QString& tableName, QObject* parent = nullptr);

protected:
    void processImage(const GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(const MonoImage* srcImage, MonoImage* dstImage) override;

private:
    bool loadLUT(const QString& tableName);

private:
    // 256 bins * 3 channels = 768
    const int _size = 768;
    std::unique_ptr<unsigned char[]> _table;
};
