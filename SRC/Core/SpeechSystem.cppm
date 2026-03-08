export module Core.SpeechSystem;
import Core.Environment;

export class CSpeechSystem final {
	SpeechEngineVariant m_variant;

	CSpeechSystem() = default;
	~CSpeechSystem() = default;

public:
	static auto& GetInstance() {
		static CSpeechSystem instance;
		return instance;
	}

	void SetEngine(SpeechEngineVariant variant);
	[[nodiscard]] auto GetEngine() const -> SpeechEngineVariant;
};
