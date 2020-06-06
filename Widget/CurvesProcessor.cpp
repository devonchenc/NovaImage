#include "CurvesProcessor.h"

#include "../Image/GeneralImage.h"
#include "../Image/MonoImage.h"

CurvesProcessor::CurvesProcessor()
{

}

CurvesProcessor::~CurvesProcessor()
{

}

void CurvesProcessor::processGeneralImage(GeneralImage* image)
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

//	PIProgressInit(VS_PROGRESS_STATUS_BAR, _T("Curves"));

	float variable = 255.0f / float(_arrayNum - 1);

	if (_channel == CURVE_CHANNEL_GRAY)
	{
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width * depth; i++)
			{
				uchar* pPixel = pImageData + j * pitch + i;
				uchar* pBackupPixel = pBackupImageData + j * pitch + i;
				*(pPixel) = interpolation(*pBackupPixel, _arrayIntensity, _arrayNum, variable);
			}

		//	PIProgressSetPercent((j + 1), height);
		}
	}
	else
	{
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				uchar* pPixel = pImageData + j * pitch + i * depth;
				uchar* pBackupPixel = pBackupImageData + j * pitch + i * depth;
				*(pPixel) = interpolation(*(pBackupPixel), _arrayBlue, _arrayNum, variable);
				*(pPixel + 1) = interpolation(*(pBackupPixel + 1), _arrayGreen, _arrayNum, variable);
				*(pPixel + 2) = interpolation(*(pBackupPixel + 2), _arrayRed, _arrayNum, variable);
			}

		//	PIProgressSetPercent((j + 1), height);
		}
	}

//	PIProgressDone();
}

void CurvesProcessor::processMonoImage(MonoImage* image)
{
	assert(image);

	int width = image->width();
	int height = image->height();;
	uchar* byteImage = image->getBYTEImage();
	float maxValue = image->getMaxValue();
	float minValue = image->getMinValue();

	float fVariable1 = float(maxValue - minValue) / (_arrayNum - 1);
	float fVariable2 = 255.0f / (_arrayNum - 1);

	if (_channel == CURVE_CHANNEL_GRAY)
	{
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				int n = j * width + i;
				byteImage[3 * n] = byteImage[3 * n + 1] = byteImage[3 * n + 2] =
					interpolation(image->getValue(n) - minValue, _arrayIntensity, _arrayNum, fVariable1, fVariable2);
			}

			//	PIProgressSetPercent(j + 1, height);
		}
	}
	else
	{
		// Pseudo-color
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				int n = j * width + i;
				byteImage[3 * n] = interpolation(image->getValue(n) - minValue, _arrayRed, _arrayNum, fVariable1, fVariable2);
				byteImage[3 * n + 1] = interpolation(image->getValue(n) - minValue, _arrayGreen, _arrayNum, fVariable1, fVariable2);
				byteImage[3 * n + 2] = interpolation(image->getValue(n) - minValue, _arrayBlue, _arrayNum, fVariable1, fVariable2);
			}

			//	PIProgressSetPercent(j + 1, height);
		}
	}

	image->copyToImage();

	//	PIProgressDone();
}

// Process float array
void CurvesProcessor::processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
{
	assert(array && pByte);
}

void CurvesProcessor::setArray(int arrayNum, uint* arrayIntensity, uint* arrayRed, uint* arrayGreen, uint* arrayBlue)
{
	_arrayNum = arrayNum;

	_arrayIntensity = arrayIntensity;
	_arrayRed = arrayRed;
	_arrayGreen = arrayGreen;
	_arrayBlue = arrayBlue;
}

// Interpolation
uchar CurvesProcessor::interpolation(uchar target, uint* array, int arrayNum, float variable)
{
	if (variable == 0)
		return array[0];

	float percentage = target / variable;
	int integer = int(percentage);
	float fraction = percentage - integer;
	if (integer == arrayNum - 1)
	{
		return uchar(round(array[integer] * variable));
	}
	else
	{
		return uchar(round((array[integer] * (1.0f - fraction) + array[integer + 1] * fraction) * variable));
	}
}

template <typename Type>
uchar CurvesProcessor::interpolation(Type target, uint* array, int arrayNum, float variable1, float variable2)
{
	if (variable1 == 0)
		return array[0];

	float percentage = float(target) / variable1;
	int integer = int(percentage);
	float fraction = percentage - integer;
	if (integer == arrayNum - 1)
	{
		return (uchar(round(array[integer] * variable2)));
	}
	else
	{
		return (uchar(round((array[integer] * (1.0f - fraction) + array[integer + 1] * fraction) * variable2)));
	}
}