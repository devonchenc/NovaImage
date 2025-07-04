﻿#pragma once

#include <memory>

#include "BaseImage.h"

class GeneralImage : public BaseImage
{
public:
    GeneralImage();
    GeneralImage(const QString& pathName);
    GeneralImage(const GeneralImage& src);

    GeneralImage& operator=(const GeneralImage& src);

public:
    // Get backup QImage pointer
    QImage* getBackupImage() const { return _backupImage.get(); }

    // Calculate new color
    uchar calcNewColor(uchar color, float bottom, float mid, float top, int minColor, int maxColor) const;

public:
    // Histogram statistic
    void histogramStatistic() override;

    float getValue(const QPoint& position) const override;
    float getValue(int x, int y) const override;
    float getValue(int index) const override;

    float getMinValue() const override { return 0.0f; }

    float getMaxValue() const override { return 255.0f; }

    BaseImage* copyImage() const override;

    bool copyToImage(BaseImage* image) const override;

    void restore() override;

private:
    // Backup origin QImage
    void backupImage();

private:
    // Backup origin QImage
    std::shared_ptr<QImage> _backupImage;
};
