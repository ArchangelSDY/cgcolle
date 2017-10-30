#include "CGColleV1StaticRule.h"

#include <QTextStream>

#include "ReadWriteUtils.h"

CGColleV1StaticRule::CGColleV1StaticRule()
{
}

int CGColleV1StaticRule::type() const
{
    return CGColleV1StaticRule::TYPE;
}

bool CGColleV1StaticRule::read(QIODevice *device)
{
    uint32_t count;
    readNumber(device, count);

    m_plans.clear();
    for (uint32_t i = 0; i < count; ++i) {
        uint8_t size;
        readNumber(device, size);

        QVector<uint32_t> plan(size);
        for (uint8_t j = 0; j < size; j++) {
            uint32_t index;
            readNumber(device, index);
            plan[j] = index;
        }

        m_plans << plan;
    }

    return true;
}

bool CGColleV1StaticRule::write(QIODevice *device)
{
    writeString(device, m_matcher);

    static const uint8_t type = CGColleV1StaticRule::TYPE;
    writeNumber(device, type);

    uint32_t count = m_plans.count();
    writeNumber(device, count);

    for (const auto &plan : m_plans) {
        uint8_t size = plan.size();
        writeNumber(device, size);

        for (const uint32_t &index : plan) {
            writeNumber(device, index);
        }
    }

    return true;
}

QString CGColleV1StaticRule::toEditString()
{
    QString str;
    QTextStream stream(&str, QIODevice::WriteOnly);
    for (const auto &plan : m_plans) {
        QStringList planStrings;
        for (const int &index : plan) {
            planStrings << QString::number(index);
        }
        stream << planStrings.join(",") << "\n";
    }

    return str;
}

bool CGColleV1StaticRule::fromEditString(const QString &raw)
{
    QStringList planStrings = raw.split('\n');
    m_plans.clear();
    for (const QString &planString : planStrings) {
        QStringList indexStrings = planString.split(',');
        if (indexStrings.isEmpty()) {
            continue;
        }
        QVector<uint32_t> plan(indexStrings.size());
        for (int i = 0; i < plan.size(); i++) {
            plan[i] = indexStrings[i].toInt();
        }
        m_plans << plan;
    }
    return true;
}

QVector<QVector<uint32_t> > &CGColleV1StaticRule::plans()
{
    return m_plans;
}
