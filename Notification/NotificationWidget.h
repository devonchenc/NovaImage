#pragma once

#include <QWidget>
#include <functional>

class QPaintEvent;
class QPushButton;

struct NotificationParams
{
	enum Type
	{
		Information,
		Warning,
		Critical
	};
	QString title;
	QString message;
	Type type = Information;
	QString detailsButtonText = "Details";
	std::function<void()> callback;
};

class NotificationWidget : public QWidget
{
	Q_OBJECT
		Q_PROPERTY(QPoint position READ pos WRITE move)
		Q_PROPERTY(float opacity READ windowOpacity WRITE setWindowOpacity)

public:
	explicit NotificationWidget(const NotificationParams& params, QWidget* parent = nullptr);

signals:
	void closeButtonClicked(NotificationWidget* current);
	void detailsButtonClicked(NotificationWidget* current);

private slots:
	void onCloseButtonClicked();
	void onDetailsButtonClicked();

private:
	void initUI(const NotificationParams& params);

	void paintEvent(QPaintEvent* event) override;

	QPushButton* _closeButton = nullptr;
	QPushButton* _detailsButton = nullptr;
};
