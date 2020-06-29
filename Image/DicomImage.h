#pragma once

#include "MonoImage.h"

class DcmDataset;

class DICOMImage : public MonoImage
{
public:
	DICOMImage(const QString& pathName);
	virtual ~DICOMImage();

public:
	bool hasPixelSpacing() override { return true; }

	float horzPixelSpacing() override { return _horzPixelSpacing; }
	float vertPixelSpacing() override { return _vertPixelSpacing; }

	void initWindowWidthAndLevel() override;

private:
	// Read data
	bool readData();

	void readMoreInfo(DcmDataset* dataset);

	void rescaleArray();

private:
	float _horzPixelSpacing;
	float _vertPixelSpacing;
	float _imagerPixelSpacing;		// Detector pixel size

	float _rescaleSlope;
	float _rescaleIntercept;
};