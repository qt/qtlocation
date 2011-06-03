#ifndef QPLACEJSONDETAILSPARSER_H
#define QPLACEJSONDETAILSPARSER_H

#include <QObject>
#include <QList>

#include "../qplace.h"

class QScriptEngine;
class QScriptValue;

QTM_BEGIN_NAMESPACE

class QPlaceJSonDetailsParser : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        ParsingError
    };

    explicit QPlaceJSonDetailsParser(QObject *parent = 0);
    virtual ~QPlaceJSonDetailsParser();

    static QPlace buildPlace(const QScriptValue &place);
    QPlace result();

signals:
    void finished(const Error &error, const QString &errorMessage);

public slots:
    void processData(const QString &data);

private:
    static void buildPlace(const QScriptValue &place, QPlace *targetPlace);
    static void processMainProvider(const QScriptValue &place, QPlace *targetPlace);
    static void processContacts(const QScriptValue &contacts, QPlace *targetPlace);
    static void processCategories(const QScriptValue &categories, QPlace *targetPlace);
    static QPlaceCategory processCategory(const QScriptValue &category);
    static void processRatings(const QScriptValue &ratings, QPlace *targetPlace);
    static QPlaceRating *processRating(const QScriptValue &rating);
    static void processAddress(const QScriptValue &address, QPlaceLocation *location);
    static void processLocations(const QScriptValue &locations, QPlace *targetPlace);
    static QPlaceLocation processLocation(const QScriptValue &location);
    static void processTags(const QScriptValue &locations, QPlace *targetPlace);
    static void processNames(const QScriptValue &locations, QPlace *targetPlace);
    static QPlaceAlternativeValue *processName(const QScriptValue &nameValue);
    static void processPremiumContents(const QScriptValue &content, QPlace *targetPlace);
    static void processPremiumVersion(const QScriptValue &content, QPlace *targetPlace);
    static void processPremiumContent(const QScriptValue &content, QPlace *targetPlace);
    static void processPremiumContentDescription(const QScriptValue &content,
                                                 const QPlaceSupplier &supplier,
                                                 QPlace *targetPlace);
    static void processPremiumContentMediaObjects(const QScriptValue &content,
                                                  const QPlaceSupplier &supplier,
                                                  QPlace *targetPlace);
    static QPlaceMediaObject *processPremiumContentMediaObject(const QScriptValue &content);
    static void processAdContent(const QScriptValue &content, QPlace *targetPlace);
    static void processAdContentPackages(const QScriptValue &content, QPlace *targetPlace);
    static void processAdContentDescriptions(const QScriptValue &content, QPlace *targetPlace);
    static QPlaceDescription *processAdContentDescription(const QScriptValue &content);
    static void processAdContentMediaObjects(const QScriptValue &content, QPlace *targetPlace);
    static QPlaceMediaObject *processAdContentMediaObject(const QScriptValue &content);
    static void processAdContentPaymentMethods(const QScriptValue &content, QPlace *targetPlace);
    static QString processAdContentPaymentMethod(const QScriptValue &content);
    static void processAdContentBusinessHours(const QScriptValue &content, QPlace *targetPlace);
    static void processAdContentClosingsNotes(const QScriptValue &content, QPlace *targetPlace);
    static QString processAdContentClosingsNote(const QScriptValue &content);
    static void processAdContentOpeningHours(const QScriptValue &content, QPlace *targetPlace);
    static QPlaceWeekdayHours *processAdContentOpeningHoursElement(const QScriptValue &content);
    static void processAdContentOpeningNotes(const QScriptValue &content, QPlace *targetPlace);
    static QString processAdContentOpeningNote(const QScriptValue &content);

private:
    QScriptEngine *engine;
    QPlace place;
};

QTM_END_NAMESPACE

#endif // QPLACEJSONDETAILSPARSER_H
