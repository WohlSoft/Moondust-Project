#pragma once
#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QProxyStyle>
#include <QAbstractItemView>
#include <QComboBox>

class PGE_ProxyStyle : public QProxyStyle
{
public:
    PGE_ProxyStyle(QStyle *style = Q_NULLPTR);
    PGE_ProxyStyle(const QString &key);
    ~PGE_ProxyStyle();

    int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const;
};

#endif // PROXYSTYLE_H
