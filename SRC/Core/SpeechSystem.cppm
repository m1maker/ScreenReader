module;
#include <cctype>
#include <condition_variable>
#include <cstdint>
#include <expected>
#include <memory_resource>
#include <mutex>
#include <queue>
#include <string_view>
#include <thread>
#include <utility>
#include <variant>
export module Core.SpeechSystem;
import Core.BuiltInSpeechEngine;
import Core.Encoding;
import Core.Logger;
import Core.MessageBuilder;
import Core.Singleton;
import Core.Speech;

export using SpeechEngineVariant = std::variant<std::monostate, BuiltInSpeechEngine /*, CSpeechEngineRuntime*/>;

struct SSpeechMessage final {
	std::pmr::string message;
	bool interrupt{false}, ssml{false};
	unsigned char rate, volume, pitch, pitch_range;
};

using SpeechMessageQueue = std::queue<SSpeechMessage>;

export class SpeechSystem final : TModule<"SpeechSystem">, public TSingleton<SpeechSystem> {
	std::pmr::memory_resource* m_pool{nullptr};
	SpeechEngineVariant m_variant;
	SpeechMessageQueue m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::jthread m_thread;

	template <typename Result = void> auto WithEngine(auto&& func) -> SpeechEngineResult<Result> {
		return std::visit(
			[&](auto&& eng) -> SpeechEngineResult<Result> {
				using T = std::decay_t<decltype(eng)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					return func(eng);
				}
				else
					return std::unexpected(ESpeechEngineError::DEFUNCT);
			},
			m_variant);
	}

	inline auto EngineSpeak(std::string_view message) -> SpeechEngineResult<SpeechMessage> {
		return WithEngine<SpeechMessage>([message](auto&& engine) { return engine.Speak(message); });
	}

	void EngineStop() {
		WithEngine<>([](auto&& engine) {
			engine.Stop();
			return SpeechEngineResult<>();
		});
	}

public:
	explicit SpeechSystem() : m_pool(MessageBuilder::GetInstance().GetPool()) {
		m_variant.emplace<BuiltInSpeechEngine>();
	}

	~SpeechSystem() { Stop(); }

	void Start();
	void Stop();

	void Speak(std::string_view message, bool interrupt = false, bool ssml = false);
	void Interrupt();
};
