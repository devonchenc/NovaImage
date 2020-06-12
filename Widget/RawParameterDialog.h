#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class RawParameterDialog : public QDialog
{
	Q_OBJECT

public:
	RawParameterDialog(const QString& pathName, QWidget* parent = nullptr);
	~RawParameterDialog();

public:
	int dataType() { return _dataType; }

	int width() { return _width; }

	int height() { return _height; }

private:
	void initUI();

private:
	QString _fileName;
	QLabel* _actualSize;

	int _dataType;
	int _width;
	int _height;
};
