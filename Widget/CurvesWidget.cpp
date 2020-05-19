#include "CurvesWidget.h"

#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QEvent>

#include "CurvesProcessor.h"

CurvesWidget::CurvesWidget(QWidget* parent)
	: BaseWidget(parent)
	, _square(nullptr)
{
	setName(tr("Curves"));

	_labelChannel = new QLabel(tr("Channel"));
	_comboboxChannel = new QComboBox();
	_comboboxChannel->addItem("RGB");
	_comboboxChannel->addItem(tr("Red"));
	_comboboxChannel->addItem(tr("Green"));
	_comboboxChannel->addItem(tr("Blue"));
	connect(_comboboxChannel, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CurvesWidget::channelChanged);
	_buttonReset = new QPushButton(tr("&Reset"));
	connect(_buttonReset, &QPushButton::clicked, this, &CurvesWidget::clickReset);
	_buttonReverse = new QPushButton(tr("R&everse"));
	connect(_buttonReverse, &QPushButton::clicked, this, &CurvesWidget::clickReverse);

	QHBoxLayout* layoutHead = new QHBoxLayout();
	layoutHead->addWidget(_labelChannel);
	layoutHead->addWidget(_comboboxChannel);
	layoutHead->addWidget(_buttonReset);
	layoutHead->addWidget(_buttonReverse);

	_labelInput = new QLabel(tr("Input:"));
	_labelInputValue = new QLabel();
	_labelOutput = new QLabel(tr("Output:"));
	_labelOutputValue = new QLabel(tr(""));
	QGridLayout* grid = new QGridLayout();
	grid->addWidget(_labelInput, 0, 0);
	grid->addWidget(_labelInputValue, 0, 1);
	grid->addWidget(_labelOutput, 1, 0);
	grid->addWidget(_labelOutputValue, 1, 1);
	QGroupBox* groupBox1 = new QGroupBox(tr(""));
	groupBox1->setLayout(grid);

	_radioCurve = new QRadioButton(tr("&Curve"));
	_radioLinear = new QRadioButton(tr("&Line"));
	connect(_radioCurve, &QRadioButton::toggled, this, &CurvesWidget::toggleCurveRadio);
	connect(_radioLinear, &QRadioButton::toggled, this, &CurvesWidget::toggleLinearRadio);
	_radioCurve->setChecked(true);
	QVBoxLayout* vbox = new QVBoxLayout();
	vbox->addWidget(_radioCurve);
	vbox->addWidget(_radioLinear);
	QGroupBox* groupBox2 = new QGroupBox(tr(""));
	groupBox2->setLayout(vbox);

	_buttonSave = new QPushButton(tr("&Save"));
	connect(_buttonSave, &QPushButton::clicked, this, &CurvesWidget::clickSave);
	_buttonLoad = new QPushButton(tr("L&oad"));
	connect(_buttonLoad, &QPushButton::clicked, this, &CurvesWidget::clickLoad);
	QVBoxLayout* vbox2 = new QVBoxLayout();
	vbox2->addWidget(_buttonSave);
	vbox2->addWidget(_buttonLoad);
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
	_processor->setArray(_square->getSize(), _square->getIntensity(), _square->getRed(), _square->getGreen(), _square->getBlue());
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

	_comboboxChannel->setCurrentIndex(CURVE_CHANNEL_GRAY);
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
	if (_square && _radioCurve->isChecked())
	{
		_square->setCurveOrLinear(true);
	}
}

void CurvesWidget::toggleLinearRadio()
{
	if (_square && _radioLinear->isChecked())
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
			_radioCurve->setChecked(curveOrLinear);
			_radioLinear->setChecked(!curveOrLinear);

			_square->setChannel(CURVE_CHANNEL_GRAY);
			loadPegArray(stream, _square->getIntensityPegsArray());
			_square->setChannel(CURVE_CHANNEL_RED);
			loadPegArray(stream, _square->getRedPegsArray());
			_square->setChannel(CURVE_CHANNEL_GREEN);
			loadPegArray(stream, _square->getGreenPegsArray());
			_square->setChannel(CURVE_CHANNEL_BLUE);
			loadPegArray(stream, _square->getBluePegsArray());

			_square->setChannel(channel);
			_comboboxChannel->setCurrentIndex(channel);

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
		_processor->process(image);
 
		repaintView();
	}
//	_isProcessing = false;
}

void CurvesWidget::updateLabelText(QString input, QString output)
{
	_labelInputValue->setText(input);
	_labelOutputValue->setText(output);
}

void CurvesWidget::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		parentWidget()->setWindowTitle(tr("Curves"));

		_labelChannel->setText(tr("Channel"));
		_buttonReset->setText(tr("&Reset"));
		_buttonReverse->setText(tr("R&everse"));
		_labelInput->setText(tr("Input:"));
		_labelOutput->setText(tr("Output:"));

		_comboboxChannel->clear();
		_comboboxChannel->addItem("RGB");
		_comboboxChannel->addItem(tr("Red"));
		_comboboxChannel->addItem(tr("Green"));
		_comboboxChannel->addItem(tr("Blue"));

		_radioCurve->setText(tr("&Curve"));
		_radioLinear->setText(tr("&Line"));

		_buttonSave->setText(tr("&Save"));
		_buttonLoad->setText(tr("L&oad"));
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
	_processor->setArray(_square->getSize(), _square->getIntensity(), _square->getRed(), _square->getGreen(), _square->getBlue());
	_processor->setChannel(_square->getChannel());
}