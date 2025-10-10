#pragma once

namespace IronMan::Core::Auth
{
	extern ULONGLONG ServerTime;
	bool Check(bool foreceCheck = false);
}