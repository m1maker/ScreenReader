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
