/****************************************************************************
** Meta object code from reading C++ file 'PythonQtScriptingConsole.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/PythonQtScriptingConsole.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PythonQtScriptingConsole.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PythonQtScriptingConsole_t {
    QByteArrayData data[19];
    char stringdata0[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PythonQtScriptingConsole_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PythonQtScriptingConsole_t qt_meta_stringdata_PythonQtScriptingConsole = {
    {
QT_MOC_LITERAL(0, 0, 24), // "PythonQtScriptingConsole"
QT_MOC_LITERAL(1, 25, 11), // "executeLine"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 9), // "storeOnly"
QT_MOC_LITERAL(4, 48, 13), // "keyPressEvent"
QT_MOC_LITERAL(5, 62, 10), // "QKeyEvent*"
QT_MOC_LITERAL(6, 73, 1), // "e"
QT_MOC_LITERAL(7, 75, 14), // "consoleMessage"
QT_MOC_LITERAL(8, 90, 7), // "message"
QT_MOC_LITERAL(9, 98, 7), // "history"
QT_MOC_LITERAL(10, 106, 10), // "setHistory"
QT_MOC_LITERAL(11, 117, 1), // "h"
QT_MOC_LITERAL(12, 119, 5), // "clear"
QT_MOC_LITERAL(13, 125, 3), // "cut"
QT_MOC_LITERAL(14, 129, 6), // "stdOut"
QT_MOC_LITERAL(15, 136, 1), // "s"
QT_MOC_LITERAL(16, 138, 6), // "stdErr"
QT_MOC_LITERAL(17, 145, 16), // "insertCompletion"
QT_MOC_LITERAL(18, 162, 19) // "appendCommandPrompt"

    },
    "PythonQtScriptingConsole\0executeLine\0"
    "\0storeOnly\0keyPressEvent\0QKeyEvent*\0"
    "e\0consoleMessage\0message\0history\0"
    "setHistory\0h\0clear\0cut\0stdOut\0s\0stdErr\0"
    "insertCompletion\0appendCommandPrompt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PythonQtScriptingConsole[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x0a /* Public */,
       4,    1,   77,    2, 0x0a /* Public */,
       7,    1,   80,    2, 0x0a /* Public */,
       9,    0,   83,    2, 0x0a /* Public */,
      10,    1,   84,    2, 0x0a /* Public */,
      12,    0,   87,    2, 0x0a /* Public */,
      13,    0,   88,    2, 0x0a /* Public */,
      14,    1,   89,    2, 0x0a /* Public */,
      16,    1,   92,    2, 0x0a /* Public */,
      17,    1,   95,    2, 0x0a /* Public */,
      18,    1,   98,    2, 0x0a /* Public */,
      18,    0,  101,    2, 0x2a /* Public | MethodCloned */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::QStringList,
    QMetaType::Void, QMetaType::QStringList,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

       0        // eod
};

void PythonQtScriptingConsole::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PythonQtScriptingConsole *_t = static_cast<PythonQtScriptingConsole *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->executeLine((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 2: _t->consoleMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: { QStringList _r = _t->history();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        case 4: _t->setHistory((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 5: _t->clear(); break;
        case 6: _t->cut(); break;
        case 7: _t->stdOut((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->stdErr((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->insertCompletion((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->appendCommandPrompt((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->appendCommandPrompt(); break;
        default: ;
        }
    }
}

const QMetaObject PythonQtScriptingConsole::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_PythonQtScriptingConsole.data,
      qt_meta_data_PythonQtScriptingConsole,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PythonQtScriptingConsole::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PythonQtScriptingConsole::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PythonQtScriptingConsole.stringdata0))
        return static_cast<void*>(const_cast< PythonQtScriptingConsole*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int PythonQtScriptingConsole::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
