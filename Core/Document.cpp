#include "Document.h"

#include <QSettings>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QTextStream>

#include "mainwindow.h"
#include "View.h"
#include "../Image/BaseImage.h"
#include "../Image/GeneralImage.h"
#include "../Image/ScanImage.h"
#include "../Image/DicomImage.h"
#include "../Image/RawImage.h"
#include "../Processor/LevelsProcessor.h"
#include "../Processor/InverseProcessor.h"
#include "../Processor/BrightnessAndContrastProcessor.h"
#include "../Processor/GammaTransformationProcessor.h"
#include "../Processor/ThresholdSegmentationProcessor.h"
#include "../Processor/EqualizationProcessor.h"
#include "../Processor/EnhancementProcessor.h"
#include "../Processor/LightFieldCorrectionProcessor.h"
#include "../Dialog/RawParameterDialog.h"

Document::Document(MainWindow* pWindow)
    : QObject(pWindow)
    , _mainWindow(pWindow)
    , _image(nullptr)
    , _modified(false)
{

}

bool Document::openFile(const QString& fileName)
{
    if (_image)
    {
        closeFile();
    }

    int type = findImageType(fileName);
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
            _image = std::make_shared<RawImage>(fileName, dlg.dataType(), dlg.width(), dlg.height(), dlg.slice(), dlg.headerSize(), dlg.endian());
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

    imageOpened();

    return true;
}

bool Document::openFolder(const QString& pathName)
{
    QDir dir(pathName);
    QStringList nameFilters;
    nameFilters << "*.dcm";
    QStringList fileList = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    if (fileList.isEmpty())
        return false;

    if (_image)
    {
        closeFile();
    }

    QVector<std::shared_ptr<DICOMImage>> imageVector;
    for (int i = 0; i < fileList.size(); i++)
    {
        QString fileName = dir.absoluteFilePath(fileList[i]);
        std::shared_ptr<DICOMImage> dicomImage = std::make_shared<DICOMImage>(fileName);

        // Judge whether open file successfully
        if (dicomImage->isOpenSucceed() == true)
        {
            imageVector.append(dicomImage);
        }
    }

    _image = std::make_shared<DICOMImage>(imageVector);

    // Judge whether open file successfully
    if (_image->isOpenSucceed() == false)
    {
        _image = nullptr;
        return false;
    }

    imageOpened();

    return true;
}

int Document::findImageType(const QString& fileName)
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
    else if (str.endsWith(".gif"))
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
    getAxialView()->resetImage();
    getCoronalView()->resetImage();
    getSagittalView()->resetImage();

    _image.reset();

    _undoStack.reset();

    _modified = false;

    if (BaseProcessor::currentProcessor())
    {
        BaseProcessor::currentProcessor()->setImage(nullptr);
    }
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
        getAxialView()->showImage(_image->getImageEntity());
        MonoImage* monoImage = dynamic_cast<MonoImage*>(_image.get());
        if (monoImage && monoImage->slice() > 1)
        {
            getCoronalView()->showImage(monoImage->getCoronalImage().get());
            getSagittalView()->showImage(monoImage->getSagittalImage().get());
        }
    }
}

void Document::repaintView(QImage* image, int viewType)
{
    if (viewType == 0)
    {
        getAxialView()->showImage(image);
    }
    else if (viewType == 1)
    {
        getCoronalView()->showImage(image);
    }
    else if (viewType == 2)
    {
        getSagittalView()->showImage(image);
    }
}

void Document::repaintView(std::shared_ptr<QImage> dstImage, int viewType)
{
    if (viewType == 0)
    {
        getAxialView()->showImage(dstImage.get());
    }
    else if (viewType == 1)
    {
        getCoronalView()->showImage(dstImage.get());
    }
    else if (viewType == 2)
    {
        getSagittalView()->showImage(dstImage.get());
    }
}

void Document::repaintView(std::vector<std::shared_ptr<QImage>> imageVec)
{
    getAxialView()->showImage(imageVec[0].get());
    if (imageVec[1].get())
    {
        getCoronalView()->showImage(imageVec[1].get());
    }
    if (imageVec[2].get())
    {
        getSagittalView()->showImage(imageVec[2].get());
    }
}

void Document::setModified(bool flag)
{
    if (_image)
    {
        _modified = flag;
    }
}

void Document::saveGraphicsItems()
{
    QString str = _image->getPathName();
    int index = str.lastIndexOf('.');
    QString fileName = str.left(index) + ".xml";

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QDomDocument doc("NovaImage");
    QDomElement root = doc.createElement("Views");
    doc.appendChild(root);

    getAxialView()->saveGraphicsItem(doc, root);
    getCoronalView()->saveGraphicsItem(doc, root);
    getSagittalView()->saveGraphicsItem(doc, root);

    QTextStream txtOutput(&file);
    txtOutput.setCodec("UTF-8");
    txtOutput << doc.toString();
    file.close();

    _modified = false;
}

void Document::loadGraphicsItems()
{
    QString str = _image->getPathName();
    int index = str.lastIndexOf('.');
    QString fileName = str.left(index) + ".xml";

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QDomDocument doc;
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();

    QDomNodeList domList = doc.elementsByTagName("View");
    for (int i = 0; i < domList.size(); i++)
    {
        QDomElement viewElem = domList.at(i).toElement();
        QDomElement sceneElem = viewElem.firstChild().toElement();
        if (sceneElem.isNull())
            break;

        int type = viewElem.attribute("Type").toInt();
        if (type == 0)
        {
            getAxialView()->loadGraphicsItem(sceneElem);
        }
        else if (type == 1)
        {
            getCoronalView()->loadGraphicsItem(sceneElem);
        }
        else if (type == 2)
        {
            getSagittalView()->loadGraphicsItem(sceneElem);
        }
    }
}

void Document::defaultImageWindow()
{
    if (_image)
    {
        setViewWindowWidthAndLevel(_image->windowWidth(), _image->windowLevel());
        applyImageWidthAndLevel();
    }
}

void Document::fullImageWindow()
{
    if (_image)
    {
        float maxValue = _image->getMaxValue();
        float minValue = _image->getMinValue();

        setViewWindowWidthAndLevel(maxValue - minValue, (maxValue + minValue) / 2);
        applyImageWidthAndLevel();
    }
}

void Document::setViewWindowWidthAndLevel(float windowWidth, float windowLevel)
{
    if (_mainWindow->isViewLinked())
    {
        getAxialView()->setWindowWidthAndLevel(windowWidth, windowLevel);
        getCoronalView()->setWindowWidthAndLevel(windowWidth, windowLevel);
        getSagittalView()->setWindowWidthAndLevel(windowWidth, windowLevel);
    }
    else
    {
        getActiveView()->setWindowWidthAndLevel(windowWidth, windowLevel);
    }
}

void Document::applyImageWidthAndLevel(bool apply)
{
    float windowWidth = getActiveView()->windowWidth();
    float windowLevel = getActiveView()->windowLevel();

    _mainWindow->levelsAdjust(windowLevel - windowWidth / 2, windowLevel + windowWidth / 2);

    if (apply)
    {
        BaseProcessor::currentProcessor()->apply();
    }
}

void Document::negativeImage()
{
    if (_image)
    {
        InverseProcessor* processor = new InverseProcessor(this);
        processor->processForView(getImage());
        processor->apply();
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

    _undoStack.undo()->copyToImage(_image.get());
    repaintView();
}

void Document::redo()
{
    if (_undoStack.isTop())
        return;

    _undoStack.redo()->copyToImage(_image.get());
    repaintView();
}

void Document::restore()
{
    if (_image)
    {
        _image->restore();

        initViewWindowWidthAndLevel();

        repaintView();
    }
}

void Document::brightnessAndContrast()
{
    if (_image)
    {
        BrightnessAndContrastProcessor* processor = new BrightnessAndContrastProcessor(this);
        processor->initUI();
        processor->processForView(getImage());
    }
}

void Document::gammaTransformation()
{
    if (_image)
    {
        GammaTransformationProcessor* processor = new GammaTransformationProcessor(this);
        processor->initUI();
        processor->processForView(getImage());
    }
}

void Document::thresholdSegmentation()
{
    if (_image)
    {
        ThresholdSegmentationProcessor* processor = new ThresholdSegmentationProcessor(this);
        processor->setImage(getImage());
        processor->initUI();
        processor->processForView(getImage());
    }
}

void Document::equalization()
{
    if (_image)
    {
        EqualizationProcessor* processor = new EqualizationProcessor(this);
        processor->initUI();
        processor->processForView(getImage());
    }
}

void Document::enhancement()
{
    if (_image)
    {
        EnhancementProcessor* processor = new EnhancementProcessor(this);
        processor->initUI();
        processor->processForView(getImage());
    }
}

void Document::lightFieldCorrection()
{
    if (_image)
    {
        LightFieldCorrectionProcessor* processor = new LightFieldCorrectionProcessor(this);
        processor->initUI();
        processor->processForView(getImage());
    }
}

void Document::imageOpened()
{
    if (_image == nullptr || _image->isOpenSucceed() == false)
        return;

    _image->histogramStatistic();

    initViewWindowWidthAndLevel();

    applyImageWidthAndLevel();

    getAxialView()->resetMatrix();
    getCoronalView()->resetMatrix();
    getSagittalView()->resetMatrix();

    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    bool fitWindow = settings.value("Image/autoFitWindow", 0).toBool();
    if (fitWindow)
    {
        getAxialView()->fitWindow();
        getCoronalView()->fitWindow();
        getSagittalView()->fitWindow();
    }
    else
    {
        getAxialView()->zoomNormal();
        getCoronalView()->zoomNormal();
        getSagittalView()->zoomNormal();
    }

    loadGraphicsItems();

    _mainWindow->imageOpened();
}

void Document::initViewWindowWidthAndLevel()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    int displayWindow = settings.value("Image/displayWindow", 0).toInt();
    if (displayWindow == 0)
    {
        // Default window
        getAxialView()->setWindowWidthAndLevel(_image->windowWidth(), _image->windowLevel());
        getCoronalView()->setWindowWidthAndLevel(_image->windowWidth(), _image->windowLevel());
        getSagittalView()->setWindowWidthAndLevel(_image->windowWidth(), _image->windowLevel());
    }
    else
    {
        // Full window
        float windowWidth = _image->getMaxValue() - _image->getMinValue();
        float windowLevel = (_image->getMaxValue() + _image->getMinValue()) / 2;
        getAxialView()->setWindowWidthAndLevel(windowWidth, windowLevel);
        getCoronalView()->setWindowWidthAndLevel(windowWidth, windowLevel);
        getSagittalView()->setWindowWidthAndLevel(windowWidth, windowLevel);
    }
}

View* Document::getActiveView() const
{
    return _mainWindow->getActiveView();
}

View* Document::getAxialView() const
{
    return _mainWindow->getAxialView();
}

View* Document::getCoronalView() const
{
    return _mainWindow->getCoronalView();
}

View* Document::getSagittalView() const
{
    return _mainWindow->getSagittalView();
}