#ifndef I2PDMANAGER_H
#define I2PDMANAGER_H

#include <QObject>
#include <QString>

class MoneroSettings;

class I2pdManager : public QObject
{
    Q_OBJECT

public:
    explicit I2pdManager(const QString &dataPath, QObject *parent = nullptr);
    ~I2pdManager();

public slots:
    void start();
    void stop();

private:
    QString i2pdSettingsDir() const;
    void extractAssets();
    void startI2pd();
    void stopI2pd();

    bool m_isStarted;
    QString m_dataPath;
};

#endif // I2PDMANAGER_H