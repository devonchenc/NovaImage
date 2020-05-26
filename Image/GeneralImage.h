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

public:
	// Histogram statistic
	void histogramStatistic() override;

	float getValue(const QPoint& position) const override;
	float getValue(int index) const override;

	float getMinValue() const override { return 0.0f; }

	float getMaxValue() const override { return 255.0f; }

private:
	// 得到图像中最大最小的像素值
	void getImageBottomAndTop();

	// Backup origin QImage
	void backupImage();

private:
	// Backup origin QImage
	QImage* _backupImage;
};