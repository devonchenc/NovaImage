#pragma once

#include "BaseWidget.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

class BaseProcessor;

class ProcessorBaseWidget : public BaseWidget
{
    Q_OBJECT

public:
    ProcessorBaseWidget(BaseProcessor* processor, QWidget* parent = nullptr);
    virtual ~ProcessorBaseWidget();

protected:
    virtual void customEvent(QEvent* event);

protected:
    BaseProcessor* _processor;
};
