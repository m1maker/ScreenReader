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

module;
#include <expected>
#include <string_view>
export module Core.ObjectNavigator;
import Core.Event;
import Core.Logger;
import Core.Object;
import Core.OutputManager;
import Core.Singleton;
import Proxies.Object;

enum class EObjectNavigatorStatus : unsigned char { MOVED = 0, NO_NEXT, NO_PREVIOUS, NO_CHILDREN, NO_PARENT, NO_VALID };

export enum class EObjectNavigatorStepDirection : unsigned char { LEFT, RIGHT, UP, DOWN };

export class ObjectNavigator : TModule<"ObjectNavigator">, public TSingleton<ObjectNavigator> {
	CObjectProxy m_objectInFocus;

	EObjectNavigatorStatus m_lastStatus{EObjectNavigatorStatus::NO_VALID};

	void Output() {
		if (m_lastStatus != EObjectNavigatorStatus::MOVED) {
			// Announce status
			m_lastStatus = EObjectNavigatorStatus::NO_VALID;
		}

		auto& output_manager = OutputManager::GetInstance();
		output_manager.Stop();
		CObjectEvent object_event{.type = EObjectEventType::FOCUS_GAINED, .object = m_objectInFocus};
		output_manager.Output(object_event);
	}

public:
	explicit ObjectNavigator() = default;

	template <EObjectNavigatorStepDirection Direction> void Step() {
		if constexpr (Direction == EObjectNavigatorStepDirection::UP) {
			auto parent = m_objectInFocus.GetParent();
			if (parent) {
				m_objectInFocus = *parent;
				m_lastStatus = EObjectNavigatorStatus::MOVED;
			}
			else
				m_lastStatus = EObjectNavigatorStatus::NO_PARENT;
		}
		else if constexpr (Direction == EObjectNavigatorStepDirection::DOWN) {
			auto children = m_objectInFocus.GetChildAt(0);
			if (children) {
				m_objectInFocus = *children;
				m_lastStatus = EObjectNavigatorStatus::MOVED;
			}
			else
				m_lastStatus = EObjectNavigatorStatus::NO_CHILDREN;
		}
		else if constexpr (Direction == EObjectNavigatorStepDirection::LEFT) {
			auto parent = m_objectInFocus.GetParent();
			auto current_index_in_parent = m_objectInFocus.GetIndex();
			if (parent && current_index_in_parent && *current_index_in_parent > 0) {
				auto previous_object = parent->GetChildAt((*current_index_in_parent) - 1);
				if (previous_object) {
					m_objectInFocus = *previous_object;
					m_lastStatus = EObjectNavigatorStatus::MOVED;
				}
				else
					m_lastStatus = EObjectNavigatorStatus::NO_PREVIOUS;
			}
			else
				m_lastStatus = EObjectNavigatorStatus::NO_PREVIOUS;
		}
		else if constexpr (Direction == EObjectNavigatorStepDirection::RIGHT) {
			int children_count_in_parent{-1};
			auto parent = m_objectInFocus.GetParent();
			if (parent)
				children_count_in_parent = parent->GetChildrenCount().value_or(-1);
			auto current_index_in_parent = m_objectInFocus.GetIndex();
			if (children_count_in_parent != -1 && current_index_in_parent &&
				*current_index_in_parent < children_count_in_parent) {
				auto next_object = parent->GetChildAt((*current_index_in_parent) + 1);
				if (next_object) {
					m_objectInFocus = *next_object;
					m_lastStatus = EObjectNavigatorStatus::MOVED;
				}
				else
					m_lastStatus = EObjectNavigatorStatus::NO_NEXT;
			}
			else
				m_lastStatus = EObjectNavigatorStatus::NO_NEXT;
		}

		Output();
	}

	void SetFocus(CObjectProxy obj) {
		if (!obj.IsValid()) [[unlikely]]
			return;

		m_objectInFocus = obj;
	}
};
