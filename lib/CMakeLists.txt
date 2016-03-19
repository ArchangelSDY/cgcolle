set(LIB_SRCS
	"CodecUtils.cpp"
	"ImageDemux.cpp"
    "ImageEntry.cpp"
)

add_library(cgcolle_lib STATIC ${LIB_SRCS})
