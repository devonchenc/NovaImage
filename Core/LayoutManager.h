#pragma once

#include <QObject>

QT_BEGIN_NAMESPACE
class QWidget;
class QFrame;
class QGridLayout;
class QVBoxLayout;
QT_END_NAMESPACE

class LayoutManager : public QObject
{
    Q_OBJECT

public:
    LayoutManager(QWidget* parent);

public:
    void setWidget(QFrame* top, QFrame* frontal, QFrame* profile, QFrame* volume);

    void oneView();

    void threeView();

private:
    QWidget* _parentWidget;

    QFrame* _topView;
    QFrame* _frontalView;
    QFrame* _profileView;
    QFrame* _volumeView;
};
