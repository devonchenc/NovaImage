#pragma once

#include <QtGlobal>
#include <QImage>

class MainWindow;
class Document;
class BaseImage;
class View;

MainWindow* getGlobalWindow();

Document* getGlobalDocument();

View* getGlobalView();

BaseImage* getGlobalImage();

void repaintView();

QPointF stringToPointF(const QString& str);

QColor stringToColor(const QString& str);

QPolygonF stringToPolygon(const QString& str);

QString pointFToString(const QPointF& point);

QString colorToString(const QColor& color);

QString polygonToString(const QPolygonF& polygon);

bool copyByteToImage(uchar* byteImage, int width, int height, QImage* pImage);

// Spline function
float spline(float* x, float* y, int n, float* t, int m, float* z);