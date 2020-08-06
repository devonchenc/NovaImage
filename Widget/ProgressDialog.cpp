#include "ProgressDialog.h"

#include <QProgressBar>
#include <QVBoxLayout>
#include <QThread>

#include "../Image/ImageReader.h"

ProgressDialog::ProgressDialog(AbstractReader* thread, QWidget* parent)
    : QDialog(parent)
    , _thread(thread)
{
    setWindowTitle(tr("Progress"));

    setWindowFlag(Qt::Popup);

    initUI();

    resize(400, 80);
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
}

void ProgressDialog::initUI()
{
    _progressBar = new QProgressBar;
    _progressBar->setOrientation(Qt::Horizontal);
    _progressBar->setMinimum(0);
    _progressBar->setMaximum(100);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(_progressBar);
    setLayout(vLayout);
}
