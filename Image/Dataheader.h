#pragma once

#ifndef DATA_HEADER_SIZE
#define DATA_HEADER_SIZE        4096
#endif

#define PANEL_NORMAL_IMAGE      11
#define PANEL_NOISE_IMAGE       12
#define PANEL_DR_SCAN_IMAGE     13
#define PANEL_CT_SCAN_IMAGE     14
#define PANEL_RT_SCAN_IMAGE     15
#define PANEL_RECON_IMAGE       19

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
    int     Tag;
    int     Attribute;
    int     Width;
    int     Height;
    int     Slice;
    int     DataType;           // DataType 0:float, 1:unsigned short, 2:unsigned char

    float   WindowWidth;
    float   WindowLevel;

    float   HorzPixelSpacing;
    float   VertPixelSpacing;
    int     SliceSpacing;
    int     Reserved0;
    int     Reserved1;
    int     Reserved2;

    // X-ray Source
    float   Voltage;
    float   Current;
    float   Frequency;
    float   FocusSize;
    float   TargetPower;
    float   TubePower;
    int     Reserved3;
    int     Reserved4;

    // Detector
    int     DetectorType;
    float   ElementSize;
    int     DetectorBitDepth;
    int     FrameBinning;
    int     Pixelbinning;
    float   IntegrationTime;
    float   Temperature;
    float   Reserved5;
    float   Reserved6;

    // Scan
    int     ScanMode;
    float   SOD;
    float   SDD;

    int     RotateCount;
    int     CurrentIndex;
    int     RotateDirection;
    float   RotateAngle;
    float	RotateStartAngle;   // Start Angle

    float   StartHeight;
    float   EndHeight;
    float   SliceThickness;
    float   LineDetectorOffset;
    float   PanelDetectorOffset;
    float   CCDDetectorOffset;
    float   SourceOffset;
    float   TROffset;
    float   Reserved7;
    float   Reserved8;

    float   TiltAngle;          // CL扫描倾斜角度
    float   DetectorRadius;     // CL扫描探测器运动半径
    float   StageRadius;        // CL扫描样品台运动半径
    float   SourceRadius;       // CL扫描射线源运动半径
    float   PCBThickness;       // PCB厚度
    float   ObjectCenterHeight; // 物体中心高度
    float   HeightOnPCB;        // 物体在PCB板上高度
    float   HeightUnderPCB;     // 物体在PCB板下高度
    float   ObjectType;
    float   Reserved9;
    float   Reserved10;

    float   Pitch;              // Pitch of Spiral scan
    int     RotaionNumber;      // Number of turns of Spiral scan
    int     LimitedScanAngle;   // Angle of limited angle scan
    float   ShortScanDistance;  // Detector offset of LFOV scan
    float   Reserved11;
    float   Reserved12;

    // Reconstruct
    int     Algorithm;          // 0:None, 1:FDK, 2:Iteration
    int     InterationNum;
    float   CenterOffset1;
    float   VertAngAlign;
    float   CenterOffset2;
    int     Calibrated;
    float   OffsetV;
    float   OffsetW;
    float   M;
    float   N;
    int     RandomMove;             // 0:None, 1:Detector, 2:Turntable
    float   TurntableRandomMoveFactor;  // Turntable random move coefficient
    float   DetectorRandomMoveFactor;
    int     OutOfView;
    float   Gamma;
    int     FeedbackAfterRecon;     // 0:No, 1:Yes
    float   Reserved13;
    float   Reserved14;
    float   Reserved15;
    float   Reserved16;

    // Image cropping parameters
    int     CutOrNot;
    int     CutBeginX;
    int     CutBeginY;
    int     CutEndX;
    int     CutEndY;
    int     OriginWidth;
    int     OriginHeight;
    float   Reserved17;
    float   Reserved18;

    // Motor Position
    int		PositionNum;
    int		PositionUnit;
    char	MotionName1[20];
    float	Position1;
    char	MotionName2[20];
    float	Position2;
    char	MotionName3[20];
    float	Position3;
    char	MotionName4[20];
    float	Position4;
    char	MotionName5[20];
    float	Position5;
    char	MotionName6[20];
    float	Position6;
    char	MotionName7[20];
    float	Position7;
    char	MotionName8[20];
    float	Position8;
    char	MotionName9[20];
    float	Position9;
    char	MotionName10[20];
    float	Position10;
    char	MotionName11[20];
    float	Position11;
    char	MotionName12[20];
    float	Position12;
    char	MotionName13[20];
    float	Position13;
    char	MotionName14[20];
    float	Position14;
    char	MotionName15[20];
    float	Position15;
    char	MotionName16[20];
    float	Position16;

    char	UserName[40];       // 操作人姓名
    char	Date[40];           // 操作时间

    // Object Information
    char    ObjectTypeName[40];     // 型号
    char    PartNum[40];        // 零件图号
    char    SerialNum[40];      // 零件编号
    char    LocationNum[40];    // 部位序号
};
