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

protected:
	virtual void press(QMouseEvent* event) = 0;

	virtual void move(QMouseEvent* event) = 0;

	virtual void release(QMouseEvent* event) = 0;

private:

};

class ZoomMouseHandler : public MouseHandler
{
public:
	ZoomMouseHandler() {}
	~ZoomMouseHandler(){}

public:
	void press(QMouseEvent* event) override;

	void move(QMouseEvent* event) override;

	void release(QMouseEvent* event) override;
};