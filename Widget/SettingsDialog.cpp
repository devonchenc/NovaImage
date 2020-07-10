#include "SettingsDialog.h"

#include <QTabWidget>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSettings>
#include <QCoreApplication>

#if _MSC_VER >= 1600 
#pragma execution_character_set("UTF-8")
#endif

SettingsDialog::SettingsDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Preferences"));

	setWindowFlag(Qt::Popup);
	
	initUI();

	loadSettings();

	resize(400, 200);
}

SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::initUI()
{
	_languageComboBox = new QComboBox;
	_languageComboBox->addItem("English");
	_languageComboBox->addItem("ÖÐÎÄ£¨¼òÌå£©");
	QHBoxLayout* hLayout1 = new QHBoxLayout;
	hLayout1->addWidget(new QLabel(tr("Language:")));
	hLayout1->addWidget(_languageComboBox);

	QWidget* generalWidget = new QWidget;
	generalWidget->setLayout(hLayout1);

	_autoFitWindowCheckBox = new QCheckBox(tr("Image display size automatically adapts to window width"));
	QRadioButton* defaultWindowButton = new QRadioButton(tr("Default Window"));
	QRadioButton* fullWindowButton = new QRadioButton(tr("Full Window"));
	_windowGroup = new QButtonGroup;
	_windowGroup->addButton(defaultWindowButton, 0);
	_windowGroup->addButton(fullWindowButton, 1);
	QHBoxLayout* hLayout2 = new QHBoxLayout;
	hLayout2->addWidget(defaultWindowButton);
	hLayout2->addWidget(fullWindowButton);
	QGroupBox* groupBox1 = new QGroupBox(tr("Image display window"));
	groupBox1->setLayout(hLayout2);
	QVBoxLayout* vLayout2 = new QVBoxLayout;
	vLayout2->addWidget(_autoFitWindowCheckBox);
	vLayout2->addWidget(groupBox1);

	QWidget* imageWidget = new QWidget;
	imageWidget->setLayout(vLayout2);

	QLabel* label = new QLabel("Hello Qt");

	QTabWidget* tabWidget = new QTabWidget(this);
	tabWidget->addTab(generalWidget, tr("General"));
	tabWidget->addTab(imageWidget, tr("Image"));
	tabWidget->addTab(label, "Reserved");

	QPushButton* acceptButton = new QPushButton(tr("Accept"));
	connect(acceptButton, &QPushButton::clicked, this, &SettingsDialog::acceptButtonClicked);
	QPushButton* rejectButton = new QPushButton(tr("Cancel"));
	connect(rejectButton, &QPushButton::clicked, this, &QDialog::reject);
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addStretch();
	hLayout->addWidget(acceptButton);
	hLayout->addStretch();
	hLayout->addWidget(rejectButton);
	hLayout->addStretch();

	QVBoxLayout* globalLayout = new QVBoxLayout;
	globalLayout->addWidget(tabWidget);
	globalLayout->addLayout(hLayout);
	setLayout(globalLayout);
}

void SettingsDialog::loadSettings()
{
	QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	int language = settings.value("General/language", 0).toInt();
	bool fitWindow = settings.value("Image/autoFitWindow", 0).toBool();
	int displayWindow = settings.value("Image/displayWindow", 0).toInt();

	_languageComboBox->setCurrentIndex(language);
	_autoFitWindowCheckBox->setChecked(fitWindow);
	_windowGroup->button(displayWindow)->setChecked(true);
}

void SettingsDialog::acceptButtonClicked()
{
	QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	int language = settings.value("General/language", 0).toInt();

	settings.setValue("General/language", _languageComboBox->currentIndex());
	settings.setValue("Image/autoFitWindow", _autoFitWindowCheckBox->isChecked());
	settings.setValue("Image/displayWindow", _windowGroup->checkedId());

	if (language != _languageComboBox->currentIndex())
	{
		// Change language
		emit changeLanguage(_languageComboBox->currentIndex());
	}

	accept();
}