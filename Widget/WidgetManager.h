#pragma once

#include <QVector>

class BaseWidget;

class WidgetManager
{
public:
    static WidgetManager* getInstance();
    static void cleanUp();

    void addWidget(BaseWidget* widget);

    void init();

    void reset();

private:
    WidgetManager() {}
    ~WidgetManager() {}

private:
    static WidgetManager* _instance;

    QVector<BaseWidget*> _vecWidget;
};
