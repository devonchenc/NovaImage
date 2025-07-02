#include "CalibrationDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QSettings>
#include <QCoreApplication>

#include "../Core/GlobalFunc.h"

CalibrationDialog::CalibrationDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Calibration"));

    setWindowFlag(Qt::Popup);

    initUI();

    loadSettings();

    resize(400, 240);
}

void CalibrationDialog::initUI()
{
    QCheckBox* enableCheckBox = new QCheckBox(tr("Enable Calibration"));

    QRadioButton* radio1 = new QRadioButton(tr("Use system default calibration"));
    QRadioButton* radio2 = new QRadioButton(tr("Enter pixel size manually"));
    QRadioButton* radio3 = new QRadioButton(tr("Calibrate using current reference line"));
    radio1->setEnabled(false);
    radio2->setEnabled(false);
    radio3->setEnabled(false);
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(radio1, 0);
    buttonGroup->addButton(radio2, 1);
    buttonGroup->addButton(radio3, 1);

    connect(radio1, &QRadioButton::toggled, this, [=](bool checked) {
        _systemSizeLabel->setEnabled(checked);
    });
    connect(radio2, &QRadioButton::toggled, this, [=](bool checked) {
        _manualSize1Label->setEnabled(checked);
        _pixelSizeEdit->setEnabled(checked);
        _manualSize2Label->setEnabled(checked);
    });
    connect(radio3, &QRadioButton::toggled, this, [=](bool checked) {
        _startPositionLabel->setEnabled(checked);
        _endPositionLabel->setEnabled(checked);
        _pixelLength1Label->setEnabled(checked);
        _pixelLength2Label->setEnabled(checked);
        _actualLength1Label->setEnabled(checked);
        _actualLengthEdit->setEnabled(checked);
        _actualLength2Label->setEnabled(checked);

        getCurrentLineInfo();
    });

    _systemSizeLabel = new QLabel(tr("1 pixel = mm"));
    _manualSize1Label = new QLabel(tr("1 pixel = "));
    _pixelSizeEdit = new QLineEdit;
    _pixelSizeEdit->setFixedWidth(80);
    _manualSize2Label = new QLabel(tr("mm"));
    _systemSizeLabel->setEnabled(false);
    _manualSize1Label->setEnabled(false);
    _pixelSizeEdit->setEnabled(false);
    _manualSize2Label->setEnabled(false);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(enableCheckBox);

    int indent = 20;
    for (QRadioButton* radio : { radio1, radio2, radio3 })
    {
        QHBoxLayout* hLayout = new QHBoxLayout;
        hLayout->addSpacing(indent);
        hLayout->addWidget(radio);
        hLayout->addStretch();
        mainLayout->addLayout(hLayout);
    }

    QHBoxLayout* h1Layout = new QHBoxLayout;
    h1Layout->addSpacing(indent * 2);
    h1Layout->addWidget(_systemSizeLabel);
    h1Layout->addStretch();
    mainLayout->insertLayout(2, h1Layout);

    QHBoxLayout* h2Layout = new QHBoxLayout;
    h2Layout->addSpacing(indent * 2);
    h2Layout->addWidget(_manualSize1Label);
    h2Layout->addWidget(_pixelSizeEdit);
    h2Layout->addWidget(_manualSize2Label);
    h2Layout->addStretch();
    mainLayout->insertLayout(4, h2Layout);

    _startPositionLabel = new QLabel(tr("Line start position:"));
    _startPositionEdit = new QLineEdit;
    _endPositionLabel = new QLabel(tr("Line end position:"));
    _endPositionEdit = new QLineEdit;
    _pixelLength1Label = new QLabel(tr("Pixel length of reference line:"));
    _pixelLengthEdit = new QLineEdit;
    _pixelLength2Label = new QLabel(tr("pixel"));
    _actualLength1Label = new QLabel(tr("Enter the actual length of the line:"));
    _actualLengthEdit = new QLineEdit;
    _actualLength2Label = new QLabel(tr("mm"));
    _startPositionLabel->setEnabled(false);
    _startPositionEdit->setEnabled(false);
    _endPositionLabel->setEnabled(false);
    _endPositionEdit->setEnabled(false);
    _pixelLength1Label->setEnabled(false);
    _pixelLengthEdit->setEnabled(false);
    _pixelLength2Label->setEnabled(false);
    _actualLength1Label->setEnabled(false);
    _actualLengthEdit->setEnabled(false);
    _actualLength2Label->setEnabled(false);

    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->addWidget(_startPositionLabel, 0, 0);
    gridLayout->addWidget(_startPositionEdit, 0, 1);
    gridLayout->addWidget(_endPositionLabel, 1, 0);
    gridLayout->addWidget(_endPositionEdit, 1, 1);
    gridLayout->addWidget(_pixelLength1Label, 2, 0);
    gridLayout->addWidget(_pixelLengthEdit, 2, 1);
    gridLayout->addWidget(_pixelLength2Label, 2, 2);
    gridLayout->addWidget(_actualLength1Label, 3, 0);
    gridLayout->addWidget(_actualLengthEdit, 3, 1);
    gridLayout->addWidget(_actualLength2Label, 3, 2);

    QHBoxLayout* h3Layout = new QHBoxLayout;
    h3Layout->addSpacing(indent * 2);
    h3Layout->addLayout(gridLayout);
    h3Layout->addStretch();
    mainLayout->addLayout(h3Layout);

    _setAsSystemLabel = new QCheckBox(tr("Set this calibration as the system default"));
    _setAsSystemLabel->setEnabled(false);
    QHBoxLayout* h4Layout = new QHBoxLayout;
    h4Layout->addSpacing(indent);
    h4Layout->addWidget(_setAsSystemLabel);
    h4Layout->addStretch();
    mainLayout->addStretch();
    mainLayout->addLayout(h4Layout);

    QObject::connect(enableCheckBox, &QCheckBox::toggled, [=](bool checked) {
        radio1->setEnabled(checked);
        radio2->setEnabled(checked);
        radio3->setEnabled(checked);
        _setAsSystemLabel->setEnabled(checked);
        if (!checked)
        {
            _systemSizeLabel->setEnabled(checked);
            _manualSize1Label->setEnabled(checked);
            _pixelSizeEdit->setEnabled(checked);
            _manualSize2Label->setEnabled(checked);
        }
    });

    QPushButton* acceptButton = new QPushButton(tr("Accept"));
    connect(acceptButton, &QPushButton::clicked, this, &CalibrationDialog::acceptButtonClicked);
    QPushButton* rejectButton = new QPushButton(tr("Cancel"));
    connect(rejectButton, &QPushButton::clicked, this, &QDialog::reject);
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(acceptButton);
    hLayout->addStretch();
    hLayout->addWidget(rejectButton);
    hLayout->addStretch();

    mainLayout->addStretch();
    mainLayout->addLayout(hLayout);

    setLayout(mainLayout);
}

void CalibrationDialog::loadSettings()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    double pixelSize = settings.value("Calibration/size", 0).toDouble();
    _pixelSizeEdit->setText(QString::number(pixelSize));
}

void CalibrationDialog::getCurrentLineInfo()
{
    MainWindow* mainWindow = getGlobalWindow();
}

void CalibrationDialog::acceptButtonClicked()
{
    if (_setAsSystemLabel->isChecked())
    {
        QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
        //settings.setValue("Calibration/size", _pixelSizeEdit->text());
    }
}