#ifndef OBJ_CUSTOM_PROPERTY_H
#define OBJ_CUSTOM_PROPERTY_H

#include <QString>
#include <QMap>
#include <QSharedPointer>


struct custom_property_enum {
    QString text;
    // TODO: Add icon support
    // QPixmap icon;
};

struct custom_property_input_extra_base {};


struct custom_property_item {
    QString text;
    QString description;
    QList<QSharedPointer<custom_property_input_extra_base>> input_extra;
};

struct custom_property_input_extra_spin_box : custom_property_input_extra_base {
    int min;
    int max;
    int step;
};

struct custom_property_input_extra_slider : custom_property_input_extra_base {
    int min;
    int max;
    int step;
};

struct custom_property_input_extra_check_box : custom_property_input_extra_base {};

struct custom_property_input_extra_double_spin_box : custom_property_input_extra_base {
    int min;
    int max;
    int step;
    int decimals;
};

struct custom_property_input_extra_line_edit : custom_property_input_extra_base {
    QString regex; // TODO: Validate RegEx
};

struct custom_property_input_extra_enum_edit : custom_property_input_extra_base {
    QString enum_name;
};

struct custom_property_input_extra_color_edit : custom_property_input_extra_base {};

struct custom_property_input_extra_group : custom_property_input_extra_base {
    QMap<QString, custom_property_item> properties;
};


struct obj_custom_property
{
    int npc_id;
    QMap<QString, custom_property_item> properties;
    QMap<QString, QMap<QString, custom_property_enum>> enums;
};




#endif // OBJ_CUSTOM_PROPERTY_H
