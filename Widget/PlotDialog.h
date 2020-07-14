#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class PlotDialog : public QDialog
{
	Q_OBJECT

public:
	PlotDialog(QWidget* parent = nullptr);
	~PlotDialog();


private:
	void initUI();

private slots:
//	void acceptButtonClicked();

private:
/*	QComboBox* _languageComboBox;
	QComboBox* _leftMouseComboBox;
	QComboBox* _rightMouseComboBox;
	QCheckBox* _autoFitWindowCheckBox;
	QButtonGroup* _windowGroup;*/
};
