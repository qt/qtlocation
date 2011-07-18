#include "qdeclarativesearchresult_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass SearchResult

    \brief The SearchResult element holds search result data.
    \inherits QObject

    SearchResult cointains many properties holding data like distance,
    place, etc.

    \ingroup qml-places
*/
QDeclarativeSearchResult::QDeclarativeSearchResult(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeSearchResult::QDeclarativeSearchResult(const QPlaceSearchResult &src,
        QObject *parent)
        : QObject(parent),
          m_place(src.place()),
          m_src(src)
{
}

QDeclarativeSearchResult::~QDeclarativeSearchResult()
{
}

void QDeclarativeSearchResult::setSearchResult(const QPlaceSearchResult &src)
{
    QPlaceSearchResult previous = m_src;
    m_src = src;

    if (previous.distance() != m_src.distance()) {
        emit distanceChanged();
    }
    if (previous.heading() != m_src.heading()) {
        emit headingChanged();
    }
    if (previous.relevance() != m_src.relevance()) {
        emit relevanceChanged();
    }
    if (previous.additionalData() != m_src.additionalData()) {
        emit additionalDataChanged();
    }
    if (previous.place() != m_src.place()) {
        m_place.setPlace(m_src.place());
        emit placeChanged();
    }
    if (previous.didYouMeanSuggestion() != m_src.didYouMeanSuggestion()) {
        emit didYouMeanSuggestionChanged();
    }
    if (previous.type() != m_src.type()) {
        emit typeChanged();
    }
    if (previous.matchType() != m_src.matchType()) {
        emit matchTypeChanged();
    }
}

QPlaceSearchResult QDeclarativeSearchResult::searchResult()
{
    m_place.setPlace(m_place.place());
    return m_src;
}

/*!
    \qmlproperty QVariantHash SearchResult::additionalData

    This property holds additional data for search result. Those are pairs of strings (key/value).
*/
void QDeclarativeSearchResult::setAdditionalData(const QVariantHash &additionalData)
{
    if (m_src.additionalData() != additionalData) {
        m_src.setAdditionalData(additionalData);
        emit additionalDataChanged();
    }
}

QVariantHash QDeclarativeSearchResult::additionalData() const
{
    return m_src.additionalData();
}

/*!
    \qmlproperty greal SearchResult::heading

    This property holds heading.
*/
void QDeclarativeSearchResult::setHeading(const qreal &data)
{
    if (m_src.heading() != data) {
        m_src.setHeading(data);
        emit headingChanged();
    }
}

qreal QDeclarativeSearchResult::heading() const
{
    return m_src.heading();
}

/*!
    \qmlproperty greal SearchResult::distance

    This property holds distance.
*/
void QDeclarativeSearchResult::setDistance(const qreal &data)
{
    if (m_src.distance() != data) {
        m_src.setDistance(data);
        emit distanceChanged();
    }
}

qreal QDeclarativeSearchResult::distance() const
{
    return m_src.distance();
}

/*!
    \qmlproperty greal SearchResult::relevance

    This property holds relevance.
*/
void QDeclarativeSearchResult::setRelevance(const qreal &data)
{
    if (m_src.relevance() != data) {
        m_src.setRelevance(data);
        emit relevanceChanged();
    }
}

qreal QDeclarativeSearchResult::relevance() const
{
    return m_src.relevance();
}

/*!
    \qmlproperty QDeclarativeSearchResult::LocationMatchType SearchResult::matchType

    This property holds mime type.
*/
void QDeclarativeSearchResult::setMatchType(const QDeclarativeSearchResult::LocationMatchType &matchType)
{
    if (m_src.matchType() != (QPlaceSearchResult::LocationMatchType)matchType) {
        m_src.setMatchType((QPlaceSearchResult::LocationMatchType)matchType);
        emit matchTypeChanged();
    }
}

QDeclarativeSearchResult::LocationMatchType QDeclarativeSearchResult::matchType() const
{
    return (QDeclarativeSearchResult::LocationMatchType)m_src.matchType();
}

/*!
    \qmlproperty QDeclarativeSearchResult::SearchResultType SearchResult::type

    This property holds type of result.
*/
void QDeclarativeSearchResult::setType(const QDeclarativeSearchResult::SearchResultType &type)
{
    if (m_src.type() != (QPlaceSearchResult::SearchResultType)type) {
        m_src.setType((QPlaceSearchResult::SearchResultType)type);
        emit typeChanged();
    }
}

QDeclarativeSearchResult::SearchResultType QDeclarativeSearchResult::type() const
{
    return (QDeclarativeSearchResult::SearchResultType)m_src.type();
}

/*!
    \qmlproperty string SearchResult::didYouMeanSuggestion

    This property holds "did you mean" suggestion.
*/
void QDeclarativeSearchResult::setDidYouMeanSuggestion(const QString &didYouMeanSuggestion)
{
    if (m_src.didYouMeanSuggestion() != didYouMeanSuggestion) {
        m_src.setDidYouMeanSuggestion(didYouMeanSuggestion);
        emit didYouMeanSuggestionChanged();
    }
}

QString QDeclarativeSearchResult::didYouMeanSuggestion() const
{
    return m_src.didYouMeanSuggestion();
}

/*!
    \qmlproperty Place SearchResult::place

    This property holds place result.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativeSearchResult::setPlace(QDeclarativePlace *place)
{
    if (m_src.place() != place->place()) {
        m_src.setPlace(place->place());
        m_place.setPlace(place->place());
        emit placeChanged();
    }
}

QDeclarativePlace *QDeclarativeSearchResult::place()
{
    return &m_place;
}
