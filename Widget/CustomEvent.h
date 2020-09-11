#pragma once

#include <QEvent>

class CustomEvent : public QEvent
{
public:
    CustomEvent() : QEvent(CustomEvent::type()) {}

    virtual ~CustomEvent() {}

    static QEvent::Type type()
    {
        if (customEventType == QEvent::None)
        {
            int generatedType = QEvent::registerEventType();
            customEventType = static_cast<QEvent::Type>(generatedType);
        }
        return customEventType;
    }

private:
    static QEvent::Type customEventType;
};
