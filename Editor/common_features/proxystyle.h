#pragma once
#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QProxyStyle>
#include <QAbstractItemView>
#include <QComboBox>

class PGE_ProxyStyle : public QProxyStyle
{

public:
    int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const;
};

#endif // PROXYSTYLE_H
