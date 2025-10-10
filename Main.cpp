#include "Include/WinHeaders.hpp"
#include "Core/Core.hpp"
#include "Helper/Asm/AsmFactory.hpp"

#ifdef _DEBUG
#pragma comment (lib,"Zydisd.lib")
#else
#pragma comment (lib,"Zydis.lib")
#endif	

using namespace IronMan;

ptr_t base = 0;
ptr_t image_size = 0x1000;
HANDLE MainThread;


//48 83 ec 78 41 8b 41 08 f2 41 0f 10 01
HIJACK_API DWORD WINAPI Bootstarp(LPVOID lpParam)
{
	Console::Attach();
	Core::Initialize((ptr_t)lpParam);
	return 0;
}

extern "C" DWORD WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		MainThread = CreateThread(0, 0, &Bootstarp, hInstance, 0, 0);
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
)
{
	return Bootstarp(NULL);
}

//int main(LPVOID lpParam){
//
//	
//	
//	Console::Attach();
//	Core::Initialize((ptr_t)lpParam);
//	return 0;
//
//	//return Bootstarp(NULL);
//
//}