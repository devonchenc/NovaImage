#pragma once

#include <QString>
#include <QPair>
#include <QVector>
#include <QObject>
#include <memory>

#include "UndoSystem.h"

class MainWindow;
class View;
class BaseImage;

enum ENUM_IMAGE_FORMAT
{
    IMAGE_FORMAT_UNKNOWN = 0,
    IMAGE_FORMAT_JPG = 1,
    IMAGE_FORMAT_PNG = 2,
    IMAGE_FORMAT_TIF = 3,
    IMAGE_FORMAT_BMP = 4,
    IMAGE_FORMAT_GIF = 5,
    IMAGE_FORMAT_NDR = 6,
    IMAGE_FORMAT_NCT = 7,
    IMAGE_FORMAT_RAW = 8,
    IMAGE_FORMAT_DAT = 9,
    IMAGE_FORMAT_DICOM = 10,
};

class Document : public QObject
{
    Q_OBJECT

public:
    Document(MainWindow* pWindow);
    ~Document() {}

public:
    bool openFile(const QString& fileName);

    bool openFolder(const QString& pathName);

    bool saveAs(const QString& fileName);

    void closeFile();

    BaseImage* getImage() const { return _image.get(); }

  //  void copyImage(const std::shared_ptr<BaseImage>& image);

    void repaintView();
    void repaintView(QImage* image, int viewType = 0);
    void repaintView(std::shared_ptr<QImage> dstImage, int viewType = 0);
    void repaintView(std::vector<std::shared_ptr<QImage>> imageVec);

    bool modified() { return _modified; }

    void setModified(bool flag);

    void saveGraphicsItems();

    void loadGraphicsItems();

    static int findImageType(const QString& fileName);

public:
    void defaultImageWindow();

    void fullImageWindow();

    void setViewWindowWidthAndLevel(float windowWidth, float windowLevel);

    void applyImageWidthAndLevel(bool apply = false);

    void negativeImage();

    void backup();

public slots:
    void undo();

    void redo();

    void restore();

    void brightnessAndContrast();
    void gammaTransformation();
    void thresholdSegmentation();
    void equalization();
    void enhancement();
    void lightFieldCorrection();

private:
    void initViewWindowWidthAndLevel();


    View* getActiveView() const;

    View* getAxialView() const;

    View* getCoronalView() const;

    View* getSagittalView() const;

private:
    MainWindow* _mainWindow;

    std::shared_ptr<BaseImage> _image;

    UndoSystem _undoStack;

    bool _modified;
};
