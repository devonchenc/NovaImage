#pragma once

#ifndef DATA_HEADER_SIZE
#define DATA_HEADER_SIZE		4096
#endif

#define PANEL_NORMAL_IMAGE		11
#define PANEL_NOISE_IMAGE		12
#define	PANEL_DR_SCAN_IMAGE		13
#define PANEL_CT_SCAN_IMAGE		14
#define PANEL_RT_SCAN_IMAGE		15
#define PANEL_RECON_IMAGE		19

#include <memory>

class DataHeader
{
public:
    DataHeader()
    {
        memset(this, 0, sizeof(DataHeader));
        Tag = 0xCDCD0000;
    }

public:
    int		Tag;
    int		Attribute;
    int		Width;
    int		Height;
    int		Slice;
    int		DataType;				// DataType 0:float, 1:unsigned short, 2:unsigned char

    float	WindowWidth;
    float	WindowLevel;

    float	HorzPixelSpacing;
    float	VertPixelSpacing;

    int		Reserved0;
    int		Reserved1;

    // X-ray Source
    float	Voltage;
    float	Current;
    float	Frequency;
    float	FocusSize;
    float	TargetPower;
    float	TubePower;

    // Detector
    int		DetectorType;
    float	ElementSize;
    int		DetectorBitDepth;
    int		FrameBinning;
    int		Pixelbinning;
    float	IntegrationTime;
    float	Reserved2;
    float	Reserved3;

    // Scan
    int		ScanMode;
    float	SOD;
    float	SDD;

    int		RotateCount;
    int		CurrentIndex;
    int		RotateDirection;
    float	RotateAngel;
    float	TROffset;

    float	StartHeight;
    float	EndHeight;
    float	SliceThickness;
    float	LineDetectorOffset;
    float	PanelDetectorOffset;
    float	CCDDetectorOffset;
    float	SourceOffset;

    float	Pitch;					// Pitch of Spiral scan
    int		RotaionNum;				// Number of turns of Spiral scan
    int		LimitedScanAng;			// Angle of limited angle scan
    float	ShortScanDistance;		// Detector offset of LFOV scan

    float	Reserved4;
    float	Reserved5;

    // Reconstruct
    int		Algorithm;				// 0:None, 1:FDK, 2:Iteration
    float	CenterOffset1;
    float	VertAngAlign;
    float	CenterOffset2;
    int		Calibrated;
    float	OffsetV;
    float	OffsetW;
    float	M;
    float	N;
    int		RandomMove;				// 0:None, 1:Detector, 2:Turntable
    float	TurntableRandomMoveFactor;	// Turntable random move coefficient
    float	DetectorRandomMoveFactor;
    int		OutOfView;
    float	Gamma;

    // Image cropping parameters
    int		CutOrNot;
    int		CutBeginX;
    int		CutBeginY;
    int		CutEndX;
    int		CutEndY;
    int		OriginWidth;
    int		OriginHeight;
    float	Reserved16;
    float	Reserved17;
    float	Reserved18;

    // Object Information
    char	Type[40];				// 型号
    char	PartNum[40];			// 零件图号
    char	SerialNum[40];			// 零件编号
    char	LocationNum[40];		// 部位序号
};
