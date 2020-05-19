#pragma once

#include <QWidget>
#include "Peg.h"

typedef QVector<Peg> PegArray;

#define NONE_PEG				-1
#define PEG_DISTANCE			3

#ifndef CURVE_CHANNEL
#define CURVE_CHANNEL
#define CURVE_CHANNEL_GRAY		0
#define CURVE_CHANNEL_RED		1
#define CURVE_CHANNEL_GREEN		2
#define CURVE_CHANNEL_BLUE		3
#endif

class CurveSquare : public QWidget
{
	Q_OBJECT

public:
	CurveSquare(QWidget* parent = nullptr);
	virtual ~CurveSquare();

public:
	virtual QSize sizeHint() const;

	virtual QSize minimumSizeHint() const;

	void init();

	void reset();

	void reverse();

	// Generate histogram
	bool generateHistogram(uint* pArray);

	int getSize() { return _size; }

	int getChannel() { return _channel; }

	void setChannel(int channel);

	void setCurveOrLinear(bool curveOrLinear);

	bool getCurveOrLinear() { return _curveOrLinear; }

	PegArray& getIntensityPegsArray() { return _pegsIntensity; }

	PegArray& getRedPegsArray() { return _pegsRed; }

	PegArray& getGreenPegsArray() { return _pegsGreen; }

	PegArray& getBluePegsArray() { return _pegsBlue; }

	uint* getIntensity() { return _arrayIntensity; }
	uint* getRed() { return _arrayRed; }
	uint* getGreen() { return _arrayGreen; }
	uint* getBlue() { return _arrayBlue; }

	// Calculate array value
	void calcArrayValue();
	void calcArrayValue(int index, bool flag);

signals:
	void resize();

	void updateImage();

	void updateLabelText(QString input, QString output);

protected:
	virtual void resizeEvent(QResizeEvent* event);

	virtual void paintEvent(QPaintEvent* event);

	virtual void mousePressEvent(QMouseEvent* event);

	virtual void mouseMoveEvent(QMouseEvent* event);

	virtual void mouseReleaseEvent(QMouseEvent* event);

private:
	void paintHistogram();

	// Paint grid
	void paintBackground();

	// Paint pegs
	void paintPegs(QColor color);

	// Paint single peg
	void paintSinglePeg(const Peg& peg, QColor color);

	// Paint connection
	void paintConnection(QColor color);

	void initPegsArray();

	// Calculate window coordinate according to x and y
	QPoint getCoordinate(int x, int y);

	// Determine if the mouse is in any peg
	int ptInAnyPeg(QPoint point) const;

	// Preprocess before adding new peg
	// �ж��Ƿ������ڵ�ǰ�����peg; ����Ϊ�²�����x����
	bool prepareAddPeg(int xCoordinate);

	// Add peg
	int	addPeg(const QPoint& point);

	// Repaint
	void repaintPeg();

	// Sort pegs according to x coordinate
	int sortPegs(const Peg& peg);

	// Use piecewise linear function to set array value, if flag equal true means adding or moving peg, false means deleting peg
	void setLinearArrayValue(int index, bool flag = true);

	// �ı�һ���߶ε�ֵ������startIndex��ʾ�߶���ʼpeg��index
	// This function is only called by setLinearArrayValue()
	void setLinearValue(int startIndex);

	// Use spline function to set array value
	void setCurveArrayValue();

	// Get input&output value of current peg
	QSize getCurrentValue(int index);

	// Get input&output value of current position
	QSize getCurrentMouseValue(const QPoint& point);

	// Remove one peg
	void removePeg(int index);

	// Remove all pegs
	void removeAllPegs();

private:
	int _size;

	// Square rect
	QRect _rectSquare;

	// ���д�ŵ����������ÿ���߶�Ӧ��Եĸ߶�
	uint* _heightArray;

	// min& max in _heightArray
	uint _minHeight, _maxHeight;

	// Curve or line
	bool _curveOrLinear;

	// Channel
	int _channel;

	// Index of active peg
	int	_activePegIndex;

	// Active peg array
	PegArray* _activePegs;

	// Record peg array of each channel
	PegArray _pegsIntensity;
	PegArray _pegsRed;
	PegArray _pegsGreen;
	PegArray _pegsBlue;

	// Point to active array
	uint* _activeArray;

	// Record array value of each channel
	uint* _arrayIntensity;
	uint* _arrayRed;
	uint* _arrayGreen;
	uint* _arrayBlue;
};