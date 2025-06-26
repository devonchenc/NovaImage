#pragma once

#include <QThread>
#include <QFile>
#include <QMessageBox>

#include "AbstractReader.h"

template <class Type>
class ImageReader : public AbstractReader
{
public:
    ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, Type* buffer);
    ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, int endian, Type* buffer);

protected:
    void run() override;

private:
    void loadSlice(QFile& file, int index);

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
        loadSlice(file, n);
    }

    file.close();

    finished();
}

template <class Type>
void ImageReader<Type>::loadSlice(QFile& file, int index)
{
    if (_waitForQuit)
        return;

    qint64 readSize = file.read((char*)(_buffer + index * _pixelPerSlice), sizeof(Type) * _pixelPerSlice);
    if (readSize != qint64(sizeof(Type)) * qint64(_pixelPerSlice))
    {
        file.close();
        QMessageBox::critical(nullptr, tr("Open image file error"),
            tr("The data size does not match the file information description!"), QMessageBox::Ok);
        QMetaObject::invokeMethod(_widget, "reject", Qt::QueuedConnection);
        return;
    }

    if (_endian == 1 && sizeof(Type) > 1)
    {
        // Convert endian byte order
        for (int i = 0; i < _pixelPerSlice; i++)
        {
            convertEndian(_buffer + index * _pixelPerSlice + i);
        }
    }

    Type* temp = _buffer + index * _pixelPerSlice;
    for (int i = 0; i < _pixelPerSlice; i++)
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

    int progress = (index + 1) * 100 / _slice;
    QMetaObject::invokeMethod(_widget, "setProgress", Qt::QueuedConnection, Q_ARG(int, progress));
}

template <class Type>
void ImageReader<Type>::convertEndian(Type* data)
{
    for (size_t i = 0; i < sizeof(Type) / 2; i++)
    {
        char temp = ((char*)data)[sizeof(Type) - i - 1];
        ((char*)data)[sizeof(Type) - i - 1] = ((char*)data)[i];
        ((char*)data)[i] = temp;
    }
}
