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

    ScanImage& operator=(const ScanImage& src);

public:
    void setSlice(int slice) override;

    bool hasPixelSpacing() const override { return true; }

    float horzPixelSpacing() const override;
    float vertPixelSpacing() const override;
    float sliceSpacing() const override;

    void initWindowWidthAndLevel() override;

    BaseImage* copyImage() const override;

    bool copyToImage(BaseImage* image) const override;

public:
    // Get reference of DataHeader
    DataHeader& getDataHeader()     { return _dataHeader; }

protected:
    bool saveAsDcm(const QString& fileName) const override;

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
