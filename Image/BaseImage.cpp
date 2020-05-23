#include "BaseImage.h"

#include <dcmtk/dcmdata/dctk.h>
#include <string>
#include <QDebug>

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
		DcmFileFormat fileformat;
		DcmDataset* dataset = fileformat.getDataset();
	/*	dataset->putAndInsertString(DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);
	//	dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
		dataset->putAndInsertString(DCM_PatientName, "Doe^John");
		dataset->putAndInsertString(DCM_WindowCenter, "0");
		dataset->putAndInsertString(DCM_WindowWidth, "200");

		Uint8* buffer = new Uint8[_width * _height];
		for (int j = 0; j < _height; j++)
		{
			for (int i = 0; i < _width; i++)
			{
				QRgb value = getPixel(QPoint(i, j));
				buffer[j * _width + i] = qRed(value);
			}
		}

		dataset->putAndInsertUint8Array(DCM_PixelData, buffer, _width * _height);
		OFCondition condition = fileformat.saveFile("D:/test.dcm", EXS_LittleEndianExplicit);
		if (condition.bad())
			qDebug() << "Error: cannot write DICOM file (" << condition.text() << ")";
		*/
		OFString strTagValue;
		if (dataset->putAndInsertString(DCM_PatientName, "John Doe").good())
		{
			dataset->findAndGetOFString(DCM_PatientName, strTagValue);
		}
		
		std::string str = std::to_string(_width);
		if (dataset->putAndInsertString(DCM_Columns, str.c_str()).good())
		{
			dataset->findAndGetOFString(DCM_Columns, strTagValue);
		}
		str = std::to_string(_height);
		if (dataset->putAndInsertString(DCM_Rows, str.c_str()).good())
		{
			dataset->findAndGetOFString(DCM_Rows, strTagValue);
		}

		dataset->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
		dataset->findAndGetOFString(DCM_PhotometricInterpretation, strTagValue);
		// 修改窗宽窗位
		if (dataset->putAndInsertString(DCM_WindowCenter, "0").good())
		{
			dataset->findAndGetOFString(DCM_WindowCenter, strTagValue);
		}
		if (dataset->putAndInsertString(DCM_WindowWidth, "2000").good())
		{
			dataset->findAndGetOFString(DCM_WindowWidth, strTagValue);
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

		OFCondition condition = dataset->putAndInsertUint16Array(DCM_PixelData, buffer, _width * _height, true);
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

		dataset->putAndInsertString(DCM_BitsStored, "12");
		dataset->findAndGetOFString(DCM_BitsStored, strTagValue);

		dataset->putAndInsertString(DCM_HighBit, "11");
		dataset->findAndGetOFString(DCM_HighBit, strTagValue);

		dataset->putAndInsertString(DCM_PixelRepresentation, "0");
		dataset->findAndGetOFString(DCM_PixelRepresentation, strTagValue);

		dataset->putAndInsertString(DCM_RescaleIntercept, "-1000");
		dataset->findAndGetOFString(DCM_RescaleIntercept, strTagValue);

		dataset->putAndInsertString(DCM_SamplesPerPixel, "1");
		dataset->findAndGetOFString(DCM_SamplesPerPixel, strTagValue);

		dataset->putAndInsertString(DCM_BitsAllocated, "16");
		dataset->findAndGetOFString(DCM_BitsAllocated, strTagValue);

		condition = dataset->saveFile(fileName.toStdString().c_str(), EXS_LittleEndianExplicit);
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

