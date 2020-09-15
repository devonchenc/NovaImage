#pragma once

#include <memory>
#include <QtGlobal>
#include <QImage>

class MainWindow;
class Document;
class BaseImage;
class View;

MainWindow* getGlobalWindow();

Document* getGlobalDocument();

View* getGlobalActiveView();

View* getGlobalAxialView();
View* getGlobalCoronalView();
View* getGlobalSagittalView();

BaseImage* getGlobalImage();

void repaintView();
void repaintView(QImage* image, int viewType = 0);
void repaintView(std::shared_ptr<QImage> dstImage, int viewType = 0);
void repaintView(std::vector<std::shared_ptr<QImage>> imageVec);

QPointF stringToPointF(const QString& str);

QColor stringToColor(const QString& str);

QPolygonF stringToPolygon(const QString& str);

QString pointFToString(const QPointF& point);

QString colorToString(const QColor& color);

QString polygonToString(const QPolygonF& polygon);

bool copyByteToImage(uchar* byteImage, int width, int height, QImage* pImage);

// Spline function
float spline(float* x, float* y, int n, float* t, int m, float* z);
