#pragma once

#ifndef OLD_DATA_HEADER_SIZE
#define OLD_DATA_HEADER_SIZE	1024
#endif

#define PANEL_NORMAL_IMAGE		11
#define PANEL_NOISE_IMAGE		12
#define	PANEL_DR_SCAN_IMAGE		13
#define PANEL_CT_SCAN_IMAGE		14
#define PANEL_RT_SCAN_IMAGE		15
#define PANEL_RECON_IMAGE		19

#include <memory>

class OldDataHeader
{
public:
	OldDataHeader()
	{
		memset(this, 0, sizeof(OldDataHeader));
	}

public:
	uint Attribute;
	ushort Width;
	ushort Height;
	ushort Slice;
	ushort ImageFrameCount;
	ushort ImageBitDepth;
	ushort Pixelbinning;

	float WindowWidth;
	float WindowLevel;

	float SOD;
	float SDD;
	float Reserved2;
	float Reserved3;

	float	Voltage;
	float	Frequency;
	float	Current;
	float	IntegrationTime;
	float	FocusSize;
	int		DetectorType;

	ushort	RotateCount;
	ushort	CurrentIndex;
	int	RotateDirection;
	float	TROffset;
	float	RotateAngel;

	float	StartHeight;
	float	EndHeight;
	float	UnitWidth;
	float	LevelHeight;
	float	LineDetectorOffset;
	float	Reserved4;
	float	SourceOffset;

	ushort	VolumeXDis;
	ushort	VolumeYDis;
	ushort	VolumeZDis;
	ushort	DataType;				// DataType 0:float, 1:unsigned short, 2:unsigned char

	int     SpecialScanMode;
	float	Pitch;
	int		RotaionNum;
	int		LimitedScanAng;
	float	ShortScanDistance;

	float	Reserved8;
	float	Reserved9;
	float	Reserved10;
	float	Reserved11;

	float	RowPixelWidth;
	float	ColumnPixelWidth;
};
