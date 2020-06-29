#pragma once

#include "MonoImage.h"
#include "Dataheader.h"

class ScanImage : public MonoImage
{
public:
	ScanImage(const QString& pathName);
	virtual ~ScanImage();

public:
	// Get reference of DataHeader
	DataHeader& getDataHeader()		{ return _dataHeader; }

protected:
	bool saveAsDcm(const QString& fileName) override;

	void initWindowWidthAndLevel() override;

private:
	// Read data header
	bool readDataHeader();

	// Read data
	bool readData();

private:
	DataHeader _dataHeader;
};