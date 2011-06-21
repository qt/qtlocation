#ifndef QDECLARATIVEALTERNATIVEVALUE_P_H
#define QDECLARATIVEALTERNATIVEVALUE_P_H

#include <qplacealternativevalue.h>
#include <QtDeclarative/qdeclarative.h>

#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeAlternativeValue : public QObject
{
    Q_OBJECT

    Q_ENUMS(TextSemantics)
    Q_ENUMS(NameType)

    Q_PROPERTY(QString key READ key WRITE setKey NOTIFY keyChanged);
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged);
    Q_PROPERTY(TextSemantics semantics READ semantics WRITE setSemantics NOTIFY semanticsChanged);
    Q_PROPERTY(NameType type READ type WRITE setType NOTIFY typeChanged);
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged);

public:
    enum TextSemantics {
        Synonim,
        Exonym,
        Unclassified
    };

    enum NameType {
        BaseName,
        ShortBaseName,
        Abbrevation,
        Unknown
    };

    explicit QDeclarativeAlternativeValue(QObject* parent = 0);
    explicit QDeclarativeAlternativeValue(const QPlaceAlternativeValue &value, QObject* parent = 0);
    ~QDeclarativeAlternativeValue();

    QPlaceAlternativeValue valueObject() const;
    void setValueObject(const QPlaceAlternativeValue &value);

    QString key() const;
    void setKey(const QString &key);

    QString language() const;
    void setLanguage(const QString &language);

    TextSemantics semantics() const;
    void setSemantics(const TextSemantics &semantics);

    NameType type() const;
    void setType(const NameType &type);

    QString value() const;
    void setValue(const QString &value);


signals:
    void keyChanged();
    void languageChanged();
    void semanticsChanged();
    void typeChanged();
    void valueChanged();

private:
    QPlaceAlternativeValue m_value;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeAlternativeValue));

#endif // QDECLARATIVEALTERNATIVEVALUE_P_H
