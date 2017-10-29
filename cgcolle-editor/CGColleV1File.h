#ifndef CGCOLLEV1FILE_H
#define CGCOLLEV1FILE_H

#include <QFileDevice>
#include <QScopedPointer>

#include "CGColleV1Entry.h"
#include "CGColleV1CompositeRule.h"

class CGColleV1File
{
public:
    CGColleV1File(const QString &path);
    ~CGColleV1File();

    bool open();
    bool save();
    QList<CGColleV1Entry> &entrys();
    QList<CGColleV1CompositeRule *> &compositeRules();
    QImage readImage(int index);

    uint32_t dataStart() const;
    uint32_t dataLength() const;
    uint32_t metaStart() const;
    uint32_t metaLength() const;

private:
    bool scan();
    bool scanMeta();
    bool scanCompositeRules();
    bool writeMeta();
    bool writeCompositeRules();

    QScopedPointer<QFileDevice> m_device;
    QList<CGColleV1Entry> m_entries;
    QList<CGColleV1CompositeRule *> m_compositeRules;
    uint32_t m_dataStart;
    uint32_t m_dataLength;
    uint32_t m_metaStart;
    uint32_t m_metaLength;
};

#endif // CGCOLLEV1FILE_H
