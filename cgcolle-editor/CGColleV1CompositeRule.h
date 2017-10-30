#ifndef CGCOLLEV1COMPOSITERULE_H
#define CGCOLLEV1COMPOSITERULE_H

#include <QIODevice>
#include <QString>

class CGColleV1CompositeRule
{
public:
    virtual ~CGColleV1CompositeRule();

    virtual int type() const = 0;
    virtual bool read(QIODevice *device) = 0;
    virtual bool write(QIODevice *device) = 0;
    virtual QString toEditString() = 0;
    virtual bool fromEditString(const QString &raw) = 0;

    QString matcher() const;
    void setMatcher(const QString &matcher);

protected:
    QString m_matcher;
};

#endif // CGCOLLEV1COMPOSITERULE_H
