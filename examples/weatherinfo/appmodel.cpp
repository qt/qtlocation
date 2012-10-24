/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "appmodel.h"

#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#include <qnmeapositioninfosource.h>
#include <qgeopositioninfo.h>
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

#include <QSignalMapper>
#include <QXmlStreamReader>
#include <QRegExp>
#include <QStringList>
#include <QUrlQuery>

WeatherData::WeatherData(QObject *parent) :
        QObject(parent)
{
}

WeatherData::WeatherData(const WeatherData &other) :
        QObject(0),
        m_dayOfWeek(other.m_dayOfWeek),
        m_weather(other.m_weather),
        m_weatherDesc(other.m_weatherDesc),
        m_tempString(other.m_tempString)
{
}

const WeatherData &WeatherData::operator=(const WeatherData &other)
{
    if (this == &other)
        return *this;

    m_dayOfWeek = other.m_dayOfWeek;
    m_weather = other.m_weather;
    m_weatherDesc = other.m_weatherDesc;
    m_tempString = other.m_tempString;
    return *this;
}

QString WeatherData::dayOfWeek() const
{
    return m_dayOfWeek;
}

QString WeatherData::weather() const
{
    return m_weather;
}

QString WeatherData::weatherDesc() const
{
    return m_weatherDesc;
}

QString WeatherData::tempString() const
{
    return m_tempString;
}

void WeatherData::setDayOfWeek(QString value)
{
    m_dayOfWeek = value;
    emit dataChanged();
}

void WeatherData::setWeather(QString value)
{
    m_weather = value;
    emit dataChanged();
}

void WeatherData::setWeatherDesc(QString value)
{
    m_weatherDesc = value;
    emit dataChanged();
}

void WeatherData::setTempString(QString value)
{
    m_tempString = value;
    emit dataChanged();
}

class AppModelPrivate
{
public:
    QGeoPositionInfoSource *src;
    QGeoCoordinate coord;
    QString city;
    QNetworkAccessManager *nam;
    QNetworkSession *ns;
    WeatherData now;
    QList<WeatherData*> forecast;
    QQmlListProperty<WeatherData> *fcProp;
    QSignalMapper *geoReplyMapper;
    QSignalMapper *weatherReplyMapper;
    bool ready;
    bool useGps;

    AppModelPrivate() :
            src(NULL),
            nam(NULL),
            ns(NULL),
            fcProp(NULL),
            ready(false),
            useGps(true)
    {}
};

static void forecastAppend(QQmlListProperty<WeatherData> *prop, WeatherData *val)
{
    Q_UNUSED(val);
    Q_UNUSED(prop);
}

static WeatherData *forecastAt(QQmlListProperty<WeatherData> *prop, int index)
{
    AppModelPrivate *d = static_cast<AppModelPrivate*>(prop->data);
    return d->forecast.at(index);
}

static int forecastCount(QQmlListProperty<WeatherData> *prop)
{
    AppModelPrivate *d = static_cast<AppModelPrivate*>(prop->data);
    return d->forecast.size();
}

static void forecastClear(QQmlListProperty<WeatherData> *prop)
{
    static_cast<AppModelPrivate*>(prop->data)->forecast.clear();
}

//! [0]
AppModel::AppModel(QObject *parent) :
        QObject(parent),
        d(new AppModelPrivate)
{
//! [0]
    d->fcProp = new QQmlListProperty<WeatherData>(this, d,
                                                          forecastAppend,
                                                          forecastCount,
                                                          forecastAt,
                                                          forecastClear);

    d->geoReplyMapper = new QSignalMapper(this);
    d->weatherReplyMapper = new QSignalMapper(this);

    connect(d->geoReplyMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(handleGeoNetworkData(QObject*)));
    connect(d->weatherReplyMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(handleWeatherNetworkData(QObject*)));

//! [1]
    // make sure we have an active network session
    d->nam = new QNetworkAccessManager(this);

    QNetworkConfigurationManager ncm;
    d->ns = new QNetworkSession(ncm.defaultConfiguration(), this);
    connect(d->ns, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    // the session may be already open. if it is, run the slot directly
    if (d->ns->isOpen())
        this->networkSessionOpened();
    // tell the system we want network
    d->ns->open();
}
//! [1]

AppModel::~AppModel()
{
    d->ns->close();
    if (d->src)
        d->src->stopUpdates();
    delete d;
}

//! [2]
void AppModel::networkSessionOpened()
{
    d->src = QGeoPositionInfoSource::createDefaultSource(this);

    if (d->src) {
        d->useGps = true;
        connect(d->src, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));
        d->src->startUpdates();
    } else {
        d->useGps = false;
        d->city = "Brisbane";
        emit cityChanged();
        this->refreshWeather();
    }
}
//! [2]

//! [3]
void AppModel::positionUpdated(QGeoPositionInfo gpsPos)
{
    d->coord = gpsPos.coordinate();

    if (!(d->useGps))
        return;

    QString latitude, longitude;
    longitude.setNum(d->coord.longitude());
    latitude.setNum(d->coord.latitude());
//! [3]
    QUrl url("http://maps.google.com/maps/geo");
    QUrlQuery query;
    query.addQueryItem("q", latitude + "," + longitude);
    query.addQueryItem("output", "xml");
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up the signal right away
    d->geoReplyMapper->setMapping(rep, rep);
    connect(rep, SIGNAL(finished()),
            d->geoReplyMapper, SLOT(map()));
    // it might have finished though while we were connecting it
    // if so, pass it straight on
    if (rep->isFinished())
        this->handleGeoNetworkData(rep);
}

void AppModel::handleGeoNetworkData(QObject *replyObj)
{
    QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(replyObj);
    if (!networkReply)
        return;

    if (!networkReply->error()) {
        QString xml = QString::fromUtf8(networkReply->readAll());

        int start = xml.indexOf("<LocalityName>");
        int end = xml.indexOf("</LocalityName>");

        QString city = xml.mid(start + 14, end - start - 14);
        if (city != d->city) {
            emit cityChanged();
            this->refreshWeather();
        }
    }
    networkReply->deleteLater();
}

void AppModel::refreshWeather()
{
    QUrl url("http://www.google.com/ig/api");
    QUrlQuery query;
    query.addQueryItem("hl", "en");
    query.addQueryItem("weather", d->city);
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    // connect up the signal right away
    d->weatherReplyMapper->setMapping(rep, rep);
    connect(rep, SIGNAL(finished()),
            d->weatherReplyMapper, SLOT(map()));
    // it might have finished though while we were connecting it
    // if so, pass it straight on
    if (rep->isFinished())
        this->handleWeatherNetworkData(rep);
}

static QString google2name(QString weather)
{
    static QHash<QString, QString> conv;
    if (conv.isEmpty()) {
        conv["mostly_cloudy"]    = "few-clouds";
        conv["cloudy"]           = "overcast";
        conv["mostly_sunny"]     = "sunny-very-few-clouds";
        conv["partly_cloudy"]    = "sunny-very-few-clouds";
        conv["sunny"]            = "sunny";
        conv["flurries"]         = "snow";
        conv["fog"]              = "fog";
        conv["haze"]             = "haze";
        conv["icy"]              = "icy";
        conv["sleet"]            = "sleet";
        conv["chance_of_sleet"]  = "sleet";
        conv["snow"]             = "snow";
        conv["chance_of_snow"]   = "snow";
        conv["mist"]             = "showers";
        conv["rain"]             = "showers";
        conv["chance_of_rain"]   = "showers";
        conv["storm"]            = "storm";
        conv["chance_of_storm"]  = "storm";
        conv["thunderstorm"]     = "thundershower";
        conv["chance_of_tstorm"] = "thundershower";
    }
    QRegExp regex("([\\w]+).gif$");
    if (regex.indexIn(weather) != -1) {
        QString i = regex.cap();
        i = i.left(i.length() - 4);
        QString name = conv.value(i);
        if (!name.isEmpty())
            return name;
    }
    return QString();
}

// little helper for repetitive code
#define GET_DATA_ATTR xml.attributes().value("data").toString()

void AppModel::handleWeatherNetworkData(QObject *replyObj)
{
    QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(replyObj);
    if (!networkReply)
        return;

    if (!networkReply->error()) {
        QString xmlData = QString::fromUtf8(networkReply->readAll());

        foreach (WeatherData *inf, d->forecast)
            delete inf;
        d->forecast.clear();

        QXmlStreamReader xml(xmlData);
        while (!xml.atEnd()) {
            xml.readNext();

            if (xml.name() == "current_conditions") {
                while (!xml.atEnd()) {
                    xml.readNext();
                    if (xml.name() == "current_conditions")
                        break;
                    if (xml.tokenType() == QXmlStreamReader::StartElement) {
                        if (xml.name() == "condition") {
                            d->now.setWeatherDesc(GET_DATA_ATTR);
                        } else if (xml.name() == "icon") {
                            d->now.setWeather(google2name(GET_DATA_ATTR));
                        } else if (xml.name() == "temp_f") {
                            d->now.setTempString(GET_DATA_ATTR + QChar(176));
                        }
                    }
                }
            }

            if (xml.name() == "forecast_conditions") {
                WeatherData *cur = NULL;

                while (!xml.atEnd()) {
                    xml.readNext();

                    if (xml.name() == "forecast_conditions") {
                        if (cur) {
                            d->forecast.append(cur);
                        }
                        break;
                    } else if (xml.tokenType() == QXmlStreamReader::StartElement) {
                        if (!cur)
                            cur = new WeatherData();
                        if (xml.name() == "day_of_week") {
                            cur->setDayOfWeek(GET_DATA_ATTR);
                        } else if (xml.name() == "icon") {
                            cur->setWeather(google2name(GET_DATA_ATTR));
                        } else if (xml.name() == "low") {
                            QString v = cur->tempString();
                            QStringList parts = v.split("/");
                            if (parts.size() >= 1)
                                parts.replace(0, GET_DATA_ATTR + QChar(176));
                            if (parts.size() == 0)
                                parts.append(GET_DATA_ATTR + QChar(176));

                            cur->setTempString(parts.join("/"));
                        } else if (xml.name() == "high") {
                            QString v = cur->tempString();
                            QStringList parts = v.split("/");
                            if (parts.size() == 2)
                                parts.replace(1, GET_DATA_ATTR + QChar(176));
                            if (parts.size() == 0)
                                parts.append("");
                            if (parts.size() == 1)
                                parts.append(GET_DATA_ATTR + QChar(176));

                            cur->setTempString(parts.join("/"));
                        }
                    }
                }
            }
        }

        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }

        emit weatherChanged();
    }
    networkReply->deleteLater();
}

bool AppModel::hasValidCity() const
{
    return (!(d->city.isEmpty()) && d->city.size() > 1 && d->city != "");
}

bool AppModel::hasValidWeather() const
{
    return hasValidCity() && (!(d->now.weather().isEmpty()) &&
                              (d->now.weather().size() > 1) &&
                              d->now.weather() != "");
}

WeatherData *AppModel::weather() const
{
    return &(d->now);
}

QQmlListProperty<WeatherData> AppModel::forecast() const
{
    return *(d->fcProp);
}

bool AppModel::ready() const
{
    return d->ready;
}

bool AppModel::hasSource() const
{
    return (d->src != NULL);
}

bool AppModel::useGps() const
{
    return d->useGps;
}

void AppModel::setUseGps(bool value)
{
    d->useGps = value;
    if (value) {
        d->city = "";
        emit cityChanged();
        emit weatherChanged();
    }
    emit useGpsChanged();
}

QString AppModel::city() const
{
    return d->city;
}

void AppModel::setCity(const QString &value)
{
    d->city = value;
    emit cityChanged();
    this->refreshWeather();
}
