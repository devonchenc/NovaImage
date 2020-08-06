#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QProgressBar;
QT_END_NAMESPACE

class AbstractReader;

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    ProgressDialog(AbstractReader* thread, QWidget* parent = nullptr);

public slots:
    void setTitle(const QString& title);

    void setProgress(int progress);

protected:
    void closeEvent(QCloseEvent* e) override;

private:
    void initUI();

private:
    AbstractReader* _thread;
    QProgressBar* _progressBar;
};
