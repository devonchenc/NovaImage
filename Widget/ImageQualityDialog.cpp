#include "ImageQualityDialog.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QInputDialog>

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
    QMenuBar* menuBar = new QMenuBar(this);
    QMenu* menu = menuBar->addMenu(tr("&Settings"));
    QAction* widthAction = menu->addAction(tr("&Line Width"));
    connect(widthAction, &QAction::triggered, this, &ImageQualityDialog::setLineWidth);

    _chartView = new ChartView;

    QVBoxLayout* globalLayout = new QVBoxLayout;
    globalLayout->setMenuBar(menuBar);
    globalLayout->addWidget(_chartView);
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
        getGlobalActiveView()->repaint();
        emit lineWidthChanged(_imageQualityItem, lineWidth);
    }
}

