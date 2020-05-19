/****************************************************************************
** Meta object code from reading C++ file 'HistogramWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../Widget/HistogramWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HistogramWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HistogramWidget_t {
    QByteArrayData data[10];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HistogramWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HistogramWidget_t qt_meta_stringdata_HistogramWidget = {
    {
QT_MOC_LITERAL(0, 0, 15), // "HistogramWidget"
QT_MOC_LITERAL(1, 16, 12), // "resetControl"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 12), // "updateBottom"
QT_MOC_LITERAL(4, 43, 6), // "bottom"
QT_MOC_LITERAL(5, 50, 9), // "updateMid"
QT_MOC_LITERAL(6, 60, 3), // "mid"
QT_MOC_LITERAL(7, 64, 9), // "updateTop"
QT_MOC_LITERAL(8, 74, 3), // "top"
QT_MOC_LITERAL(9, 78, 10) // "clickReset"

    },
    "HistogramWidget\0resetControl\0\0"
    "updateBottom\0bottom\0updateMid\0mid\0"
    "updateTop\0top\0clickReset"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HistogramWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    1,   40,    2, 0x06 /* Public */,
       5,    1,   43,    2, 0x06 /* Public */,
       7,    1,   46,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    4,
    QMetaType::Void, QMetaType::Float,    6,
    QMetaType::Void, QMetaType::Float,    8,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void HistogramWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HistogramWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resetControl(); break;
        case 1: _t->updateBottom((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 2: _t->updateMid((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 3: _t->updateTop((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 4: _t->clickReset(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HistogramWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HistogramWidget::resetControl)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (HistogramWidget::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HistogramWidget::updateBottom)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (HistogramWidget::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HistogramWidget::updateMid)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (HistogramWidget::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HistogramWidget::updateTop)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject HistogramWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_HistogramWidget.data,
    qt_meta_data_HistogramWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HistogramWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HistogramWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HistogramWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int HistogramWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void HistogramWidget::resetControl()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void HistogramWidget::updateBottom(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HistogramWidget::updateMid(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void HistogramWidget::updateTop(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
