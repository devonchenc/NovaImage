#include "document.h"
#include "mainwindow.h"
#include "view.h"
#include "Image/BaseImage.h"
#include "Image/GeneralImage.h"
#include "Image/ScanImage.h"
#include "Image/DicomImage.h"

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

	// TODO
	_image->histogramStatistic();

//	getView()->zoomFitWindow();
	
	getView()->showImage(_image->getImageEntity());

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
	getView()->repaint();
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

View* Document::getView() const
{
	return pMainWindow->getView();
}

// Repaint view
void Document::repaintView()
{
	getView()->showImage(_image->getImageEntity());
}