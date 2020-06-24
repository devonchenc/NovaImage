#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QCheckBox;
QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	SettingsDialog(QWidget* parent = nullptr);
	~SettingsDialog();

private:
	void initUI();

	void loadSettings();

private slots:
	void acceptButtonClicked();

private:
	QCheckBox* _autoFitWindowCheckBox;
};
