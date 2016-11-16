#ifndef RECOVERY_H
#define RECOVERY_H
#include <QByteArray>
#include <QDir>
#include <QIODevice>
#include <QTemporaryFile>
#include <QTimer>
#include <QDebug>

#include "ConfigManager.h"

class AutoSave : public QObject
{
public:
    AutoSave();
    void exec( bool skipCheck );
    bool isBusy();
    virtual ~AutoSave();
    void resetAutoSaveTimer( qint32 saveInterval );
    qint32 getAutoSaveTimerInterval();
    void cleanup();

public slots:
    void exec();

private:
    bool isEnabled;
    static const int DEFAULT_SAVE_INTERVAL_MINUTES = 2;
    static const int DEFAULT_AUTO_SAVE_INTERVAL = DEFAULT_SAVE_INTERVAL_MINUTES * 60 * 1000;
    static const int DEFAULT_AUTO_SAVE_FILE_COUNT = 10;  // 10 history for each project
    static const int m_autoSaveShortTime = 10 * 1000; // 10s short loop
    QTimer* m_autoSaveTimer = new QTimer();
    qint32 m_autoSaveInterval = 0;
    qint32 m_saveCount = 0;
    qint32 m_maxSaveCount = 10;  // Currently hard-coded
    bool isEmergency = false;
    bool useIndexing = true;
    QList<QByteArray> m_recoverFiles;
    QStringList m_recoverFilesLocation;
    QTemporaryFile* m_recoveryObject = new QTemporaryFile();
    QString recoverFileTemplate;
    QString m_recoverPath = ConfigManager::inst()->recoveryFile();
    QString m_currentRecoverFileName;
    QString m_indexFileLocation = QDir( m_recoverPath ).filePath( QString( "lmms_recover.idx" ) );
    QFile* m_indexFile = new QFile( m_indexFileLocation );

    void refreshRecoverFile();
    void addIndexEntry( QString projectTitle, qint64 timeStamp, QString Path );
    void updateIndex();
    void writeRecoverFile();


};

#endif // RECOVERY_H
