#pragma once

#include <QDockWidget>

class PluginInterface : public QObject
{
    Q_OBJECT

public:
    virtual ~PluginInterface() {}

    virtual QDockWidget* createDockWidget(QWidget* parent = nullptr) = 0;

    virtual QString name() = 0;

signals:
    void openFile(const QString& fileName);

    void notify(const QString& title, const QString& message, int type);
};

Q_DECLARE_INTERFACE(PluginInterface, "Nova.Plugin.Interface");
