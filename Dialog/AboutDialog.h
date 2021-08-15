#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog() {}

public:


private:
    void initUI();

private:
    QLabel* _expectedSizeLabel;
};
