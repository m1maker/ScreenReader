// Event to speech.
module;

#include <array>
#include <memory_resource>
#include <new>
#include <string>
#include <vector>
export module Core.Outputs.Speech;
import Core.FocusManager;
import Core.Logger;
import Proxies.Object;

export class COutputSpeech final : TModule<"SpeechOutput"> {
public:
	void Stop();
	void FocusChange(CObjectProxy obj);
	void ValueChange(CObjectProxy obj);
	void StateChange(CObjectProxy obj);
	void SelectionChange(CObjectProxy obj);

	void CursorMove(CObjectProxy obj);
};
