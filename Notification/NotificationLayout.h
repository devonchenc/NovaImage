#pragma once

#include "NotificationWidget.h"

#include <QObject>
#include <QElapsedTimer>
#include <QBasicTimer>

class NotificationLayout : public QObject
{
    Q_OBJECT

public:
    NotificationLayout(QObject* parent = nullptr);
    ~NotificationLayout() override;

    void addNotification(const NotificationParams& params);
    void addNotification(const QString& title, const QString& message, NotificationParams::Type type);

    void setLayoutType(Qt::Alignment align);
    void setDisplayTimeMs(int displayTimeMS);

private slots:
    void onCloseClicked(NotificationWidget* notification);
    void onDetailsClicked(NotificationWidget* notification);
    void onWidgetDestroyed();

private:
    void layoutWidgets();
    void clear();

    void timerEvent(QTimerEvent* event) override;

private:
    struct NotificationWidgetParams;
    using NotificationPair = QPair<NotificationWidget*, NotificationWidgetParams>;
    using WindowNotifications = std::vector<NotificationPair>;

    WindowNotifications _vecWidget;

    Qt::Alignment layoutType = Qt::AlignBottom | Qt::AlignRight;

    int _displayTimeMs = 6000;
    const int _maximumDisplayCount = 5;
    QElapsedTimer _elapsedTimer;
    QBasicTimer _basicTimer;
};
