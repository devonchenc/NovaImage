#include "MonoImageProxy.h"

#include "../Core/GlobalFunc.h"

MonoImageProxy::MonoImageProxy(MonoImage* image, int width, int height, int type)
    : _image(image)
    , _width(width)
    , _height(height)
    , _type(type)
    , _byteImage(new uchar[_width * _height * 3])
    , _pImage(nullptr)
{
    _pImage = std::make_shared<QImage>(_width, _height, QImage::Format_RGB888);
}

MonoImageProxy::MonoImageProxy(const MonoImageProxy& src)
    : _image(src._image)
    , _width(src._width)
    , _height(src._height)
    , _type(src._type)
    , _byteImage(new uchar[_width * _height * 3])
{
    _pImage.reset(new QImage(*src._pImage));
    memcpy(_byteImage, src._byteImage, sizeof(uchar) * _width * _height * 3);
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