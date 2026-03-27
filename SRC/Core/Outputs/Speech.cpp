// Event to speech.
module Core.Outputs.Speech;
import Core.MessageBuilder;
import Core.Object;
import Core.SpeechSystem;
import Core.Text;

void COutputSpeech::Stop() {
	SpeechSystem::GetInstance().Stop();
}

// Various Announcers
void COutputSpeech::FocusChange(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto announcement = MessageBuilder::GetInstance().CreateString();
	MessageBuilder::GetInstance().BuildFocusAnnouncement(announcement, obj);
	SpeechSystem::GetInstance().Speak(announcement);
}

void COutputSpeech::ValueChange(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto type = obj.GetType();
	if (!type || !IsObjectValue(*type))
		return;
	auto announcement = MessageBuilder::GetInstance().CreateString();
	MessageBuilder::GetInstance().BuildValueAnnouncement(announcement, obj);
	SpeechSystem::GetInstance().Speak(announcement);
}

void COutputSpeech::StateChange(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto announcement = MessageBuilder::GetInstance().CreateString();
	MessageBuilder::GetInstance().BuildStateAnnouncement(announcement, obj);
	SpeechSystem::GetInstance().Speak(announcement);
}

void COutputSpeech::SelectionChange(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;
}

void COutputSpeech::CursorMove(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto text_provider = obj.GetAs<CTextProviderProxy>();
	auto cursor = text_provider.GetCursor();
	if (!cursor) {
		return;
	}

	ETextGranularity granularity{ETextGranularity::CHARACTER};
	auto announcement = MessageBuilder::GetInstance().CreateString();
	MessageBuilder::GetInstance().FindAnnouncementOfCursorPosition(announcement, text_provider, granularity);
	if (granularity == ETextGranularity::CHARACTER)
		SpeechSystem::GetInstance().Spell(announcement);
	else
		SpeechSystem::GetInstance().Speak(announcement);
}
