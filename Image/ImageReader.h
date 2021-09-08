#pragma once

#include <float.h>
#include <QThread>
#include <QFile>
#include <QMessageBox>

class AbstractReader : public QThread
{
    Q_OBJECT

public:
    AbstractReader(const QString& pathName, QObject* parent = nullptr)
        : QThread(parent)
        , _widget(nullptr)
        , _waitForQuit(false)
        , _pathName(pathName)
        , _minValue(FLT_MAX)
        , _maxValue(-FLT_MAX)
    {}

    void setWidget(QWidget* widget) { _widget = widget; }

    void waitForQuit() { _waitForQuit = true; }

    float minValue() { return _minValue; }
    float maxValue() { return _maxValue; }

protected:
    QWidget* _widget;
    bool _waitForQuit;

    QString _pathName;

    float _minValue;
    float _maxValue;
};

template <class Type>
class ImageReader : public AbstractReader
{
public:
    ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, Type* buffer);
    ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, int endian, Type* buffer);

protected:
    void run() override;

private:
    // Convert endian byte order
    void convertEndian(Type* data);

private:
    int _headSize;
    int _pixelPerSlice;
    int _slice;
    int _endian;
    Type* _buffer;
};

template <class Type>
ImageReader<Type>::ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, Type* buffer)
    : AbstractReader(pathName, nullptr)
    , _headSize(headSize)
    , _pixelPerSlice(pixelCount)
    , _slice(slice)
    , _endian(0)
    , _buffer(buffer)
{

}

template <class Type>
ImageReader<Type>::ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, int endian, Type* buffer)
    : AbstractReader(pathName, nullptr)
    , _headSize(headSize)
    , _pixelPerSlice(pixelCount)
    , _slice(slice)
    , _endian(endian)
    , _buffer(buffer)
{

}

template <class Type>
void ImageReader<Type>::run()
{
    QFile file(_pathName);
    if (!file.open(QFile::ReadOnly))
    {
        QMetaObject::invokeMethod(_widget, "reject", Qt::QueuedConnection);
        return;
    }

    QMetaObject::invokeMethod(_widget, "setTitle", Qt::QueuedConnection, Q_ARG(const QString&, tr("Loading file...")));

    file.seek(_headSize);
    for (int n = 0; n < _slice; n++)
    {
        qint64 readSize = file.read((char*)(_buffer + n * _pixelPerSlice), sizeof(Type) * _pixelPerSlice);
        if (readSize != qint64(sizeof(Type)) * qint64(_pixelPerSlice))
        {
            file.close();
            QMessageBox::critical(nullptr, QObject::tr("Open image file error"),
                QObject::tr("The data size does not match the file information description!"), QMessageBox::Ok);
            QMetaObject::invokeMethod(_widget, "reject", Qt::QueuedConnection);
            return;
        }

        if (_waitForQuit)
            return;

        if (_endian == 1 && sizeof(Type) > 1)
        {
            // Convert endian byte order
            for (int i = 0; i < _pixelPerSlice; i++)
            {
                convertEndian(_buffer + n * _pixelPerSlice + i);
            }
        }

        Type* temp = _buffer + n * _pixelPerSlice;
        for (int i = 1; i < _pixelPerSlice; i++)
        {
            if (_minValue > temp[i])
            {
                _minValue = temp[i];
            }
            if (_maxValue < temp[i])
            {
                _maxValue = temp[i];
            }
        }

        int progress = (n + 1) * 100 / _slice;
        QMetaObject::invokeMethod(_widget, "setProgress", Qt::QueuedConnection, Q_ARG(int, progress));
    }

    file.close();

    QMetaObject::invokeMethod(_widget, "accept", Qt::QueuedConnection);
}

template <class Type>
void ImageReader<Type>::convertEndian(Type* data)
{
    for (int i = 0; i < sizeof(Type) / 2; i++)
    {
        char temp = ((char*)data)[sizeof(Type) - i - 1];
        ((char*)data)[sizeof(Type) - i - 1] = ((char*)data)[i];
        ((char*)data)[i] = temp;
    }
}