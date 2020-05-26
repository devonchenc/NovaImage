#include "LevelsWidget.h"

#include <QLineEdit>
#include <QHBoxLayout>
#include <QEvent>

#include "LevelsProcessor.h"
#include "HistogramWidget.h"
#include "../GlobalFunc.h"
#include "../Image/BaseImage.h"

LevelsWidget::LevelsWidget(QWidget* parent)
	: BaseWidget(parent)
{
	setName(tr("Levels"));

	_histogram = new HistogramWidget();
	connect(_histogram, &HistogramWidget::resetControl, this, &LevelsWidget::reset);
	connect(_histogram, &HistogramWidget::updateBottom, this, &LevelsWidget::updateBottom);
	connect(_histogram, &HistogramWidget::updateMid, this, &LevelsWidget::updateMid);
	connect(_histogram, &HistogramWidget::updateTop, this, &LevelsWidget::updateTop);

	_editMin = new QLineEdit();
	_editMin->setText("0");
	_editMin->setMaximumWidth(100);
	connect(_editMin, &QLineEdit::editingFinished, this, &LevelsWidget::updateHistogram);
	_editMid = new QLineEdit();
	_editMid->setText("1");
	_editMid->setMaximumWidth(100);
	connect(_editMid, &QLineEdit::editingFinished, this, &LevelsWidget::updateHistogram);
	_editMax = new QLineEdit();
	_editMax->setText("255");
	_editMax->setMaximumWidth(100);
	connect(_editMax, &QLineEdit::editingFinished, this, &LevelsWidget::updateHistogram);

	QHBoxLayout* hbox = new QHBoxLayout;
	hbox->addWidget(_editMin);
	hbox->addStretch();
	hbox->addWidget(_editMid);
	hbox->addStretch();
	hbox->addWidget(_editMax);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_histogram);
	layout->addLayout(hbox);

	setLayout(layout);

	_processor = new LevelsProcessor;
}

LevelsWidget::~LevelsWidget()
{
	if (_processor)
	{
		delete _processor;
	}
}

void LevelsWidget::init()
{
	BaseImage* image = getGlobalImage();
	assert(image);

	float minValue = image->getMinValue();
	float maxValue = image->getMaxValue();
	float mid = 1.0f;

	_editMin->setText(QString::number(minValue));
	_editMid->setText(QString::number(mid));
	_editMax->setText(QString::number(maxValue));

	_histogram->init();
}

void LevelsWidget::reset()
{
	BaseImage* image = getGlobalImage();
	if (image)
	{
		float minValue = image->getMinValue();
		float maxValue = image->getMaxValue();
		float mid = 1.0f;

		_editMin->setText(QString::number(minValue));
		_editMid->setText(QString::number(mid));
		_editMax->setText(QString::number(maxValue));
	}
	else
	{
		_editMin->setText("0");
		_editMid->setText("1.0");
		_editMax->setText("255");
	}

	_histogram->reset();

	levelsAdjust();
}

void LevelsWidget::updateHistogram()
{
	float bottom = _editMin->text().toFloat();
	float mid = _editMid->text().toFloat();
	float top = _editMax->text().toFloat();

	BaseImage* image = getGlobalImage();
	if (image)
	{
		float minValue = image->getMinValue();
		float maxValue = image->getMaxValue();
		if (bottom < minValue)
		{
			_editMin->setText(QString::number(minValue));
		}
		else if (bottom > top)
		{
			_editMin->setText(QString::number(top));
		}
		if (top < bottom)
		{
			_editMax->setText(QString::number(bottom));
		}
		else if (top > maxValue)
		{
			_editMax->setText(QString::number(minValue));
		}
	}
	else
	{
		if (bottom < 0)
		{
			_editMin->setText("0");
		}
		else if (bottom > top)
		{
			_editMin->setText(QString::number(top));
		}
		if (top < bottom)
		{
			_editMax->setText(QString::number(bottom));
		}
		else if (top > 255)
		{
			_editMax->setText("255");
		}
	}

	_histogram->setParameters(bottom, mid, top);

	levelsAdjust();
}

void LevelsWidget::updateBottom(float bottom)
{
	_editMin->setText(QString::number(bottom));

	levelsAdjust();
}

void LevelsWidget::updateMid(float mid)
{
	_editMid->setText(QString::number(mid));

	levelsAdjust();
}

void LevelsWidget::updateTop(float top)
{
	_editMax->setText(QString::number(top));

	levelsAdjust();
}

// Levels adjust
void LevelsWidget::levelsAdjust()
{
	BaseImage* image = getGlobalImage();
	if (image)
	{
		float bottom = _editMin->text().toFloat();
		float mid = _editMid->text().toFloat();
		float top = _editMax->text().toFloat();
		_processor->setPara(bottom, mid, top);
		_processor->process(image);

		repaintView();
	}
}

void LevelsWidget::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		parentWidget()->setWindowTitle(tr("Levels"));
	}
	BaseWidget::changeEvent(event);
}