/***************************************************************************
 *   This file is part of pytextmonitor                                    *
 *                                                                         *
 *   pytextmonitor is free software: you can redistribute it and/or        *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   pytextmonitor is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with pytextmonitor. If not, see http://www.gnu.org/licenses/    *
 ***************************************************************************/

#ifndef EXTSYSMON_H
#define EXTSYSMON_H

#include <Plasma/DataEngine>
#include <QProcess>

class ExtendedSysMon : public Plasma::DataEngine
{
    Q_OBJECT

public:
    ExtendedSysMon(QObject *parent, const QVariantList &args);
    // update functions
    QMap<QString, QVariant> getCurrentDesktop(const QString cmd);
    QString getCustomCmd(const QString cmd);
    float getGpu(const QString device);
    float getGpuTemp(const QString device);
    float getHddTemp(const QString cmd, const QString device);
    QMap<QString, QVariant> getPlayerInfo(const QString playerName,
                                          const QString mpdAddress = 0,
                                          const QString mpdPort = 0,
                                          QString mpris = 0);
    QMap<QString, QVariant> getPsStats();
    int getUpgradeInfo(const QString cmd);

protected:
    bool sourceRequestEvent(const QString &source);
    bool updateSourceEvent(const QString &source);
    QStringList sources() const;

private:
    // configuration
    QMap<QString, QString> configuration;
    bool debug;
    // reread configuration
    QString getAllHdd();
    QString getAutoGpu();
    QString getAutoMpris();
    QStringList getDesktopNames();
    void readConfiguration();
    QMap<QString, QString> updateConfiguration(const QMap<QString, QString> rawConfig);
};

#endif /* EXTSYSMON_H */
