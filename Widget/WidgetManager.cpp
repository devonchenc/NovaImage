#include "WidgetManager.h"

#include "BaseWidget.h"

WidgetManager* WidgetManager::_instance = nullptr;

WidgetManager* WidgetManager::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new WidgetManager;
	}

	return _instance;
}

void WidgetManager::cleanUp()
{
	if (_instance)
	{
		delete _instance;
		_instance = nullptr;
	}
}

void WidgetManager::addWidget(BaseWidget* widget)
{
	_vecWidget.append(widget);
}

void WidgetManager::init()
{
	for (auto widget : _vecWidget)
	{
		widget->init();
	}
}

void WidgetManager::reset()
{
	for (auto widget : _vecWidget)
	{
		widget->reset();
	}
}