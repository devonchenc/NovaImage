#pragma once

#include <QWidget>

#define HS_MARGIN				10
#define CURSOR_SIZE				9

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class HistogramProcessor;

class HistogramWidget : public QWidget
{
    Q_OBJECT

public:
    HistogramWidget(QWidget* parent = nullptr);
    virtual ~HistogramWidget();

public:
    QSize sizeHint() const override;

    QSize minimumSizeHint() const override;

    void init();

    void reset();

    void setParameters(float bottom, float mid, float top);

public slots:
    void clickReset();

signals:
    void resetControl();

    void updateBottom(float bottom);

    void updateMid(float mid);

    void updateTop(float top);

protected:
    void resizeEvent(QResizeEvent* event) override;

    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

    void changeEvent(QEvent* event) override;

private:
    void paintCursor();

    void paintHistogram();

    // Set image window
    void setBottomAndTop(bool* pArray, int arrayNum);

    // Allocate memory
    void allocateMemory(int oldWidth = 0);

    void copySelectArray(bool* array, int arrayNum);

    void generateHistogram();

    // Calculate selected area
    void calcSelectArea();

    // Calculate temporary selected area
    void calcSelectTempArea();

    // Get rect of cursor
    QRect getCursorRect(int index);

    // Change index to height
    int indexToHeight(int i);

private:
    // Height array of each line
    uint* _heightArray;

    // Min and max value in _heightArray
    uint _minHeight, _maxHeight;

    // Selected start point and finish point
    int _start, _finish;

    // Selected array
    bool* _select;

    // Temporary selected array
    bool* _selectTemp;

    // Width and height of histogram
    QRect _rectHistogram;

    // Cursor position
    int _cursorPos[3];

    // Drag flag
    int _drag;

    // Min and max value of image
    float _minValue, _maxValue;

    // 色阶调整对应的控件
    float _bottom;
    float _mid;
    float _top;

    QPushButton* _resetButton;

    // Image processor
    HistogramProcessor* _processor;
};
