#pragma once

#include "../Image/BaseProcessor.h"

class HistogramProcessor : public BaseProcessor
{
//	DECLARE_TEMPLATE_INTERFACE

public:
	HistogramProcessor();
	~HistogramProcessor();

protected:
	virtual void processGeneralImage(GeneralImage* image);

//	template<typename Type>
//	void processTemplate(ImageDataTemplate<Type>* image);

	// Process float array
	virtual void processArray(float* array, int width, int height, float minValue, float maxValue, uchar* pByte);

public:
	void setWindowArray(bool* array, int arrayNum);

private:
	bool* _array;
	int _arrayNum;
};