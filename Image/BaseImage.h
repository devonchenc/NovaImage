#pragma once

#include <memory>
#include <optional>
#include <QString>
#include <QImage>

class BaseImage
{
public:
    BaseImage();
    BaseImage(const QString& pathName);
    BaseImage(const BaseImage& src);
    virtual ~BaseImage() {}

    BaseImage& operator=(const BaseImage& src);

public:
    // Get min and max value of image
    virtual float getMinValue() const = 0;
    virtual float getMaxValue() const = 0;

    // Histogram statistic
    virtual void histogramStatistic() = 0;

    virtual float getValue(const QPoint& position) const = 0;
    virtual float getValue(int x, int y) const = 0;
    virtual float getValue(int index) const = 0;
    virtual float getValue(float x, float y) const;

    virtual float getValueWithType(int type, int index) const { Q_UNUSED(type); return getValue(index); }

    virtual bool hasPixelSpacing() const { return false; }

    virtual float horzPixelSpacing() const { return 0; }
    virtual float vertPixelSpacing() const { return 0; }
    virtual float sliceSpacing() const { return 0; }

    // Create a deep copy of image
    virtual BaseImage* copyImage() const = 0;

    virtual bool copyToImage(BaseImage* image) const = 0;

    virtual void restore() = 0;

    virtual void setViewType(int type) { Q_UNUSED(type); }

    virtual int viewType() const { return 0; }

    virtual QImage* getImageEntity() const { return _pImage.get(); }

    virtual QString getDescription() const { return QString(); }

    virtual int slice() const { return 1; }

    virtual void setSlice(int slice) { Q_UNUSED(slice); }
    virtual void setSlice(int type, int slice) { Q_UNUSED(type); Q_UNUSED(slice); }

    virtual int currentSlice() const { return 0; }
    virtual int currentSlice(int type) const { Q_UNUSED(type); return 0; }

public:
    int width() const { return _width; }

    int height() const { return _height; }

    float windowWidth() const { return _windowWidth; }

    float windowLevel() const { return _windowLevel; }

    bool isOpenSucceed() const { return _openSucceed; }

    QString getPathName() const { return _pathName; }

    // Get each channel's pixel array
    const uint* getGrayPixelArray() const { return _grayPixelNumber; }
    const uint* getRedPixelArray() const { return _redPixelNumber; }
    const uint* getGreenPixelArray() const { return _greenPixelNumber; }
    const uint* getBluePixelArray() const { return _bluePixelNumber; }

    void setCalibrationSpacing(float spacing) { _calibrationSpacing = spacing; }

    QRgb getPixel(const QPoint& position) const;
    QRgb getPixel(int x, int y) const;

    bool save(const QString& fileName) const;

    void copyToArray(uchar* array) const;

    bool copyFromArray(const uchar* byteImage, int width, int height);

protected:
    virtual bool saveAsDcm(const QString& fileName) const;

    virtual bool saveAsRaw(const QString& fileName) const;

protected:
    std::shared_ptr<QImage> _pImage;

    int _width, _height;

    float _windowWidth;
    float _windowLevel;

    std::optional<float> _calibrationSpacing;

    QString _pathName;

    bool _openSucceed;

    // Storage the pixel number of each channel(0-255)
    uint _grayPixelNumber[256];
    uint _redPixelNumber[256];
    uint _greenPixelNumber[256];
    uint _bluePixelNumber[256];
};
