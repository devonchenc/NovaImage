#include "DicomImage.h"

#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>

#include <QDebug>
#include "ImageDataTemplate.h"

DICOMImage::DICOMImage()
{

}

DICOMImage::DICOMImage(const QString& pathName)
	: MonoImage(pathName)
{
	// Read data
	if (readData() == false)
	{
		_openSucceed = false;
		return;
	}
	
	// Find top and bottom value in data
	if (_imageData->findTopAndBottom() == false)
	{
		_openSucceed = false;
		return;
	}
	// Convert float data to uchar data
	if (_imageData->convertToByte(_byteImage) == false)
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

	DcmFileFormat fileformat;
	OFCondition condition = fileformat.loadFile(_pathName.toStdString().c_str());
	if (!condition.good())
	{
		qDebug() << "Load Dimcom File Error: " << condition.text();
		return false;
	}

	DcmDataset* dataset = fileformat.getDataset();
	E_TransferSyntax xfer = dataset->getOriginalXfer();
	if (xfer == EXS_JPEG2000)
	{
		qDebug() << "Do not support JPEG2000 transfer syntax";
		return false;
	}
	DicomImage* pDicomImg = new DicomImage(dataset, xfer);
	if (pDicomImg->getStatus() != EIS_Normal)
	{
		qDebug() << pDicomImg->getStatus();
		return false;
	}

	_width = pDicomImg->getWidth();
	_height = pDicomImg->getHeight();
	_slice = pDicomImg->getNumberOfFrames();

	int depth = pDicomImg->getDepth();
	if (depth <= 8)
	{
		_imageData = new ImageDataTemplate<uchar>(_width * _height);
		const uchar* pixelData = static_cast<const uchar*>(pDicomImg->getOutputData(depth));
		uchar* originalData = static_cast<uchar*>(_imageData->getOriginalData());
		memcpy(originalData, pixelData, sizeof(uchar) * _width * _height);
	}
	else if (depth <= 16)
	{
		_imageData = new ImageDataTemplate<ushort>(_width * _height);
		const ushort* pixelData = static_cast<const ushort*>(pDicomImg->getOutputData(depth));
		ushort* originalData = static_cast<ushort*>(_imageData->getOriginalData());
		memcpy(originalData, pixelData, sizeof(ushort) * _width * _height);
	}
	else if (depth <= 32)
	{
		_imageData = new ImageDataTemplate<uint>(_width * _height);
		const uint* pixelData = static_cast<const uint*>(pDicomImg->getOutputData(depth));
		uint* originalData = static_cast<uint*>(_imageData->getOriginalData());
		memcpy(originalData, pixelData, sizeof(uint) * _width * _height);
	}

	// Allocate memory
	if (allocateMemory() == false)
	{
		_openSucceed = false;
		return false;
	}

	readMoreInfo(dataset);

	DJDecoderRegistration::cleanup();

	return true;
}

void DICOMImage::readMoreInfo(DcmDataset* dataset)
{
	const char* tagValue = new char[30];
	dataset->findAndGetString(DCM_PixelSpacing, tagValue);

	_horzPixelSpacing = _vertPixelSpacing = atof(tagValue);
}