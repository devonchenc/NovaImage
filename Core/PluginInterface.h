#pragma once

#include <QDockWidget>

class PluginInterface
{
public:
	virtual QDockWidget* createDockWidget(QWidget* parent = nullptr) = 0;

	virtual QString name() = 0;
};

Q_DECLARE_INTERFACE(PluginInterface, "Nova.Plugin.Interface");