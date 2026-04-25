module Core.ObjectHandler;
import Core.App;
import Core.Event;
import Core.FocusManager;
import Core.Object;
import Core.OutputManager;

ObjectHandler::ObjectHandler()
	: m_focusManager(FocusManager::GetInstance()), m_outputManager(OutputManager::GetInstance()) {}

void ObjectHandler::Handle(CObjectEvent& event) {
	auto& _ = ScreenReaderApp::GetInstance().GetSettings();
	if (event.type == EObjectEventType::FOCUS_GAINED) {
		if (m_focusManager.GetFocus() == event.object)
			return;
		m_outputManager.Stop();
		m_focusManager.SetFocus(event.object);
		m_outputManager.WhereAmI();
	}
	else if (m_focusManager.GetFocus() == event.object)
		m_outputManager.Stop();
	m_outputManager.Output(event);
}
