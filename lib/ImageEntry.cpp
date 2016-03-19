#include "ImageEntry.h"

using namespace cgcolle;

ImageEntry::ImageEntry(const std::string &name, const std::string &path) :
    name(name) ,
    path(path) ,
    mainFrameId(0) ,
    size(0) ,
    offset(0)
{
}