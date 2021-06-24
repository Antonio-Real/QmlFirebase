#include "firebaseuser.h"


/*!
    \qmltype FirebaseUser
    \inqmlmodule Firebase
    \ingroup Firebase
    \brief Type that encapsulate user data for easy access.

    FirebaseUser simply holds data about the users authenticated in the firebase. This type is used by \l FirebaseAuth to hold a reference
    to the user currently authenticated and can also be used to create models/lists of all available users in the \l FirebaseDatabase.
*/
FirebaseUser::FirebaseUser(QObject *parent) : QObject(parent)
{
}

/*!
    \qmlproperty string FirebaseUser::email

    Specifies the user's email.
 */
QString FirebaseUser::email() const
{
    return m_email;
}

void FirebaseUser::setEmail(const QString &email)
{
    if(m_email == email)
        return;

    m_email = email;
    emit emailChanged();
}

/*!
    \qmlproperty string FirebaseUser::idToken

    Specifies the users \a idToken, often used for actions that require authentication such as writing/reading data from the \l FirebaseDatabase or managing the user profile with \l FirebaseAuth.
    This token expires after 1 hour and should be refreshed using the \l refreshToken.

    \note for more information about use, see \l FirebaseAuth::currentUser
 */
QString FirebaseUser::idToken() const
{
    return m_idToken;
}

void FirebaseUser::setIdToken(const QString &idToken)
{
    if(m_idToken == idToken)
        return;

    m_idToken = idToken;
    emit idTokenChanged();
}


/*!
    \qmlproperty string FirebaseUser::refreshToken

    Returns the refresh token, tipically used to refresh the \l idToken after it expires.

    \note for more information about use, see \l FirebaseAuth::exchangeRefreshToken
 */
QString FirebaseUser::refreshToken() const
{
    return m_refreshToken;
}

void FirebaseUser::setRefreshToken(const QString &refreshToken)
{
    if(m_refreshToken == refreshToken)
        return;

    m_refreshToken = refreshToken;
    emit refreshTokenChanged();
}

/*!
    \qmlproperty string FirebaseUser::userId

    Specifies the unique identifier of the user in the entire Firebase project.

    \warning this identifier will change if the email of the user is altered.
 */
QString FirebaseUser::userId() const
{
    return m_userId;
}

void FirebaseUser::setUserId(const QString &userId)
{
    if(m_userId == userId)
        return;

    m_userId = userId;
    emit userIdChanged();
}

/*!
    \qmlproperty string FirebaseUser::name

    Returns the \a name of the user.
 */
QString FirebaseUser::name() const
{
    return m_name;
}

void FirebaseUser::setName(const QString &name)
{
    if(m_name == name)
        return;

    m_name = name;
    emit nameChanged();
}

/*!
    \qmlproperty bool FirebaseUser::emailVerified

    Specifies if the email of this user has already been verified. A verification email can be sent using \l FirebaseAuth::sendEmailVerification()
 */
bool FirebaseUser::emailVerified() const
{
    return m_emailVerified;
}

void FirebaseUser::setEmailVerified(bool emailVerified)
{
    if(m_emailVerified == emailVerified)
        return;

    m_emailVerified = emailVerified;
    emit emailVerifiedChanged();
}

/*!
    \qmlproperty string FirebaseUser::photoUrl

    Url that holds the profile picture of the user.
 */
QString FirebaseUser::photoUrl() const
{
    return m_photoUrl;
}

void FirebaseUser::setPhotoUrl(const QString &photoUrl)
{
    if(m_photoUrl == photoUrl)
        return;

    m_photoUrl = photoUrl;
    emit photoUrlChanged();
}
