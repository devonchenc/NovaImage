#include "HistogramProcessor.h"

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

HistogramProcessor::HistogramProcessor()
	: _array(nullptr)
{

}

HistogramProcessor::~HistogramProcessor()
{

}

void HistogramProcessor::processGeneralImage(GeneralImage* image)
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

	// Statistical non-zero number
	int count = 0;
	int min = _arrayNum - 1;
	int max = 0;
	for (int i = 0; i < _arrayNum; i++)
	{
		if (_array[i] == true)
		{
			if (min > i)
			{
				min = i;
			}
			if (max < i)
			{
				max = i;
			}
			count++;
		}
	}
	if (count == 0)
	{
		// Restore image by using backup image
		image->restoreImage();
		return;
	}

	uchar actualMin = round(min * 255.0f / (_arrayNum - 1));
	uchar actualMax = round(max * 255.0f / (_arrayNum - 1));

	float variable = float(_arrayNum - 1) / 255.0f;

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width * depth; i++)
		{
			uchar* pPixel = pImageData + j * pitch + i;
			uchar* pBackupPixel = pBackupImageData + j * pitch + i;

			if (*(pBackupPixel) >= actualMax)
			{
				*(pPixel) = 255;
			}
			else if (*(pBackupPixel) <= actualMin)
			{
				*(pPixel) = 0;
			}
			else
			{
				int index = round(*(pBackupPixel) * variable);
				if (_array[index])
				{
					*(pPixel) = (*(pBackupPixel) - actualMin) * 255 / (actualMax - actualMin);
				}
				else
				{
					*(pPixel) = 0;
				}
			}
		}
	}
}

void HistogramProcessor::processMonoImage(MonoImage* image)
{
	assert(image);

	int width = image->width();
	int height = image->height();
	uchar* byteImage = image->getBYTEImage();
	float maxValue = image->getMaxValue();
	float minValue = image->getMinValue();

	// Statistical non-zero number
	int count = 0;
	int min = _arrayNum - 1;
	int max = 0;
	for (int i = 0; i < _arrayNum; i++)
	{
		if (_array[i] == true)
		{
			if (min > i)
			{
				min = i;
			}
			if (max < i)
			{
				max = i;
			}
			count++;
		}
	}
	if (count == 0)
	{
		// Convert data to byte
		image->convertToByte();
		image->copyToImage();
		return;
	}

	float actualMin = float(min * float(maxValue - minValue) / (_arrayNum - 1) + minValue);
	float actualMax = float(max * float(maxValue - minValue) / (_arrayNum - 1) + minValue);

	for (int i = 0; i < width * height; i++)
	{
		if (image->getValue(i) >= actualMax)
		{
			byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 255;
		}
		else if (image->getValue(i) <= actualMin)
		{
			byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 0;
		}
		else
		{
			int index = round(float(image->getValue(i) - minValue) * (_arrayNum - 1) / float(maxValue - minValue));
			if (_array[index])
			{
				byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] =
					round((image->getValue(i) - actualMin) * 255.0f / (actualMax - actualMin));
			}
			else
			{
				byteImage[3 * i] = byteImage[3 * i + 1] = byteImage[3 * i + 2] = 0;
			}
		}
	}

	image->copyToImage();
}

// Process float array
void HistogramProcessor::processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
	assert(array && pByte);

}

void HistogramProcessor::setWindowArray(bool* array, int arrayNum)
{
	_array = array;
	_arrayNum = arrayNum;
}