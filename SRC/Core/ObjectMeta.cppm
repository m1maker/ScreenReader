module;
#include <array>
#include <bitset>
#include <string_view>
#include <utility>
export module Core.ObjectMeta;
import Core.Object;

enum class EObjectGroup : unsigned char {
	UNKNOWN = 0,
	PARENT,
	CONTAINER,
	DATA_VIEW,
	DATA_ELIMENT,
	INPUT,
	MATH,
	NAVIGATION,
	WEB,
	VALUE,
	FEEDBACK,
	COUNT
};

using ObjectGroupMask = std::bitset<std::to_underlying(EObjectGroup::COUNT)>;

struct SObjectMeta final {
	std::string_view speech_name;
	ObjectGroupMask group_flags;
};

[[nodiscard]] static constexpr auto InitializeMeta(EObjectType type) -> SObjectMeta {
	SObjectMeta meta;
	return meta;
}

using ObjectMetaArray = std::array<SObjectMeta, std::to_underlying(EObjectType::COUNT)>;

[[nodiscard]] static consteval auto InitializeMetaArray() -> ObjectMetaArray {
	ObjectMetaArray array;
	for (size_t i = 0; i < static_cast<size_t>(EObjectType::COUNT); ++i) {
		auto meta = InitializeMeta(static_cast<EObjectType>(i));
		array[i] = meta;
	}
	return array;
}

static constexpr ObjectMetaArray cObjectMetadata = InitializeMetaArray();
