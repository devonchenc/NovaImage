#pragma once

#include "../Image/BaseProcessor.h"

class LevelsProcessor : public BaseProcessor
{
public:
	LevelsProcessor();
	~LevelsProcessor();

protected:
	void processGeneralImage(GeneralImage* image) override;

	void processMonoImage(MonoImage* image) override;

	// Process float array
	virtual void processArray(float* array, int width, int height, float fMinValue, float fMaxValue, uchar* pByte);

public:
	void setPara(float bottom, float mid, float top);

private:
	// Pointer to levels widget
	float _bottom, _mid, _top;
};