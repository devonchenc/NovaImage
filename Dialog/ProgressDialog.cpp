#include "ProgressDialog.h"

#include <QProgressBar>
#include <QVBoxLayout>
#include <QThread>
#include <QTimer>

#include "../Image/Reader/ImageReader.h"

ProgressDialog::ProgressDialog(AbstractReader* thread, QWidget* parent)
    : QDialog(parent)
    , _thread(thread)
{
    setWindowTitle(tr("Progress"));

    setWindowFlag(Qt::Popup);

    initUI();

    setFixedSize(400, 80);

    // Delay execution of initialization logic until after the dialog is shown
    QTimer::singleShot(0, this, [this]() {
        _thread->setWidget(this);
        _thread->start();
    });
}

void ProgressDialog::setTitle(const QString& title)
{
    setWindowTitle(title);
}

void ProgressDialog::setProgress(int progress)
{
    _progressBar->setValue(progress);
}

void ProgressDialog::closeEvent(QCloseEvent*)
{
    _thread->waitForQuit();
    _thread->quit();
    _thread->wait();
}

void ProgressDialog::initUI()
{
    _progressBar = new QProgressBar;
    _progressBar->setOrientation(Qt::Horizontal);
    _progressBar->setMinimum(0);
    _progressBar->setMaximum(100);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(_progressBar);
    setLayout(vLayout);
}
