#include "ScanImage.h"

#include <QFile>
#include "ImageDataTemplate.h"

ScanImage::ScanImage(const QString& pathName)
	: MonoImage(pathName)
{
	// Read data header
	if (readDataHeader() == false)
	{
		_openSucceed = false;
		return;
	}
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

ScanImage::~ScanImage()
{

}

bool ScanImage::saveAsDcm(const QString& fileName)
{
	return true;
}

void ScanImage::initWindowWidthAndLevel()
{
	if (_dataHeader.WindowWidth == 0 && _dataHeader.WindowLevel == 0)
	{
		_windowWidth = _imageData->getMaximumValue() - _imageData->getMinimumValue();
		_windowLevel = (_imageData->getMaximumValue() + _imageData->getMinimumValue()) / 2.0f;
	}
	else
	{
		_windowWidth = _dataHeader.WindowWidth;
		_windowLevel = _dataHeader.WindowLevel;
	}
}

// Read data header
bool ScanImage::readDataHeader()
{
	QFile file(_pathName);
	if (!file.open(QFile::ReadOnly))
		return false;

	qint64 size = file.size();

	file.read((char*)(&_dataHeader), sizeof(DataHeader));
	file.close();

	_width = _dataHeader.Width;
	_height = _dataHeader.Height;
	_slice = _dataHeader.Slice == 0 ? 1 : _dataHeader.Slice;
	if (_width * _height == 0)
		return false;

	qint64 expectSize = _width * _height * _slice * sizeof(float) + DATA_HEADER_SIZE;
	if (expectSize > size)
	{
		QMessageBox::critical(nullptr, QObject::tr("Open image file error"),
			QObject::tr("The data size does not match the file information description!"), QMessageBox::Ok);
		return false;
	}

	return true;
}

// Read data
bool ScanImage::readData()
{
	_imageData = new ImageDataTemplate<float>(_width * _height);

	float* originalData = static_cast<float*>(_imageData->getOriginalData());

	QFile file(_pathName);
	if (!file.open(QFile::ReadOnly))
		return false;

	file.seek(DATA_HEADER_SIZE);
	qint64 readSize = file.read((char*)originalData, sizeof(float) * _width * _height);
	file.close();

	if (readSize != sizeof(float) * _width * _height)
	{
		QMessageBox::critical(nullptr, QObject::tr("Open image file error"),
			QObject::tr("The data size does not match the file information description!"), QMessageBox::Ok);
		return false;
	}

	return true;
}