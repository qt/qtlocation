/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDateTime>
#include <QMap>
#include <QtGlobal>
#include <android/log.h>
#include <jni.h>
#include <QGeoPositionInfo>
#include "qgeopositioninfosource_android_p.h"

#include "jnipositioning.h"

static JavaVM *javaVM = 0;
jclass positioningClass;

static jmethodID providerListMethodId;
static jmethodID lastKnownPositionMethodId;
static jmethodID startUpdatesMethodId;
static jmethodID stopUpdatesMethodId;
static jmethodID requestUpdateMethodId;

static const char logTag[] = "QtPositioning";
static const char classErrorMsg[] = "Can't find class \"%s\"";
static const char methodErrorMsg[] = "Can't find method \"%s%s\"";

namespace AndroidPositioning {
typedef QMap<int, QGeoPositionInfoSourceAndroid * > PositionSourceMap;
Q_GLOBAL_STATIC(PositionSourceMap, idToSource)

    struct AttachedJNIEnv
    {
        AttachedJNIEnv()
        {
            attached = false;
            if (javaVM->GetEnv((void**)&jniEnv, JNI_VERSION_1_6) < 0) {
                if (javaVM->AttachCurrentThread(&jniEnv, NULL) < 0) {
                    __android_log_print(ANDROID_LOG_ERROR, logTag, "AttachCurrentThread failed");
                    jniEnv = 0;
                    return;
                }
                attached = true;
            }
        }

        ~AttachedJNIEnv()
        {
            if (attached)
                javaVM->DetachCurrentThread();
        }
        bool attached;
        JNIEnv *jniEnv;
    };

    int registerPositionInfoSource(QGeoPositionInfoSourceAndroid *src)
    {
        static bool firstInit = true;
        if (firstInit) {
            qsrand( QDateTime::currentDateTime().toTime_t() );
            firstInit = false;
        }

        int key;
        do {
            key = qrand();
        } while (idToSource()->contains(key));

        idToSource()->insert(key, src);
        return key;
    }

    void unregisterPositionInfoSource(int key)
    {
        idToSource()->remove(key);
    }

    enum PositionProvider
    {
        PROVIDER_GPS = 0,
        PROVIDER_NETWORK = 1,
        PROVIDER_PASSIVE = 2
    };


    QGeoPositionInfoSource::PositioningMethods availableProviders()
    {
        QGeoPositionInfoSource::PositioningMethods ret =
                static_cast<QGeoPositionInfoSource::PositioningMethods>(0);
        AttachedJNIEnv env;
        if (!env.jniEnv)
            return ret;
        jintArray jProviders = static_cast<jintArray>(env.jniEnv->CallStaticObjectMethod(
                                                          positioningClass, providerListMethodId));
        jint *providers = env.jniEnv->GetIntArrayElements(jProviders, 0);
        const uint size = env.jniEnv->GetArrayLength(jProviders);
        for (uint i = 0; i < size; i++) {
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
        env.jniEnv->ReleaseIntArrayElements(jProviders, providers, 0);

        return ret;
    }

    QGeoPositionInfo positionInfoFromJavaLocation(JNIEnv * jniEnv, const jobject &location)
    {
        QGeoPositionInfo info;
        jclass thisClass = jniEnv->GetObjectClass(location);
        if (!thisClass)
            return QGeoPositionInfo();

        jmethodID mid = jniEnv->GetMethodID(thisClass, "getLatitude", "()D");
        jdouble latitude = jniEnv->CallDoubleMethod(location, mid);
        mid = jniEnv->GetMethodID(thisClass, "getLongitude", "()D");
        jdouble longitude = jniEnv->CallDoubleMethod(location, mid);
        QGeoCoordinate coordinate(latitude, longitude);

        //altitude
        mid = jniEnv->GetMethodID(thisClass, "hasAltitude", "()Z");
        jboolean attributeExists = jniEnv->CallBooleanMethod(location, mid);
        if (attributeExists) {
            mid = jniEnv->GetMethodID(thisClass, "getAltitude", "()D");
            jdouble value = jniEnv->CallDoubleMethod(location, mid);
            coordinate.setAltitude(value);
        }

        info.setCoordinate(coordinate);

        //time stamp
        mid = jniEnv->GetMethodID(thisClass, "getTime", "()J");
        jlong timestamp = jniEnv->CallLongMethod(location, mid);
        info.setTimestamp(QDateTime::fromMSecsSinceEpoch(timestamp));

        //accuracy
        mid = jniEnv->GetMethodID(thisClass, "hasAccuracy", "()Z");
        attributeExists = jniEnv->CallBooleanMethod(location, mid);
        if (attributeExists) {
            mid = jniEnv->GetMethodID(thisClass, "getAccuracy", "()F");
            jfloat accuracy = jniEnv->CallFloatMethod(location, mid);
            info.setAttribute(QGeoPositionInfo::HorizontalAccuracy, accuracy);
        }

        //ground speed
        mid = jniEnv->GetMethodID(thisClass, "hasSpeed", "()Z");
        attributeExists = jniEnv->CallBooleanMethod(location, mid);
        if (attributeExists) {
            mid = jniEnv->GetMethodID(thisClass, "getSpeed", "()F");
            jfloat speed = jniEnv->CallFloatMethod(location, mid);
            info.setAttribute(QGeoPositionInfo::GroundSpeed, speed);
        }

        //bearing
        mid = jniEnv->GetMethodID(thisClass, "hasBearing", "()Z");
        attributeExists = jniEnv->CallBooleanMethod(location, mid);
        if (attributeExists) {
            mid = jniEnv->GetMethodID(thisClass, "getBearing", "()F");
            jfloat bearing = jniEnv->CallFloatMethod(location, mid);
            info.setAttribute(QGeoPositionInfo::Direction, bearing);
        }

        return info;
    }

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly)
    {
        AttachedJNIEnv env;
        if (!env.jniEnv)
            return QGeoPositionInfo();

        jobject location = env.jniEnv->CallStaticObjectMethod(positioningClass,
                                                              lastKnownPositionMethodId,
                                                              fromSatellitePositioningMethodsOnly);
        if (location == 0)
            return QGeoPositionInfo();

        return positionInfoFromJavaLocation(env.jniEnv, location);
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
        AttachedJNIEnv env;
        if (!env.jniEnv)
            return QGeoPositionInfoSource::UnknownSourceError;

        QGeoPositionInfoSourceAndroid *source = AndroidPositioning::idToSource()->value(androidClassKey);

        if (source) {
            int errorCode = env.jniEnv->CallStaticIntMethod(positioningClass, startUpdatesMethodId,
                                             androidClassKey,
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
        AttachedJNIEnv env;
        if (!env.jniEnv)
            return;

        env.jniEnv->CallStaticVoidMethod(positioningClass, stopUpdatesMethodId, androidClassKey);
    }

    QGeoPositionInfoSource::Error requestUpdate(int androidClassKey)
    {
        AttachedJNIEnv env;
        if (!env.jniEnv)
            return QGeoPositionInfoSource::UnknownSourceError;

        QGeoPositionInfoSourceAndroid *source = AndroidPositioning::idToSource()->value(androidClassKey);

        if (source) {
            int errorCode = env.jniEnv->CallStaticIntMethod(positioningClass, requestUpdateMethodId,
                                             androidClassKey,
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
}


static void positionUpdated(JNIEnv *env, jobject /*thiz*/, jobject location, jint androidClassKey, jboolean isSingleUpdate)
{
    QGeoPositionInfo info = AndroidPositioning::positionInfoFromJavaLocation(env, location);

    QGeoPositionInfoSourceAndroid *source = AndroidPositioning::idToSource()->value(androidClassKey);
    if (!source) {
        qFatal("positionUpdated: source == 0");
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

static void locationProvidersDisabled(JNIEnv *env, jobject /*thiz*/, jint androidClassKey)
{
    Q_UNUSED(env);
    QGeoPositionInfoSourceAndroid *source = AndroidPositioning::idToSource()->value(androidClassKey);
    if (!source) {
        qFatal("locationProvidersDisabled: source == 0");
        return;
    }

    QMetaObject::invokeMethod(source, "locationProviderDisabled", Qt::AutoConnection);
}


#define FIND_AND_CHECK_CLASS(CLASS_NAME) \
clazz = env->FindClass(CLASS_NAME); \
if (!clazz) { \
    __android_log_print(ANDROID_LOG_FATAL, logTag, classErrorMsg, CLASS_NAME); \
    return JNI_FALSE; \
}

#define GET_AND_CHECK_STATIC_METHOD(VAR, CLASS, METHOD_NAME, METHOD_SIGNATURE) \
VAR = env->GetStaticMethodID(CLASS, METHOD_NAME, METHOD_SIGNATURE); \
if (!VAR) { \
    __android_log_print(ANDROID_LOG_FATAL, logTag, methodErrorMsg, METHOD_NAME, METHOD_SIGNATURE); \
    return JNI_FALSE; \
}

static JNINativeMethod methods[] = {
    {"positionUpdated", "(Landroid/location/Location;IZ)V", (void *)positionUpdated},
    {"locationProvidersDisabled", "(I)V", (void *) locationProvidersDisabled}
};

static bool registerNatives(JNIEnv *env)
{
    jclass clazz;
    FIND_AND_CHECK_CLASS("org/qtproject/qt5/android/positioning/QtPositioning");
    positioningClass = static_cast<jclass>(env->NewGlobalRef(clazz));

    if (env->RegisterNatives(positioningClass, methods, sizeof(methods) / sizeof(methods[0])) < 0) {
        __android_log_print(ANDROID_LOG_FATAL, logTag, "RegisterNatives failed");
        return JNI_FALSE;
    }

    GET_AND_CHECK_STATIC_METHOD(providerListMethodId, positioningClass, "providerList", "()[I");
    GET_AND_CHECK_STATIC_METHOD(lastKnownPositionMethodId, positioningClass, "lastKnownPosition", "(Z)Landroid/location/Location;");
    GET_AND_CHECK_STATIC_METHOD(startUpdatesMethodId, positioningClass, "startUpdates", "(III)I");
    GET_AND_CHECK_STATIC_METHOD(stopUpdatesMethodId, positioningClass, "stopUpdates", "(I)V");
    GET_AND_CHECK_STATIC_METHOD(requestUpdateMethodId, positioningClass, "requestUpdate", "(II)I");
    return true;
}

Q_DECL_EXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void * /*reserved*/)
{
    typedef union {
        JNIEnv *nativeEnvironment;
        void *venv;
    } UnionJNIEnvToVoid;

    __android_log_print(ANDROID_LOG_INFO, logTag, "Positioning start");
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    javaVM = 0;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        __android_log_print(ANDROID_LOG_FATAL, logTag, "GetEnv failed");
        return -1;
    }
    JNIEnv *env = uenv.nativeEnvironment;
    if (!registerNatives(env)) {
        __android_log_print(ANDROID_LOG_FATAL, logTag, "registerNatives failed");
        return -1;
    }

    javaVM = vm;
    return JNI_VERSION_1_4;
}

