target_sources(${PROJECT_NAME}
    PRIVATE
        EventListenerAtspi.cpp
        ObjectAtspi.cpp
        UinputDevice.cpp
    PUBLIC FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES
        EventListenerAtspi.cppm
        ObjectAtspi.cppm
        PlatformDependentWorkerLinux.cppm
        UinputDevice.cppm
)