# QmlFirebase
Firebase REST API based components compatible with Qt and QML.

## Why does this exist
You can find the much better written [QtFirebase](https://github.com/Larpon/QtFirebase) project, however it only has support for Android/IOS. You can also use Google's official [C++ Firebase API](https://firebase.google.com/docs/reference/cpp) but it has many dependencies and I struggled to get it to work on smaller embedded systems such as the Raspberry Pi. 

By using the REST API instead in conjunction with Qt's powerfull cross platform framework you end up with classes that will work on any system that can run Qt.

### Dependencies
- [Qt Network Authorization module](https://doc.qt.io/qt-5/qtnetworkauth-index.html)

### Usage
1. Download the repository contents into a path of youre choosing.
2. Add the line `include (<path/to/QmlFirebase>/QmlFirebase.pri)` in your .pro.
3. Add `import Firebase 1.0` to use the module on your QML files.


### Features implemented
- Ability to signup/signin and update/delete the firebase user profile.
- Ability to authenticate using a Google account (sometimes doesn't work on some browsers, not entirely sure why).
- Add, update and remove data from Firebase Database.
- Can register listeners to specific locations on the database, when data is changed a signal will be emitted.
- For simplicity sake, made the REST API requests work asynchronously so no threads are needed.

### Useful links
1. [Firebase Auth REST](https://firebase.google.com/docs/reference/rest/auth) - Has all the endpoints for authentication requests and details the payload formats.
2. [Firebase Database REST](https://firebase.google.com/docs/reference/rest/database) - Has all the endpoints for adding/deleting/updating data on the Firebase Realtime Database.
3. [Pyrebase](https://github.com/thisbejim/Pyrebase) - Also REST API based, written in python, useful if using Qt for python.
4. [Qt Firebase REST API](https://github.com/Sriep/Qt_Firebase_REST_API.git) - Main inspiration, works well but doesn't have any QML support (which led to the making of this one).

### Documentation
Documentation can be found in [here](https://antonio-real.github.io/QmlFirebase/).

### Support
If people use this at all I can continue improving the code (which needs lots of refactoring anyways).
Feel free to raise any issues or pull requests.
