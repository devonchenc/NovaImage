#pragma once

#include "MonoImage.h"
#include "Dataheader.h"
#include "OldDataheader.h"

class QFile;

class ScanImage : public MonoImage
{
public:
    ScanImage(const QString& pathName);
    ScanImage(const ScanImage& src);
    virtual ~ScanImage();

    ScanImage& operator=(const ScanImage& src);

public:
    void setSlice(int slice) override;

    bool hasPixelSpacing() override { return true; }

    float horzPixelSpacing() override { return _dataHeader.HorzPixelSpacing; }
    float vertPixelSpacing() override { return _dataHeader.VertPixelSpacing; }

    void initWindowWidthAndLevel() override;

    BaseImage* copyImage() const override;

    bool copyToImage(BaseImage* image) const override;

public:
    // Get reference of DataHeader
    DataHeader& getDataHeader()		{ return _dataHeader; }

protected:
    bool saveAsDcm(const QString& fileName) override;

private:
    // Read data header
    bool readDataHeader();

    // Read data
    bool readData();

    bool isNewHeader(QFile& file);

    void convertHeader(const OldDataHeader& dh);

private:
    DataHeader _dataHeader;
};
