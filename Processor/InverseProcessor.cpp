#include "InverseProcessor.h"

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

InverseProcessor::InverseProcessor()
{

}

InverseProcessor::~InverseProcessor()
{

}

void InverseProcessor::processGeneralImage(GeneralImage* image)
{
	assert(image);

	int width = image->width();
	int height = image->height();
	QImage* imageEntity = image->getImageEntity();
	QImage* backupImageEntity = image->getBackupImage();
	uchar* pImageData = (uchar*)imageEntity->bits();
	uchar* pBackupImageData = (uchar*)backupImageEntity->bits();
	int pitch = imageEntity->bytesPerLine();
	int depth = imageEntity->depth() / 8;

	if (pImageData[0] == pBackupImageData[0])
	{
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				uchar* pPixel = pImageData + j * pitch + i * depth;
				uchar* pBackupPixel = pBackupImageData + j * pitch + i * depth;
				for (int n = 0; n < qMin(depth, 3); n++)
				{
					*(pPixel + n) = 255 - *(pBackupPixel + n);
				}
			}
		}
	}
	else
	{
		image->restoreImage();
	}
}

void InverseProcessor::processMonoImage(MonoImage* image)
{
	assert(image);

	int width = image->width();
	int height = image->height();
	uchar* byteImage = image->getBYTEImage();

	for (int i = 0; i < width * height * 3; i++)
	{
		byteImage[i] = 255 - byteImage[i];
	}

	image->copyToImage();
}

// Process float array
void InverseProcessor::processArray(float* array, int width, int height, uchar* pByte)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
	assert(array && pByte);
	
}
