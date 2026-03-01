// Platform dependent worker trait.
module;
#include <Core/Logger.h>
#include <Core/StaticInterface.h>
export module Traits.PlatformDependentWorker;
import Core.PlatformError;

/*
A platform dependent worker is the screen reader's most basic and main loop.
*/
export template <typename Derived, typename Error> class TPlatformDependentWorker {
protected:
	void PushError(EPlatformError error) const noexcept {
		g_logger.Log(CLogger::ERROR, "Platform", std::string(PlatformErrorToString(error)));
	}

public:
	BindStaticInterface(Derived);

	/*
	The Loop function should run as long as g_running is true.
	*/

	ProxyMethod(Loop);
	void Throw(const Error* error) { return Impl().do_Throw(error); }
};
