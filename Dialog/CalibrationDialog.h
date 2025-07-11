#pragma once

#include <QDialog>

#include "../Diagram/DiagramLengthItem.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QButtonGroup;
class QLabel;
class QLineEdit;
class QCheckBox;
QT_END_NAMESPACE

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    CalibrationDialog(QWidget* parent = nullptr);

public slots:
    void lengthItemSelected(const DiagramLengthItem* item);

private:
    void initUI();

    void loadSettings();

    void getCurrentLineInfo();

    void setWidgetText(const DiagramLengthItem* item);

private slots:
    void acceptButtonClicked();

private:
    QCheckBox* _enableCheckBox;
    QButtonGroup* _radioGroup;
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
