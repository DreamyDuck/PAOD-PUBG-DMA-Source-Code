#include "Include/WinHeaders.hpp"
#include "DirectX11.h"
#include "Imgui_dx11.h"
#include "Imgui_win32.h"
#include "Core/DirectX/Drawing.h"
#include "Core/FilePak.h"
#include <d3d11.h>
#include <shlobj.h>
#include "Core/Config.h"
#include <sddl.h>
#include "Core/SDK/SDK.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_FAILURE_STRINGS
#include "stb_image.h"
#include <dwmapi.h>
#include <DirectXColors.h>
#include <dxgi.h>
#pragma comment(lib, "dwmapi.lib")


extern IronMan::ServerResult g_ServerResult;
namespace IronMan::Core
{
	using namespace SDK;
	ImFont* OverlayEngine::Font16px = nullptr;
	ImFont* OverlayEngine::Font14px = nullptr;
	ImFont* OverlayEngine::Font18px = nullptr;
	ImFont* OverlayEngine::Font20px = nullptr;
	ImFont* OverlayEngine::Font30px = nullptr;

	delegate<void()> OverlayEngine::OnUpdate;
	delegate<void()> OverlayEngine::OnTick;
	delegate<void()> OverlayEngine::OnPrePresent;
	delegate<void()> OverlayEngine::OnPresent;
	delegate<void()> OverlayEngine::OnPostPresent;
	delegate<void(WndEventArgs&)> OverlayEngine::OnWndProc;

	ID3D11Device* OverlayEngine::mDevice;
	IDXGISwapChain* OverlayEngine::mSwapChain;
	ID3D11DeviceContext* OverlayEngine::mContext;

	HWND OverlayEngine::mOverlayWindow;

	int	OverlayEngine::mTargetScreenLeft;
	int	OverlayEngine::mTargetScreenTop;
	int	OverlayEngine::mTargetScreenWidth;
	int	OverlayEngine::mTargetScreenHeight;

	ID3D11RenderTargetView* OverlayEngine::mMainRenderTargetView;

	static bool InitImGui = false;

	KeyState       OverlayEngine::mKeyMap[256];
	USHORT       OverlayEngine::mApiKeyMap[256];
	KeyState       OverlayEngine::mApiKeyStateMap[256];
	std::map<int, std::vector< std::function<void(void)>>> OverlayEngine::mHotkeys;

	bool OverlayEngine::Initialize()
	{
		// Update target window position
		UpdateTargetWindow();

		// Setup overlay window
		if (!SetupWindow())
			return false;

		return true;
	}

	void OverlayEngine::HideWindow()
	{
		if (mOverlayWindow)
		{
			ShowWindow(mOverlayWindow, SW_HIDE);
		}
	}

	void OverlayEngine::InitWindow()
	{
		ShowWindow(mOverlayWindow, SW_SHOW);
		UpdateWindow(mOverlayWindow);
		OverlayEngine::Transparent();
		OverlayEngine::UpdateTargetWindow();
	}

	void OverlayEngine::Start()
	{
		Update();
		Tick();
		Render();
	}

	void OverlayEngine::Shutdown()
	{
	}

	bool OverlayEngine::bInitSuccess()
	{
		return InitImGui;
	}

	DWORD ThreadInitGui(LPVOID s)
	{
		typedef bool(__stdcall* initGUI)();
		initGUI Init = (initGUI)s;
		try
		{
			Init();
		}
		catch (...)
		{
			CONSOLE_INFO2(u8"ImGui绘制加载失败!");
			InitImGui = false;
			TerminateProcess(GetCurrentProcess(), 1);
		}
		InitImGui = true;
		return 1;
	}

	bool OverlayEngine::SetupWindow()
	{
		WNDCLASSEXW windowClassEx = { 0 };
		windowClassEx.cbSize = sizeof(WNDCLASSEXW);
		windowClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		windowClassEx.lpfnWndProc = WindowProc;
		windowClassEx.hInstance = GetModuleHandle(NULL);
		windowClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClassEx.hbrBackground = (HBRUSH)RGB(0, 0, 0);
		windowClassEx.lpszClassName = L"Qt5QWindowIcon";
		if (!RegisterClassExW(&windowClassEx))
			return false;
		WNDCLASSEXW wclass;
		GetClassInfoExW(GetModuleHandle(NULL), (L"Qt5QWindowIcon"), &wclass);


		constexpr auto style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_POPUPWINDOW;
		constexpr auto exStyle = WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_PALETTEWINDOW;

		OverlayEngine::mOverlayWindow = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
			(L"Qt5QWindowIcon"), (L"obs64"), WS_POPUP,
			OverlayEngine::mTargetScreenLeft,
			OverlayEngine::mTargetScreenTop,
			OverlayEngine::mTargetScreenWidth,
			OverlayEngine::mTargetScreenHeight,
			NULL, NULL, GetModuleHandle(NULL), NULL);
		if (!mOverlayWindow)
			return false;
		SetLayeredWindowAttributes(mOverlayWindow, RGB(0, 0, 0), 255, LWA_ALPHA);
		//SetLayeredWindowAttributes(mOverlayWindow, 0, 0, LWA_COLORKEY);

		MARGINS margins = { -1 };
		if (FAILED(DwmExtendFrameIntoClientArea(mOverlayWindow, &margins)))
			return false;

		DWM_BLURBEHIND bb = { 0 };
		bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
		bb.fEnable = true;
		bb.hRgnBlur = NULL;
		DwmEnableBlurBehindWindow(mOverlayWindow, &bb);


		CreateDirect3D();
		CreateThread(NULL, NULL, ThreadInitGui, InitializeImgui, NULL, NULL);
		return true;
	}

	bool OverlayEngine::CreateDirect3D()
	{
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd = { 0 };
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = mOverlayWindow;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &mSwapChain, &mDevice, &featureLevel, &mContext) != S_OK)
		{
			MessageBoxA(NULL, "Error D3D11CreateDeviceAndSwapChain!", "ERROR", MB_OK | MB_ICONHAND);
			return false;
		}
		CreateRenderTarget();
		return true;
	}

	void OverlayEngine::Update()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGuiIO& io = ImGui::GetIO();
		io.KeyCtrl = OverlayEngine::IsKeyDown(VK_CONTROL);
		io.KeyShift = OverlayEngine::IsKeyDown(VK_SHIFT);
		io.KeyAlt = OverlayEngine::IsKeyDown(VK_MENU);
		ImGui::NewFrame();
		OnUpdate();
	}

	void OverlayEngine::Tick()
	{
	}

	void OverlayEngine::Render()
	{
		OnPrePresent();
		OnPresent();
		OnPostPresent();
		ImGui::Render();
		mContext->OMSetRenderTargets(1, &mMainRenderTargetView, NULL);
		mContext->ClearRenderTargetView(mMainRenderTargetView, Vars.Misc.DisplayMode == 0 ? DirectX::Colors::Transparent : DirectX::Colors::Black);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		mSwapChain->Present(!Vars.Menu.LockFlame ? 1 : 0, 0);//vsync
		//mSwapChain->Present(0, 0);//vsync
	}

	void OverlayEngine::CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (pBackBuffer)
		{
			mDevice->CreateRenderTargetView(pBackBuffer, NULL, &mMainRenderTargetView);
			pBackBuffer->Release();
		}
		else
		{
			MessageBoxA(0, "RenderTarget Failed!", 0, 0);
		}

	}

	void OverlayEngine::CleanupRenderTarget()
	{
		if (mMainRenderTargetView) { mMainRenderTargetView->Release(); mMainRenderTargetView = NULL; }
	}

	LRESULT OverlayEngine::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_COMMNOTIFY)
			return 0;

		bool process = true;
		ProcessMessage(message, wParam, lParam);
		WndEventArgs args{ true,wParam, lParam,message ,hWnd };
		for (const auto& func : OnWndProc.func_list)
		{
			func(args);
			if (!args.Process)
			{
				process = false;
			}
		}

		extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
			return true;

		switch (message)
		{
			/*case WM_NCHITTEST:
				return HTTRANSPARENT;*/
		case WM_SYSKEYDOWN:
			if (wParam == VK_F4)
				return 0;
			break;
		case WM_SHOWWINDOW:
			if (!wParam)
				return 0;
			break;
		case WM_SIZE:
			if (mDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				CleanupRenderTarget();
				mSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		}
		return process ? DefWindowProc(hWnd, message, wParam, lParam) : 0;
	}

	bool OverlayEngine::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		switch (uMsg) {
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
		case WM_XBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
		case WM_XBUTTONUP:
			return ProcessMouseMessage(uMsg, wParam, lParam);
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			return ProcessKeybdMessage(uMsg, wParam, lParam);
		default:
			return false;
		}

	}

	bool OverlayEngine::ProcessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		auto key = VK_LBUTTON;
		auto state = KeyState::None;
		switch (uMsg) {
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			state = uMsg == WM_MBUTTONUP ? KeyState::Up : KeyState::Down;
			key = VK_MBUTTON;
			break;

		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			state = uMsg == WM_RBUTTONUP ? KeyState::Up : KeyState::Down;
			key = VK_RBUTTON;
			break;

		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			state = uMsg == WM_LBUTTONUP ? KeyState::Up : KeyState::Down;
			key = VK_LBUTTON;
			break;
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_XBUTTONDBLCLK:
			state = uMsg == WM_XBUTTONUP ? KeyState::Up : KeyState::Down;
			//key = (*(WORD*)((ptr_t)&wParam + 2) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
			key = (HIWORD(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2;
			break;
		default:
			return false;
		}

		if (state == KeyState::Up && mKeyMap[key] == KeyState::Down)
			mKeyMap[key] = KeyState::Pressed;
		else
			mKeyMap[key] = state;

		return true;
	}

	bool OverlayEngine::ProcessKeybdMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		auto key = wParam;
		auto state = KeyState::None;

		switch (uMsg) {
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			state = KeyState::Down;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			state = KeyState::Up;
			break;
		default:
			return false;
		}

		if (state == KeyState::Up && mKeyMap[int(key)] == KeyState::Down) {
			mKeyMap[int(key)] = KeyState::Pressed;

			for (const auto& hotkey_callback : mHotkeys[key])
			{
				if (hotkey_callback)
					hotkey_callback();

			}
		}
		else {
			mKeyMap[int(key)] = state;
		}

		return true;
	}

	void OverlayEngine::ProcessFunction()
	{
		static std::unordered_map<int, bool>KeyPress_;
		for (int i = 0; i < 200; i++)
		{
			auto findResult = mHotkeys.find(i);
			if (findResult != mHotkeys.end())
			{
				auto Res = KeyPress_.find(findResult->first);
				if (Res == KeyPress_.end())
				{
					KeyPress_.emplace(findResult->first, false);
					Res = KeyPress_.find(findResult->first);
				}

				if ((GetAsyncKeyState(findResult->first) & 0x8000) || Res->second)
				{
					if (!Res->second)
						Res->second = true;
					if (GetAsyncKeyState(findResult->first) == 0)
					{
						Res->second = false;
						for (const auto& hotkey_callback : findResult->second)
						{
							if (hotkey_callback)
								hotkey_callback();
						}
					}

				}

			}
			continue;
		}
	}

	SHORT OverlayEngine::GetKeyState(std::uint32_t vk)
	{
		if (vk == 27)
			return false;
		return GetAsyncKeyState(vk);
		//return mKeyMap[vk];
	}

	bool OverlayEngine::IsKeyDown(std::uint32_t vk)
	{
		if (vk == 27)
			return false;
		return mApiKeyStateMap[vk] == KeyState::Down || GetDMA().IsKeyDown(vk);
	}

	bool OverlayEngine::WasKeyPressed(std::uint32_t vk)
	{
		if (vk == 27)
			return false;
		return (mApiKeyStateMap[vk] == KeyState::Pressed) || GetDMA().WasKeyPressed(vk);
	}

	void OverlayEngine::SetKeyPressed(uint32_t vk, bool set)
	{
		if (vk == 27)
			return;
		mApiKeyStateMap[vk] = KeyState::None;
	}

	void OverlayEngine::UpdateKeyState()
	{
		for (int i = 0; i < 256; i++)
		{
			auto state = KeyState::None;
			mApiKeyMap[i] = GetAsyncKeyState(i);
			if (mApiKeyMap[i])
				state = KeyState::Down;
			else
				state = KeyState::Up;
			if (state == KeyState::Up && mApiKeyStateMap[i] == KeyState::Down)
			{
				mApiKeyStateMap[i] = KeyState::Pressed;
			}
			else
				mApiKeyStateMap[i] = state;
		}
	}

	void OverlayEngine::RegisterHotkey(std::uint32_t vk, std::function<void(void)> f)
	{
		mHotkeys[vk].push_back(f);
	}

	void OverlayEngine::RemoveHotkey(std::uint32_t vk)
	{
		mHotkeys[vk].clear();
	}

	void OverlayEngine::ActiveWindow(HWND hwnd)
	{
		auto dwCurID = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
		auto dwForeID = GetWindowThreadProcessId(hwnd, NULL);
		AttachThreadInput(dwCurID, dwForeID, TRUE);
		SetForegroundWindow(hwnd);
		AttachThreadInput(dwCurID, dwForeID, FALSE);
	}

	void OverlayEngine::UpdateTargetWindow()
	{
		mTargetScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		mTargetScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		//mTargetScreenWidth = 660;
		//mTargetScreenHeight = 660;

		mTargetScreenLeft = 0;
		mTargetScreenTop = 0;
	}

	void OverlayEngine::Transparent()
	{
		SetWindowLong(mOverlayWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		//LONG nRet = ::GetWindowLong(mOverlayWindow, GWL_EXSTYLE);
		//SetWindowLong(mOverlayWindow, GWL_EXSTYLE, nRet | WS_EX_TRANSPARENT);
		//SetWindowDisplayAffinity(mOverlayWindow, WDA_MONITOR);
	}

	void OverlayEngine::UnTransparent()
	{
		SetWindowLong(mOverlayWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
		//LONG nRet = ::GetWindowLong(mOverlayWindow, GWL_EXSTYLE);
		//SetWindowLong(mOverlayWindow, GWL_EXSTYLE, nRet & ~WS_EX_TRANSPARENT);
		//SetWindowDisplayAffinity(mOverlayWindow, WDA_NONE);
	}

	bool OverlayEngine::InitializeImgui()
	{
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(mOverlayWindow, OverlayEngine::mTargetScreenWidth, OverlayEngine::mTargetScreenHeight);
		ImGui_ImplDX11_Init(mDevice, mContext);
		{
			ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;

			char szPath[MAX_PATH] = { 0 };
			SHGetSpecialFolderPathA(NULL, szPath, CSIDL_FONTS, FALSE);

			LANGID lid = GetSystemDefaultLangID();
			bool loadKorea = false;
			if (lid == 0x0412 || lid == 0x0812)
				loadKorea = true;

			void* msyh_data = nullptr;

			size_t msyh_data_size = Package::GetPackage()->FindFile(HASH("SourceHanSans-Medium.ttc"), (void**)&msyh_data);
			if (!msyh_data)
			{
				MessageBoxA(NULL, "Cannot find TTC font file \"msyh.ttc\" in configured font directories.\r\nClick OK to terminate.", "ERROR", MB_OK | MB_ICONHAND);
				TerminateProcess(GetCurrentProcess(), 0);
			}

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			const ImWchar* glyphRanges = nullptr;

			switch (lid)
			{
			case 0x0412:
			case 0x0812:
				glyphRanges = io.Fonts->GetGlyphRangesKorean();//这个需要换字体
				break;
			case 0x0402://俄语系
			case 0x0419:
				glyphRanges = io.Fonts->GetGlyphRangesCyrillic();
				break;
			default:
				glyphRanges = io.Fonts->GetGlyphRangesChineseFull();
				break;
			}

			Font14px = io.Fonts->AddFontFromMemoryTTF(msyh_data, (int)msyh_data_size, 14.0f, NULL, glyphRanges);

			Font16px = io.Fonts->AddFontFromMemoryTTF(msyh_data, (int)msyh_data_size, 16.0f, NULL, glyphRanges);

			Font18px = io.Fonts->AddFontFromMemoryTTF(msyh_data, (int)msyh_data_size, 18.0f, NULL, glyphRanges);

			Font20px = io.Fonts->AddFontFromMemoryTTF(msyh_data, (int)msyh_data_size, 20.0f, NULL, glyphRanges);

			Font30px = io.Fonts->AddFontFromMemoryTTF(msyh_data, (int)msyh_data_size, 30.0f, NULL, glyphRanges);

			io.FontDefault = Font14px;
			io.Fonts->Build();


			memset((void*)msyh_data, 0, msyh_data_size);
			free((void*)msyh_data);
		}

		/// Style
		{
			ImColor mainColor = ImColor(int(9), int(10), int(31), 255);
			ImColor bodyColor = ImColor(int(9), int(10), int(31), 255);
			ImColor fontColor = ImColor(int(255), int(255), int(255), 255);

			auto& style = ImGui::GetStyle();

			ImVec4 mainColorHovered = ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w);
			ImVec4 mainColorActive = ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w);
			ImVec4 menubarColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w - 0.8f);
			ImVec4 frameBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .1f);
			ImVec4 tooltipBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .05f);

			style.Colors[ImGuiCol_HeaderActive] = ImColor(254, 254, 254, 255);
			style.Colors[ImGuiCol_Text] = ImColor(254, 254, 254, 255);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImColor(9, 10, 31, 255);
			style.Colors[ImGuiCol_ChildBg] = ImColor(9, 10, 31, 255);
			//style.Colors[ImGuiCol_ChildBg] = ImColor(255, 255, 255, 255);
			style.Colors[ImGuiCol_PopupBg] = tooltipBgColor;
			style.Colors[ImGuiCol_Border] = ImColor(250, 125, 0, 255);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImColor(14, 14, 14, 255);
			style.Colors[ImGuiCol_FrameBgHovered] = ImColor(15, 15, 15, 255);
			style.Colors[ImGuiCol_FrameBgActive] = ImColor(15, 15, 15, 255);
			style.Colors[ImGuiCol_TitleBg] = mainColor;
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
			style.Colors[ImGuiCol_TitleBgActive] = mainColor;
			style.Colors[ImGuiCol_MenuBarBg] = menubarColor;
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(frameBgColor.x + .05f, frameBgColor.y + .05f, frameBgColor.z + .05f, frameBgColor.w);
			style.Colors[ImGuiCol_ScrollbarGrab] = mainColor;
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = mainColorHovered;
			style.Colors[ImGuiCol_ScrollbarGrabActive] = mainColorActive;
			style.Colors[ImGuiCol_CheckMark] = ImColor(240, 240, 240, 255);
			style.Colors[ImGuiCol_SliderGrab] = mainColorHovered;
			style.Colors[ImGuiCol_SliderGrabActive] = mainColorActive;
			style.Colors[ImGuiCol_Button] = ImColor(14, 14, 14, 255);
			style.Colors[ImGuiCol_ButtonHovered] = ImColor(28, 28, 28);
			style.Colors[ImGuiCol_ButtonActive] = ImColor(56, 56, 56);
			style.Colors[ImGuiCol_Header] = mainColor;
			style.Colors[ImGuiCol_HeaderHovered] = mainColorHovered;
			style.Colors[ImGuiCol_HeaderActive] = mainColorActive;

			style.Colors[ImGuiCol_Separator] = ImColor(3, 60, 50);
			style.Colors[ImGuiCol_SeparatorHovered] = ImColor(6, 254, 227);
			style.Colors[ImGuiCol_SeparatorActive] = ImColor(6, 254, 227);

			style.Colors[ImGuiCol_ResizeGrip] = mainColor;
			style.Colors[ImGuiCol_ResizeGripHovered] = mainColorHovered;
			style.Colors[ImGuiCol_ResizeGripActive] = mainColorActive;
			style.Colors[ImGuiCol_PlotLines] = mainColor;
			style.Colors[ImGuiCol_PlotLinesHovered] = mainColorHovered;
			style.Colors[ImGuiCol_PlotHistogram] = mainColor;
			style.Colors[ImGuiCol_PlotHistogramHovered] = mainColorHovered;
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
			//style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
			style.Alpha = 1.0f;
			style.WindowPadding = ImVec2(0, 0);
			style.WindowMinSize = ImVec2(32, 32);
			style.WindowRounding = 0.f;
			style.WindowBorderSize = 0.f;
			style.PopupBorderSize = 0.f;
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.ChildRounding = 0.0f;
			style.ChildBorderSize = 1.f;
			style.FramePadding = ImVec2(4, 3);
			style.FrameRounding = 0.0f;
			style.ItemSpacing = ImVec2(4, 3);
			style.ItemInnerSpacing = ImVec2(4, 4);
			style.TouchExtraPadding = ImVec2(0, 0);
			style.IndentSpacing = 21.0f;
			style.ColumnsMinSpacing = 3.0f;
			style.ScrollbarSize = 8.f;
			style.ScrollbarRounding = 0.0f;
			style.GrabMinSize = 1.0f;
			style.GrabRounding = 0.0f;
			style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
			style.DisplayWindowPadding = ImVec2(22, 22);
			style.DisplaySafeAreaPadding = ImVec2(4, 4);
			style.AntiAliasedLines = true;
			style.AntiAliasedFill = true;
			style.CurveTessellationTol = 1.25f;
			ImGui::SetColorEditOptions(ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions
				| ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar);
		}

		return true;
	}

	void* OverlayEngine::CreateTexture(const char* buffer, size_t len)
	{
		HRESULT hr;
		int x, y, n;
		stbi_uc* data = stbi_load_from_memory((stbi_uc*)buffer, (int)len, &x, &y, &n, 0);
		if (data)
		{
			D3D11_TEXTURE2D_DESC desc = {};
			desc.Width = (UINT)x;
			desc.Height = (UINT)y;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA tbsd = { 0 };

			tbsd.pSysMem = (void*)data;
			tbsd.SysMemPitch = (UINT)(x * 4);
			tbsd.SysMemSlicePitch = 0;

			ID3D11Texture2D* tex;
			hr = mDevice->CreateTexture2D(&desc, &tbsd, &tex);
			assert(SUCCEEDED(hr));

			ID3D11ShaderResourceView* _srv;
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC srv;
				memset(&srv, 0, sizeof(srv));
				srv.Format = desc.Format;
				srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srv.Texture2D.MipLevels = 1;
				srv.Texture2D.MostDetailedMip = 0;
				hr = mDevice->CreateShaderResourceView((ID3D11Resource*)tex, &srv, &_srv);
				assert(SUCCEEDED(hr));
			}
			tex->Release();
			stbi_image_free(data);
			return _srv;
		}
		return nullptr;
	}

	ImVec2 OverlayEngine::GetViewPort()
	{
		return ImVec2((float)mTargetScreenWidth, (float)mTargetScreenHeight);
	}

}

