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
	if (allocateMemory() == false)
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
	E_TransferSyntax xfer = pDataset->getOriginalXfer();
	if (xfer == EXS_JPEG2000)
	{
		qDebug() << "Do not support JPEG2000 transfer syntax";
		return false;
	}
	DicomImage* pDicomImg = new DicomImage(pDataset, xfer);
	if (pDicomImg->getStatus() != EIS_Normal)
	{
		qDebug() << pDicomImg->getStatus();
		return false;
	}

	_width = pDicomImg->getWidth();
	_height = pDicomImg->getHeight();
	_slice = pDicomImg->getNumberOfFrames();
	int depth = pDicomImg->getDepth();

	ushort* pixelData = (ushort*)(pDicomImg->getOutputData(depth));

	_originalData = new ushort[_width * _height];
	memcpy(_originalData, pixelData, sizeof(ushort) * _width * _height);

	DJDecoderRegistration::cleanup();

	return true;
}
