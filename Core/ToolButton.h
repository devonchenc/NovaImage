#pragma once

#include <QToolButton>

class MouseHandler;

class ToolButton : public QToolButton
{
    Q_OBJECT

public:
    ToolButton(QWidget* parent = nullptr);
    virtual ~ToolButton();

public:
    void setIconByName(const QString& fileName);

    void setCurrentAction(QAction* action);

    QAction* currentAction() const { return _currentAction; }

    void activeAction(QAction* action, bool leftMouseButton = true);

    void setMouseHandler(MouseHandler* handler);

    MouseHandler* mouseHandler() const { return _handler; }

    static void setLeftMouseButton(ToolButton* toolButton);

    static void unsetLeftMouseButton();

    static ToolButton* leftMouseButton() { return _leftMouseButton; }

    static void setRightMouseButton(ToolButton* toolButton);

    static void unsetRightMouseButton();

    static ToolButton* rightMouseButton() { return _rightMouseButton; }

private:
    static void updateAllButtonsIcon();

    void updateIcon();

private:
    QString _fileName;

    QAction* _currentAction;

    MouseHandler* _handler;

    static ToolButton* _leftMouseButton;
    static ToolButton* _rightMouseButton;

    static QVector<ToolButton*> _vec;
};

