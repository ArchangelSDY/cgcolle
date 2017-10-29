#include "CGColleV1File.h"

#include <QFile>
#include <QImage>

#include "CGColleV1CartesianProductRule.h"

CGColleV1File::CGColleV1File(const QString &path) :
    m_device(new QFile(path))
{

}

CGColleV1File::~CGColleV1File()
{
    for (CGColleV1CompositeRule *rule : m_compositeRules) {
        delete rule;
    }
}

bool CGColleV1File::open()
{
    if (!m_device->open(QIODevice::ReadWrite)) {
        return false;
    }

    uchar *ptr = m_device->map(0, 4);
    static const char CGC[4] = { 'C','G', 'C', '1' };
    if (qstrncmp((const char *)ptr, CGC, 4) != 0) {
        return false;
    }

    if (!scan()) {
        return false;
    }

    return true;
}

bool CGColleV1File::save()
{
    m_device->seek(m_metaStart - 8);

    if (!writeMeta()) {
        return false;
    }

    if (!writeCompositeRules()) {
        return false;
    }

    m_device->resize(m_device->pos());

    return true;
}

QList<CGColleV1Entry> &CGColleV1File::entrys()
{
    return m_entries;
}

QList<CGColleV1CompositeRule *> &CGColleV1File::compositeRules()
{
    return m_compositeRules;
}

QImage CGColleV1File::readImage(int index)
{
    const CGColleV1Entry &entry = m_entries[index];

    m_device->seek(m_dataStart + entry.dataOffset);
    return QImage::fromData(m_device->read(entry.fileSize));
}

#define readNumber(dst) \
    m_device->read((char *)&dst, sizeof(dst));
#define readString(dst) \
    { \
        uint32_t nameSize = 0; \
        readNumber(nameSize); \
        QByteArray nameBuf = m_device->read(nameSize); \
        dst = QString::fromUtf8(nameBuf); \
    }

bool CGColleV1File::scan()
{
    // Skip header
    m_device->seek(8);

    while (!m_device->atEnd()) {
        QByteArray chunkType = m_device->read(4);
        if (chunkType == "META") {
            if (!scanMeta()) {
                return false;
            }
        } else if (chunkType == "CPRL") {
            if (!scanCompositeRules()) {
                return false;
            }
        } else if (chunkType == "DATA") {
            uint32_t chunkLength = 0;
            readNumber(chunkLength);

            m_dataStart = m_device->pos();
            m_dataLength = chunkLength;

            // Skip data for now
            m_device->seek(m_device->pos() + chunkLength);
        } else {
            // Unknown chunk, skip
            uint32_t chunkLength = 0;
            readNumber(chunkLength);
            m_device->seek(m_device->pos() + chunkLength);
        }
    }

    return true;
}

bool CGColleV1File::scanMeta()
{
    uint32_t metaLength = 0;
    readNumber(metaLength);

    m_metaStart = m_device->pos();
    m_metaLength = metaLength;

    uint32_t imagesCount = 0;
    readNumber(imagesCount);

    int dataOffset = 0;
    for (uint32_t i = 0; i < imagesCount; ++i) {
        CGColleV1Entry entry;

        readNumber(entry.type);

        readString(entry.scene);
        readString(entry.name);
        readNumber(entry.fileSize);
        readNumber(entry.width);
        readNumber(entry.height);
        readNumber(entry.offsetX);
        readNumber(entry.offsetY);
        readNumber(entry.layerId);
        readNumber(entry.compositionMethod);
        readNumber(entry.flags);

        entry.dataOffset = dataOffset;

        m_entries << entry;

        dataOffset += entry.fileSize;
    }

    return true;
}

bool CGColleV1File::scanCompositeRules()
{
    qint64 cprlStart = m_device->pos();

    uint32_t cprlLength = 0;
    readNumber(cprlLength);

    uint32_t rulesCount = 0;
    readNumber(rulesCount);

    for (uint32_t i = 0; i < rulesCount; ++i) {
        QString matcher;
        readString(matcher);

        uint8_t type;
        readNumber(type);

        if (type == CGColleV1CartesianProductRule::TYPE) {
            // Cartesian Product Rule
            CGColleV1CompositeRule *rule = new CGColleV1CartesianProductRule();
            rule->setMatcher(matcher);
            if (!rule->read(m_device.data())) {
                return false;
            }

            m_compositeRules << rule;
        } else {
            // Unknown rule
            // Skip all remaining CPRL chunk data
            m_device->seek(cprlStart + 8 + cprlLength);
            return true;
        }
    }

    return true;
}

#define writeNumber(number) \
    m_device->write((const char *)(&number), sizeof(number));

#define writeString(str) \
    { \
        QByteArray buf = str.toUtf8(); \
        int size = buf.size(); \
        writeNumber(size); \
        m_device->write(buf.data(), buf.size()); \
    }

bool CGColleV1File::writeMeta()
{
    qint64 start = m_device->pos();
    m_device->write("META");

    uint32_t metaLength = 0;
    writeNumber(metaLength);

    uint32_t count = m_entries.count();
    writeNumber(count);

    for (const auto &entry : m_entries) {
        writeNumber(entry.type);
        writeString(entry.scene);
        writeString(entry.name);
        writeNumber(entry.fileSize);
        writeNumber(entry.width);
        writeNumber(entry.height);
        writeNumber(entry.offsetX);
        writeNumber(entry.offsetY);
        writeNumber(entry.layerId);
        writeNumber(entry.compositionMethod);
        writeNumber(entry.flags);
    }

    qint64 pos = m_device->pos();
    m_device->seek(start + 4);
    metaLength = pos - start - 8;
    writeNumber(metaLength);
    m_device->seek(pos);

    return true;
}

bool CGColleV1File::writeCompositeRules()
{
    qint64 start = m_device->pos();
    m_device->write("CPRL");

    uint32_t cprlLength = 0;
    writeNumber(cprlLength);

    uint32_t count = m_compositeRules.count();
    writeNumber(count);

    for (CGColleV1CompositeRule *rule : m_compositeRules) {
        rule->write(m_device.data());
    }

    qint64 pos = m_device->pos();
    m_device->seek(start + 4);
    cprlLength = pos - start - 8;
    writeNumber(cprlLength);
    m_device->seek(pos);

    return true;
}

uint32_t CGColleV1File::dataStart() const
{
    return m_dataStart;
}

uint32_t CGColleV1File::dataLength() const
{
    return m_dataLength;
}

uint32_t CGColleV1File::metaStart() const
{
    return m_metaStart;
}

uint32_t CGColleV1File::metaLength() const
{
    return m_metaLength;
}
