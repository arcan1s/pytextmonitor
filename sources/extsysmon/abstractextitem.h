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

#ifndef ABSTRACTEXTITEM_H
#define ABSTRACTEXTITEM_H

#include <QDialog>
#include <QVariant>


class AbstractExtItem : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive)
    Q_PROPERTY(int apiVersion READ apiVersion WRITE setApiVersion)
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(QStringList directories READ directories)
    Q_PROPERTY(int interval READ interval WRITE setInterval)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(int number READ number WRITE setNumber)

public:
    explicit AbstractExtItem(QWidget *parent = nullptr,
                             const QString desktopName = QString(),
                             const QStringList directories = QStringList(),
                             const bool debugCmd = false);
    virtual ~AbstractExtItem();
    // get methods
    int apiVersion();
    QString comment();
    QStringList directories();
    QString fileName();
    int interval();
    bool isActive();
    QString name();
    int number();
    QString tag(const QString _type);
    // set methods
    void setApiVersion(const int _apiVersion = 0);
    void setActive(const bool _state = true);
    void setComment(const QString _comment = QString("empty"));
    void setInterval(const int _interval = 1);
    void setName(const QString _name = QString("none"));
    void setNumber(int _number = -1);

public slots:
    virtual void readConfiguration();
    virtual QVariantMap run() = 0;
    virtual int showConfiguration() = 0;
    bool tryDelete();
    virtual void writeConfiguration();

private:
    QString m_fileName;
    QStringList m_dirs;
    bool debug;
    // properties
    int m_apiVersion = 0;
    bool m_active = true;
    QString m_comment = QString("empty");
    int m_interval = 1;
    QString m_name = QString("none");
    int m_number = -1;
};


#endif /* ABSTRACTEXTITEM_H */
