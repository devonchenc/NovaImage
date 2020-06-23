#include "NotificationLayout.h"
#include "NotificationWidget.h"

#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QApplication>

struct NotificationLayout::NotificationWidgetParams
{
	void initAnimation(NotificationWidget* target)
	{
		positionAnimation = new QPropertyAnimation(target, "position", target);
		const int durationTimeMs = 120;
		positionAnimation->setDuration(durationTimeMs);

		opacityAnimation = new QPropertyAnimation(target, "opacity", target);
		opacityAnimation->setDuration(durationTimeMs * 2);
	}

	void decrementTime(int elapsedMs)
	{
		if (elapsedMs > remainTimeMs)
		{
			remainTimeMs = 0;
		}
		else
		{
			remainTimeMs -= elapsedMs;
		}
	}

	std::function<void()> callback;
	int remainTimeMs = 0;
	QPropertyAnimation* positionAnimation = nullptr;
	QPropertyAnimation* opacityAnimation = nullptr;
};

NotificationLayout::NotificationLayout()
	: QObject(nullptr)
{
	_basicTimer.start(60, this);
}

NotificationLayout::~NotificationLayout()
{
	clear();
}

void NotificationLayout::addNotification(const NotificationParams& params)
{
	NotificationWidget* widget = new NotificationWidget(params);
	connect(widget, &NotificationWidget::closeButtonClicked, this, &NotificationLayout::onCloseClicked);
	connect(widget, &NotificationWidget::detailsButtonClicked, this, &NotificationLayout::onDetailsClicked);
	connect(widget, &QObject::destroyed, this, &NotificationLayout::onWidgetDestroyed);

	NotificationWidgetParams widgetParams;
	widgetParams.initAnimation(widget);

	widgetParams.callback = std::move(params.callback);
	widgetParams.remainTimeMs = _displayTimeMs;

	_vecWidget.emplace_back(widget, widgetParams);

	layoutWidgets();
}

void NotificationLayout::addNotification(const QString& title, const QString& message, NotificationParams::Type type)
{
	NotificationParams params;
	params.title = title;
	params.message = message;
	params.type = type;
	addNotification(params);
}

void NotificationLayout::layoutWidgets()
{
	int totalHeight = 0;

	int size = std::min(static_cast<int>(_vecWidget.size()), _maximumDisplayCount);
	std::vector<NotificationPair> widgetsToDisplay(_vecWidget.begin(), _vecWidget.begin() + size);
	if (layoutType & Qt::AlignTop)
	{
		std::reverse(widgetsToDisplay.begin(), widgetsToDisplay.end());
	}

	for (const NotificationPair& pair : widgetsToDisplay)
	{
		NotificationWidget* widget = pair.first;
		bool justCreated = false;
		if (widget->isVisible() == false)
		{
			justCreated = true;
			QPropertyAnimation* opacityAnimation = pair.second.opacityAnimation;
			widget->show();
			opacityAnimation->setStartValue(0.0f);
			opacityAnimation->setEndValue(1.0f);
			opacityAnimation->start();
		}

		QRect desktopRect = QApplication::desktop()->availableGeometry();
		int x = (layoutType & Qt::AlignLeft) ? 0 : (desktopRect.width() - widget->width());
		int y = (layoutType & Qt::AlignTop) ? totalHeight : (desktopRect.height() - widget->height() - totalHeight);
		QPoint widgetPos(x, y);

		if (justCreated)
		{
			widget->move(widgetPos);
		}
		else
		{
			QPropertyAnimation* positionAnimation = pair.second.positionAnimation;
			positionAnimation->stop();
			positionAnimation->setStartValue(widget->pos());
			positionAnimation->setEndValue(widgetPos);
			positionAnimation->start();
		}

		totalHeight += widget->size().height();
	}
}

void NotificationLayout::clear()
{
	for (WindowNotifications::iterator iter = _vecWidget.begin(); iter != _vecWidget.end(); ++iter)
	{
		QWidget* widget = iter->first;
		disconnect(widget, &QObject::destroyed, this, &NotificationLayout::onWidgetDestroyed);
		delete widget;
	}
	_vecWidget.clear();
}

void NotificationLayout::timerEvent(QTimerEvent* /*event*/)
{
	int elapsedMs = _elapsedTimer.restart();
	for (WindowNotifications::iterator iter = _vecWidget.begin(); iter != _vecWidget.end();)
	{
		NotificationWidget* widget = iter->first;
		NotificationWidgetParams& params = iter->second;
		params.decrementTime(elapsedMs);
		if (params.remainTimeMs == 0)
		{
			iter = _vecWidget.erase(iter);
			disconnect(widget, &QObject::destroyed, this, &NotificationLayout::onWidgetDestroyed);
			delete widget;
		}
		else
		{
			++iter;
		}
	}

	layoutWidgets();
}

void NotificationLayout::setLayoutType(Qt::Alignment type)
{
	if (layoutType == type)
		return;

	Q_ASSERT((type & Qt::AlignLeft || type & Qt::AlignRight) &&
		(type & Qt::AlignBottom) || (type & Qt::AlignTop));

	layoutType = type;

	//now remove all NotificationPair
	clear();
}

void NotificationLayout::setDisplayTimeMs(int displayTimeMs)
{
	_displayTimeMs = displayTimeMs;
}

void NotificationLayout::onCloseClicked(NotificationWidget* notification)
{
	delete notification;
}

void NotificationLayout::onDetailsClicked(NotificationWidget* notification)
{
	WindowNotifications::iterator iter = std::find_if(_vecWidget.begin(), _vecWidget.end(), [notification](const NotificationPair& pair)
	{
		return pair.first == notification;
	});
	Q_ASSERT(iter != _vecWidget.end());
	iter->second.callback();

	delete notification;
}

void NotificationLayout::onWidgetDestroyed()
{
	NotificationWidget* notification = static_cast<NotificationWidget*>(sender());
	WindowNotifications::iterator widgetsIter = std::find_if(_vecWidget.begin(), _vecWidget.end(), [notification](const NotificationPair& pair)
	{
		return pair.first == notification;
	});
	if (widgetsIter != _vecWidget.end())
	{
		_vecWidget.erase(widgetsIter);
		layoutWidgets();
		return;
	}
}