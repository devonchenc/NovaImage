﻿#include "SettingsDialog.h"

#include <QTabWidget>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QAction>
#include <QSettings>
#include <QCoreApplication>

#include "../Core/GlobalFunc.h"
#include "../Core/mainwindow.h"
#include "../Core/ToolButton.h"

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Settings"));

    setWindowFlag(Qt::Popup);

    initUI();

    loadSettings();

    resize(400, 200);
}

void SettingsDialog::initUI()
{
    QTabWidget* tabWidget = new QTabWidget(this);
    tabWidget->addTab(createGeneralWidget(), tr("General"));
    tabWidget->addTab(createImageWidget(), tr("Image"));
    tabWidget->addTab(createCalibrationWidget(), tr("Calibration"));

    QPushButton* acceptButton = new QPushButton(tr("Accept"));
    connect(acceptButton, &QPushButton::clicked, this, &SettingsDialog::acceptButtonClicked);
    QPushButton* rejectButton = new QPushButton(tr("Cancel"));
    connect(rejectButton, &QPushButton::clicked, this, &QDialog::reject);
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(acceptButton);
    hLayout->addStretch();
    hLayout->addWidget(rejectButton);
    hLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addLayout(hLayout);
    setLayout(mainLayout);
}

QWidget* SettingsDialog::createGeneralWidget()
{
    _languageComboBox = new QComboBox;
    _languageComboBox->addItem("English");
    _languageComboBox->addItem(QStringLiteral("中文（简体）"));
    QHBoxLayout* hLayout1 = new QHBoxLayout;
    hLayout1->addWidget(new QLabel("Language:"));
    hLayout1->addWidget(_languageComboBox);

    MainWindow* mainWindow = getGlobalWindow();
    QVector<QAction*> vec = mainWindow->mouseActionVector();

    _leftMouseComboBox = new QComboBox;
    _leftMouseComboBox->setIconSize(QSize(24, 24));
    _leftMouseComboBox->addItem(tr("None"));
    _rightMouseComboBox = new QComboBox;
    _rightMouseComboBox->setIconSize(QSize(24, 24));
    _rightMouseComboBox->addItem(tr("None"));
    for (int i = 0; i < vec.size(); i++)
    {
        QString icon = QString(":/icon/svg/%1.svg").arg(vec[i]->objectName());
        _leftMouseComboBox->addItem(QIcon(icon), vec[i]->text());
        _rightMouseComboBox->addItem(QIcon(icon), vec[i]->text());
    }

    QGridLayout* hLayout2 = new QGridLayout;
    hLayout2->addWidget(new QLabel(tr("Left Mouse:")), 0, 0);
    hLayout2->addWidget(_leftMouseComboBox, 0, 1);
    hLayout2->addWidget(new QLabel(tr("Right Mouse:")), 1, 0);
    hLayout2->addWidget(_rightMouseComboBox, 1, 1);
    QGroupBox* groupBox1 = new QGroupBox(tr("Default mouse operation"));
    groupBox1->setLayout(hLayout2);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addLayout(hLayout1);
    vLayout->addWidget(groupBox1);

    QWidget* generalWidget = new QWidget;
    generalWidget->setLayout(vLayout);

    return generalWidget;
}

QWidget* SettingsDialog::createImageWidget()
{
    _autoFitWindowCheckBox = new QCheckBox(tr("Image display size automatically adapts to window width"));
    QRadioButton* defaultWindowButton = new QRadioButton(tr("Default Window"));
    QRadioButton* fullWindowButton = new QRadioButton(tr("Full Window"));
    _windowGroup = new QButtonGroup;
    _windowGroup->addButton(defaultWindowButton, 0);
    _windowGroup->addButton(fullWindowButton, 1);
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(defaultWindowButton);
    hLayout->addWidget(fullWindowButton);
    QGroupBox* groupBox1 = new QGroupBox(tr("Image display window"));
    groupBox1->setLayout(hLayout);
    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(_autoFitWindowCheckBox);
    vLayout->addWidget(groupBox1);

    QWidget* imageWidget = new QWidget;
    imageWidget->setLayout(vLayout);

    return imageWidget;
}

QWidget* SettingsDialog::createCalibrationWidget()
{
    _enableCalibrationCheckBox = new QCheckBox(tr("Enable calibration"));
    QObject::connect(_enableCalibrationCheckBox, &QCheckBox::toggled, [=](bool checked) {
        _size1Label->setEnabled(checked);
        _pixelSizeEdit->setEnabled(checked);
        _size2Label->setEnabled(checked);
    });

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(_enableCalibrationCheckBox);

    _size1Label = new QLabel(tr("1 pixel = "));
    _pixelSizeEdit = new QLineEdit;
    _pixelSizeEdit->setFixedWidth(80);
    _size2Label = new QLabel(tr("mm"));
    QDoubleValidator* doubleValidator = new QDoubleValidator(_pixelSizeEdit);
    _pixelSizeEdit->setValidator(doubleValidator);
    doubleValidator->setRange(0.0, 1000.0);
    doubleValidator->setDecimals(7);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);

    _size1Label->setEnabled(false);
    _pixelSizeEdit->setEnabled(false);
    _size2Label->setEnabled(false);

    int indent = 20;
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addSpacing(indent);
    hLayout->addWidget(_size1Label);
    hLayout->addWidget(_pixelSizeEdit);
    hLayout->addWidget(_size2Label);
    hLayout->addStretch();
    vLayout->addLayout(hLayout);

    QWidget* calibrationWidget = new QWidget;
    calibrationWidget->setLayout(vLayout);

    return calibrationWidget;
}

void SettingsDialog::loadSettings()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    int language = settings.value("General/language", 0).toInt();
    QString leftMouseString = settings.value("General/leftMouse", tr("None")).toString();
    QString rightMouseString = settings.value("General/rightMouse", tr("None")).toString();
    bool fitWindow = settings.value("Image/autoFitWindow", 0).toBool();
    int displayWindow = settings.value("Image/displayWindow", 0).toInt();
    bool enableCalibration = settings.value("Calibration/enable", false).toBool();
    double pixelSize = settings.value("Calibration/size", 0).toDouble();

    _languageComboBox->setCurrentIndex(language);

    MainWindow* mainWindow = getGlobalWindow();
    QVector<QAction*> vec = mainWindow->mouseActionVector();
    if (leftMouseString == "None")
    {
        _leftMouseComboBox->setCurrentIndex(0);
    }
    else
    {
        for (int i = 0; i < vec.size(); i++)
        {
            if (leftMouseString == vec[i]->objectName())
            {
                _leftMouseComboBox->setCurrentIndex(i + 1);
                break;
            }
        }
    }

    if (rightMouseString == "None")
    {
        _rightMouseComboBox->setCurrentIndex(0);
    }
    else
    {
        for (int i = 0; i < vec.size(); i++)
        {
            if (rightMouseString == vec[i]->objectName())
            {
                _rightMouseComboBox->setCurrentIndex(i + 1);
                break;
            }
        }
    }

    _autoFitWindowCheckBox->setChecked(fitWindow);
    _windowGroup->button(displayWindow)->setChecked(true);
    _enableCalibrationCheckBox->setChecked(enableCalibration);
    _pixelSizeEdit->setText(QString::number(pixelSize));
}

void SettingsDialog::acceptButtonClicked()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    int language = settings.value("General/language", 0).toInt();

    settings.setValue("General/language", _languageComboBox->currentIndex());
    if (language != _languageComboBox->currentIndex())
    {
        // Change language
        emit changeLanguage(_languageComboBox->currentIndex());
    }

    MainWindow* mainWindow = getGlobalWindow();
    QVector<QAction*> vec = mainWindow->mouseActionVector();
    if (_leftMouseComboBox->currentIndex() == 0)
    {
        settings.setValue("General/leftMouse", "None");

        ToolButton::unsetLeftMouseButton();
    }
    else
    {
        QAction* action = vec[_leftMouseComboBox->currentIndex() - 1];
        settings.setValue("General/leftMouse", action->objectName());
        action->trigger();
        ToolButton* toolButton = qobject_cast<ToolButton*>(action->parentWidget());
        toolButton->activeAction(action, true);
    }

    if (_rightMouseComboBox->currentIndex() == 0)
    {
        settings.setValue("General/rightMouse", "None");

        ToolButton::unsetRightMouseButton();
    }
    else
    {
        QAction* action = vec[_rightMouseComboBox->currentIndex() - 1];
        settings.setValue("General/rightMouse", action->objectName());
        action->trigger();
        ToolButton* toolButton = qobject_cast<ToolButton*>(action->parentWidget());
        toolButton->activeAction(action, false);
    }

    settings.setValue("Image/autoFitWindow", _autoFitWindowCheckBox->isChecked());
    settings.setValue("Image/displayWindow", _windowGroup->checkedId());

    settings.setValue("Calibration/enable", _enableCalibrationCheckBox->isChecked());
    settings.setValue("Calibration/size", _pixelSizeEdit->text());

    accept();
}
