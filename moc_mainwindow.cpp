/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "ui/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[18];
    char stringdata0[305];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 17), // "updateAspectRatio"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 15), // "settingsChanged"
QT_MOC_LITERAL(4, 46, 8), // "fileOpen"
QT_MOC_LITERAL(5, 55, 16), // "activateCanvas3D"
QT_MOC_LITERAL(6, 72, 14), // "setCameraAxisX"
QT_MOC_LITERAL(7, 87, 14), // "setCameraAxisY"
QT_MOC_LITERAL(8, 102, 14), // "setCameraAxisZ"
QT_MOC_LITERAL(9, 117, 23), // "updateCameraTranslation"
QT_MOC_LITERAL(10, 141, 21), // "updateCameraRotationN"
QT_MOC_LITERAL(11, 163, 21), // "updateCameraRotationV"
QT_MOC_LITERAL(12, 185, 21), // "updateCameraRotationU"
QT_MOC_LITERAL(13, 207, 13), // "resetUpVector"
QT_MOC_LITERAL(14, 221, 16), // "updateCameraClip"
QT_MOC_LITERAL(15, 238, 23), // "updateCameraHeightAngle"
QT_MOC_LITERAL(16, 262, 20), // "setCameraAxonometric"
QT_MOC_LITERAL(17, 283, 21) // "loadLSystemFileButton"

    },
    "MainWindow\0updateAspectRatio\0\0"
    "settingsChanged\0fileOpen\0activateCanvas3D\0"
    "setCameraAxisX\0setCameraAxisY\0"
    "setCameraAxisZ\0updateCameraTranslation\0"
    "updateCameraRotationN\0updateCameraRotationV\0"
    "updateCameraRotationU\0resetUpVector\0"
    "updateCameraClip\0updateCameraHeightAngle\0"
    "setCameraAxonometric\0loadLSystemFileButton"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x0a /* Public */,
       3,    0,   95,    2, 0x0a /* Public */,
       4,    0,   96,    2, 0x0a /* Public */,
       5,    0,   97,    2, 0x0a /* Public */,
       6,    0,   98,    2, 0x0a /* Public */,
       7,    0,   99,    2, 0x0a /* Public */,
       8,    0,  100,    2, 0x0a /* Public */,
       9,    0,  101,    2, 0x0a /* Public */,
      10,    0,  102,    2, 0x0a /* Public */,
      11,    0,  103,    2, 0x0a /* Public */,
      12,    0,  104,    2, 0x0a /* Public */,
      13,    0,  105,    2, 0x0a /* Public */,
      14,    0,  106,    2, 0x0a /* Public */,
      15,    0,  107,    2, 0x0a /* Public */,
      16,    0,  108,    2, 0x0a /* Public */,
      17,    0,  109,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateAspectRatio(); break;
        case 1: _t->settingsChanged(); break;
        case 2: _t->fileOpen(); break;
        case 3: _t->activateCanvas3D(); break;
        case 4: _t->setCameraAxisX(); break;
        case 5: _t->setCameraAxisY(); break;
        case 6: _t->setCameraAxisZ(); break;
        case 7: _t->updateCameraTranslation(); break;
        case 8: _t->updateCameraRotationN(); break;
        case 9: _t->updateCameraRotationV(); break;
        case 10: _t->updateCameraRotationU(); break;
        case 11: _t->resetUpVector(); break;
        case 12: _t->updateCameraClip(); break;
        case 13: _t->updateCameraHeightAngle(); break;
        case 14: _t->setCameraAxonometric(); break;
        case 15: _t->loadLSystemFileButton(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
