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
	// Histogram statistic
	void histogramStatistic() override;

	float getValue(const QPoint& position) const override;

	int slice() const override { return _slice; }

	float getMinValue() const override;

	float getMaxValue() const override;

protected:
	bool allocateMemory();

protected:
	ImageData* _imageData;

	int _slice;

	int _currentSlice;
};