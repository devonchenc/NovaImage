#include "DicomImage.h"

#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>

#include <QDebug>
#include "ImageDataTemplate.h"

DICOMImage::DICOMImage(const QString& pathName)
	: MonoImage(pathName)
	, _rescaleSlope(0)
	, _rescaleIntercept(0)
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
	if (_imageData->findTopAndBottom() == false)
	{
		_openSucceed = false;
		return;
	}

	rescaleArray();
	
	initWindowWidthAndLevel();

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

void DICOMImage::initWindowWidthAndLevel()
{
	if (_windowWidth == 0 && _windowLevel == 0)
	{
		_windowWidth = _imageData->getMaximumValue() - _imageData->getMinimumValue();
		_windowLevel = (_imageData->getMaximumValue() + _imageData->getMinimumValue()) / 2.0f;
	}
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

	readMoreInfo(dataset);

	DJDecoderRegistration::cleanup();

	return true;
}

void DICOMImage::readMoreInfo(DcmDataset* dataset)
{
	const char* tagValue = nullptr;
	dataset->findAndGetString(DCM_PixelSpacing, tagValue);
	if (tagValue)
	{
		_horzPixelSpacing = _vertPixelSpacing = atof(tagValue);
	}

	tagValue = nullptr;
	dataset->findAndGetString(DCM_DistanceSourceToPatient, tagValue);
	if (tagValue)
	{
	//	m_DataHeader.MESOD = float(atof(tagValue));
	}

	tagValue = nullptr;
	dataset->findAndGetString(DCM_DistanceSourceToDetector, tagValue);
	if (tagValue)
	{
		//	m_DataHeader.MESOD = float(atof(tagValue));
	}

	tagValue = nullptr;
	dataset->findAndGetString(DCM_DetectorElementSize, tagValue);
	if (tagValue)
	{
		//	m_DataHeader.MEUnitWidth = float(atof(tagValue));
	}

	tagValue = nullptr;
	dataset->findAndGetString(DCM_WindowCenter, tagValue);
	if (tagValue)
	{
		_windowLevel = float(atof(tagValue));
	}

	tagValue = nullptr;
	dataset->findAndGetString(DCM_WindowWidth, tagValue);
	if (tagValue)
	{
		_windowWidth = float(atof(tagValue));
	}

	tagValue = nullptr;
	dataset->findAndGetString(DCM_RescaleSlope, tagValue);
	if (tagValue)
	{
		_rescaleSlope = float(atof(tagValue));
	}

	tagValue = nullptr;
	dataset->findAndGetString(DCM_RescaleIntercept, tagValue);
	if (tagValue)
	{
		_rescaleIntercept = float(atof(tagValue));
	}
}

void DICOMImage::rescaleArray()
{
	if (_rescaleSlope != 0)
	{
		_imageData->rescaleArray(_rescaleSlope * 0.5f, _rescaleIntercept);
	}
}