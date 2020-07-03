#include "RecentFiles.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QString>
#include <QStringList>
#include <QSettings>

RecentFiles::RecentFiles(QMainWindow* parent)
	: QObject(parent)
	, _recentMenu(new QMenu(parent))
	, _recentMenuTriggeredAction(nullptr)
{
	// create the sub-menu
	_recentMenu->setTitle(tr("Open Recent"));

	// create an action for all possible entries in the sub-menu
	for (int i = 0; i < _maxRecentFiles; i++)
	{
		_recentFileActions[i] = new QAction(_recentMenu);
		_recentFileActions[i]->setText("---");
		_recentFileActions[i]->setVisible(false);
		connect(_recentFileActions[i], &QAction::triggered, this, &RecentFiles::openRecentFile);
		_recentMenu->addAction(_recentFileActions[i]);
	}

	_clearAction = new QAction(_recentMenu);
	_clearAction->setText(tr("Clear Recently Opened"));
	_clearAction->setVisible(false);
	connect(_clearAction, &QAction::triggered, this, &RecentFiles::clearRecentFiles);
	_recentMenu->addSeparator();
	_recentMenu->addAction(_clearAction);

	// Set some defaults
	QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	if (!settings.value(recentFileCount).isValid())
	{
		settings.setValue(recentFileCount, QVariant(6));
	}

	// If there are no recent files, initialize an empty list
	if (!settings.allKeys().contains(recentFileListId))
	{
		settings.setValue(recentFileListId, QVariant(QStringList()));
	}

	updateRecentFiles(settings);
}

RecentFiles::~RecentFiles()
{
	// delete m_recentMenu; // might be bad because parent menu will delete.
	_recentMenu = nullptr;
}

void RecentFiles::attachToMenuAfterItem(QMenu* menu, QAction* action, const char* slotName)
{
	QList<QAction*> actionList = menu->actions();

	int indexOfItemToFollow = 0;
	for (; indexOfItemToFollow < actionList.size(); indexOfItemToFollow++)
	{
		QAction* act = actionList.at(indexOfItemToFollow);
		if (act == action)
		{
			break;
		}
	}

	int indexOfItemToPreceed = indexOfItemToFollow + 1;
	if (indexOfItemToPreceed >= actionList.size())
	{
		// append to end of menu
		_recentMenuTriggeredAction = menu->addMenu(_recentMenu);
	}
	else
	{
		// insert into menu
		QAction* before = actionList.at(indexOfItemToPreceed);
		menu->insertSeparator(before);
		_recentMenuTriggeredAction = menu->insertMenu(before, _recentMenu);
	}

	connect(this, SIGNAL(openFile(QString)), parent(), slotName);
}

void RecentFiles::openRecentFile()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action)
	{
		emit openFile(action->data().toString());
	}
}

void RecentFiles::clearRecentFiles()
{
	QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	settings.setValue(recentFileListId, QVariant(QStringList()));

	updateRecentFiles(settings);
}

void RecentFiles::purgeMissingFilesFromList(QStringList& recentFileList)
{
	for (QMutableStringListIterator i(recentFileList); i.hasNext();)
	{
		QString fileName = i.next(); // stash filename and advance iterator
		if (!QFile::exists(fileName))
		{
			i.remove(); // removes item last jumped by i.next()
		}
	}
}

QString RecentFiles::strippedName(const QString& fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

void RecentFiles::setMenuEnabled(bool enable)
{
	_recentMenuTriggeredAction->setEnabled(enable);
}

int RecentFiles::numberOfRecentFilesToSave()
{
	QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	return settings.value(recentFileCount, 6).toInt();
}

void RecentFiles::resetText()
{
	_recentMenu->setTitle(tr("Open Recent"));
	_clearAction->setText(tr("Clear Recently Opened"));
}

void RecentFiles::setMostRecentFile(const QString& fileName)
{
	if (fileName.isEmpty())
		return;

	QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	QStringList recentFileList = settings.value(recentFileListId).toStringList();
	int numOfRecentFiles = settings.value(recentFileCount, 6).toInt();

	recentFileList.removeAll(fileName);
	recentFileList.prepend(fileName);

	if (recentFileList.size() > numOfRecentFiles)
	{
		for (int i = recentFileList.size() - 1; i >= numOfRecentFiles; i--)
		{
			recentFileList.removeAt(i);
		}
	}
	
	settings.setValue(recentFileListId, QVariant(recentFileList));

	updateRecentFiles(settings);
}

void RecentFiles::setNumOfRecentFiles(int num)
{
	QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	settings.setValue(recentFileCount, QVariant(num));

	updateRecentFiles(settings);
}

QStringList RecentFiles::getRecentFiles() const
{
	QSettings settings(QCoreApplication::applicationDirPath() + "/Config.ini", QSettings::IniFormat);
	QStringList recentFileList = settings.value(recentFileListId).toStringList();
	return recentFileList;
}

void RecentFiles::updateRecentFiles(QSettings& settings)
{
	int numOfRecentFiles = settings.value(recentFileCount, 6).toInt();

	QStringList recentFileList = settings.value(recentFileListId).toStringList();
	purgeMissingFilesFromList(recentFileList);
	settings.setValue(recentFileListId, QVariant(recentFileList));

	// If there are no recent files, then the menu item (that would show the list)
	// should not be visible.
	if (_recentMenuTriggeredAction)
	{
		if (numOfRecentFiles == 0)
		{
			_recentMenuTriggeredAction->setVisible(false);
		}
		else
		{
			_recentMenuTriggeredAction->setVisible(true);
		}
	}

	for (int i = 0; i < _maxRecentFiles; ++i)
	{
		if (i < recentFileList.count() && i < numOfRecentFiles)
		{
			QString text = strippedName(recentFileList[i]);
		//	_recentFileActions[j]->setText(text);
			_recentFileActions[i]->setText(QString::number(i + 1) + " " + recentFileList[i]);
			_recentFileActions[i]->setData(recentFileList[i]);

			_recentFileActions[i]->setVisible(true);
		}
		else
		{
			_recentFileActions[i]->setVisible(false);
		}
	}

	for (int i = numOfRecentFiles; i < recentFileList.count(); i++)
	{
		_recentFileActions[i]->setVisible(false);
	}

	if (recentFileList.count() > 0)
	{
		_clearAction->setVisible(true);
	}
	else
	{
		_clearAction->setVisible(false);
	}

	getRecentFiles();
}