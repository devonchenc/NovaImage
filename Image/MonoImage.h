#pragma once

#include "BaseImage.h"

class ImageData;

class MonoImage : public BaseImage
{
public:
	MonoImage();
	MonoImage(const QString& pathName);
	virtual ~MonoImage();

	bool copyToImage();

public:
	int slice() const override { return _slice; }

	float getMinValue() const override;

	float getMaxValue() const override;

	// Histogram statistic
	void histogramStatistic() override;

	float getValue(const QPoint& position) const override;
	float getValue(int x, int y) const override;
	float getValue(int index) const override;

	bool hasPixelSpacing() override { return false; }

	float horzPixelSpacing() override { return 0; }
	float vertPixelSpacing() override { return 0; }

	virtual void initWindowWidthAndLevel() = 0;

public:
	// get uchar data
	uchar* getBYTEImage() { return _byteImage; }

	bool convertToByte();

protected:
	bool saveAsRaw(const QString& fileName) override;

protected:
	bool allocateMemory();

protected:
	ImageData* _imageData;

	uchar* _byteImage;

	int _slice;

	int _currentSlice;
};