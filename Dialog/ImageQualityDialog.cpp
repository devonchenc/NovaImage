#include "ImageQualityDialog.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMenuBar>
#include <QInputDialog>
#include <QLabel>

#include "../Widget/ImageQualityChartView.h"
#include "../Diagram/DiagramImageQualityItem.h"
#include "../Core/GlobalFunc.h"
#include "../Core/View.h"

ImageQualityDialog::ImageQualityDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Image Quality"));

    setWindowFlag(Qt::Popup);

    initUI();

    resize(700, 300);

    setStyleSheet("background-color:none");
}

ImageQualityDialog::~ImageQualityDialog()
{

}

void ImageQualityDialog::setData(QGraphicsLineItem* lineItem, const QVector<qreal>& points)
{
    _imageQualityItem = qgraphicsitem_cast<DiagramImageQualityItem*>(lineItem);
    connect(_imageQualityItem, &DiagramImageQualityItem::itemDeleted, this, &ImageQualityDialog::close);
    connect(_chartView, &ImageQualityChartView::leftRatioChanged, _imageQualityItem, &DiagramImageQualityItem::setLeftRatio);
    connect(_chartView, &ImageQualityChartView::rightRatioChanged, _imageQualityItem, &DiagramImageQualityItem::setRightRatio);

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

void ImageQualityDialog::setRatio(float leftRatio, float rightRatio)
{
    _chartView->setRatio(leftRatio, rightRatio);
}

void ImageQualityDialog::updateResult(qreal AHeight, qreal BHeight, qreal CHeight, qreal quality)
{
    _aValueLabel->setText(QString::number(AHeight, 'f', 2));
    _bValueLabel->setText(QString::number(BHeight, 'f', 2));
    _cValueLabel->setText(QString::number(CHeight, 'f', 2));
    _resultLabel->setText(QString::number(quality, 'f', 2) + "%");
}

void ImageQualityDialog::initUI()
{
    QLabel* aLabel = new QLabel("A:");
    aLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    _aValueLabel = new QLabel;
    _aValueLabel->setFixedWidth(40);
    _aValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QLabel* bLabel = new QLabel("B:");
    bLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    _bValueLabel = new QLabel;
    _bValueLabel->setFixedWidth(40);
    _bValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QLabel* cLabel = new QLabel("C:");
    cLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    _cValueLabel = new QLabel;
    _cValueLabel->setFixedWidth(40);
    _cValueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QLabel* resultLabel = new QLabel(tr("Quality:"));
    _resultLabel = new QLabel;
    _resultLabel->setFixedWidth(40);
    _resultLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QGridLayout* grid = new QGridLayout;
    grid->addWidget(aLabel, 0, 0);
    grid->addWidget(_aValueLabel, 0, 1);
    grid->addWidget(bLabel, 1, 0);
    grid->addWidget(_bValueLabel, 1, 1);
    grid->addWidget(cLabel, 2, 0);
    grid->addWidget(_cValueLabel, 2, 1);
    grid->addWidget(resultLabel, 3, 0);
    grid->addWidget(_resultLabel, 3, 1);

    _chartView = new ImageQualityChartView(this);
    connect(_chartView, &ImageQualityChartView::sendResult, this, &ImageQualityDialog::updateResult);

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

