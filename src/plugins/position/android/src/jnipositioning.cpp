/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDateTime>
#include <QMap>
#include <QRandomGenerator>
#include <QGeoPositionInfo>
#include <QJniEnvironment>
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
#include <QCoreApplication>
#include <android/log.h>
#include "qgeopositioninfosource_android_p.h"
#include "qgeosatelliteinfosource_android_p.h"
#include "jnipositioning.h"

class GlobalClassRefWrapper
{
public:
    GlobalClassRefWrapper() = default;
    ~GlobalClassRefWrapper()
    {
        if (m_classRef) {
            QJniEnvironment env;
            if (env.jniEnv())
                env->DeleteGlobalRef(m_classRef);
        }
    }

    bool init(const char *className)
    {
        QJniEnvironment env;
        if (env.jniEnv()) {
            if (m_classRef) {
                env->DeleteGlobalRef(m_classRef);
                m_classRef = nullptr;
            }

            m_classRef = env.findClass(className); // it returns global ref!
        }
        return m_classRef != nullptr;
    }

    jclass operator()() { return m_classRef; }

private:
    jclass m_classRef = nullptr;
};

static GlobalClassRefWrapper positioningClass;

static jmethodID providerListMethodId;
static jmethodID lastKnownPositionMethodId;
static jmethodID startUpdatesMethodId;
static jmethodID stopUpdatesMethodId;
static jmethodID requestUpdateMethodId;
static jmethodID startSatelliteUpdatesMethodId;

static const char logTag[] = "qt.positioning.android";
static const char methodErrorMsg[] = "Can't find method \"%s%s\"";

Q_LOGGING_CATEGORY(lcPositioning, logTag)

namespace {

/*!
    \internal
    This class encapsulates satellite system types, as defined by Android
    GnssStatus API. Initialize during JNI_OnLoad() by the init() method, from
    the Java side, rather than hard-coding.
*/
class ConstellationMapper
{
public:
    static bool init()
    {
        m_gnssStatusObject = nullptr;
        if (QNativeInterface::QAndroidApplication::sdkVersion() > 23) {
            m_gnssStatusObject = QJniEnvironment().findClass("android/location/GnssStatus");
            if (!m_gnssStatusObject)
                return false;
        }
        // no need to query it for API level <= 23
        return true;
    }

    static QGeoSatelliteInfo::SatelliteSystem toSatelliteSystem(int constellationType)
    {
        if (!m_gnssStatusObject)
            return QGeoSatelliteInfo::Undefined;

        static const int gps =
                QJniObject::getStaticField<jint>(m_gnssStatusObject, "CONSTELLATION_GPS");
        static const int glonass =
                QJniObject::getStaticField<jint>(m_gnssStatusObject, "CONSTELLATION_GLONASS");
        static const int galileo =
                QJniObject::getStaticField<jint>(m_gnssStatusObject, "CONSTELLATION_GALILEO");
        static const int beidou =
                QJniObject::getStaticField<jint>(m_gnssStatusObject, "CONSTELLATION_BEIDOU");
        static const int qzss =
                QJniObject::getStaticField<jint>(m_gnssStatusObject, "CONSTELLATION_QZSS");

        if (constellationType == gps) {
            return QGeoSatelliteInfo::GPS;
        } else if (constellationType == glonass) {
            return QGeoSatelliteInfo::GLONASS;
        } else if (constellationType == galileo) {
            return QGeoSatelliteInfo::GALILEO;
        } else if (constellationType == beidou) {
            return QGeoSatelliteInfo::BEIDOU;
        } else if (constellationType == qzss){
            return QGeoSatelliteInfo::QZSS;
        } else {
            qCWarning(lcPositioning) << "Unknown satellite system" << constellationType;
            return QGeoSatelliteInfo::Undefined;
        }
    }

private:
    static jclass m_gnssStatusObject;
};

jclass ConstellationMapper::m_gnssStatusObject = nullptr;

} // anonymous namespace

namespace AndroidPositioning {
    typedef QMap<int, QGeoPositionInfoSourceAndroid * > PositionSourceMap;
    typedef QMap<int, QGeoSatelliteInfoSourceAndroid * > SatelliteSourceMap;

    Q_GLOBAL_STATIC(PositionSourceMap, idToPosSource)

    Q_GLOBAL_STATIC(SatelliteSourceMap, idToSatSource)

    int registerPositionInfoSource(QObject *obj)
    {
        int key = -1;
        if (obj->inherits("QGeoPositionInfoSource")) {
            QGeoPositionInfoSourceAndroid *src = qobject_cast<QGeoPositionInfoSourceAndroid *>(obj);
            Q_ASSERT(src);
            do {
                key = qAbs(int(QRandomGenerator::global()->generate()));
            } while (idToPosSource()->contains(key));

            idToPosSource()->insert(key, src);
        } else if (obj->inherits("QGeoSatelliteInfoSource")) {
            QGeoSatelliteInfoSourceAndroid *src = qobject_cast<QGeoSatelliteInfoSourceAndroid *>(obj);
            Q_ASSERT(src);
            do {
                key = qAbs(int(QRandomGenerator::global()->generate()));
            } while (idToSatSource()->contains(key));

            idToSatSource()->insert(key, src);
        }

        return key;
    }

    void unregisterPositionInfoSource(int key)
    {
        idToPosSource()->remove(key);
        idToSatSource()->remove(key);
    }

    enum PositionProvider
    {
        PROVIDER_GPS = 0,
        PROVIDER_NETWORK = 1,
        PROVIDER_PASSIVE = 2
    };


    QGeoPositionInfoSource::PositioningMethods availableProviders()
    {
        QGeoPositionInfoSource::PositioningMethods ret = QGeoPositionInfoSource::NoPositioningMethods;
        QJniEnvironment env;
        if (!env.jniEnv())
            return ret;
        QJniObject jniProvidersObj =
                QJniObject::callStaticObjectMethod(positioningClass(), providerListMethodId);
        jintArray jProviders = jniProvidersObj.object<jintArray>();
        jint *providers = env->GetIntArrayElements(jProviders, nullptr);
        const int size = env->GetArrayLength(jProviders);
        for (int i = 0; i < size; i++) {
            switch (providers[i]) {
            case PROVIDER_GPS:
                ret |= QGeoPositionInfoSource::SatellitePositioningMethods;
                break;
            case PROVIDER_NETWORK:
                ret |= QGeoPositionInfoSource::NonSatellitePositioningMethods;
                break;
            case PROVIDER_PASSIVE:
                //we ignore as Qt doesn't have interface for it right now
                break;
            default:
                __android_log_print(ANDROID_LOG_INFO, logTag, "Unknown positioningMethod");
            }
        }

        env->ReleaseIntArrayElements(jProviders, providers, 0);

        return ret;
    }

    QGeoPositionInfo positionInfoFromJavaLocation(const jobject &location)
    {
        QGeoPositionInfo info;

        QJniObject jniObject(location);
        if (!jniObject.isValid())
            return QGeoPositionInfo();

        const jdouble latitude = jniObject.callMethod<jdouble>("getLatitude");
        const jdouble longitude = jniObject.callMethod<jdouble>("getLongitude");

        QGeoCoordinate coordinate(latitude, longitude);

        // altitude
        jboolean attributeExists = jniObject.callMethod<jboolean>("hasAltitude");
        if (attributeExists) {
            const jdouble value = jniObject.callMethod<jdouble>("getAltitude");
            if (!qFuzzyIsNull(value))
                coordinate.setAltitude(value);
        }

        info.setCoordinate(coordinate);

        // time stamp
        const jlong timestamp = jniObject.callMethod<jlong>("getTime");
        info.setTimestamp(QDateTime::fromMSecsSinceEpoch(timestamp, Qt::UTC));

        // horizontal accuracy
        attributeExists = jniObject.callMethod<jboolean>("hasAccuracy");
        if (attributeExists) {
            const jfloat accuracy = jniObject.callMethod<jfloat>("getAccuracy");
            if (!qFuzzyIsNull(accuracy))
                info.setAttribute(QGeoPositionInfo::HorizontalAccuracy, qreal(accuracy));
        }

        // vertical accuracy
        // The check for method existence happens inside QJniObject. If the
        // method is not found, 0 (or 0.0, or false) is returned, so we do not
        // need to handle it specially.
        attributeExists = jniObject.callMethod<jboolean>("hasVerticalAccuracy");
        if (attributeExists) {
            const jfloat accuracy = jniObject.callMethod<jfloat>("getVerticalAccuracyMeters");
            if (!qFuzzyIsNull(accuracy))
                info.setAttribute(QGeoPositionInfo::VerticalAccuracy, qreal(accuracy));
        }

        // ground speed
        attributeExists = jniObject.callMethod<jboolean>("hasSpeed");
        if (attributeExists) {
            const jfloat speed = jniObject.callMethod<jfloat>("getSpeed");
            if (!qFuzzyIsNull(speed))
                info.setAttribute(QGeoPositionInfo::GroundSpeed, qreal(speed));
        }

        // bearing
        attributeExists = jniObject.callMethod<jboolean>("hasBearing");
        if (attributeExists) {
            const jfloat bearing = jniObject.callMethod<jfloat>("getBearing");
            if (!qFuzzyIsNull(bearing))
                info.setAttribute(QGeoPositionInfo::Direction, qreal(bearing));
        }

        return info;
    }

    QList<QGeoSatelliteInfo> satelliteInfoFromJavaLocation(JNIEnv *jniEnv,
                                                           jobjectArray satellites,
                                                           QList<QGeoSatelliteInfo>* usedInFix)
    {
        QList<QGeoSatelliteInfo> sats;
        jsize length = jniEnv->GetArrayLength(satellites);
        for (int i = 0; i<length; i++) {
            jobject element = jniEnv->GetObjectArrayElement(satellites, i);
            if (QJniEnvironment::checkAndClearExceptions(jniEnv)) {
                qCWarning(lcPositioning) << "Cannot process all satellite data due to exception.";
                break;
            }

            QJniObject jniObj = QJniObject::fromLocalRef(element);
            if (!jniObj.isValid())
                continue;

            QGeoSatelliteInfo info;

            // signal strength
            const jfloat snr = jniObj.callMethod<jfloat>("getSnr");
            info.setSignalStrength(int(snr));

            // ignore any satellite with no signal whatsoever
            if (qFuzzyIsNull(snr))
                continue;

            // prn
            const jint prn = jniObj.callMethod<jint>("getPrn");
            info.setSatelliteIdentifier(prn);

            if (prn >= 1 && prn <= 32)
                info.setSatelliteSystem(QGeoSatelliteInfo::GPS);
            else if (prn >= 65 && prn <= 96)
                info.setSatelliteSystem(QGeoSatelliteInfo::GLONASS);
            else if (prn >= 193 && prn <= 200)
                info.setSatelliteSystem(QGeoSatelliteInfo::QZSS);
            else if ((prn >= 201 && prn <= 235) || (prn >= 401 && prn <= 437))
                info.setSatelliteSystem(QGeoSatelliteInfo::BEIDOU);
            else if (prn >= 301 && prn <= 336)
                info.setSatelliteSystem(QGeoSatelliteInfo::GALILEO);

            // azimuth
            const jfloat azimuth = jniObj.callMethod<jfloat>("getAzimuth");
            info.setAttribute(QGeoSatelliteInfo::Azimuth, qreal(azimuth));

            // elevation
            const jfloat elevation = jniObj.callMethod<jfloat>("getElevation");
            info.setAttribute(QGeoSatelliteInfo::Elevation, qreal(elevation));

            // Used in fix - true if this satellite is actually used in
            // determining the position.
            const jboolean inFix = jniObj.callMethod<jboolean>("usedInFix");

            sats.append(info);

            if (inFix)
                usedInFix->append(info);
        }

        return sats;
    }

    QList<QGeoSatelliteInfo> satelliteInfoFromJavaGnssStatus(jobject gnssStatus,
                                                             QList<QGeoSatelliteInfo>* usedInFix)
    {
        QJniObject jniStatus(gnssStatus);
        QList<QGeoSatelliteInfo> sats;

        const int satellitesCount = jniStatus.callMethod<jint>("getSatelliteCount");
        for (int i = 0; i < satellitesCount; ++i) {
            QGeoSatelliteInfo info;

            // signal strength - this is actually a carrier-to-noise density,
            // but the values are very close to what was previously returned by
            // getSnr() method of the GpsSatellite API.
            const jfloat cn0 = jniStatus.callMethod<jfloat>("getCn0DbHz", "(I)F", i);
            info.setSignalStrength(static_cast<int>(cn0));

            // satellite system
            const jint constellationType =
                    jniStatus.callMethod<jint>("getConstellationType", "(I)I", i);
            info.setSatelliteSystem(ConstellationMapper::toSatelliteSystem(constellationType));

            // satellite identifier
            const jint svId = jniStatus.callMethod<jint>("getSvid", "(I)I", i);
            info.setSatelliteIdentifier(svId);

            // azimuth
            const jfloat azimuth = jniStatus.callMethod<jfloat>("getAzimuthDegrees", "(I)F", i);
            info.setAttribute(QGeoSatelliteInfo::Azimuth, static_cast<qreal>(azimuth));

            // elevation
            const jfloat elevation = jniStatus.callMethod<jfloat>("getElevationDegrees", "(I)F", i);
            info.setAttribute(QGeoSatelliteInfo::Elevation, static_cast<qreal>(elevation));

            // Used in fix - true if this satellite is actually used in
            // determining the position.
            const jboolean inFix = jniStatus.callMethod<jboolean>("usedInFix", "(I)Z", i);

            sats.append(info);

            if (inFix)
                usedInFix->append(info);
        }

        return sats;
    }

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly)
    {
        QJniEnvironment env;
        if (!env.jniEnv())
            return QGeoPositionInfo();

        if (!requestionPositioningPermissions())
            return {};

        QJniObject locationObj = QJniObject::callStaticObjectMethod(
                positioningClass(), lastKnownPositionMethodId, fromSatellitePositioningMethodsOnly);
        jobject location = locationObj.object();
        if (location == nullptr)
            return QGeoPositionInfo();

        const QGeoPositionInfo info = positionInfoFromJavaLocation(location);

        return info;
    }

    inline int positioningMethodToInt(QGeoPositionInfoSource::PositioningMethods m)
    {
        int providerSelection = 0;
        if (m & QGeoPositionInfoSource::SatellitePositioningMethods)
            providerSelection |= 1;
        if (m & QGeoPositionInfoSource::NonSatellitePositioningMethods)
            providerSelection |= 2;

        return providerSelection;
    }

    QGeoPositionInfoSource::Error startUpdates(int androidClassKey)
    {
        QJniEnvironment env;
        if (!env.jniEnv())
            return QGeoPositionInfoSource::UnknownSourceError;

        QGeoPositionInfoSourceAndroid *source = AndroidPositioning::idToPosSource()->value(androidClassKey);

        if (source) {
            if (!requestionPositioningPermissions())
                return QGeoPositionInfoSource::AccessError;

            int errorCode = QJniObject::callStaticMethod<jint>(
                    positioningClass(), startUpdatesMethodId, androidClassKey,
                    positioningMethodToInt(source->preferredPositioningMethods()),
                    source->updateInterval());
            switch (errorCode) {
            case 0:
            case 1:
            case 2:
            case 3:
                return static_cast<QGeoPositionInfoSource::Error>(errorCode);
            default:
                break;
            }
        }

        return QGeoPositionInfoSource::UnknownSourceError;
    }

    //used for stopping regular and single updates
    void stopUpdates(int androidClassKey)
    {
        QJniObject::callStaticMethod<void>(positioningClass(), stopUpdatesMethodId,
                                           androidClassKey);
    }

    QGeoPositionInfoSource::Error requestUpdate(int androidClassKey)
    {
        QJniEnvironment env;
        if (!env.jniEnv())
            return QGeoPositionInfoSource::UnknownSourceError;

        QGeoPositionInfoSourceAndroid *source = AndroidPositioning::idToPosSource()->value(androidClassKey);

        if (source) {
            if (!requestionPositioningPermissions())
                return QGeoPositionInfoSource::AccessError;

            int errorCode = QJniObject::callStaticMethod<jint>(
                    positioningClass(), requestUpdateMethodId, androidClassKey,
                    positioningMethodToInt(source->preferredPositioningMethods()));
            switch (errorCode) {
            case 0:
            case 1:
            case 2:
            case 3:
                return static_cast<QGeoPositionInfoSource::Error>(errorCode);
            default:
                break;
            }
        }
        return QGeoPositionInfoSource::UnknownSourceError;
    }

    QGeoSatelliteInfoSource::Error startSatelliteUpdates(int androidClassKey, bool isSingleRequest, int requestTimeout)
    {
        QJniEnvironment env;
        if (!env.jniEnv())
            return QGeoSatelliteInfoSource::UnknownSourceError;

        QGeoSatelliteInfoSourceAndroid *source = AndroidPositioning::idToSatSource()->value(androidClassKey);

        if (source) {
            if (!requestionPositioningPermissions())
                return QGeoSatelliteInfoSource::AccessError;

            int interval = source->updateInterval();
            if (isSingleRequest)
                interval = requestTimeout;
            int errorCode = QJniObject::callStaticMethod<jint>(positioningClass(),
                                                               startSatelliteUpdatesMethodId,
                                                               androidClassKey, interval,
                                                               isSingleRequest);
            switch (errorCode) {
            case -1:
            case 0:
            case 1:
            case 2:
                return static_cast<QGeoSatelliteInfoSource::Error>(errorCode);
            default:
                qCWarning(lcPositioning)
                        << "startSatelliteUpdates: Unknown error code" << errorCode;
                break;
            }
        }
        return QGeoSatelliteInfoSource::UnknownSourceError;
    }

    bool requestionPositioningPermissions()
    {
        // If the code is running as a service, we can't request permissions.
        // We can only check if we have the needed permissions. Also make sure
        // to request the background location permissions.
        if (!QNativeInterface::QAndroidApplication::isActivityContext()) {
            const auto permission = QtAndroidPrivate::PreciseBackgroundLocation;
            const auto result = QtAndroidPrivate::checkPermission(permission).result();
            if (result != QtAndroidPrivate::Authorized) {
                qCWarning(lcPositioning)
                        << "Position data not available due to missing permission" << permission;
            }
            return result == QtAndroidPrivate::Authorized;
        } else {
            // Running from a normal Activity. Checking and requesting the
            // permissions if necessary.

            // Android v23+ requires runtime permission check and requests
            const auto permission = QtAndroidPrivate::PreciseLocation;
            auto checkFuture = QtAndroidPrivate::checkPermission(permission);
            if (checkFuture.result() == QtAndroidPrivate::Denied) {
                auto requestFuture = QtAndroidPrivate::requestPermission(permission);
                if (requestFuture.result() != QtAndroidPrivate::Authorized) {
                    qCWarning(lcPositioning)
                            << "Position data not available due to missing permission"
                            << permission;
                    return false;
                }
            }

            return true;
        }
    }
}

static void positionUpdated(JNIEnv *env, jobject thiz, jobject location,
                            jint androidClassKey, jboolean isSingleUpdate)
{
    Q_UNUSED(env);
    Q_UNUSED(thiz);
    QGeoPositionInfo info = AndroidPositioning::positionInfoFromJavaLocation(location);

    QGeoPositionInfoSourceAndroid *source = AndroidPositioning::idToPosSource()->value(androidClassKey);
    if (!source) {
        qCWarning(lcPositioning) << "positionUpdated: source == 0";
        return;
    }

    //we need to invoke indirectly as the Looper thread is likely to be not the same thread
    if (!isSingleUpdate)
        QMetaObject::invokeMethod(source, "processPositionUpdate", Qt::AutoConnection,
                              Q_ARG(QGeoPositionInfo, info));
    else
        QMetaObject::invokeMethod(source, "processSinglePositionUpdate", Qt::AutoConnection,
                              Q_ARG(QGeoPositionInfo, info));
}

static void locationProvidersDisabled(JNIEnv *env, jobject thiz, jint androidClassKey)
{
    Q_UNUSED(env);
    Q_UNUSED(thiz);
    QObject *source = AndroidPositioning::idToPosSource()->value(androidClassKey);
    if (!source)
        source = AndroidPositioning::idToSatSource()->value(androidClassKey);
    if (!source) {
        qCWarning(lcPositioning) << "locationProvidersDisabled: source == 0";
        return;
    }

    QMetaObject::invokeMethod(source, "locationProviderDisabled", Qt::AutoConnection);
}

static void locationProvidersChanged(JNIEnv *env, jobject thiz, jint androidClassKey)
{
    Q_UNUSED(env);
    Q_UNUSED(thiz);
    QObject *source = AndroidPositioning::idToPosSource()->value(androidClassKey);
    if (!source) {
        qCWarning(lcPositioning) << "locationProvidersChanged: source == 0";
        return;
    }

    QMetaObject::invokeMethod(source, "locationProvidersChanged", Qt::AutoConnection);
}

static void notifySatelliteInfoUpdated(const QList<QGeoSatelliteInfo> &inView,
                                       const QList<QGeoSatelliteInfo> &inUse,
                                       jint androidClassKey, jboolean isSingleUpdate)
{
    QGeoSatelliteInfoSourceAndroid *source = AndroidPositioning::idToSatSource()->value(androidClassKey);
    if (!source) {
        qCWarning(lcPositioning) << "notifySatelliteInfoUpdated: source == 0";
        return;
    }

    QMetaObject::invokeMethod(source, "processSatelliteUpdateInView", Qt::AutoConnection,
                              Q_ARG(QList<QGeoSatelliteInfo>, inView), Q_ARG(bool, isSingleUpdate));

    QMetaObject::invokeMethod(source, "processSatelliteUpdateInUse", Qt::AutoConnection,
                              Q_ARG(QList<QGeoSatelliteInfo>, inUse), Q_ARG(bool, isSingleUpdate));
}

static void satelliteGpsUpdated(JNIEnv *env, jobject thiz, jobjectArray satellites,
                                jint androidClassKey, jboolean isSingleUpdate)
{
    Q_UNUSED(thiz);
    QList<QGeoSatelliteInfo> inUse;
    QList<QGeoSatelliteInfo> sats =
            AndroidPositioning::satelliteInfoFromJavaLocation(env, satellites, &inUse);

    notifySatelliteInfoUpdated(sats, inUse, androidClassKey, isSingleUpdate);
}

static void satelliteGnssUpdated(JNIEnv *env, jobject thiz, jobject gnssStatus,
                                 jint androidClassKey, jboolean isSingleUpdate)
{
    Q_UNUSED(env);
    Q_UNUSED(thiz);

    QList<QGeoSatelliteInfo> inUse;
    QList<QGeoSatelliteInfo> sats =
            AndroidPositioning::satelliteInfoFromJavaGnssStatus(gnssStatus, &inUse);

    notifySatelliteInfoUpdated(sats, inUse, androidClassKey, isSingleUpdate);
}

#define GET_AND_CHECK_STATIC_METHOD(VAR, CLASS, METHOD_NAME, METHOD_SIGNATURE)                     \
    VAR = env.findStaticMethod(CLASS, METHOD_NAME, METHOD_SIGNATURE);                              \
    if (!VAR) {                                                                                    \
        __android_log_print(ANDROID_LOG_FATAL, logTag, methodErrorMsg, METHOD_NAME,                \
                            METHOD_SIGNATURE);                                                     \
        return false;                                                                              \
    }

static bool registerNatives()
{
     const JNINativeMethod methods[] = {
        {"positionUpdated", "(Landroid/location/Location;IZ)V", (void *)positionUpdated},
        {"locationProvidersDisabled", "(I)V", (void *) locationProvidersDisabled},
        {"satelliteGpsUpdated", "([Landroid/location/GpsSatellite;IZ)V", (void *)satelliteGpsUpdated},
        {"locationProvidersChanged", "(I)V", (void *) locationProvidersChanged},
        {"satelliteGnssUpdated", "(Landroid/location/GnssStatus;IZ)V", (void *)satelliteGnssUpdated}
    };

    QJniEnvironment env;
    if (!env.jniEnv()) {
        __android_log_print(ANDROID_LOG_FATAL, logTag, "Failed to create environment");
        return false;
    }

    if (!positioningClass.init("org/qtproject/qt/android/positioning/QtPositioning")) {
        __android_log_print(ANDROID_LOG_FATAL, logTag, "Failed to create global class ref");
        return false;
    }

    if (!env.registerNativeMethods(positioningClass(), methods,
                                   sizeof(methods) / sizeof(methods[0]))) {
        __android_log_print(ANDROID_LOG_FATAL, logTag, "Failed to register native methods");
        return false;
    }

    GET_AND_CHECK_STATIC_METHOD(providerListMethodId, positioningClass(), "providerList", "()[I");
    GET_AND_CHECK_STATIC_METHOD(lastKnownPositionMethodId, positioningClass(), "lastKnownPosition",
                                "(Z)Landroid/location/Location;");
    GET_AND_CHECK_STATIC_METHOD(startUpdatesMethodId, positioningClass(), "startUpdates", "(III)I");
    GET_AND_CHECK_STATIC_METHOD(stopUpdatesMethodId, positioningClass(), "stopUpdates", "(I)V");
    GET_AND_CHECK_STATIC_METHOD(requestUpdateMethodId, positioningClass(), "requestUpdate",
                                "(II)I");
    GET_AND_CHECK_STATIC_METHOD(startSatelliteUpdatesMethodId, positioningClass(),
                                "startSatelliteUpdates", "(IIZ)I");

    return true;
}

Q_DECL_EXPORT jint JNICALL JNI_OnLoad(JavaVM * /*vm*/, void * /*reserved*/)
{
    static bool initialized = false;
    if (initialized)
        return JNI_VERSION_1_6;
    initialized = true;

    __android_log_print(ANDROID_LOG_INFO, logTag, "Positioning start");

    if (!registerNatives()) {
        __android_log_print(ANDROID_LOG_FATAL, logTag, "registerNatives() failed");
        return -1;
    }

    if (!ConstellationMapper::init()) {
        __android_log_print(ANDROID_LOG_ERROR, logTag,
                            "Failed to extract constellation type constants. "
                            "Satellite system will be undefined!");
    }

    return JNI_VERSION_1_6;
}

