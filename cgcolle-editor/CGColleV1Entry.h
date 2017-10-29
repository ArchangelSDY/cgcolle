#ifndef CGCOLLEV1ENTRY_H
#define CGCOLLEV1ENTRY_H

#include <QString>
#include <QFlag>

class CGColleV1Entry
{
public:
    enum class Type
    {
        BaseImage = 0,
        LayerImage = 1,
    };

    enum class Flag
    {
        NoFlag = 0x0,
    };
    Q_DECLARE_FLAGS(Flags, Flag);

    Type type;
    int dataOffset;
    QString scene;
    QString name;
    uint32_t fileSize;
    uint32_t width;
    uint32_t height;
    uint32_t offsetX;
    uint32_t offsetY;
    uchar layerId;
    uchar compositionMethod;
    Flags flags;

    CGColleV1Entry();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CGColleV1Entry::Flags)

#endif // CGCOLLEV1ENTRY_H
