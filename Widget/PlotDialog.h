#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QGraphicsLineItem;
QT_END_NAMESPACE

class PlotDialog : public QDialog
{
	Q_OBJECT

public:
	PlotDialog(QWidget* parent = nullptr);
	~PlotDialog();

	void setData(QGraphicsLineItem* lineItem, const QVector<qreal>& points);

private:
	void initUI();

private slots:
//	void acceptButtonClicked();
	void deleteLine();

private:
	QTabWidget* _tabWidget;
};
