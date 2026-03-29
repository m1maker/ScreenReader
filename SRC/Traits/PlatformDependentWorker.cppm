// Platform dependent worker trait.
module;
#include <utility>
export module Traits.PlatformDependentWorker;
import Core.PlatformError;

/*
A platform dependent worker is the screen reader's most basic and main loop.
*/
export template <typename Derived, typename Error> class TPlatformDependentWorker {
protected:
	void PushError(EPlatformError error) const noexcept {
		// g_logger.Log(Logger::ERROR, "Platform", std::string(PlatformErrorToString(error)));
	}

public:
	/*
	The Loop function should run as long as g_running is true.
	*/

	void Loop(this auto&& self) { std::forward<decltype(self)>(self).do_Loop(); }
	void Throw(this auto&& self, const Error* error) { return std::forward<decltype(self)>(self).do_Throw(error); }
};
