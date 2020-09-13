#include "ProcessorBaseWidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QEvent>
#include <QApplication>

#include "../Processor/BaseProcessor.h"
#include "CustomEvent.h"

ProcessorBaseWidget::ProcessorBaseWidget(BaseProcessor* processor, QWidget* parent)
    : BaseWidget(parent)
    , _processor(processor)
{

}

ProcessorBaseWidget::~ProcessorBaseWidget()
{

}

void ProcessorBaseWidget::customEvent(QEvent* event)
{
    Q_UNUSED(event);
    _processor->apply();
}