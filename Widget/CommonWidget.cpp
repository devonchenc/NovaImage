#include "CommonWidget.h"

#include <QGridLayout>
#include <QEvent>

#include "../Processor/CommonProcessor.h"
#include "../Core/GlobalFunc.h"

CommonWidget::CommonWidget(QWidget* parent)
    : BaseWidget(parent)
{
    setName(tr("Common"));

    _brightLabel = new QLabel(tr("Bright"));
    _contrastLabel = new QLabel(tr("Contrast"));
    _brightSlider = new QSlider(Qt::Orientation::Horizontal);
    _brightSlider->setMinimum(-100);
    _brightSlider->setMaximum(100);
    connect(_brightSlider, &QSlider::valueChanged, this, &CommonWidget::brightValueChanged);
    _contrastSlider = new QSlider(Qt::Orientation::Horizontal);
    _contrastSlider->setMinimum(-100);
    _contrastSlider->setMaximum(100);
    connect(_contrastSlider, &QSlider::valueChanged, this, &CommonWidget::contrastValueChanged);
    _brightValueLabel = new QLabel("0");
    _brightValueLabel->setFixedWidth(20);
    _contrastValueLabel = new QLabel("0");
    _contrastValueLabel->setFixedWidth(20);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(_brightLabel, 0, 0);
    layout->addWidget(_brightSlider, 0, 1);
    layout->addWidget(_brightValueLabel, 0, 2);
    layout->addWidget(_contrastLabel, 1, 0);
    layout->addWidget(_contrastSlider, 1, 1);
    layout->addWidget(_contrastValueLabel, 1, 2);

    setLayout(layout);

    _processor = new CommonProcessor(this);
}

CommonWidget::~CommonWidget()
{

}

void CommonWidget::init()
{
    reset();
}

void CommonWidget::reset()
{
    _brightSlider->setValue(0);
    _contrastSlider->setValue(0);
    _brightValueLabel->setText(QString::number(0));
    _contrastValueLabel->setText(QString::number(0));
}

void CommonWidget::brightValueChanged(int value)
{
    _brightValueLabel->setText(QString::number(value));

    setBrightnessAndContrast();
}

void CommonWidget::contrastValueChanged(int value)
{
    _contrastValueLabel->setText(QString::number(value));

    setBrightnessAndContrast();
}

void CommonWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        parentWidget()->setWindowTitle(tr("Common"));

        _brightLabel->setText(tr("Bright"));
        _contrastLabel->setText(tr("Contrast"));
    }
    BaseWidget::changeEvent(event);
}

void CommonWidget::setBrightnessAndContrast()
{
    BaseImage* image = getGlobalImage();
    if (image)
    {
        _processor->setBrightnessAndContrast(_brightSlider->value(), _contrastSlider->value());
        _processor->setImage(image);
        _processor->process();

        repaintView();
    }
}
