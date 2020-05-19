#pragma once

#include "BaseImage.h"

class GeneralImage : public BaseImage
{
public:
	GeneralImage();
	GeneralImage(const QString& pathName);
	virtual ~GeneralImage();

public:
	// Get backup QImage pointer
	QImage* getBackupImage() const { return _backupImage; }

	// Restore image by using backup image
	void restoreImage();

	// Calculate new color
	uchar calcNewColor(uchar color, float bottom, float mid, float top, int minColor, int maxColor);

	// Histogram statistic
	virtual void histogramStatistic();

    virtual float getValue(const QPoint& position) const;

	virtual float getMinValue() { return 0.0f; }

	virtual float getMaxValue() { return 255.0f; }

private:
	// 得到图像中最大最小的像素值
	void getImageBottomAndTop();

	// Backup origin QImage
	void backupImage();

private:
	// Backup origin QImage
	QImage* _backupImage;
};