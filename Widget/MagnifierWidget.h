#pragma once

#include <QWidget>
#include <QTimer>

class MagnifierWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MagnifierWidget(QWidget* parent = nullptr);

public:
    void setMagnifyArea(QSize size);

    void setMagnifyTimes(int times);

protected:
    void paintEvent(QPaintEvent*) override;

    void mousePressEvent(QMouseEvent*) override;

    void mouseReleaseEvent(QMouseEvent*) override;

public slots:
    void updatePosition();

private:
    enum Direction { TopLeft, TopRight, BottomLeft, BottomRight};
    Direction _displayDirection;
    QSize _magnifyAeraSize;
    int _magnifyTimes;
    QSize _magnifierSize;
    QSize _totalSize;
    QTimer _timer;
};
