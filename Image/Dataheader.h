#pragma once

#ifndef DATA_HEADER_SIZE
#define DATA_HEADER_SIZE	1024
#endif

#include <memory>

class DataHeader
{
public:
	DataHeader()
	{
		memset(this, 0, sizeof(DataHeader));
	}

public:
	uint Attribute;
	ushort Width;
	ushort Height;
	ushort Slice;
};
