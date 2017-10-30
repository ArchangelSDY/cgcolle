#ifndef CGCOLLEV1STATICRULE_H
#define CGCOLLEV1STATICRULE_H

#include <QVector>

#include "CGColleV1CompositeRule.h"

class CGColleV1StaticRule : public CGColleV1CompositeRule
{
public:
    static const int TYPE = 1;

    CGColleV1StaticRule();

    virtual bool read(QIODevice *device) override;
    virtual bool write(QIODevice *device) override;
    virtual QString toEditString() override;
    virtual bool fromEditString(const QString &raw) override;

    QVector<QVector<uint32_t> > &plans();

private:
    QVector<QVector<uint32_t> > m_plans;
};

#endif // CGCOLLEV1STATICRULE_H
