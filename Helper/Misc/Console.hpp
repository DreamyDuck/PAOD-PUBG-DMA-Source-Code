#pragma once

namespace IronMan::Console
{
	/// <summary>
	/// Create and attach a console window to the current process
	/// </summary>
	void Attach();


	/// <summary>
	/// Detach and destroy the attached console
	/// </summary>
	void Detach();

	/// <summary>
	/// Replacement to printf that works with the newly created console
	/// </summary>
	/// <returns>if true output is complete</returns>
	bool Print(const char* fmt, ...);

	/// <summary>
	/// Blocks execution until a key is pressed on the console window
	/// </summary>
	/// <returns>key</returns>
	char ReadKey();

	/// <summary>
	/// Show console window
	/// </summary>
	void Show();

	/// <summary>
	/// hide console window
	/// </summary>
	void Hide();

#define CONCOLOR_DEFAULT() SetConsoleTextAttribute(GetStdHandle( STD_OUTPUT_HANDLE) ,FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED )
#define CONCOLOR(color) SetConsoleTextAttribute(GetStdHandle( STD_OUTPUT_HANDLE) ,color )
#define CONSOLE_PRINT(msg,...) CONCOLOR_DEFAULT(); IronMan::Console::Print(msg,##__VA_ARGS__)
#define CONSOLE_DPRINT(msg,...) IronMan::Console::DPrint(msg,##__VA_ARGS__)
#ifdef ENABLE_CONSOLE
#define CONSOLE_INFO(msg,...) CONCOLOR(FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); CONSOLE_PRINT(Utils::FormatString( /**/(u8"[输出信息] %s"),/**/(msg)).c_str(),##__VA_ARGS__)
#define CONSOLE_INFO2(msg,...)CONCOLOR(FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); CONSOLE_PRINT(Utils::FormatString( /**/(u8"[输出信息] %s"),/**/(msg)).c_str(),##__VA_ARGS__)
#define CONSOLE_ERROR(msg,...) CONCOLOR(FOREGROUND_INTENSITY | FOREGROUND_RED); CONSOLE_PRINT( Utils::FormatString( "[Error] %s",msg).c_str(),##__VA_ARGS__)
#define CONSOLE_DERROR(msg,...) CONCOLOR(FOREGROUND_RED); CONSOLE_DPRINT( msg,##__VA_ARGS__)
#define CONSOLE_WARINING(msg,...) CONCOLOR(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY ); CONSOLE_PRINT(Utils::FormatString( "[WARINING] %s",msg).c_str(),##__VA_ARGS__)
#else
#define CONSOLE_INFO(msg,...)
#define CONSOLE_INFO2(msg,...) CONCOLOR(FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); CONSOLE_PRINT(Utils::FormatString( /**/("[INFO] %s"),/**/(msg)).c_str(),##__VA_ARGS__)
#define CONSOLE_ERROR(msg,...) 
#define CONSOLE_DERROR(msg,...)
#define CONSOLE_WARINING(msg,...) 
#endif

	
}