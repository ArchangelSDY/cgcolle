#include "CGColleV1CartesianProductRule.h"

#include <QTextStream>

CGColleV1CartesianProductRule::CGColleV1CartesianProductRule()
{
}

int CGColleV1CartesianProductRule::type() const
{
    return CGColleV1CartesianProductRule::TYPE;
}

bool CGColleV1CartesianProductRule::read(QIODevice *device)
{
#define readNumber(dst) \
    device->read((char *)&dst, sizeof(dst));

    uint8_t count;
    readNumber(count);

    m_layerRanges.clear();
    for (uint8_t j = 0; j < count; ++j) {
        uint8_t min;
        readNumber(min);
        uint8_t max;
        readNumber(max);
        m_layerRanges << QPair<uint8_t, uint8_t>(min, max);
    }

    return true;
}

bool CGColleV1CartesianProductRule::write(QIODevice *device)
{
#define writeNumber(number) \
    device->write((const char *)(&number), sizeof(number));

#define writeString(str) \
    { \
        QByteArray buf = str.toUtf8(); \
        int size = buf.size(); \
        writeNumber(size); \
        device->write(buf.data(), buf.size()); \
    }

    writeString(m_matcher);

    static const uint8_t type = CGColleV1CartesianProductRule::TYPE;
    writeNumber(type);

    uint8_t count = m_layerRanges.count();
    writeNumber(count);

    for (auto pair : m_layerRanges) {
        uint8_t min = pair.first;
        uint8_t max = pair.second;
        writeNumber(min);
        writeNumber(max);
    }

    return true;
}

QString CGColleV1CartesianProductRule::toEditString()
{
    QString str;
    QTextStream stream(&str, QIODevice::WriteOnly);
    for (const auto &layer : m_layerRanges) {
        stream << QString::number(layer.first) << "," << QString::number(layer.second) << "\n";
    }
    return str;
}

bool CGColleV1CartesianProductRule::fromEditString(const QString &raw)
{
    QStringList layerStrings = raw.split('\n');
    m_layerRanges.clear();
    for (const QString &layer : layerStrings) {
        QStringList parts = layer.split(',');
        if (parts.count() != 2) {
            continue;
        }
        int min = parts[0].toInt();
        int max = parts[1].toInt();
        m_layerRanges << QPair<uint8_t, uint8_t>(min, max);
    }
    return true;
}

QList<QPair<uint8_t, uint8_t> > &CGColleV1CartesianProductRule::layerRanges()
{
    return m_layerRanges;
}
