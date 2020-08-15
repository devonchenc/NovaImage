﻿#pragma once

#include "MonoImage.h"

class MonoImageProxy
{
public:
    MonoImageProxy(MonoImage* image, int width, int height, int type = TOP_VIEW);
    MonoImageProxy(const MonoImageProxy& src);
    ~MonoImageProxy();

public:
    uchar* getBYTEImage() const { return _byteImage; }

    std::shared_ptr<QImage> getImageEntity() const { return _pImage; }

    void copyByteToImage();

protected:
    MonoImage* _image;

    int _width, _height;

    int _type;

    uchar* _byteImage;

    std::shared_ptr<QImage> _pImage;
};
