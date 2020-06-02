#pragma once

#include <QToolButton>

class MouseHandler;

class ToolButton : public QToolButton
{
	Q_OBJECT

public:
	ToolButton(QWidget* parent = nullptr);
	virtual	~ToolButton();

public:
	void setIconByName(const QString& fileName);

	void setMouseHandler(MouseHandler* handler);

	MouseHandler* mouseHandler();

	static void setLeftMouseButton(ToolButton* toolButton);

	static void setRightMouseButton(ToolButton* toolButton);

private:
	static void updateAllButtonsIcon();

	void updateIcon();

private:
	QString _fileName;

	MouseHandler* _handler;

	static ToolButton* _leftMouseButton;
	static ToolButton* _rightMouseButton;

	static QVector<ToolButton*> _vec;
};

