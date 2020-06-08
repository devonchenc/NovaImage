#include "Document.h"

#include "mainwindow.h"
#include "View.h"
#include "Image/BaseImage.h"
#include "Image/GeneralImage.h"
#include "Image/ScanImage.h"
#include "Image/DicomImage.h"
#include "Processor/LevelsProcessor.h"
#include "Processor/InverseProcessor.h"

Document::Document(MainWindow* pWindow)
	: pMainWindow(pWindow)
	, _image(nullptr)
{

}

Document::~Document()
{

}

bool Document::openFile(const QString& fileName)
{
	if (_image)
	{
		closeFile();
	}

	int type = findType(fileName);
	if (type == IMAGE_FORMAT_NDR || type == IMAGE_FORMAT_NCT)
	{
		_image = std::make_shared<ScanImage>(fileName);
	}
	else if (type == IMAGE_FORMAT_DICOM)
	{
		_image = std::make_shared<DICOMImage>(fileName);
	}
	else if (type == IMAGE_FORMAT_DAT || type == IMAGE_FORMAT_RAW)
	{

	}
	else if (type != IMAGE_FORMAT_UNKNOWN)
	{
		_image = std::make_shared<GeneralImage>(fileName);
	}
	else
	{
		_image = nullptr;
		return false;
	}

	// Judge whether open file successfully
	if (_image->isOpenSucceed() == false)
	{
		_image = nullptr;
		return false;
	}

	_image->histogramStatistic();

	getView()->showImage(_image->getImageEntity());
	getView()->setWindowWidthAndLevel(_image->getMaxValue() - _image->getMinValue(), (_image->getMaxValue() + _image->getMinValue()) / 2);
//	getView()->fitWindow();

	pMainWindow->imageOpened();

	return true;
}

int Document::findType(const QString &fileName)
{
	QString str = fileName.toLower();
	if (str.endsWith(".jpg") || str.endsWith(".jpeg"))
	{
		return IMAGE_FORMAT_JPG;
	}
	else if (str.endsWith(".png"))
	{
		return IMAGE_FORMAT_PNG;
	}
	else if (str.endsWith(".tif") || str.endsWith(".tiff"))
	{
		return IMAGE_FORMAT_TIF;
	}
	else if (str.endsWith(".bmp"))
	{
		return IMAGE_FORMAT_BMP;
	}
	else if (str.endsWith(".GIF"))
	{
		return IMAGE_FORMAT_GIF;
	}
	else if (str.endsWith(".dr"))
	{
		return IMAGE_FORMAT_NDR;
	}
	else if (str.endsWith(".ct"))
	{
		return IMAGE_FORMAT_NCT;
	}
	else if (str.endsWith(".raw"))
	{
		return IMAGE_FORMAT_RAW;
	}
	else if (str.endsWith(".dat"))
	{
		return IMAGE_FORMAT_DAT;
	}
	else if (str.endsWith(".dcm"))
	{
		return IMAGE_FORMAT_DICOM;
	}

	return IMAGE_FORMAT_UNKNOWN;
}

bool Document::saveAs(const QString& fileName)
{
	if (_image)
	{
		return _image->save(fileName);
	}
	else
	{
		return false;
	}
}

void Document::closeFile()
{
	_image.reset();

	getView()->resetImage();
}

void Document::copyImage(const std::shared_ptr<BaseImage>& image)
{
	if (_image)
	{
		closeFile();
	}

	_image = image;

	_image->histogramStatistic();

//	getView()->zoomFitWindow();
	getView()->repaint();
}

// Repaint view
void Document::repaintView()
{
	getView()->showImage(_image->getImageEntity());
}

void Document::restoreImageWindow()
{
	float maxValue = _image->getMaxValue();
	float minValue = _image->getMinValue();

	getView()->setWindowWidthAndLevel(maxValue - minValue, (maxValue + minValue) / 2);
}

void Document::applyImageWidthAndLevel()
{
	float windowWidth = getView()->windowWidth();
	float windowLevel = getView()->windowLevel();

	LevelsProcessor processor;
	processor.setPara(windowLevel - windowWidth / 2, 1.0f, windowLevel + windowWidth / 2);
	processor.process(getImage());

	repaintView();
}

void Document::inverseImage()
{
	float maxValue = _image->getMaxValue();
	float minValue = _image->getMinValue();
	InverseProcessor processor;
	processor.process(getImage());

	repaintView();
}

View* Document::getView() const
{
	return pMainWindow->getView();
}