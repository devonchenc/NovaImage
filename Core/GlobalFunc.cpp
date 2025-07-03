#include "GlobalFunc.h"

#include <QApplication>
#include "mainwindow.h"
#include "Document.h"

MainWindow* getGlobalWindow()
{
    foreach(QWidget* w, qApp->topLevelWidgets())
        if (MainWindow* mainWin = qobject_cast<MainWindow*>(w))
            return mainWin;
    return nullptr;
}

Document* getGlobalDocument()
{
    MainWindow* mainWindow = getGlobalWindow();
    return mainWindow->getDocument();
}

View* getGlobalActiveView()
{
    MainWindow* mainWindow = getGlobalWindow();
    return mainWindow->getActiveView();
}

View* getGlobalAxialView()
{
    MainWindow* mainWindow = getGlobalWindow();
    return mainWindow->getAxialView();
}

View* getGlobalCoronalView()
{
    MainWindow* mainWindow = getGlobalWindow();
    return mainWindow->getCoronalView();
}

View* getGlobalSagittalView()
{
    MainWindow* mainWindow = getGlobalWindow();
    return mainWindow->getSagittalView();
}

BaseImage* getGlobalImage()
{
    Document* document = getGlobalDocument();
    if (document)
    {
        return document->getImage();
    }
    else
    {
        return nullptr;
    }
}

void repaintView()
{
    Document* document = getGlobalDocument();
    if (document)
    {
        document->repaintView();
    }
}

void repaintView(const QImage* image, int viewType)
{
    Document* document = getGlobalDocument();
    if (document)
    {
        document->repaintView(image, viewType);
    }
}

void repaintView(std::shared_ptr<QImage> dstImage, int viewType)
{
    Document* document = getGlobalDocument();
    if (document)
    {
        document->repaintView(dstImage, viewType);
    }
}

void repaintView(const std::vector<std::shared_ptr<QImage>>& imageVec)
{
    Document* document = getGlobalDocument();
    if (document)
    {
        document->repaintView(imageVec);
    }
}

QPointF stringToPointF(const QString& str)
{
    int index = str.indexOf(' ');
    QPointF point;
    point.setX(str.left(index).toFloat());
    point.setY(str.right(str.length() - index - 1).toFloat());
    return point;
}

QColor stringToColor(const QString& str)
{
    QColor color;
    int index1 = str.indexOf(' ');
    int index2 = str.indexOf(' ', index1 + 1);
    color.setRed(str.left(index1).toInt());
    color.setGreen(str.mid(index1 + 1, index2 - index1).toInt());
    color.setBlue(str.right(str.length() - index2 - 1).toInt());
    return color;
}

QPolygonF stringToPolygon(const QString& str)
{
    QPolygonF polygon;

    QString temp = str;
    while (!temp.isEmpty())
    {
        int index = temp.indexOf(',');
        QPointF point = stringToPointF(temp.left(index));
        polygon.push_back(point);
        temp = temp.right(temp.length() - index - 1);
    }

    return polygon;
}

QString pointFToString(const QPointF& point)
{
    return QString("%1 %2").arg(point.x()).arg(point.y());
}

QString colorToString(const QColor& color)
{
    return QString("%1 %2 %3").arg(color.red()).arg(color.green()).arg(color.blue());
}

QString polygonToString(const QPolygonF& polygon)
{
    QString str;
    for (int i = 0; i < polygon.size(); i++)
    {
        str += pointFToString(polygon.at(i)) + ',';
    }
    return str;
}

bool copyByteToImage(const uchar* byteImage, int width, int height, QImage* pImage)
{
    if (byteImage == nullptr || pImage == nullptr)
        return false;

    if (width != pImage->width() || height != pImage->height())
        return false;

    uchar* pData = pImage->bits();
    int pitch = pImage->bytesPerLine();
    int depth = pImage->depth() / 8;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width * depth; i++)
        {
            uchar* pixel = pData + j * pitch + i;
            *pixel = byteImage[(j * width) * depth + i];
        }
    }

    return true;
}

// Spline function
float spline(float* x, float* y, int n, float* t, int m, float* z)
{
    float* dy = new float[n];
    memset(dy, 0, sizeof(float) * n);
    dy[0] = -0.5f;

    float* ddy = new float[n];
    memset(ddy, 0, sizeof(float) * n);

    float h1 = 0;
    float* s = new float[n];
    float h0 = x[1] - x[0];
    s[0] = 3.0f * (y[1] - y[0]) / (2.0f * h0) - ddy[0] * h0 / 4.0f;
    for (int j = 1; j <= n - 2; j++)
    {
        h1 = x[j + 1] - x[j];
        float alpha = h0 / (h0 + h1);
        float beta = (1.0f - alpha) * (y[j] - y[j - 1]) / h0;
        beta = 3.0f * (beta + alpha * (y[j + 1] - y[j]) / h1);
        dy[j] = -alpha / (2.0f + (1.0f - alpha) * dy[j - 1]);
        s[j] = (beta - (1.0f - alpha) * s[j - 1]);
        s[j] = s[j] / (2.0f + (1.0f - alpha) * dy[j - 1]);
        h0 = h1;
    }
    dy[n - 1] = (3.0f * (y[n - 1] - y[n - 2]) / h1 + ddy[n - 1] * h1 / 2.0f - s[n - 2]) / (2.0f + dy[n - 2]);
    for (int j = n - 2; j >= 0; j--)
    {
        dy[j] = dy[j] * dy[j + 1] + s[j];
    }
    for (int j = 0; j <= n - 2; j++)
    {
        s[j] = x[j + 1] - x[j];
    }
    for (int j = 0; j <= n - 2; j++)
    {
        h1 = s[j] * s[j];
        ddy[j] = 6.0f * (y[j + 1] - y[j]) / h1 - 2.0f * (2.0f * dy[j] + dy[j + 1]) / s[j];
    }
    h1 = s[n - 2] * s[n - 2];
    ddy[n - 1] = 6.0f * (y[n - 2] - y[n - 1]) / h1 + 2.0f * (2.0f * dy[n - 1] + dy[n - 2]) / s[n - 2];
    float g = 0.0f;
    for (int i = 0; i <= n - 2; i++)
    {
        h1 = 0.5f * s[i] * (y[i] + y[i + 1]);
        h1 = h1 - s[i] * s[i] * s[i] * (ddy[i] + ddy[i + 1]) / 24.0f;
        g = g + h1;
    }
    for (int j = 0; j <= m - 1; j++)
    {
        int i;
        if (t[j] >= x[n - 1])
        {
            i = n - 2;
        }
        else
        {
            i = 0;
            while (t[j] > x[i + 1])
            {
                i = i + 1;
            }
        }
        h1 = (x[i + 1] - t[j]) / s[i];
        h0 = h1 * h1;
        z[j] = (3.0f * h0 - 2.0f * h0 * h1) * y[i];
        z[j] = z[j] + s[i] * (h0 - h0 * h1) * dy[i];
        h1 = (t[j] - x[i]) / s[i];
        h0 = h1 * h1;
        z[j] = z[j] + (3.0f * h0 - 2.0f * h0 * h1) * y[i + 1];
        z[j] = z[j] - s[i] * (h0 - h0 * h1) * dy[i + 1];
    }
    delete[] s;
    delete[] dy;
    delete[] ddy;

    return g;
}
