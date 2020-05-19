﻿#pragma once

#include <QString>
#include <QPair>
#include <QVector>

class MainWindow;
class View;
class BaseImage;

enum ENUM_IMAGE_FORMAT
{
	IMAGE_FORMAT_UNKNOWN = 0,
	IMAGE_FORMAT_JPG = 1,
	IMAGE_FORMAT_PNG = 2,
	IMAGE_FORMAT_TIF = 3,
	IMAGE_FORMAT_BMP = 4,
	IMAGE_FORMAT_GIF = 5,
	IMAGE_FORMAT_NDR = 6,
	IMAGE_FORMAT_NCT = 7,
	IMAGE_FORMAT_RAW = 8,
	IMAGE_FORMAT_DAT = 9,
};

class Document
{
public:
	Document(MainWindow* pWindow);
	~Document();

public:
	bool openFile(const QString& fileName);

	bool saveAs(const QString& fileName);

	void closeFile();

	BaseImage* getImage() const { return _image.get(); }

	void copyImage(const std::shared_ptr<BaseImage>& image);

	float getZoomFactor() const { return _zoomFactor; }
	void setZoomFactor(float factor);

	// Repaint view
	void repaintView();

private:
	View* getView() const;

	int findType(const QString& fileName);

private:
	MainWindow* pMainWindow;
	std::shared_ptr<BaseImage> _image;
	float _zoomFactor;
};