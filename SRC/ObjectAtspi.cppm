export module ObjectAtspi;
#include <atspi.h>
#include "Object.h"

export class CObjectAtspi final : public IObject {
	ATSPIAccessible* m_accessible{nullptr};
public:

};

