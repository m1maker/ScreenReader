export module Core.ObjectHandler;
import Core.Event;
import Core.FocusManager;
import Core.Logger;
import Core.OutputManager;
import Core.Singleton;

export class ObjectHandler final : TModule<"ObjectHandler">, public TSingleton<ObjectHandler> {
	FocusManager& m_focusManager;
	OutputManager& m_outputManager;

public:
	explicit ObjectHandler();

	void Handle(CObjectEvent& event);
};
