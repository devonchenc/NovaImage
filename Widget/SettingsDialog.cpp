#include "SettingsDialog.h"

#include <QTabWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSettings>
#include <QCoreApplication>

SettingsDialog::SettingsDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Preferences"));

	setWindowFlag(Qt::Popup);
	
	initUI();

	loadSettings();
}

SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::initUI()
{
	_autoFitWindowCheckBox = new QCheckBox(tr("Image display size automatically adapts to window width"));
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(_autoFitWindowCheckBox);

	QWidget* widget = new QWidget;
	widget->setLayout(vLayout);

	QLabel* label = new QLabel("Hello Qt");

	QTabWidget* tabWidget = new QTabWidget(this);
	tabWidget->addTab(widget, tr("Image"));
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
	QSettings setting(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	bool fitWindow = setting.value("Image/autoFitWindow", 1).toBool();

	_autoFitWindowCheckBox->setChecked(fitWindow);
}

void SettingsDialog::acceptButtonClicked()
{
	QSettings setting(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	setting.setValue("Image/autoFitWindow", _autoFitWindowCheckBox->isChecked());

	accept();
}