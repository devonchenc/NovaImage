#pragma once

#include "TemplateImage.h"

class DICOMImage : public TemplateImage<ushort>
{
public:
	DICOMImage();
	DICOMImage(const QString& pathName);
	~DICOMImage();

public:

private:
	// Read data
	bool readData();

private:

};