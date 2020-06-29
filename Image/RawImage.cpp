#include "RawImage.h"

#include <QFile>
#include "ImageDataTemplate.h"

RawImage::RawImage(const QString& pathName, int type, int width, int height, int headerSize)
	: MonoImage(pathName)
	, _dataType(type)
	, _headerSize(headerSize)
{
	_width= width;
	_height = height;

	// Read data
	if (readData() == false)
	{
		_openSucceed = false;
		return;
	}
	// Allocate memory
	if (allocateMemory() == false)
	{
		_openSucceed = false;
		return;
	}
	// Find top and bottom value in data
	if (_imageData->findTopAndBottom() == false)
	{
		_openSucceed = false;
		return;
	}

	initWindowWidthAndLevel();

	// Convert float data to uchar data
	if (convertToByte() == false)
	{
		_openSucceed = false;
		return;
	}
	// Copy to image
	if (copyToImage() == false)
	{
		_openSucceed = false;
		return;
	}

	_openSucceed = true;
}

RawImage::~RawImage()
{

}

void RawImage::initWindowWidthAndLevel()
{
	_windowWidth = _imageData->getMinimumValue();
	_windowLevel = _imageData->getMaximumValue();
}

// Read data
bool RawImage::readData()
{
	if (_width <= 0 || _height <= 0)
		return false;

	QFile file(_pathName);
	if (!file.open(QFile::ReadOnly))
		return false;

	qint64 size = file.size();
	file.seek(_headerSize);

	int pixelCount = _width * _height;
	qint64 readCount;
	switch (_dataType)
	{
	case 0:
		_imageData = new ImageDataTemplate<uchar>(pixelCount);
		readCount = file.read((char*)_imageData->getOriginalData(), sizeof(uchar) * pixelCount) / sizeof(uchar);
		break;
	case 1:
		_imageData = new ImageDataTemplate<ushort>(pixelCount);
		readCount = file.read((char*)_imageData->getOriginalData(), sizeof(ushort) * pixelCount) / sizeof(ushort);
		break;
	case 2:
		_imageData = new ImageDataTemplate<uint>(pixelCount);
		readCount = file.read((char*)_imageData->getOriginalData(), sizeof(uint) * pixelCount) / sizeof(uint);
		break;
	case 3:
		_imageData = new ImageDataTemplate<float>(pixelCount);
		readCount = file.read((char*)_imageData->getOriginalData(), sizeof(float) * pixelCount) / sizeof(float);
		break;
	case 4:
		_imageData = new ImageDataTemplate<double>(pixelCount);
		readCount = file.read((char*)_imageData->getOriginalData(), sizeof(double) * pixelCount) / sizeof(double);
		break;
	}
	if (readCount != pixelCount)
	{
		QMessageBox::critical(nullptr, QObject::tr("Open image file error"),
			QObject::tr("The data size does not match the file information description!"), QMessageBox::Ok);
		return false;
	}

	return true;
}
