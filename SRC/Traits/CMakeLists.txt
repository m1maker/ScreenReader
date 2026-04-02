target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES
        EventListener.cppm
        PlatformDependentWorker.cppm
        RefCountedObject.cppm
)