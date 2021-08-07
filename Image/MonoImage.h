#pragma once

#include "BaseImage.h"

class ImageData;
class MonoImageProxy;

#ifndef VIEW_TYPE
#define AXIAL_VIEW              0
#define CORONAL_VIEW            1
#define SAGITTAL_VIEW           2
#define VOLUME_VIEW             3
#endif

class MonoImage : public BaseImage
{
public:
    MonoImage();
    MonoImage(const QString& pathName);
    MonoImage(const MonoImage& src);
    virtual ~MonoImage();

    MonoImage& operator=(const MonoImage& src);

    bool copyByteToAllImage();

    bool copyByteToImage();

    bool copyByteToImage(QImage* dstImage);

public:
    int slice() const override { return _slice; }

    void setSlice(int slice) override;
    void setSlice(int type, int slice) override;

    int currentSlice() const override;
    int currentSlice(int type) const override;

    float getMinValue() const override;

    float getMaxValue() const override;

    // Histogram statistic
    void histogramStatistic() override;

    float getValue(const QPoint& position) const override;
    float getValue(int x, int y) const override;
    float getValue(int index) const override;

    float getValueWithType(int type, int index) const override;

    bool hasPixelSpacing() override { return false; }

    float horzPixelSpacing() override { return 0; }
    float vertPixelSpacing() override { return 0; }

    virtual void initWindowWidthAndLevel() = 0;

    void restore() override;

    void setViewType(int type) override;

    int viewType() override { return _currentViewType; }

    QImage* getImageEntity() const override;

public:
    void setValue(int index, float value);

    // get uchar data
    uchar* getBYTEImage(int& width, int& height);

    bool convertAllToByte();

    bool convertToByte();

    std::shared_ptr<QImage> getCoronalImage() const;
    std::shared_ptr<QImage> getSagittalImage() const;

protected:
    bool saveAsRaw(const QString& fileName) override;

protected:
    bool allocateMemory();

protected:
    ImageData* _imageData;

    MonoImageProxy* _axialProxy;
    MonoImageProxy* _coronalProxy;
    MonoImageProxy* _sagittalProxy;

    int _currentViewType;

    int _slice;

    int _currentAxialSlice;
    int _currentCoronalSlice;
    int _currentSagittalSlice;
};
