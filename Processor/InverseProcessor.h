#pragma once

#include "BaseProcessor.h"

class InverseProcessor : public BaseProcessor
{
public:
	InverseProcessor();
	~InverseProcessor();

protected:
	void processGeneralImage(GeneralImage* image) override;

	void processMonoImage(MonoImage* image) override;

	// Process float array
	virtual void processArray(float* array, int width, int height, uchar* pByte);
};