#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QCheckBox;
QT_END_NAMESPACE

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    CalibrationDialog(QWidget* parent = nullptr);

public:


private:
    void initUI();

private slots:
    void acceptButtonClicked();

private:
    QLabel* _systemSizeLabel;
    QLabel* _manualSize1Label;
    QLineEdit* _pixelSizeEdit;
    QLabel* _manualSize2Label;
    QCheckBox* _setAsSystemLabel;
};
