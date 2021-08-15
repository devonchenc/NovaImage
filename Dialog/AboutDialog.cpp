#include "AboutDialog.h"

#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About NovaImage"));

    setWindowFlag(Qt::Popup);

    initUI();

    resize(400, 200);
}

void AboutDialog::initUI()
{
    //QLabel* nameLabel = new QLabel(tr("File Name:"));
    //QLabel* actualSize = new QLabel(tr("Actual Size:"));
    //QLabel* unit1Label = new QLabel(tr("bytes"));
    //QLabel* unit2Label = new QLabel(tr("bytes"));
    //QLabel* expectedSize = new QLabel(tr("Expected Size:"));
    //_expectedSizeLabel = new QLabel;

    //QGridLayout* grid1 = new QGridLayout;
    //grid1->addWidget(nameLabel, 0, 0);
    //grid1->addWidget(new QLabel(_fileName), 0, 1, 1, 2);
    //grid1->addWidget(actualSize, 1, 0);
    //grid1->addWidget(new QLabel(QString::number(_actualSize)), 1, 1);
    //grid1->addWidget(unit1Label, 1, 2);
    //grid1->addWidget(expectedSize, 2, 0);
    //grid1->addWidget(_expectedSizeLabel, 2, 1);
    //grid1->addWidget(unit2Label, 2, 2);
    //QGroupBox* groupBox1 = new QGroupBox(tr("File Information"));
    //groupBox1->setLayout(grid1);

    //QLabel* dataLabel = new QLabel(tr("Data type:"));
    //_typeComboBox = new QComboBox;
    //_typeComboBox->addItem("8-bit Unsigned Char");
    //_typeComboBox->addItem("16-bit Unsigned Short");
    //_typeComboBox->addItem("32-bit Unsigned Int");
    //_typeComboBox->addItem("32-bit Float");
    //_typeComboBox->addItem("64-bit Double Float");
    //QLabel* widthLabel = new QLabel(tr("Image width:"));
    //_widthComboBox = new QComboBox;
    //_widthComboBox->addItem("128");
    //_widthComboBox->addItem("256");
    //_widthComboBox->addItem("512");
    //_widthComboBox->addItem("1024");
    //_widthComboBox->addItem("2048");
    //_widthComboBox->setEditable(true);
    //QLabel* heightLabel = new QLabel(tr("Image height:"));
    //_heightComboBox = new QComboBox;
    //_heightComboBox->addItem("128");
    //_heightComboBox->addItem("256");
    //_heightComboBox->addItem("512");
    //_heightComboBox->addItem("1024");
    //_heightComboBox->addItem("2048");
    //_heightComboBox->setEditable(true);
    //QLabel* sliceLabel = new QLabel(tr("Image slice:"));
    //_sliceComboBox = new QComboBox;
    //_sliceComboBox->addItem("128");
    //_sliceComboBox->addItem("256");
    //_sliceComboBox->addItem("512");
    //_sliceComboBox->addItem("1024");
    //_sliceComboBox->addItem("2048");
    //_sliceComboBox->setEditable(true);
    //connect(_typeComboBox, &QComboBox::currentTextChanged, this, &RawParameterDialog::updateExpectedSize);
    //connect(_widthComboBox, &QComboBox::currentTextChanged, this, &RawParameterDialog::updateExpectedSize);
    //connect(_heightComboBox, &QComboBox::currentTextChanged, this, &RawParameterDialog::updateExpectedSize);
    //connect(_sliceComboBox, &QComboBox::currentTextChanged, this, &RawParameterDialog::updateExpectedSize);
    //QLabel* headerLabel = new QLabel(tr("Header size:"));
    //_headerEdit = new QLineEdit("0");
    //QValidator* validator = new QRegExpValidator(QRegExp("[0-9]+$"));
    //_headerEdit->setValidator(validator);
    //connect(_headerEdit, &QLineEdit::textChanged, this, &RawParameterDialog::updateExpectedSize);
    //QLabel* header2Label = new QLabel(tr("bytes"));

    //QGridLayout* grid2 = new QGridLayout;
    //grid2->addWidget(dataLabel, 0, 0);
    //grid2->addWidget(_typeComboBox, 0, 1);
    //grid2->addWidget(widthLabel, 1, 0);
    //grid2->addWidget(_widthComboBox, 1, 1);
    //grid2->addWidget(new QLabel(tr("pixels")), 1, 2);
    //grid2->addWidget(heightLabel, 2, 0);
    //grid2->addWidget(_heightComboBox, 2, 1);
    //grid2->addWidget(new QLabel(tr("pixels")), 2, 2);
    //grid2->addWidget(sliceLabel, 3, 0);
    //grid2->addWidget(_sliceComboBox, 3, 1);
    //grid2->addWidget(new QLabel(tr("pixels")), 3, 2);
    //grid2->addWidget(headerLabel, 4, 0);
    //grid2->addWidget(_headerEdit, 4, 1);
    //grid2->addWidget(header2Label, 4, 2);
    //QGroupBox* groupBox2 = new QGroupBox(tr("Image Information"));
    //groupBox2->setLayout(grid2);

    //QPushButton* acceptButton = new QPushButton(tr("Accept"));
    //connect(acceptButton, &QPushButton::clicked, this, &RawParameterDialog::acceptButtonClicked);
    //QPushButton* rejectButton = new QPushButton(tr("Cancel"));
    //connect(rejectButton, &QPushButton::clicked, this, &QDialog::reject);
    //QHBoxLayout* hLayout = new QHBoxLayout();
    //hLayout->addStretch();
    //hLayout->addWidget(acceptButton);
    //hLayout->addStretch();
    //hLayout->addWidget(rejectButton);
    //hLayout->addStretch();

    //QVBoxLayout* vLayout = new QVBoxLayout;
    //vLayout->addWidget(groupBox1);
    //vLayout->addWidget(groupBox2);
    //vLayout->addLayout(hLayout);
    //setLayout(vLayout);
}

