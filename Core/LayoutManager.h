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
    void setWidget(QWidget* axial, QWidget* coronal, QWidget* sagittal, QWidget* volume);

    void singleView(int viewType);
    void threeView();
    void volumeView();

private:
    void removeWidget();

private:
    QWidget* _parentWidget;

    QWidget* _axialView;
    QWidget* _coronalView;
    QWidget* _sagittalView;
    QWidget* _volumeView;

    QGridLayout* _gridLayout;
};
