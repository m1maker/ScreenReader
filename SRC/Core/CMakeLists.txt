target_sources(ScreenReader
    PRIVATE
        EventHandler.cpp
        EventToSpeech.cpp
        KeyboardHandler.cpp
        main.cpp
    PUBLIC FILE_SET CXX_MODULES FILES
        Action.cppm
        App.cppm
        AppState.cppm
        Config.cppm
        Device.cppm
        EventQueue.cppm
        FocusManager.cppm
        Timer.cppm
)