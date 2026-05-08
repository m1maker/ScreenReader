// Event to speech.
module;
export module Core.Outputs.Speech;
import Core.Event;
import Core.Logger;
import Proxies.Object;

export class COutputSpeech final : TModule<"SpeechOutput"> {
public:
	void Stop();
	void Output(CObjectEvent event);
};
