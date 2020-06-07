#pragma once

#include <QMouseEvent>

class ToolButton;

class MouseHandler
{
public:
	MouseHandler() {}
	virtual ~MouseHandler() {}

public:
	static void handlePress(QMouseEvent* event);

	static void handleMove(QMouseEvent* event);

	static void handleRelease(QMouseEvent* event);

	static void setLeftButton(ToolButton* button);

	static void setRightButton(ToolButton* button);

protected:
	virtual void press(QMouseEvent* event) = 0;

	virtual void move(QMouseEvent* event) = 0;

	virtual void release(QMouseEvent* event) = 0;

protected:
	static QPoint _mousePos;

private:
	static ToolButton* _leftButton;
	static ToolButton* _rightButton;
};

class ImageWindowMouseHandler : public MouseHandler
{
public:
	ImageWindowMouseHandler();

	void press(QMouseEvent* event) override;

	void move(QMouseEvent* event) override;

	void release(QMouseEvent* event) override;

private:
	bool CalcImageWindow(QPoint point, float& bottom, float& top);

	int _horzOrVert = 0;
	float _bottom;
	float _top;
};

class ZoomMouseHandler : public MouseHandler
{
public:
	void press(QMouseEvent* event) override;

	void move(QMouseEvent* event) override;

	void release(QMouseEvent* event) override;
};

class SelectMouseHandler : public MouseHandler
{
public:
	void press(QMouseEvent* event) override;

	void move(QMouseEvent* event) override;

	void release(QMouseEvent* event) override;
};

class MoveMouseHandler : public MouseHandler
{
public:
	void press(QMouseEvent* event) override;

	void move(QMouseEvent* event) override;

	void release(QMouseEvent* event) override;
};

class DrawMouseHandler : public MouseHandler
{
public:
	void press(QMouseEvent* event) override;

	void move(QMouseEvent* event) override;

	void release(QMouseEvent* event) override;
};