﻿#pragma once

#include <QString>
#include <QImage>

class BaseImage
{
public:
	BaseImage();
	BaseImage(const QString& pathName);
	virtual ~BaseImage();

public:
	QImage* getImageEntity() const	{ return _pImage; }

	int width() const				{ return _width; }

	int height() const				{ return _height; }

	virtual int slice() const		{ return 1; }

	// Get min and max value of image
	virtual float getMinValue() const = 0;
	virtual float getMaxValue() const = 0;

	// Histogram statistic
	virtual void histogramStatistic() = 0;

	virtual float getValue(const QPoint& position) const = 0;
	virtual float getValue(int x, int y) const = 0;
	virtual float getValue(int index) const = 0;

	virtual bool hasPixelSpacing()	{ return false; }

	virtual float horzPixelSpacing() { return 0; }
	virtual float vertPixelSpacing() { return 0; }

public:
	bool isOpenSucceed()			{ return _openSucceed; }

	QString getPathName() const		{ return _pathName; }

	// Get each channel's pixel array
	uint* getGrayPixelArray()		{ return _grayPixelNumber; }
	uint* getRedPixelArray()		{ return _redPixelNumber; }
	uint* getGreenPixelArray()		{ return _greenPixelNumber; }
	uint* getBluePixelArray()		{ return _bluePixelNumber; }

	QRgb getPixel(const QPoint& position) const;
	QRgb getPixel(int x, int y) const;

	bool save(const QString& fileName);
	
    void copyToArray(uchar* array);
	
    bool copyFromArray(uchar* byteImage, int width, int height);

protected:
	QImage* _pImage;

	int _width, _height;

	QString _pathName;

	bool _openSucceed;

	// Storage the pixel number of each channel(0-255)
	uint _grayPixelNumber[256];
	uint _redPixelNumber[256];
	uint _greenPixelNumber[256];
	uint _bluePixelNumber[256];
};