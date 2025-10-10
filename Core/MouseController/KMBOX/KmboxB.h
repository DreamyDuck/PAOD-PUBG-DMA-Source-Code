#pragma once

#ifndef _COM_H_
#define _COM_H_

#pragma warning(disable: 4530)
#pragma warning(disable: 4786)
#pragma warning(disable: 4800)

#include <cmath>
#include <cassert>
#include <windows.h>
#include <algorithm>
#include <unordered_map>
#include <SetupAPI.h> 
#include <devguid.h>
#pragma comment(lib, "setupapi.lib")

namespace IronMan
{
	class _base_com   //虚基类 基本串口接口
	{
	protected:
		volatile int _port;  //串口号
		volatile HANDLE _com_handle;//串口句柄
		char _com_str[20];
		DCB _dcb;     //波特率，停止位，等
		COMMTIMEOUTS _co;  // 超时时间
		virtual bool open_port() = 0;
		void init() //初始化
		{
			memset(_com_str, 0, 20);
			memset(&_co, 0, sizeof(_co));
			memset(&_dcb, 0, sizeof(_dcb));
			_dcb.DCBlength = sizeof(_dcb);
			_com_handle = INVALID_HANDLE_VALUE;
		}
		virtual bool setup_port()
		{
			if (!is_open())
				return false;

			if (!SetupComm(_com_handle, 8192, 8192))
				return false; //设置推荐缓冲区

			if (!GetCommTimeouts(_com_handle, &_co))
				return false;
			_co.ReadIntervalTimeout = 0xFFFFFFFF;
			_co.ReadTotalTimeoutMultiplier = 0;
			_co.ReadTotalTimeoutConstant = 0;
			_co.WriteTotalTimeoutMultiplier = 0;
			_co.WriteTotalTimeoutConstant = 2000;
			if (!SetCommTimeouts(_com_handle, &_co))
				return false; //设置超时时间

			if (!PurgeComm(_com_handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
				return false; //清空串口缓冲区

			return true;
		}
		inline void set_com_port(int port)
		{
			char p[12];
			_port = port;
			strcpy_s(_com_str, "\\\\.\\COM");
			_ltoa_s(_port, p, 10);
			strcat_s(_com_str, p);
		}
	public:
		_base_com()
		{
			init();
		}
		virtual ~_base_com()
		{
			close();
		}
		//设置串口参数：波特率，停止位，等 支持设置字符串 "115200, 8, n, 1"
		bool set_state(char* set_str)
		{
			if (is_open())
			{
				if (!GetCommState(_com_handle, &_dcb))
					return false;
				if (!BuildCommDCBA(set_str, &_dcb))
					return false;
				return SetCommState(_com_handle, &_dcb) == TRUE;
			}
			return false;
		}
		//设置内置结构串口参数：波特率，停止位
		bool set_state(int BaudRate, int ByteSize = 8, int Parity = NOPARITY, int StopBits = ONESTOPBIT)
		{
			if (is_open())
			{
				if (!GetCommState(_com_handle, &_dcb))
					return false;
				_dcb.BaudRate = BaudRate;
				_dcb.ByteSize = (BYTE)ByteSize;
				_dcb.Parity = (BYTE)Parity;
				_dcb.StopBits = (BYTE)StopBits;
				return SetCommState(_com_handle, &_dcb) == TRUE;
			}
			return false;
		}
		//打开串口 缺省 115200, 8, n, 1
		inline bool open(int port)
		{
			return open(port, 115200);
		}
		//打开串口 缺省 baud_rate, 8, n, 1
		inline bool open(int port, int baud_rate)
		{
			if (port < 1 || port > 1024)
			{
				return false;
			}

			set_com_port(port);

			if (!open_port())
			{
				return false;
			}

			if (!setup_port())
			{
				return false;
			}

			return set_state(baud_rate);
		}
		//打开串口
		inline bool open(int port, char* set_str)
		{
			if (port < 1 || port > 1024)
				return false;

			set_com_port(port);

			if (!open_port())
				return false;

			if (!setup_port())
				return false;

			return set_state(set_str);

		}
		inline bool set_buf(int in, int out)
		{
			return is_open() ? SetupComm(_com_handle, in, out) : false;
		}
		//关闭串口
		inline virtual void close()
		{
			if (is_open())
			{
				CloseHandle(_com_handle);
				_com_handle = INVALID_HANDLE_VALUE;
			}
		}
		//判断串口是或打开
		inline bool is_open()
		{
			return _com_handle != INVALID_HANDLE_VALUE;
		}
		//获得串口句炳
		HANDLE get_handle()
		{
			return _com_handle;
		}
		operator HANDLE()
		{
			return _com_handle;
		}
	};

	class _sync_com : public _base_com
	{
	protected:
		//打开串口
		virtual bool open_port()
		{
			if (is_open())
				close();

			_com_handle = CreateFileA(
				_com_str,
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);
			assert(is_open());
			return is_open();//检测串口是否成功打开
		}

	public:

		_sync_com()
		{
		}
		//同步读
		int read(char* buf, int buf_len)
		{
			if (!is_open())
				return 0;

			buf[0] = '\0';

			COMSTAT  stat;
			DWORD error;

			if (ClearCommError(_com_handle, &error, &stat) && error > 0) //清除错误
			{
				PurgeComm(_com_handle, PURGE_RXABORT | PURGE_RXCLEAR); /*清除输入缓冲区*/
				return 0;
			}

			unsigned long r_len = 0;

			buf_len = std::min(buf_len - 1, (int)stat.cbInQue);
			if (!ReadFile(_com_handle, buf, buf_len, &r_len, NULL))
				r_len = 0;
			buf[r_len] = '\0';

			return r_len;
		}
		//同步写
		int write(char* buf, int buf_len)
		{
			if (!is_open() || !buf)
				return 0;

			DWORD    error;
			if (ClearCommError(_com_handle, &error, NULL) && error > 0) //清除错误
				PurgeComm(_com_handle, PURGE_TXABORT | PURGE_TXCLEAR);

			unsigned long w_len = 0;
			if (!WriteFile(_com_handle, buf, buf_len, &w_len, NULL))
				w_len = 0;

			return w_len;
		}
		//同步写
		inline int write(char* buf)
		{
			assert(buf);
			return write(buf, (int)strlen(buf));
		}
	};

	class _asyn_com : public _base_com
	{
	protected:

		OVERLAPPED _ro, _wo; // 重叠I/O

		virtual bool open_port()
		{
			if (is_open())
				close();

			_com_handle = CreateFileA(
				_com_str,
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //重叠I/O
				NULL
			);
			assert(is_open());
			return is_open();//检测串口是否成功打开
		}

	public:

		_asyn_com()
		{
			memset(&_ro, 0, sizeof(_ro));
			memset(&_wo, 0, sizeof(_wo));

			_ro.hEvent = CreateEvent(NULL, true, false, NULL);
			assert(_ro.hEvent != INVALID_HANDLE_VALUE);

			_wo.hEvent = CreateEvent(NULL, true, false, NULL);
			assert(_wo.hEvent != INVALID_HANDLE_VALUE);
		}
		virtual ~_asyn_com()
		{
			close();

			if (_ro.hEvent != INVALID_HANDLE_VALUE)
				CloseHandle(_ro.hEvent);

			if (_wo.hEvent != INVALID_HANDLE_VALUE)
				CloseHandle(_wo.hEvent);
		}
		//异步读
		int read(char* buf, int buf_len, int time_wait = 20)
		{
			if (!is_open())
				return 0;
			buf[0] = '\0';
			COMSTAT  stat;
			DWORD error;
			if (ClearCommError(_com_handle, &error, &stat) && error > 0) //清除错误
			{
				PurgeComm(_com_handle, PURGE_RXABORT | PURGE_RXCLEAR); /*清除输入缓冲区*/
				return 0;
			}

			if (!stat.cbInQue)// 缓冲区无数据
				return 0;

			unsigned long r_len = 0;

			buf_len = std::min((int)(buf_len - 1), (int)stat.cbInQue);

			if (!ReadFile(_com_handle, buf, buf_len, &r_len, &_ro)) //2000 下 ReadFile 始终返回 True
			{
				if (GetLastError() == ERROR_IO_PENDING) // 结束异步I/O
				{
					WaitForSingleObject(_ro.hEvent, time_wait); //等待20ms
					if (!GetOverlappedResult(_com_handle, &_ro, &r_len, false))
					{
						if (GetLastError() != ERROR_IO_INCOMPLETE)//其他错误
							r_len = 0;
					}
				}
				else
					r_len = 0;
			}

			buf[r_len] = '\0';
			return r_len;
		}
		//异步写
		int write(char* buf, int buf_len)
		{
			if (!is_open())
				return 0;
			if (buf_len != 7)
			{
				DWORD    error;
				if (ClearCommError(_com_handle, &error, NULL) && error > 0) //清除错误
					PurgeComm(_com_handle, PURGE_TXABORT | PURGE_TXCLEAR);
			}

			unsigned long w_len = 0;
			if (!WriteFile(_com_handle, buf, buf_len, &w_len, &_wo))
				if (GetLastError() != ERROR_IO_PENDING)
					w_len = 0;
			return w_len;
		}
		//异步写
		inline int write(char* buf)
		{
			assert(buf);
			return write(buf, (int)strlen(buf));
		}

		inline int writeA(void* buf, int len)
		{
			assert(buf);
			return write((char*)buf, len);
		}
	};

	//当接受到数据送到窗口的消息
#define ON_COM_RECEIVE WM_USER + 618  //  WPARAM 端口号

	class _thread_com : public _asyn_com
	{
	protected:
		volatile HANDLE _thread_handle; //辅助线程
		volatile HWND _notify_hwnd; // 通知窗口
		volatile long _notify_num;//接受多少字节(>_notify_num)发送通知消息
		volatile bool _run_flag; //线程运行循环标志
		void (*_func)(int port);

		OVERLAPPED _wait_o; //WaitCommEvent use

		//线程收到消息自动调用, 如窗口句柄有效, 送出消息, 包含窗口编号
		virtual void on_receive()
		{
			if (_notify_hwnd)
				PostMessage(_notify_hwnd, ON_COM_RECEIVE, WPARAM(_port), LPARAM(0));
			else
			{
				if (_func)
					_func(_port);
			}
		}
		//打开串口,同时打开监视线程
		virtual bool open_port()
		{
			if (_asyn_com::open_port())
			{
				_run_flag = true;
				DWORD id;
				_thread_handle = CreateThread(NULL, 0, com_thread, this, 0, &id); //辅助线程
				assert(_thread_handle);
				if (!_thread_handle)
				{
					CloseHandle(_com_handle);
					_com_handle = INVALID_HANDLE_VALUE;
				}
				else
					return true;
			}
			return false;
		}

	public:
		_thread_com()
		{
			_notify_num = 0;
			_notify_hwnd = NULL;
			_thread_handle = NULL;
			_func = NULL;

			memset(&_wait_o, 0, sizeof(_wait_o));
			_wait_o.hEvent = CreateEvent(NULL, true, false, NULL);
			assert(_wait_o.hEvent != INVALID_HANDLE_VALUE);
		}
		~_thread_com()
		{
			close();

			if (_wait_o.hEvent != INVALID_HANDLE_VALUE)
				CloseHandle(_wait_o.hEvent);
		}

		static _thread_com& Instance()
		{
			static _thread_com instance;
			return instance;
		}

		void GetCOMPortByDescription(const std::string& targetDescription, std::unordered_map<std::string, int>& outMap) {
			HDEVINFO hDevInfo = SetupDiGetClassDevsA(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
			if (hDevInfo == INVALID_HANDLE_VALUE) return;

			if (outMap.size())
				outMap.clear();

			SP_DEVINFO_DATA deviceInfoData;
			deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

			for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &deviceInfoData); ++i) {
				char buf[512];
				DWORD nSize = 0;

				if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &deviceInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)buf, sizeof(buf), &nSize) && nSize > 0) {
					buf[nSize] = '\0';
					std::string deviceDescription = buf;

					size_t comPos = deviceDescription.find("COM");
					size_t endPos = deviceDescription.find(")", comPos);

					if (comPos != std::string::npos && endPos != std::string::npos && deviceDescription.find(targetDescription) != std::string::npos) {
						auto FindResult = outMap.find(deviceDescription);
						if (FindResult == outMap.end())
							outMap.emplace(deviceDescription, stoi(deviceDescription.substr(comPos + 3, endPos - comPos - 3)));
					}
				}
			}

			SetupDiDestroyDeviceInfoList(hDevInfo);
			return;
		}

		void Screen(int X, int Y)
		{
			char cmd[1024] = { 0 };
			sprintf_s(cmd, "km.Screen(%d, %d)\r\n", X, Y);
			this->write(cmd);
		}

		void Zero(int Value)
		{
			char cmd[1024] = { 0 };
			sprintf_s(cmd, "km.zero(%d)\r\n", Value);
			this->write(cmd);
		}

		int write_data(BYTE* dat, DWORD length)
		{
			if (length > 15) {
				return -2;
			}
			BYTE buf[64];
			memset(buf, 0xff, 64);
			memcpy(buf, dat, length);
			this->writeA(buf, 15);
			return 1;
		}

		int write_cmd(BYTE cmd, BYTE* dat, DWORD length)
		{
			if (length > 15 - 1) return -2;
			BYTE buf[63];
			buf[0] = (length + 1) & 0xff;
			buf[1] = cmd;
			if (length > 0)
				memcpy(buf + 2, dat, length);
			return write_data(buf, length + 2);
		}

		void mouse_event_control(BYTE e, SHORT x, SHORT y)
		{
			BYTE cmd[12];
			memset(cmd, 0, 12);
			cmd[0] = e;
			if (e >= 1 && e <= 7) {
				write_cmd(16, cmd, 5);
			}
			else if (e == 9) {
				if (x < -128 || x>127 || y < -128 || y>127) return;
				cmd[1] = (BYTE)x;
				cmd[2] = (BYTE)y;
				write_cmd(16, cmd, 5);
			}
			else if (e == 91) {
				if (x < -32768 || x>32767 || y < -32768 || y>32767) return;
				cmd[1] = (BYTE)((x & 0xff00) >> 8);
				cmd[2] = x & 0xff;
				cmd[3] = (BYTE)((y & 0xff00) >> 8);
				cmd[4] = y & 0xff;
				write_cmd(16, cmd, 5);
			}
			else if (e == 10) {
				if (x < -128 || x>127) return;
				cmd[1] = (BYTE)x;
				write_cmd(16, cmd, 5);
			}
			else if (e == 13 || e == 14) {
				cmd[1] = (BYTE)x;
				write_cmd(16, cmd, 5);
			}
		}

		void SendMoveJSMH(float X, float Y)
		{
			mouse_event_control(91,(SHORT)X,(SHORT)Y);
		}

		void LeftDownJSMH()
		{
			mouse_event_control(1,0,0);
		}

		void LeftUpJSMH()
		{
			mouse_event_control(2, 0, 0);
		}

		void SendMoveLurker(float X, float Y)
		{
			BYTE SendMove[] = { 0x57,0xAB,0x02,0x00,0x00,0x00,0x00 };

			if (X < 0.f)
				if (X >= -127.f)
					X = 256.f + X;
				else
					X = 129.f;
			else
				if (X > 127.f)
					X = 127.f;
			if (Y < 0.f)
				if (Y >= -127.f)
					Y = 256.f + Y;
				else
					Y = 129.f;
			else
				if (Y > 127.f)
					Y = 127.f;

			BYTE MoveX = (BYTE)X;
			BYTE MoveY = (BYTE)Y;
			*(BYTE*)(SendMove + 4) = MoveX;
			*(BYTE*)(SendMove + 5) = MoveY;
			this->writeA((void*)SendMove, 7);
		}

		void ClickLurker(int But)
		{
			BYTE SendMove[] = { 0x57,0xAB,0x02,0x00,0x00,0x00,0x00 };
			*(BYTE*)(SendMove + 3) = (BYTE)But;
			this->writeA((void*)SendMove, 7);
		}

		void SetFreq(int Freq)
		{
			char cmd[1024] = { 0 };
				sprintf_s(cmd, "km.freq(%d)\r\n", Freq);
			this->write(cmd);
		}

		void SendMove(int X, int Y, int Setp = 0)
		{
			char cmd[1024] = { 0 };
			if (Setp == 0)
				sprintf_s(cmd, "km.move(%d, %d)\r\n", X, Y);
			else
				sprintf_s(cmd, "km.move(%d, %d, %d)\r\n", X, Y, Setp);
			this->write(cmd);
		}

		void Click(int But)
		{
			char cmd[1024] = { 0 };
			sprintf_s(cmd, "km.click(%d)\r\n", But);
			this->write(cmd);
		}

		void downLeft(int But)
		{
			char cmd[1024] = { 0 };
			sprintf_s(cmd, "km.left(%d)\r\n", But);
			this->write(cmd);
		}
		//设定发送通知, 接受字符最小值
		void set_notify_num(int num)
		{
			_notify_num = num;
		}
		int get_notify_num()
		{
			return _notify_num;
		}
		//送消息的窗口句柄
		inline void set_hwnd(HWND hWnd)
		{
			_notify_hwnd = hWnd;
		}
		inline HWND get_hwnd()
		{
			return _notify_hwnd;
		}
		inline void set_func(void (*f)(int))
		{
			_func = f;
		}
		//关闭线程及串口
		virtual void close()
		{
			if (is_open())
			{
				_run_flag = false;
				SetCommMask(_com_handle, 0);
				SetEvent(_wait_o.hEvent);

				if (WaitForSingleObject(_thread_handle, 100) != WAIT_OBJECT_0)
					TerminateThread(_thread_handle, 0);

				CloseHandle(_com_handle);
				CloseHandle(_thread_handle);

				_thread_handle = NULL;
				_com_handle = INVALID_HANDLE_VALUE;
				ResetEvent(_wait_o.hEvent);
			}
		}
		/*辅助线程控制*/
		//获得线程句柄
		HANDLE get_thread()
		{
			return _thread_handle;
		}
		//暂停监视线程
		bool suspend()
		{
			return _thread_handle != NULL ? SuspendThread(_thread_handle) != 0xFFFFFFFF : false;
		}
		//恢复监视线程
		bool resume()
		{
			return _thread_handle != NULL ? ResumeThread(_thread_handle) != 0xFFFFFFFF : false;
		}
		//重建监视线程
		bool restart()
		{
			if (_thread_handle) /*只有已有存在线程时*/
			{
				_run_flag = false;
				SetCommMask(_com_handle, 0);
				SetEvent(_wait_o.hEvent);

				if (WaitForSingleObject(_thread_handle, 100) != WAIT_OBJECT_0)
					TerminateThread(_thread_handle, 0);

				CloseHandle(_thread_handle);

				_run_flag = true;
				_thread_handle = NULL;

				DWORD id;
				_thread_handle = CreateThread(NULL, 0, com_thread, this, 0, &id);
				return (_thread_handle != NULL); //辅助线程
			}
			return false;
		}

	private:
		//监视线程
		static DWORD WINAPI com_thread(LPVOID para)
		{
			_thread_com* pcom = (_thread_com*)para;


			if (!SetCommMask(pcom->_com_handle, EV_RXCHAR | EV_ERR))
				return 0;

			COMSTAT  stat;
			DWORD error;

			for (DWORD length, mask = 0; pcom->_run_flag && pcom->is_open(); mask = 0)
			{
				if (!WaitCommEvent(pcom->_com_handle, &mask, &pcom->_wait_o))
				{
					if (GetLastError() == ERROR_IO_PENDING)
					{
						GetOverlappedResult(pcom->_com_handle, &pcom->_wait_o, &length, true);
					}
				}

				if (mask & EV_ERR) // == EV_ERR
					ClearCommError(pcom->_com_handle, &error, &stat);

				if (mask & EV_RXCHAR) // == EV_RXCHAR
				{
					ClearCommError(pcom->_com_handle, &error, &stat);
					if (stat.cbInQue > (DWORD)pcom->_notify_num)
						pcom->on_receive();
				}
			}

			return 0;
		}

	};
	typedef _thread_com _com; //名称简化

	inline _thread_com& GetKmbox() { return _thread_com::Instance(); };

#endif //_COM_H_
}