set(ENCODER_SRCS
    "ImageDiffGenerator.cpp"
    "ImageMux.cpp"
    "NameImageGrouper.cpp"
    "main.cpp"
)

add_executable(cgcolle_encoder ${ENCODER_SRCS})
target_link_libraries(cgcolle_encoder cgcolle_lib ${Boost_LIBRARIES} ${OpenCV_LIBRARIES})