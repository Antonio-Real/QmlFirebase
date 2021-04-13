#ifndef AUTHUTILS_H
#define AUTHUTILS_H
#include <QString>

namespace AuthUtils {

static const QString endpoint_signUp("https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=");
static const QString endpoint_signIn("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=");
static const QString endpoint_signInOAuth("https://identitytoolkit.googleapis.com/v1/accounts:signInWithIdp?key=");
static const QString endpoint_fetchProviders("https://identitytoolkit.googleapis.com/v1/accounts:createAuthUri?key=");
static const QString endpoint_sendPasswordReset("https://identitytoolkit.googleapis.com/v1/accounts:sendOobCode?key=");
static const QString endpoint_verifyPasswordReset("https://identitytoolkit.googleapis.com/v1/accounts:resetPassword?key=");
static const QString endpoint_confirmPasswordReset("https://identitytoolkit.googleapis.com/v1/accounts:resetPassword?key=");
static const QString endpoint_changeEmail("https://identitytoolkit.googleapis.com/v1/accounts:update?key=");
static const QString endpoint_changePassword("https://identitytoolkit.googleapis.com/v1/accounts:update?key=");
static const QString endpoint_profileUpdate("https://identitytoolkit.googleapis.com/v1/accounts:update?key=");
static const QString endpoint_getUserData("https://identitytoolkit.googleapis.com/v1/accounts:lookup?key=");
static const QString endpoint_sendEmailVerification("https://identitytoolkit.googleapis.com/v1/accounts:sendOobCode?key=");
static const QString endpoint_confirmEmailVerification("https://identitytoolkit.googleapis.com/v1/accounts:update?key=");
static const QString endpoint_deleteAccount("https://identitytoolkit.googleapis.com/v1/accounts:delete?key=");
static const QString endpoint_refreshToken("https://securetoken.googleapis.com/v1/token?key=");

// For showing a clearer message to the user
static QString parseJSONerror(QString err)
{
    // Exchange refresh token for ID token
    if(err == "TOKEN_EXPIRED")
        return "User credential not valid, sign in again";
    else if(err == "USER_DISABLED")
        return "User account disabled, contact administrator";
    else if(err == "USER_NOT_FOUND")
        return "User with the corresponding refreshToken not found";
    else if(err == "INVALID_REFRESH_TOKEN")
        return "Invalid refresh token was provided";
    else if(err == "INVALID_GRANT_TYPE")
        return "MISSING_REFRESH_TOKEN";
    else if(err == "MISSING_REFRESH_TOKEN")
        return "No refresh token provided";
    else if(err == "USER_DISABLED")
        return "User account disabled, contact administrator";

    // Sign up with email/password
    else if(err == "EMAIL_EXISTS")
        return "Email already exists, please use another";
    else if(err == "OPERATION_NOT_ALLOWED")
        return "Password sign-in disabled for this project";
    else if(err == "TOO_MANY_ATTEMPTS_TRY_LATER")
        return "User account disabled, contact administrator";
    else if(err == "USER_DISABLED")
        return "User account disabled, contact administrator";

    // Sign in with email/password
    else if(err == "EMAIL_NOT_FOUND" || err == "INVALID_EMAIL")
        return "Email was not found, verify if it is correct";
    else if(err == "USER_DISABLED")
        return "User account disabled, contact administrator";
    else if(err == "INVALID_PASSWORD")
        return "Password is invalid";

    // Change email
    else if(err == "INVALID_ID_TOKEN")
        return "User credential not valid, sign in again";
    // Change password
    else if(err == "WEAK_PASSWORD")
        return "Password must be 6 characters long or more";

    // Get user profile data
    else if(err == "USER_NOT_FOUND")
        return "User account disabled, contact administrator";

    // Return back same string if it isn't in this list
    else
        return err;
}

}

#endif // AUTHUTILS_H
