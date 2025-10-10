#pragma once
#include <d3d11.h>
#include "Include/delegate.hpp"
#include "Core/Imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Core/Imgui/imgui_internal.h"
#include <map>
#pragma comment(lib, "d3d11.lib")

namespace IronMan::Core
{
	enum class KeyState
	{
		None = 1,
		Down,
		Up,
		Pressed /*Down and then up*/
	};

	struct WndEventArgs
	{
	public:
		bool Process;
		WPARAM WParam;
		LPARAM LParm;
		UINT Msg;
		HWND HWnd;
	};

	class OverlayEngine
	{
	public:
		static bool Initialize();
		static void InitWindow();
		static void HideWindow();
		static void Start();
		static void Shutdown();
		static bool bInitSuccess();
		static void UpdateTargetWindow();
		static void Transparent();
		static void UnTransparent();
	public:
		static ImFont* Font16px;
		static ImFont* Font14px;
		static ImFont* Font18px;
		static ImFont* Font20px;
		static ImFont* Font30px;
	public:
		// Input
		static SHORT      GetKeyState(uint32_t vk);
		static bool       IsKeyDown(uint32_t vk);
		static bool       WasKeyPressed(uint32_t vk);
		static void		  SetKeyPressed(uint32_t vk, bool set);
		static void		  UpdateKeyState();

		static void RegisterHotkey(uint32_t vk, std::function<void(void)> f);
		static void RemoveHotkey(uint32_t vk);

		static void ActiveWindow(HWND hwnd);

		static void* CreateTexture(const char* buffer, size_t len);
		static ImVec2 GetViewPort();
		static void ProcessFunction();
	public:
		static delegate<void()> OnUpdate;
		static delegate<void()> OnTick;
		static delegate<void()> OnPrePresent;
		static delegate<void()> OnPresent;
		static delegate<void()> OnPostPresent;
		static delegate<void(WndEventArgs&)> OnWndProc;
	private:
		static bool SetupWindow();
		static bool CreateDirect3D();
		static bool InitializeImgui();
		static void Update();
		static void Tick();
		static void Render();

		// D3D Function
		static void CreateRenderTarget();
		static void CleanupRenderTarget();

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static bool ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		static bool ProcessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		static bool ProcessKeybdMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		static ID3D11Device* mDevice;
		static IDXGISwapChain* mSwapChain;
		static ID3D11DeviceContext* mContext;

		static HWND mOverlayWindow;
	public:
		static int	mTargetScreenLeft;
		static int	mTargetScreenTop;
		static int	mTargetScreenWidth;
		static int	mTargetScreenHeight;

		static ID3D11RenderTargetView* mMainRenderTargetView;
		static KeyState       mKeyMap[256];
		static USHORT       mApiKeyMap[256];
		static KeyState     mApiKeyStateMap[256];
		static std::map<int, std::vector< std::function<void(void)>>> mHotkeys;
	};
}