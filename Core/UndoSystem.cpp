#include "UndoSystem.h"

#include "../Image/BaseImage.h"

UndoSystem::~UndoSystem()
{
	reset();
}

void UndoSystem::backup(BaseImage* image)
{
	int stackSize = _imageStack.size();
	if (_currentIndex < stackSize - 1)
	{
		for (int i = _currentIndex + 1; i < stackSize; i++)
		{
			delete _imageStack[i];
		}
		_imageStack.erase(_imageStack.begin() + _currentIndex + 1, _imageStack.end());
	}

	BaseImage* backupImage = image->copyImage();
	_imageStack.push_back(backupImage);
	_currentIndex++;
}

BaseImage* UndoSystem::undo()
{
	return _imageStack[--_currentIndex];
}

BaseImage* UndoSystem::redo()
{
	return _imageStack[++_currentIndex];
}

void UndoSystem::reset()
{
	for (int i = 0; i < _imageStack.size(); i++)
	{
		delete _imageStack[i];
	}
	_imageStack.clear();
}
