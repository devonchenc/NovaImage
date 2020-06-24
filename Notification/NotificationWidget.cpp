#include "NotificationWidget.h"

#include <QString>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QGraphicsOpacityEffect>

namespace NotificationWidgetDetails
{
	static std::vector<std::pair<QMessageBox::Icon, NotificationParams::Type>> notificationIconsConvertor =
	{
	  std::make_pair(QMessageBox::Information, NotificationParams::Information),
	  std::make_pair(QMessageBox::Warning, NotificationParams::Warning),
	  std::make_pair(QMessageBox::Critical, NotificationParams::Critical)
	};

	QMessageBox::Icon Convert(const NotificationParams::Type& type)
	{
		using IconNode = std::pair<QMessageBox::Icon, NotificationParams::Type>;
		auto iter = std::find_if(notificationIconsConvertor.begin(), notificationIconsConvertor.end(), [type](const IconNode& node)
		{
			return node.second == type;
		});
		Q_ASSERT(iter != notificationIconsConvertor.end());
		return iter->first;
	}

	QString ColorToHTML(const QColor& color)
	{
		QString ret = QString("#%1%2%3%4")
			.arg(color.alpha(), 2, 16, QChar('0'))
			.arg(color.red(), 2, 16, QChar('0'))
			.arg(color.green(), 2, 16, QChar('0'))
			.arg(color.blue(), 2, 16, QChar('0'));
		return ret;
	}
} //namespace NotificationWidgetDetails

NotificationWidget::NotificationWidget(const NotificationParams& params, QWidget* parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

	setAttribute(Qt::WA_TranslucentBackground); // Indicates that the background will be transparent
	setAttribute(Qt::WA_ShowWithoutActivating); // At the show, the widget does not get the focus automatically

	_effect = new QGraphicsOpacityEffect;
	_effect->setOpacity(0.85);
	setGraphicsEffect(_effect);

	initUI(params);

	QRect geometry = QApplication::desktop()->availableGeometry();
	setFixedWidth(geometry.width() / 6);
	setMaximumHeight(geometry.height() / 3);
}

void NotificationWidget::onCloseButtonClicked()
{
	emit closeButtonClicked(this);
}

void NotificationWidget::onDetailsButtonClicked()
{
	emit detailsButtonClicked(this);
}

void NotificationWidget::initUI(const NotificationParams& params)
{
	QHBoxLayout* titleLayout = new QHBoxLayout;
	titleLayout->setSpacing(12);

	QFontMetrics fm(font());
	int fontHeight = fm.height() * 2;
	QLabel* iconLabel = new QLabel();
	QMessageBox::Icon icon = NotificationWidgetDetails::Convert(params.type);
	QPixmap image = QMessageBox::standardIcon(icon).scaled(QSize(fontHeight, fontHeight), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	iconLabel->setPixmap(image);
	iconLabel->setScaledContents(false);
	titleLayout->addWidget(iconLabel);

	if (params.title.isEmpty() == false)
	{
		QString title = params.title;
		QLabel* labelTitle = new QLabel(title);
		labelTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
		labelTitle->setStyleSheet("font-weight: bold;");
		titleLayout->addWidget(labelTitle);
	}

	QLabel* messageLabel = new QLabel(params.message);
	messageLabel->setMinimumHeight(20);
	messageLabel->setMaximumHeight(20);
	messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	messageLabel->setWordWrap(true);
	QHBoxLayout* mesLayout = new QHBoxLayout;
	mesLayout->addSpacing(20);
	mesLayout->addWidget(messageLabel);

	QVBoxLayout* messageLayout = new QVBoxLayout();
	messageLayout->setSpacing(5);
	messageLayout->addItem(titleLayout);
	messageLayout->addLayout(mesLayout);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->setContentsMargins(10, 10, 10, 10);
	mainLayout->addItem(messageLayout);

	QPalette palette;
	QColor baseColor = palette.color(QPalette::Midlight);
	QColor buttonColor = baseColor;
	buttonColor.setAlpha(0);
	QColor pressedColor = baseColor;
	pressedColor.setAlpha(255);

	QString styleSheet = QString("QPushButton {"
		"border-radius: 1px;"
		"background-color: " +
		NotificationWidgetDetails::ColorToHTML(buttonColor) + ";"
		"padding: 5px;"
		"}"
		"QPushButton:pressed {"
		"background-color: " +
		NotificationWidgetDetails::ColorToHTML(pressedColor) + ";"
		"}");

	QVBoxLayout* buttonsLayout = new QVBoxLayout();
	buttonsLayout->setContentsMargins(0, 0, 0, 0);
	buttonsLayout->setSpacing(5);
	{
		_closeButton = new QPushButton(tr("Close"));
		_closeButton->setStyleSheet(styleSheet);
		_closeButton->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
		buttonsLayout->addWidget(_closeButton);
		connect(_closeButton, &QPushButton::clicked, this, &NotificationWidget::onCloseButtonClicked);
	}
	if (params.callback)
	{
		_detailsButton = new QPushButton(params.detailsButtonText);
		_detailsButton->setStyleSheet(styleSheet);
		_detailsButton->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
		buttonsLayout->addWidget(_detailsButton);
		connect(_detailsButton, &QPushButton::clicked, this, &NotificationWidget::onDetailsButtonClicked);
	}
	mainLayout->addItem(buttonsLayout);
	setLayout(mainLayout);
}

void NotificationWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QRect roundedRect;
	const int radius = 10;
	roundedRect.setX(rect().x() + radius / 2);
	roundedRect.setY(rect().y() + radius / 2);
	roundedRect.setWidth(rect().width() - radius);
	roundedRect.setHeight(rect().height() - radius);

	QPalette palette;
	QColor rectColor = palette.color(QPalette::Window);
	painter.setBrush(QBrush(rectColor));
	QPen roundedRectPen(Qt::black);
	painter.setPen(roundedRectPen);
	painter.drawRoundedRect(roundedRect, radius, radius);

	QPen pen(Qt::gray, 1);
	painter.setPen(pen);
	QRect closeButtonGeometry = _closeButton->geometry();
	//horizontal line
	if (_detailsButton != nullptr)
	{
		QRect detailsButtonGeometry = _detailsButton->geometry();
		int y = (closeButtonGeometry.bottom() + detailsButtonGeometry.top()) / 2;
		QPoint left(qMin(closeButtonGeometry.left(), detailsButtonGeometry.left()), y);
		QPoint right(qMax(closeButtonGeometry.right(), detailsButtonGeometry.right()), y);
		painter.drawLine(left, right);
	}

	//vertical line
	//close button and details button have Preferred size policy
	int x = closeButtonGeometry.left() - pen.width();
	QPoint top(x, roundedRect.top() + roundedRectPen.width());
	QPoint bottom(x, roundedRect.bottom() - roundedRectPen.width());
	painter.drawLine(top, bottom);
}
