#include "I2pdManager.h"
#include "qt/MoneroSettings.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QSettings>
#include <vector>

#include "../../external/i2pd/libi2pd/api.h"
#include <iostream>

// Worker object to run blocking i2pd calls in a separate thread
class I2pdWorker : public QObject
{
    Q_OBJECT

public slots:
    void startI2pd()
    {
        
        // This is a blocking call that will run until StopI2P is called from another thread
        auto logStream = std::make_shared<std::ostream>(std::cout.rdbuf());
        i2p::api::StartI2P(logStream);
    }

private:
    std::string m_conf_key_str;
    std::string m_conf_val_str;
};

I2pdManager::I2pdManager(QObject *parent)
    : QObject(parent)
    , m_isStarted(false)
    , m_isInitialized(false)
    , m_thread(nullptr)
    , m_worker(nullptr)
{
}

void I2pdManager::init(const QString &dataPath) {
    QString confPath = QDir(dataPath).filePath("i2pd.conf");

    std::vector<char*> argv_vec;
    argv_vec.push_back(const_cast<char*>("monero-gui")); 
    argv_vec.push_back(const_cast<char*>("--conf"));
    argv_vec.push_back(const_cast<char*>(confPath.toStdString().c_str()));
    
    int argc = argv_vec.size();
    char** argv = argv_vec.data();
    i2p::api::InitI2P(argc, argv, "monero-gui");
}

I2pdManager::~I2pdManager()
{
    if (m_isStarted) {
        stop();
        if (m_thread) {
            m_thread->wait(); // Wait on destructor to ensure clean exit
        }
    }
}

bool I2pdManager::isRunning() const
{
    return m_isStarted;
}

void I2pdManager::start(const QString &dataPath)
{
    std::cout << "starting i2pd" << std::endl;
    std::cout << "i2pd datadir: " << dataPath.toStdString() << std::endl;
    if (m_isStarted) {
        return;
    }

    if (!m_isInitialized) {
        init(dataPath);
        m_isInitialized = true;
    }
    
    extractAssets(dataPath);

    m_thread = new QThread();
    m_worker = new I2pdWorker();
    m_worker->moveToThread(m_thread);

    connect(m_thread, &QThread::started, m_worker, [this, dataPath](){
        m_worker->startI2pd();
    });

    connect(m_thread, &QThread::finished, this, [this](){
        m_thread->deleteLater();
        m_thread = nullptr;
        m_worker = nullptr;
    });

    m_thread->start();
    m_isStarted = true;
    emit runningChanged(true);
}

void I2pdManager::stop()
{
    std::cout << "stopping i2pd" << std::endl;
    if (!m_isStarted) {
        return;
    }

    i2p::api::StopI2P();
    //i2p::api::TerminateI2P();

    if (m_thread) {
        m_thread->quit();
        m_thread = nullptr;
    }

    m_isStarted = false;
    emit runningChanged(false);
    std::cout << "finished stopping i2pd" << std::endl;
}

void I2pdManager::extractAssets(const QString &dataPath)
{
    QString confPath = QDir(dataPath).filePath("i2pd.conf");
    if (QFile::exists(confPath)) {
        return;
    }

    // Create parent directory
    QDir().mkpath(dataPath);

    // Copy main config
    QFile f(":/i2pd/i2pd.conf");
    f.open(QIODevice::ReadOnly);
    QByteArray confData = f.readAll();
    f.close();

    // Prepend datadir
    QByteArray datadirLine = "datadir=" + dataPath.toUtf8() + "\n";
    confData.prepend(datadirLine);

    // Write config
    QFile outFile(confPath);
    outFile.open(QIODevice::WriteOnly);
    outFile.write(confData);
    outFile.close();

    // Copy other assets
    QDirIterator it(":/i2pd", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString resourcePath = it.next();
        if (resourcePath.endsWith("i2pd.conf")) continue;

        QString relativePath = resourcePath;
        relativePath.remove(0, QString(":/i2pd/").length());
        
        QString targetFilePath = QDir(dataPath).filePath(relativePath);
        QFileInfo fileInfo(targetFilePath);
        QDir().mkpath(fileInfo.absolutePath());

        QFile::copy(resourcePath, targetFilePath);
    }
}

#include "I2pdManager.moc"
