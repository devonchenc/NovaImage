#pragma once

#include <QToolButton>

class ToolButton : public QToolButton
{
	Q_OBJECT

public:
	ToolButton(QWidget* parent = nullptr);
	virtual	~ToolButton();

public:
	void setIconByName(const QString& fileName);

	static void setLeftMouseButton(ToolButton* toolButton);

	static void setRightMouseButton(ToolButton* toolButton);

protected:
	void mousePressEvent(QMouseEvent* event) override;

private:
	static void updateAllButtonsIcon();

	void updateIcon();

private:
	QString _fileName;

	static ToolButton* _leftMouseButton;
	static ToolButton* _rightMouseButton;

	static QVector<ToolButton*> _vec;
};

