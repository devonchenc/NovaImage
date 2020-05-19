#include "BaseImage.h"

BaseImage::BaseImage() :
	_pImage(nullptr),
	_width(0),
	_height(0),
	_openSucceed(false)
{
	memset(_grayPixelNumber, 0, sizeof(uint) * 256);
}

BaseImage::BaseImage(const QString& pathName) :
	_pImage(nullptr),
	_width(0),
	_height(0),
	_pathName(pathName),
	_openSucceed(false)
{

}

BaseImage::~BaseImage()
{
	if (_pImage)
	{
		delete _pImage;
		_pImage = nullptr;
	}
}

QRgb BaseImage::getPixel(const QPoint& position) const
{
	return _pImage->pixel(position);
}

void BaseImage::copyToArray(uchar* array)
{
    uchar* pData = _pImage->bits();
    int pitch = _pImage->bytesPerLine();
    int depth = _pImage->depth() / 8;
    for (int j = 0; j < _height; j++)
    {
        for (int i = 0; i < _width; i++)
        {
            uchar* pixel = pData + j * pitch + i * depth;
            array[j * _width + i] = *pixel;
        }
    }
}

bool BaseImage::copyFromArray(uchar* array, int width, int height)
{
    if (array == nullptr || _pImage == nullptr)
        return false;

    if (width != this->width() || height != this->height())
        return false;

    uchar* pData = _pImage->bits();
    int pitch = _pImage->bytesPerLine();
    int depth = _pImage->depth() / 8;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            uchar* pixel = pData + j * pitch + i * depth;
            *pixel = array[j * width + i];
            for (int n = 1; n < depth - 1; n++)
            {
                *(pixel + n) = array[j * width + i];
            }
        }
    }

    return true;
}

