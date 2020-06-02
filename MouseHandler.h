#pragma once

#include <QMouseEvent>

class MouseHandler
{
public:
	MouseHandler() {}
	virtual ~MouseHandler(){}

public:
	static void handlePress(QMouseEvent* event);

	static void handleMove(QMouseEvent* event);

	static void handleRelease(QMouseEvent* event);

	static void setLeftHandler(MouseHandler* handler);

	static void setRightHandler(MouseHandler* handler);

protected:
	virtual void press(QMouseEvent* event) = 0;

	virtual void move(QMouseEvent* event) = 0;

	virtual void release(QMouseEvent* event) = 0;

protected:
	static QPoint _mousePos;

private:
	static MouseHandler* _leftHandler;
	static MouseHandler* _rightHandler;
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