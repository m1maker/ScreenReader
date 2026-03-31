target_sources(${PROJECT_NAME}
    PRIVATE
        Speech.cpp
    PUBLIC FILE_SET CXX_MODULES FILES
        Speech.cppm
)