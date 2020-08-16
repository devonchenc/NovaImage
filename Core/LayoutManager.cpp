#include "LayoutManager.h"

#include <QWidget>
#include <QGridLayout>

#include "GlobalFunc.h"
#include "Document.h"
#include "View.h"

LayoutManager::LayoutManager(QWidget* parent)
    : QObject(parent)
    , _parentWidget(parent)
    , _topView(nullptr)
    , _frontalView(nullptr)
    , _profileView(nullptr)
    , _volumeView(nullptr)
    , _gridLayout(new QGridLayout)
{

}

void LayoutManager::setWidget(QWidget* top, QWidget* frontal, QWidget* profile, QWidget* volume)
{
    _topView = top;
    _frontalView = frontal;
    _profileView = profile;
    _volumeView = volume;

    _parentWidget->setLayout(_gridLayout);
}

void LayoutManager::oneView()
{
    removeWidget();
    _frontalView->hide();
    _profileView->hide();
    _volumeView->hide();

    _gridLayout->addWidget(_topView, 0, 0);

    _gridLayout->setRowStretch(0, 1);
    _gridLayout->setRowStretch(1, 0);
    _gridLayout->setRowStretch(2, 0);

    _gridLayout->setColumnStretch(0, 1);
    _gridLayout->setColumnStretch(1, 0);
}

void LayoutManager::threeView()
{
    removeWidget();
    _frontalView->show();
    _profileView->show();
    _volumeView->show();

    _gridLayout->addWidget(_topView, 0, 0);
    _gridLayout->addWidget(_frontalView, 0, 1);
    _gridLayout->addWidget(_profileView, 1, 0);
    _gridLayout->addWidget(_volumeView, 1, 1);

    _gridLayout->setRowStretch(0, 1);
    _gridLayout->setRowStretch(1, 1);
    _gridLayout->setRowStretch(2, 0);

    _gridLayout->setColumnStretch(0, 1);
    _gridLayout->setColumnStretch(1, 1);
}

void LayoutManager::volumeView()
{
    removeWidget();
    _frontalView->show();
    _profileView->show();
    _volumeView->show();

    _gridLayout->addWidget(_topView, 0, 0);
    _gridLayout->addWidget(_frontalView, 1, 0);
    _gridLayout->addWidget(_profileView, 2, 0);
    _gridLayout->addWidget(_volumeView, 0, 1, 3, 1);

    _gridLayout->setRowStretch(0, 1);
    _gridLayout->setRowStretch(1, 1);
    _gridLayout->setRowStretch(2, 1);

    _gridLayout->setColumnStretch(0, 1);
    _gridLayout->setColumnStretch(1, 3);
}

void LayoutManager::removeWidget()
{
    for (int i = (_gridLayout->count() - 1); i >= 0; --i)
    {
        QLayoutItem* item = _gridLayout->takeAt(i);
        if (item)
        {
            _gridLayout->removeWidget(item->widget());
        }
    }
}