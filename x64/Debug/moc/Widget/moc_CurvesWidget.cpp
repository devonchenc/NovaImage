/****************************************************************************
** Meta object code from reading C++ file 'CurvesWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../Widget/CurvesWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CurvesWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CurvesWidget_t {
    QByteArrayData data[15];
    char stringdata0[168];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CurvesWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CurvesWidget_t qt_meta_stringdata_CurvesWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CurvesWidget"
QT_MOC_LITERAL(1, 13, 14), // "channelChanged"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 5), // "value"
QT_MOC_LITERAL(4, 35, 10), // "clickReset"
QT_MOC_LITERAL(5, 46, 12), // "clickReverse"
QT_MOC_LITERAL(6, 59, 16), // "toggleCurveRadio"
QT_MOC_LITERAL(7, 76, 17), // "toggleLinearRadio"
QT_MOC_LITERAL(8, 94, 9), // "clickSave"
QT_MOC_LITERAL(9, 104, 9), // "clickLoad"
QT_MOC_LITERAL(10, 114, 12), // "resizeSquare"
QT_MOC_LITERAL(11, 127, 11), // "updateImage"
QT_MOC_LITERAL(12, 139, 15), // "updateLabelText"
QT_MOC_LITERAL(13, 155, 5), // "input"
QT_MOC_LITERAL(14, 161, 6) // "output"

    },
    "CurvesWidget\0channelChanged\0\0value\0"
    "clickReset\0clickReverse\0toggleCurveRadio\0"
    "toggleLinearRadio\0clickSave\0clickLoad\0"
    "resizeSquare\0updateImage\0updateLabelText\0"
    "input\0output"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CurvesWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x0a /* Public */,
       4,    0,   67,    2, 0x0a /* Public */,
       5,    0,   68,    2, 0x0a /* Public */,
       6,    0,   69,    2, 0x0a /* Public */,
       7,    0,   70,    2, 0x0a /* Public */,
       8,    0,   71,    2, 0x0a /* Public */,
       9,    0,   72,    2, 0x0a /* Public */,
      10,    0,   73,    2, 0x09 /* Protected */,
      11,    0,   74,    2, 0x09 /* Protected */,
      12,    2,   75,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   13,   14,

       0        // eod
};

void CurvesWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CurvesWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->channelChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->clickReset(); break;
        case 2: _t->clickReverse(); break;
        case 3: _t->toggleCurveRadio(); break;
        case 4: _t->toggleLinearRadio(); break;
        case 5: _t->clickSave(); break;
        case 6: _t->clickLoad(); break;
        case 7: _t->resizeSquare(); break;
        case 8: _t->updateImage(); break;
        case 9: _t->updateLabelText((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CurvesWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseWidget::staticMetaObject>(),
    qt_meta_stringdata_CurvesWidget.data,
    qt_meta_data_CurvesWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CurvesWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CurvesWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CurvesWidget.stringdata0))
        return static_cast<void*>(this);
    return BaseWidget::qt_metacast(_clname);
}

int CurvesWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
