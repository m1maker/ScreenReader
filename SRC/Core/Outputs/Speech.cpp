// Event to speech.
module Core.Outputs.Speech;
import Core.MessageBuilder;
import Core.Object;
import Core.SpeechSystem;
import Core.Text;

void COutputSpeech::Stop() {
	SpeechSystem::GetInstance().Interrupt();
}

// Various Announcers
void COutputSpeech::Output(CObjectEvent event) {
	if (!event.object.IsValid()) [[unlikely]]
		return;

	auto message = MessageBuilder::GetInstance().CreateMessage();
	using enum EObjectEventType;
	switch (event.type) {
	case FOCUS_GAINED:
		MessageBuilder::GetInstance().BuildFocusAnnouncement(message, event.object);
		break;
	case STATE_CHANGED:
		MessageBuilder::GetInstance().BuildStateAnnouncement(message, event.object);
		break;
	case NAME_CHANGED:
		MessageBuilder::GetInstance().BuildNameAnnouncement(message, event.object);
		break;
	case DESCRIPTION_CHANGED:
		MessageBuilder::GetInstance().BuildDescriptionAnnouncement(message, event.object);
		break;
	case VALUE_CHANGED:
		MessageBuilder::GetInstance().BuildValueAnnouncement(message, event.object);
		break;
	case SELECTION_CHANGED:
		MessageBuilder::GetInstance().BuildSelectionAnnouncement(message, event.object);
		break;
	case TEXT_SELECTION_CHANGED:
		MessageBuilder::GetInstance().BuildTextSelectionAnnouncement(message, event.object);
		break;
	case CURSOR_MOVED:
		MessageBuilder::GetInstance().BuildCursorAnnouncement(message, event.object);
		break;
	default:
		break;
	}
	SpeechSystem::GetInstance().Speak(message);
}
