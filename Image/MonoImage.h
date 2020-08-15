#pragma once

#include "BaseImage.h"

class ImageData;
class MonoImageProxy;

#ifndef VIEW_TYPE
    #define TOP_VIEW            0
    #define FRONTAL_VIEW        1
    #define PROFILE_VIEW        2
#endif

class MonoImage : public BaseImage
{
public:
    MonoImage();
    MonoImage(const QString& pathName);
    MonoImage(const MonoImage& src);
    virtual ~MonoImage();

    bool copyToAllImage();

    bool copyToImage();

public:
    int slice() const override { return _slice; }

    void setSlice(int slice) override;

    int currentSlice() const override;

    int currentSlice(int type) const override;

    float getMinValue() const override;

    float getMaxValue() const override;

    // Histogram statistic
    void histogramStatistic() override;

    float getValue(const QPoint& position) const override;
    float getValue(int x, int y) const override;
    float getValue(int index) const override;

    bool hasPixelSpacing() override { return false; }

    float horzPixelSpacing() override { return 0; }
    float vertPixelSpacing() override { return 0; }

    virtual void initWindowWidthAndLevel() = 0;

    void restore() override;

    void setType(int type) override;

public:
    // get uchar data
    uchar* getBYTEImage(int& width, int& height);

    bool convertAllToByte();

    bool convertToByte();

    std::shared_ptr<QImage> getFrontalSlice() const;
    std::shared_ptr<QImage> getProfileSlice() const;

protected:
    bool saveAsRaw(const QString& fileName) override;

protected:
    bool allocateMemory();

protected:
    ImageData* _imageData;

    MonoImageProxy* _topProxy;
    MonoImageProxy* _frontalProxy;
    MonoImageProxy* _profileProxy;

    int _currentType;

    int _slice;

    int _currentTopSlice;
    int _currentFrontalSlice;
    int _currentProfileSlice;
};
