#pragma once

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

class BaseImage;
class GeneralImage;
class MonoImage;
class RegionImage;

// Base class of image processing algorithm
class BaseProcessor
{
public:
	BaseProcessor();
	~BaseProcessor();

public:
	static BaseProcessor* getProcessor();

	BaseProcessor* setCurrentProcessor();

	// Process image
	void process(BaseImage* image);

	// Process float array
	virtual void processArray(float* pArray, int width, int height, float minValue, float maxValue, uchar* pByte)	{}

protected:
	virtual void processGeneralImage(GeneralImage* image) {}

	virtual void processMonoImage(MonoImage* image) {}

//	virtual void ProcessRegionImage(RegionImage* pImage);

	// Convert float data to uchar data
	void convertToByte(float* pArray, int width, int height, float minValue, float maxValue, uchar* pByte);

private:
	static BaseProcessor* _currentProcessor;
};