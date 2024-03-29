﻿#include "DicomImage.h"

#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>

#include <QDebug>
#include "ImageDataTemplate.h"

DICOMImage::DICOMImage(const QString& pathName)
    : MonoImage(pathName)
    , _horzPixelSpacing(0)
    , _vertPixelSpacing(0)
    , _sliceSpacing(0)
    , _imagerPixelSpacing(0)
    , _sliceThickness(0)
    , _SOD(0)
    , _SDD(0)
    , _imagePositionPatientZ(0)
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
    std::shared_ptr<DICOMImage> firstImage = imageVector.first();
    _width = firstImage->width();
    _height = firstImage->height();
    int elementSize = firstImage->_imageData->getElementSize();

    _windowWidth = firstImage->_windowWidth;
    _windowLevel = firstImage->_windowLevel;

    _horzPixelSpacing = firstImage->_horzPixelSpacing;
    _vertPixelSpacing = firstImage->_vertPixelSpacing;
    _sliceSpacing = firstImage->_sliceSpacing;
    _imagerPixelSpacing = firstImage->_imagerPixelSpacing;
    _sliceThickness = firstImage->_sliceThickness;
    _SOD = firstImage->_SOD;
    _SDD = firstImage->_SDD;
    _imagePositionPatientZ = firstImage->_imagePositionPatientZ;
    _bitsAllocated = firstImage->_bitsAllocated;
    _bitsStored = firstImage->_bitsStored;
    _rescaleSlope = firstImage->_rescaleSlope;
    _rescaleIntercept = firstImage->_rescaleIntercept;

    _seriesDescription = firstImage->_seriesDescription;
    _patientsName = firstImage->_seriesDescription;

    _studyInstanceUID = firstImage->_studyInstanceUID;
    _seriesInstanceUID = firstImage->_seriesInstanceUID;
    _SOPInstanceUID = firstImage->_SOPInstanceUID;

    if (_sliceSpacing == 0 && imageVector.size() > 1)
    {
        // Use _imagePositionPatientZ to calculate _sliceSpacing
        _sliceSpacing = fabs(_imagePositionPatientZ - imageVector[1]->_imagePositionPatientZ);
    }

    _pathName = firstImage->_pathName;

    // Count slice
    int slicesBefore = 0;
    for (int i = imageVector.size() - 1; i >= 0; i--)
    {
        if (imageVector[i]->width() != _width || imageVector[i]->height() != _height
            || imageVector[i]->slice() != 1 || imageVector[i]->_imageData->getElementSize() != elementSize)
        {
            imageVector.remove(i);
            continue;
        }

        slicesBefore++;
    }

    // Recalculate the slice number based on the spacing
    if (slicesBefore == 1)
    {
        _slice = 1;
    }
    else
    {
        _slice = ceil(_sliceSpacing / _horzPixelSpacing * (slicesBefore - 1));
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

    // Copy data from first image
    void* firstSliceData = imageVector.first()->_imageData->getOriginalData();
    uchar* originalData = static_cast<uchar*>(_imageData->getOriginalData());
    memcpy(originalData, firstSliceData, elementSize * _width * _height);
    // Interpolate image data
    if (_slice > 1)
    {
        void* lastSliceData = imageVector.last()->_imageData->getOriginalData();
        uchar* originalData = static_cast<uchar*>(_imageData->getOriginalData());
        memcpy(originalData + (_slice - 1) * elementSize * _width * _height, lastSliceData, elementSize * _width * _height);

        float factor = _horzPixelSpacing / _sliceSpacing;
        for (int n = 1; n < _slice - 1; n++)
        {
            float index = n * factor;
            int integerIndex = floor(index);
            float ratio = index - integerIndex;
            _imageData->interpolateData(n, imageVector[integerIndex]->_imageData->getOriginalData(), imageVector[integerIndex + 1]->_imageData->getOriginalData(), ratio);
        }
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
    , _sliceSpacing(src._sliceSpacing)
    , _imagerPixelSpacing(src._imagerPixelSpacing)
    , _sliceThickness(src._sliceThickness)
    , _SOD(src._SOD)
    , _SDD(src._SDD)
    , _imagePositionPatientZ(src._imagePositionPatientZ)
    , _bitsAllocated(src._bitsAllocated)
    , _bitsStored(src._bitsStored)
    , _rescaleSlope(src._rescaleSlope)
    , _rescaleIntercept(src._rescaleIntercept)
    , _seriesDescription(src._seriesDescription)
    , _patientsName(src._patientsName)
    , _studyInstanceUID(src._studyInstanceUID)
    , _seriesInstanceUID(src._seriesInstanceUID)
    , _SOPInstanceUID(src._SOPInstanceUID)
{

}

DICOMImage& DICOMImage::operator=(const DICOMImage& src)
{
    if (this == &src)
        return *this;

    MonoImage::operator=(src);

    _horzPixelSpacing = src._horzPixelSpacing;
    _vertPixelSpacing = src._vertPixelSpacing;
    _sliceSpacing = src._sliceSpacing;
    _imagerPixelSpacing = src._imagerPixelSpacing;
    _sliceThickness = src._sliceThickness;

    _SOD = src._SOD;
    _SDD = src._SDD;

    _imagePositionPatientZ = src._imagePositionPatientZ;
    _bitsAllocated = src._bitsAllocated;
    _bitsStored = src._bitsStored;

    _rescaleSlope = src._rescaleSlope;
    _rescaleIntercept = src._rescaleIntercept;

    _seriesDescription = src._seriesDescription;
    _patientsName = src._seriesDescription;

    _studyInstanceUID = src._studyInstanceUID;
    _seriesInstanceUID = src._seriesInstanceUID;
    _SOPInstanceUID = src._SOPInstanceUID;

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
    dataset->findAndGetString(DCM_SliceThickness, tagValue);
    if (condition.good() && tagValue)
    {
        _sliceThickness = float(atof(tagValue));
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

    tagValue = nullptr;
    dataset->findAndGetString(DCM_SpacingBetweenSlices, tagValue);
    if (condition.good() && tagValue)
    {
        _sliceSpacing = float(atof(tagValue));
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_ImagePositionPatient, tagValue);
    if (condition.good() && tagValue)
    {
        char* pos = strrchr((char*)tagValue, '\\');
        if (pos)
        {
            char* subValue = pos + 1;
            _imagePositionPatientZ = float(atof(subValue));
        }
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_SeriesDescription, tagValue);
    if (condition.good() && tagValue)
    {
        _seriesDescription = tagValue;
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_PatientName, tagValue);
    if (condition.good() && tagValue)
    {
        _patientsName = tagValue;
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_StudyInstanceUID, tagValue);
    if (condition.good() && tagValue)
    {
        _studyInstanceUID = tagValue;
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_SeriesInstanceUID, tagValue);
    if (condition.good() && tagValue)
    {
        _seriesInstanceUID = tagValue;
    }

    tagValue = nullptr;
    dataset->findAndGetString(DCM_SOPInstanceUID, tagValue);
    if (condition.good() && tagValue)
    {
        _SOPInstanceUID = tagValue;
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