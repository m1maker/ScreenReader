module;
#include <cctype>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <utility>
#include <variant>
#include <vector>
export module Core.AudioSystem;
import Core.Audio;
import Core.Environment;
import Core.Logger;
import Core.Singleton;

export using AudioData = std::vector<signed short int>;
export using AudioDataQueue = std::queue<AudioData>;

export class AudioSystem final : TModule<"AudioSystem">, public TSingleton<AudioSystem> {
	AudioEngineVariant m_variant;
	AudioDataQueue m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cv;

public:
	explicit AudioSystem() { m_variant.emplace<DefaultAudioEngine>(); }

	~AudioSystem() = default;

	template <typename F> decltype(auto) WithEngine(F&& func) {
		return std::visit(
			[&](auto&& eng) -> decltype(auto) {
				using T = std::decay_t<decltype(eng)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					return func(eng);
				}
			},
			m_variant);
	}
};
