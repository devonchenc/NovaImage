#pragma once

#include "BaseWidget.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

class ProcessorWidget : public BaseWidget
{
    Q_OBJECT

public:
    ProcessorWidget(QWidget* parent = nullptr);
    virtual ~ProcessorWidget();

    void setProcessorWidget(QWidget* processorWidget);

protected:
    void changeEvent(QEvent* event) override;

private:
    QVBoxLayout* _vLayout;
    QPushButton* _applyButton;
};
