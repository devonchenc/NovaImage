#include "ImageQualityDialog.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QInputDialog>
#include <QLabel>

#include "ChartView.h"
#include "../Diagram/DiagramImageQualityItem.h"
#include "../Core/GlobalFunc.h"
#include "../Core/View.h"

ImageQualityDialog::ImageQualityDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Image Quality"));

    setWindowFlag(Qt::Popup);

    initUI();

    resize(500, 300);

    setStyleSheet("background-color:none");
}

ImageQualityDialog::~ImageQualityDialog()
{
    if (_chartView)
    {
        delete _chartView;
        _chartView = nullptr;
    }
}

void ImageQualityDialog::setData(QGraphicsLineItem* lineItem, const QVector<qreal>& points)
{
    _imageQualityItem = qgraphicsitem_cast<DiagramImageQualityItem*>(lineItem);
    connect(_imageQualityItem, &DiagramImageQualityItem::itemDeleted, this, &ImageQualityDialog::close);

    if (!_chartView->hasValidData())
    {
        _chartView->setData(points);
    }
    else
    {
        // Update existing plot
        _chartView->updateData(points);
    }
}

void ImageQualityDialog::initUI()
{
    QLabel* aLabel = new QLabel("A:");
    _aValueLabel = new QLabel;
    _aValueLabel->setMaximumWidth(20);
    QLabel* bLabel = new QLabel("B:");
    _bValueLabel = new QLabel;
    _bValueLabel->setMaximumWidth(20);
    QLabel* cLabel = new QLabel("C:");
    _cValueLabel = new QLabel;
    _cValueLabel->setMaximumWidth(20);
    QLabel* resultLabel = new QLabel("Image Quality:");
    _resultLabel = new QLabel;
    _resultLabel->setMaximumWidth(20);

    QGridLayout* grid = new QGridLayout;
    grid->addWidget(aLabel, 0, 0);
    grid->addWidget(_aValueLabel, 0, 1);
    grid->addWidget(bLabel, 1, 0);
    grid->addWidget(_bValueLabel, 1, 1);
    grid->addWidget(cLabel, 2, 0);
    grid->addWidget(_cValueLabel, 2, 1);
    grid->addWidget(resultLabel, 3, 0);
    grid->addWidget(_resultLabel, 3, 1);

    _chartView = new ChartView2;
    QHBoxLayout* globalLayout = new QHBoxLayout;
    globalLayout->addWidget(_chartView);
    globalLayout->addLayout(grid);
    setLayout(globalLayout);
}

void ImageQualityDialog::setLineWidth()
{
    bool ok;
    int lineWidth = QInputDialog::getInt(this, tr("Line Width"), tr("Width:"),
        _imageQualityItem->width(), 1, 300, 1, &ok);
    if (ok)
    {
        _imageQualityItem->setWidth(lineWidth);
        _imageQualityItem->scene()->update();
        emit lineWidthChanged(_imageQualityItem, lineWidth);
    }
}

