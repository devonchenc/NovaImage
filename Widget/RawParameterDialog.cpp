#include "RawParameterDialog.h"

#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>

RawParameterDialog::RawParameterDialog(const QString& pathName, QWidget* parent)
	: QDialog(parent)
	, _fileName(pathName)
{
	initUI();
}

RawParameterDialog::~RawParameterDialog()
{

}

void RawParameterDialog::initUI()
{
	QLabel* nameLabel = new QLabel(tr("File Name:"));
	QLabel* fileLabel = new QLabel(_fileName);
	QLabel* expectedSize = new QLabel(tr("Expected Size:"));
	QLabel* expectedSize2 = new QLabel;
	QLabel* actualSize = new QLabel(tr("Actual Size:"));
	_actualSize = new QLabel;

	QGridLayout* grid1 = new QGridLayout();
	grid1->addWidget(nameLabel, 0, 0);
	grid1->addWidget(fileLabel, 0, 1);
	grid1->addWidget(expectedSize, 1, 0);
	grid1->addWidget(expectedSize2, 1, 1);
	grid1->addWidget(actualSize, 2, 0);
	grid1->addWidget(_actualSize, 2, 1);
	QGroupBox* groupBox1 = new QGroupBox(tr("File Information"));
	groupBox1->setLayout(grid1);

	QLabel* dataLabel = new QLabel(tr("Data type:"));
	QComboBox* typeComboBox = new QComboBox;
	typeComboBox->addItem(tr("8-bit Unsigned Char"));
	typeComboBox->addItem(tr("16-bit Unsigned Short"));
	typeComboBox->addItem(tr("32-bit Unsigned Int"));
	typeComboBox->addItem(tr("32-bit Float"));
	typeComboBox->addItem(tr("64-bit Double Float"));
	QLabel* widthLabel = new QLabel(tr("Image width:"));
	QComboBox* widthComboBox = new QComboBox;
	widthComboBox->addItem("128");
	widthComboBox->addItem("256");
	widthComboBox->addItem("512");
	widthComboBox->addItem("1024");
	widthComboBox->addItem("2048");
	widthComboBox->setEditable(true);
	QLabel* heightLabel = new QLabel(tr("Image height:"));
	QComboBox* heightComboBox = new QComboBox;
	heightComboBox->addItem("128");
	heightComboBox->addItem("256");
	heightComboBox->addItem("512");
	heightComboBox->addItem("1024");
	heightComboBox->addItem("2048");
	heightComboBox->setEditable(true);
	QLabel* headerLabel = new QLabel(tr("Header size:"));
	QLineEdit* headerEdit = new QLineEdit;
	QValidator* validator = new QRegExpValidator(QRegExp("[0-9]+$"));
	headerEdit->setValidator(validator);
	QLabel* header2Label = new QLabel(tr("Byte"));

	QGridLayout* grid2 = new QGridLayout();
	grid2->addWidget(dataLabel, 0, 0);
	grid2->addWidget(typeComboBox, 0, 1);
	grid2->addWidget(widthLabel, 1, 0);
	grid2->addWidget(widthComboBox, 1, 1);
	grid2->addWidget(heightLabel, 2, 0);
	grid2->addWidget(heightComboBox, 2, 1);
	grid2->addWidget(headerLabel, 3, 0);
	grid2->addWidget(headerEdit, 3, 1);
	grid2->addWidget(header2Label, 3, 2);
	QGroupBox* groupBox2 = new QGroupBox(tr("Image Information"));
	groupBox2->setLayout(grid2);

	QPushButton* acceptButton = new QPushButton(tr("Accept"));
	connect(acceptButton, &QPushButton::clicked, this, &QDialog::accept);
	QPushButton* rejectButton = new QPushButton(tr("Cancel"));
	connect(rejectButton, &QPushButton::clicked, this, &QDialog::reject);
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(acceptButton);
	hLayout->addStretch();
	hLayout->addWidget(rejectButton);

	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(groupBox1);
	vLayout->addWidget(groupBox2);
	vLayout->addLayout(hLayout);
	setLayout(vLayout);
}