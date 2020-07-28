#include "Document.h"

#include <QSettings>
#include <QCoreApplication>

#include "mainwindow.h"
#include "View.h"
#include "../Image/BaseImage.h"
#include "../Image/GeneralImage.h"
#include "../Image/ScanImage.h"
#include "../Image/DicomImage.h"
#include "../Image/RawImage.h"
#include "../Processor/LevelsProcessor.h"
#include "../Processor/InverseProcessor.h"
#include "../Widget/RawParameterDialog.h"

Document::Document(MainWindow* pWindow)
    : pMainWindow(pWindow)
    , _image(nullptr)
    , _modified(false)
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
        RawParameterDialog dlg(fileName);
        if (dlg.exec() == QDialog::Accepted)
        {
            dlg.dataType();
            // Create raw image
            _image = std::make_shared<RawImage>(fileName, dlg.dataType(), dlg.width(), dlg.height(), dlg.headerSize());
        }
        else
        {
            _image = nullptr;
            return false;
        }
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

    getView()->showImage(_image->getImageEntity(), true);

    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    int displayWindow = settings.value("Image/displayWindow", 0).toInt();
    if (displayWindow == 0)
    {
        getView()->setWindowWidthAndLevel(_image->windowWidth(), _image->windowLevel());
    }
    else
    {
        float windowWidth = _image->getMaxValue() - _image->getMinValue();
        float windowLevel = (_image->getMaxValue() + _image->getMinValue()) / 2;
        getView()->setWindowWidthAndLevel(windowWidth, windowLevel);
    }

    bool fitWindow = settings.value("Image/autoFitWindow", 0).toBool();
    if (fitWindow)
    {
        getView()->fitWindow();
    }
    else
    {
        getView()->zoomNormal();
    }

	getView()->loadGraphicsItem();

    pMainWindow->imageOpened();

    return true;
}

int Document::findType(const QString& fileName)
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
    else if (str.endsWith(".dr") || str.endsWith(".ndr"))
    {
        return IMAGE_FORMAT_NDR;
    }
    else if (str.endsWith(".ct") || str.endsWith(".nct"))
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
    getView()->resetImage();

    _image.reset();

    _undoStack.reset();

    _modified = false;
}
/*
void Document::copyImage(const std::shared_ptr<BaseImage>& image)
{
    if (_image)
    {
        closeFile();
    }

    _image = image;

    _image->histogramStatistic();

    getView()->repaint();
}*/

// Repaint view
void Document::repaintView()
{
    if (_image)
    {
        getView()->showImage(_image->getImageEntity());
    }
}

void Document::saveGraphicsItem()
{
    getView()->saveGraphicsItem();
    _modified = false;
}

void Document::ROIWindow(const QRectF& rect)
{
    if (_image == nullptr)
        return;

    QRectF intersectedRect = rect.intersected(QRectF(0, 0, _image->width(), _image->height()));
    if (intersectedRect.isEmpty())
        return;

    float minValue = _image->getMaxValue();
    float maxValue = _image->getMinValue();
    for (int j = 0; j < intersectedRect.height(); j++)
    {
        for (int i = 0; i < intersectedRect.width(); i++)
        {
            float value = _image->getValue(i + int(intersectedRect.x()), j + int(intersectedRect.y()));
            if (minValue > value)
            {
                minValue = value;
            }
            if (maxValue < value)
            {
                maxValue = value;
            }
        }
    }

    getView()->setWindowWidthAndLevel(maxValue - minValue, (maxValue + minValue) / 2);
}

void Document::defaultImageWindow()
{
    if (_image)
    {
        getView()->setWindowWidthAndLevel(_image->windowWidth(), _image->windowLevel());
    }
}

void Document::fullImageWindow()
{
    if (_image)
    {
        float maxValue = _image->getMaxValue();
        float minValue = _image->getMinValue();

        getView()->setWindowWidthAndLevel(maxValue - minValue, (maxValue + minValue) / 2);
    }
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
    if (_image)
    {
        InverseProcessor processor;
        processor.process(getImage());

        repaintView();
    }
}

void Document::backup()
{
    _undoStack.backup(_image.get());
}

void Document::undo()
{
    if (_undoStack.isEmpty())
        return;

    BaseImage* undoImage = _undoStack.undo()->copyImage();
    _image.reset(undoImage);;
    repaintView();
}

void Document::redo()
{
    if (_undoStack.isTop())
        return;

    BaseImage* undoImage = _undoStack.redo()->copyImage();
    _image.reset(undoImage);
    repaintView();
}

void Document::restore()
{
    if (_image)
    {
        _image->restore();
        repaintView();
    }
}

View* Document::getView() const
{
    return pMainWindow->getView();
}
