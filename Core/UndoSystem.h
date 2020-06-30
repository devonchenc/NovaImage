#pragma once

#include <QVector>

class BaseImage;

class UndoSystem
{
public:
	~UndoSystem();

	void backup(BaseImage* image);

	BaseImage* undo();

	BaseImage* redo();

	bool isEmpty() { return _currentIndex < 1; }

	bool isTop() { return _currentIndex + 1 == _imageStack.length(); }

	void reset();

private:
	QVector<BaseImage*> _imageStack;

	int _currentIndex = -1;
};
