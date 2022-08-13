#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class QButtonGroup;
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

    void loadSettings();

private slots:
    void acceptButtonClicked();

private:
    QComboBox* _languageComboBox;
    QComboBox* _leftMouseComboBox;
    QComboBox* _rightMouseComboBox;
    QCheckBox* _autoFitWindowCheckBox;
    QButtonGroup* _windowGroup;
};
