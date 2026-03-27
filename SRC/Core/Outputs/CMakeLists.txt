target_sources(ScreenReader
    PRIVATE
        Speech.cpp
    PUBLIC FILE_SET CXX_MODULES FILES
        Speech.cppm
)