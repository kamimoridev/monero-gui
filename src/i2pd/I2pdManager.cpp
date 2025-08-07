#include "I2pdManager.h"
#include "qt/MoneroSettings.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QSettings>
#include <vector>

#include "../../external/i2pd/libi2pd/api.h"

I2pdManager::I2pdManager(MoneroSettings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
    , m_isStarted(false)
{
}

I2pdManager::~I2pdManager()
{
    stop();
}

void I2pdManager::start()
{
    if (m_isStarted) {
        return;
    }
    
    extractAssets();
    startI2pd();
    m_isStarted = true;
}

void I2pdManager::stop()
{
    if (!m_isStarted) {
        return;
    }

    stopI2pd();
    m_isStarted = false;
}

QString I2pdManager::i2pdSettingsDir() const
{
    QFileInfo settingsFile(m_settings->fileName().isEmpty() ? QSettings().fileName() : m_settings->fileName());
    return QDir(settingsFile.absolutePath()).filePath("i2pd");
}

void I2pdManager::extractAssets()
{
    QString targetDir = i2pdSettingsDir();
    if (QFile::exists(QDir(targetDir).filePath("i2pd.conf"))) {
        return;
    }

    QDirIterator it(":/i2pd", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString resourcePath = it.next();
        QString relativePath = resourcePath;
        relativePath.remove(0, QString(":/i2pd/").length());
        
        QString targetFilePath = QDir(targetDir).filePath(relativePath);
        QFileInfo fileInfo(targetFilePath);
        QDir().mkpath(fileInfo.absolutePath());

        QFile::copy(resourcePath, targetFilePath);
    }
}

void I2pdManager::startI2pd()
{
    QString dataDir = i2pdSettingsDir();
    QString confPath = QDir(dataDir).filePath("i2pd.conf");

    std::string datadir_str = "--datadir=" + dataDir.toStdString();
    std::string conf_str = "--conf=" + confPath.toStdString();

    std::vector<char*> argv_vec;
    argv_vec.push_back(const_cast<char*>("monero-gui")); 
    argv_vec.push_back(const_cast<char*>(datadir_str.c_str()));
    argv_vec.push_back(const_cast<char*>(conf_str.c_str()));
    
    int argc = argv_vec.size();
    char** argv = argv_vec.data();

    i2p::api::InitI2P(argc, argv, "monero-gui");
    i2p::api::StartI2P();
}

void I2pdManager::stopI2pd()
{
    i2p::api::StopI2P();
    i2p::api::TerminateI2P();
}
