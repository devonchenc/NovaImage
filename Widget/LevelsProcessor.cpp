
#include "LevelsProcessor.h"

LevelsProcessor::LevelsProcessor()
	: _bottom(0)
	, _mid(0)
	, _top(0)
{

}

LevelsProcessor::~LevelsProcessor()
{

}

void LevelsProcessor::processGeneralImage(GeneralImage* image)
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

	int channel = 0;

	int minColor = 0;
	int maxColor = 255;

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			uchar* pPixel = pImageData + j * pitch + i * depth;
			uchar* pBackupPixel = pBackupImageData + j * pitch + i * depth;
			switch (channel)
			{
			case 0:
				*(pPixel) = image->calcNewColor(*(pBackupPixel), _bottom, _mid, _top, minColor, maxColor);
				*(pPixel + 1) = image->calcNewColor(*(pBackupPixel + 1), _bottom, _mid, _top, minColor, maxColor);
				*(pPixel + 2) = image->calcNewColor(*(pBackupPixel + 2), _bottom, _mid, _top, minColor, maxColor);
				break;
			case 1:
				*(pPixel + 2) = image->calcNewColor(*(pBackupPixel + 2), _bottom, _mid, _top, minColor, maxColor);
				break;
			case 2:
				*(pPixel + 1) = image->calcNewColor(*(pBackupPixel + 1), _bottom, _mid, _top, minColor, maxColor);
				break;
			case 3:
				*(pPixel) = image->calcNewColor(*(pBackupPixel), _bottom, _mid, _top, minColor, maxColor);
				break;
			}
		}
	}
}

template<typename Type>
void LevelsProcessor::processTemplate(TemplateImage<Type>* image)
{
	assert(image);

	int width = image->width();
	int height = image->height();
	Type* pProcessingData = image->getProcessingData();
	uchar* pBYTEImage = image->getBYTEImage();
	Type maxValue = image->getMaximumValue();
	Type minValue = image->getMinimumValue();

	int channel = 0;

	float variable1 = 255.0f / (float)(_top - _bottom);
	int midColor = round(255 * _mid / (1 + _mid));
	float variable2 = (float)(midColor) / (float)((_bottom + _top) / 2.0f - _bottom);
	float variable3 = (float)(255.0f - midColor) / (float)(_top - (_bottom + _top) / 2.0f);

	for (int i = 0; i < width * height; i++)
	{
		if (pProcessingData[i] <= _bottom)
		{
			switch (channel)
			{
			case 0:
				pBYTEImage[3 * i] = pBYTEImage[3 * i + 1] = pBYTEImage[3 * i + 2] = 0;
				break;
			case 1:
				pBYTEImage[3 * i + 2] = 0;
				break;
			case 2:
				pBYTEImage[3 * i + 1] = 0;
				break;
			case 3:
				pBYTEImage[3 * i] = 0;
				break;
			}
		}
		else if (pProcessingData[i] >= _top)
		{
			switch (channel)
			{
			case 0:
				pBYTEImage[3 * i] = pBYTEImage[3 * i + 1] = pBYTEImage[3 * i + 2] = 255;
				break;
			case 1:
				pBYTEImage[3 * i + 2] = 255;
				break;
			case 2:
				pBYTEImage[3 * i + 1] = 255;
				break;
			case 3:
				pBYTEImage[3 * i] = 255;
				break;
			}
		}
		else
		{
			if (_mid == 1)
			{
				switch (channel)
				{
				case 0:
					pBYTEImage[3 * i] = pBYTEImage[3 * i + 1] = pBYTEImage[3 * i + 2] =
						uchar((pProcessingData[i] - _bottom) * variable1);
					break;
				case 1:
					pBYTEImage[3 * i + 2] = uchar((pProcessingData[i] - _bottom) * variable1);
					break;
				case 2:
					pBYTEImage[3 * i + 1] = uchar((pProcessingData[i] - _bottom) * variable1);
					break;
				case 3:
					pBYTEImage[3 * i] = uchar((pProcessingData[i] - _bottom) * variable1);
					break;
				}
			}
			else	// if mid is not equal to 1
			{
				if (pProcessingData[i] < (_bottom + _top) / 2)
				{
					switch (channel)
					{
					case 0:
						pBYTEImage[3 * i] = pBYTEImage[3 * i + 1] = pBYTEImage[3 * i + 2] =
							uchar((pProcessingData[i] - _bottom) * variable2);
						break;
					case 1:
						pBYTEImage[3 * i + 2] = uchar((pProcessingData[i] - _bottom) * variable2);
						break;
					case 2:
						pBYTEImage[3 * i + 1] = uchar((pProcessingData[i] - _bottom) * variable2);
						break;
					case 3:
						pBYTEImage[3 * i] = uchar((pProcessingData[i] - _bottom) * variable2);
						break;
					}
				}
				else
				{
					switch (channel)
					{
					case 0:
						pBYTEImage[3 * i] = pBYTEImage[3 * i + 1] = pBYTEImage[3 * i + 2] =
							uchar((pProcessingData[i] - (_bottom + _top) / 2) * variable3 + midColor);
						break;
					case 1:
						pBYTEImage[3 * i + 2] = uchar((pProcessingData[i] - (_bottom + _top) / 2) * variable3 + midColor);
						break;
					case 2:
						pBYTEImage[3 * i + 1] = uchar((pProcessingData[i] - (_bottom + _top) / 2) * variable3 + midColor);
						break;
					case 3:
						pBYTEImage[3 * i] = uchar((pProcessingData[i] - (_bottom + _top) / 2) * variable3 + midColor);
						break;
					}
				}
			}
		}
	}

	image->copyToImage();
}

// Process float array
void LevelsProcessor::processArray(float* pArray, int width, int height, float minValue, float maxValue, uchar* pByte)
{
	assert(pArray && pByte);

	int channel = 0;


/*	float variable1 = 255.0f / (float)(_top - _bottom);
	int midColor = round(255 * _mid / (1 + _mid));
	float fVariable2 = (float)(midColor) / (float)((_bottom + _top) / 2.0f - _bottom);
	float fVariable3 = (float)(255.0f - midColor) / (float)(_top - (_bottom + fTop) / 2.0f);

	for (int i = 0; i < width * height; i++)
	{
		if (pArray[i] <= _bottom)
		{
			switch (nChannel)
			{
			case 0:
				pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = 0;
				break;
			case 1:
				pByte[3 * i + 2] = 0;
				break;
			case 2:
				pByte[3 * i + 1] = 0;
				break;
			case 3:
				pByte[3 * i] = 0;
				break;
			}
		}
		else if (pArray[i] >= _top)
		{
			switch (nChannel)
			{
			case 0:
				pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = 255;
				break;
			case 1:
				pByte[3 * i + 2] = 255;
				break;
			case 2:
				pByte[3 * i + 1] = 255;
				break;
			case 3:
				pByte[3 * i] = 255;
				break;
			}
		}
		else
		{
			if (_mid == 1)
			{
				switch (channel)
				{
				case 0:
					pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = uchar((pArray[i] - fBottom) * fVariable1);
					break;
				case 1:
					pByte[3 * i + 2] = uchar((pArray[i] - fBottom) * fVariable1);
					break;
				case 2:
					pByte[3 * i + 1] = uchar((pArray[i] - fBottom) * fVariable1);
					break;
				case 3:
					pByte[3 * i] = uchar((pArray[i] - fBottom) * fVariable1);
					break;
				}
			}
			else	// if mid is not equal to 1
			{
				if (pArray[i] < (_bottom + _top) / 2)
				{
					switch (channel)
					{
					case 0:
						pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] = uchar((pArray[i] - fBottom) * fVariable2);
						break;
					case 1:
						pByte[3 * i + 2] = uchar((pArray[i] - fBottom) * fVariable2);
						break;
					case 2:
						pByte[3 * i + 1] = uchar((pArray[i] - fBottom) * fVariable2);
						break;
					case 3:
						pByte[3 * i] = uchar((pArray[i] - fBottom) * fVariable2);
						break;
					}
				}
				else
				{
					switch (nChannel)
					{
					case 0:
						pByte[3 * i] = pByte[3 * i + 1] = pByte[3 * i + 2] =
							BYTE((pArray[i] - (fBottom + fTop) / 2) * fVariable3 + nMidColor);
						break;
					case 1:
						pByte[3 * i + 2] = BYTE((pArray[i] - (fBottom + fTop) / 2) * fVariable3 + nMidColor);
						break;
					case 2:
						pByte[3 * i + 1] = BYTE((pArray[i] - (fBottom + fTop) / 2) * fVariable3 + nMidColor);
						break;
					case 3:
						pByte[3 * i] = BYTE((pArray[i] - (fBottom + fTop) / 2) * fVariable3 + nMidColor);
						break;
					}
				}
			}
		}
	}*/
}

void LevelsProcessor::setPara(float bottom, float mid, float top)
{
	_bottom = bottom;
	_mid = mid;
	_top = top;
}