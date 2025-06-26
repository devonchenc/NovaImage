#pragma once

#include <QObject>
#include <QStringList>
#include <QMainWindow>
#include <QSettings>

class QMenu;
class QAction;

#define recentFileCount "RecentFiles/numOfRecentFiles"
#define recentFileListId "RecentFiles/fileList"

class RecentFiles : public QObject
{
    Q_OBJECT

public:
    explicit RecentFiles(QMainWindow* parent = nullptr);
    ~RecentFiles();

    // Inserts the sub-menu into another Menu
    void attachToMenuAfterItem(QMenu* menu, QAction* action, const char* slotName);

    QStringList getRecentFiles() const;

    void setMostRecentFile(const QString& fileName);

    void setMenuEnabled(bool enable);

    // returns how many recent files are being remembered
    int numberOfRecentFilesToSave();

    void resetText();

    static const int _maxRecentFiles = 6;

public slots:
    // The application can set the number of recent files retained/reported here
    void setNumOfRecentFiles(int num);

signals:
    // emitted when user selects item from "Open Recent" sub-menu
    void openFile(QString fileName);

private slots:
    void openRecentFile();

    void clearRecentFiles();

private:
    void purgeMissingFilesFromList(QStringList& recentFileList);
    void updateRecentFiles(QSettings& settings);

    QMenu* _recentMenu;
    QAction* _recentMenuTriggeredAction;
    QAction* _recentFileActions[_maxRecentFiles];
    QAction* _clearAction;
};
