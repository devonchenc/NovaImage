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
        _waitForQuit = true;
    }

    void finished()
    {
        if (!_waitForQuit)
        {
            QMetaObject::invokeMethod(_widget, "accept", Qt::QueuedConnection);
        }
    }

    float minValue() const { return _minValue; }
    float maxValue() const { return _maxValue; }

protected:
    QWidget* _widget;

    std::atomic<bool> _waitForQuit;

    QString _pathName;

    float _minValue;
    float _maxValue;
};