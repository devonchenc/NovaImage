#include "ProcessorWidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QEvent>
#include <QCustomEvent>
#include <QApplication>

#include "../Processor/CommonProcessor.h"
#include "../Core/GlobalFunc.h"

ProcessorWidget::ProcessorWidget(QWidget* parent)
    : BaseWidget(parent)
{
    setName(tr("Image Processing"));

    _applyButton = new QPushButton(tr("&Apply"));
    connect(_applyButton, &QPushButton::clicked, this, &ProcessorWidget::apply);

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

void ProcessorWidget::apply()
{
    QWidget* widget = qobject_cast<QWidget*>(_vLayout->itemAt(0)->widget());
    if (widget)
    {
        // TODO
     //   widget->accept();
        const QEvent::Type MyEvent = (QEvent::Type)9393;
        QApplication::postEvent(widget, new QCustomEvent(MyEvent));
    }
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
