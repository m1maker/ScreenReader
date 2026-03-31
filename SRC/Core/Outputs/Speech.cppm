// Event to speech.
module;

#include <array>
#include <memory_resource>
#include <new>
#include <string>
#include <vector>
export module Core.Outputs.Speech;
import Core.Event;
import Core.FocusManager;
import Core.Logger;
import Proxies.Object;

export class COutputSpeech final : TModule<"SpeechOutput"> {
public:
	void Stop();
	void Output(CObjectEvent event);
};
