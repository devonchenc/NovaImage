#pragma once

#include "MonoImage.h"

class RawImage : public MonoImage
{
public:
	RawImage(const QString& pathName, int type, int width, int height);
	virtual ~RawImage();

public:
	bool hasPixelSpacing() override { return true; }

	float horzPixelSpacing() override { return _horzPixelSpacing; }
	float vertPixelSpacing() override { return _vertPixelSpacing; }

private:
	// Read data
	bool readData();

private:
	float _horzPixelSpacing;
	float _vertPixelSpacing;
	float _imagerPixelSpacing;		// Detector pixel size
};