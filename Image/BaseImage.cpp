#include "BaseImage.h"

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <string>
#include <QDebug>
#include <QFile>

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

QRgb BaseImage::getPixel(int x, int y) const
{
	return _pImage->pixel(QPoint(x, y));
}

bool BaseImage::save(const QString& fileName)
{
	if (fileName.endsWith("dcm", Qt::CaseInsensitive))
	{
		return saveAsDcm(fileName);
	}
	else if (fileName.endsWith("raw", Qt::CaseInsensitive))
	{
		return saveAsRaw(fileName);
	}
	else
	{
		return getImageEntity()->save(fileName);
	}
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

bool BaseImage::saveAsDcm(const QString& fileName)
{
	Uint8* buffer = new Uint8[_width * _height];
	for (int j = 0; j < _height; j++)
	{
		for (int i = 0; i < _width; i++)
		{
			QRgb value = getPixel(QPoint(i, j));
			buffer[j * _width + i] = qRed(value);
		}
	}

	DcmFileFormat fileformat;
	DcmDataset* dataset = fileformat.getDataset();
	OFCondition condition = dataset->putAndInsertUint8Array(DCM_PixelData, buffer, _width * _height, true);
	delete[] buffer;
	if (condition.bad())
	{
		qDebug() << "Error: cannot insert array (" << condition.text() << ")";
		return false;
	}

	condition = dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
	if (condition.bad())
	{
		qDebug() << "Error: cannot choose representation (" << condition.text() << ")";
		return false;
	}

	dataset->putAndInsertString(DCM_Modality, "CT");
	dataset->putAndInsertUint16(DCM_Columns, _width);
	dataset->putAndInsertUint16(DCM_Rows, _height);
	dataset->putAndInsertString(DCM_NumberOfFrames, "1");
	dataset->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
	dataset->putAndInsertString(DCM_BitsAllocated, "8");
	dataset->putAndInsertString(DCM_BitsStored, "8");
	dataset->putAndInsertString(DCM_HighBit, "7");
	dataset->putAndInsertString(DCM_PixelRepresentation, "0");
	condition = dataset->saveFile(fileName.toStdString().c_str(), EXS_LittleEndianExplicit);
	return condition.good();
}

bool BaseImage::saveAsRaw(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly))
		return false;

	uchar* buffer = new uchar[_width * _height];
	for (int j = 0; j < _height; j++)
	{
		for (int i = 0; i < _width; i++)
		{
			QRgb value = getPixel(QPoint(i, j));
			buffer[j * _width + i] = qRed(value);
		}
	}

	file.write((const char*)buffer, sizeof(uchar) * _width * _height);
	file.close();

	delete[] buffer;

	return true;
}