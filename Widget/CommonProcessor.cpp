
#include "CommonProcessor.h"

//#include "../Image/BaseImage.h"

CommonProcessor::CommonProcessor()
	: _brightness(0)
	, _contrast(0)
{

}

CommonProcessor::~CommonProcessor()
{

}

void CommonProcessor::processGeneralImage(GeneralImage* image)
{
	assert(image);

	int width = image->width();
	int height = image->height();
	QImage* entityImage = image->getImageEntity();
	QImage* backupImageEntity = image->getBackupImage();
	uchar* pImageData = (uchar*)entityImage->bits();
	uchar* pBackupImageData = (uchar*)backupImageEntity->bits();
	int pitch = entityImage->bytesPerLine();
	int depth = entityImage->depth() / 8;

//	PIProgressInit(VS_PROGRESS_STATUS_BAR, _T("Brightness&Contrast"));

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width * depth; i++)
		{
			uchar* pPixel = pImageData + j * pitch + i;
			uchar* pBackupPixel = pBackupImageData + j * pitch + i;
			float result = (*(pBackupPixel)-127.0f) * (_contrast + 100) / 100.0f + 127.0f;
			result *= (100.0f + _brightness / 2.0f) / 100.0f;
			if (result >= 255)
			{
				*(pPixel) = 255;
			}
			else if (result <= 0)
			{
				*(pPixel) = 0;
			}
			else
			{
				*(pPixel) = round(result);
			}
		}

	//	PIProgressSetPercent((j + 1), height);
	}

//	PIProgressDone();
}

template<typename Type>
void CommonProcessor::processTemplate(TemplateImage<Type>* image)
{
	assert(image);

	int width = image->width();
	int height = image->height();
	Type* pProcessingData = image->getProcessingData();
	uchar* pBYTEImage = image->getBYTEImage();
	Type maxValue = image->getMaximumValue();
	Type minValue = image->getMinimumValue();

	float average = float(minValue + maxValue) / 2.0f;
	float variable;
	if (maxValue != minValue)
	{
		variable = 255.0f / float(maxValue - minValue);
	}
	else
	{
		variable = 0.0f;
	}

	for (int i = 0; i < width * height; i++)
	{
		float result = float(pProcessingData[i] - average) * (_contrast + 100) / 100.0f + average;
		result *= (100.0f + _brightness / 2.0f) / 100.0f;
		if (result >= maxValue)
		{
			pBYTEImage[3 * i] = pBYTEImage[3 * i + 1] = pBYTEImage[3 * i + 2] = 255;
		}
		else if (result <= minValue)
		{
			pBYTEImage[3 * i] = pBYTEImage[3 * i + 1] = pBYTEImage[3 * i + 2] = 0;
		}
		else
		{
			pBYTEImage[3 * i] = pBYTEImage[3 * i + 1] = pBYTEImage[3 * i + 2] = uchar((result - minValue) * variable);
		}
	}

	// Copy to image
	image->copyToImage();
}

// Process float array
void CommonProcessor::processArray(float* pArray, int width, int height, float minValue, float maxValue, uchar* pByte)
{
	assert(pArray && pByte);

/*	float fAverage = float(minValue + maxValue) / 2.0f;
	float fVariable;
	if (maxValue != minValue)
	{
		fVariable = 255.0f / float(maxValue - minValue);
	}
	else
	{
		fVariable = 0.0f;
	}

	for (int i = 0; i < width * height; i++)
	{
		float fResult = float(pArray[i] - fAverage) * (m_nContrast + 100) / 100.0f + fAverage;
		fResult *= (100.0f + m_nBrightness / 2.0f) / 100.0f;
		if (fResult >= maxValue)
		{
			pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = 255;
		}
		else if (fResult <= minValue)
		{
			pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = 0;
		}
		else
		{
			pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = BYTE((fResult - minValue) * fVariable);
		}
	}*/
}

void CommonProcessor::setBrightnessAndContrast(int brightness, int contrast)
{
	_brightness = brightness;
	_contrast = contrast;
}