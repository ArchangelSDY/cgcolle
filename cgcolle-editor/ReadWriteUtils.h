#ifndef READWRITEUTILS_H
#define READWRITEUTILS_H

#define readNumber(device, dst) \
    device->read((char *)&dst, sizeof(dst));

#define readString(device, dst) \
    { \
        uint32_t nameSize = 0; \
        readNumber(nameSize); \
        QByteArray nameBuf = device->read(nameSize); \
        dst = QString::fromUtf8(nameBuf); \
    }

#define writeNumber(device, number) \
    device->write((const char *)(&number), sizeof(number));

#define writeString(device, str) \
    { \
        QByteArray buf = str.toUtf8(); \
        int size = buf.size(); \
        writeNumber(device, size); \
        device->write(buf.data(), buf.size()); \
    }

#endif // READWRITEUTILS_H
