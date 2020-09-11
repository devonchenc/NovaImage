#pragma once

#include "MonoImage.h"

class RawImage : public MonoImage
{
public:
    RawImage(const QString& pathName, int type, int width, int height, int slice, int headerSize);
    RawImage(const RawImage& src);
    virtual ~RawImage();

public:
    void setSlice(int slice) override;

    void initWindowWidthAndLevel() override;

    BaseImage* copyImage() const override;

    bool copyToImage(BaseImage* image) const override;

private:
    // Read data
    bool readData();

private:
    int _dataType;
    int _headerSize;
};
