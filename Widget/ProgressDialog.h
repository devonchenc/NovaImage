#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QProgressBar;
QT_END_NAMESPACE

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    ProgressDialog(QWidget* parent = nullptr);

public slots:
    void setTitle(const QString& title);

    void setProgress(int progress);

private:
    void initUI();

private slots:
//    void acceptButtonClicked();

private:
    QProgressBar* _progressBar;
};
