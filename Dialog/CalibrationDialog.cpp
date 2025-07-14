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
#include <QDoubleValidator>
#include <QMessageBox>

#include "../Core/GlobalFunc.h"
#include "../Core/View.h"
#include "../Core/GraphicsScene.h"
#include "../Core/Document.h"
#include "../Image/BaseImage.h"

CalibrationDialog::CalibrationDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Calibration"));

    setWindowFlag(Qt::Popup);

    initUI();

    resize(450, 360);
}

void CalibrationDialog::initUI()
{
    _currentCalibrationLabel = new QLabel;
    _enableCheckBox = new QCheckBox(tr("Enable Calibration"));

    QRadioButton* radio1 = new QRadioButton(tr("Use system default calibration"));
    QRadioButton* radio2 = new QRadioButton(tr("Enter pixel size manually"));
    QRadioButton* radio3 = new QRadioButton(tr("Calibrate using current reference line"));
    radio1->setEnabled(false);
    radio2->setEnabled(false);
    radio3->setEnabled(false);
    _radioGroup = new QButtonGroup(this);
    _radioGroup->addButton(radio1, 0);
    _radioGroup->addButton(radio2, 1);
    _radioGroup->addButton(radio3, 2);

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

        if (checked)
        {
            getCurrentLineInfo();
        }
    });

    _systemSizeLabel = new QLabel(tr("1 pixel = ") + " mm");
    _manualSize1Label = new QLabel(tr("1 pixel = "));
    _pixelSizeEdit = new QLineEdit;
    _pixelSizeEdit->setFixedWidth(100);
    QDoubleValidator* doubleValidator1 = new QDoubleValidator(_pixelSizeEdit);
    _pixelSizeEdit->setValidator(doubleValidator1);
    doubleValidator1->setRange(0.0, 10000.0);
    doubleValidator1->setDecimals(7);
    doubleValidator1->setNotation(QDoubleValidator::StandardNotation);
    _manualSize2Label = new QLabel(tr("mm"));
    _systemSizeLabel->setEnabled(false);
    _manualSize1Label->setEnabled(false);
    _pixelSizeEdit->setEnabled(false);
    _manualSize2Label->setEnabled(false);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(_currentCalibrationLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(_enableCheckBox);

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
    mainLayout->insertLayout(4, h1Layout);

    QHBoxLayout* h2Layout = new QHBoxLayout;
    h2Layout->addSpacing(indent * 2);
    h2Layout->addWidget(_manualSize1Label);
    h2Layout->addWidget(_pixelSizeEdit);
    h2Layout->addWidget(_manualSize2Label);
    h2Layout->addStretch();
    mainLayout->insertLayout(6, h2Layout);

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
    _startPositionEdit->setFixedWidth(100);
    _endPositionEdit->setFixedWidth(100);
    _pixelLengthEdit->setFixedWidth(100);
    _actualLengthEdit->setFixedWidth(100);
    QDoubleValidator* doubleValidator2 = new QDoubleValidator(_actualLengthEdit);
    _actualLengthEdit->setValidator(doubleValidator2);
    doubleValidator2->setRange(0.0, 10000.0);
    doubleValidator2->setDecimals(7);
    doubleValidator2->setNotation(QDoubleValidator::StandardNotation);
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

    QObject::connect(_enableCheckBox, &QCheckBox::toggled, [=](bool checked) {
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
    float pixelSize = settings.value("Calibration/size", 0).toFloat();
    _systemSizeLabel->setText(tr("1 pixel = ") + QString::number(pixelSize) + " mm");

    BaseImage* image = getGlobalImage();
    if (!image)
        return;

    std::optional<float> calibrationSize = image->getCalibrationSpacing();
    if (calibrationSize)
    {
        _currentCalibrationLabel->setText(tr("Current image pixel size: ") + QString::number(calibrationSize.value()) + " mm");
    }
    else
    {
        _currentCalibrationLabel->setText(tr("Current image is uncalibrated."));
    }
}

void CalibrationDialog::getCurrentLineInfo()
{
    GraphicsScene* scene = getGlobalActiveView()->scene();
    DiagramLengthItem* item = scene->focusLengthItem();
    setWidgetText(item);
}

void CalibrationDialog::setWidgetText(const DiagramLengthItem* item)
{
    if (item == nullptr)
        return;

    QString point1 = "(" + QString::number(item->p1().x()) + ", " + QString::number(item->p1().y()) + ")";
    _startPositionEdit->setText(point1);
    QString point2 = "(" + QString::number(item->p2().x()) + ", " + QString::number(item->p2().y()) + ")";
    _endPositionEdit->setText(point2);
    QString pixelLength = QString::number(item->pixelLength());
    _pixelLengthEdit->setText(pixelLength);
}

void CalibrationDialog::lengthItemSelected(const DiagramLengthItem* item)
{
    setWidgetText(item);
}

void CalibrationDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    loadSettings();
}

void CalibrationDialog::acceptButtonClicked()
{
    Document* document = getGlobalDocument();
    int checkedId = _radioGroup->checkedId();
    if (document && _enableCheckBox->isChecked() && checkedId >= 0 && checkedId < 3)
    {
        float pixelSize = 0;
        if (checkedId == 0)
        {
            QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
            pixelSize = settings.value("Calibration/size", 0).toFloat();
        }
        else if (checkedId == 1)
        {
            pixelSize = _pixelSizeEdit->text().toFloat();
        }
        else if (checkedId == 2)
        {
            float pixelLength = _pixelLengthEdit->text().toFloat();
            float actualLength = _actualLengthEdit->text().toFloat();
            if (pixelLength <= 0)
            {
                QMessageBox::critical(nullptr, tr("Invalid parameter"), tr("The pixel length must be greater than 0!"), QMessageBox::Ok);
                return;
            }
            if (actualLength <= 0)
            {
                QMessageBox::critical(nullptr, tr("Invalid parameter"), tr("The actual length must be greater than 0!"), QMessageBox::Ok);
                return;
            }
            pixelSize = actualLength / pixelLength;
        }
        document->saveCalibrationInfo(pixelSize);

        BaseImage* image = getGlobalImage();
        if (image)
        {
            image->setCalibrationSpacing(pixelSize);
        }

        if (_setAsSystemLabel->isChecked())
        {
            QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
            settings.setValue("Calibration/size", pixelSize);
        }
        accept();
    }
    else
    {
        reject();
    }
}