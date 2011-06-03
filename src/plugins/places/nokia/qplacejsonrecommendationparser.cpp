#include "qplacejsonrecommendationparser.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

#include "qplacejsondetailsparser.h"
#include <qplace.h>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

static const char *recommendations_element = "recommendations";
static const char *recommendations_nearby_element = "nearby";
static const char *recommendations_distance_element = "distance";
static const char *recommendations_place_element = "place";

QTM_USE_NAMESPACE

QPlaceJSonRecommendationParser::QPlaceJSonRecommendationParser(QObject *parent) :
    QObject(parent),
    engine(NULL)
{
}

QPlaceJSonRecommendationParser::~QPlaceJSonRecommendationParser()
{
}

QList<QPlaceSearchResult> QPlaceJSonRecommendationParser::results()
{
    return searchResults;
}

void QPlaceJSonRecommendationParser::processData(const QString &data)
{
    if (!engine) {
        engine = new QScriptEngine(this);
    }
    searchResults.clear();

    QScriptValue sv = engine->evaluate("(" + data + ")");
    if (sv.isValid()) {
        sv = sv.property(recommendations_element);
        if (sv.isValid()) {
            QScriptValueIterator it(sv.property(recommendations_nearby_element));
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    processResultElement(it.value());
                }
            }
            emit finished(NoError, QString());
            return;
        }
    }
    emit finished(ParsingError, QString("JSON data are invalid"));
}

void QPlaceJSonRecommendationParser::processResultElement(const QScriptValue &value)
{
    QPlaceSearchResult result;
    result.setType(QPlaceSearchResult::Place);

    // Processing properties
    QScriptValue distance = value.property(recommendations_distance_element);
    if (distance.isValid() && !distance.toString().isEmpty()) {
        bool isConverted;
        double distanceValue = distance.toString().toDouble(&isConverted);
        if (isConverted) {
            result.setDistance(distanceValue);
        }
    }
    QScriptValue place = value.property(recommendations_place_element);
    if (place.isValid()) {
        QPlace newPlace = QPlaceJSonDetailsParser::buildPlace(place);
        result.setPlace(newPlace);
        searchResults.append(result);
    }
}
