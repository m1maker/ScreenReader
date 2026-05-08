export module Core.ObjectHandler;
import Core.Event;
import Core.Logger;
import Core.Singleton;

export class ObjectHandler final : TModule<"ObjectHandler">, public TSingleton<ObjectHandler> {
public:
	explicit ObjectHandler() = default;

	void Handle(CObjectEvent& event);
};
