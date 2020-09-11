#pragma once

#include "BaseWidget.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

class ProcessorShellWidget : public BaseWidget
{
    Q_OBJECT

public:
    ProcessorShellWidget(QWidget* parent = nullptr);
    virtual ~ProcessorShellWidget();

    void setProcessorWidget(QWidget* processorWidget);

public slots:
    void apply();

protected:
    void changeEvent(QEvent* event) override;

private:
    QVBoxLayout* _vLayout;
    QPushButton* _applyButton;
};
