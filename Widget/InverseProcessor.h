#pragma once

#include "../Image/BaseProcessor.h"

class InverseProcessor : public BaseProcessor
{
public:
	InverseProcessor();
	~InverseProcessor();

protected:
	void processGeneralImage(GeneralImage* image) override;

	void processMonoImage(MonoImage* image) override;

	// Process float array
	virtual void processArray(float* pArray, int width, int height, uchar* pByte);
};