#include "RawParameterDialog.h"

#include <QFileInfo>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QEvent>
#include <QSettings>
#include <QCoreApplication>

RawParameterDialog::RawParameterDialog(const QString& pathName, QWidget* parent)
    : QDialog(parent)
    , _fileName(pathName)
{
    QFileInfo fileInfo(_fileName);
    _actualSize = fileInfo.size();

    setWindowTitle(tr("Open Raw file"));

    setWindowFlag(Qt::Popup);

    initUI();

    loadSettings();

    updateExpectedSize();

    resize(400, 350);
}

void RawParameterDialog::initUI()
{
    QLabel* nameLabel = new QLabel(tr("File Name:"));
    QLabel* actualSize = new QLabel(tr("Actual Size:"));
    QLabel* unit1Label = new QLabel(tr("bytes"));
    QLabel* unit2Label = new QLabel(tr("bytes"));
    QLabel* expectedSize = new QLabel(tr("Expected Size:"));
    _expectedSizeLabel = new QLabel;

    QGridLayout* grid1 = new QGridLayout;
    grid1->addWidget(nameLabel, 0, 0);
    grid1->addWidget(new QLabel(_fileName), 0, 1, 1, 2);
    grid1->addWidget(actualSize, 1, 0);
    grid1->addWidget(new QLabel(QString::number(_actualSize)), 1, 1);
    grid1->addWidget(unit1Label, 1, 2);
    grid1->addWidget(expectedSize, 2, 0);
    grid1->addWidget(_expectedSizeLabel, 2, 1);
    grid1->addWidget(unit2Label, 2, 2);
    QGroupBox* groupBox1 = new QGroupBox(tr("File Information"));
    groupBox1->setLayout(grid1);

    QLabel* dataLabel = new QLabel(tr("Data type:"));
    _typeComboBox = new QComboBox;
    _typeComboBox->addItem("8-bit Unsigned Char");
    _typeComboBox->addItem("16-bit Unsigned Short");
    _typeComboBox->addItem("32-bit Unsigned Int");
    _typeComboBox->addItem("32-bit Float");
    _typeComboBox->addItem("64-bit Double Float");
    QLabel* widthLabel = new QLabel(tr("Image width:"));
    _widthComboBox = new QComboBox;
    _widthComboBox->addItem("128");
    _widthComboBox->addItem("256");
    _widthComboBox->addItem("512");
    _widthComboBox->addItem("1024");
    _widthComboBox->addItem("2048");
    _widthComboBox->setEditable(true);
    QLabel* heightLabel = new QLabel(tr("Image height:"));
    _heightComboBox = new QComboBox;
    _heightComboBox->addItem("128");
    _heightComboBox->addItem("256");
    _heightComboBox->addItem("512");
    _heightComboBox->addItem("1024");
    _heightComboBox->addItem("2048");
    _heightComboBox->setEditable(true);
    QLabel* sliceLabel = new QLabel(tr("Image slice:"));
    _sliceComboBox = new QComboBox;
    _sliceComboBox->addItem("128");
    _sliceComboBox->addItem("256");
    _sliceComboBox->addItem("512");
    _sliceComboBox->addItem("1024");
    _sliceComboBox->addItem("2048");
    _sliceComboBox->setEditable(true);
    connect(_typeComboBox, &QComboBox::currentTextChanged, this, &RawParameterDialog::updateExpectedSize);
    connect(_widthComboBox, &QComboBox::currentTextChanged, this, &RawParameterDialog::updateExpectedSize);
    connect(_heightComboBox, &QComboBox::currentTextChanged, this, &RawParameterDialog::updateExpectedSize);
    connect(_sliceComboBox, &QComboBox::currentTextChanged, this, &RawParameterDialog::updateExpectedSize);
    QLabel* headerLabel = new QLabel(tr("Header size:"));
    _headerEdit = new QLineEdit("0");
    QValidator* validator = new QRegExpValidator(QRegExp("[0-9]+$"));
    _headerEdit->setValidator(validator);
    connect(_headerEdit, &QLineEdit::textChanged, this, &RawParameterDialog::updateExpectedSize);
    QLabel* header2Label = new QLabel(tr("bytes"));

    QRadioButton* littleEndianButton = new QRadioButton(tr("Little-endian byte order"));
    QRadioButton* bigEndianButton = new QRadioButton(tr("Big-endian byte order"));
    _endianGroup = new QButtonGroup;
    _endianGroup->addButton(littleEndianButton, 0);
    _endianGroup->addButton(bigEndianButton, 1);

    QGridLayout* grid2 = new QGridLayout;
    grid2->addWidget(dataLabel, 0, 0);
    grid2->addWidget(_typeComboBox, 0, 1);
    grid2->addWidget(widthLabel, 1, 0);
    grid2->addWidget(_widthComboBox, 1, 1);
    grid2->addWidget(new QLabel(tr("pixels")), 1, 2);
    grid2->addWidget(heightLabel, 2, 0);
    grid2->addWidget(_heightComboBox, 2, 1);
    grid2->addWidget(new QLabel(tr("pixels")), 2, 2);
    grid2->addWidget(sliceLabel, 3, 0);
    grid2->addWidget(_sliceComboBox, 3, 1);
    grid2->addWidget(new QLabel(tr("pixels")), 3, 2);
    grid2->addWidget(headerLabel, 4, 0);
    grid2->addWidget(_headerEdit, 4, 1);
    grid2->addWidget(header2Label, 4, 2);
    grid2->addWidget(littleEndianButton, 5, 0);
    grid2->addWidget(bigEndianButton, 5, 1);
    QGroupBox* groupBox2 = new QGroupBox(tr("Image Information"));
    groupBox2->setLayout(grid2);

    QPushButton* acceptButton = new QPushButton(tr("Accept"));
    connect(acceptButton, &QPushButton::clicked, this, &RawParameterDialog::acceptButtonClicked);
    QPushButton* rejectButton = new QPushButton(tr("Cancel"));
    connect(rejectButton, &QPushButton::clicked, this, &QDialog::reject);
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(acceptButton);
    hLayout->addStretch();
    hLayout->addWidget(rejectButton);
    hLayout->addStretch();

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(groupBox1);
    vLayout->addWidget(groupBox2);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
}

void RawParameterDialog::loadSettings()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
    int type = settings.value("Raw/type", 1).toInt();
    QString width = settings.value("Raw/width", 1024).toString();
    QString height = settings.value("Raw/height", 1024).toString();
    QString slice = settings.value("Raw/slice", 1024).toString();
    QString headerSize = settings.value("Raw/headerSize", 0).toString();
    int endian = settings.value("Raw/endian", 0).toInt();

    _typeComboBox->setCurrentIndex(type);
    _widthComboBox->setCurrentText(width);
    _heightComboBox->setCurrentText(height);
    _sliceComboBox->setCurrentText(slice);
    _headerEdit->setText(headerSize);
    _endianGroup->button(endian)->setChecked(true);
}

void RawParameterDialog::updateExpectedSize()
{
    _dataType = _typeComboBox->currentIndex();
    _width = _widthComboBox->currentText().toInt();
    _height = _heightComboBox->currentText().toInt();
    _slice = _sliceComboBox->currentText().toInt();
    _headerSize = _headerEdit->text().toInt();
    _endian = _endianGroup->checkedId();

    _expectedSize = _headerSize;
    switch (_dataType)
    {
    case 0:
        _expectedSize += sizeof(uchar) * qint64(_width) * qint64(_height) * qint64(_slice);
        break;
    case 1:
        _expectedSize += sizeof(ushort) * qint64(_width) * qint64(_height) * qint64(_slice);
        break;
    case 2:
        _expectedSize += sizeof(uint) * qint64(_width) * qint64(_height) * qint64(_slice);
        break;
    case 3:
        _expectedSize += sizeof(float) * qint64(_width) * qint64(_height) * qint64(_slice);
        break;
    case 4:
        _expectedSize += sizeof(double) * qint64(_width) * qint64(_height) * qint64(_slice);
        break;
    }

    _expectedSizeLabel->setText(QString::number(_expectedSize));
}

void RawParameterDialog::acceptButtonClicked()
{
    updateExpectedSize();

    if (_expectedSize > _actualSize)
    {
        QMessageBox::critical(nullptr, QObject::tr("Open image file error"),
                              QObject::tr("The data size does not match the file information description!"), QMessageBox::Ok);
        return;
    }
    else
    {
        QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
        settings.setValue("Raw/type", _dataType);
        settings.setValue("Raw/width", _width);
        settings.setValue("Raw/height", _height);
        settings.setValue("Raw/slice", _slice);
        settings.setValue("Raw/headerSize", _headerSize);
        settings.setValue("Raw/endian", _endian);

        accept();
    }
}
