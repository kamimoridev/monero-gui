/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
****************************************************************************/
// Copyright (c) 2014-2024, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef MONEROSETTINGS_H
#define MONEROSETTINGS_H

#include <QtQml/qqmlparserstatus.h>
#include <QObject>
#include <QVariant>
#include <QJSValue>
#include <QHash>
#include <QMetaProperty>
#include <QTimerEvent>
#include <QDir>
#include <QFileInfo>
#include <memory>

class QSettings;

class MoneroSettings : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(bool portable READ portable WRITE setPortable NOTIFY portableChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)

public:
    explicit MoneroSettings(QObject *parent = nullptr);

    bool portable() const;
    bool setPortable(bool enabled);

    QString fileName() const;
    void setFileName(const QString &fileName);

    Q_INVOKABLE bool getI2pNodeConnectionEnabled() const;
    Q_INVOKABLE void setI2pNodeConnectionEnabled(bool enabled);

    Q_INVOKABLE QString settingsDirectory() const;
    static QString portableFolderName();

signals:
    void portableChanged();

protected:
    void timerEvent(QTimerEvent *event) override;
    void classBegin() override;
    void componentComplete() override;

private slots:
    void _q_propertyChanged();

private:
    void init();
    void load();
    void store();
    void reset();
    QVariant readProperty(const QMetaProperty &property) const;
    bool portableConfigExists();
    static QString portableFilePath();
    std::unique_ptr<QSettings> portableSettings() const;
    std::unique_ptr<QSettings> unportableSettings() const;
    void swap(std::unique_ptr<QSettings> newSettings);
    void setWritable(bool enabled);

    QHash<const char *, QVariant> m_changedProperties;
    int m_timerId = 0;
    bool m_initialized = false;
    bool m_writable = true;
    std::unique_ptr<QSettings> m_settings;
    QString m_fileName = QString("");
    static const int settingsWriteDelay = 1000;
};

#endif // MONEROSETTINGS_H
