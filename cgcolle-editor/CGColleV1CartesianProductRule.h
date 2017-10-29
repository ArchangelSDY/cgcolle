#ifndef CGCOLLEV1CARTESIANPRODUCTRULE_H
#define CGCOLLEV1CARTESIANPRODUCTRULE_H

#include "CGColleV1CompositeRule.h"

#include <QList>

class CGColleV1CartesianProductRule : public CGColleV1CompositeRule
{
public:
    static const int TYPE = 0;

    CGColleV1CartesianProductRule();

    virtual bool read(QIODevice *device) override;
    virtual bool write(QIODevice *device) override;
    virtual QString toEditString() override;
    virtual bool fromEditString(const QString &raw) override;

    QList<QPair<uint8_t, uint8_t> > &layerRanges();

private:
    QList<QPair<uint8_t, uint8_t> > m_layerRanges;
};

#endif // CGCOLLEV1CARTESIANPRODUCTRULE_H
