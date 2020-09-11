#include "ProcessorShellWidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QEvent>
#include <QApplication>

#include "../Processor/CommonProcessor.h"
#include "../Core/GlobalFunc.h"
#include "CustomEvent.h"

ProcessorShellWidget::ProcessorShellWidget(QWidget* parent)
    : BaseWidget(parent)
{
    setName(tr("Image Processing"));

    _applyButton = new QPushButton(tr("&Apply"));
    connect(_applyButton, &QPushButton::clicked, this, &ProcessorShellWidget::apply);

    _vLayout = new QVBoxLayout;
    _vLayout->addStretch();
    _vLayout->addWidget(_applyButton);

    setLayout(_vLayout);
}

ProcessorShellWidget::~ProcessorShellWidget()
{

}

void ProcessorShellWidget::setProcessorWidget(QWidget* processorWidget)
{
    QWidgetItem* item = dynamic_cast<QWidgetItem*>(_vLayout->itemAt(0));
    if (item && item->widget())
    {
        delete item->widget();
    }

    _vLayout->insertWidget(0, processorWidget);
}

void ProcessorShellWidget::apply()
{
    QWidget* widget = qobject_cast<QWidget*>(_vLayout->itemAt(0)->widget());
    if (widget)
    {
        QApplication::postEvent(widget, new CustomEvent);
    }
}

void ProcessorShellWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        parentWidget()->setWindowTitle(tr("Image Processing"));

        _applyButton->setText(tr("&Apply"));
    }
    BaseWidget::changeEvent(event);
}
