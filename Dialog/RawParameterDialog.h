#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QLineEdit;
class QButtonGroup;
QT_END_NAMESPACE

class RawParameterDialog : public QDialog
{
    Q_OBJECT

public:
    RawParameterDialog(const QString& pathName, QWidget* parent = nullptr);

public:
    int dataType() { return _dataType; }

    int width() { return _width; }

    int height() { return _height; }

    int slice() { return _slice; }

    int headerSize() { return _headerSize; }

    int endian() { return _endian; }

private:
    void initUI();

    void loadSettings();

private slots:
    void updateExpectedSize();

    void acceptButtonClicked();

private:
    QString _fileName;
    QLabel* _expectedSizeLabel;
    QComboBox* _typeComboBox;
    QComboBox* _widthComboBox;
    QComboBox* _heightComboBox;
    QComboBox* _sliceComboBox;
    QLineEdit* _headerEdit;
    QButtonGroup* _endianGroup;

    qint64 _actualSize;
    qint64 _expectedSize;
    int _dataType;
    int _width;
    int _height;
    int _slice;
    int _headerSize;
    int _endian;
};
