/****************************************************************************
** Meta object code from reading C++ file 'PythonQt.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PythonQt.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PythonQt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PythonQt_t {
    QByteArrayData data[9];
    char stringdata0[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PythonQt_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PythonQt_t qt_meta_stringdata_PythonQt = {
    {
QT_MOC_LITERAL(0, 0, 8), // "PythonQt"
QT_MOC_LITERAL(1, 9, 12), // "pythonStdOut"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 3), // "str"
QT_MOC_LITERAL(4, 27, 12), // "pythonStdErr"
QT_MOC_LITERAL(5, 40, 17), // "pythonHelpRequest"
QT_MOC_LITERAL(6, 58, 12), // "cppClassName"
QT_MOC_LITERAL(7, 71, 25), // "systemExitExceptionRaised"
QT_MOC_LITERAL(8, 97, 8) // "exitCode"

    },
    "PythonQt\0pythonStdOut\0\0str\0pythonStdErr\0"
    "pythonHelpRequest\0cppClassName\0"
    "systemExitExceptionRaised\0exitCode"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PythonQt[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       5,    1,   40,    2, 0x06 /* Public */,
       7,    1,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QByteArray,    6,
    QMetaType::Void, QMetaType::Int,    8,

       0        // eod
};

void PythonQt::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PythonQt *_t = static_cast<PythonQt *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pythonStdOut((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->pythonStdErr((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->pythonHelpRequest((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 3: _t->systemExitExceptionRaised((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PythonQt::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PythonQt::pythonStdOut)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (PythonQt::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PythonQt::pythonStdErr)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (PythonQt::*_t)(const QByteArray & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PythonQt::pythonHelpRequest)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (PythonQt::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PythonQt::systemExitExceptionRaised)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject PythonQt::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PythonQt.data,
      qt_meta_data_PythonQt,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PythonQt::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PythonQt::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PythonQt.stringdata0))
        return static_cast<void*>(const_cast< PythonQt*>(this));
    return QObject::qt_metacast(_clname);
}

int PythonQt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void PythonQt::pythonStdOut(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PythonQt::pythonStdErr(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PythonQt::pythonHelpRequest(const QByteArray & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PythonQt::systemExitExceptionRaised(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_PythonQtPrivate_t {
    QByteArrayData data[1];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PythonQtPrivate_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PythonQtPrivate_t qt_meta_stringdata_PythonQtPrivate = {
    {
QT_MOC_LITERAL(0, 0, 15) // "PythonQtPrivate"

    },
    "PythonQtPrivate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PythonQtPrivate[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void PythonQtPrivate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject PythonQtPrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PythonQtPrivate.data,
      qt_meta_data_PythonQtPrivate,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PythonQtPrivate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PythonQtPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PythonQtPrivate.stringdata0))
        return static_cast<void*>(const_cast< PythonQtPrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int PythonQtPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
