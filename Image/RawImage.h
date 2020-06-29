#pragma once

#include "MonoImage.h"

class RawImage : public MonoImage
{
public:
	RawImage(const QString& pathName, int type, int width, int height, int headerSize);
	virtual ~RawImage();

public:
	void initWindowWidthAndLevel() override;

private:
	// Read data
	bool readData();

private:
	int _dataType;
	int _headerSize;
};