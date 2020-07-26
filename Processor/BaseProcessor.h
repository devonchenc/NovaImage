#pragma once

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

class BaseImage;
class GeneralImage;
class MonoImage;
class RegionImage;

#include <QtGlobal>

// Base class of image processing algorithm
class BaseProcessor
{
public:
    BaseProcessor();
    virtual ~BaseProcessor();

public:
    static BaseProcessor* getProcessor();

    BaseProcessor* setCurrentProcessor();

    // Process image
    void process(BaseImage* image);

    // Process float array
    virtual void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte)
    {Q_UNUSED(array);Q_UNUSED(width);Q_UNUSED(height);Q_UNUSED(minValue);Q_UNUSED(maxValue);Q_UNUSED(pByte);}

protected:
    virtual void processGeneralImage(GeneralImage* image) { Q_UNUSED(image); }

    virtual void processMonoImage(MonoImage* image) { Q_UNUSED(image); }

    //	virtual void ProcessRegionImage(RegionImage* pImage);

    // Convert float data to uchar data
    void convertToByte(float* array, int width, int height, float minValue, float maxValue, uchar* pByte);

private:
    static BaseProcessor* _currentProcessor;
};
