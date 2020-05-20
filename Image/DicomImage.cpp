#include "DicomImage.h"

#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>

#include <QDebug>

DICOMImage::DICOMImage()
{

}

DICOMImage::DICOMImage(const QString& pathName) :
	TemplateImage(pathName)
{
	// Read data
	if (readData() == false)
	{
		_openSucceed = false;
		return;
	}
	// Allocate memory
/*	if (allocateMemory() == false)
	{
		_openSucceed = false;
		return;
	}
	// Find top and bottom value in data
	if (findTopAndBottom(_originalData, _width * _height) == false)
	{
		_openSucceed = false;
		return;
	}
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
	*/
	_openSucceed = true;
}

DICOMImage::~DICOMImage()
{

}

// Read data
bool DICOMImage::readData()
{
	DJDecoderRegistration::registerCodecs();

	std::string str = _pathName.toStdString();

	DcmFileFormat fileformat;
	OFCondition condition = fileformat.loadFile(str.c_str());
	if (!condition.good())
	{
		qDebug() << "Load Dimcom File Error: " << condition.text();
		return false;
	}

	DcmDataset* pDataset = fileformat.getDataset();
	DicomImage* pDicomImg = new DicomImage(pDataset, pDataset->getOriginalXfer());
	if (pDicomImg->getStatus() != EIS_Normal)
	{
		qDebug() << pDicomImg->getStatus();
		return false;
	}

	return true;
}
