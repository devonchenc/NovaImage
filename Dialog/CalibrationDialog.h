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

private:
    void initUI();

    void loadSettings();

    void getCurrentLineInfo();

private slots:
    void acceptButtonClicked();

private:
    QLabel* _systemSizeLabel;
    QLabel* _manualSize1Label;
    QLineEdit* _pixelSizeEdit;
    QLabel* _manualSize2Label;
    QLabel* _startPositionLabel;
    QLineEdit* _startPositionEdit;
    QLabel* _endPositionLabel;
    QLineEdit* _endPositionEdit;
    QLabel* _pixelLength1Label;
    QLineEdit* _pixelLengthEdit;
    QLabel* _pixelLength2Label;
    QLabel* _actualLength1Label;
    QLineEdit* _actualLengthEdit;
    QLabel* _actualLength2Label;
    QCheckBox* _setAsSystemLabel;
};
