#pragma once

#include <QThread>
#include <QFile>

template <class Type>
class ImageReader : public QThread
{
public:
    ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, Type* buffer);

public:
    void setWidget(QWidget* widget) { _widget = widget; }

protected:
    void run() override;

private:
    QString _pathName;
    int _headSize;
    int _pixelPerSlice;
    int _slice;
    Type* _buffer;
    QWidget* _widget;
};


template <class Type>
ImageReader<Type>::ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, Type* buffer)
    : QThread(nullptr)
    , _pathName(pathName)
    , _headSize(headSize)
    , _pixelPerSlice(pixelCount)
    , _slice(slice)
    , _buffer(buffer)
    , _widget(nullptr)
{

}

template <class Type>
void ImageReader<Type>::run()
{
    QFile file(_pathName);
    if (!file.open(QFile::ReadOnly))
        return;

    if (_widget)
    {
        QMetaObject::invokeMethod(_widget, "setTitle", Qt::QueuedConnection, Q_ARG(const QString&, tr("Loading file...")));
    }

    file.seek(_headSize);
    for (int i = 0; i < _slice; i++)
    {
        file.read((char*)(_buffer + i * _pixelPerSlice), sizeof(Type) * _pixelPerSlice);
        msleep(20);

        if (_widget)
        {
            int progress = (i + 1) * 100 / _slice;
            QMetaObject::invokeMethod(_widget, "setProgress", Qt::QueuedConnection, Q_ARG(int, progress));
        }
    }

    file.close();

    QMetaObject::invokeMethod(_widget, "accept", Qt::QueuedConnection);
}