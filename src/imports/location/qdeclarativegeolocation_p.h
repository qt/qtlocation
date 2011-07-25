#ifndef QDECLARATIVEGEOLOCATION_P_H
#define QDECLARATIVEGEOLOCATION_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qgeolocation.h>
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativegeoaddress_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGeoLocation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantHash additionalData READ additionalData WRITE setAdditionalData NOTIFY additionalDataChanged);
    Q_PROPERTY(QDeclarativeGeoAddress* address READ address WRITE setAddress NOTIFY addressChanged);
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCoordinate> navigationPositions READ navigationPositions NOTIFY navigationPositionsChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged);
    Q_PROPERTY(QString locationId READ locationId WRITE setLocationId NOTIFY locationIdChanged);
    Q_PROPERTY(QDeclarativeGeoBoundingBox* viewport READ viewport WRITE setViewport NOTIFY viewportChanged);

public:
    explicit QDeclarativeGeoLocation(QObject* parent = 0);
    explicit QDeclarativeGeoLocation(const QGeoLocation &src, QObject* parent = 0);
    ~QDeclarativeGeoLocation();

    QGeoLocation location() ;
    void setLocation(const QGeoLocation &src);

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QDeclarativeGeoAddress *address();
    void setAddress(QDeclarativeGeoAddress *address);
    QDeclarativeCoordinate *coordinate();
    void setCoordinate(QDeclarativeCoordinate *coordinate);
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
    QDeclarativeGeoBoundingBox *viewport();
    void setViewport(QDeclarativeGeoBoundingBox *boundingBox);

signals:
    void additionalDataChanged();
    void addressChanged();
    void coordinateChanged();
    void navigationPositionsChanged();
    void labelChanged();
    void locationIdChanged();
    void viewportChanged();

private:
    void synchronizeNavigationPositions();

private:
    QDeclarativeGeoAddress m_address;
    QDeclarativeCoordinate m_coordinate;
    QDeclarativeGeoBoundingBox m_boundingBox;
    QList<QDeclarativeCoordinate*> m_navigationPositions;
    QGeoLocation m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoLocation));

#endif // QDECLARATIVELOCATION_P_H
