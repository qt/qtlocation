#include "qplacecontent.h"
#include "qplacecontent_p.h"

#if !defined(Q_CC_MWERKS)
template<> QT_PREPEND_NAMESPACE(QPlaceContentPrivate) *QSharedDataPointer<QT_PREPEND_NAMESPACE(QPlaceContentPrivate)>::clone()
{
    return d->clone();
}
#endif

QT_USE_NAMESPACE

/* Constructs an empty content object */
QPlaceContent::QPlaceContent()
    :d_ptr(0)
{
}

/*!
    Constructs a new copy of \a other
*/
QPlaceContent::QPlaceContent(const QPlaceContent &other)
    :d_ptr(other.d_ptr)
{
}

/*!
    Assigns the \a other content object to this
*/
QPlaceContent &QPlaceContent::operator=(const QPlaceContent &other)
{
    if (this != &other ) {
        d_ptr = other.d_ptr;
    }
    return *this;
}

/*!
    Destroys the content object
*/
QPlaceContent::~QPlaceContent()
{
}

/*!
    Returns the content type.
*/
QPlaceContent::Type QPlaceContent::type() const
{
    if (!d_ptr)
        return InvalidType;
    return d_ptr->type();
}

/*!
    Returns true if the content object is equivalent to \a other,
    otherwise returns false.
*/
bool QPlaceContent::operator==(const QPlaceContent &other) const
{
    // An invalid content object is only equal to another invalid content object
    if (!d_ptr)
        return !other.d_ptr;

    if (type() != other.type())
        return false;

    return d_ptr->compare(other.d_ptr);
}

bool QPlaceContent::operator!=(const QPlaceContent &other) const
{
    return !(*this == other);
}

/*!
    \internal
    Constructs a new content object from the given pointer \a d.
*/
QPlaceContent::QPlaceContent(QPlaceContentPrivate *d)
    :d_ptr(d)
{
}
