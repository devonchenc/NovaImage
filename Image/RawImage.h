﻿#pragma once

#include "MonoImage.h"

class RawImage : public MonoImage
{
public:
    RawImage(const QString& pathName, int type, int width, int height, int slice, int headerSize, int endian);
    RawImage(const RawImage& src);

    RawImage& operator=(const RawImage& src);

public:
    void setSlice(int slice) override;

    void initWindowWidthAndLevel() override;

    BaseImage* copyImage() const override;

    bool copyToImage(BaseImage* image) const override;

private:
    // Read data
    bool readData(int endian);

private:
    int _dataType;
    int _headerSize;
};
