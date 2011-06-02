#include "qplacejsonreviewparser.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

#include "../qplacereview.h"
#include "../qplacesupplier.h"
#include "qplacesuppliersrepository.h"

#if defined(QT_PLACES_LIBRARY)
    #include <QDebug>
#endif

static const char *reviews_element = "reviews";
static const char *review_element = "review";
static const char *review_count = "totalNumberOfReviews";
static const char *review_id_element = "a_id";
static const char *review_date_element = "creationDate";
static const char *review_description_element = "description";
static const char *review_minus_element = "minusCount";
static const char *review_originator_element = "originatorUrl";
static const char *review_plus_element = "plusCount";
static const char *review_rating_element = "rating";
static const char *review_title_element = "title";
static const char *review_username_element = "user";
static const char *review_userid_element = "uuid";
static const char *review_vendor_element = "vendor";
static const char *review_vendorname_element = "vendorDisplayName";
static const char *review_vendoricon_element = "vendorIconUrl";

using namespace QT_PLACES_NAMESPACE;

QPlaceJSonReviewParser::QPlaceJSonReviewParser(QObject *parent) :
    QObject(parent),
    engine(NULL),
    allReviews(0)
{
}

QPlaceJSonReviewParser::~QPlaceJSonReviewParser()
{
}

QList<QPlaceReview> QPlaceJSonReviewParser::results()
{
    return reviews;
}

uint QPlaceJSonReviewParser::allReviewsCount()
{
    return allReviews;
}

QPlaceReview QPlaceJSonReviewParser::buildReview(const QScriptValue &review)
{
    QPlaceReview newReview;
    QScriptValue value = review.property(review_id_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setReviewId(value.toString());
    }
    value = review.property(review_date_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setDate(value.toString());
    }
    value = review.property(review_minus_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        bool isConverted;
        uint number = value.toString().toUInt(&isConverted);
        if (isConverted) {
            newReview.setNotHelpfulVotings(number);
        }
    }
    value = review.property(review_plus_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        bool isConverted;
        uint number = value.toString().toUInt(&isConverted);
        if (isConverted) {
            newReview.setHelpfulVotings(number);
        }
    }
    value = review.property(review_originator_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setOriginatorURL(value.toString());
    }
    value = review.property(review_description_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setDescription(value.toString());
    }
    value = review.property(review_title_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setTitle(value.toString());
    }
    value = review.property(review_username_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setUserName(value.toString());
    }
    value = review.property(review_userid_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setUserId(value.toString());
    }

    QString name, id, icon;
    value = review.property(review_vendorname_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        name = value.toString();
    }
    value = review.property(review_vendor_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        id = value.toString();
    }
    value = review.property(review_vendoricon_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        icon = value.toString();
    }
    if (!name.isEmpty() || !id.isEmpty()) {
        QPlaceSupplier sup;
        sup.setName(name);
        sup.setSupplierId(id);
        sup.setSupplierIconURL(icon);
        newReview.setSupplier(QPlaceSuppliersRepository::instance()->addSupplier(sup));
    }

    value = review.property(review_rating_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        bool isConverted;
        double number = value.toString().toDouble(&isConverted);
        if (isConverted) {
            newReview.setRating(number);
        }
    }
    return newReview;
}

void QPlaceJSonReviewParser::processData(const QString &data)
{
    if (!engine) {
        engine = new QScriptEngine(this);
    }
    reviews.clear();

    QScriptValue sv = engine->evaluate("(" + data + ")");
    if (sv.isValid()) {
        sv = sv.property(reviews_element);
        if (sv.isValid()) {
            processReviews(sv);
            emit finished(NoError, QString());
        } else {
            emit finished(ParsingError, QString("JSON data are invalid"));
        }
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}

void QPlaceJSonReviewParser::processReviews(const QScriptValue &reviewsElement)
{
    QScriptValue value = reviewsElement.property(review_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    reviews.append(buildReview(it.value()));
                }
            }
        } else {
            reviews.append(buildReview(value));
        }
    }
    value = reviewsElement.property(review_count);
    if (value.isValid()) {
        allReviews = value.toUInt32();
    }
}
