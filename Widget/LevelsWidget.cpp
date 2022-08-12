#include "LevelsWidget.h"

#include <QLineEdit>
#include <QHBoxLayout>
#include <QEvent>

#include "../Processor/LevelsProcessor.h"
#include "HistogramWidget.h"
#include "../Core/GlobalFunc.h"
#include "../Image/BaseImage.h"

LevelsWidget::LevelsWidget(QWidget* parent)
    : BaseWidget(parent)
{
    setName(tr("Levels"));

    _histogram = new HistogramWidget();
    connect(_histogram, &HistogramWidget::resetControl, this, &LevelsWidget::resetButtonClicked);
    connect(_histogram, &HistogramWidget::apply, this, &LevelsWidget::apply);
    connect(_histogram, &HistogramWidget::updateBottom, this, &LevelsWidget::updateBottom);
    connect(_histogram, &HistogramWidget::updateMid, this, &LevelsWidget::updateMid);
    connect(_histogram, &HistogramWidget::updateTop, this, &LevelsWidget::updateTop);

    _minEdit = new QLineEdit;
    _minEdit->setText("0");
    _minEdit->setMaximumWidth(100);
    connect(_minEdit, &QLineEdit::editingFinished, this, &LevelsWidget::updateHistogram);
    _midEdit = new QLineEdit;
    _midEdit->setText("1");
    _midEdit->setMaximumWidth(100);
    connect(_midEdit, &QLineEdit::editingFinished, this, &LevelsWidget::updateHistogram);
    _maxEdit = new QLineEdit;
    _maxEdit->setText("255");
    _maxEdit->setMaximumWidth(100);
    connect(_maxEdit, &QLineEdit::editingFinished, this, &LevelsWidget::updateHistogram);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(_minEdit);
    hbox->addStretch();
    hbox->addWidget(_midEdit);
    hbox->addStretch();
    hbox->addWidget(_maxEdit);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(_histogram);
    layout->addLayout(hbox);

    setLayout(layout);

    _processor = new LevelsProcessor(this);
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

    _minEdit->setText(QString::number(minValue));
    _midEdit->setText(QString::number(mid));
    _maxEdit->setText(QString::number(maxValue));

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

        _minEdit->setText(QString::number(minValue));
        _midEdit->setText(QString::number(mid));
        _maxEdit->setText(QString::number(maxValue));
    }
    else
    {
        _minEdit->setText("0");
        _midEdit->setText("1.0");
        _maxEdit->setText("255");
    }

    _histogram->reset();
}

// Levels adjust
void LevelsWidget::levelsAdjust(float bottom, float mid, float top)
{
    _minEdit->setText(QString::number(bottom));
    _midEdit->setText(QString::number(mid));
    _maxEdit->setText(QString::number(top));

    BaseImage* image = getGlobalImage();
    if (image)
    {
        _processor->setPara(bottom, mid, top);
        _processor->processForView(image);
    }
}

void LevelsWidget::setParameters(float bottom, float mid, float top)
{
    BaseImage* image = getGlobalImage();
    if (image)
    {
        float minValue = image->getMinValue();
        float maxValue = image->getMaxValue();
        if (bottom < minValue)
        {
            _minEdit->setText(QString::number(minValue));
        }
        else if (bottom > top)
        {
            _minEdit->setText(QString::number(top));
        }
        if (top < bottom)
        {
            _maxEdit->setText(QString::number(bottom));
        }
        else if (top > maxValue)
        {
            _maxEdit->setText(QString::number(maxValue));
        }
    }
    else
    {
        if (bottom < 0)
        {
            _minEdit->setText("0");
        }
        else if (bottom > top)
        {
            _minEdit->setText(QString::number(top));
        }
        if (top < bottom)
        {
            _maxEdit->setText(QString::number(bottom));
        }
        else if (top > 255)
        {
            _maxEdit->setText("255");
        }
    }

    _histogram->setParameters(bottom, mid, top);
}

void LevelsWidget::resetButtonClicked()
{
    reset();

    float bottom = _minEdit->text().toFloat();
    float mid = _midEdit->text().toFloat();
    float top = _maxEdit->text().toFloat();
    levelsAdjust(bottom, mid, top);
}

void LevelsWidget::apply()
{
    _processor->apply();

    reset();
}

void LevelsWidget::updateHistogram()
{
    float bottom = _minEdit->text().toFloat();
    float mid = _midEdit->text().toFloat();
    float top = _maxEdit->text().toFloat();

    setParameters(bottom, mid, top);

    levelsAdjust(bottom, mid, top);
}

void LevelsWidget::updateBottom(float bottom)
{
    _minEdit->setText(QString::number(bottom));

    float mid = _midEdit->text().toFloat();
    float top = _maxEdit->text().toFloat();
    levelsAdjust(bottom, mid, top);
}

void LevelsWidget::updateMid(float mid)
{
    _midEdit->setText(QString::number(mid));

    float bottom = _minEdit->text().toFloat();
    float top = _maxEdit->text().toFloat();
    levelsAdjust(bottom, mid, top);
}

void LevelsWidget::updateTop(float top)
{
    _maxEdit->setText(QString::number(top));

    float bottom = _minEdit->text().toFloat();
    float mid = _midEdit->text().toFloat();
    levelsAdjust(bottom, mid, top);
}

void LevelsWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        parentWidget()->setWindowTitle(tr("Levels"));
    }
    BaseWidget::changeEvent(event);
}