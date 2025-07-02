#include "CalibrationDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>

CalibrationDialog::CalibrationDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Calibration"));

    setWindowFlag(Qt::Popup);

    initUI();

    resize(400, 240);
}

void CalibrationDialog::initUI()
{
    QCheckBox* enableCheckBox = new QCheckBox(tr("Enable Calibration"));

    QRadioButton* radio1 = new QRadioButton(tr("Use system default calibration"));
    QRadioButton* radio2 = new QRadioButton(tr("Enter pixel size manually"));
    QRadioButton* radio3 = new QRadioButton(tr("Calibrate using reference line"));
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

    _setAsSystemLabel = new QCheckBox(tr("Set this calibration as the system default"));
    _setAsSystemLabel->setEnabled(false);
    QHBoxLayout* h3Layout = new QHBoxLayout;
    h3Layout->addSpacing(indent);
    h3Layout->addWidget(_setAsSystemLabel);
    h3Layout->addStretch();
    mainLayout->addStretch();
    mainLayout->addLayout(h3Layout);

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

void CalibrationDialog::acceptButtonClicked()
{

}