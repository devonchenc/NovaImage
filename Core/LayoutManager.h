#pragma once

#include <QObject>

QT_BEGIN_NAMESPACE
class QWidget;
class QGridLayout;
class QVBoxLayout;
QT_END_NAMESPACE

class LayoutManager : public QObject
{
    Q_OBJECT

public:
    LayoutManager(QWidget* parent);

public:
    void setWidget(QWidget* top, QWidget* frontal, QWidget* profile, QWidget* volume);

    void oneView();
    void threeView();
    void volumeView();

private:
    void removeWidget();

private:
    QWidget* _parentWidget;

    QWidget* _topView;
    QWidget* _frontalView;
    QWidget* _profileView;
    QWidget* _volumeView;

    QGridLayout* _gridLayout;
};
