// Event to speech.
module Core.Outputs.Speech;
import Core.FocusManager;
import Core.MessageBuilder;
import Core.Object;
import Core.SpeechSystem;
import Core.Text;

void COutputSpeech::Stop() {
	SpeechSystem::GetInstance().Stop();
}

// Various Announcers
void COutputSpeech::Output(CObjectEvent event) {
	if (!event.object.IsValid()) [[unlikely]]
		return;

	MessageBuilder::GetInstance().Reset();
	using enum EObjectEventType;
	switch (event.type) {
	case FOCUS_GAINED:
		MessageBuilder::GetInstance().BuildFocusAnnouncement(event.object);
		break;
	case STATE_CHANGED:
		MessageBuilder::GetInstance().BuildStateAnnouncement(event.object);
		break;
	case NAME_CHANGED:
		MessageBuilder::GetInstance().BuildNameAnnouncement(event.object);
		break;
	case DESCRIPTION_CHANGED:
		MessageBuilder::GetInstance().BuildDescriptionAnnouncement(event.object);
		break;
	case VALUE_CHANGED:
		MessageBuilder::GetInstance().BuildValueAnnouncement(event.object);
		break;
	case SELECTION_CHANGED:
		MessageBuilder::GetInstance().BuildSelectionAnnouncement(event.object);
		break;
	case CURSOR_MOVED:
		MessageBuilder::GetInstance().BuildCursorAnnouncement(event.object);
		break;
	default:
		break;
	}
	SpeechSystem::GetInstance().Speak(MessageBuilder::GetInstance());
}
