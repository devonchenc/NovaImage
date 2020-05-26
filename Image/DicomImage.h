#pragma once

#include "MonoImage.h"

class DICOMImage : public MonoImage
{
public:
	DICOMImage();
	DICOMImage(const QString& pathName);
	virtual ~DICOMImage();

private:
	// Read data
	bool readData();

private:

};