/***************************************************************************
 *   This file is part of awesome-widgets                                  *
 *                                                                         *
 *   awesome-widgets is free software: you can redistribute it and/or      *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   awesome-widgets is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with awesome-widgets. If not, see http://www.gnu.org/licenses/  *
 ***************************************************************************/

#include "awesome-widget.h"

#include <QRegExp>
#include <math.h>

#include "graphicalitem.h"
#include <pdebug/pdebug.h>


void AwesomeWidget::connectToEngine()
{
    if (debug) qDebug() << PDEBUG;
    QRegExp regExp;

    // battery
    regExp = QRegExp(QString("(^|bar[0-9].*)(ac|bat.*)"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (configuration[QString("batteryTooltip")].toInt() == 2) ||
            (foundBars.indexOf(regExp) > -1))
        extsysmonEngine->connectSource(QString("battery"),
                                       this, configuration[QString("interval")].toInt());
    // cpu
    regExp = QRegExp(QString("(^|bar[0-9].*)cpu(?!cl).*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (configuration[QString("cpuTooltip")].toInt() == 2) ||
            (foundBars.indexOf(regExp) > -1)) {
        sysmonEngine->connectSource(QString("cpu/system/TotalLoad"),
                                    this, configuration[QString("interval")].toInt());
        for (int i=0; i<counts[QString("cpu")]; i++)
            sysmonEngine->connectSource(QString("cpu/cpu") + QString::number(i) + QString("/TotalLoad"),
                                        this, configuration[QString("interval")].toInt());
    }
    // cpuclock
    regExp = QRegExp(QString("cpucl.*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (configuration[QString("cpuclTooltip")].toInt() == 2)) {
        sysmonEngine->connectSource(QString("cpu/system/AverageClock"),
                                    this, configuration[QString("interval")].toInt());
        for (int i=0; i<counts[QString("cpu")]; i++)
            sysmonEngine->connectSource(QString("cpu/cpu") + QString::number(i) + QString("/clock"),
                                        this, configuration[QString("interval")].toInt());
    }
    // custom command
    regExp = QRegExp(QString("custom.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("custom"),
                                       this, configuration[QString("interval")].toInt());
    // desktop
    regExp = QRegExp(QString(".*desktop.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("desktop"),
                                       this, configuration[QString("interval")].toInt());
    // disk speed
    regExp = QRegExp(QString("hdd[rw].*"));
    if (foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<configuration[QString("disk")].split(QString("@@")).count(); i++) {
            sysmonEngine->connectSource(configuration[QString("disk")].split(QString("@@"))[i] + QString("/Rate/rblk"),
                                        this, configuration[QString("interval")].toInt());
            sysmonEngine->connectSource(configuration[QString("disk")].split(QString("@@"))[i] + QString("/Rate/wblk"),
                                        this, configuration[QString("interval")].toInt());
        }
    // fan
    regExp = QRegExp(QString("fan.*"));
    if (foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<configuration[QString("fanDevice")].split(QString("@@")).count(); i++)
            sysmonEngine->connectSource(configuration[QString("fanDevice")].split(QString("@@"))[i],
                                        this, configuration[QString("interval")].toInt());
    // gpu
    regExp = QRegExp(QString("(^|bar[0-9].*)gpu"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (foundBars.indexOf(regExp) > -1))
        extsysmonEngine->connectSource(QString("gpu"),
                                       this, configuration[QString("interval")].toInt());
    // gputemp
    regExp = QRegExp(QString("gputemp"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("gputemp"),
                                       this, configuration[QString("interval")].toInt());
    // mount
    regExp = QRegExp(QString("(^|bar[0-9].*)hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb).*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (foundBars.indexOf(regExp) > -1))
        for (int i=0; i<configuration[QString("mount")].split(QString("@@")).count(); i++) {
            sysmonEngine->connectSource(QString("partitions") + configuration[QString("mount")].split(QString("@@"))[i] + QString("/filllevel"),
                                        this, configuration[QString("interval")].toInt());
            sysmonEngine->connectSource(QString("partitions") + configuration[QString("mount")].split(QString("@@"))[i] + QString("/freespace"),
                                        this, configuration[QString("interval")].toInt());
            sysmonEngine->connectSource(QString("partitions") + configuration[QString("mount")].split(QString("@@"))[i] + QString("/usedspace"),
                                        this, configuration[QString("interval")].toInt());
        }
    // hddtemp
    regExp = QRegExp(QString("hddtemp.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("hddtemp"),
                                       this, configuration[QString("interval")].toInt());
    // memory
    regExp = QRegExp(QString("(^|bar[0-9].*)mem.*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (configuration[QString("memTooltip")].toInt() == 2) ||
            (foundBars.indexOf(regExp) > -1)) {
        sysmonEngine->connectSource(QString("mem/physical/free"),
                                    this, configuration[QString("interval")].toInt());
        sysmonEngine->connectSource(QString("mem/physical/used"),
                                    this, configuration[QString("interval")].toInt());
        sysmonEngine->connectSource(QString("mem/physical/application"),
                                    this, configuration[QString("interval")].toInt());
    }
    // network
    networkDeviceUpdate = 0;
    regExp = QRegExp(QString("(down|up|netdev)"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (configuration[QString("downTooltip")].toInt() == 2)) {
        sysmonEngine->connectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/transmitter/data"),
                                    this, configuration[QString("interval")].toInt());
        sysmonEngine->connectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/receiver/data"),
                                    this, configuration[QString("interval")].toInt());
    }
    // package manager
    regExp = QRegExp(QString("pkgcount.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("pkg"),
                                       this, 60*60*1000, Plasma::AlignToHour);
    // player
    regExp = QRegExp(QString("(album|artist|duration|progress|title)"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("player"),
                                       this, configuration[QString("interval")].toInt());
    // ps
    regExp = QRegExp(QString("ps.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->connectSource(QString("ps"),
                                       this, configuration[QString("interval")].toInt());
    // swap
    regExp = QRegExp(QString("(^|bar[0-9].*)swap.*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (configuration[QString("swapTooltip")].toInt() == 2) ||
            (foundBars.indexOf(regExp) > -1)) {
        sysmonEngine->connectSource(QString("mem/swap/free"),
                                    this, configuration[QString("interval")].toInt());
        sysmonEngine->connectSource(QString("mem/swap/used"),
                                    this, configuration[QString("interval")].toInt());
    }
    // temp
    regExp = QRegExp(QString("temp.*"));
    if (foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<configuration[QString("tempDevice")].split(QString("@@")).count(); i++)
            sysmonEngine->connectSource(configuration[QString("tempDevice")].split(QString("@@"))[i],
                                        this, configuration[QString("interval")].toInt());
    // time
    regExp = QRegExp(QString("(^|iso|short|long|c)time"));
    if (foundKeys.indexOf(regExp) > -1)
        timeEngine->connectSource(QString("Local"),
                                  this, 1000);
    // uptime
    regExp = QRegExp(QString("(^|c)uptime"));
    if (foundKeys.indexOf(regExp) > -1)
        sysmonEngine->connectSource(QString("system/uptime"),
                                    this, configuration[QString("interval")].toInt());
}


void AwesomeWidget::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source" << sourceName;

    // regular expressions
    QRegExp cpuRegExp = QRegExp(QString("cpu/cpu.*/TotalLoad"));
    QRegExp cpuclRegExp = QRegExp(QString("cpu/cpu.*/clock"));
    QRegExp hddrRegExp = QRegExp(QString("disk/.*/Rate/rblk"));
    QRegExp hddwRegExp = QRegExp(QString("disk/.*/Rate/wblk"));
    QRegExp mountFillRegExp = QRegExp(QString("partitions/.*/filllevel"));
    QRegExp mountFreeRegExp = QRegExp(QString("partitions/.*/freespace"));
    QRegExp mountUsedRegExp = QRegExp(QString("partitions/.*/usedspace"));
    QRegExp netRecRegExp = QRegExp(QString("network/interfaces/.*/receiver/data"));
    QRegExp netTransRegExp = QRegExp(QString("network/interfaces/.*/transmitter/data"));
    QRegExp tempRegExp = QRegExp(QString("lmsensors/.*"));

    if (data.keys().isEmpty()) return;
    if (sourceName == QString("battery")) {
        for (int i=0; i<data.keys().count(); i++) {
            if (data.keys()[i] == QString("ac")) {
                if (data[QString("ac")].toBool())
                    values[QString("ac")] = configuration[QString("acOnline")];
                else
                    values[QString("ac")] = configuration[QString("acOffline")];
            } else {
                values[data.keys()[i]] = QString("%1").arg(data[data.keys()[i]].toFloat(), 3, 'f', 0);
                if (foundBars.indexOf(QRegExp(QString("bar[0-9].*bat"))) > -1) {
                    for (int j=0; j<foundBars.count(); j++) {
                        if (getItemByTag(foundBars[j])->getBar() != data.keys()[i]) continue;
                        values[foundBars[j]] = values[data.keys()[i]];
                    }
                }
            }
        }
        if ((configuration[QString("batteryTooltip")].toInt() == 2) &&
                (!isnan(data[QString("bat")].toFloat()))) {
            if (tooltipValues[QString("bat")].count() > configuration[QString("tooltipNumber")].toInt())
                tooltipValues[QString("bat")].takeFirst();
            if (data[QString("ac")].toBool())
                tooltipValues[QString("bat")].append(data[QString("bat")].toFloat());
            else
                tooltipValues[QString("bat")].append(-data[QString("bat")].toFloat());
        }
    } else if (sourceName == QString("cpu/system/TotalLoad")) {
        values[QString("cpu")] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
        if (foundBars.indexOf(QRegExp(QString("bar[0-9].*cpu(?!cl).*"))) > -1) {
            for (int j=0; j<foundBars.count(); j++) {
                if (getItemByTag(foundBars[j])->getBar() != QString("cpu")) continue;
                values[foundBars[j]] = QString("%1").arg(data[QString("value")].toFloat());
            }
        }
        if ((configuration[QString("cpuTooltip")].toInt() == 2) &&
                (!isnan(data[QString("value")].toFloat()))) {
            if (tooltipValues[QString("cpu")].count() > configuration[QString("tooltipNumber")].toInt())
                tooltipValues[QString("cpu")].takeFirst();
            tooltipValues[QString("cpu")].append(data[QString("value")].toFloat());
        }
    } else if (sourceName.contains(cpuRegExp)) {
        QString number = sourceName;
        number.remove(QString("cpu/cpu"));
        number.remove(QString("/TotalLoad"));
        values[QString("cpu") + number] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
        if (foundBars.indexOf(QRegExp(QString("bar[0-9].*cpu(?!cl).*"))) > -1) {
            for (int j=0; j<foundBars.count(); j++) {
                if (getItemByTag(foundBars[j])->getBar() != (QString("cpu") + number)) continue;
                values[foundBars[j]] = values[QString("cpu") + number];
            }
        }
    } else if (sourceName == QString("cpu/system/AverageClock")) {
        values[QString("cpucl")] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
        if ((configuration[QString("cpuclTooltip")].toInt() == 2) &&
                (!isnan(data[QString("value")].toFloat()))) {
            if (tooltipValues[QString("cpucl")].count() > configuration[QString("tooltipNumber")].toInt())
                tooltipValues[QString("cpucl")].takeFirst();
            tooltipValues[QString("cpucl")].append(data[QString("value")].toFloat());
        }
    } else if (sourceName.contains(cpuclRegExp)) {
        QString number = sourceName;
        number.remove(QString("cpu/cpu"));
        number.remove(QString("/clock"));
        values[QString("cpucl") + number] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
    } else if (sourceName == QString("custom")) {
        for (int i=0; i<data.keys().count(); i++)
            values[data.keys()[i]] = data[data.keys()[i]].toString();
    } else if (sourceName == QString("desktop")) {
        values[QString("desktop")] = data[QString("currentName")].toString();
        values[QString("ndesktop")] = QString("%1").arg(data[QString("currentNumber")].toInt());
        values[QString("tdesktops")] = QString("%1").arg(data[QString("number")].toInt());
    } else if (sourceName.contains(hddrRegExp)) {
        QString device = sourceName;
        device.remove(QString("/Rate/rblk"));
        for (int i=0; i<counts[QString("disk")]; i++)
            if (configuration[QString("disk")].split(QString("@@"))[i] == device) {
                values[QString("hddr") + QString::number(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 0);
                break;
            }
    } else if (sourceName.contains(hddwRegExp)) {
        QString device = sourceName;
        device.remove(QString("/Rate/wblk"));
        for (int i=0; i<counts[QString("disk")]; i++)
            if (configuration[QString("disk")].split(QString("@@"))[i] == device) {
                values[QString("hddw") + QString::number(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 0);
                break;
            }
    } else if (sourceName == QString("gpu")) {
        values[QString("gpu")] = QString("%1").arg(data[QString("GPU")].toFloat(), 5, 'f', 1);
        if (foundBars.indexOf(QRegExp(QString("bar[0-9].*gpu"))) > -1) {
            for (int j=0; j<foundBars.count(); j++) {
                if (getItemByTag(foundBars[j])->getBar() != QString("gpu")) continue;
                values[foundBars[j]] = values[QString("gpu")];
            }
        }
    } else if (sourceName == QString("gputemp")) {
        values[QString("gputemp")] = QString("%1").arg(getTemp(data[QString("GPUTemp")].toFloat()), 4, 'f', 1);
    } else if (sourceName.contains(mountFillRegExp)) {
        QString mount = sourceName;
        mount.remove(QString("partitions"));
        mount.remove(QString("/filllevel"));
        for (int i=0; i<counts[QString("mount")]; i++)
            if (configuration[QString("mount")].split(QString("@@"))[i] == mount) {
                values[QString("hdd") + QString::number(i)] = QString("%1").arg(data[QString("value")].toFloat(), 5, 'f', 1);
                if (foundBars.indexOf(QRegExp(QString("bar[0-9].*hdd([0-9].*"))) > -1) {
                    for (int j=0; j<foundBars.count(); j++) {
                        if (getItemByTag(foundBars[j])->getBar() != (QString("hdd") + QString::number(i))) continue;
                        values[foundBars[j]] = values[QString("hdd") + QString::number(i)];
                    }
                }
            }
    } else if (sourceName.contains(mountFreeRegExp)) {
        QString mount = sourceName;
        mount.remove(QString("partitions"));
        mount.remove(QString("/freespace"));
        for (int i=0; i<counts[QString("mount")]; i++)
            if (configuration[QString("mount")].split(QString("@@"))[i] == mount) {
                values[QString("hddfreemb") + QString::number(i)] = QString("%1").arg(
                            data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
                values[QString("hddfreegb") + QString::number(i)] = QString("%1").arg(
                            data[QString("value")].toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
                break;
            }
    } else if (sourceName.contains(mountUsedRegExp)) {
        QString mount = sourceName;
        mount.remove(QString("partitions"));
        mount.remove(QString("/usedspace"));
        for (int i=0; i<counts[QString("mount")]; i++)
            if (configuration[QString("mount")].split(QString("@@"))[i] == mount) {
                values[QString("hddmb") + QString::number(i)] = QString("%1").arg(
                            data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
                values[QString("hddgb") + QString::number(i)] = QString("%1").arg(
                            data[QString("value")].toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
                // total
                values[QString("hddtotmb") + QString::number(i)] = QString("%1").arg(
                            values[QString("hddfreemb") + QString::number(i)].toInt() +
                            values[QString("hddmb") + QString::number(i)].toInt());
                values[QString("hddtotgb") + QString::number(i)] = QString("%1").arg(
                            values[QString("hddfreegb") + QString::number(i)].toFloat() +
                            values[QString("hddgb") + QString::number(i)].toFloat(),
                            5, 'f', 1);
                break;
            }
    } else if (sourceName == QString("hddtemp")) {
        for (int i=0; i<data.keys().count(); i++)
            for (int j=0; j<counts[QString("hddtemp")]; j++)
                if (data.keys()[i] == configuration[QString("hdd")].split(QString("@@"))[j]) {
                    values[QString("hddtemp") + QString::number(j)] = QString("%1").arg(getTemp(data[data.keys()[i]].toFloat()), 4, 'f', 1);
                    break;
                }
    } else if (sourceName == QString("mem/physical/application")) {
        values[QString("memmb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
        values[QString("memgb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 5, 'f', 1);
    } else if (sourceName == QString("mem/physical/free")) {
        values[QString("memfreemb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
        values[QString("memfreegb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 4, 'f', 1);
    } else if (sourceName == QString("mem/physical/used")) {
        values[QString("memusedmb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 0, 'f', 0);
        values[QString("memusedgb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 4, 'f', 1);
        // total
        values[QString("memtotmb")] = QString("%1").arg(
                    values[QString("memusedmb")].toInt() + values[QString("memfreemb")].toInt(), 5);
        values[QString("memtotgb")] = QString("%1").arg(
                    values[QString("memusedgb")].toFloat() + values[QString("memfreegb")].toFloat(),
                    4, 'f', 1);
        // percentage
        values[QString("mem")] = QString("%1").arg(100.0 * values[QString("memmb")].toFloat() / values[QString("memtotmb")].toFloat(),
                                                   5, 'f', 1);
        if ((configuration[QString("memTooltip")].toInt() == 2) &&
                (!isnan(values[QString("mem")].toFloat()))) {
            if (tooltipValues[QString("mem")].count() > configuration[QString("tooltipNumber")].toInt())
                tooltipValues[QString("mem")].takeFirst();
            tooltipValues[QString("mem")].append(values[QString("mem")].toFloat());
        }
        if (foundBars.indexOf(QRegExp(QString("bar[0-9].*mem"))) > -1) {
            for (int j=0; j<foundBars.count(); j++) {
                if (getItemByTag(foundBars[j])->getBar() != QString("mem")) continue;
                values[foundBars[j]] = values[QString("mem")];
            }
        }
    } else if (sourceName.contains(netRecRegExp)) {
        values[QString("down")] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
        if ((configuration[QString("downTooltip")].toInt() == 2) &&
                (!isnan(data[QString("value")].toFloat()))) {
            if (tooltipValues[QString("down")].count() > configuration[QString("tooltipNumber")].toInt())
                tooltipValues[QString("down")].takeFirst();
            tooltipValues[QString("down")].append(data[QString("value")].toFloat());
        }
        networkDeviceUpdate++;
        if (networkDeviceUpdate >= 30) {
            networkDeviceUpdate = 0;
            if (configuration[QString("useCustomNetdev")].toInt() == 0) {
                sysmonEngine->disconnectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/transmitter/data"), this);
                sysmonEngine->disconnectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/receiver/data"), this);
                updateNetworkDevice(false);
                sysmonEngine->connectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/transmitter/data"),
                                            this, configuration[QString("interval")].toInt());
                sysmonEngine->connectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/receiver/data"),
                                            this, configuration[QString("interval")].toInt());
            }
        }
    } else if (sourceName.contains(netTransRegExp)) {
        values[QString("up")] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 0);
        if ((configuration[QString("downTooltip")].toInt() == 2) &&
                (!isnan(data[QString("value")].toFloat()))) {
            if (tooltipValues[QString("up")].count() > configuration[QString("tooltipNumber")].toInt())
                tooltipValues[QString("up")].takeFirst();
            tooltipValues[QString("up")].append(data[QString("value")].toFloat());
        }
    } else if (sourceName == QString("pkg")) {
        for (int i=0; i<data.keys().count(); i++)
            values[data.keys()[i].toLower()] = QString("%1").arg(data[data.keys()[i]].toInt(), 2);
    } else if (sourceName == QString("player")) {
        values[QString("album")] = data[QString("album")].toString();
        values[QString("artist")] = data[QString("artist")].toString();
        values[QString("duration")] = data[QString("duration")].toString();
        values[QString("progress")] = data[QString("progress")].toString();
        values[QString("title")] = data[QString("title")].toString();
    } else if (sourceName == QString("ps")) {
        values[QString("ps")] = data[QString("ps")].toString();
        values[QString("pscount")] = QString("%1").arg(data[QString("psCount")].toInt(), 2);
        values[QString("pstotal")] = QString("%1").arg(data[QString("psTotal")].toInt(), 3);
    } else if (sourceName == QString("mem/swap/free")) {
        values[QString("swapfreemb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
        values[QString("swapfreegb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 4, 'f', 1);
    } else if (sourceName == QString("mem/swap/used")) {
        values[QString("swapmb")] = QString("%1").arg(data[QString("value")].toFloat() / 1024.0, 5, 'f', 0);
        values[QString("swapgb")] = QString("%1").arg(data[QString("value")].toFloat() / (1024.0 * 1024.0), 4, 'f', 1);
        // total
        values[QString("swaptotmb")] = QString("%1").arg(
                    values[QString("swapmb")].toInt() + values[QString("swapfreemb")].toInt(), 5);
        values[QString("swaptotgb")] = QString("%1").arg(
                    values[QString("swapgb")].toFloat() + values[QString("swapfreegb")].toFloat(), 4, 'f', 1);
        // percentage
        values[QString("swap")] = QString("%1").arg(100.0 * values[QString("swapmb")].toFloat() / values[QString("swaptotmb")].toFloat(),
                                                    5, 'f', 1);
        if ((configuration[QString("swapTooltip")].toInt() == 2) &&
                (!isnan(values[QString("swap")].toFloat()))) {
            if (tooltipValues[QString("swap")].count() > configuration[QString("tooltipNumber")].toInt())
                tooltipValues[QString("swap")].takeFirst();
            tooltipValues[QString("swap")].append(values[QString("swap")].toFloat());
        }
        if (foundBars.indexOf(QRegExp(QString("bar[0-9].*swap"))) > -1) {
            for (int j=0; j<foundBars.count(); j++) {
                if (getItemByTag(foundBars[j])->getBar() != QString("swap")) continue;
                values[foundBars[j]] = values[QString("swap")];
            }
        }
    } else if (sourceName.contains(tempRegExp)) {
        if (data[QString("units")].toString() == QString("rpm")) {
            for (int i=0; i<counts[QString("fan")]; i++)
                if (sourceName == configuration[QString("fanDevice")].split(QString("@@"))[i]) {
                    values[QString("fan") + QString::number(i)] = QString("%1").arg(data[QString("value")].toFloat(), 4, 'f', 1);
                    break;
                }
        } else {
            for (int i=0; i<counts[QString("temp")]; i++)
                if (sourceName == configuration[QString("tempDevice")].split(QString("@@"))[i]) {
                    values[QString("temp") + QString::number(i)] = QString("%1").arg(getTemp(data[QString("value")].toFloat()), 4, 'f', 1);
                    break;
                }
        }
    } else if (sourceName == QString("Local")) {
        values[QString("time")] = data[QString("DateTime")].toDateTime().toString(Qt::TextDate);
        values[QString("isotime")] = data[QString("DateTime")].toDateTime().toString(Qt::ISODate);
        values[QString("shorttime")] = data[QString("DateTime")].toDateTime().toString(Qt::SystemLocaleShortDate);
        values[QString("longtime")] = data[QString("DateTime")].toDateTime().toString(Qt::SystemLocaleLongDate);
        QStringList timeKeys = getTimeKeys();
        values[QString("ctime")] = configuration[QString("customTime")];
        for (int i=0; i<timeKeys.count(); i++)
            values[QString("ctime")].replace(QString("$") + timeKeys[i],
                                             data[QString("DateTime")].toDateTime().toString(timeKeys[i]));
    } else if (sourceName == QString("system/uptime")) {
        int uptime = data[QString("value")].toFloat();
        int seconds = uptime - uptime % 60;
        int minutes = seconds / 60 % 60;
        int hours = ((seconds / 60) - minutes) / 60 % 24;
        int days = (((seconds / 60) - minutes) / 60 - hours) / 24;
        values[QString("uptime")] = QString("%1d%2h%3m").arg(days, 3).arg(hours, 2).arg(minutes, 2);
        values[QString("cuptime")] = configuration[QString("customUptime")];
        values[QString("cuptime")].replace(QString("$dd"), QString("%1").arg(days, 3, 10, QChar('0')));
        values[QString("cuptime")].replace(QString("$d"), QString("%1").arg(days));
        values[QString("cuptime")].replace(QString("$hh"), QString("%1").arg(hours, 2, 10, QChar('0')));
        values[QString("cuptime")].replace(QString("$h"), QString("%1").arg(hours));
        values[QString("cuptime")].replace(QString("$mm"), QString("%1").arg(minutes, 2, 10, QChar('0')));
        values[QString("cuptime")].replace(QString("$m"), QString("%1").arg(minutes));
    }
}


void AwesomeWidget::disconnectFromEngine()
{
    if (debug) qDebug() << PDEBUG;
    QRegExp regExp;

    // battery
    regExp = QRegExp(QString("(^|bar[0-9].*)(ac|bat.*)"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (configuration[QString("batteryTooltip")].toInt() == 2) ||
            (foundBars.indexOf(regExp) > -1))
        extsysmonEngine->disconnectSource(QString("battery"), this);
    // cpu
    regExp = QRegExp(QString("(^|bar[0-9].*)cpu(?!cl).*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
        (configuration[QString("cpuTooltip")].toInt() == 2) ||
        (foundBars.indexOf(regExp) > -1)) {
        sysmonEngine->disconnectSource(QString("cpu/system/TotalLoad"), this);
        for (int i=0; i<counts[QString("cpu")]; i++)
            sysmonEngine->disconnectSource(QString("cpu/cpu") + QString::number(i) + QString("/TotalLoad"), this);
    }
    // cpuclock
    regExp = QRegExp(QString("cpucl.*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
        (configuration[QString("cpuclTooltip")].toInt() == 2)) {
        sysmonEngine->disconnectSource(QString("cpu/system/AverageClock"), this);
        for (int i=0; i<counts[QString("cpu")]; i++)
            sysmonEngine->disconnectSource(QString("cpu/cpu") + QString::number(i) + QString("/clock"), this);
    }
    // custom command
    regExp = QRegExp(QString("custom.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->disconnectSource(QString("custom"), this);
    // desktop
    regExp = QRegExp(QString(".*desktop.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->disconnectSource(QString("desktop"), this);
    // disk speed
    regExp = QRegExp(QString("hdd[rw].*"));
    if (foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<configuration[QString("disk")].split(QString("@@")).count(); i++) {
            sysmonEngine->disconnectSource(configuration[QString("disk")].split(QString("@@"))[i] + QString("/Rate/rblk"), this);
            sysmonEngine->disconnectSource(configuration[QString("disk")].split(QString("@@"))[i] + QString("/Rate/wblk"), this);
        }
    // fan
    regExp = QRegExp(QString("fan.*"));
    if (foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<configuration[QString("fanDevice")].split(QString("@@")).count(); i++)
            sysmonEngine->disconnectSource(configuration[QString("fanDevice")].split(QString("@@"))[i], this);
    // gpu
    regExp = QRegExp(QString("(^|bar[0-9].*)gpu"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (foundBars.indexOf(regExp) > -1))
        extsysmonEngine->disconnectSource(QString("gpu"), this);
    // gputemp
    regExp = QRegExp(QString("gputemp"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->disconnectSource(QString("gputemp"), this);
    // mount
    regExp = QRegExp(QString("(^|bar[0-9].*)hdd([0-9]|mb|gb|freemb|freegb|totmb|totgb).*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
            (foundBars.indexOf(regExp) > -1))
        for (int i=0; i<configuration[QString("mount")].split(QString("@@")).count(); i++) {
            sysmonEngine->disconnectSource(QString("partitions") + configuration[QString("mount")].split(QString("@@"))[i] + QString("/filllevel"), this);
            sysmonEngine->disconnectSource(QString("partitions") + configuration[QString("mount")].split(QString("@@"))[i] + QString("/freespace"), this);
            sysmonEngine->disconnectSource(QString("partitions") + configuration[QString("mount")].split(QString("@@"))[i] + QString("/usedspace"), this);
        }
    // hddtemp
    regExp = QRegExp(QString("hddtemp.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->disconnectSource(QString("hddtemp"), this);
    // memory
    regExp = QRegExp(QString("(^|bar[0-9].*)mem.*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
        (configuration[QString("memTooltip")].toInt() == 2) ||
        (foundBars.indexOf(regExp) > -1)) {
        sysmonEngine->disconnectSource(QString("mem/physical/free"), this);
        sysmonEngine->disconnectSource(QString("mem/physical/used"), this);
        sysmonEngine->disconnectSource(QString("mem/physical/application"), this);
    }
    // network
    regExp = QRegExp(QString("(down|up|netdev)"));
    if ((foundKeys.indexOf(regExp) > -1) ||
        (configuration[QString("downTooltip")].toInt() == 2)) {
        sysmonEngine->disconnectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/transmitter/data"), this);
        sysmonEngine->disconnectSource(QString("network/interfaces/") + values[QString("netdev")] + QString("/receiver/data"), this);
    }
    // package manager
    regExp = QRegExp(QString("pkgcount.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->disconnectSource(QString("pkg"), this);
    // player
    regExp = QRegExp(QString("(album|artist|duration|progress|title)"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->disconnectSource(QString("player"), this);
    // ps
    regExp = QRegExp(QString("ps.*"));
    if (foundKeys.indexOf(regExp) > -1)
        extsysmonEngine->disconnectSource(QString("ps"), this);
    // swap
    regExp = QRegExp(QString("(^|bar[0-9].*)swap.*"));
    if ((foundKeys.indexOf(regExp) > -1) ||
        (configuration[QString("swapTooltip")].toInt() == 2) ||
        (foundBars.indexOf(regExp) > -1)) {
        sysmonEngine->disconnectSource(QString("mem/swap/free"), this);
        sysmonEngine->disconnectSource(QString("mem/swap/used"), this);
    }
    // temp
    regExp = QRegExp(QString("temp.*"));
    if (foundKeys.indexOf(regExp) > -1)
        for (int i=0; i<configuration[QString("tempDevice")].split(QString("@@")).count(); i++)
            sysmonEngine->disconnectSource(configuration[QString("tempDevice")].split(QString("@@"))[i], this);
    // time
    regExp = QRegExp(QString("(^|iso|short|long|c)time"));
    if (foundKeys.indexOf(regExp) > -1)
        timeEngine->disconnectSource(QString("Local"), this);
    // uptime
    regExp = QRegExp(QString("(^|c)uptime"));
    if (foundKeys.indexOf(regExp) > -1)
        sysmonEngine->disconnectSource(QString("system/uptime"), this);
}