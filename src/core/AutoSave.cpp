#include <QApplication>
#include <QGuiApplication>
#include <QDateTime>

#include "AutoSave.h"
#include "Song.h"

AutoSave::AutoSave()
{
    m_autoSaveInterval = ConfigManager::inst()->value( "ui", "saveinterval" ).toInt() < 1 ?
                 DEFAULT_AUTO_SAVE_INTERVAL : ConfigManager::inst()->value( "ui", "saveinterval" ).toInt();
    QString recoverFile = QDir( m_recoverPath ).filePath( "recover_XXXXXX.mmp" );
    recoverFileTemplate = recoverFile;
    m_recoveryObject->setFileTemplate( recoverFileTemplate );
    m_recoveryObject->setAutoRemove( false );

    if ( ! m_indexFile->open( QIODevice::ReadWrite | QIODevice::Append ) )
    {
        useIndexing = false;
    }

    connect( m_autoSaveTimer, SIGNAL( timeout() ), this, SLOT( exec() ) );
}

AutoSave::~AutoSave()
{
    delete m_recoveryObject;
    delete m_autoSaveTimer;
    cleanup();
}

void AutoSave::resetAutoSaveTimer( qint32 saveInterval = ConfigManager::inst()->
                   value( "ui", "saveinterval" ).toInt() * 60 * 1000 )
{
    if ( saveInterval < m_autoSaveShortTime ) // No 'saveinterval' in .lmmsrc.xml
    {
        saveInterval = DEFAULT_AUTO_SAVE_INTERVAL;
    }

    m_autoSaveTimer->start( saveInterval );
}

void AutoSave::cleanup()
{

}

void AutoSave::exec( bool skipCheck = false )
{
    isEmergency = ( !skipCheck );

    if ( !skipCheck )
    {
        if ( this->isBusy() )
        {
            if ( m_autoSaveInterval != m_autoSaveShortTime )
            {
                this->resetAutoSaveTimer( m_autoSaveShortTime );
                return;
            }  // End of interval comparsion
        }  // End of isBusy

        m_saveCount = ( m_saveCount + 1 ) % 11;
    }
    else
    {
        m_saveCount++;
    }// End of skipCheck

    writeRecoverFile();
}

void AutoSave::refreshRecoverFile()
{
    while (m_recoverFilesLocation.length() > m_maxSaveCount) {
        QFile::remove(m_recoverFilesLocation[0]);
        m_recoverFilesLocation.removeFirst();
    }

}

void AutoSave::addIndexEntry( QString projectTitle, qint64 timeStamp, QString Path )
{
    QString saveReason = QString::number( isEmergency ? 1 : 0 );
    m_recoverFilesLocation << Path ;
    // For each entry, we have: <Project Title>/<Timestamp>/<Save reason>/<Recovery File location>
    QStringList recoveryIndexEntry;
    recoveryIndexEntry << projectTitle << QString::number(timeStamp) << saveReason << Path;
    QByteArray entryData;

    //Encode to one line using \x00 as seperator
    foreach ( const QString &item, recoveryIndexEntry )
    {
        entryData.append( item );
        entryData.append( '\x00' );
    }
    m_recoverFiles[m_saveCount - 1] = entryData;
}

void AutoSave::updateIndex()
{
    if ( !useIndexing )
    {
        return;
    }
    QByteArray indexData;
    foreach ( const QByteArray &item, m_recoverFiles )
    {
        indexData.append( item );
        indexData.append( '\n' );
    }
    m_indexFile->write( indexData );
}

void AutoSave::writeRecoverFile()
{
    if ( m_recoveryObject->open() )
    {
        m_currentRecoverFileName = m_recoveryObject->fileName();
    }

    updateIndex();
    Engine::getSong()->saveProjectFile( m_currentRecoverFileName );
}

bool AutoSave::isBusy()
{
    if(  Engine::getSong()->isPlaying() || Engine::getSong()->isExporting() ||
            QApplication::mouseButtons() || QGuiApplication::keyboardModifiers()  )
    {
        return true;
    }
    else
    {
        return false;
    }
}
