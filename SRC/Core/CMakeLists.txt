#
# This file is part of the Screen Reader project.
#
# Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

target_sources(${PROJECT_NAME}
    PRIVATE
        AudioEnginePortAudio.cpp
        AudioSystem.cpp
        EventHandler.cpp
        KeyboardHandler.cpp
        MessageBuilder.cpp
        main.cpp
        Object.cpp
        ObjectHandler.cpp
        OutputManager.cpp
        SpeechEngineEspeakNg.cpp
        SpeechSystem.cpp
        Utterance.cpp
    PUBLIC FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES
        Action.cppm
        App.cppm
        AppState.cppm
        Audio.cppm
        AudioEnginePortAudio.cppm
        AudioSystem.cppm
        CapabilityMeta.cppm
        Config.cppm
        Device.cppm
        Encoding.cppm
        Environment.cppm
        Event.cppm
        EventHandler.cppm
        EventQueue.cppm
        FocusManager.cppm
        KeyboardHandler.cppm
        KeyInfo.cppm
        KeyMeta.cppm
        Logger.cppm
        Message.cppm
        MessageBuilder.cppm
        Object.cppm
        ObjectHandler.cppm
        ObjectMeta.cppm
        ObjectNavigator.cppm
        OutputManager.cppm
        Outputs.cppm
        PlatformError.cppm
        Rect.cppm
        Rotor.cppm
        Singleton.cppm
        Speech.cppm
        SpeechEngineEspeakNg.cppm
        SpeechSystem.cppm
        StateMeta.cppm
        Text.cppm
        Timer.cppm
        UnicodeData.cppm
        Utterance.cppm
)