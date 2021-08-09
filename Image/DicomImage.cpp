#include "DicomImage.h"

#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>

#include <QDebug>
#include "ImageDataTemplate.h"

DICOMImage::DICOMImage(const QString& pathName)
    : MonoImage(pathName)
    , _horzPixelSpacing(0)
    , _vertPixelSpacing(0)
    , _imagerPixelSpacing(0)
    , _SOD(0)
    , _SDD(0)
    , _bitsAllocated(0)
    , _bitsStored(0)
    , _rescaleSlope(0)
    , _rescaleIntercept(0)
{
    // Read data
    if (readData() == false)
    {
        _openSucceed = false;
        return;
    }

    // Allocate memory
    if (allocateMemory() == false)
    {
        _openSucceed = false;
        return;
    }

    // Find top and bottom value in data
    if (_imageData->findTopAndBottom() == false)
    {
        _openSucceed = false;
        return;
    }

    rescaleArray();

    rescaleTopAndBottom();

    initWindowWidthAndLevel();

    // Convert float data to uchar data
    if (convertToByte() == false)
    {
        _openSucceed = false;
        return;
    }
    // Copy to image
    if (copyByteToImage() == false)
    {
        _openSucceed = false;
        return;
    }

    _openSucceed = true;
}

DICOMImage::DICOMImage(QVector<std::shared_ptr<DICOMImage>>& imageVector)
    : MonoImage()
{
    _width = imageVector[0]->width();
    _height = imageVector[0]->height();
    _slice = 0;
    int elementSize = imageVector[0]->_imageData->getElementSize();

    _windowWidth = imageVector[0]->_windowWidth;
    _windowLevel = imageVector[0]->_windowLevel;

    _horzPixelSpacing = imageVector[0]->_horzPixelSpacing;
    _vertPixelSpacing = imageVector[0]->_vertPixelSpacing;
    _imagerPixelSpacing = imageVector[0]->_imagerPixelSpacing;
    _SOD = imageVector[0]->_SOD;
    _SDD = imageVector[0]->_SDD;
    _bitsAllocated = imageVector[0]->_bitsAllocated;
    _bitsStored = imageVector[0]->_bitsStored;
    _rescaleSlope = imageVector[0]->_rescaleSlope;
    _rescaleIntercept = imageVector[0]->_rescaleIntercept;

    _pathName = imageVector[0]->_pathName;

    // Count slice
    for (int i = 0; i < imageVector.size(); i++)
    {
        if (imageVector[i]->width() != _width || imageVector[i]->height() != _height)
            continue;

        if (imageVector[i]->_imageData->getElementSize() != elementSize)
            continue;

        _slice += imageVector[i]->slice();
    }

    if (elementSize == 1)
    {
        _imageData = new ImageDataTemplate<uchar>(_width, _height, _slice);
    }
    else if (elementSize == 2)
    {
        _imageData = new ImageDataTemplate<ushort>(_width, _height, _slice);
    }
    else if (elementSize == 4)
    {
        _imageData = new ImageDataTemplate<uint>(_width, _height, _slice);
    }

    // Copy data
    int currentSlice = 0;
    for (int i = 0; i < imageVector.size(); i++)
    {
        if (imageVector[i]->width() != _width || imageVector[i]->height() != _height)
            continue;

        if (imageVector[i]->_imageData->getElementSize() != elementSize)
            continue;

        int singleSlice = imageVector[i]->slice();
        void* singleData = imageVector[i]->_imageData->getOriginalData();
        uchar* originalData = static_cast<uchar*>(_imageData->getOriginalData());
        memcpy(originalData + currentSlice * elementSize * _width * _height, singleData, elementSize * _width * _height * singleSlice);
        currentSlice += singleSlice;
    }

    _currentAxialSlice = round(_slice / 2.0) - 1;
    _currentCoronalSlice = round(_height / 2.0) - 1;
    _currentSagittalSlice = round(_width / 2.0) - 1;

    // Allocate memory
    if (allocateMemory() == false)
    {
        _openSucceed = false;
        return;
    }

    // Find top and bottom value in data
    if (_imageData->findTopAndBottom() == false)
    {
        _openSucceed = false;
        return;
    }

    rescaleArray();

    rescaleTopAndBottom();

    initWindowWidthAndLevel();

    // Convert float data to uchar data
    if (convertToByte() == false)
    {
        _openSucceed = false;
        return;
    }
    // Copy to image
    if (copyByteToImage() == false)
    {
        _openSucceed = false;
        return;
    }

    _openSucceed = true;
}

DICOMImage::DICOMImage(const DICOMImage& src)
    : MonoImage(src)
    , _horzPixelSpacing(src._horzPixelSpacing)
    , _vertPixelSpacing(src._vertPixelSpacing)
    , _imagerPixelSpacing(src._imagerPixelSpacing)
    , _SOD(src._SOD)
    , _SDD(src._SDD)
    , _bitsAllocated(src._bitsAllocated)
    , _bitsStored(src._bitsStored)
    , _rescaleSlope(src._rescaleSlope)
    , _rescaleIntercept(src._rescaleIntercept)
{

}

DICOMImage& DICOMImage::operator=(const DICOMImage& src)
{
    if (this == &src)
        return *this;

    MonoImage::operator=(src);

    _horzPixelSpacing = src._horzPixelSpacing;
    _vertPixelSpacing = src._vertPixelSpacing;
    _imagerPixelSpacing = src._imagerPixelSpacing;

    _SOD = src._SOD;
    _SDD = src._SDD;

    _bitsAllocated = src._bitsAllocated;
    _bitsStored = src._bitsStored;

    _rescaleSlope = src._rescaleSlope;
    _rescaleIntercept = src._rescaleIntercept;

    return *this;
}

void DICOMImage::setSlice(int slice)
{
    MonoImage::setSlice(slice);

    rescaleArray();
}

void DICOMImage::setSlice(int type, int slice)
{
    MonoImage::setSlice(type, slice);

    rescaleArray(type);
}

void DICOMImage::initWindowWidthAndLevel()
{
    if (_windowWidth == 0 && _windowLevel == 0)
    {
        _windowWidth = _imageData->getMaximumValue() - _imageData->getMinimumValue();
        _windowLevel = (_imageData->getMaximumValue() + _imageData->getMinimumValue()) / 2.0f;
    }
}

BaseImage* DICOMImage::copyImage() const
{
    return new DICOMImage(*this);
}

bool DICOMImage::copyToImage(BaseImage* image) const
{
    DICOMImage* destImage = dynamic_cast<DICOMImage*>(image);
    if (!destImage)
        return false;

    *destImage = *this;

    return true;
}

void DICOMImage::restore()
{
    _imageData->restoreData();

    _imageData->findTopAndBottom();

    rescaleArray();

    rescaleTopAndBottom();

    convertToByte();

    copyByteToImage();
}

// Read data
bool DICOMImage::readData()
{
    DJDecoderRegistration::registerCodecs();

    DcmFileFormat fileformat;
    OFCondition condition = fileformat.loadFile(_pathName.toStdString().c_str());
    if (!condition.good())
    {
        qDebug() << "Load Dimcom File Error: " << condition.text();
        return false;
    }

    DcmDataset* dataset = fileformat.getDataset();
    E_TransferSyntax xfer = dataset->getOriginalXfer();
    if (xfer == EXS_JPEG2000)
    {
        qDebug() << "Do not support JPEG2000 transfer syntax";
        return false;
    }
    DicomImage* pDicomImg = new DicomImage(dataset, xfer);
    if (pDicomImg->getStatus() != EIS_Normal)
    {
        qDebug() << pDicomImg->getStatus();
        delete pDicomImg;
        return false;
    }

    _width = pDicomImg->getWidth();
    _height = pDicomImg->getHeight();
    _slice = pDicomImg->getNumberOfFrames();

    int depth = pDicomImg->getDepth();
    if (depth <= 8)
    {
        _imageData = new ImageDataTemplate<uchar>(_width, _height, _slice);
        const uchar* pixelData = static_cast<const uchar*>(pDicomImg->getOutputData(depth));
        uchar* originalData = static_cast<uchar*>(_imageData->getOriginalData());
        memcpy(originalData, pixelData, sizeof(uchar) * _width * _height * _slice);
    }
    else if (depth <= 16)
    {
        _imageData = new ImageDataTemplate<ushort>(_width, _height, _slice);
        const ushort* pixelData = static_cast<const ushort*>(pDicomImg->getOutputData(depth));
        ushort* originalData = static_cast<ushort*>(_imageData->getOriginalData());
        memcpy(originalData, pixelData, sizeof(ushort) * _width * _height * _slice);
    }
    else if (depth <= 32)
    {
        _imageData = new ImageDataTemplate<uint>(_width, _height, _slice);
        const uint* pixelData = static_cast<const uint*>(pDicomImg->getOutputData(depth));
        uint* originalData = static_cast<uint*>(_imageData->getOriginalData());
        memcpy(originalData, pixelData, sizeof(uint) * _width * _height * _slice);
    }

    readMoreInfo(dataset);

    delete pDicomImg;

    DJDecoderRegistration::cleanup();

    return true;
}

void DICOMImage::readMoreInfo(DcmDataset* dataset)
{
    const char* tagValue = nullptr;
    OFCondition condition = dataset->findAndGetString(DCM_PixelSpacing, tagValue);
    if (condition.good() && tagValue)
    {
        _horzPixelSpacing = _vertPixelSpacing = atof(tagValue);
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_DistanceSourceToPatient, tagValue);
    if (condition.good() && tagValue)
    {
        _SOD = float(atof(tagValue));
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_DistanceSourceToDetector, tagValue);
    if (condition.good() && tagValue)
    {
        _SDD = float(atof(tagValue));
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_DetectorElementSize, tagValue);
    if (condition.good() && tagValue)
    {
        // m_DataHeader.MEUnitWidth = float(atof(tagValue));
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_WindowCenter, tagValue);
    if (tagValue)
    {
        _windowLevel = float(atof(tagValue));
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_WindowWidth, tagValue);
    if (condition.good() && tagValue)
    {
        _windowWidth = float(atof(tagValue));
    }

    dataset->findAndGetUint16(DCM_BitsAllocated, _bitsAllocated);

    dataset->findAndGetUint16(DCM_BitsStored, _bitsStored);

    tagValue = nullptr;
    dataset->findAndGetString(DCM_RescaleSlope, tagValue);
    if (condition.good() && tagValue)
    {
        _rescaleSlope = float(atof(tagValue));
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_RescaleIntercept, tagValue);
    if (condition.good() && tagValue)
    {
        _rescaleIntercept = float(atof(tagValue));
    }
}

void DICOMImage::rescaleArray()
{
    if (_rescaleSlope != 0)
    {
        _imageData->rescaleArray(_rescaleSlope * 0.5f, _rescaleIntercept);
    }
}

void DICOMImage::rescaleArray(int type)
{
    if (_rescaleSlope != 0)
    {
        _imageData->rescaleArray(type, _rescaleSlope * 0.5f, _rescaleIntercept);
    }
}

void DICOMImage::rescaleTopAndBottom()
{
    if (_rescaleSlope != 0)
    {
        _imageData->rescaleTopAndBottom(_rescaleSlope * 0.5f, _rescaleIntercept);
    }
}