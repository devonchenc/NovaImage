#include "LayoutManager.h"

#include <QWidget>
#include <QFrame>
#include <QGridLayout>

LayoutManager::LayoutManager(QWidget* parent)
    : QObject(parent)
    , _parentWidget(parent)
    , _topView(nullptr)
    , _frontalView(nullptr)
    , _profileView(nullptr)
    , _volumeView(nullptr)
{

}

void LayoutManager::setWidget(QFrame* top, QFrame* frontal, QFrame* profile, QFrame* volume)
{
    _topView = top;
    _frontalView = frontal;
    _profileView = profile;
    _volumeView = volume;
}

void LayoutManager::oneView()
{
    QLayout* layout = _parentWidget->layout();
    if (layout)
        delete layout;

    _frontalView->hide();
    _profileView->hide();
    _volumeView->hide();

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(_topView);
    _parentWidget->setLayout(vLayout);
}

void LayoutManager::threeView()
{
    QLayout* layout = _parentWidget->layout();
    if (layout)
        delete layout;

    _frontalView->show();
    _profileView->show();
    _volumeView->show();

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget(_topView, 0, 0);
    gridLayout->addWidget(_frontalView, 0, 1);
    gridLayout->addWidget(_profileView, 1, 0);
    gridLayout->addWidget(_volumeView, 1, 1);
    _parentWidget->setLayout(gridLayout);
}
