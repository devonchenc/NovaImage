#include "ImageData.h"

#include <iostream>
#include "../GlobalFunc.h"

using namespace std;

ImageData::ImageData()
	: _byteImage(nullptr)
{

}

ImageData::ImageData(unsigned long pixelCount)
	: _byteImage(nullptr)
	, _pixelCount(pixelCount)
{

}

ImageData::~ImageData()
{
	if (_byteImage)
	{
		delete[] _byteImage;
		_byteImage = nullptr;
	}
}

// Allocate memory
bool ImageData::allocateMemory()
{
	try
	{
		// 3 channels
		_byteImage = new uchar[_pixelCount * 3];
	}
	catch (const std::bad_alloc& e)
	{
		std::cout << "Allocation failed: " << e.what() << std::endl;
	}

	return true;
}