#ifndef QDECLARATIVESEARCHRESULT_P_H
#define QDECLARATIVESEARCHRESULT_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qplacesearchresult.h>
#include "qdeclarativeplace_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeSearchResult : public QObject
{
    Q_OBJECT

    Q_ENUMS(LocationMatchType);
    Q_ENUMS(SearchResultType);

    Q_PROPERTY(QVariantHash additionalData READ additionalData WRITE setAdditionalData NOTIFY additionalDataChanged);
    Q_PROPERTY(qreal relevance READ relevance WRITE setRelevance NOTIFY relevanceChanged);
    Q_PROPERTY(qreal distance READ distance WRITE setDistance NOTIFY distanceChanged);
    Q_PROPERTY(qreal heading READ heading WRITE setHeading NOTIFY headingChanged);
    Q_PROPERTY(LocationMatchType matchType READ matchType WRITE setMatchType NOTIFY matchTypeChanged);
    Q_PROPERTY(SearchResultType type READ type WRITE setType NOTIFY typeChanged);
    Q_PROPERTY(QDeclarativePlace* place READ place WRITE setPlace NOTIFY placeChanged);
    Q_PROPERTY(QString didYouMeanSuggestion READ didYouMeanSuggestion WRITE setDidYouMeanSuggestion NOTIFY didYouMeanSuggestionChanged);

public:
    enum LocationMatchType {
        PointAddress,
        Interpolated,
        Undefined
    };

    enum SearchResultType {
        Place,
        DidYouMeanSuggestion,
        Unassigned
    };

    explicit QDeclarativeSearchResult(QObject* parent = 0);
    explicit QDeclarativeSearchResult(const QPlaceSearchResult &src, QObject* parent = 0);
    ~QDeclarativeSearchResult();

    QPlaceSearchResult searchResult() const;
    void setSearchResult(const QPlaceSearchResult &src);

    qreal relevance() const;
    void setRelevance(const qreal &relevance);
    qreal distance() const;
    void setDistance(const qreal &distance);
    qreal heading() const;
    void setHeading(const qreal &heading);
    LocationMatchType matchType() const;
    void setMatchType(const LocationMatchType &matchType);
    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    SearchResultType type() const;
    void setType(const SearchResultType &type);
    QDeclarativePlace *place();
    void setPlace(QDeclarativePlace *place);
    QString didYouMeanSuggestion() const;
    void setDidYouMeanSuggestion(const QString &didYouMeanSuggestion);

signals:
    void relevanceChanged();
    void distanceChanged();
    void headingChanged();
    void matchTypeChanged();
    void typeChanged();
    void placeChanged();
    void additionalDataChanged();
    void didYouMeanSuggestionChanged();

private:
    QDeclarativePlace m_place;
    QPlaceSearchResult m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeSearchResult));

#endif // QDECLARATIVESEARCHRESULT_P_H
