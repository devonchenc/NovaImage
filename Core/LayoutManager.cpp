#include "LayoutManager.h"

#include <QWidget>
#include <QGridLayout>

#include "GlobalFunc.h"
#include "Document.h"
#include "View.h"

LayoutManager::LayoutManager(QWidget* parent)
    : QObject(parent)
    , _parentWidget(parent)
    , _axialView(nullptr)
    , _coronalView(nullptr)
    , _sagittalView(nullptr)
    , _volumeView(nullptr)
    , _gridLayout(new QGridLayout)
{
    _gridLayout->setContentsMargins(0, 0, 0, 0);
}

void LayoutManager::setWidget(QWidget* axial, QWidget* coronal, QWidget* sagittal, QWidget* volume)
{
    _axialView = axial;
    _coronalView = coronal;
    _sagittalView = sagittal;
    _volumeView = volume;

    _parentWidget->setLayout(_gridLayout);
}

void LayoutManager::singleView(int viewType)
{
    removeWidget();

    _axialView->hide();
    _coronalView->hide();
    _sagittalView->hide();
    _volumeView->hide();

    if (viewType == 0)
    {
        _axialView->show();
        _gridLayout->addWidget(_axialView, 0, 0);
    }
    else if (viewType == 1)
    {
        _coronalView->show();
        _gridLayout->addWidget(_coronalView, 0, 0);
    }
    else if (viewType == 2)
    {
        _sagittalView->show();
        _gridLayout->addWidget(_sagittalView, 0, 0);
    }
    else if (viewType == 3)
    {
        _volumeView->show();
        _gridLayout->addWidget(_volumeView, 0, 0);
    }

    _gridLayout->setRowStretch(0, 1);
    _gridLayout->setRowStretch(1, 0);
    _gridLayout->setRowStretch(2, 0);

    _gridLayout->setColumnStretch(0, 1);
    _gridLayout->setColumnStretch(1, 0);

    _gridLayout->setSpacing(2);
}

void LayoutManager::threeView()
{
    removeWidget();

    _axialView->show();
    _coronalView->show();
    _sagittalView->show();
    _volumeView->hide();

    _gridLayout->addWidget(_axialView, 0, 0, 2, 1);
    _gridLayout->addWidget(_coronalView, 0, 1);
    _gridLayout->addWidget(_sagittalView, 1, 1);

    _gridLayout->setRowStretch(0, 1);
    _gridLayout->setRowStretch(1, 1);
    _gridLayout->setRowStretch(2, 0);

    _gridLayout->setColumnStretch(0, 1);
    _gridLayout->setColumnStretch(1, 1);

    _gridLayout->setSpacing(2);
}

void LayoutManager::fourView()
{
    removeWidget();

    _axialView->show();
    _coronalView->show();
    _sagittalView->show();
    _volumeView->show();

    _gridLayout->addWidget(_axialView, 0, 0);
    _gridLayout->addWidget(_coronalView, 0, 1);
    _gridLayout->addWidget(_sagittalView, 1, 0);
    _gridLayout->addWidget(_volumeView, 1, 1);

    _gridLayout->setRowStretch(0, 1);
    _gridLayout->setRowStretch(1, 1);
    _gridLayout->setRowStretch(2, 0);

    _gridLayout->setColumnStretch(0, 1);
    _gridLayout->setColumnStretch(1, 1);

    _gridLayout->setSpacing(2);
}

void LayoutManager::volumeView()
{
    removeWidget();

    _axialView->show();
    _coronalView->show();
    _sagittalView->show();
    _volumeView->show();

    _gridLayout->addWidget(_axialView, 0, 0);
    _gridLayout->addWidget(_coronalView, 1, 0);
    _gridLayout->addWidget(_sagittalView, 2, 0);
    _gridLayout->addWidget(_volumeView, 0, 1, 3, 1);

    _gridLayout->setRowStretch(0, 1);
    _gridLayout->setRowStretch(1, 1);
    _gridLayout->setRowStretch(2, 1);

    _gridLayout->setColumnStretch(0, 1);
    _gridLayout->setColumnStretch(1, 3);

    _gridLayout->setSpacing(2);
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