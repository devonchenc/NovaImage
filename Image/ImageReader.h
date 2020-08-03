#pragma once

#include <QThread>
#include <QFile>

class AbstractReader : public QThread
{
    Q_OBJECT

public:
    AbstractReader(const QString& pathName, QObject* parent = nullptr)
        : QThread(parent)
        , _pathName(pathName)
        , _widget(nullptr)
        , _minValue(FLT_MAX)
        , _maxValue(-FLT_MAX)
    {}

    void setWidget(QWidget* widget) { _widget = widget; }

    float minValue() { return _minValue; }
    float maxValue() { return _maxValue; }

protected:
    QWidget* _widget;
    QString _pathName;

    float _minValue;
    float _maxValue;
};

template <class Type>
class ImageReader : public AbstractReader
{
public:
    ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, Type* buffer);

protected:
    void run() override;

private:
    int _headSize;
    int _pixelPerSlice;
    int _slice;
    Type* _buffer;
};


template <class Type>
ImageReader<Type>::ImageReader(const QString& pathName, int headSize, int pixelCount, int slice, Type* buffer)
    : AbstractReader(pathName, nullptr)
    , _headSize(headSize)
    , _pixelPerSlice(pixelCount)
    , _slice(slice)
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
    for (int i = 0; i < _slice; i++)
    {
        qint64 readSize = file.read((char*)(_buffer + i * _pixelPerSlice), sizeof(Type) * _pixelPerSlice);
        if (readSize != qint64(sizeof(Type)) * qint64(_pixelPerSlice))
        {
            file.close();
            QMessageBox::critical(nullptr, QObject::tr("Open image file error"),
                QObject::tr("The data size does not match the file information description!"), QMessageBox::Ok);
            QMetaObject::invokeMethod(_widget, "reject", Qt::QueuedConnection);
            return;
        }

        Type* temp = _buffer + i * _pixelPerSlice;
        for (unsigned long i = 1; i < _pixelPerSlice; i++)
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

        int progress = (i + 1) * 100 / _slice;
        QMetaObject::invokeMethod(_widget, "setProgress", Qt::QueuedConnection, Q_ARG(int, progress));
    }

    file.close();

    QMetaObject::invokeMethod(_widget, "accept", Qt::QueuedConnection);
}