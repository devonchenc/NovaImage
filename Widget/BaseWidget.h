#pragma once

#include <QWidget>

class BaseWidget : public QWidget
{
	Q_OBJECT

public:
    BaseWidget(QWidget* parent = nullptr) { Q_UNUSED(parent); }

public:
	virtual void init() {}

	virtual void reset() {}

	// Set name
	void setName(QString name)	{ _name = name; }

	// Get name
	QString getName()			{ return _name; }

protected:
	// Widget name
	QString _name;
};
