#ifndef I2PDMANAGER_H
#define I2PDMANAGER_H

#include <QObject>
#include <QString>
#include <QThread>

class I2pdWorker;

class I2pdManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

public:
    explicit I2pdManager(QObject *parent = nullptr);
    ~I2pdManager();

    bool isRunning() const;

public slots:
    void start(const QString &dataPath);
    void stop();

signals:
    void runningChanged(bool running);

private:
    void extractAssets(const QString &dataPath);
    void init(const QString &dataPath);

    QThread *m_thread;
    I2pdWorker *m_worker;
    bool m_isStarted;
    bool m_isInitialized;
};

#endif // I2PDMANAGER_H
