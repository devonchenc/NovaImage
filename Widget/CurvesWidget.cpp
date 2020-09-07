#include "CurvesWidget.h"

#include <QLabel>
#include <QRadioButton>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QEvent>

#include "../Processor/CurvesProcessor.h"
#include "../Core/GlobalFunc.h"
#include "../Image/BaseImage.h"

CurvesWidget::CurvesWidget(QWidget* parent)
    : BaseWidget(parent)
    , _square(nullptr)
{
    setName(tr("Curves"));

    _channelLabel = new QLabel(tr("Channel"));
    _channelComboBox = new QComboBox();
    _channelComboBox->addItem("RGB");
    _channelComboBox->addItem(tr("Red"));
    _channelComboBox->addItem(tr("Green"));
    _channelComboBox->addItem(tr("Blue"));
    connect(_channelComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CurvesWidget::channelChanged);
    _resetButton = new QPushButton(tr("&Reset"));
    connect(_resetButton, &QPushButton::clicked, this, &CurvesWidget::clickReset);
    _reverseButton = new QPushButton(tr("R&everse"));
    connect(_reverseButton, &QPushButton::clicked, this, &CurvesWidget::clickReverse);

    QHBoxLayout* layoutHead = new QHBoxLayout();
    layoutHead->addWidget(_channelLabel);
    layoutHead->addWidget(_channelComboBox);
    layoutHead->addWidget(_resetButton);
    layoutHead->addWidget(_reverseButton);

    _inputLabel = new QLabel(tr("Input:"));
    _inputValueLabel = new QLabel();
    _outputLabel = new QLabel(tr("Output:"));
    _outputValueLabel = new QLabel(tr(""));
    QGridLayout* grid = new QGridLayout;
    grid->addWidget(_inputLabel, 0, 0);
    grid->addWidget(_inputValueLabel, 0, 1);
    grid->addWidget(_outputLabel, 1, 0);
    grid->addWidget(_outputValueLabel, 1, 1);
    QGroupBox* groupBox1 = new QGroupBox(tr(""));
    groupBox1->setLayout(grid);

    _curveRadio = new QRadioButton(tr("&Curve"));
    _linearRadio = new QRadioButton(tr("&Line"));
    connect(_curveRadio, &QRadioButton::toggled, this, &CurvesWidget::toggleCurveRadio);
    connect(_linearRadio, &QRadioButton::toggled, this, &CurvesWidget::toggleLinearRadio);
    _curveRadio->setChecked(true);
    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(_curveRadio);
    vbox->addWidget(_linearRadio);
    QGroupBox* groupBox2 = new QGroupBox(tr(""));
    groupBox2->setLayout(vbox);

    _saveButton = new QPushButton(tr("&Save"));
    connect(_saveButton, &QPushButton::clicked, this, &CurvesWidget::clickSave);
    _loadButton = new QPushButton(tr("L&oad"));
    connect(_loadButton, &QPushButton::clicked, this, &CurvesWidget::clickLoad);
    QVBoxLayout* vbox2 = new QVBoxLayout();
    vbox2->addWidget(_saveButton);
    vbox2->addWidget(_loadButton);
    QHBoxLayout* layoutBottom = new QHBoxLayout();
    layoutBottom->addWidget(groupBox1);
    layoutBottom->addWidget(groupBox2);
    layoutBottom->addLayout(vbox2);

    _square = new CurveSquare();
    connect(_square, &CurveSquare::resize, this, &CurvesWidget::resizeSquare);
    connect(_square, &CurveSquare::updateImage, this, &CurvesWidget::updateImage);
    connect(_square, &CurveSquare::updateLabelText, this, &CurvesWidget::updateLabelText);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(layoutHead);
    layout->addWidget(_square);
    layout->addLayout(layoutBottom);

    setLayout(layout);

    _processor = new CurvesProcessor;
    _processor->setArray(_square->getArraySize(), _square->getIntensity(), _square->getRed(), _square->getGreen(), _square->getBlue());
    _processor->setChannel(_square->getChannel());
}

CurvesWidget::~CurvesWidget()
{
    if (_processor)
    {
        delete _processor;
    }
}

void CurvesWidget::init()
{
    _square->setChannel(CURVE_CHANNEL_GRAY);
    _square->init();

    _channelComboBox->setCurrentIndex(CURVE_CHANNEL_GRAY);
    generateHistogram();
}

void CurvesWidget::reset()
{
    _square->reset();
    connectSqureWithProcessor();
}

void CurvesWidget::channelChanged(int value)
{
    _square->setChannel(value);
    _processor->setChannel(value);

    if (value == 0)
    {
        // Channel changed from color to gray
        updateImage();
    }

    generateHistogram();
}

void CurvesWidget::clickReset()
{
    reset();

    updateImage();
}

void CurvesWidget::clickReverse()
{
    _square->reverse();
}

void CurvesWidget::toggleCurveRadio()
{
    if (_square && _curveRadio->isChecked())
    {
        _square->setCurveOrLinear(true);
    }
}

void CurvesWidget::toggleLinearRadio()
{
    if (_square && _linearRadio->isChecked())
    {
        _square->setCurveOrLinear(false);
    }
}

void CurvesWidget::clickSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Curve"), "/", tr("Curve file (*.cur)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&file);
            stream << _square->getChannel() << endl;
            stream << int(_square->getCurveOrLinear()) << endl;
            savePegArray(stream, _square->getIntensityPegsArray());
            savePegArray(stream, _square->getRedPegsArray());
            savePegArray(stream, _square->getGreenPegsArray());
            savePegArray(stream, _square->getBluePegsArray());
        }
    }
}

void CurvesWidget::savePegArray(QTextStream& stream, const PegArray& pegs)
{
    stream << pegs.size() << endl;
    for (int i = 0; i < pegs.size(); i++)
    {
        stream << pegs[i].realX() << " " << pegs[i].realY() << endl;
    }
}

void CurvesWidget::clickLoad()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Curve"), "/", tr("Curve file (*.cur)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            int channel, curveOrLinear;
            stream >> channel;
            stream >> curveOrLinear;
            _square->setCurveOrLinear(curveOrLinear);
            _curveRadio->setChecked(curveOrLinear);
            _linearRadio->setChecked(!curveOrLinear);

            _square->setChannel(CURVE_CHANNEL_GRAY);
            loadPegArray(stream, _square->getIntensityPegsArray());
            _square->setChannel(CURVE_CHANNEL_RED);
            loadPegArray(stream, _square->getRedPegsArray());
            _square->setChannel(CURVE_CHANNEL_GREEN);
            loadPegArray(stream, _square->getGreenPegsArray());
            _square->setChannel(CURVE_CHANNEL_BLUE);
            loadPegArray(stream, _square->getBluePegsArray());

            _square->setChannel(channel);
            _channelComboBox->setCurrentIndex(channel);

            updateImage();
        }
    }
}

void CurvesWidget::loadPegArray(QTextStream& stream, PegArray& pegs)
{
    int size;
    stream >> size;
    pegs.clear();
    for (int i = 0; i < size; i++)
    {
        qreal x, y;
        stream >> x >> y;
        pegs.append(Peg(x, y));
    }
    _square->calcArrayValue();
}

void CurvesWidget::resizeSquare()
{
    generateHistogram();
    connectSqureWithProcessor();
}

void CurvesWidget::updateImage()
{
    //	_isProcessing = true;

    BaseImage* image = getGlobalImage();
    if (image)
    {
        _processor->setImage(image);
        _processor->process();

        repaintView();
    }
    //	_isProcessing = false;
}

void CurvesWidget::updateLabelText(QString input, QString output)
{
    _inputValueLabel->setText(input);
    _outputValueLabel->setText(output);
}

void CurvesWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        parentWidget()->setWindowTitle(tr("Curves"));

        _channelLabel->setText(tr("Channel"));
        _resetButton->setText(tr("&Reset"));
        _reverseButton->setText(tr("R&everse"));
        _inputLabel->setText(tr("Input:"));
        _outputLabel->setText(tr("Output:"));

        _channelComboBox->clear();
        _channelComboBox->addItem("RGB");
        _channelComboBox->addItem(tr("Red"));
        _channelComboBox->addItem(tr("Green"));
        _channelComboBox->addItem(tr("Blue"));

        _curveRadio->setText(tr("&Curve"));
        _linearRadio->setText(tr("&Line"));

        _saveButton->setText(tr("&Save"));
        _loadButton->setText(tr("L&oad"));
    }
    BaseWidget::changeEvent(event);
}

void CurvesWidget::generateHistogram()
{
    BaseImage* image = getGlobalImage();
    if (image == nullptr)
        return;

    // Generate histogram according to current channel
    switch (_square->getChannel())
    {
    case CURVE_CHANNEL_GRAY:
    {
        _square->generateHistogram(image->getGrayPixelArray());
    }
        break;
    case CURVE_CHANNEL_RED:
    {
        _square->generateHistogram(image->getRedPixelArray());
    }
        break;
    case CURVE_CHANNEL_GREEN:
    {
        _square->generateHistogram(image->getGreenPixelArray());
    }
        break;
    case CURVE_CHANNEL_BLUE:
    {
        _square->generateHistogram(image->getBluePixelArray());
    }
        break;
    }
}

void CurvesWidget::connectSqureWithProcessor()
{
    _processor->setArray(_square->getArraySize(), _square->getIntensity(), _square->getRed(), _square->getGreen(), _square->getBlue());
    _processor->setChannel(_square->getChannel());
}
