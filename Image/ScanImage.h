#pragma once

#include "TemplateImage.h"
#include "Dataheader.h"

class ScanImage : public TemplateImage<float>
{
public:
	ScanImage();
	ScanImage(const QString& pathName);
	~ScanImage();

public:
	// Get reference of DataHeader
	DataHeader& getDataHeader()		{ return _dh; }

private:
	// Read data header
	bool readDataHeader();

	// Read data
	bool readData();

private:
	DataHeader _dh;
};