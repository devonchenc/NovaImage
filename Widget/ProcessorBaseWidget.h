#pragma once

#include "BaseWidget.h"

class BaseProcessor;

class ProcessorBaseWidget : public BaseWidget
{
    Q_OBJECT

public:
    ProcessorBaseWidget(BaseProcessor* processor, QWidget* parent = nullptr);
    virtual ~ProcessorBaseWidget() {}

protected:
    virtual void customEvent(QEvent* event);

protected:
    BaseProcessor* _processor;
};
