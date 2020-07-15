#pragma once

#include <QString>
#include <QImage>

class BaseImage
{
public:
	BaseImage();
	BaseImage(const QString& pathName);
	BaseImage(const BaseImage& src);
	virtual ~BaseImage();

public:
	// Get min and max value of image
	virtual float getMinValue() const = 0;
	virtual float getMaxValue() const = 0;

	// Histogram statistic
	virtual void histogramStatistic() = 0;

	virtual float getValue(const QPoint& position) const = 0;
	virtual float getValue(int x, int y) const = 0;
	virtual float getValue(int index) const = 0;
	virtual float getValue(float x, float y) const;

	virtual bool hasPixelSpacing()	{ return false; }

	virtual float horzPixelSpacing() { return 0; }
	virtual float vertPixelSpacing() { return 0; }

	// Create a deep copy of image
	virtual BaseImage* copyImage() const = 0;

	virtual void restore() = 0;

public:
	QImage* getImageEntity() const { return _pImage; }

	int width() const { return _width; }

	int height() const { return _height; }

	virtual int slice() const { return 1; }

	float windowWidth() { return _windowWidth; }

	float windowLevel() { return _windowLevel; }

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
	virtual bool saveAsDcm(const QString& fileName);

	virtual bool saveAsRaw(const QString& fileName);

protected:
	QImage* _pImage;

	int _width, _height;

	float _windowWidth;
	float _windowLevel;

	QString _pathName;

	bool _openSucceed;

	// Storage the pixel number of each channel(0-255)
	uint _grayPixelNumber[256];
	uint _redPixelNumber[256];
	uint _greenPixelNumber[256];
	uint _bluePixelNumber[256];
};