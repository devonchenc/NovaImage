#pragma once

typedef unsigned char uchar;
#define uiNR_OF_GREY (256)

int CLAHE(uchar* pImage, unsigned int uiXRes, unsigned int uiYRes, uchar Min,
    uchar Max, unsigned int uiNrX, unsigned int uiNrY,
    unsigned int uiNrBins, float fCliplimit);