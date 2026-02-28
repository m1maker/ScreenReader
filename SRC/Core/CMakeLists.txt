target_sources(ScreenReader
    PRIVATE
        EventHandler.cpp
        EventToSpeech.cpp
        KeyboardHandler.cpp
        main.cpp
    PUBLIC FILE_SET CXX_MODULES FILES
        Timer.cppm
)