#pragma once

#include <QVector>

class BaseWidget;

class WidgetManager
{
private:
	WidgetManager() {}
	~WidgetManager() {}

public:
	static WidgetManager* getInstance();
	static void cleanUp();

	void addWidget(BaseWidget* widget);

	void init();

	void reset();

private:
	static WidgetManager* _instance;

	QVector<BaseWidget*> _vecWidget;
};