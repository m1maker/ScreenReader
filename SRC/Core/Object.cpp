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

// Some implementations of common object methods.
module;
#include <iomanip>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
module Core.Object;
import Core.ObjectMeta;

/*
The object type query also applies to state names.
For example, there's no such state as EObjectState::UNCHECKED, but there is CHECKABLE.
We need to understand what kind of object this is to more accurately determine the states for announcements when we
don't request `require_all`.
*/
void GetObjectStateNames(std::pmr::string& out, EObjectType type, ObjectStates states, bool require_all) {
	// Capability / Infrastructure States (Usually only for logging/require_all).
	if (require_all) {
		if (states[std::to_underlying(EObjectState::VISIBLE)])
			out += " visible";
		if (states[std::to_underlying(EObjectState::ENABLED)])
			out += " enabled";
		if (states[std::to_underlying(EObjectState::FOCUSABLE)])
			out += " focusable";
		if (states[std::to_underlying(EObjectState::FOCUSED)])
			out += " focused";
		if (states[std::to_underlying(EObjectState::SELECTABLE)])
			out += " selectable";
		if (states[std::to_underlying(EObjectState::CHECKABLE)])
			out += " checkable";
		if (states[std::to_underlying(EObjectState::EDITABLE)])
			out += " editable";
		if (states[std::to_underlying(EObjectState::EXPANDABLE)])
			out += " expandable";
		if (states[std::to_underlying(EObjectState::RESIZABLE)])
			out += " resizable";
	}

	// Interactive / Crucial States (Always announced or contextually forced).
	if (states[std::to_underlying(EObjectState::BUSY)])
		out += " busy";
	if (states[std::to_underlying(EObjectState::LOADING)])
		out += " loading";

	// Selection Logic.
	if (states[std::to_underlying(EObjectState::SELECTED)]) {
		out += " selected";
	}

	// Toggle/Checked Logic (Normalization).
	// For Toggle Buttons, we prefer "pressed" over "checked".
	if (type == EObjectType::TOGGLE_BUTTON) {
		if (states[std::to_underlying(EObjectState::PRESSED)]) {
			out += " pressed";
		}
		else {
			out += " not pressed";
		}
	}

	// For Checkboxes (and generic checkables that aren't toggle buttons).
	else {
		if (states[std::to_underlying(EObjectState::CHECKED)]) {
			out += " checked";
		}
		else if (states[std::to_underlying(EObjectState::INDETERMINATE)]) {
			out += " partially checked";
		}
		else if (type == EObjectType::CHECKBOX ||
			(states[std::to_underlying(EObjectState::CHECKABLE)] && require_all)) {
			out += " not checked";
		}

		// Handle non-toggle-button 'pressed' state (e.g. normal button).
		if (states[std::to_underlying(EObjectState::PRESSED)])
			out += " pressed";
	}

	// Expansion Logic.
	if (states[std::to_underlying(EObjectState::EXPANDED)]) {
		out += " expanded";
	}
	else if (states[std::to_underlying(EObjectState::COLLAPSED)]) {
		out += " collapsed";
	}

	// Text / Input specific normalization.
	if (states[std::to_underlying(EObjectState::READONLY)])
		out += " read-only";
	if (states[std::to_underlying(EObjectState::SECURE)])
		out += " secure";
	if (states[std::to_underlying(EObjectState::INVALID)])
		out += " invalid";
	if (states[std::to_underlying(EObjectState::REQUIRED)]) {
		// Only announce "required" for input types unless logging all.
		if (IsObjectInGroup(type, EObjectGroup::INPUT) || require_all) {
			out += " required";
		}
	}

	if (states[std::to_underlying(EObjectState::MULTI_LINE)])
		out += " multi-line";
	if (states[std::to_underlying(EObjectState::MULTI_SELECTABLE)])
		out += " multi-selectable";

	// Global attributes.
	if (states[std::to_underlying(EObjectState::HOVERED)] && require_all)
		out += " hovered";
	if (states[std::to_underlying(EObjectState::DEFAULT)])
		out += " default";
	if (states[std::to_underlying(EObjectState::MODAL)])
		out += " modal";
	if (states[std::to_underlying(EObjectState::VISITED)])
		out += " visited";
}
