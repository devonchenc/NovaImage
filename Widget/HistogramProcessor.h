#pragma once

#include "../Image/BaseProcessor.h"

class HistogramProcessor : public BaseProcessor
{
public:
	HistogramProcessor();
	~HistogramProcessor();

protected:
	void processGeneralImage(GeneralImage* image) override;

	void processMonoImage(MonoImage* image) override;

	// Process float array
	void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte) override;

public:
	void setWindowArray(bool* array, int arrayNum);

private:
	bool* _array;
	int _arrayNum;
};