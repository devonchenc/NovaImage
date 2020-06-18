#include "CommonWidget.h"

#include <QGridLayout>
#include <QEvent>

#include "../Processor/CommonProcessor.h"
#include "../Core/GlobalFunc.h"

CommonWidget::CommonWidget(QWidget* parent)
	: BaseWidget(parent)
{
	setName(tr("Common"));

	_labelBright = new QLabel(tr("Bright"));
	_labelContrast = new QLabel(tr("Contrast"));
	_sliderBright = new QSlider(Qt::Orientation::Horizontal);
	_sliderBright->setMinimum(-100);
	_sliderBright->setMaximum(100);
	connect(_sliderBright, &QSlider::valueChanged, this, &CommonWidget::brightValueChanged);
	_sliderContrast = new QSlider(Qt::Orientation::Horizontal);
	_sliderContrast->setMinimum(-100);
	_sliderContrast->setMaximum(100);
	connect(_sliderContrast, &QSlider::valueChanged, this, &CommonWidget::contrastValueChanged);
	_labelBrightNum = new QLabel("0");
	_labelContrastNum = new QLabel("0");

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(_labelBright, 0, 0);
	layout->addWidget(_sliderBright, 0, 1);
	layout->addWidget(_labelBrightNum, 0, 2);
	layout->addWidget(_labelContrast, 1, 0);
	layout->addWidget(_sliderContrast, 1, 1);
	layout->addWidget(_labelContrastNum, 1, 2);

	setLayout(layout);

	_processor = new CommonProcessor;
}

CommonWidget::~CommonWidget()
{
	if (_processor)
	{
		delete _processor;
	}
}

void CommonWidget::init()
{
	reset();
}

void CommonWidget::reset()
{
	_sliderBright->setValue(0);
	_sliderContrast->setValue(0);
	_labelBrightNum->setText(QString::number(0));
	_labelContrastNum->setText(QString::number(0));
}

void CommonWidget::brightValueChanged(int x)
{
	_labelBrightNum->setText(QString::number(x));

	SetBrightnessAndContrast();
}

void CommonWidget::contrastValueChanged(int x)
{
	_labelContrastNum->setText(QString::number(x));

	SetBrightnessAndContrast();
}

void CommonWidget::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		parentWidget()->setWindowTitle(tr("Common"));

		_labelBright->setText(tr("Bright"));
		_labelContrast->setText(tr("Contrast"));
	}
	BaseWidget::changeEvent(event);
}

void CommonWidget::SetBrightnessAndContrast()
{
	BaseImage* image = getGlobalImage();
	if (image)
	{
		_processor->setBrightnessAndContrast(_sliderBright->value(), _sliderContrast->value());
		_processor->process(image);

		repaintView();
	}
}