﻿#pragma once

#include "MonoImage.h"

class MonoImageProxy
{
public:
    MonoImageProxy(MonoImage* image, int width, int height, int type = AXIAL_VIEW);
    MonoImageProxy(const MonoImageProxy& src);
    ~MonoImageProxy();

    MonoImageProxy& operator=(const MonoImageProxy& src);

public:
    uchar* getBYTEImage() const { return _byteImage; }

    std::shared_ptr<QImage> getImageEntity() const { return _pImage; }

    void copyByteToImage();
    void copyByteToImage(QImage* dstImage) const;

protected:
    MonoImage* _image;

    int _width, _height;

    int _type;

    uchar* _byteImage;

    std::shared_ptr<QImage> _pImage;
};
