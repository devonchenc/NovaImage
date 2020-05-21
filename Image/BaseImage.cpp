#include "BaseImage.h"

#include <dcmtk/dcmdata/dctk.h>
#include <string>

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

bool BaseImage::save(const QString& fileName)
{
	if (fileName.endsWith("dcm", Qt::CaseInsensitive))
	{
		DcmFileFormat newfileformat;
		DcmDataset* newdataset = newfileformat.getDataset();

		OFString strTagValue;
		if (newdataset->putAndInsertString(DCM_PatientName, "John Doe").good())
		{
			newdataset->findAndGetOFString(DCM_PatientName, strTagValue);
		}
		
		std::string str = std::to_string(_width);
		if (newdataset->putAndInsertString(DCM_Columns, str.c_str()).good())
		{
			newdataset->findAndGetOFString(DCM_Columns, strTagValue);
		}
		str = std::to_string(_height);
		if (newdataset->putAndInsertString(DCM_Rows, str.c_str()).good())
		{
			newdataset->findAndGetOFString(DCM_Rows, strTagValue);
		}

		newdataset->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
		newdataset->findAndGetOFString(DCM_PhotometricInterpretation, strTagValue);
		// 修改窗宽窗位
		if (newdataset->putAndInsertString(DCM_WindowCenter, "0").good())
		{
			newdataset->findAndGetOFString(DCM_WindowCenter, strTagValue);
		}
		if (newdataset->putAndInsertString(DCM_WindowWidth, "2000").good())
		{
			newdataset->findAndGetOFString(DCM_WindowWidth, strTagValue);
		}

		Uint16* buffer = new Uint16[_width * _height];
		for (int j = 0; j < _height; j++)
		{
			for (int i = 0; i < _width; i++)
			{
				QRgb value = getPixel(QPoint(i, j));
				buffer[j * _width + i] = qRed(value);
			}
		}

		OFCondition condition = newdataset->putAndInsertUint16Array(DCM_PixelData, buffer, _width * _height, true);
		if (condition.bad())
			return false;

		condition = newdataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
		if (condition.bad())
			return false;

		newdataset->putAndInsertString(DCM_BitsStored, "12");
		newdataset->findAndGetOFString(DCM_BitsStored, strTagValue);

		newdataset->putAndInsertString(DCM_HighBit, "11");
		newdataset->findAndGetOFString(DCM_HighBit, strTagValue);

		newdataset->putAndInsertString(DCM_PixelRepresentation, "0");
		newdataset->findAndGetOFString(DCM_PixelRepresentation, strTagValue);

		newdataset->putAndInsertString(DCM_RescaleIntercept, "-1000");
		newdataset->findAndGetOFString(DCM_RescaleIntercept, strTagValue);

		newdataset->putAndInsertString(DCM_SamplesPerPixel, "1");
		newdataset->findAndGetOFString(DCM_SamplesPerPixel, strTagValue);

		newdataset->putAndInsertString(DCM_BitsAllocated, "16");
		newdataset->findAndGetOFString(DCM_BitsAllocated, strTagValue);

		condition = newdataset->saveFile(fileName.toStdString().c_str(), EXS_LittleEndianExplicit);
		return condition.good();
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

