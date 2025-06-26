#pragma once

#include <QThread>
#include <QFile>
#include <QMessageBox>

#include "AbstractReader.h"

class MultiDicomReader : public AbstractReader
{
public:
    MultiDicomReader(const QString& pathName, const QStringList& fileList, std::shared_ptr<QVector<std::shared_ptr<DICOMImage>>> imageVector)
        : AbstractReader(pathName, nullptr)
        , _fileList(fileList)
        , _fileNum(fileList.size())
        , _imageVector(imageVector)
    {
    }

protected:
    void run() override
    {
        QDir dir(_pathName);
        if (!dir.exists())
        {
            QMetaObject::invokeMethod(_widget, "reject", Qt::QueuedConnection);
            return;
        }

        QMetaObject::invokeMethod(_widget, "setTitle", Qt::QueuedConnection, Q_ARG(const QString&, tr("Loading DICOM files...")));

        for (int n = 0; n < _fileNum; n++)
        {
            readOneDicom(dir, n);
        }

        finished();
    }

private:
    void readOneDicom(const QDir& dir, int index)
    {
        if (_waitForQuit)
            return;

        QString fileName = dir.absoluteFilePath(_fileList[index]);
        std::shared_ptr<DICOMImage> dicomImage = std::make_shared<DICOMImage>(fileName);

        // Judge whether open file successfully
        if (!dicomImage->isOpenSucceed())
        {
            QMessageBox::critical(nullptr, tr("Open DICOM file error"),
                tr("Open DICOM ") + fileName + tr(" error!"), QMessageBox::Ok);
            QMetaObject::invokeMethod(_widget, "reject", Qt::QueuedConnection);
            return;
        }

        _imageVector->append(dicomImage);

        int progress = (index + 1) * 100 / _fileNum;
        QMetaObject::invokeMethod(_widget, "setProgress", Qt::QueuedConnection, Q_ARG(int, progress));
    }

private:
    const QStringList _fileList;
    const int _fileNum;
    std::shared_ptr<QVector<std::shared_ptr<DICOMImage>>> _imageVector;
};
