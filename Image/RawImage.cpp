#include "RawImage.h"

#include "ImageDataTemplate.h"

RawImage::RawImage(const QString& pathName, int type, int width, int height)
	: MonoImage(pathName)
{
	// Read data
	if (readData() == false)
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
	// Convert float data to uchar data
	if (_imageData->convertToByte(_byteImage) == false)
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

// Read data
bool RawImage::readData()
{


	return true;
}
