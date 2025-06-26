#pragma once

#include "BaseProcessor.h"

class LookupTableProcessor : public BaseProcessor
{
public:
    LookupTableProcessor(const QString& tableName, QObject* parent = nullptr);
    ~LookupTableProcessor();

protected:
    void processImage(const GeneralImage* srcImage, GeneralImage* dstImage) override;

    void processImage(const MonoImage* srcImage, MonoImage* dstImage) override;

private:
    bool loadLUT(const QString& tableName);

private:
    const int _size = 768;
    unsigned char* _table;
};
