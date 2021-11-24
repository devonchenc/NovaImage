#pragma once

#include "MonoImage.h"

class DcmDataset;

class DICOMImage : public MonoImage
{
public:
    DICOMImage(const QString& pathName);
    DICOMImage(QVector<std::shared_ptr<DICOMImage>>& imageVector);
    DICOMImage(const DICOMImage& src);
    virtual ~DICOMImage() {}

    DICOMImage& operator=(const DICOMImage& src);

public:
    void setSlice(int slice) override;
    void setSlice(int type, int slice) override;

    bool hasPixelSpacing() override { return true; }

    float horzPixelSpacing() override { return _horzPixelSpacing; }
    float vertPixelSpacing() override { return _vertPixelSpacing; }
    float sliceSpacing() override { return _sliceSpacing; }

    void initWindowWidthAndLevel() override;

    BaseImage* copyImage() const override;

    bool copyToImage(BaseImage* image) const override;

    void restore() override;

    QString getDescription() { return _seriesDescription; }

private:
    // Read data
    bool readData();

    void readMoreInfo(DcmDataset* dataset);

    void rescaleArray();
    void rescaleArray(int type);

    void rescaleTopAndBottom();

private:
    float _horzPixelSpacing;
    float _vertPixelSpacing;
    float _sliceSpacing;

    float _imagerPixelSpacing;      // Detector pixel size
    float _sliceThickness;

    float _SOD;
    float _SDD;

    float _imagePositionPatientZ;

    ushort _bitsAllocated;
    ushort _bitsStored;

    float _rescaleSlope;
    float _rescaleIntercept;

    QString _seriesDescription;
    QString _patientsName;

    QString _studyInstanceUID;
    QString _seriesInstanceUID;
    QString _SOPInstanceUID;
};
