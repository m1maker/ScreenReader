module;
#include <cctype>
#include <condition_variable>
#include <cstdint>
#include <expected>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>
#include <utility>
#include <variant>
#include <vector>
export module Core.AudioSystem;
import Core.Audio;
import Core.Environment;
import Core.Logger;
import Core.Singleton;

export using AudioData = std::vector<signed short int>;
using AudioDataQueue = std::queue<AudioData>;

export class AudioSystem final : TModule<"AudioSystem">, public TSingleton<AudioSystem> {
	AudioEngineVariant m_variant;
	AudioDataQueue m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::jthread m_thread;

	template <typename Result = void> auto WithEngine(auto&& func) -> AudioEngineResult<Result> {
		return std::visit(
			[&](auto&& eng) -> AudioEngineResult<Result> {
				using T = std::decay_t<decltype(eng)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					return func(eng);
				}
				else
					return std::unexpected(EAudioEngineError::DEFUNCT);
			},
			m_variant);
	}

	[[nodiscard]] inline auto Initialize(SAudioParameters parameters) -> AudioEngineResult<> {
		return WithEngine<>([parameters](auto&& engine) { return engine.Initialize(parameters); });
	}
	inline void Uninitialize() {
		WithEngine<>([](auto&& engine) {
			engine.Uninitialize();
			return AudioEngineResult<>();
		});
	}

	[[nodiscard]] auto Write(const signed short int* buffer, unsigned long long frames) -> AudioEngineResult<> {
		return WithEngine<>([buffer, frames](auto&& engine) { return engine.Write(buffer, frames); });
	}

	inline void Wait() {
		WithEngine<>([](auto&& engine) {
			engine.Wait();
			return AudioEngineResult<>();
		});
	}
	inline void Drop() {
		WithEngine<>([](auto&& engine) {
			engine.Drop();
			return AudioEngineResult<>();
		});
	}

public:
	explicit AudioSystem() { m_variant.emplace<DefaultAudioEngine>(); }

	~AudioSystem() = default;

	void Start();
	void Stop();

	void PushData(const AudioData&& data);
};
