#pragma once

#include <QThread>
#include <QWidget>
#include <mutex>

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
    {
    }

    void setWidget(QWidget* widget) { _widget = widget; }

    void waitForQuit()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _waitForQuit = true;
    }

    void finished()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_waitForQuit)
        {
            QMetaObject::invokeMethod(_widget, "accept", Qt::QueuedConnection);
        }
    }

    float minValue() { return _minValue; }
    float maxValue() { return _maxValue; }

protected:
    QWidget* _widget;

    std::mutex _mutex;
    bool _waitForQuit;

    QString _pathName;

    float _minValue;
    float _maxValue;
};