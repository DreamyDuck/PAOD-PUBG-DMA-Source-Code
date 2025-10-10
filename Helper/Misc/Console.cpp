#include "Include/WinHeaders.hpp"
#include "Console.hpp"
#include "Utils.hpp"

namespace IronMan::Console
{
	static HWND   _hConsole;
	static HANDLE _hOut;
	static HANDLE _hOld_out;
	static HANDLE _hErr;
	static HANDLE _hOld_err;
	static HANDLE _hIn;
	static HANDLE _hOld_in;


	void Attach()
	{
		char title[56];
		sprintf_s(title, ("%s"), "Paod Console Window");
		_hConsole = GetConsoleWindow();
		if (_hConsole)
		{
			ShowWindow(_hConsole, SW_SHOW);

		}
		else
		{
			::AllocConsole() && ::AttachConsole(ATTACH_PARENT_PROCESS);
			_hConsole = GetConsoleWindow();
		}
		_hOld_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_hOld_in = GetStdHandle(STD_INPUT_HANDLE);
		_hOld_err = GetStdHandle(STD_ERROR_HANDLE);

		_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		_hIn = GetStdHandle(STD_INPUT_HANDLE);
		_hErr = GetStdHandle(STD_ERROR_HANDLE);

		COORD size = { 200, 80 };
		SetConsoleScreenBufferSize(_hOut, size); // 重新设置缓冲区大小

		SetConsoleMode(_hOut,
			ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
		//FILE* out;
		//freopen_s(&out, ("CONOUT$"), "w", stdout);

		DWORD mode;
		GetConsoleMode(_hIn, &mode);
		//SetConsoleMode(_hIn, mode & ENABLE_MOUSE_INPUT); // 禁用鼠标输入
		SetConsoleMode(_hIn, mode & ~ENABLE_MOUSE_INPUT); // 禁用鼠标输入
		GetConsoleMode(_hIn, &mode);
		SetConsoleMode(_hIn, mode & ~ENABLE_QUICK_EDIT_MODE); // 禁用快速编辑模式
		//SetConsoleMode(_hIn, mode & ENABLE_QUICK_EDIT_MODE); // 禁用快速编辑模式
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleTitleA(title);
		SetWindowPos(_hConsole, 0, 100, 100, 800, 600, SWP_NOZORDER);
	}

	void Detach()
	{
		if (_hOut && _hErr && _hIn) {
			FreeConsole();

			if (_hOld_out)
				SetStdHandle(STD_OUTPUT_HANDLE, _hOld_out);
			if (_hOld_err)
				SetStdHandle(STD_ERROR_HANDLE, _hOld_err);
			if (_hOld_in)
				SetStdHandle(STD_INPUT_HANDLE, _hOld_in);
		}
	}

	bool Print(const char* fmt, ...)
	{
		if (!_hOut)
			return false;

		char buf[2048];
		va_list va_args;
		va_start(va_args, fmt);
		_vsnprintf_s(buf, 2048, fmt, va_args);
		va_end(va_args);
		auto outStr = Utils::FormatString(("[-] %s\r\n"), buf);

		SetConsoleMode(_hOut,
			ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
		//FILE* out;
		//freopen_s(&out, ("CONOUT$"), "w", stdout);
		return !!WriteConsoleA(_hOut, outStr.data(), static_cast<DWORD>(outStr.length()), nullptr, nullptr);
	}

	char ReadKey()
	{
		if (!_hIn)
			return false;

		auto key = char{ 0 };
		auto keysread = DWORD{ 0 };

		ReadConsoleA(_hIn, &key, 1, &keysread, nullptr);

		return key;
	}

	void Show()
	{
		if (_hConsole != nullptr)
		{
			ShowWindow(_hConsole, SW_SHOW);
			SetForegroundWindow(_hConsole);
		}
	}

	void Hide()
	{
		if (_hConsole != nullptr)
		{
			ShowWindow(_hConsole, SW_HIDE);
		}
	}

}