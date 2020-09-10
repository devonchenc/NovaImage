#include "ProcessorWidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QEvent>

#include "../Processor/CommonProcessor.h"
#include "../Core/GlobalFunc.h"

ProcessorWidget::ProcessorWidget(QWidget* parent)
    : BaseWidget(parent)
{
    setName(tr("Image Processing"));

    _applyButton = new QPushButton(tr("&Apply"));

    _vLayout = new QVBoxLayout;
    _vLayout->addStretch();
    _vLayout->addWidget(_applyButton);

    setLayout(_vLayout);
}

ProcessorWidget::~ProcessorWidget()
{

}

void ProcessorWidget::setProcessorWidget(QWidget* processorWidget)
{
    QWidgetItem* item = dynamic_cast<QWidgetItem*>(_vLayout->itemAt(0));
    if (item && item->widget())
    {
        delete item->widget();
    }

    _vLayout->insertWidget(0, processorWidget);
}

void ProcessorWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        parentWidget()->setWindowTitle(tr("Image Processing"));

        _applyButton->setText(tr("&Apply"));
    }
    BaseWidget::changeEvent(event);
}
