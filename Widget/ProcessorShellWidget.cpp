#include "ProcessorShellWidget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QEvent>
#include <QApplication>

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
