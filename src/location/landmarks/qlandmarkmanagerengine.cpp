/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkmanagerengine.h"
#include "qlandmark.h"
#include "qlandmarkcategory.h"
#include "qlandmarkid.h"
#include "qlandmarkcategoryid.h"

#include "qlandmarkrequests_p.h"
#include "qlandmark_p.h"
#include "qlandmarkcategory_p.h"

#include "qlandmarkabstractrequest.h"
#include "qlandmarkidfetchrequest.h"
#include "qlandmarkfetchrequest.h"
#include "qlandmarkfetchbyidrequest.h"
#include "qlandmarkremoverequest.h"
#include "qlandmarksaverequest.h"
#include "qlandmarkcategoryidfetchrequest.h"
#include "qlandmarkcategoryfetchrequest.h"
#include "qlandmarkcategoryfetchbyidrequest.h"
#include "qlandmarkcategoryremoverequest.h"
#include "qlandmarkcategorysaverequest.h"
#include "qlandmarkimportrequest.h"
#include "qlandmarkexportrequest.h"

#include "qlandmarknamesort.h"

#include "qlandmarkattributefilter.h"
#include "qlandmarkboxfilter.h"
#include "qlandmarkcategoryfilter.h"
#include "qlandmarkintersectionfilter.h"
#include "qlandmarkidfilter.h"
#include "qlandmarknamefilter.h"
#include "qlandmarkproximityfilter.h"
#include "qlandmarkunionfilter.h"

#include "qgeocoordinate.h"

#include <QWeakPointer>

QTM_BEGIN_NAMESPACE

bool matchString(const QString &sourceString, const QString &matchString, QLandmarkFilter::MatchFlags matchFlags )
{
    Qt::CaseSensitivity cs;
    if (matchFlags & QLandmarkFilter::MatchCaseSensitive)
        cs = Qt::CaseSensitive;
    else
        cs = Qt::CaseInsensitive;
    if ((matchFlags & 3) == QLandmarkFilter::MatchEndsWith) {
        return sourceString.endsWith(matchString, cs);
    } else if ((matchFlags & 3) == QLandmarkFilter::MatchStartsWith) {
        return sourceString.startsWith(matchString, cs);
    } else if ((matchFlags & 3) == QLandmarkFilter::MatchContains) {
        return sourceString.contains(matchString,cs);
    } else if (matchFlags & QLandmarkFilter::MatchFixedString) {
        return sourceString.compare(matchString,cs) == 0;
    } else {
        return QVariant(sourceString) == QVariant(matchString);
    }
}

QVariant getLandmarkAttribute(const QString key, const QLandmark &landmark)
{
    if (key == "name") {
        return landmark.name();
    } else if (key == "description") {
        return landmark.description();
    } else if (key == "countryCode") {
        return landmark.address().countryCode();
    } else if (key == "country") {
        return landmark.address().country();
    } else if (key == "state") {
        return landmark.address().state();
    } else if (key == "city") {
        return landmark.address().city();
    } else if (key == "district") {
        return landmark.address().district();
    }  else if (key == "district") {
        return landmark.address().district();
    } else if (key == "street") {
        return landmark.address().street();
    } else if (key == "postcode") {
        return landmark.address().postcode();
    } else if (key == "phoneNumber") {
        return landmark.phoneNumber();
    } else {
        return QVariant(); // shouldn't be possible
    }
}

QStringList commonLandmarkKeys = QStringList() << "name"
                                << "description"
                                << "countryCode"
                                << "country"
                                << "state"
                                << "county"
                                << "city"
                                << "district"
                                << "street"
                                << "postcode"
                                << "phoneNumber";

/*!
    \class QLandmarkManagerEngine
    \brief The QLandmarkManagerEngine class provides the interface for all implementations
    of the landmark manager backend functionality.

    \inmodule QtLocation
    \since 1.1

    \ingroup landmarks-backend

    Instances of this class are usually provided by \l QLandmarkManagerEngineFactory, which is loaded
    from a plugin.
*/

/*!
    Constructs a landmark manager engine.
*/
QLandmarkManagerEngine::QLandmarkManagerEngine()
{
}

/*!
    Destructor.
*/
QLandmarkManagerEngine::~QLandmarkManagerEngine()
{
}

/*! Returns the manager name for this QLandmarkManagerEngine */
QString QLandmarkManagerEngine::managerName() const
{
    return QString(QLatin1String("base"));
}

/*!
  Returns the parameters with which this engine was constructed.  Note that
  the engine may have discarded unused or invalid parameters at the time of
  construction, and these will not be returned.
 */
QMap<QString, QString> QLandmarkManagerEngine::managerParameters() const
{
    return QMap<QString, QString>(); // default implementation requires no parameters.
}

/*!
  Returns the unique URI of this manager, which is built from the manager name and the parameters
  used to construct it.
 */
QString QLandmarkManagerEngine::managerUri() const
{
    return QLandmarkManager::buildUri(managerName(), managerParameters(), managerVersion());
}

/*!
  Returns the engine backend implementation version number
 */
int QLandmarkManagerEngine::managerVersion() const
{
    return 0;
}

/*!
    Returns a list of landmark identifiers which match the given \a filter and are sorted according to
    the given \a sortOrders. The \a limit defines the maximum number of landmark IDs to return and the
    \a offset defines the index offset of the first landmark ID.
    A \a limit of -1 means that IDs of all matching landmarks should be returned.

    Any error which occurs will be saved in \a error and \a errorString.
 */
QList<QLandmarkId> QLandmarkManagerEngine::landmarkIds(const QLandmarkFilter& /*filter*/,
        int /*limit*/, int /*offset*/, const QList<QLandmarkSortOrder>& /*sortOrders*/, QLandmarkManager::Error* /*error*/,
        QString */* errorString */) const
{
    return QList<QLandmarkId>();
}

/*!
    Returns a list of category identifiers
    The \a limit defines the maximum number of IDs to return and the \a offset defines the index offset
    of the first ID.  A \a limit of -1 means IDs for all categories should be returned.
    Any error which occurs will be saved in \a error and \a errorString.
    The identifiers are returned in order according to the given \a nameSort.
*/
QList<QLandmarkCategoryId> QLandmarkManagerEngine::categoryIds(int /*limit*/, int /*offset*/, const QLandmarkNameSort& /*nameSort*/, QLandmarkManager::Error* /*error*/,
        QString* /*errorString*/) const
{
    return QList<QLandmarkCategoryId>();
}

/*!
    Returns the landmark in the datastore identified by \a landmarkId.

    Any errors encountered are:stored in \a error and \a errorString.
    The \a error is set to QLandmarkManager::LandmarkDoesNotExistError if the landmark could not be found.
*/
QLandmark QLandmarkManagerEngine::landmark(const QLandmarkId &/*landmarkId*/, QLandmarkManager::Error * /*error*/,
        QString* /*errorString*/) const
{
    return QLandmark();
}

/*!
    Returns a list of landmarks which match the given \a landmarkIds.  The engine will populate \a errorMap
    (the map of indices of the \a landmarkIds list to errors) for the indexes where the landmark could not
    be retrieved.

    Overall operation errors are stored in \a error and
    \a errorString.  \a error is set to QLandmarkManager::NoError,
    all landmarks were successfully retrieved.
*/
QList<QLandmark> QLandmarkManagerEngine::landmarks(const QList<QLandmarkId>& /*landmarkIds*/, QMap<int, QLandmarkManager::Error>* /*errorMap*/,
                                        QLandmarkManager::Error* /*error*/, QString* /*errorString*/) const
{
    return QList<QLandmark>();
}

/*!
    Returns a list of landmarks which match the given \a filter and are sorted according to the \a sortOrders.
    The \a limit defines the maximum number of landmarks to return and the \a offset defines the index offset
    of the first landmark.  A \a limit of -1 means all matching landmarks should be returned.

    Overall operation errors are stored in \a error and \a errorString.
*/
QList<QLandmark> QLandmarkManagerEngine::landmarks(const QLandmarkFilter& /*filter*/, int /*limit*/, int /*offset*/,
                    const QList<QLandmarkSortOrder>& /*sortOrders*/, QLandmarkManager::Error* /*error*/, QString* /*errorString*/) const
{
    return QList<QLandmark>();
}

/*!
    Returns the category in the datastore identified by \a categoryId.

    Any errors encountered are stored in \a error and \a errorString.
        A QLandmarkManager::CategoryDoesNotExist error is set if the category could not be found.
*/
QLandmarkCategory QLandmarkManagerEngine::category(const QLandmarkCategoryId& /*categoryId*/,
        QLandmarkManager::Error* /*error*/, QString* /*errorString*/) const
{
    return QLandmarkCategory();
}

/*!
    Returns a list of categories which match the given \a categoryIds.  The engine will populate \a errorMap
    (the map of indices of the \a categoryIds list to errors) for the indexes where the category could not
    be retrieved.

    Overall operation errors are stored in \a error and
    \a errorString.  \a error is set to QLandmarkManager::NoError, if
    all categories were successfully retrieved.
*/
QList<QLandmarkCategory> QLandmarkManagerEngine::categories(const QList<QLandmarkCategoryId>& /*categoryIds*/,
                                                            QMap<int, QLandmarkManager::Error>* /*errorMap*/,
                                                            QLandmarkManager::Error* /*error*/, QString* /*errorString*/) const
{
    return QList<QLandmarkCategory>();
}

/*!
    Returns a list of categories.
    The \a limit defines the maximum number of categories to return and the \a offset defines the index offset
    of the first category.  A \a limit of -1 means all categories should be returned.
    The returned categories ordered according to the gien \a nameSort.
*/
QList<QLandmarkCategory> QLandmarkManagerEngine::categories(int /*limit*/, int /*offset*/, const QLandmarkNameSort& /*nameSort*/,
                                                            QLandmarkManager::Error* /*error*/, QString* /*errorString*/) const
{
    return QList<QLandmarkCategory>();
}

/*!
    Adds the given \a landmark to the datastore if \a landmark has a
    default-constructed identifer, or an identifier with the manager
    URI set to the URI of this manager and an empty id.

    If the manager URI of the identifier of the \a landmark is neither
    empty nor equal to the URI of this manager, or the id member of the
    identifier is not empty, but does not exist in the manager,
    the operation will fail and and \a error will be set to
    \c QLandmarkManager::LandmarkDoesNotExistError.

    Alternatively, the function will update the existing landmark in the
    datastore if \a landmark has a non-empty id and currently exists
    within the datastore.

    Returns false on failure or true on success.  On successful save
    of a landmark with an empty id, it will be assigned a valid
    id and have its manager URI set to the URI of this manager.

    The engine must emit the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Any errors encountered during this operation should be stored in
    \a error and \a errorString.
*/
bool QLandmarkManagerEngine::saveLandmark(QLandmark* /*landmark*/,
        QLandmarkManager::Error* /*error*/, QString* /*errorString*/)
{
    return false;
}

/*!
    Adds the list of \a landmarks to the datastore.
    Returns true if the landmarks were saved successfully, otherwise returns
    false.

    The engine will populate \a errorMap (the map of indices of the
    \a landmarks list to errors) for every index for which the landmark could not be
    saved.


    For each newly saved landmark that was successful, the identifier
    of the landmark will be updated with a new value.

    The engine emits the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Overall operation errors are stored in \a error and
    \a errorString.  \a error is set to QLandmarkManager::NoError,
    if all \a landmarks were successfully saved.

    \sa QLandmarkManagerEngine::saveLandmark()
*/
bool QLandmarkManagerEngine::saveLandmarks(QList<QLandmark> * /*landmarks*/, QMap<int, QLandmarkManager::Error>* /*errorMap*/,
        QLandmarkManager::Error* /*error*/, QString* /*errorString*/)
{
    return false;
}

/*!
    Remove the landmark identified by \a landmarkId from the datastore.

    Returns true if the landmark was removed successfully, otherwise
    returnse false.

    The engine emits the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Any errors encountered during this operation should be stored to
    \a error and \a errorString.
*/
bool QLandmarkManagerEngine::removeLandmark(const QLandmarkId& /*landmarkId*/, QLandmarkManager::Error* /*error*/, QString* /*errorString*/)
{
    return false;
}

/*!
    Removes every landmark whose identifier is contained in the list
    of \a landmarkIds.  Returns true if all landmarks were removed
    successfully, otherwise false.

    The engine populates \a errorMap (the map of indices of the
    \a landmarkIds list to errors) for every index for which the landmark could not be
    removed.

    The engine also emits the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Overall operation errors are stored in \a error and
    \a errorString.  \a error is set to QLandmarkManager::NoError, if
    all landmarks were successfully removed.

    \sa QLandmarkManagerEngine::removeLandmark()
*/
bool QLandmarkManagerEngine::removeLandmarks(const QList<QLandmarkId>& /*landmarkIds*/, QMap<int, QLandmarkManager::Error>* /*errorMap*/,
        QLandmarkManager::Error* /*error*/, QString* /*errorString*/)
{
    return false;
}

/*!
    Adds the given \a category to the datastore if \a category has a
    default-constructed identifier, or an identifier with the manager
    URI set to the URI of this manager and an empty id.

    If the manager URI of the identifier of the \a category is neither
    empty nor equal to the URI  of this manager, or the id member of the
    identifier is not empty, but does not exist in the manager,
    the operation should fail and \a error should be set to
    \c QLandmarkManager::CategoryDoesNotExistError.

    Alternatively, the function should update the existing category in the
    datastore if \a category has a non-empty id and currently exists
    within the datastore.

    Returns false on failure or true on success.  On successful save
    of a category with an invalid id, it should be assigned a valid
    id and have its manager URI set to the URI of this manager.

    The engine returns the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Overall operations errors should be stored in \a error and
    \a errorString.
*/
bool QLandmarkManagerEngine::saveCategory(QLandmarkCategory* /*category*/,
        QLandmarkManager::Error* /*error*/, QString* /*errorString*/)
{
    return false;
}

/*!
    Removes the category identified by \a categoryId from the datastore.

    Returns true if the category was removed successfully, otherwise
    returnse false.

    The engine emits the appropriate signals to inform clients of changes
    to the datastore resulting from this operation

    Overall operational errors are stored in \a error and
    \a errorString.
*/
bool QLandmarkManagerEngine::removeCategory(const QLandmarkCategoryId& /*categoryId*/,
        QLandmarkManager::Error* /*error*/, QString* /*errorString*/)
{
    return false;
}

/*!
    Reads landmarks from the given \a device and saves them.  The data from the \a device
    is expected to adhere to the provided \a format.  If no \a format is provided,
    the manager engine tries to autodetect the \a format.

    The \a option can be used to control whether categories in the imported
    file will be added during the import.  If the \c AttachSingleCategory option is used, then
    all the landmarks in the import file are assigned to the category identified by
    \a categoryId, in all other circumstances \a categoryId is ignored.  If \a categoryId
    doesn't exist when using \c AttachSingleCategory, QLandmarkManager::CategoryDoesNotExist error is returned.  Note that
    some file formats may not support categories at all.

    Returns true if all landmarks could be imported, otherwise returns false.

    Overall operational errors are stored in \a error and
    \a errorString.
*/
bool QLandmarkManagerEngine::importLandmarks(QIODevice* /*device*/, const QString& /*format*/, QLandmarkManager::TransferOption /*option*/, const QLandmarkCategoryId& /*categoryId*/,
        QLandmarkManager::Error *error, QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NotSupportedError;
    *errorString ="Not supported";
    return false;
}

/*!
    Writes landmarks to the given \a device.  The landmarks will be written
    according to the specified \a format.  If  \a landmarkIds is empty, then
    all landmarks will be exported, otherwise only those landmarks that
    match \a landmarkIds will be exported.

    The \a option can be used to control whether categories will be exported or not.
    Note that the \c AttachSingleCategory option has no meaning during
    export and the manager will export as if \a option was \c IncludeCategoryData.
    Also, be aware that some file formats may not support categories at all and for
    these formats, the \a option is always treated as if it was \c ExcludeCategoryData.

    Returns true if all specified landmarks were successfully exported,
    otherwise returns false.

    Overall operation errors are stored in \a error and
    \a errorString.
*/
bool QLandmarkManagerEngine::exportLandmarks(QIODevice * /*device*/, const QString& /*format*/, const QList<QLandmarkId> &/*landmarkIds*/, QLandmarkManager::TransferOption /*option*/,
        QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NotSupportedError;
    *errorString = "Not supported";
    return false;
}

/*!
    Returns the supported file formats for the given transfer \a operation, i.e. import or export.
    Errors are stored in \a error and \a errorString.
*/
QStringList QLandmarkManagerEngine::supportedFormats(QLandmarkManager::TransferOperation /*operation*/ , QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    return QStringList();
}

/*!
    \fn QLandmarkManager::SupportLevel QLandmarkManagerEngine::filterSupportLevel(const QLandmarkFilter &filter,
                                                            QLandmarkManager::Error *error, QString *errorString) const

    Returns the support level the manager engine provides for the given \a filter.  Errors are stored in \a error
    and \a errorString.
*/

/*!
    \fn QLandmarkManager::SupportLevel QLandmarkManagerEngine::sortOrderSupportLevel(const QLandmarkSortOrder &sortOrder,
                                                            QLandmarkManager::Error *error, QString *errorString) const

    Returns the support level the manager engine provides for the given \a sortOrder.  Errors are stored in \a error
    and \a errorString.
*/

/*!
    \fn bool QLandmarkManagerEngine::isFeatureSupported(QLandmarkManager::ManagerFeature feature, QLandmarkManager::Error *error, QString *errorString) const

    Returns true if the manager engine supports the given \a feature, otherwise returns false;   Errors are stored in
    \a error and \a errorString.
*/

/*!
    \fn bool QLandmarkManagerEngine::isReadOnly(QLandmarkManager::Error *error, QString *errorString) const

    Returns true if the manager engine is exclusively read only.  Meaning
    landmarks and categories cannot be added, modified or removed.  Errors are stored in \a error and \a errorString.
*/

/*!
    \fn bool QLandmarkManagerEngine::isReadOnly(const QLandmarkId &landmarkId,
                                                QLandmarkManager::Error *error, QString *errorString) const

    Returns true if the landmark identified by \a landmarkId
    considered read-only by the manager engine.

    If the \a landmarkId does not refer to an existing landmark,
    it is considered writable unless the manager engine is exclusively read-only.
    Errors are stored in \a error and \a errorString.
 */

 /*!
    \fn bool QLandmarkManagerEngine::isReadOnly(const QLandmarkCategoryId &categoryId,
                                                QLandmarkManager::Error *error, QString *errorString) const

    Returns true if the category identified by \a categoryId is
    considered read-only by the manager engine.

    If \a categoryId does not refer to an existing category,
    it is considered writable unless the manager engine is exclusively read-only.
    Errors are stored in \a error and \a errorString.
*/

/*!
   \fn QStringList QLandmarkManagerEngine::searchableLandmarkAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const
    Returns the list of landmark attribute keys that may be used in a QLandmarkAttributeFilter.
    Errors are stored in \a error and \a errorString.
*/

/*!
    Notifies the manager engine that the given \a request has been destroyed.
*/
void QLandmarkManagerEngine::requestDestroyed(QLandmarkAbstractRequest* request)
{
    Q_UNUSED(request);
}

/*!
    Asks the manager engine to begin the given \a request
    which is currently in a re(startable) state.

    Returns true if the request was started successfully,
    else returns false.

    \sa QLandmarkAbstractRequest::start()
*/
bool QLandmarkManagerEngine::startRequest(QLandmarkAbstractRequest* request)
{
    Q_UNUSED(request);
    return false;
}

/*!
    Asks the manager engine to cancel the given \a request which was
    previously started and is currently in a cancellable state.
    Returns true if cancelation of the request was started successfully,
    otherwise returns false.

     \sa startRequest(), QLandmarkAbstractRequest::cancel()
 */
bool QLandmarkManagerEngine::cancelRequest(QLandmarkAbstractRequest* request)
{
    Q_UNUSED(request);
    return false;
}

/*!
    Blocks until the manager engine has completed the given \a request
    which was previously started, or until \a msecs milliseconds have passed.
    Returns true if the request was completed, and false if the request was not in the
    \c QLandmarkAbstractRequest::Active state, no progress could be reported or
    if the engine does not support waitForFinished functionality.

    \sa startRequest()
 */
bool QLandmarkManagerEngine::waitForRequestFinished(QLandmarkAbstractRequest* request, int msecs)
{
    Q_UNUSED(request);
    Q_UNUSED(msecs);
    return false;
}

/*!
    \fn QLandmarkManagerEngine::dataChanged()

    This signal is emitted some time after changes occur to the datastore managed by this
    engine, and the engine is unable to precisely determine which changes occurred, or if the
    engine considers the changes to be radical enough to require clients to reload all data.

    If this signal is emitted, no other signals may be emitted for the associated changes.

    As it is possible that other processes (or other devices) may have caused the
    changes, the timing can not be determined.

    \sa landmarksAdded(), landmarksChanged(), landmarksRemoved(), categoriesAdded(),
    categoriesChanged(), categoriesRemoved()
*/

/*!
    \fn QLandmarkManagerEngine::landmarksAdded(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted some time after a set of landmarks has been added to
    the datastore managed by the engine and where the \l dataChanged() signal was not emitted for those changes.
    As it is possible that other processes(or other devices) may
    have added the landmarks, the exact timing cannot be determined.

    There may be one or more landmark identifiers in the \a landmarkIds list.

    \sa dataChanged(), landmarksChanged(), landmarksRemoved()
*/

/*!
    \fn QLandmarkManagerEngine::landmarksChanged(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted some time after a set of landmarks have been modified in
    the datastore managed by this engine and where the \l dataChanged() signal was not emitted for those changes.
    As it is possible that other processes(or other devices) may have modified the landmarks,
    the timing cannot be determined.

    Note that removal of a category will not trigger a \c landmarksChanged signal for landmarks belonging to that category.

    There may be one ore more landmark identifiers in the \a landmarkIds list.
    \sa dataChanged(), landmarksAdded(), landmarksRemoved()
*/


/*!
    \fn QLandmarkManagerEngine::landmarksRemoved(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted some time after a set of landmarks have been removed from the
    datastore managed by this engine and where the \l dataChanged() signal was not emitted for those changes.
    As it is possible that other processes(or other devices) may have removed the landmarks,
    the timing cannot be determined.

    There may be one ore more landmark identifiers in the \a landmarkIds list.
    \sa dataChanged(), landmarksAdded(), landmarksChanged()
*/

/*!
   \fn QLandmarkManagerEngine::categoriesAdded(const QList<QLandmarkCategoryId> &categoryIds)

   This signal is emitted some time after a set of categories has been added to the datastore
   managed by this engine and where the \l dataChanged() signal was not emitted for those changes.
   As it is possible that other processes(or other devices) may
   have added the landmarks, the exact timing cannot be determined.

   There may be one or more category identifiers in the \a categoryIds list.

   \sa dataChanged(), categoriesChanged(), categoriesRemoved()
*/

/*!
    \fn QLandmarkManagerEngine::categoriesChanged(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted some time after a set of categories have been modified in the datastore
    managed by the engine and where the \l dataChanged() signal was not emitted for those changes.
    As it is possible that other processes(or other devices) may have modified the categories,
    the timing cannot be determined.

    There may be one ore more category identifiers in the \a categoryIds list.

    \sa dataChanged(), categoriesAdded(), categoriesRemoved()
*/

/*!
    \fn QLandmarkManagerEngine::categoriesRemoved(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted some time after a set of categories have been removed from the datastore
    managed by this engine and where the \l dataChanged() signal was not emitted for those changes.
    As it is possible that other processes(or other devices) may have removed the categories,
    the timing cannot be determined.

    There may be one ore more category identifiers in the \a categoryIds list.

    \sa dataChanged(), categoriesAdded(), categoriesChanged()
*/

/*!
    Updates the given asynchronous request \a req by setting the new \a state
    of the request.  If the new state is different, the stateChanged() signal will be emitted
    by the request.

*/
void QLandmarkManagerEngine::updateRequestState(QLandmarkAbstractRequest *req, QLandmarkAbstractRequest::State state)
{
    if (req) {
        QMutexLocker ml(&req->d_ptr->mutex);
        if (req->d_ptr->state != state) {
        req->d_ptr->state = state;
        ml.unlock();
        emit req->stateChanged(state);
        }
    }
}

/*!
    Updates the given QLandmarkIdFetchRequest \a req with the latest \a result,
    and operation \a error and \a errorString.  In addition, the state of the request
    will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkIdFetchRequest(QLandmarkIdFetchRequest* req, const QList<QLandmarkId>& result,
        QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkIdFetchRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkIdFetchRequestPrivate * rd = static_cast<QLandmarkIdFetchRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->landmarkIds = result;
        bool emitState = rd->state != newState;
        rd->state =newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkFetchRequest \a req with the latest \a result,
    and operation \a error and \a errorString.  In addition, the state of the request
    will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkFetchRequest(QLandmarkFetchRequest* req, const QList<QLandmark>& result,
        QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkFetchRequest> ireq(req); //Take this incase the first emit deletes us.
        QLandmarkFetchRequestPrivate * rd = static_cast<QLandmarkFetchRequestPrivate*>(ireq.data()->d_ptr);

        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->landmarks = result;
        bool emitState = rd->state != newState;
        rd->state =newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit req->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkFetchByIdRequest \a req with the latest \a result,
    operation \a error and \a errorString and map of input index to individual errors, \a errorMap.
     In addition, the state of the request
    will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkFetchByIdRequest(QLandmarkFetchByIdRequest* req, const QList<QLandmark>& result, QLandmarkManager::Error error,
        const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkFetchByIdRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkFetchByIdRequestPrivate* rd = static_cast<QLandmarkFetchByIdRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->errorMap = errorMap;
        rd->landmarks = result;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && req)
            emit req->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkRemoveRequest \a req with the operation \a error and
    \a errorString and map of input index to individual errors, \a errorMap.  In addition,
    the state of the request will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req, QLandmarkManager::Error error, const QString &errorString,
        const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkRemoveRequest>ireq(req);//Take this in case the first emit delete us
        QLandmarkRemoveRequestPrivate* rd = static_cast<QLandmarkRemoveRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->errorMap = errorMap;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit req->stateChanged(newState);
    }

}

/*!
    Updates the given QLandmarkSaveRequest \a req with the latest \a result, operation \a error
    and \a errorString, and map of input index to individual errors, \a errorMap.
    In addition, the state of the request will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkSaveRequest(QLandmarkSaveRequest* req, const QList<QLandmark>& result, QLandmarkManager::Error error,
        const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkSaveRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkSaveRequestPrivate* rd = static_cast<QLandmarkSaveRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->errorMap = errorMap;
        rd->landmarks = result;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit req->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkCategoryIdFetchRequest \a req with the latest \a result,
    and operation \a error and \a errorString.  In addition, the state of the request
    will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkCategoryIdFetchRequest(QLandmarkCategoryIdFetchRequest* req, const QList<QLandmarkCategoryId>& result,
        QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)

{
    if (req) {
        QWeakPointer<QLandmarkCategoryIdFetchRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkCategoryIdFetchRequestPrivate* rd = static_cast<QLandmarkCategoryIdFetchRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->categoryIds = result;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit req->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkCategoryFetchRequest \a req with the latest \a result,
    and operation \a error and \a errorString.  In addition, the state of the request
    will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkCategoryFetchRequest(QLandmarkCategoryFetchRequest* req, const QList<QLandmarkCategory>& result,
        QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)

{
    if (req) {
        QWeakPointer<QLandmarkCategoryFetchRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkCategoryFetchRequestPrivate* rd = static_cast<QLandmarkCategoryFetchRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->categories = result;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit req->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkCategoryFetchByIdRequest \a req with the latest \a result,
    and operation \a error and \a errorString, and map of input index to individual errors, \a errorMap.
   In addition, the state of the request will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkCategoryFetchByIdRequest(QLandmarkCategoryFetchByIdRequest* req, const QList<QLandmarkCategory>& result, QLandmarkManager::Error error,
        const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkCategoryFetchByIdRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkCategoryFetchByIdRequestPrivate* rd = static_cast<QLandmarkCategoryFetchByIdRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->errorMap = errorMap;
        rd->categories = result;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit req->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkCategoryRemoveRequest \a req with the operation \a error and
    \a errorString and map of input index to individual errors, \a errorMap.  In addition,
    the state of the request will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkCategoryRemoveRequest(QLandmarkCategoryRemoveRequest* req, QLandmarkManager::Error error, const QString &errorString,
        const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkCategoryRemoveRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkRemoveRequestPrivate* rd = static_cast<QLandmarkRemoveRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->errorMap = errorMap;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit req->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkSaveCategoryRequest \a req with the latest \a result, operation error \a error
    and \a errorString, and map of input index to individual errors, \a errorMap.
    In addition, the state of the request will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkCategorySaveRequest(QLandmarkCategorySaveRequest* req, const QList<QLandmarkCategory>& result, QLandmarkManager::Error error,
        const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkCategorySaveRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkCategorySaveRequestPrivate* rd = static_cast<QLandmarkCategorySaveRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->errorMap = errorMap;
        rd->categories = result;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit req->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkImportRequest \a req with the operation \a error and \a errorString.
    In addition the state of the request is changed to \a newState.  This function may also be used
    to update the \a ids of the landmarks which have been imported.

    It then causes the request to emit its resultsAvailable() signal to notify the clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
*/
void QLandmarkManagerEngine::updateLandmarkImportRequest(QLandmarkImportRequest *req, const QList<QLandmarkId> &ids, QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkImportRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkImportRequestPrivate* rd = static_cast<QLandmarkImportRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        rd->landmarkIds = ids;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState && ireq)
            emit req->stateChanged(newState);
    }
}

/*!
    Updates the given QLandmarkExportRequest \a req with the operation \a error and \a errorString.
    In addition the state of the request is changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify the clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
*/
void QLandmarkManagerEngine::updateLandmarkExportRequest(QLandmarkExportRequest *req, QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QLandmarkExportRequest> ireq(req); //Take this in case the first emit delete us
        QLandmarkExportRequestPrivate* rd = static_cast<QLandmarkExportRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->mutex);
        rd->error = error;
        rd->errorString = errorString;
        bool emitState = rd->state != newState;
        rd->state = newState;
        ml.unlock();
        emit req->resultsAvailable();
        if (emitState  && req)
            emit req->stateChanged(newState);
    }
}

/*!
  Compares two landmarks (\a a and \a b) using the given list of \a sortOrders.  Returns a negative number if \a a should appear
  before \a b according to the sort order, a positive number if \a a should appear after \a b according to the sort order,
  and zero if the two are unable to be sorted.
 */
int QLandmarkManagerEngine::compareLandmark(const QLandmark& a, const QLandmark& b, const QList<QLandmarkSortOrder>& sortOrders)
{
    int comparison = 0;
    for(int i=0; i < sortOrders.count(); ++i) {

        switch (sortOrders.at(i).type())
        {
            case (QLandmarkSortOrder::NameSort):
            {
                const QLandmarkNameSort nameSort = sortOrders.at(i);
                comparison = compareName(a, b, nameSort);
                break;
            }
            default:
                comparison =0;
        }

        if (comparison != 0)
            break;
    }
    return comparison;
}


/*!
  Compares two landmarks (\a a and \a b) by name.

  Returns a negative number if \a a should appear before \a b according to the \a nameSort,
  a positive number if \a a should appear after \a b according to the \a nameSort,
  and zero if the two are unable to be sorted.

  Assuming an ascending order sort, an integer less than, equal to, or greater than zero
  is returned if \a a is less than, equal to or greater than \a b.

  \a nameSort specifies whether an ascending or descending order is used and whether
  the sort is case sensitive or not.
 */
int QLandmarkManagerEngine::compareName(const QLandmark &a, const QLandmark &b, const QLandmarkNameSort &nameSort)
{
    int result = QString::compare(a.name(), b.name(), nameSort.caseSensitivity());

    if (nameSort.direction() == Qt::DescendingOrder)
        result *= -1;

    return result;
}

/*!
  Performs an insertion sort \a landmark into the \a sorted list, according to the provided \a sortOrders list.
  The first QLandmarkSortOrder in the list has the highest priority; if the \a landmark is deemed equal to another
  in the \a sorted list, the second QLandmarkSortOrder in the list is used (and so on until either the landmark is inserted
  or there are no more sort order objects in the list).
 */
void QLandmarkManagerEngine::addSorted(QList<QLandmark>* sorted, const QLandmark& landmark, const QList<QLandmarkSortOrder>& sortOrders)
{
    if (sortOrders.count() > 0) {
        for (int i = 0; i < sorted->size(); i++) {
            // check to see if the new landmark should be inserted here
            int comparison = compareLandmark(sorted->at(i), landmark, sortOrders);
            if (comparison > 0) {
                sorted->insert(i, landmark);
                return;
            }
        }
    }

    // hasn't been inserted yet?  append to the list.
    sorted->append(landmark);
}

/*!
  Returns true if the supplied \a landmark matches the supplied \a filter.
 */
bool QLandmarkManagerEngine::testFilter(const QLandmarkFilter& filter, const QLandmark& landmark)
{
    switch(filter.type()) {
        case QLandmarkFilter::DefaultFilter:
            return true;

        case QLandmarkFilter::AttributeFilter:
        {
            const QLandmarkAttributeFilter attribFilter(filter);
            QStringList filterKeys = attribFilter.attributeKeys();

            if (attribFilter.operationType() ==  QLandmarkAttributeFilter::AndOperation) {
                QVariant lmAttributeValue;
                foreach(const QString filterKey, filterKeys)
                {
                    if (commonLandmarkKeys.contains(filterKey)) {
                        lmAttributeValue = getLandmarkAttribute(filterKey, landmark);

                        if (lmAttributeValue.type() == QVariant::String) {
                            QString lmString = lmAttributeValue.toString();
                            QString attribString = attribFilter.attribute(filterKey).toString();
                            if (matchString(lmString, attribString, attribFilter.matchFlags(filterKey)))
                                continue;
                        } else if (attribFilter.attribute(filterKey) == lmAttributeValue) {
                            continue;
                        }

                        return false;
                    } else {
                        return false;
                    }
                }
                return true;
            } else {//must be OR operation
                QVariant lmAttributeValue;
                foreach(const QString filterKey, filterKeys) {
                    if (commonLandmarkKeys.contains(filterKey)) {

                        lmAttributeValue = getLandmarkAttribute(filterKey, landmark);

                        if (lmAttributeValue.type() == QVariant::String) {
                            QString lmString = lmAttributeValue.toString();
                            QString attribString = attribFilter.attribute(filterKey).toString();
                            if (matchString(lmString, attribString, attribFilter.matchFlags(filterKey)))
                                return true;
                        }

                        if (attribFilter.attribute(filterKey) == lmAttributeValue) {
                            return true;
                        }
                    }
                }
                return false;
            }
        }
        case QLandmarkFilter::BoxFilter:
        {
            const QLandmarkBoxFilter boxFilter(filter);

            if (!boxFilter.boundingBox().isValid())
                return false;

            double tly = boxFilter.boundingBox().topLeft().latitude();
            double bry = boxFilter.boundingBox().bottomRight().latitude();
            double tlx = boxFilter.boundingBox().topLeft().longitude();
            double brx = boxFilter.boundingBox().bottomRight().longitude();

            bool latWrap = (tly < bry);
            bool longWrap = (tlx > brx);

            if (latWrap)
                return false;

            //check if landmark is outside the box's latitudes
            if ( landmark.coordinate().latitude() < bry && landmark.coordinate().latitude() > tly)
                return false;

            double lmx = landmark.coordinate().longitude();
            if (longWrap) {
                //if (landmark.coordinate().longitude() <= tlx || landmark.coordinate().longitude() >= brx)
                if ( ((lmx > 0.0) && (lmx<= tlx)) || ((lmx < 0.0) && (lmx >= brx)))
                    return false;
            } else {
                if (lmx < tlx || lmx > brx)
                    return false;
            }

            //landmark must be within the bounds to reach here.
            return true;
        }
        case QLandmarkFilter::CategoryFilter:
        {
            const QLandmarkCategoryFilter categoryFilter(filter);
            QList<QLandmarkCategoryId> categories = landmark.categoryIds();
            foreach(const QLandmarkCategoryId id, categories)
            {
                if (id == categoryFilter.categoryId())
                    return true;
            }
            return false;
        }
        case QLandmarkFilter::IntersectionFilter:
        {
            const QLandmarkIntersectionFilter andFilter(filter);
            const QList<QLandmarkFilter>& terms = andFilter.filters();
            if (terms.count() ==0)
                return false;

            for(int i=0; i < terms.count();i++) {
                    if (!testFilter(terms.at(i), landmark))
                        return false;
            }
            return true;
        }
        case QLandmarkFilter::LandmarkIdFilter:
        {
            QLandmarkIdFilter idFilter(filter);
            const QList<QLandmarkId> &ids = idFilter.landmarkIds();
            foreach(const QLandmarkId id, ids) {
                if (id == landmark.landmarkId())
                    return true;
            }
            return false;
        }
        case QLandmarkFilter::InvalidFilter:
        {
            return false;
        }
        case QLandmarkFilter::NameFilter:
        {
            QLandmarkNameFilter nameFilter(filter);
            return matchString(landmark.name(), nameFilter.name(), nameFilter.matchFlags());
        }
        case QLandmarkFilter::ProximityFilter:
        {
            QLandmarkProximityFilter proximityFilter(filter);

            qreal distance = proximityFilter.center().distanceTo(landmark.coordinate());
            if (distance < proximityFilter.radius() || qFuzzyCompare(distance, proximityFilter.radius()))
                return true;
            else
                return false;
        }
        case QLandmarkFilter::UnionFilter:
        {
            const QLandmarkUnionFilter orFilter(filter);
            const QList<QLandmarkFilter>& terms = orFilter.filters();
            if (terms.count() == 0)
                return false;
            else {
                for (int i=0; i < terms.count(); i++) {
                      if (testFilter(terms.at(i), landmark))
                        return true;
                }
                return false;
            }
        }
    }
    return false;
}

/*! Sorts the given list of \a landmarks according to the provided \a sortOrders */
QList<QLandmarkId> QLandmarkManagerEngine::sortLandmarks(const QList<QLandmark>& landmarks, const QList<QLandmarkSortOrder>& sortOrders)
{
    QList<QLandmarkId> landmarkIds;
    QList<QLandmark> sortedLandmarks;
    if(!sortOrders.isEmpty()) {
        foreach (const QLandmark& landmark, landmarks) {
            QLandmarkManagerEngine::addSorted(&sortedLandmarks, landmark, sortOrders);
        }

        foreach (const QLandmark& landmark, sortedLandmarks) {
            landmarkIds.append(landmark.landmarkId());
        }
    } else {
        foreach(const QLandmark& landmark, landmarks) {
            landmarkIds.append(landmark.landmarkId());
        }
    }
    return landmarkIds;
}

#include "moc_qlandmarkmanagerengine.cpp"

QTM_END_NAMESPACE
