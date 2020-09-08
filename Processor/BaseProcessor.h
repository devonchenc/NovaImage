#pragma once

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

class BaseImage;
class GeneralImage;
class MonoImage;
class RegionImage;

#include <QtGlobal>
#include <QObject>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

// Base class of image processing algorithm
class BaseProcessor : public QObject
{
    Q_OBJECT

public:
    BaseProcessor();
    virtual ~BaseProcessor();

public:
    static BaseProcessor* getProcessor();

    BaseProcessor* setCurrentProcessor();

    virtual void initUI() {}

    void setImage(BaseImage* image) { _image = image; }

    // Process image
    void process();

    // Process float array
    virtual void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
    { Q_UNUSED(array);Q_UNUSED(width);Q_UNUSED(height);Q_UNUSED(minValue);Q_UNUSED(maxValue);Q_UNUSED(pByte); }

protected:
    virtual void processGeneralImage(GeneralImage* image) { Q_UNUSED(image); }

    virtual void processMonoImage(MonoImage* image) { Q_UNUSED(image); }

    //	virtual void ProcessRegionImage(RegionImage* pImage);

    // Convert float data to uchar data
    void convertToByte(float* array, int width, int height, float minValue, float maxValue, uchar* pByte);

signals:
    void createWidget(QWidget* widget);

protected:
    BaseImage* _image;

private:
    static BaseProcessor* _currentProcessor;
};
