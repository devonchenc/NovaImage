#pragma once

#include "../Image/BaseProcessor.h"

class CommonProcessor : public BaseProcessor
{
//	DECLARE_TEMPLATE_INTERFACE

public:
	CommonProcessor();
	~CommonProcessor();

protected:
	virtual void processGeneralImage(GeneralImage* pImage);

//	template<typename Type>
//	void processTemplate(ImageDataTemplate<Type>* pImage);

	// Process float array
	virtual void processArray(float* pArray, int width, int height, float minValue, float maxValue, uchar* pByte);

public:
	void setBrightnessAndContrast(int brightness, int contrast);

private:
	int _brightness;
	int _contrast;
};