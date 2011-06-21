#ifndef QDECLARATIVELOCATION_P_H
#define QDECLARATIVELOCATION_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qplacelocation.h>
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativeaddress_p.h"
#include "qdeclarativealternativevalue_p.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeLocation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantHash additionalData READ additionalData WRITE setAdditionalData NOTIFY additionalDataChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeAlternativeValue> alternativeLabels READ alternativeLabels NOTIFY alternativeLabelsChanged)
    Q_PROPERTY(QDeclarativeAddress* address READ address WRITE setAddress NOTIFY addressChanged);
    Q_PROPERTY(QDeclarativeCoordinate* displayPosition READ displayPosition WRITE setDisplayPosition NOTIFY displayPositionChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCoordinate> navigationPositions READ navigationPositions NOTIFY navigationPositionsChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged);
    Q_PROPERTY(QString locationId READ locationId WRITE setLocationId NOTIFY locationIdChanged);
    Q_PROPERTY(int locationScore READ locationScore WRITE setLocationScore NOTIFY locationScoreChanged);
    Q_PROPERTY(QDeclarativeGeoBoundingBox* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged);

public:
    explicit QDeclarativeLocation(QObject* parent = 0);
    explicit QDeclarativeLocation(const QPlaceLocation &src, QObject* parent = 0);
    ~QDeclarativeLocation();

    QPlaceLocation location() const;
    void setLocation(const QPlaceLocation &src);

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QDeclarativeListProperty<QDeclarativeAlternativeValue> alternativeLabels();
    static void alternativeValue_append(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                  QDeclarativeAlternativeValue* value);
    static int alternativeValue_count(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop);
    static QDeclarativeAlternativeValue* alternativeValue_at(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop, int index);
    static void alternativeValue_clear(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop);
    QDeclarativeAddress *address();
    void setAddress(QDeclarativeAddress *address);
    QDeclarativeCoordinate *displayPosition();
    void setDisplayPosition(QDeclarativeCoordinate *position);
    QDeclarativeListProperty<QDeclarativeCoordinate> navigationPositions();
    static void navigationPosition_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop,
                                  QDeclarativeCoordinate* value);
    static int navigationPosition_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    static QDeclarativeCoordinate* navigationPosition_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index);
    static void navigationPosition_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    QString label() const;
    void setLabel(const QString &label);
    QString locationId() const;
    void setLocationId(const QString &locationId);
    int locationScore() const;
    void setLocationScore(const int &score);
    QDeclarativeGeoBoundingBox *mapView();
    void setMapView(QDeclarativeGeoBoundingBox *boundingBox);

signals:
    void additionalDataChanged();
    void alternativeLabelsChanged();
    void addressChanged();
    void displayPositionChanged();
    void navigationPositionsChanged();
    void labelChanged();
    void locationIdChanged();
    void locationScoreChanged();
    void mapViewChanged();

private:
    void synchronizeAlternativeValues();
    void synchronizeNavigationPositions();

private:
    QList<QDeclarativeAlternativeValue*> m_alternativeValues;
    QDeclarativeAddress m_address;
    QDeclarativeCoordinate m_displayPosition;
    QDeclarativeGeoBoundingBox m_boundingBox;
    QList<QDeclarativeCoordinate*> m_navigationPositions;
    QPlaceLocation m_src;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLocation));

#endif // QDECLARATIVELOCATION_P_H
