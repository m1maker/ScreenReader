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

	MessageBuilder::GetInstance().Reset();
	MessageBuilder::GetInstance().BuildFocusAnnouncement(obj);
	SpeechSystem::GetInstance().Speak(MessageBuilder::GetInstance());
}

void COutputSpeech::ValueChange(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto type = obj.GetType();
	if (!type || !IsObjectValue(*type))
		return;
	MessageBuilder::GetInstance().Reset();
	MessageBuilder::GetInstance().BuildValueAnnouncement(obj);
	SpeechSystem::GetInstance().Speak(MessageBuilder::GetInstance());
}

void COutputSpeech::StateChange(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	MessageBuilder::GetInstance().Reset();
	MessageBuilder::GetInstance().BuildStateAnnouncement(obj);
	SpeechSystem::GetInstance().Speak(MessageBuilder::GetInstance());
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
	MessageBuilder::GetInstance().Reset();
	MessageBuilder::GetInstance().FindAnnouncementOfCursorPosition(text_provider, granularity);
	if (granularity == ETextGranularity::CHARACTER)
		SpeechSystem::GetInstance().Spell(MessageBuilder::GetInstance());
	else
		SpeechSystem::GetInstance().Speak(MessageBuilder::GetInstance());
}
