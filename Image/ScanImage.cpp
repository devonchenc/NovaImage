#include "ScanImage.h"

#include <QFile>

#include "ImageDataTemplate.h"
#include "ImageReader.h"
#include "../Widget/ProgressDialog.h"

ScanImage::ScanImage(const QString& pathName)
    : MonoImage(pathName)
{
    // Read data header
    if (readDataHeader() == false)
    {
        _openSucceed = false;
        return;
    }
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

    initWindowWidthAndLevel();

    // Convert float data to uchar data
    if (convertAllToByte() == false)
    {
        _openSucceed = false;
        return;
    }
    // Copy to image
    if (copyByteToAllImage() == false)
    {
        _openSucceed = false;
        return;
    }

    _openSucceed = true;
}

ScanImage::ScanImage(const ScanImage& src)
    : MonoImage(src)
    , _dataHeader(src._dataHeader)
{

}

ScanImage::~ScanImage()
{

}

void ScanImage::setSlice(int slice)
{
    MonoImage::setSlice(slice);

    convertToByte();

    copyByteToImage();
}

bool ScanImage::saveAsDcm(const QString& fileName)
{
    Q_UNUSED(fileName);
    return true;
}

void ScanImage::initWindowWidthAndLevel()
{
    if (_dataHeader.WindowWidth == 0 && _dataHeader.WindowLevel == 0)
    {
        _windowWidth = _imageData->getMaximumValue() - _imageData->getMinimumValue();
        _windowLevel = (_imageData->getMaximumValue() + _imageData->getMinimumValue()) / 2.0f;
    }
    else
    {
        _windowWidth = _dataHeader.WindowWidth;
        _windowLevel = _dataHeader.WindowLevel;
    }
}

BaseImage* ScanImage::copyImage() const
{
    return new ScanImage(*this);
}

bool ScanImage::copyToImage(BaseImage* image) const
{
    ScanImage* destImage = dynamic_cast<ScanImage*>(image);
    if (!destImage)
        return false;

    return true;
}

// Read data header
bool ScanImage::readDataHeader()
{
    QFile file(_pathName);
    if (!file.open(QFile::ReadOnly))
        return false;

    bool isNew = isNewHeader(file);
    if (isNew)
    {
        // New version
        file.read((char*)(&_dataHeader), sizeof(DataHeader));
        file.close();
    }
    else
    {
        // Old version
        OldDataHeader dh;
        file.read((char*)(&dh), sizeof(OldDataHeader));
        file.close();

        // Convert old header to new header
        convertHeader(dh);
    }

    _width = _dataHeader.Width;
    _height = _dataHeader.Height;
    _slice = _dataHeader.Slice == 0 ? 1 : _dataHeader.Slice;
    _currentAxialSlice = round(_slice / 2.0) - 1;
    _currentCoronalSlice = round(_height / 2.0) - 1;
    _currentSagittalSlice = round(_width / 2.0) - 1;
    if (_width * _height <= 0)
        return false;

    qint64 expectSize = _width * _height * _slice * sizeof(float);
    if (isNew)
    {
        expectSize += DATA_HEADER_SIZE;
    }
    else
    {
        expectSize += OLD_DATA_HEADER_SIZE;
    }
    if (expectSize > file.size())
    {
        QMessageBox::critical(nullptr, QObject::tr("Open image file error"),
                              QObject::tr("The data size does not match the file information description!"), QMessageBox::Ok);
        return false;
    }

    return true;
}

// Read data
bool ScanImage::readData()
{
    QFile file(_pathName);
    if (!file.open(QFile::ReadOnly))
        return false;

    bool isNew = isNewHeader(file);
    file.close();

    int headerSize = 0;
    if (isNew)
    {
        headerSize = DATA_HEADER_SIZE;
    }
    else
    {
        headerSize = OLD_DATA_HEADER_SIZE;
    }

    AbstractReader* reader = nullptr;
    if (_dataHeader.DataType == 0)
    {
        _imageData = new ImageDataTemplate<float>(_width, _height, _slice);
        float* originalData = static_cast<float*>(_imageData->getOriginalData());
        reader = new ImageReader<float>(_pathName, headerSize, _width * _height, _slice, originalData);
    }
    else if (_dataHeader.DataType == 1)
    {
        _imageData = new ImageDataTemplate<ushort>(_width, _height, _slice);
        ushort* originalData = static_cast<ushort*>(_imageData->getOriginalData());
        reader = new ImageReader<ushort>(_pathName, headerSize, _width * _height, _slice, originalData);
    }
    else if (_dataHeader.DataType == 2)
    {
        _imageData = new ImageDataTemplate<uchar>(_width, _height, _slice);
        uchar* originalData = static_cast<uchar*>(_imageData->getOriginalData());
        reader = new ImageReader<uchar>(_pathName, headerSize, _width * _height, _slice, originalData);
    }
    ProgressDialog dlg(reader);
    reader->setWidget(&dlg);
    reader->start();
    reader->deleteLater();

    if (dlg.exec() == QDialog::Accepted)
    {
        // Get min and max value in data
        _imageData->setMinimumValue(reader->minValue());
        _imageData->setMaximumValue(reader->maxValue());

        return true;
    }
    else
    {
        return false;
    }

    return true;
}

bool ScanImage::isNewHeader(QFile& file)
{
    int tag;
    file.seek(0);
    file.read((char*)(&tag), sizeof(int));
    file.seek(0);
    return tag == int(0xCDCD0000);
}

void ScanImage::convertHeader(const OldDataHeader& dh)
{
    _dataHeader.Attribute = dh.Attribute;
    _dataHeader.Width = dh.Width;
    _dataHeader.Height = dh.Height;
    _dataHeader.Slice = dh.Slice;

    _dataHeader.DataType = dh.DataType;
    _dataHeader.HorzPixelSpacing = dh.RowPixelWidth;
    _dataHeader.VertPixelSpacing = dh.ColumnPixelWidth;

    _dataHeader.Voltage = dh.Voltage;
    _dataHeader.Current = dh.Current;
    _dataHeader.Frequency = dh.Frequency;
    _dataHeader.FocusSize = dh.FocusSize;

    _dataHeader.DetectorType = dh.DetectorType;
    _dataHeader.ElementSize = dh.UnitWidth;
    _dataHeader.FrameBinning = dh.ImageFrameCount;
    _dataHeader.DetectorBitDepth = dh.ImageBitDepth;
    _dataHeader.Pixelbinning = dh.Pixelbinning;
    _dataHeader.IntegrationTime = dh.IntegrationTime;

    _dataHeader.SOD = dh.SOD;
    _dataHeader.SDD = dh.SDD;
    _dataHeader.IntegrationTime = dh.IntegrationTime;
    _dataHeader.IntegrationTime = dh.IntegrationTime;
    _dataHeader.IntegrationTime = dh.IntegrationTime;
    _dataHeader.IntegrationTime = dh.IntegrationTime;
}
