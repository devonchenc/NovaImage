#pragma once

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#include <QtGlobal>
#include <QObject>

class BaseImage;
class GeneralImage;
class MonoImage;
class RegionImage;
class ProcessorBaseWidget;

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

// Base class of image processing algorithm
class BaseProcessor : public QObject
{
    Q_OBJECT

public:
    BaseProcessor(bool temporary, QObject* parent);
    virtual ~BaseProcessor();

public:
    static BaseProcessor* currentProcessor();

    BaseProcessor* setCurrentProcessor();

    virtual void initUI() {}

    void apply();

    void setImage(BaseImage* image);

    bool isTemporary() { return _temporary; }

    // Process image
    void process();

    void processForView(BaseImage* image);

protected:
    virtual void processImage(GeneralImage*, GeneralImage*) {}

    virtual void processImage(MonoImage*, MonoImage*) {}

    //	virtual void ProcessRegionImage(RegionImage* pImage);

    // Convert float data to uchar data
    void convertToByte(float* array, int width, int height, float minValue, float maxValue, uchar* pByte);

signals:
    void createWidget(QWidget* widget);

protected:
    BaseImage* _srcImage;
    BaseImage* _dstImage;

    ProcessorBaseWidget* _processorWidget;

private:
    static BaseProcessor* _currentProcessor;

    bool _temporary;
};
