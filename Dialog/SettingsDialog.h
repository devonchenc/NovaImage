#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class QButtonGroup;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget* parent = nullptr);

signals:
    void changeLanguage(int language);

private:
    void initUI();

    QWidget* createGeneralWidget();

    QWidget* createImageWidget();

    QWidget* createCalibrationWidget();

    void loadSettings();

private slots:
    void acceptButtonClicked();

private:
    QComboBox* _languageComboBox;
    QComboBox* _leftMouseComboBox;
    QComboBox* _rightMouseComboBox;
    QCheckBox* _autoFitWindowCheckBox;
    QCheckBox* _enableCalibrationCheckBox;
    QLabel* _size1Label;
    QLineEdit* _pixelSizeEdit;
    QLabel* _size2Label;
    QButtonGroup* _windowGroup;
};
