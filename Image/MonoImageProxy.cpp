#include "MonoImageProxy.h"

#include "../Core/GlobalFunc.h"

MonoImageProxy::MonoImageProxy(MonoImage* image, int width, int height, Type type)
    : _image(image)
    , _width(width)
    , _height(height)
    , _type(type)
    , _byteImage(nullptr)
    , _pImage(nullptr)
{
    _byteImage = new uchar[_width * _height * 3];
    _pImage = std::make_shared<QImage>(_width, _height, QImage::Format_RGB888);
}

MonoImageProxy::~MonoImageProxy()
{
    if (_byteImage)
    {
        delete[] _byteImage;
        _byteImage = nullptr;
    }
}

void MonoImageProxy::copyByteToImage()
{
    ::copyByteToImage(_byteImage, _width, _height, _pImage.get());
}