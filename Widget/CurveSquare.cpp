#include "CurveSquare.h"

#include <QPainter>
#include <QMouseEvent>
#include <cmath>

#include "../Core/GlobalFunc.h"

CurveSquare::CurveSquare(QWidget* parent)
    : QWidget(parent)
    , _size(256)
    , _arraySize(_size + 1)
    , _rectSquare(30, 0, _size, _size)
    , _curveOrLinear(true)
    , _channel(0)
    , _activePegIndex(0)
{
    _heightArray = new uint[_size];
    memset(_heightArray, 0, sizeof(uint) * _size);

    initPegsArray();

    // Initialize array
    _arrayIntensity = new uint[_arraySize];
    _arrayRed = new uint[_arraySize];
    _arrayGreen = new uint[_arraySize];
    _arrayBlue = new uint[_arraySize];
    for (int i = 0; i < _arraySize; i++)
    {
        _arrayIntensity[i] = _arrayRed[i] = _arrayGreen[i] = _arrayBlue[i] = i;
    }
    _activeArray = _arrayIntensity;

    setMouseTracking(true);
}

CurveSquare::~CurveSquare()
{
    if (_heightArray)
    {
        delete[] _heightArray;
    }
    if (_arrayIntensity)
    {
        delete[] _arrayIntensity;
    }
    if (_arrayRed)
    {
        delete[] _arrayRed;
    }
    if (_arrayGreen)
    {
        delete[] _arrayGreen;
    }
    if (_arrayBlue)
    {
        delete[] _arrayBlue;
    }
}

void CurveSquare::init()
{
    removeAllPegs();

    repaint();
}

void CurveSquare::reset()
{
    removeAllPegs();

    repaint();
}

void CurveSquare::reverse()
{
    for (int i = 0; i < _activePegs->size(); i++)
    {
        QPoint point(_activePegs->at(i).x(), _size - _activePegs->at(i).y());
        _activePegs->operator[](i).setPoint(point);
    }
    // Reverse array
    for (int i = 0; i < _arraySize; i++)
    {
        _activeArray[i] = _arraySize - _activeArray[i] - 1;
    }
    repaint();

    emit updateImage();
}

QSize CurveSquare::sizeHint() const
{
    return QSize(256, 256);
}

QSize CurveSquare::minimumSizeHint() const
{
    return QSize(256, 256);
}

void CurveSquare::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    QSize size = event->size();

    int newSize = qMin(size.width(), size.height()) - 30;
    if (_size == newSize)
        return;

    _size = newSize;
    _rectSquare = QRect(30, 0, _size, _size);

    if (_heightArray)
    {
        delete[] _heightArray;
    }
    _heightArray = new uint[_size];
    memset(_heightArray, 0, sizeof(uint) * _size);

    Peg::setSize(_size);

    if (_arrayIntensity)
    {
        delete[] _arrayIntensity;
    }
    if (_arrayRed)
    {
        delete[] _arrayRed;
    }
    if (_arrayGreen)
    {
        delete[] _arrayGreen;
    }
    if (_arrayBlue)
    {
        delete[] _arrayBlue;
    }
    _arraySize = _size + 1;
    _arrayIntensity = new uint[_arraySize];
    _arrayRed = new uint[_arraySize];
    _arrayGreen = new uint[_arraySize];
    _arrayBlue = new uint[_arraySize];
    if (_channel == CURVE_CHANNEL_GRAY)
    {
        _activeArray = _arrayIntensity;
    }
    else if (_channel == CURVE_CHANNEL_RED)
    {
        _activeArray = _arrayRed;
    }
    else if (_channel == CURVE_CHANNEL_GREEN)
    {
        _activeArray = _arrayGreen;
    }
    else if (_channel == CURVE_CHANNEL_BLUE)
    {
        _activeArray = _arrayBlue;
    }
    calcArrayValue();

    repaint();

    // Emit signal to parent widget
    emit resize();
}

void CurveSquare::paintEvent(QPaintEvent* /*event*/)
{
    paintHistogram();
    paintBackground();
}

void CurveSquare::paintHistogram()
{
    if (getGlobalDocument() == nullptr)
        return;

    if (_maxHeight == 0)
        return;

    QPainter painter(this);
    painter.setPen(QPen(Qt::blue));

    for (int i = 0; i < _size; i++)
    {
        painter.drawLine(i + _rectSquare.left(), _rectSquare.bottom(), i + _rectSquare.left(), _rectSquare.bottom() - _heightArray[i]);
    }
}

// Paint grid
void CurveSquare::paintBackground()
{
    QPainter painter(this);

    // Draw a horizontal ribbon and a vertical ribbon
    int gap = 6;
    QPoint point(_rectSquare.left() - gap, _rectSquare.top());
    int colorWidth = 15;
    for (int i = 0; i < _size; i++)
    {
        int j = 255 - i * 255 / (_size - 1);
        QPen pen(QColor(j, j, j));
        painter.setPen(pen);
        painter.drawLine(point.x() - colorWidth, point.y() + i, point.x(), point.y() + i);
    }

    point = QPoint(_rectSquare.left(), _rectSquare.bottom() + gap);
    for (int i = 0; i < _size; i++)
    {
        int j = i * 255 / (_size - 1);
        QPen pen(QColor(j, j, j));
        painter.setPen(pen);
        painter.drawLine(point.x() + i, point.y(), point.x() + i, point.y() + colorWidth);
    }

    // Draw square, three horizontal lines and three vertical lines
    QPen pen(Qt::black);
    painter.setPen(pen);
    painter.drawLine(_rectSquare.topLeft(), _rectSquare.topRight());
    painter.drawLine(_rectSquare.topRight(), _rectSquare.bottomRight());
    painter.drawLine(_rectSquare.bottomLeft(), _rectSquare.bottomRight());
    painter.drawLine(_rectSquare.topLeft(), _rectSquare.bottomLeft());

    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.drawLine(QPoint(_rectSquare.left(), _rectSquare.top() + _size / 4),
                     QPoint(_rectSquare.right(), _rectSquare.top() + _size / 4));
    painter.drawLine(QPoint(_rectSquare.left(), _rectSquare.top() + _size / 2),
                     QPoint(_rectSquare.right(), _rectSquare.top() + _size / 2));
    painter.drawLine(QPoint(_rectSquare.left(), _rectSquare.top() + _size * 3/ 4),
                     QPoint(_rectSquare.right(), _rectSquare.top() + _size * 3 / 4));

    painter.drawLine(QPoint(_rectSquare.left() + _size / 4, _rectSquare.top()),
                     QPoint(_rectSquare.left() + _size / 4, _rectSquare.bottom()));
    painter.drawLine(QPoint(_rectSquare.left() + _size / 2, _rectSquare.top()),
                     QPoint(_rectSquare.left() + _size / 2, _rectSquare.bottom()));
    painter.drawLine(QPoint(_rectSquare.left() + _size * 3 / 4, _rectSquare.top()),
                     QPoint(_rectSquare.left() + _size * 3 / 4, _rectSquare.bottom()));

    QColor color;
    switch (_channel)
    {
    case CURVE_CHANNEL_GRAY:
        color = Qt::black;
        break;
    case CURVE_CHANNEL_RED:
        color = Qt::red;
        break;
    case CURVE_CHANNEL_GREEN:
        color = Qt::green;
        break;
    case CURVE_CHANNEL_BLUE:
        color = Qt::blue;
        break;
    default:
        color = Qt::black;
        break;
    }

    // Paint pegs
    paintPegs(color);

    // Paint connection
    paintConnection(color);
}

void CurveSquare::paintPegs(QColor color)
{
    // Traverse pegs array
    for (int i = 0; i < _activePegs->size(); i++)
    {
        paintSinglePeg(_activePegs->at(i), color);
    }

    // Paint active peg as a solid square
    if (_activePegIndex != NONE_PEG && _activePegIndex < _activePegs->size())
    {
        QPoint point = _activePegs->at(_activePegIndex).point(_rectSquare.topLeft());

        int size = 2;
        QRect rect(point.x() - size, point.y() - size, size * 2, size * 2);
        QPainter painter(this);
        painter.setBrush(QBrush(color));
        painter.drawRect(rect);
    }
}

// Paint single peg
void CurveSquare::paintSinglePeg(const Peg& peg, QColor color)
{
    QPoint point = peg.point(_rectSquare.topLeft());

    QPainter painter(this);
    painter.setPen(QPen(color));
    int size = 2;
    painter.drawLine(point.x() - size, point.y() - size, point.x() + size, point.y() - size);
    painter.drawLine(point.x() + size, point.y() - size, point.x() + size, point.y() + size);
    painter.drawLine(point.x() - size, point.y() + size, point.x() + size, point.y() + size);
    painter.drawLine(point.x() - size, point.y() - size, point.x() - size, point.y() + size);
}

// Paint connection
void CurveSquare::paintConnection(QColor color)
{
    if (_activeArray == nullptr)
        return;

    // Connect all pegs
    QVector<QLine> lines;
    for (int i = 0; i < _arraySize - 1; i++)
    {
        lines.append(QLine(getCoordinate(i, _activeArray[i]), getCoordinate(i + 1, _activeArray[i + 1])));
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(color);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawLines(lines);
}

void CurveSquare::initPegsArray()
{
    Peg::setSize(_size);
    Peg pegStart(0, _size);
    Peg pegEnd(_size, 0);

    _pegsIntensity.clear();
    _pegsIntensity.append(pegStart);
    _pegsIntensity.append(pegEnd);

    _pegsRed.clear();
    _pegsRed.append(pegStart);
    _pegsRed.append(pegEnd);

    _pegsGreen.clear();
    _pegsGreen.append(pegStart);
    _pegsGreen.append(pegEnd);

    _pegsBlue.clear();
    _pegsBlue.append(pegStart);
    _pegsBlue.append(pegEnd);

    _activePegs = &_pegsIntensity;
}

// Calculate the coordinates on the control according to the x and y values
QPoint CurveSquare::getCoordinate(int x, int y)
{
    return QPoint(x + _rectSquare.left(), _size - y + _rectSquare.top());
}

void CurveSquare::mousePressEvent(QMouseEvent* event)
{
    QPoint point = event->pos();
    if (event->button() == Qt::LeftButton)
    {
        if (_rectSquare.contains(point))
        {
            setCursor(Qt::SizeAllCursor);

            QPoint ptDummy(point.x() - _rectSquare.left(), point.y() - _rectSquare.top());
            _activePegIndex = ptInAnyPeg(ptDummy);
            if (_activePegIndex == NONE_PEG)
            {
                // The mouse point is not on any peg
                if (prepareAddPeg(ptDummy.x()))
                {
                    // Add new peg
                    _activePegIndex = addPeg(ptDummy);
                    // Change m_Array value
                    calcArrayValue(_activePegIndex, true);
                    repaintPeg();
                }
            }
        }
    }
    else if(event->button() & Qt::RightButton)
    {
        PegArray& pegs = *_activePegs;
        if (_activePegIndex != NONE_PEG)
        {
            // Refresh the black spot where the original m_nActivePegIndex is located
            QRect rect(pegs[_activePegIndex].x() - 2, pegs[_activePegIndex].y() - 2,
                       pegs[_activePegIndex].x() + 2, pegs[_activePegIndex].y() + 2);
            repaint(rect);
        }

        QPoint ptDummy(point.x() - _rectSquare.left(), point.y() - _rectSquare.top());
        _activePegIndex = ptInAnyPeg(ptDummy);
        // The number of pegs is at least three before the delete operation is allowed
        if (_activePegIndex != NONE_PEG && pegs.size() >= 3)
        {
            removePeg(_activePegIndex);
            calcArrayValue(_activePegIndex, false);
            repaintPeg();
            if (_activePegIndex > pegs.size() - 1)
            {
                _activePegIndex--;
            }
        }
    }
}

void CurveSquare::mouseMoveEvent(QMouseEvent* event)
{
    QPoint point = event->pos();
    QPoint ptDummy(point.x() - _rectSquare.left(), point.y() - _rectSquare.top());

    QRect rect(_rectSquare.left(), _rectSquare.top(), _rectSquare.width() + 1, _rectSquare.height() + 1);
    if (rect.contains(point))
    {
        // Display input&output value of current position
        QSize size = getCurrentMouseValue(point);
        QString input = QString("%1").arg(size.width());
        QString output = QString("%1").arg(size.height());
        emit updateLabelText(input, output);

        if (ptInAnyPeg(ptDummy) != NONE_PEG)
        {
            setCursor(Qt::SizeAllCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
    else
    {
        // Point is out of _rectSquare
        emit updateLabelText("", "");
    }

    if ((event->buttons() & Qt::LeftButton) == false)
        return;

    // Not allowed to pull peg out of m_rectSquare
    if (!_rectSquare.contains(point))
        return;

    PegArray& pegs = *_activePegs;
    // Deal with the first and last pegs
    if (_activePegIndex == 0)
    {
        if (ptDummy.x() < pegs[1].x() - PEG_DISTANCE)
        {
            pegs[0].setPoint(ptDummy);
            repaintPeg();
            calcArrayValue(_activePegIndex, true);
        }
    }
    else if (_activePegIndex == pegs.size() - 1)
    {
        if (ptDummy.x() > pegs[_activePegIndex - 1].x() + PEG_DISTANCE)
        {
            pegs[_activePegIndex].setPoint(ptDummy);
            repaintPeg();
            calcArrayValue(_activePegIndex, true);
        }
    }
    else if (_activePegIndex != -1)
    {
        if ((ptDummy.x() > pegs[_activePegIndex - 1].x() + PEG_DISTANCE) &&
                (ptDummy.x() < pegs[_activePegIndex + 1].x() - PEG_DISTANCE))
        {
            int flag = 0;           // Whether a peg merge event has occurred

            pegs[_activePegIndex].setPoint(ptDummy);

            ////////////////////////////// Merge pegs ////////////////////////////////
            // Merge with previous peg
            if ((pegs[_activePegIndex].x() - pegs[_activePegIndex - 1].x() < 12) &&
                    (abs(pegs[_activePegIndex].y() - pegs[_activePegIndex - 1].y()) < 12))
            {
                removePeg(_activePegIndex);
                flag = 1;
            }
            // Merge with next peg
            else if ((pegs[_activePegIndex + 1].x() - pegs[_activePegIndex].x() < 12) &&
                     (abs(pegs[_activePegIndex + 1].y() - pegs[_activePegIndex].y()) < 12))
            {
                removePeg(_activePegIndex);
                flag = 2;
            }

            repaintPeg();

            calcArrayValue(_activePegIndex, true);
            if (flag)
            {
                // nFlag==1 : minus 1, nFlag==2 : minus 0
                _activePegIndex -= (2 - flag);
                return;
            }
        }
    }
}

void CurveSquare::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);

    emit updateImage();
}

int CurveSquare::ptInAnyPeg(QPoint point) const
{
    for (int i = 0; i < _activePegs->size(); i++)
    {
        if (_activePegs->at(i).rect().contains(point))
        {
            return i;
        }
    }
    return NONE_PEG;
}

// Preprocess before adding new peg
bool CurveSquare::prepareAddPeg(int xCoordinate)
{
    PegArray& pegs = *_activePegs;
    int i = 0, leftDis = 0, rightDis = 0;

    if (xCoordinate < pegs[0].x())
    {
        leftDis = pegs[0].x() - xCoordinate;
    }
    else
    {
        while (i < pegs.size() && pegs[i].x() < xCoordinate)
        {
            leftDis = xCoordinate - pegs[i].x();
            i++;
        }
    }

    if (i == pegs.size())
    {
        rightDis = xCoordinate - pegs[i - 1].x();
    }
    else
    {
        rightDis = pegs[i].x() - xCoordinate;
    }
    if (leftDis < PEG_DISTANCE || rightDis < PEG_DISTANCE)
    {
        return false;
    }

    return true;
}

int CurveSquare::addPeg(const QPoint& point)
{
    Peg peg(point);
    _activePegs->append(peg);
    // Sort according to the coordinate of peg
    return sortPegs(peg);
}

// Repaint
void CurveSquare::repaintPeg()
{
    QRect rect(_rectSquare);
    rect.adjust(-2, -2, 3, 3);
    repaint(rect);
}

int CurveSquare::sortPegs(const Peg& peg)
{
    PegArray& pegs = *_activePegs;
    if (pegs.size() == 2)
        return 0;

    // i从0开始循环的原因是有可能在首节点之前插入peg
    for (int i = 0; i < pegs.size() - 1; i++)
    {
        if (peg.x() < pegs[i].x())
        {
            // 刚插入的peg的横坐标不是最大，则进行排序
            for (int j = pegs.size() - 2; j > i - 1; j--)
            {
                pegs[j + 1] = pegs[j];
            }
            pegs[i] = peg;
            return i;
        }
    }
    return pegs.size() - 1;
}

void CurveSquare::calcArrayValue()
{
    if (_curveOrLinear)
    {
        // Curve
        setCurveArrayValue();
    }
    else
    {
        // Linear
        for (int i = 0; i < _activePegs->size(); i++)
        {
            setLinearArrayValue(i);
        }
    }
}

void CurveSquare::calcArrayValue(int index, bool flag)
{
    if (index == NONE_PEG)
        return;

    if (_curveOrLinear)
    {
        // Curve
        setCurveArrayValue();
    }
    else
    {
        // Linear
        // Calculator array value in modified interval
        setLinearArrayValue(index, flag);
    }
}

// Use piecewise linear function to set array value
void CurveSquare::setLinearArrayValue(int index, bool flag)
{
    if (index == 0)
    {
        QSize size = getCurrentValue(index);
        // The values before the first peg are set to be the same as its cy
        for (int i = 0; i <= size.width(); i++)
        {
            _activeArray[i] = size.height();
        }
        // Update array value between first peg and second peg
        setLinearValue(index);
    }
    else if (index > _activePegs->size() - 1 && !flag)
    {
        QSize size = getCurrentValue(index - 1);
        for (int i = _arraySize - 1; i >= size.width(); i--)
        {
            _activeArray[i] = size.height();
        }
    }
    else if (index == _activePegs->size() - 1)
    {
        if (flag)
        {
            QSize size = getCurrentValue(index);
            for (int i = _arraySize - 1; i >= size.width(); i--)
            {
                _activeArray[i] = size.height();
            }
            setLinearValue(index - 1);
        }
        else
        {
            // When deleting the penultimate peg
            setLinearValue(index - 1);
        }
    }
    else
    {
        if (flag)
        {
            setLinearValue(index - 1);
            setLinearValue(index);
        }
        else
        {
            // When deleting peg
            setLinearValue(index - 1);
        }
    }
}

// Get input&output value of current peg
QSize CurveSquare::getCurrentValue(int index)
{
    QPoint point(_activePegs->at(index).point());
    return QSize(point.x(), _size - point.y());
}

// Get input&output value of current position
QSize CurveSquare::getCurrentMouseValue(const QPoint& point)
{
    return QSize((point.x() - _rectSquare.left()) * 255 / _size, 255 - (point.y() - _rectSquare.top()) * 255 / _size);
}

// This function is only called by setLinearArrayValue()
void CurveSquare::setLinearValue(int startIndex)
{
    if (startIndex < 0 || startIndex > _activePegs->size() - 2)
        return;

    QSize sizeStart = getCurrentValue(startIndex);
    QSize sizeEnd = getCurrentValue(startIndex + 1);

    for (int x = sizeStart.width(); x < sizeEnd.width(); x++)
    {
        _activeArray[x] = sizeStart.height() + (x - sizeStart.width()) * (sizeEnd.height() - sizeStart.height()) / (sizeEnd.width() - sizeStart.width());
    }
}

// Use spline function to set array value
void CurveSquare::setCurveArrayValue()
{
    int num = int(_activePegs->size());
    if (num == 2)
    {
        // Don't use spline within two pegs
        setLinearArrayValue(0, true);
        setLinearArrayValue(1, true);
        return;
    }
    float* x = new float[num];
    float* y = new float[num];
    for (int i = 0; i < num; i++)
    {
        *(x + i) = (float)getCurrentValue(i).width();
        *(y + i) = (float)getCurrentValue(i).height();
    }

    // m is the range of points to be interpolated
    int m = getCurrentValue(num - 1).width() - getCurrentValue(0).width() - 1;
    float* t = new float[m];
    for (int i = 0; i < m; i++)
    {
        *(t + i) = (float)getCurrentValue(0).width() + 1 + i;
    }
    float* z = new float[m];
    spline(x, y, num, t, m, z);

    // Calculate result
    for (int i = 0; i < m; i++)
    {
        if (*(z + i) < 0)
        {
            *(z + i) = 1;
        }
        else if (*(z + i) > _size)
        {
            *(z + i) = _size - 1;
        }
        _activeArray[getCurrentValue(0).width() + 1 + i] = uint(*(z + i));
    }
    // Deal with these points before the first peg and after the last peg
    for (int i = 0; i < getCurrentValue(0).width() + 1; i++)
    {
        _activeArray[i] = (uint)getCurrentValue(0).height();
    }
    for (int i = 0; getCurrentValue(num - 1).width() + i < _arraySize; i++)
    {
        _activeArray[getCurrentValue(num - 1).width() + i] = (uint)getCurrentValue(num - 1).height();
    }
    delete[] x;
    delete[] y;
    delete[] t;
    delete[] z;
}

// Remove one peg
void CurveSquare::removePeg(int index)
{
    _activePegs->removeAt(index);
}

// Remove all pegs
void CurveSquare::removeAllPegs()
{
    _activePegs->clear();

    _activePegs->append(Peg(0, _size));
    _activePegs->append(Peg(_size, 0));
    _activePegIndex = 0;
    // Initialize array
    for (int i = 0; i < _arraySize; i++)
    {
        _activeArray[i] = i;
    }
}

// Generate histogram
bool CurveSquare::generateHistogram(const uint* pArray)
{
    for (int i = 0; i < _size - 1; i++)
    {
        float fIndex = i * 255.0f / float(_size - 1);
        int index = floor(fIndex);
        float fraction = fIndex - index;
        _heightArray[i] = (int)round(pArray[index] * (1.0f - fraction) + pArray[index + 1] * fraction);
    }
    _heightArray[_size - 1] = pArray[255];

    _minHeight = _maxHeight = _heightArray[0];
    // find max and min value in _heightArray
    for (int i = 1; i < _size; i++)
    {
        if (_maxHeight < _heightArray[i])
        {
            _maxHeight = _heightArray[i];
        }
        if (_minHeight > _heightArray[i])
        {
            _minHeight = _heightArray[i];
        }
    }
    // In case all pixels value are zero
    if (_minHeight == 0 && _maxHeight == 0)
    {
        repaint();
        return false;
    }

    // Calculate height
    for (int i = 0; i < _size; i++)
    {
        _heightArray[i] = (_heightArray[i] - _minHeight) * (_size - 1) / (_maxHeight - _minHeight) + 1;
    }

    repaint();

    return true;
}

void CurveSquare::setChannel(int channel)
{
    if (_channel == channel)
        return;

    int oldChannel = _channel;
    _channel = channel;

    switch (_channel)
    {
    case CURVE_CHANNEL_GRAY:
        _activePegs = &_pegsIntensity;
        _activeArray = _arrayIntensity;
        break;
    case CURVE_CHANNEL_RED:
        _activePegs = &_pegsRed;
        _activeArray = _arrayRed;
        break;
    case CURVE_CHANNEL_GREEN:
        _activePegs = &_pegsGreen;
        _activeArray = _arrayGreen;
        break;
    case CURVE_CHANNEL_BLUE:
        _activePegs = &_pegsBlue;
        _activeArray = _arrayBlue;
        break;
    }

    _activePegIndex = 0;

    if (oldChannel == 0)
    {
        // Assign the same value to RGB channels
        _pegsRed = _pegsIntensity;
        _pegsGreen = _pegsIntensity;
        _pegsBlue = _pegsIntensity;

        memcpy(_arrayRed, _arrayIntensity, sizeof(uint) * _arraySize);
        memcpy(_arrayGreen, _arrayIntensity, sizeof(uint) * _arraySize);
        memcpy(_arrayBlue, _arrayIntensity, sizeof(uint) * _arraySize);
    }

    repaint();
}

void CurveSquare::setCurveOrLinear(bool curveOrLinear)
{
    _curveOrLinear = curveOrLinear;

    calcArrayValue();
    repaintPeg();

    emit updateImage();
}
