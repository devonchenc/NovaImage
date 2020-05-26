#pragma once

#include "../Image/BaseProcessor.h"

class LevelsProcessor : public BaseProcessor
{
//	DECLARE_TEMPLATE_INTERFACE

public:
	LevelsProcessor();
	~LevelsProcessor();

protected:
	virtual void processGeneralImage(GeneralImage* pImage);

//	template<typename Type>
//	void processTemplate(ImageDataTemplate<Type>* pImage);

	// Process float array
	virtual void processArray(float* pArray, int width, int height, float fMinValue, float fMaxValue, uchar* pByte);

public:
	void setPara(float bottom, float mid, float top);

private:
	// Pointer to levels widget
	float _bottom, _mid, _top;
};