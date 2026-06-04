/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

module Core.ObjectHandler;
import Core.App;
import Core.Event;
import Core.FocusManager;
import Core.Object;
import Core.OutputManager;

void ObjectHandler::Handle(CObjectEvent& event) {
	auto& _ = ScreenReaderApp::GetInstance().GetSettings();
	auto& focus_manager = FocusManager::GetInstance();
	auto& output_manager = OutputManager::GetInstance();

	if (event.type == EObjectEventType::FOCUS_GAINED) {
		if (focus_manager.GetFocus() == event.object)
			return;
		output_manager.Stop();
		focus_manager.SetFocus(event.object);
		output_manager.WhereAmI();
	}
	else if (focus_manager.GetFocus() == event.object) {
		output_manager.Stop();
	}
	else
		return;
	output_manager.Output(event);
}
