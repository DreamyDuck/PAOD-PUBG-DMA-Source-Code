#include "Include/WinHeaders.hpp"
#include "Menu.h"
#include "Config.h"
#include "DirectX/DirectX11.h"
#include "ItemManager.h"
#include "Colorset.h"
#include "FilePak.h"
#include "SDK/SDK.h"
#include "SkinWeaponManager.h"

#define LOCALIZE_Menu(en,cn) (Vars.Menu.language ? en: cn)
bool enableHideFunc = false;
bool enableBunnyHop = false;
std::string hudPath1;
std::string hudPath2;
bool setInitPID = false;
ImVec2 MousePos;
ImVec2 MenuPos;
ImVec2 BMenuPos;
ImVec2 BMenuSize;
std::string RadarIP;
int HideFunction = 0;
namespace IronMan::Core
{
	extern std::shared_ptr<std::unordered_map<hash_t, std::vector<SDK::FSkinMapData>>> WeapSkinMap;
	std::unordered_map<std::string, int>KmboxName;
	const char* Controller_items[] = { "KMBOX", "KMBOX Net","Lurker",u8"鍵鼠魔盒" };
	const char* Display_itemsCN[] = { u8"串流媒體", u8"融合顯示" };
	const char* Display_itemsEN[] = { "Stream", "Fusion display" };
	const char* Rank_items[] = { "Steam", "Kakao","Psn","Stadia" };

	static int CurrentKmboxItem = 0;
	bool FirstInitKmbox = false;

	static char* BackLv1Array[200] = { 0 };
	static int BackLv1Size = 0;
	static char* BackLv2Array[200] = { 0 };
	static int BackLv2Size = 0;
	static char* BackLv3Array[200] = { 0 };
	static int BackLv3Size = 0;
	static char* ArmorLv1Array[200] = { 0 };
	static int ArmorLv1Size = 0;
	static char* ArmorLv2Array[200] = { 0 };
	static int ArmorLv2Size = 0;
	static char* ArmorLv3Array[200] = { 0 };
	static int ArmorLv3Size = 0;
	static char* HelmetLv1Array[200] = { 0 };
	static int HelmetLv1Size = 0;
	static char* HelmetLv2Array[200] = { 0 };
	static int HelmetLv2Size = 0;
	static char* HelmetLv3Array[200] = { 0 };
	static int HelmetLv3Size = 0;


	char AimHotKey[256][64] =
	{
		"LB",//0-1
		"RB",//1-2
		"MB",//2-3
		"XB1",//3-5
		"XB2",//4-6
		"5",//5
		"6",//6
		"7",//7
		"Back",//8
		"Tab",//9
		"10",//10
		"11",//11
		"12",//12
		"Ent",//13
		"14",//14
		"15",//15
		"SFT",//16
		"CTL",//17
		"ALT",//18
		"PAU",//19
		"CAP",//20
		"21",//21
		"22",//22
		"23",//23
		"24",//24
		"25",//25
		"26",//26
		"OFF",//27
		"28",//28
		"29",//29
		"30",//30
		"31",//31
		"SP",//32
		"PU",//33
		"PD",//34
		"END",//35
		"HOME",//36
		"L",//37
		"UP",//38
		"R",//39
		"DOWN",//40
		"41",//41
		"42",//42
		"43",//43
		"44",//44
		"INS",//45
		"DEL",//46
		"47",//47
		"0",//48
		"1",//49
		"2",//50
		"3",//51
		"4",//52
		"5",//53
		"6",//54
		"7",//55
		"8",//56
		"9",//57
		"58",//58
		"59",//59
		"60",//60
		"61",//61
		"62",//62
		"63",//63
		"64",//64
		"A",//65
		"B",//66
		"C",//67
		"D",//68
		"E",//69
		"F",//70
		"G",//71
		"H",//72
		"I",//73
		"J",//74
		"K",//75
		"L",//76
		"M",//77
		"N",//78
		"O",//79
		"P",//80
		"Q",//81
		"R",//82
		"S",//83
		"T",//84
		"U",//85
		"V",//86
		"W",//87
		"X",//88
		"Y",//89
		"Z",//90
		"WIN",//91
		"92",//92
		"WIN",//93
		"94",//94
		"95",//95
		"0",//96
		"1",//97
		"2",//98
		"3",//99
		"4",//100
		"5",//101
		"6",//102
		"7",//103
		"8",//104
		"9",//105
		"*",//106
		"+",//107
		"108",//108
		"-",//109
		".",//110
		"/",//111
		"F1",//112
		"F2",//113
		"F3",//114
		"F4",//115
		"F5",//116
		"F6",//117
		"F7",//118
		"F8",//119
		"F9",//120
		"F10",//121
		"F11",//122
		"F12",//123
		"124",//124
		"125",//125
		"126",//126
		"127",//127
		"128",//128
		"129",//129
		"130",//130
		"131",//131
		"132",//132
		"133",//133
		"134",//134
		"135",//135
		"136",//136
		"137",//137
		"138",//138
		"139",//139
		"140",//140
		"141",//141
		"142",//142
		"143",//143
		"NUM",//144
		"SCR",//145
		"146",//146
		"147",//147
		"148",//148
		"149",//149
		"150",//150
		"151",//151
		"152",//152
		"153",//153
		"154",//154
		"155",//155
		"156",//156
		"157",//157
		"158",//158
		"159",//159
		"160",//160
		"161",//161
		"162",//162
		"163",//163
		"164",//164
		"165",//165
		"166",//166
		"167",//167
		"168",//168
		"169",//169
		"170",//170
		"171",//171
		"172",//172
		"173",//173
		"174",//174
		"175",//175
		"176",//176
		"177",//177
		"178",//178
		"179",//179
		"180",//180
		"181",//181
		"182",//182
		"183",//183
		"184",//184
		"185",//185
		";'",//186
		"='",//187
		",'",//188
		"-'",//189
		".'",//190
		"/'",//191
		"~'",//192
		"193",//193
		"194",//194
		"195",//195
		"196",//196
		"197",//197
		"198",//198
		"199",//199
		"200",//200
		"201",//201
		"202",//202
		"203",//203
		"204",//204
		"205",//205
		"206",//206
		"207",//207
		"208",//208
		"209",//209
		"210",//210
		"211",//211
		"212",//212
		"213",//213
		"214",//214
		"215",//215
		"216",//216
		"217",//217
		"218",//218
		"['",//219
		"220",//220
		"]'",//221
		"\"'",//222
		"223",//223
		"224",//224
		"225",//225
		"226",//226
		"227",//227
		"228",//228
		"229",//229
		"230",//230
		"231",//231
		"232",//232
		"233",//233
		"234",//234
		"235",//235
		"236",//236
		"237",//237
		"238",//238
		"239",//239
		"240",//240
		"241",//241
		"242",//242
		"243",//243
		"244",//244
		"245",//245
		"246",//246
		"247",//247
		"248",//248
		"249",//249
		"250",//250
		"251",//251
		"252",//252
		"253",//253
		"254",//254
		"255",//255
	};

	int iTab = 0;
	long long g_expireDate = 0;
	std::vector<int>SaveHotKey;
	char NameText1[32];
	bool IsFind1 = false;
	char NameText2[32];
	bool IsFind2 = false;
	char NameText3[32];
	bool IsFind3 = false;
	char NameText4[32];
	bool IsFind4 = false;
	char NameText5[32];
	bool IsFind5 = false;
	char NameText6[32];
	bool IsFind6 = false;
	char NameText7[32];
	bool IsFind7 = false;
	char NameText8[32];
	bool IsFind8 = false;
	char NameText9[32];
	bool IsFind9 = false;
	char NameText10[32];
	bool IsFind10 = false;
	char NameText11[32];
	bool IsFind11 = false;
	char NameText12[32];
	bool IsFind12 = false;
	char NameText13[32];
	bool IsFind13 = false;
	char NameText14[32];
	bool IsFind14 = false;
	char NameText15[32];
	bool IsFind15 = false;
	char NameText16[32];
	bool IsFind16 = false;
	char NameText17[32];
	bool IsFind17 = false;
	char NameText18[32];
	bool IsFind18 = false;
	char NameText19[32];
	bool IsFind19 = false;
	char NameText20[32];
	bool IsFind20 = false;
	float RainbowColor = 0.f;

	void imgui_rainbow_rect(int x, int y, int width, int height, float flSpeed, float& flRainbow)
	{
		ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

		Colorset colColor(0, 0, 0);

		flRainbow += flSpeed;
		if (flRainbow > 1.f)
			flRainbow = 0.f;

		for (int i = 0; i < height; i++)
		{
			float hue = (1.f / (float)height) * i;
			hue -= flRainbow;
			if (hue < 0.f) hue += 1.f;

			Colorset colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
			windowDrawList->AddRectFilled(ImVec2(x, y + i), ImVec2(x + width, height), colRainbow.GetU32());
		}
	}


	bool Menu::m_English = false;
	ImTextureID Menu::VisualNormal;
	ImTextureID Menu::VisualActive;
	ImTextureID Menu::AimbotNormal;
	ImTextureID Menu::AimbotActive;
	ImTextureID Menu::MiscNormal;
	ImTextureID Menu::MiscActive;
	ImTextureID Menu::WeaponNormal;
	ImTextureID Menu::WeaponActive;
	ImTextureID Menu::BoxNormal1;
	ImTextureID Menu::BoxActive1;
	ImTextureID Menu::BoxNormal2;
	ImTextureID Menu::BoxActive2;
	ImTextureID Menu::Project1Normal;
	ImTextureID Menu::Project1Active;
	ImTextureID Menu::Project2Normal;
	ImTextureID Menu::Project2Active;
	ImTextureID Menu::ChineseNormal;
	ImTextureID Menu::ChineseActive;
	ImTextureID Menu::EnglishNormal;
	ImTextureID Menu::EnglishActive;
	ImTextureID Menu::HumanMoveable;
	ImTextureID Menu::WeaponHold;

	bool AutoConnectRadar = false;
	HANDLE ConnectedThread = NULL;
	int ConnectState = 0;
	bool IsTryConnected = false;
	ptr_t ConnectedStartTime = 0;

	HANDLE BNetThread = NULL;
	ptr_t ConnectedBNetStartTime = 0;

	ImVec2 RenderText(const ImFont* font, float fontSize, float clip_w, ImVec2 pos, ImU32 col, ImU32 outlineCol, DWORD alignment, const char* text, ...)
	{
		auto drawList = ImGui::GetCurrentWindow()->DrawList;

		va_list va_alist;
		char buf[1024];
		va_start(va_alist, text);
		_vsnprintf_s(buf, sizeof(buf), text, va_alist);
		va_end(va_alist);

		// Pull default font/size from the shared ImDrawListSharedData instance
		if (font == NULL)
			font = drawList->_Data->Font;
		if (fontSize == 0.0f)
			fontSize = font->FontSize;


		ImVec2 text_size = font->CalcTextSizeA(fontSize, FLT_MAX, -1.0f, buf, nullptr, NULL);
		// Round
		text_size.x = (float)(int)(text_size.x + 0.95f);

		if (alignment & FONT_RIGHT)
			pos.x -= text_size.x;
		if (alignment & FONT_CENTER)
			pos.x -= text_size.x / 2;

		pos.y -= text_size.y / 2;
		ImVec4* cpu_fine_clip_rect = nullptr;
		if (clip_w > 0)
		{
			cpu_fine_clip_rect = reinterpret_cast<ImVec4*>(_malloca(sizeof(ImVec4)));
			cpu_fine_clip_rect->x = 0;
			cpu_fine_clip_rect->y = 0;
			cpu_fine_clip_rect->z = pos.x + clip_w;
			cpu_fine_clip_rect->w = pos.y + text_size.y;
		}
		if (outlineCol != 0)
		{
			drawList->AddText(font, fontSize, pos - ImVec2(1.0f, 0.0f), ImGui::GetColorU32(outlineCol), buf, nullptr, 0, cpu_fine_clip_rect);
			drawList->AddText(font, fontSize, pos - ImVec2(0.0f, 1.0f), ImGui::GetColorU32(outlineCol), buf, nullptr, 0, cpu_fine_clip_rect);
			drawList->AddText(font, fontSize, pos + ImVec2(1.0f, 0.0f), ImGui::GetColorU32(outlineCol), buf, nullptr, 0, cpu_fine_clip_rect);
			drawList->AddText(font, fontSize, pos + ImVec2(0.0f, 1.0f), ImGui::GetColorU32(outlineCol), buf, nullptr, 0, cpu_fine_clip_rect);
		}
		drawList->AddText(font, fontSize, pos, ImGui::GetColorU32(col), buf, nullptr, 0, cpu_fine_clip_rect);
		return text_size;
	}

	void RenderTabName(std::string Name)
	{
		ImVec2 NextPos = ImGui::GetNextWindowPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(NextPos.x + 326.f, NextPos.y + 6.f), ImVec2(NextPos.x + 326.f + 180.f, NextPos.y + 6.f + 22.f), ImColor(42, 45, 54), 14.f);
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(NextPos.x + 484.f, NextPos.y + 6.f), ImVec2(NextPos.x + 484.f + 42.f, NextPos.y + 6.f + 22.f), ImGui::GetColorU32(ImVec4(0.8549f, 0.3686f, 0.3686f, 1.0f)), 14.f);
		RenderText(OverlayEngine::Font16px, 16, 0, ImVec2(NextPos.x + 416.f, NextPos.y + 16.f), 0xFFFFFFFF, 0, FONT_CENTER, Name.c_str());
		RenderText(OverlayEngine::Font16px, 20, 0, ImVec2(NextPos.x + 506.f, NextPos.y + 16.f), 0xFFFFFFFF, 0, FONT_CENTER, (u8"="));
	}

	void SetDefaultAimSettings()
	{
		Vars.Aimbot.HighAimbot = false;
		Vars.Aimbot.AimbotDrag = false;
		Vars.Aimbot.DownValue = 0.f;
		Vars.Aimbot.Aimbot = true;
		Vars.Aimbot.CheckReloading = true;

		Vars.Aimbot.GrenadePredict = true;
		Vars.Aimbot.HandNotLock = true;
		Vars.Aimbot.VisibleCheck = true;
		Vars.Aimbot.AR_Knock = true;
		Vars.Aimbot.SR_Knock = true;
		Vars.Aimbot.DMR_Knock = true;
		Vars.Aimbot.AR_PredictionPoint = true;
		Vars.Aimbot.SR_PredictionPoint = true;
		Vars.Aimbot.DMR_PredictionPoint = true;
		Vars.Aimbot.Team = true;
		Vars.Aimbot.FovShow = true;
		Vars.Aimbot.Crosshair = true;
		Vars.Aimbot.CrosshairSize = 5.f;

		Vars.Aimbot.SingleStep = true;
		Vars.Aimbot.SR_MOVESTEP_X1 = 5.f;
		Vars.Aimbot.SR_MOVESTEP_Y1 = 5.f;
		Vars.Aimbot.DMR_MOVESTEP_X1 = 5.f;
		Vars.Aimbot.DMR_MOVESTEP_Y1 = 5.f;

		Vars.Aimbot.SR_MOVESTEP_X2 = 5.f;
		Vars.Aimbot.SR_MOVESTEP_Y2 = 5.f;
		Vars.Aimbot.DMR_MOVESTEP_X2 = 5.f;
		Vars.Aimbot.DMR_MOVESTEP_Y2 = 5.f;

		Vars.Aimbot.ShotGunAutoShot = true;
		Vars.Aimbot.SRGunAutoShot = false;
		Vars.Aimbot.AutoShot = false;
		Vars.Aimbot.AimWhileScoping = false;

		Vars.Aimbot.AimMode = 0;

		Vars.Aimbot.AR_NoRecoil1 = true;
		Vars.Aimbot.AR_P_X1 = 0.12f;
		Vars.Aimbot.AR_P_Y1 = 0.12f;
		Vars.Aimbot.AR_RecoilValue1 = 100.f;
		Vars.Aimbot.AR_SmoothValue1 = 25.f;
		Vars.Aimbot.AR_FovRange1 = 5.f;

		Vars.Aimbot.AR_NoRecoil2 = true;
		Vars.Aimbot.AR_P_X2 = 0.35f;
		Vars.Aimbot.AR_P_Y2 = 0.35f;
		Vars.Aimbot.AR_RecoilValue2 = 100.f;
		Vars.Aimbot.AR_SmoothValue2 = 35.f;
		Vars.Aimbot.AR_FovRange2 = 5.f;

		Vars.Aimbot.SR_NoRecoil1 = false;
		Vars.Aimbot.SR_baseSmoothing1 = 2.5f;
		Vars.Aimbot.SR_RecoilValue1 = 100.f;
		Vars.Aimbot.SR_SmoothValue1 = 15.f;
		Vars.Aimbot.SR_FovRange1 = 6.f;

		Vars.Aimbot.SR_NoRecoil2 = false;
		Vars.Aimbot.SR_baseSmoothing2 = 2.2f;
		Vars.Aimbot.SR_RecoilValue2 = 100.f;
		Vars.Aimbot.SR_SmoothValue2 = 25.f;
		Vars.Aimbot.SR_FovRange2 = 8.f;

		Vars.Aimbot.DMR_NoRecoil1 = false;
		Vars.Aimbot.DMR_baseSmoothing1 = 3.3f;
		Vars.Aimbot.DMR_RecoilValue1 = 100.f;
		Vars.Aimbot.DMR_SmoothValue1 = 15.f;
		Vars.Aimbot.DMR_FovRange1 = 6.f;

		Vars.Aimbot.DMR_NoRecoil2 = true;
		Vars.Aimbot.DMR_baseSmoothing2 = 2.5f;
		Vars.Aimbot.DMR_RecoilValue2 = 100.f;
		Vars.Aimbot.DMR_SmoothValue2 = 35.f;
		Vars.Aimbot.DMR_FovRange2 = 8.f;

		Vars.Aimbot.SR_baseFOV = 10.f;
		Vars.Aimbot.SR_MaxSmoothIncrease = 0.20f;
		Vars.Aimbot.DMR_baseFOV = 10.f;
		Vars.Aimbot.DMR_MaxSmoothIncrease;

		Vars.Aimbot.I_X = 0.10f;
		Vars.Aimbot.D_X = 0.10f;
		Vars.Aimbot.I_Y = 0.10f;
		Vars.Aimbot.D_Y = 0.10f;
	}

	auto InitComboArray = [=](auto InArray, auto& OutArray, int Current, int* SetID)
		{
			int Numi = 0;
			for (auto iter : InArray)
			{
				try
				{
					if (Numi == 0)
					{
						if (!OutArray[Numi])
						{
							OutArray[Numi] = (char*)malloc(0x10);
							ZeroMemory(OutArray[Numi], 0x10);
							strcat(OutArray[Numi], "None");
						}
						Numi++;
					}
					if (!OutArray[Numi])
					{
						auto weaponData = SkinWeaponManager::GetHandWeapon(hash_(iter.SkinItemName));
						if (weaponData && !weaponData->formatName_.empty())
						{
							OutArray[Numi] = (char*)malloc(weaponData->formatName_.size() + 1);
							ZeroMemory(OutArray[Numi], weaponData->formatName_.size());
							strcat(OutArray[Numi], (char*)weaponData->formatName_.data());
						}
						else
						{
							OutArray[Numi] = (char*)malloc(iter.SkinItemName.size() + 1);
							ZeroMemory(OutArray[Numi], iter.SkinItemName.size() + 1);
							strcat(OutArray[Numi], (char*)iter.SkinItemName.data());
						}
					}
					else
					{
						auto weaponData = SkinWeaponManager::GetHandWeapon(hash_(iter.SkinItemName));
						if (weaponData && !weaponData->formatName_.empty())
						{
							if (strcmp(OutArray[Numi], weaponData->formatName_.data()) != 0)
							{
								ZeroMemory(OutArray[Numi], weaponData->formatName_.size() + 1);
								strcat(OutArray[Numi], (char*)weaponData->formatName_.data());
							}

						}
						else
						{
							if (strcmp(OutArray[Numi], iter.SkinItemName.data()) != 0)
							{
								ZeroMemory(OutArray[Numi], iter.SkinItemName.size() + 1);
								strcat(OutArray[Numi], (char*)iter.SkinItemName.data());
							}
						}
					}
					if (Current == 0)
						*SetID = 0;
					else if (Current == Numi)
						*SetID = iter.SkinItemID.ComparisonIndex;
				}
				catch (...)
				{

				}
				Numi++;
			}
		};

	DWORD ThreadTryConnect(LPVOID s)
	{
		std::string IPString = std::string((char*)s);
		while (IPString.back() == '/')
		{
			if (IPString.back() == '/')
			{
				IPString.pop_back();
			}
		}
		if (IPString.find("127.0.0.1") != std::string::npos || IPString.find("localhost") != std::string::npos)
		{
			IPString += ":39111";
		}
		httplib::Client cliSNT(IPString);

		auto Result = cliSNT.Get("/map?id=123");
		if (!Result)
			return 1;
		auto ResString = Result->body;
		if (ResString.find("timestamp") != std::string::npos)
		{
			ConnectState = 2;
			IsTryConnected = false;
			Utils::hasNullTerminator(IPString.data(), IPString.size() + 1);
			RadarIP = std::string(IPString.data());
			CONSOLE_INFO2(u8"连接到本地共享雷达成功!");
			CONSOLE_INFO2(u8"雷达访问地址:%s", IPString.c_str());
			Vars.Misc.ShareRadarState = true;
		}
		else
		{
			IsTryConnected = false;
			ConnectState = 0;
			ConnectedStartTime = 0;
			Vars.Misc.ShareRadarState = false;
		}
		return 1;
	}

	DWORD ThreadConnectBoxNet(LPVOID s)
	{
		if (Vars.Aimbot.BNetStatus == 0)
		{
			if (kmNet_init(Vars.Aimbot.txt_IP, Vars.Aimbot.txt_Port, Vars.Aimbot.txt_UUID) == 0)
			{
				Vars.Aimbot.BNetStatus = 1;
				Vars.Aimbot.AimBotMethod = 2;
				Config->Save(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}
		}
		else
		{
			Vars.Aimbot.BNetStatus = 0;
			Vars.Aimbot.AimBotMethod = 0;
		}
		return 1;
	}

	bool Menu::Initialize(bool isEng)
	{
		m_English = isEng;
		void* hudVisualNormal = nullptr, * hudVisualActive = nullptr, * hudAimbotNormal = nullptr, * hudAimbotActive = nullptr, * hudMiscNormal = nullptr,
			* hudMiscActive = nullptr, * hudWeaponNormal = nullptr, * hudWeaponActive = nullptr, * hudBoxNormal1 = nullptr, * hudBoxActive1 = nullptr,
			* hudBoxNormal2 = nullptr, * hudBoxActive2 = nullptr, * hudProject1Normal = nullptr, * hudProject1Active = nullptr, * hudProject2Normal = nullptr,
			* hudProject2Active = nullptr, * hudChineseNormal = nullptr, * hudChineseActive = nullptr, * hudEnglishNormal = nullptr, * hudEnglishActive = nullptr,
			* hudHumanMoveable = nullptr, * hudWeaponHold = nullptr;
		size_t retSize;
		{
			retSize = Package::GetPackage()->FindFile(HASH("人物关.png"), (void**)&hudVisualNormal);
			if (hudVisualNormal)
				Menu::VisualNormal = OverlayEngine::CreateTexture((const char*)hudVisualNormal, retSize);
			else
				MessageBoxA(0, "hudhudVisualNormal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("人物开.png"), (void**)&hudVisualActive);
			if (hudVisualActive)
				Menu::VisualActive = OverlayEngine::CreateTexture((const char*)hudVisualActive, retSize);
			else
				MessageBoxA(0, "hudhudVisualActive", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("自瞄关.png"), (void**)&hudAimbotNormal);
			if (hudAimbotNormal)
				Menu::AimbotNormal = OverlayEngine::CreateTexture((const char*)hudAimbotNormal, retSize);
			else
				MessageBoxA(0, "hudAimbotNormal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("自瞄开.png"), (void**)&hudAimbotActive);
			if (hudAimbotActive)
				Menu::AimbotActive = OverlayEngine::CreateTexture((const char*)hudAimbotActive, retSize);
			else
				MessageBoxA(0, "hudAimbotActive", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("杂项设置关.png"), (void**)&hudMiscNormal);
			if (hudMiscNormal)
				Menu::MiscNormal = OverlayEngine::CreateTexture((const char*)hudMiscNormal, retSize);
			else
				MessageBoxA(0, "hudMiscNormal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("杂项设置开.png"), (void**)&hudMiscActive);
			if (hudMiscActive)
				Menu::MiscActive = OverlayEngine::CreateTexture((const char*)hudMiscActive, retSize);
			else
				MessageBoxA(0, "hudMiscActive", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("项目1关.png"), (void**)&hudProject1Normal);
			if (hudProject1Normal)
				Menu::Project1Normal = OverlayEngine::CreateTexture((const char*)hudProject1Normal, retSize);
			else
				MessageBoxA(0, "hudProject1Normal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("项目1开.png"), (void**)&hudProject1Active);
			if (hudProject1Active)
				Menu::Project1Active = OverlayEngine::CreateTexture((const char*)hudProject1Active, retSize);
			else
				MessageBoxA(0, "hudProject1Active", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("枪支关.png"), (void**)&hudWeaponNormal);
			if (hudWeaponNormal)
				Menu::WeaponNormal = OverlayEngine::CreateTexture((const char*)hudWeaponNormal, retSize);
			else
				MessageBoxA(0, "hudWeaponNormal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("枪支开.png"), (void**)&hudWeaponActive);
			if (hudWeaponActive)
				Menu::WeaponActive = OverlayEngine::CreateTexture((const char*)hudWeaponActive, retSize);
			else
				MessageBoxA(0, "hudWeaponActive", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("箱子关1.png"), (void**)&hudBoxNormal1);
			if (hudBoxNormal1)
				Menu::BoxNormal1 = OverlayEngine::CreateTexture((const char*)hudBoxNormal1, retSize);
			else
				MessageBoxA(0, "hudBoxNormal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("箱子开1.png"), (void**)&hudBoxActive1);
			if (hudBoxActive1)
				Menu::BoxActive1 = OverlayEngine::CreateTexture((const char*)hudBoxActive1, retSize);
			else
				MessageBoxA(0, "hudBoxActive", 0, 0);

			retSize = Package::GetPackage()->FindFile(HASH("箱子关2.png"), (void**)&hudBoxNormal2);
			if (hudBoxNormal2)
				Menu::BoxNormal2 = OverlayEngine::CreateTexture((const char*)hudBoxNormal2, retSize);
			else
				MessageBoxA(0, "hudBoxNormal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("箱子开2.png"), (void**)&hudBoxActive2);
			if (hudBoxActive2)
				Menu::BoxActive2 = OverlayEngine::CreateTexture((const char*)hudBoxActive2, retSize);
			else
				MessageBoxA(0, "hudBoxActive", 0, 0);

			retSize = Package::GetPackage()->FindFile(HASH("项目2关.png"), (void**)&hudProject2Normal);
			if (hudProject2Normal)
				Menu::Project2Normal = OverlayEngine::CreateTexture((const char*)hudProject2Normal, retSize);
			else
				MessageBoxA(0, "hudProject2Normal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("项目2开.png"), (void**)&hudProject2Active);
			if (hudProject2Active)
				Menu::Project2Active = OverlayEngine::CreateTexture((const char*)hudProject2Active, retSize);
			else
				MessageBoxA(0, "hudProject2Active", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("繁体字关.png"), (void**)&hudChineseNormal);
			if (hudChineseNormal)
				Menu::ChineseNormal = OverlayEngine::CreateTexture((const char*)hudChineseNormal, retSize);
			else
				MessageBoxA(0, "hudChineseNormal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("繁体字开.png"), (void**)&hudChineseActive);
			if (hudChineseActive)
				Menu::ChineseActive = OverlayEngine::CreateTexture((const char*)hudChineseActive, retSize);
			else
				MessageBoxA(0, "hudChineseActive", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("英文关.png"), (void**)&hudEnglishNormal);
			if (hudEnglishNormal)
				Menu::EnglishNormal = OverlayEngine::CreateTexture((const char*)hudEnglishNormal, retSize);
			else
				MessageBoxA(0, "hudEnglishNormal", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("英文开.png"), (void**)&hudEnglishActive);
			if (hudEnglishActive)
				Menu::EnglishActive = OverlayEngine::CreateTexture((const char*)hudEnglishActive, retSize);
			else
				MessageBoxA(0, "hudEnglishActive", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("HumanTexture.png"), (void**)&hudHumanMoveable);
			if (hudHumanMoveable)
				Menu::HumanMoveable = OverlayEngine::CreateTexture((const char*)hudHumanMoveable, retSize);
			else
				MessageBoxA(0, "hudHumanMoveable", 0, 0);
			retSize = Package::GetPackage()->FindFile(HASH("WeaponHold.png"), (void**)&hudWeaponHold);
			if (hudWeaponHold)
				Menu::WeaponHold = OverlayEngine::CreateTexture((const char*)hudWeaponHold, retSize);
			else
				MessageBoxA(0, "hudWeaponHold", 0, 0);
		}

		if (!hudVisualNormal || !hudVisualActive || !hudAimbotNormal || !hudAimbotActive || !hudMiscNormal || !hudMiscActive || !hudProject1Normal || !hudProject1Active
			|| !hudWeaponNormal || !hudWeaponActive || !hudBoxNormal1 || !hudBoxActive1 || !hudProject2Normal || !hudProject2Active || !hudChineseNormal || !hudChineseActive
			|| !hudEnglishNormal || !hudEnglishActive || !hudHumanMoveable || !hudWeaponHold || !hudBoxNormal2 || !hudBoxActive2)
		{
			MessageBoxA(NULL, "Menu Res NotFind! - MenuIcon", "ERROR", MB_OK | MB_ICONHAND);
			return false;
		}

		if (hudVisualNormal) { free((void*)hudVisualNormal); }
		if (hudVisualActive) { free((void*)hudVisualActive); }
		if (hudAimbotNormal) { free((void*)hudAimbotNormal); }
		if (hudAimbotActive) { free((void*)hudAimbotActive); }
		if (hudMiscNormal) { free((void*)hudMiscNormal); }
		if (hudMiscActive) { free((void*)hudMiscActive); }
		if (hudProject1Normal) { free((void*)hudProject1Normal); }
		if (hudProject1Active) { free((void*)hudProject1Active); }
		if (hudWeaponNormal) { free((void*)hudWeaponNormal); }
		if (hudWeaponActive) { free((void*)hudWeaponActive); }
		if (hudBoxNormal1) { free((void*)hudBoxNormal1); }
		if (hudBoxActive1) { free((void*)hudBoxActive1); }
		if (hudBoxNormal2) { free((void*)hudBoxNormal2); }
		if (hudBoxActive2) { free((void*)hudBoxActive2); }
		if (hudProject2Normal) { free((void*)hudProject2Normal); }
		if (hudProject2Active) { free((void*)hudProject2Active); }
		if (hudChineseNormal) { free((void*)hudChineseNormal); }
		if (hudChineseActive) { free((void*)hudChineseActive); }
		if (hudEnglishNormal) { free((void*)hudEnglishNormal); }
		if (hudEnglishActive) { free((void*)hudEnglishActive); }
		if (hudHumanMoveable) { free((void*)hudHumanMoveable); }
		if (hudWeaponHold) { free((void*)hudWeaponHold); }
		OverlayEngine::OnPresent += OnRender;
		//return true;
		return ItemManager::GetManager()->Initialize();
	}

	void Menu::OnRender()
	{
		RenderAnnoce();
		ProcessInputMessage();
		MousePos = ImGui::GetMousePos();

		static float BackupAR_PX = 0.f;
		static float BackupAR_PY = 0.f;
		static float BackupAR_PX2 = 0.f;
		static float BackupAR_PY2 = 0.f;
		static float BackupWeaponType = 0.f;
		static int BackupAimType = 0;

		auto WeaponType = SDK::sdk::GetWeaponType();

		if (BackupAR_PX != Vars.Aimbot.AR_P_X1 || BackupAR_PY != Vars.Aimbot.AR_P_Y1
			|| BackupAR_PX2 != Vars.Aimbot.AR_P_X2 || BackupAR_PY2 != Vars.Aimbot.AR_P_Y2
			|| BackupWeaponType != WeaponType
			|| BackupAimType != Vars.Aimbot.AimMode)
		{
			BackupAR_PX = Vars.Aimbot.AR_P_X1;
			BackupAR_PY = Vars.Aimbot.AR_P_Y1;

			BackupAR_PX2 = Vars.Aimbot.AR_P_X2;
			BackupAR_PY2 = Vars.Aimbot.AR_P_Y2;

			BackupWeaponType = WeaponType;
			BackupAimType = Vars.Aimbot.AimMode;
			setInitPID = false;
		}
		if (Vars.Aimbot.Controller_Current == 0)
		{
			if (Vars.Aimbot.BProStatus != 1)
				GetKmbox().GetCOMPortByDescription("USB", KmboxName);
			if (!FirstInitKmbox)
			{
				FirstInitKmbox = true;
				int loopCount = 0;
				for (auto iter : KmboxName)
				{
					auto NumBaud = atoi(Vars.Aimbot.txt_BPPort);
					if (GetKmbox().open(iter.second, NumBaud))
					{
						Vars.Aimbot.BProStatus = 1;
						Vars.Aimbot.AimBotMethod = 1;
						CurrentKmboxItem = loopCount;
						break;
					}
					loopCount++;
				}
			}
		}
		else if (Vars.Aimbot.Controller_Current == 1)
		{
			if (Vars.Aimbot.BNetStatus != 1 && !Utils::IsSafeReadPtr(Vars.Aimbot.txt_IP, 1) && !Utils::IsSafeReadPtr(Vars.Aimbot.txt_Port, 1) && !Utils::IsSafeReadPtr(Vars.Aimbot.txt_UUID, 1)
				&& *(int*)Vars.Aimbot.txt_IP != 0 && *(int*)Vars.Aimbot.txt_Port && *(int*)Vars.Aimbot.txt_UUID)
			{
				if (!FirstInitKmbox)
				{
					BNetThread = CreateThread(NULL, NULL, ThreadConnectBoxNet, NULL, NULL, NULL);
					FirstInitKmbox = true;
					ConnectedBNetStartTime = GetTickCount64();
				}
				else if (GetTickCount64() - ConnectedBNetStartTime >= 5000 && BNetThread)
				{
					TerminateThread(BNetThread, -1);
					BNetThread = NULL;
					ConnectedBNetStartTime = 0;
				}
			}
			else
			{
				FirstInitKmbox = true;
			}
		}
		else if (Vars.Aimbot.Controller_Current == 2)
		{
			if (Vars.Aimbot.LurkerStatus != 1)
				GetKmbox().GetCOMPortByDescription("USB", KmboxName);
			if (!FirstInitKmbox)
			{
				FirstInitKmbox = true;
				int loopCount = 0;
				for (auto iter : KmboxName)
				{
					auto NumBaud = atoi(Vars.Aimbot.txt_LKPort);
					if (GetKmbox().open(iter.second, NumBaud))
					{
						Vars.Aimbot.LurkerStatus = 1;
						Vars.Aimbot.AimBotMethod = 3;
						CurrentKmboxItem = loopCount;
						break;
					}
					loopCount++;
				}
			}
		}
		else if (Vars.Aimbot.Controller_Current == 3)
		{
			if (Vars.Aimbot.JSMHStatus != 1)
				GetKmbox().GetCOMPortByDescription("USB", KmboxName);
			if (!FirstInitKmbox)
			{
				FirstInitKmbox = true;
				int loopCount = 0;
				for (auto iter : KmboxName)
				{
					auto NumBaud = atoi(Vars.Aimbot.txt_JSPort);
					if (GetKmbox().open(iter.second, NumBaud))
					{
						Vars.Aimbot.JSMHStatus = 1;
						Vars.Aimbot.AimBotMethod = 4;
						CurrentKmboxItem = loopCount;
						break;
					}
					loopCount++;
				}
			}
		}

		//if (Vars.Misc.WeaponSkin && g_ServerResult.Version == 3)
		//{
		//	auto BackLv1FindResult = WeapSkinMap->find(hash_("Back_Lv1"));
		//	if (BackLv1FindResult != WeapSkinMap->end())
		//	{
		//		if (BackLv1FindResult->second.size() && !Utils::IsSafeReadPtr(BackLv1FindResult->second.data(), 1))
		//		{
		//			InitComboArray(BackLv1FindResult->second, BackLv1Array, Vars.Misc.BackLv1_Skin, &Vars.Misc.BackLv1_SetID);
		//			BackLv1Size = (int)BackLv1FindResult->second.size();
		//		}
		//	}
		//	else
		//		BackLv1Size = 0;
		//	auto BackLv2FindResult = WeapSkinMap->find(hash_("Back_Lv2"));
		//	if (BackLv2FindResult != WeapSkinMap->end())
		//	{
		//		if (BackLv2FindResult->second.size() && !Utils::IsSafeReadPtr(BackLv2FindResult->second.data(), 1))
		//		{
		//			InitComboArray(BackLv2FindResult->second, BackLv2Array, Vars.Misc.BackLv2_Skin, &Vars.Misc.BackLv2_SetID);
		//			BackLv2Size = (int)BackLv2FindResult->second.size();
		//		}
		//	}
		//	else
		//		BackLv2Size = 0;
		//	auto BackLv3FindResult = WeapSkinMap->find(hash_("Back_Lv3"));
		//	if (BackLv3FindResult != WeapSkinMap->end())
		//	{
		//		if (BackLv3FindResult->second.size() && !Utils::IsSafeReadPtr(BackLv3FindResult->second.data(), 1))
		//		{
		//			InitComboArray(BackLv3FindResult->second, BackLv3Array, Vars.Misc.BackLv3_Skin, &Vars.Misc.BackLv3_SetID);
		//			BackLv3Size = (int)BackLv3FindResult->second.size();
		//		}
		//	}
		//	else
		//		BackLv3Size = 0;
		//	auto ArmorLv1FindResult = WeapSkinMap->find(hash_("Armor_Lv1"));
		//	if (ArmorLv1FindResult != WeapSkinMap->end())
		//	{
		//		if (ArmorLv1FindResult->second.size() && !Utils::IsSafeReadPtr(ArmorLv1FindResult->second.data(), 1))
		//		{
		//			InitComboArray(ArmorLv1FindResult->second, ArmorLv1Array, Vars.Misc.ArmorLv1_Skin, &Vars.Misc.ArmorLv1_SetID);
		//			ArmorLv1Size = (int)ArmorLv1FindResult->second.size();
		//		}
		//	}
		//	else
		//		ArmorLv1Size = 0;
		//	auto ArmorLv2FindResult = WeapSkinMap->find(hash_("Armor_Lv2"));
		//	if (ArmorLv2FindResult != WeapSkinMap->end())
		//	{
		//		if (ArmorLv2FindResult->second.size() && !Utils::IsSafeReadPtr(ArmorLv2FindResult->second.data(), 1))
		//		{
		//			InitComboArray(ArmorLv2FindResult->second, ArmorLv2Array, Vars.Misc.ArmorLv2_Skin, &Vars.Misc.ArmorLv2_SetID);
		//			ArmorLv2Size = (int)ArmorLv2FindResult->second.size();
		//		}
		//	}
		//	else
		//		ArmorLv2Size = 0;
		//	auto ArmorLv3FindResult = WeapSkinMap->find(hash_("Armor_Lv3"));
		//	if (ArmorLv3FindResult != WeapSkinMap->end())
		//	{
		//		if (ArmorLv3FindResult->second.size() && !Utils::IsSafeReadPtr(ArmorLv3FindResult->second.data(), 1))
		//		{
		//			InitComboArray(ArmorLv3FindResult->second, ArmorLv3Array, Vars.Misc.ArmorLv3_Skin, &Vars.Misc.ArmorLv3_SetID);
		//			ArmorLv3Size = (int)ArmorLv3FindResult->second.size();
		//		}
		//	}
		//	else
		//		ArmorLv3Size = 0;
		//	auto HelmetLv1FindResult = WeapSkinMap->find(hash_("Helmet_Lv1"));
		//	if (HelmetLv1FindResult != WeapSkinMap->end())
		//	{
		//		if (HelmetLv1FindResult->second.size() && !Utils::IsSafeReadPtr(HelmetLv1FindResult->second.data(), 1))
		//		{
		//			InitComboArray(HelmetLv1FindResult->second, HelmetLv1Array, Vars.Misc.HelmetLv1_Skin, &Vars.Misc.HelmetLv1_SetID);
		//			HelmetLv1Size = (int)HelmetLv1FindResult->second.size();
		//		}
		//	}
		//	else
		//		HelmetLv1Size = 0;
		//	auto HelmetLv2FindResult = WeapSkinMap->find(hash_("Helmet_Lv2"));
		//	if (HelmetLv2FindResult != WeapSkinMap->end())
		//	{
		//		if (HelmetLv2FindResult->second.size() && !Utils::IsSafeReadPtr(HelmetLv2FindResult->second.data(), 1))
		//		{
		//			InitComboArray(HelmetLv2FindResult->second, HelmetLv2Array, Vars.Misc.HelmetLv2_Skin, &Vars.Misc.HelmetLv2_SetID);
		//			HelmetLv2Size = (int)HelmetLv2FindResult->second.size();
		//		}
		//	}
		//	else
		//		HelmetLv2Size = 0;
		//	auto HelmetLv3FindResult = WeapSkinMap->find(hash_("Helmet_Lv3"));
		//	if (HelmetLv3FindResult != WeapSkinMap->end())
		//	{
		//		if (HelmetLv3FindResult->second.size() && !Utils::IsSafeReadPtr(HelmetLv3FindResult->second.data(), 1))
		//		{
		//			InitComboArray(HelmetLv3FindResult->second, HelmetLv3Array, Vars.Misc.HelmetLv3_Skin, &Vars.Misc.HelmetLv3_SetID);
		//			HelmetLv3Size = (int)HelmetLv3FindResult->second.size();
		//		}
		//	}
		//	else
		//		HelmetLv3Size = 0;
		//}
		//else
		//{
		//	HelmetLv1Size = 0;
		//	HelmetLv2Size = 0;
		//	HelmetLv3Size = 0;
		//	ArmorLv1Size = 0;
		//	ArmorLv2Size = 0;
		//	ArmorLv3Size = 0;
		//	BackLv1Size = 0;
		//	BackLv2Size = 0;
		//	BackLv3Size = 0;
		//	Vars.Misc.HelmetLv1_SetID = 0;
		//	Vars.Misc.HelmetLv2_SetID = 0;
		//	Vars.Misc.HelmetLv3_SetID = 0;
		//	Vars.Misc.ArmorLv1_SetID = 0;
		//	Vars.Misc.ArmorLv2_SetID = 0;
		//	Vars.Misc.ArmorLv3_SetID = 0;
		//	Vars.Misc.BackLv1_SetID = 0;
		//	Vars.Misc.BackLv2_SetID = 0;
		//	Vars.Misc.BackLv3_SetID = 0;
		//}

		//if (!AutoConnectRadar)
		//{
		//	if (Vars.Misc.ShareRadar && !Utils::IsSafeReadPtr(Vars.Misc.txt_RadarIP, 1) && *(int*)Vars.Misc.txt_RadarIP != 0)
		//	{
		//		if (ConnectState == 0)
		//		{
		//			ConnectedThread = CreateThread(NULL, NULL, ThreadTryConnect, Vars.Misc.txt_RadarIP, NULL, NULL);
		//			if (ConnectedThread != NULL)
		//			{
		//				IsTryConnected = true;
		//				ConnectState = 1;
		//				ConnectedStartTime = GetTickCount64();
		//			}

		//		}
		//	}
		//	AutoConnectRadar = true;
		//}


		if (IsTryConnected && GetTickCount64() - ConnectedStartTime >= 5000)
		{
			TerminateThread(ConnectedThread, -1);
			ConnectedThread = NULL;
			IsTryConnected = false;
			ConnectState = 0;
			ConnectedStartTime = 0;
		}

		if (!Vars.Menu.Opened)
			return;
		ChineseMenu();
	}

	void Menu::ProcessInputMessage()
	{
		ImGuiStyle* Style = &ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();
		static bool keydown[256];
		POINT MousePoint;
		GetCursorPos(&MousePoint);
		io.MousePos = ImVec2(MousePoint.x, MousePoint.y);
		if (Vars.Misc.BattlefieldUnlock && (Vars.Menu.Opened || Vars.Misc.BattlefieldInfo))
		{
			POINT GameCursorPos = SDK::sdk::GetCursorPosition();
			if (Vars.Menu.Opened || Vars.Misc.BattlefieldInfo)
				Drawing::DrawCircleFilled(ImVec2((float)GameCursorPos.x, (float)GameCursorPos.y), 4.f, IM_COL32(255, 255, 255, 255)); // 在屏幕上绘制一个红色的圆形虚拟鼠标
			if (GetDMA().IsKeyDown(VK_LBUTTON) || GetAsyncKeyState(VK_LBUTTON))
			{
				if (Vars.Menu.Opened)
				{
					if ((float)GameCursorPos.x >= MenuPos.x && (float)GameCursorPos.y >= MenuPos.y && (float)GameCursorPos.x <= MenuPos.x + 772.f && (float)GameCursorPos.y <= MenuPos.y + 500.f)
					{
						SetCursorPos((int)GameCursorPos.x, (int)GameCursorPos.y);
					}
				}
				if (Vars.Misc.BattlefieldInfo)
				{
					if ((float)GameCursorPos.x >= BMenuPos.x && (float)GameCursorPos.y >= BMenuPos.y && (float)GameCursorPos.x <= BMenuPos.x + BMenuSize.x && (float)GameCursorPos.y <= BMenuPos.y + BMenuSize.y)
					{
						SetCursorPos((int)GameCursorPos.x, (int)GameCursorPos.y);
					}
				}
				io.MouseDown[0] = true;
			}
			else
				io.MouseDown[0] = false;

			for (int i = 0; i < 256; i++)
			{
				if (GetDMA().IsKeyDown(i) || GetAsyncKeyState(i) != 0)
				{
					if (!keydown[i] && i >= 65 && i <= 90)
					{
						if (GetKeyState(VK_CAPITAL) & 0x0001)
							io.AddInputCharacter(char(i));
						else
							io.AddInputCharacter(char(i + 32));

						keydown[i] = true;
					}
					else {
						if (!keydown[i] && i >= 48 && i <= 57)
						{
							keydown[i] = true;
							io.AddInputCharacter(char(i));
						}
						else {
							io.KeysDown[i] = true;
						}
					}
				}
				else {
					keydown[i] = false;
					io.KeysDown[i] = false;
				}
			}
		}

	}

	void Menu::ChineseMenu()
	{
		static bool bIsTransparent = false;
		ImGuiStyle& style = ImGui::GetStyle();
		auto BackupRound = style.ChildRounding;
		style.ChildRounding = 5.f;
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
		ImGui::SetNextWindowSize(ImVec2(772, 500));
		ImGui::SetNextWindowBgAlpha(1.f);
		ImGui::Begin(("##neo private internal"), &Vars.Menu.Opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			ImVec2 NextPos = ImGui::GetNextWindowPos();
			MenuPos = NextPos;
			if (Vars.Misc.DisplayMode == 0)
			{
				if (MousePos.x >= NextPos.x && MousePos.y >= NextPos.y && MousePos.x <= NextPos.x + 772.f && MousePos.y <= NextPos.y + 700.f)
				{
					if (!bIsTransparent)
					{
						OverlayEngine::UnTransparent();
						bIsTransparent = true;
					}
				}
				else
				{
					if (bIsTransparent)
					{
						OverlayEngine::Transparent();
						bIsTransparent = false;
					}
				}
			}
			else
			{
				OverlayEngine::UnTransparent();
				bIsTransparent = true;
			}
			//tm time2;
			//localtime_s(&time2, &g_expireDate);
			//char timeStr2[20];
			//strftime(timeStr2, sizeof(timeStr2), ("%Y/%m/%d %H:%M:%S"), &time2);

			ImGui::PushFont(OverlayEngine::Font18px);
			//ImGui::GetOverlayDrawList()->AddText(NextPos + ImVec2(100, 10), ImGui::FloatArrayGetColorU32(ImVec4(0.86f, 1.f, 0.79f, 1.f)), Utils::FormatString("DMA-PaoD %s", timeStr2).c_str());
			ImGui::PopFont();

			imgui_rainbow_rect(NextPos.x - 4, NextPos.y + 1, 8, 559, 0.00004f, RainbowColor);
			ImGui::PushFont(OverlayEngine::Font14px);
			ImGui::SameLine(3, 0, 3);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.43f, 0.43f, 0.43f, 1.f));
			ImGui::BeginChild(("##tabs"), ImVec2(90, 494), false, ImGuiWindowFlags_NoScrollWithMouse);
			{
				ImGui::SameLine(20, 0, 12);
				ImGui::ImageButton(Vars.Menu.tab == 0 ? VisualActive : VisualNormal, ImVec2(32, 41));
				if (ImGui::IsItemClicked())
					Vars.Menu.tab = 0;
				ImGui::SameLine(20, 0, 55);
				ImGui::ImageButton(Vars.Menu.tab == 1 ? AimbotActive : AimbotNormal, ImVec2(32, 41));
				if (ImGui::IsItemClicked())
					Vars.Menu.tab = 1;
				ImGui::SameLine(20, 0, 55);
				ImGui::ImageButton(Vars.Menu.tab == 2 ? MiscActive : MiscNormal, ImVec2(32, 41));
				if (ImGui::IsItemClicked())
					Vars.Menu.tab = 2;
				ImGui::SameLine(20, 0, 55);
				ImGui::ImageButton(Vars.Menu.tab == 3 ? WeaponActive : WeaponNormal, ImVec2(32, 41));
				if (ImGui::IsItemClicked())
					Vars.Menu.tab = 3;
				ImGui::SameLine(20, 0, 55);
				ImGui::ImageButton(Vars.Menu.tab == 4 ? BoxActive1 : BoxNormal1, ImVec2(32, 41));
				if (ImGui::IsItemClicked())
					Vars.Menu.tab = 4;
				ImGui::SameLine(20, 0, 55);
				ImGui::ImageButton(Vars.Menu.tab == 5 ? BoxActive2 : BoxNormal2, ImVec2(32, 41));
				if (ImGui::IsItemClicked())
					Vars.Menu.tab = 5;
				ImGui::SameLine(18, 0, 55);
				ImGui::ImageButton(Vars.Menu.config == 0 ? Project1Active : Project1Normal, ImVec2(30, 29));
				if (ImGui::IsItemClicked())
				{
					Config->Save(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
					Vars.Menu.config = 0;
					Config->Load(hudPath1);
					Vars.Menu.config = 0;
				}
				ImGui::SameLine(18, 0, 40);
				ImGui::ImageButton(Vars.Menu.config == 1 ? Project2Active : Project2Normal, ImVec2(30, 29));
				if (ImGui::IsItemClicked())
				{
					Config->Save(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
					Vars.Menu.config = 1;
					Config->Load(hudPath2);
					Vars.Menu.config = 1;
				}
				ImGui::SameLine(16, 0, 50);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.f));
				ImGui::Text(("Language"));
				ImGui::SameLine(0, 0, 20);
				ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.42352f, 0.42352f, 0.42352f, 1.f));
				ImGui::Separator();
				ImGui::PopStyleColor(2);
				ImGui::SameLine(6, 0, 10);
				ImGui::ImageButton(Vars.Menu.language == 0 ? ChineseActive : ChineseNormal, ImVec2(20, 20));
				if (ImGui::IsItemClicked())
					Vars.Menu.language = 0;
				ImGui::SameLine(50, 0, 0);
				ImGui::ImageButton(Vars.Menu.language == 1 ? EnglishActive : EnglishNormal, ImVec2(20, 20));
				if (ImGui::IsItemClicked())
					Vars.Menu.language = 1;

			}ImGui::EndChild();
			ImGui::PopStyleColor();
			ImGui::PopFont();
			switch (Vars.Menu.tab)
			{
			case 0:
				RenderEsp();
				break;
			case 1:
				RenderAimbot();
				break;
			case 2:
				RenderMisc();
				break;
			case 3:
				RenderWeapon();
				break;
			case 4:
				RenderEquipment();
				break;
			case 5:
				RenderEquipment2();
				break;
			default:
				break;
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
		style.ChildRounding = BackupRound;
	}

	void Menu::add_hotkey(const char* text, int* option_toggle, char* NameText, bool* IsFind)
	{
		ImGui::Text(text);
		SetTooltip(LOCALIZE_Menu(u8"Hotkeys can now be set repeatedly. If you want to disable the hotkey, set it to ESC, which is the one in the upper left corner of the keyboard.", u8"热键现在可以重复设置,如果想使热键无效则设置成ESC也就在键盘左上角那个"));
		ImGui::SameLine(134, 0, -2);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.f, 1.0f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.32f, 0.32f, 0.32f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.f, 0.f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4039f, 0.5725f, 0.4039f, 1.0f));
		ImGui::Button(NameText, ImVec2(72, 20));
		SetTooltip(LOCALIZE_Menu(u8"Hotkeys can now be set repeatedly. If you want to disable the hotkey, set it to ESC, which is the one in the upper left corner of the keyboard.", u8"热键现在可以重复设置,如果想使热键无效则设置成ESC也就在键盘左上角那个"));
		ImGui::PopStyleColor(4);
		//if (strlen(NameText) <= 2)
		{
			int TempKeyValue = 0;
			if (*option_toggle > 10)
				strcpy(NameText, AimHotKey[*option_toggle]);
			else
			{
				switch (*option_toggle)
				{
				case 1:
					TempKeyValue = 0;
					break;
				case 2:
					TempKeyValue = 1;
					break;
				case 4:
					TempKeyValue = 2;
					break;
				case 5:
					TempKeyValue = 3;
					break;
				case 6:
					TempKeyValue = 4;
					break;
				default:
					TempKeyValue = *option_toggle = 2;
					break;
				}
				strcpy(NameText, AimHotKey[TempKeyValue]);
			}
		}
		//if (strlen(NameText) <= 2)
		{
			int TempKeyValue = 0;
			if (*option_toggle > 10)
				strcpy(NameText, AimHotKey[*option_toggle]);
			else
			{
				switch (*option_toggle)
				{
				case 1:
					TempKeyValue = 0;
					break;
				case 2:
					TempKeyValue = 1;
					break;
				case 4:
					TempKeyValue = 2;
					break;
				case 5:
					TempKeyValue = 3;
					break;
				case 6:
					TempKeyValue = 4;
					break;
				default:
					TempKeyValue = *option_toggle = 2;
					break;
				}
				strcpy(NameText, AimHotKey[TempKeyValue]);
			}
		}
		if (ImGui::IsItemClicked() || *IsFind)
		{
			bool firstClick = false;
			if (!*IsFind)
			{
				*IsFind = true;
				firstClick = true;
			}
			ImGuiIO& io = ImGui::GetIO();
			strcpy(NameText, (u8"..."));
			for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
			{
				if (ImGui::IsMouseClicked(i))
				{
					if (firstClick)
					{
						firstClick = false;
					}
					else
					{
						int Value = 0;
						switch (i)
						{
						case 0:
							Value = 1;
							break;
						case 1:
							Value = 2;
							break;
						case 2:
							Value = 4;
							break;
						case 3:
							Value = 5;
							break;
						case 4:
							Value = 6;
							break;
						default:
							Value = i + 1;
							break;
						}
						//bool skip = false;
						//for (int j = 0; j < SaveHotKey.size(); j++)
						//{
						//	if (SaveHotKey[j] == Value && Value != *option_toggle)
						//	{
						//		skip = true;
						//		break;
						//	}
						//}
						//if (skip)
						//	break;
						*option_toggle = Value;
						strcpy(NameText, AimHotKey[i]);
						*IsFind = false;
						break;
					}
				}

			}
			for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
			{
				if (ImGui::IsKeyReleased(i))
				{
					//bool skip = false;
					//for (int j = 0; j < SaveHotKey.size(); j++)
					//{
					//	if (SaveHotKey[j] == i && i != *option_toggle)
					//	{
					//		skip = true;
					//		break;
					//	}
					//}
					//if (skip || i == 1)
					//	break;
					*option_toggle = i;
					strcpy(NameText, AimHotKey[i]);
					*IsFind = false;
				}
			}
		}
		ImGui::NewLine();
	}

	void Menu::RenderEquipment()
	{
		ImGui::SameLine(290, 0, 0);
		RenderTabName(LOCALIZE_Menu(u8"Equipment", u8"装备"));
		ImGui::SameLine(94, 0, 36);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16470f, 0.17647f, 0.20784f, 1.f));
		ImGui::BeginChild(("##tabs1"), ImVec2(163, 220), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Armor Lv2", u8"装备 等级II"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##ArmorLv2"), Vars.espItem.ArmorLv2Color, 0);
			ImGui::SameLine(-2, 0, 20);
			ImGui::Checkbox1(LOCALIZE_Menu("Helmet(Lv.2)", u8"头盔(等级2)"), &Vars.espItem.HelmetLv2, Vars.espItem.ArmorLv2Color);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Police Vest(Lv.2)", u8"警用背心(Lv.2)"), &Vars.espItem.VestLv2, Vars.espItem.ArmorLv2Color);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Backpack(Lv.2)", u8"背包(Lv.2)"), &Vars.espItem.BackpackLv2, Vars.espItem.ArmorLv2Color);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Interference Backpack", u8"电磁干扰背包(Lv.2)"), &Vars.espItem.InterferenceBackpack, Vars.espItem.ArmorLv2Color);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Emergency Pickup", u8"紧急接驳"), &Vars.espItem.EmergencyPickup, Vars.espItem.ArmorLv2Color);
			ImGui::SameLine(10, 0, 20);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Armor Lv3", u8"装备 等级III"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##ArmorLv3"), Vars.espItem.ArmorLv3Color, 0);
			ImGui::SameLine(-2, 0, 24);
			ImGui::Checkbox1(LOCALIZE_Menu("Helmet(Lv.3)", u8"头盔(等级3)"), &Vars.espItem.HelmetLv3, Vars.espItem.ArmorLv3Color);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Military Vest(Lv.3)", u8"军用背心(Lv.3)"), &Vars.espItem.VestLv3, Vars.espItem.ArmorLv3Color);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Backpack(Lv.3)", u8"背包(Lv.3)"), &Vars.espItem.BackpackLv3, Vars.espItem.ArmorLv3Color);
			ImGui::SameLine(-2, 0, 18);
		}ImGui::EndChild();
		ImGui::SameLine(263, 0, 0);
		ImGui::BeginChild(("##tabs2"), ImVec2(312, 160), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Sight", u8"瞄准镜"));
			ImGui::PopFont();
			ImGui::SameLine(224, 0, 0);
			ImGui::ColorEdit3((u8"Color##Sight"), Vars.espItem.SightColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"Canted Sight", u8"側瞄具"), &Vars.espItem.CantedSight, Vars.espItem.SightColor);
			ImGui::SameLine(164, 0, 0);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"6x Scope", u8"6倍瞄准镜"), &Vars.espItem.Scope6x, Vars.espItem.SightColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"Red Dot Sight", u8"红点瞄具"), &Vars.espItem.RedDotSight, Vars.espItem.SightColor);
			ImGui::SameLine(164, 0, 0);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"8x Scope", u8"8倍瞄准镜"), &Vars.espItem.Scope8x, Vars.espItem.SightColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"Holographic Sight", u8"全像瞄准镜"), &Vars.espItem.HolographicSight, Vars.espItem.SightColor);
			ImGui::SameLine(164, 0, 0);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"15x Scope", u8"15倍瞄准镜"), &Vars.espItem.Scope15x, Vars.espItem.SightColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"2x Scope", u8"2倍瞄准镜"), &Vars.espItem.Scope2x, Vars.espItem.SightColor);
			ImGui::SameLine(164, 0, 0);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"Thermal 4x Scope", u8"热感应4倍瞄准镜"), &Vars.espItem.Thermal4xScope, Vars.espItem.SightColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"3x Scope", u8"3倍瞄准镜"), &Vars.espItem.Scope3x, Vars.espItem.SightColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"4x Scope", u8"4倍瞄准镜"), &Vars.espItem.Scope4x, Vars.espItem.SightColor);

		}ImGui::EndChild();

		ImGui::SameLine(581, 0, 0);
		ImGui::BeginChild(("##tabs3"), ImVec2(183, 160), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Magazine", u8"弹匣"));
			ImGui::PopFont();
			ImGui::SameLine(94, 0, 0);
			ImGui::ColorEdit3(("Color##Magazine"), Vars.espItem.MagazineColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu(u8"Ext.QuickDraw Mag(SMG)", u8"加长快抽弹匣(冲锋枪)"), &Vars.espItem.ExtQuickDrawMagSMG, Vars.espItem.MagazineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Ext.QuickDraw Mag(AR,DMR)", u8"加长快抽弹匣(突及步枪,精准射手步枪)"), &Vars.espItem.ExtQuickDrawMagARDMR, Vars.espItem.MagazineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Ext.QuickDraw Mag(DMR,SR)", u8"加长快抽弹匣(精准射手步枪,狙击枪) "), &Vars.espItem.ExtQuickDrawMagDMRSR, Vars.espItem.MagazineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Extended Mag(SMG)", u8"加长弹匣(冲锋枪)"), &Vars.espItem.ExtendedMagSMG, Vars.espItem.MagazineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Extended Mag(AR,DMR)", u8"加长弹匣(突击步枪,精准射手步枪)"), &Vars.espItem.ExtendedMagARDMR, Vars.espItem.MagazineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Extended Mag(DMR,SR)", u8"加长弹匣(精准射手步枪,狙击枪)"), &Vars.espItem.ExtendedMagDMRSR, Vars.espItem.MagazineColor);

		}ImGui::EndChild();
		ImGui::SameLine(94, 0, 226);
		ImGui::BeginChild(("##tabs4"), ImVec2(163, 226), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Ammo", u8"彈藥"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##Ammo"), Vars.espItem.AmmoColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu(".45 ACP", u8".45 ACP"), &Vars.espItem.ACP45, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("5.56mm", u8"5.56毫米子彈"), &Vars.espItem._556mm, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("7.62mm", u8"7.62毫米子彈"), &Vars.espItem._762mm, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("5.7mm", u8"5.7毫米子彈"), &Vars.espItem._57mm, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("9mm", u8"9毫米子彈"), &Vars.espItem._9mm, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("40mm Smoke Grenade", u8"40mm 烟雾弹"), &Vars.espItem._40mm, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("60mm Mortar Shell", u8"60mm 迫击炮弹"), &Vars.espItem._60mm, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("12 Gauge", u8"12 铅径"), &Vars.espItem._12Gauge, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("12 Gauge Slug", u8"12 铅径弹头"), &Vars.espItem._12GaugeSlug, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu(".300 Magnum", u8".300 麦格农"), &Vars.espItem._300Magnum, Vars.espItem.AmmoColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("FlareAmmo", u8"信号弹"), &Vars.espItem.FlareAmmo, Vars.espItem.AmmoColor);
		}ImGui::EndChild();
		ImGui::SameLine(263, 0, -60);
		ImGui::BeginChild(("##tabs5"), ImVec2(312, 180), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Muzzle", u8"枪口"));
			ImGui::PopFont();
			ImGui::SameLine(224, 0, 0);
			ImGui::ColorEdit3(("Color##Muzzle"), Vars.espItem.MuzzleColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Chock(SG)", u8"缩口(霰弹枪)"), &Vars.espItem.ChockSG, Vars.espItem.MuzzleColor);
			ImGui::SameLine(164, 0, 0);
			ImGui::Checkbox1(LOCALIZE_Menu("Suppressor(SMG)", u8"消音器(冲锋枪)"), &Vars.espItem.SuppressorSMG, Vars.espItem.MuzzleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Duckbill(SG)", u8"鸭嘴缩口(霰弹枪)"), &Vars.espItem.DuckbillSG, Vars.espItem.MuzzleColor);
			ImGui::SameLine(164, 0, 0);
			ImGui::Checkbox1(LOCALIZE_Menu("Suppressor(AR,DMR)", u8"消音器(突击步枪)"), &Vars.espItem.SuppressorARDMR, Vars.espItem.MuzzleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Flash Hider(SMG)", u8"消焰器(冲锋枪)"), &Vars.espItem.FlashHiderSMG, Vars.espItem.MuzzleColor);
			ImGui::SameLine(164, 0, 0);
			ImGui::Checkbox1(LOCALIZE_Menu("Suppressor(DMR,SR)", u8"消音器(狙击枪)"), &Vars.espItem.SuppressorDMRSR, Vars.espItem.MuzzleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Flash Hider(AR,DMR)", u8"消焰器(突击步枪)"), &Vars.espItem.FlashHiderARDMR, Vars.espItem.MuzzleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Flash Hider(DMR,SR)", u8"消焰器(狙击枪)"), &Vars.espItem.FlashHiderDMRSR, Vars.espItem.MuzzleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Compensator(SMG)", u8"补偿器(冲锋枪)"), &Vars.espItem.CompensatorSMG, Vars.espItem.MuzzleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Compensator(AR,DMR)", u8"补偿器(突击步枪)"), &Vars.espItem.CompensatorARDMR, Vars.espItem.MuzzleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Compensator(DMR,SR)", u8"补偿器(狙击枪)"), &Vars.espItem.CompensatorDMRSR, Vars.espItem.MuzzleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Muzzle Brake", u8"枪口制退器"), &Vars.espItem.MuzzleBrake, Vars.espItem.MuzzleColor);
		}ImGui::EndChild();
		ImGui::SameLine(412, 0, 0);

		ImGui::SameLine(581, 0, 0);
		ImGui::BeginChild(("##tabs6"), ImVec2(183, 140), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Grip", u8"握把"));
			ImGui::PopFont();
			ImGui::SameLine(94, 0, 0);
			ImGui::ColorEdit3(("Color##Grip"), Vars.espItem.GripColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Lightweight Grip", u8"轻型握把"), &Vars.espItem.LightweightGrip, Vars.espItem.GripColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Thumbgrip", u8"拇指握把"), &Vars.espItem.Thumbgrip, Vars.espItem.GripColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Vertical Foregrip", u8"垂直握把"), &Vars.espItem.VerticalForegrip, Vars.espItem.GripColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Angled Foregrip", u8"斜角前握把"), &Vars.espItem.AngledForegrip, Vars.espItem.GripColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Halfgrip", u8"半截式握把"), &Vars.espItem.Halfgrip, Vars.espItem.GripColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Laser Sight", u8"镭射瞄准具"), &Vars.espItem.LaserSight, Vars.espItem.GripColor);

		}ImGui::EndChild();
		ImGui::SameLine(263, 0, 186);
		ImGui::BeginChild(("##tabs7"), ImVec2(312, 100), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Buttstock", u8"枪托"));
			ImGui::PopFont();
			ImGui::SameLine(224, 0, 0);
			ImGui::ColorEdit3(("Color##Buttstock"), Vars.espItem.ButtstockColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Cheek Pad(DMR,SR)", u8"脸颊垫"), &Vars.espItem.CheekPad, Vars.espItem.ButtstockColor);
			ImGui::SameLine(164, 0, 0);
			ImGui::Checkbox1(LOCALIZE_Menu("Tactical Stock(SMG,AR)", u8"战术枪托"), &Vars.espItem.TacticalStock, Vars.espItem.ButtstockColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Folding Stock", u8"折叠式枪托"), &Vars.espItem.FoldingStock, Vars.espItem.ButtstockColor);
			ImGui::SameLine(164, 0, 0);
			ImGui::Checkbox1(LOCALIZE_Menu("Bullet Loops(SG,SR)", u8"子弹袋"), &Vars.espItem.BulletLoops, Vars.espItem.ButtstockColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Heave Stock(SMG,AR)", u8"重型枪托"), &Vars.espItem.HeaveStock, Vars.espItem.ButtstockColor);

		}ImGui::EndChild();
		ImGui::SameLine(581, 0, -40);
		ImGui::BeginChild(("##tabs8"), ImVec2(183, 140), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Medicine", u8"药品"));
			ImGui::PopFont();
			ImGui::SameLine(224, 0, 0);
			ImGui::ColorEdit3(("Color##Medicine"), Vars.espItem.MedicineColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Med Kit", u8"医疗包"), &Vars.espItem.MedKit, Vars.espItem.MedicineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("First Aid Kit", u8"急救包"), &Vars.espItem.FirstAidKit, Vars.espItem.MedicineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Bandage", u8"绷带"), &Vars.espItem.Bandage, Vars.espItem.MedicineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("PainKiller", u8"止痛剂"), &Vars.espItem.PainKiller, Vars.espItem.MedicineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Energy Drink", u8"能量饮料"), &Vars.espItem.EnergyDrink, Vars.espItem.MedicineColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Adrenaline Syringe", u8"肾上腺素注射器"), &Vars.espItem.AdrenalineSyringe, Vars.espItem.MedicineColor);
		}ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void Menu::RenderEquipment2()
	{
		ImGui::SameLine(290, 0, 0);
		RenderTabName(LOCALIZE_Menu(u8"Misc", u8"其他"));
		ImGui::SameLine(94, 0, 36);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16470f, 0.17647f, 0.20784f, 1.f));
		ImGui::BeginChild(("##tabs1"), ImVec2(163, 300), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Tactical", u8"战术装备"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##Tactical"), Vars.espItem.TacticalColor, 0);
			ImGui::SameLine(-2, 0, 20);
			ImGui::Checkbox1(LOCALIZE_Menu("TraumaBag", u8"紧急处理装备"), &Vars.espItem.yingjichulizhuangbei, Vars.espItem.TacticalColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Spotter Scope", u8"观测镜"), &Vars.espItem.guanchejing, Vars.espItem.TacticalColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("IntegratedRepair", u8"多合一维修组件"), &Vars.espItem.duoheyi, Vars.espItem.TacticalColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Drone", u8"无人机"), &Vars.espItem.wurenji, Vars.espItem.TacticalColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("TacPack", u8"战术背包"), &Vars.espItem.zhanshubeibao, Vars.espItem.TacticalColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("BulletproofShield", u8"防弹盾牌"), &Vars.espItem.BulletproofShield, Vars.espItem.TacticalColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("NeonCoin", u8"钱币"), &Vars.espItem.NeonCoin, Vars.espItem.TacticalColor);


			ImGui::SameLine(10, 0, 20);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Useable", u8"可使用的"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##Useable"), Vars.espItem.UseableColor, 0);
			ImGui::SameLine(-2, 0, 24);
			ImGui::Checkbox1(LOCALIZE_Menu("SelfRevive", u8"自救除颤器"), &Vars.espItem.SelfRevive, Vars.espItem.UseableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("CoverStruct", u8"紧急掩体信号弹"), &Vars.espItem.CoverStructDropHandFlare, Vars.espItem.UseableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("BattleReadyKit", u8"战斗准备套件"), &Vars.espItem.BattleReadyKit, Vars.espItem.UseableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Chamber Key", u8"密室钥匙"), &Vars.espItem.ChamberKeys, Vars.espItem.UseableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Revival Transmitter", u8"复活发射器"), &Vars.espItem.Revival_Transmitter, Vars.espItem.UseableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Fantasy Brawl", u8"奇幻大乱斗"), &Vars.espItem.CraftMode, Vars.espItem.UseableColor);
			ImGui::SameLine(-2, 0, 18);
		}ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void Menu::RenderWeapon()
	{
		ImGui::SameLine(290, 0, 0);
		RenderTabName(LOCALIZE_Menu(u8"Weapon", u8"武器"));
		ImGui::SameLine(94, 0, 36);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16470f, 0.17647f, 0.20784f, 1.f));
		ImGui::BeginChild(("##tabs1"), ImVec2(163, 270), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"AR", u8"突击步枪"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##AR"), Vars.espItem.ARColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("AKM"), &Vars.espItem.AKM, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Groza"), &Vars.espItem.Groza, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("ACE32"), &Vars.espItem.ACE32, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Beryl M762"), &Vars.espItem.M762, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Mk47 Mutant"), &Vars.espItem.MK47, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("K2"), &Vars.espItem.K2, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("QBZ"), &Vars.espItem.QBZ, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("AUG"), &Vars.espItem.AUG, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("G36C"), &Vars.espItem.G36C, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("M416"), &Vars.espItem.M416, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("M16A4"), &Vars.espItem.M16A4, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("SCAR-L"), &Vars.espItem.SCARL, Vars.espItem.ARColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("FAMAS"), &Vars.espItem.FAMAS, Vars.espItem.ARColor);
		}ImGui::EndChild();
		ImGui::SameLine(263, 0, 0);
		ImGui::BeginChild(("##tabs2"), ImVec2(163, 160), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"DMR", u8"射手步槍"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##DMR"), Vars.espItem.DMRColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("VSS"), &Vars.espItem.VSS, Vars.espItem.DMRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("QBU"), &Vars.espItem.QBU, Vars.espItem.DMRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("SLR"), &Vars.espItem.SLR, Vars.espItem.DMRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("SKS"), &Vars.espItem.SKS, Vars.espItem.DMRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Mk12"), &Vars.espItem.Mk12, Vars.espItem.DMRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Mk14"), &Vars.espItem.Mk14, Vars.espItem.DMRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Mini14"), &Vars.espItem.Mini14, Vars.espItem.DMRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Dragunov"), &Vars.espItem.Dragunov, Vars.espItem.DMRColor);
		}ImGui::EndChild();
		ImGui::SameLine(432, 0, 0);
		ImGui::BeginChild(("##tabs3"), ImVec2(163, 160), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"SR", u8"狙击枪"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##SR"), Vars.espItem.SRColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("M24"), &Vars.espItem.M24, Vars.espItem.SRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("AWM"), &Vars.espItem.AWM, Vars.espItem.SRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Win94"), &Vars.espItem.Win94, Vars.espItem.SRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Kar98k"), &Vars.espItem.Kar98k, Vars.espItem.SRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Lynx AMR"), &Vars.espItem.LynxAMR, Vars.espItem.SRColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Mosin Nagant"), &Vars.espItem.MosinNagant, Vars.espItem.SRColor);
		}ImGui::EndChild();
		ImGui::SameLine(601, 0, 0);
		ImGui::BeginChild(("##tabs4"), ImVec2(163, 160), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Throwable", u8"投擲物"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##Throwable"), Vars.espItem.ThrowableColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("C4", u8"C4"), &Vars.espItem.C4, Vars.espItem.ThrowableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Smoke Boom", u8"烟雾弹"), &Vars.espItem.SmokeBoom, Vars.espItem.ThrowableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Flash Bang", u8"闪光弹"), &Vars.espItem.FlashBang, Vars.espItem.ThrowableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Molotov", u8"燃烧弹"), &Vars.espItem.Molotov, Vars.espItem.ThrowableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Frag Grenade", u8"破片手榴彈"), &Vars.espItem.FragGrenade, Vars.espItem.ThrowableColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("BlueZone Grenade", u8"蓝圈手榴弹"), &Vars.espItem.BlueZoneGrenade, Vars.espItem.ThrowableColor);

		}ImGui::EndChild();
		ImGui::SameLine(94, 0, 276);
		ImGui::BeginChild(("##tabs5"), ImVec2(163, 176), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"ShotGun", u8"霰弹枪"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##ShotGun"), Vars.espItem.ShotGunColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("O12"), &Vars.espItem.O12, Vars.espItem.ShotGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("DBS"), &Vars.espItem.DBS, Vars.espItem.ShotGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("S12K"), &Vars.espItem.S12K, Vars.espItem.ShotGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("S686"), &Vars.espItem.S686, Vars.espItem.ShotGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("S1897"), &Vars.espItem.S1897, Vars.espItem.ShotGunColor);

		}ImGui::EndChild();
		ImGui::SameLine(263, 0, -110);
		ImGui::BeginChild(("##tabs6"), ImVec2(163, 180), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"SMG", u8"冲锋枪"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##SMG"), Vars.espItem.SMGColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("P90"), &Vars.espItem.P90, Vars.espItem.SMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("MP9"), &Vars.espItem.MP9, Vars.espItem.SMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("MP5K"), &Vars.espItem.MP5K, Vars.espItem.SMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Vector"), &Vars.espItem.Vector, Vars.espItem.SMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("UMP45"), &Vars.espItem.UMP45, Vars.espItem.SMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Micro UZI"), &Vars.espItem.MicroUZI, Vars.espItem.SMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("PP-19 Bizon", u8"野牛冲锋枪"), &Vars.espItem.PP19Bizon, Vars.espItem.SMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Tommy Gun", u8"汤姆逊冲锋枪"), &Vars.espItem.TommyGun, Vars.espItem.SMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("JS9", u8"JS9"), &Vars.espItem.JS9, Vars.espItem.SMGColor);
		}ImGui::EndChild();
		ImGui::SameLine(432, 0, 0);
		ImGui::BeginChild(("##tabs7"), ImVec2(163, 180), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"HandGun", u8"手枪"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##HandGun"), Vars.espItem.HandGunColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("R45"), &Vars.espItem.R45, Vars.espItem.HandGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("P92"), &Vars.espItem.P92, Vars.espItem.HandGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("P18C"), &Vars.espItem.P18C, Vars.espItem.HandGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("P1911"), &Vars.espItem.P1911, Vars.espItem.HandGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("R1895"), &Vars.espItem.R1895, Vars.espItem.HandGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Skoripion"), &Vars.espItem.Skoripion, Vars.espItem.HandGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Sawed-Off", u8"削短型霰弹枪"), &Vars.espItem.SawedOff, Vars.espItem.HandGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("Deagle"), &Vars.espItem.Deagle, Vars.espItem.HandGunColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("StunGun", u8"电击枪"), &Vars.espItem.StunGun, Vars.espItem.HandGunColor);
		}ImGui::EndChild();
		ImGui::SameLine(601, 0, 0);
		ImGui::BeginChild(("##tabs8"), ImVec2(163, 180), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Melle", u8"近战"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##Melle"), Vars.espItem.MelleColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Crowbar", u8"铁撬"), &Vars.espItem.Crowbar, Vars.espItem.MelleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Sickle", u8"镰刀"), &Vars.espItem.Sickle, Vars.espItem.MelleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Machete", u8"开山刀"), &Vars.espItem.Machete, Vars.espItem.MelleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Pan", u8"平底锅"), &Vars.espItem.Pan, Vars.espItem.MelleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Pick Axe", u8"镐"), &Vars.espItem.PickAex, Vars.espItem.MelleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Gas Can", u8"汽油桶"), &Vars.espItem.JerryCan, Vars.espItem.MelleColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Mountain Bike", u8"山地自行车"), &Vars.espItem.MountainBike, Vars.espItem.MelleColor);

		}ImGui::EndChild();
		ImGui::SameLine(263, 0, 186);
		ImGui::BeginChild(("##tabs9"), ImVec2(163, 100), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"LMG", u8"轻机枪"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##LMG"), Vars.espItem.LMGColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("MG3"), &Vars.espItem.MG3, Vars.espItem.LMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("M249"), &Vars.espItem.M249, Vars.espItem.LMGColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(("DP-28"), &Vars.espItem.DP28, Vars.espItem.LMGColor);

		}ImGui::EndChild();
		ImGui::SameLine(432, 0, 0);
		ImGui::BeginChild(("##tabs10"), ImVec2(163, 100), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"ETC", u8"额外的"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##ETC"), Vars.espItem.ETCColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Crossbow", u8"驽弓"), &Vars.espItem.Crossbow, Vars.espItem.ETCColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Flare Gun", u8"信号枪"), &Vars.espItem.FlareGun, Vars.espItem.ETCColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("M79", u8"M79烟雾弹枪"), &Vars.espItem.M79, Vars.espItem.ETCColor);

		}ImGui::EndChild();
		ImGui::SameLine(601, 0, 0);
		ImGui::BeginChild(("##tabs11"), ImVec2(163, 100), false);
		{
			ImGui::SameLine(10, 0, 8);
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::Text(LOCALIZE_Menu(u8"Transmitter", u8"发射器"));
			ImGui::PopFont();
			ImGui::SameLine(74, 0, 0);
			ImGui::ColorEdit3(("Color##Transmitter"), Vars.espItem.TransmitterColor, 0);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Mortar", u8"迫击炮"), &Vars.espItem.Mortar, Vars.espItem.TransmitterColor);
			ImGui::SameLine(-2, 0, 18);
			ImGui::Checkbox1(LOCALIZE_Menu("Panzerfaust", u8"铁拳发射器"), &Vars.espItem.Panzerfaust, Vars.espItem.TransmitterColor);

		}ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void Menu::RenderMisc()
	{
		ImGui::SameLine(290, 0, 0);
		RenderTabName(LOCALIZE_Menu(u8"Setup", u8"设置"));
		ImGui::SameLine(94, 0, 36);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16470f, 0.17647f, 0.20784f, 1.f));
		ImGui::BeginChild(("##tabs1"), ImVec2(220, 184), false);
		{
			ImGui::SameLine(8, 0, 8);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Projectile Warning", u8"手雷轨迹预警"), &Vars.Misc.Projectile);
			SetTooltip(LOCALIZE_Menu(u8"Projectile Warning", u8"启动手雷轨迹预警"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Stop Aiming While Reloading", u8"装弹时停止自瞄"), &Vars.Aimbot.CheckReloading);
			SetTooltip(LOCALIZE_Menu(u8"Stop Aiming While Reloading", u8"启动装弹时停止自瞄"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Battlefied Info", u8"战局信息"), &Vars.Misc.BattlefieldInfo);
			SetTooltip(LOCALIZE_Menu(u8"Battlefied Info", u8"启动战局信息"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Share Radar", u8"共享雷达"), &Vars.Misc.ShareRadar);
			SetTooltip(LOCALIZE_Menu(u8"Share Radar", u8"启动共享雷达"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Warning Direction", u8"预警方向"), &Vars.Misc.WarningExt);
			SetTooltip(LOCALIZE_Menu(u8"Warning Direction", u8"启动预警方向"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Grenade Predict", u8"手雷预判自瞄"), &Vars.Aimbot.GrenadePredict);
			SetTooltip(LOCALIZE_Menu(u8"Grenade Predict", u8"启动手雷预判自瞄"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Aiming While Hold Melee Weapon", u8"近战武器锁定"), &Vars.Aimbot.HandNotLock);
			SetTooltip(LOCALIZE_Menu(u8"Aiming While Hold Melee Weapon", u8"启动近战武器锁定"));
			if (g_ServerResult.Version >= 2)
			{
				ImGui::SameLine(8, 0, 20);
				ImGui::Checkbox2(g_ServerResult.Version >= 2 ? LOCALIZE_Menu(u8"Active Physical Model Rendering", u8"启动实体模型渲染") : LOCALIZE_Menu(u8"Active Physical Model Rendering(Unauthorized)", u8"启动实体模型渲染(未授权)"), &Vars.Menu.PhysxTrace);
				SetTooltip(LOCALIZE_Menu(u8"Enable solid model rendering to accurately determine cover", u8"启动实体模型渲染以进行精准判断掩体"));
			}



		}ImGui::EndChild();
		ImGui::SameLine(320, 0, 0);
		ImGui::BeginChild(("##tabs2"), ImVec2(220, 184), false);
		{
			ImGui::SameLine(8, 0, 8);
			static const char* multi_itemsen[8] = { "Grenade", "StickGrenade", "BluezoneGrenade", "DecoyGrenade", "SmokeBomb", "FlashBang","C4","Molotov" };
			static const char* multi_itemscn[8] = { u8"手榴弹", u8"粘性炸弹", u8"蓝区手榴弹", u8"诱饵手榴弹", u8"烟雾弹", u8"闪光弹",u8"C4炸弹",u8"燃烧瓶" };

			ImGui::MultiCombo(LOCALIZE_Menu("Display", u8"需要展示的"), (bool*)Vars.Misc.multiProject_num, LOCALIZE_Menu(multi_itemsen, multi_itemscn), 8);
			SetTooltip(LOCALIZE_Menu(u8"Select the type of projectiles you want to display", u8"选择需要展示的投掷物类型"));
			ImGui::SameLine(8, 0, 24);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Projectile Trajectory", u8"投掷物轨迹线"), &Vars.Misc.PorjectileLine);
			SetTooltip(LOCALIZE_Menu(u8"Projectile Trajectory", u8"开启投掷物轨迹线"));
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##ProjectileColor"), Vars.Misc.ProjectileColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Grenade Explosion Time", u8"手雷爆炸倒数计时"), &Vars.Misc.PorjectileTime);
			SetTooltip(LOCALIZE_Menu(u8"Grenade Explosion Time", u8"开启手雷爆炸倒数计时"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(Vars.Misc.ProjectileType ?
				LOCALIZE_Menu(u8"Display Mode(Texture)", u8"投掷物显示模式(图片)") : LOCALIZE_Menu(u8"Display Mode(Characters)", u8"投掷物显示模式(文字)"), &Vars.Misc.ProjectileType);
			SetTooltip(LOCALIZE_Menu(u8"Select the drawing style to display the projectile", u8"选择展示投掷物的绘制形式"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Damage Radius", u8"伤害范围"), &Vars.Misc.ProjectRadius);
			SetTooltip(LOCALIZE_Menu(u8"Enable projectile damage range drawing", u8"开启投掷物伤害范围绘制"));
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##ProjectRadiusColor"), Vars.Misc.ProjectRadiusColor, 0);
			ImGui::SameLine(14, 0, 20);
			ImGui::Text(LOCALIZE_Menu(u8"Damage Radius", u8"投掷伤害半径"));
			ImGui::SameLine(82, 0, 0);
			ImGui::PushItemWidth(94);
			ImGui::SliderFloat("##ProjectDamageDistance", &Vars.Misc.ProjectDamageDistance, 10.f, 100.f, u8"%.0f");
			SetTooltip(LOCALIZE_Menu(u8"Specifies how much damage is displayed above the grenade explosion radius!", u8"指定显示多少伤害以上的手雷爆炸半径！"));
			ImGui::PopItemWidth();
			ImGui::SameLine(14, 0, 20);
			ImGui::Text(LOCALIZE_Menu(u8"Project Radius", u8"投掷伤害范围"));
			ImGui::SameLine(82, 0, 0);
			ImGui::PushItemWidth(94);
			ImGui::SliderFloat("##ProjectRadiusDistance", &Vars.Misc.ProjectRadiusDistance, 20.f, 100.f, u8"%.0f");
			SetTooltip(LOCALIZE_Menu(u8"Draw the explosion range of the grenade within the specified range!", u8"在指定范围内绘制手雷的爆炸范围！"));
			ImGui::PopItemWidth();

		}ImGui::EndChild();

		ImGui::SameLine(94, 0, 190);
		ImGui::BeginChild(("##tabs3"), ImVec2(446, 260), false);
		{
			if (Vars.Misc.ShareRadar)
			{
				ImGui::SameLine(194, 0, 14);
				ImGui::Text(LOCALIZE_Menu(u8"Radar IP Address", u8"雷达IP地址"));
				ImGui::SameLine(188, 0, 24);
				ImGui::PushItemWidth(200);
				ImGui::InputText(LOCALIZE_Menu(u8"##IP", u8"##IP2"), Vars.Misc.txt_RadarIP, 45);
				SetTooltip(LOCALIZE_Menu(u8"Enter the correct domain name pointing to the shared radar", u8"输入正确指向共享雷达的域名"));
				ImGui::PopItemWidth();
				ImGui::SameLine(390, 0, 0);
				if (ImGui::Button(ConnectState == 0 ? LOCALIZE_Menu(u8"Connect", u8"连接") :
					ConnectState == 1 ? LOCALIZE_Menu(u8"Connecting", u8"连接中") : LOCALIZE_Menu(u8"Connected", u8"已连接")))
				{
					if (ConnectState == 0)
					{
						ConnectedThread = CreateThread(NULL, NULL, ThreadTryConnect, Vars.Misc.txt_RadarIP, NULL, NULL);
						IsTryConnected = true;
						ConnectState = 1;
						ConnectedStartTime = GetTickCount64();
					}
					else if (ConnectState == 2)
					{
						ConnectState = 0;
						RadarIP = "";
					}
				}
				SetTooltip(LOCALIZE_Menu(u8"Connect to shared radar", u8"与共享雷达建立连接"));
				ImGui::SameLine(290, 0, 24);
				if (ImGui::Button(LOCALIZE_Menu(u8"Copy Radar URL", u8"复制雷达URL"), ImVec2(98, 24)))
				{
					Utils::writeToClipboard(RadarIP);
				}
				SetTooltip(LOCALIZE_Menu(u8"Copy the link of the shared radar to your clipboard", u8"将共享雷达的连结复制到贴上板"));
				ImGui::SameLine(14, 0, -48);
			}
			else
			{
				ImGui::SameLine(14, 0, 14);
			}
			ImGui::Text(LOCALIZE_Menu(u8"DisplayMode", u8"绘制方式"));
			ImGui::SameLine(10, 0, 24);
			ImGui::PushItemWidth(160);
			if (Vars.Menu.language)
				ImGui::Combo(("##display"), &Vars.Misc.DisplayMode, Display_itemsEN, IM_ARRAYSIZE(Display_itemsEN));
			else
				ImGui::Combo(("##display"), &Vars.Misc.DisplayMode, Display_itemsCN, IM_ARRAYSIZE(Display_itemsCN));
			SetTooltip(LOCALIZE_Menu(u8"Switch between streaming and blending modes (streaming mode locks the screen refresh rate by default, while blending mode does not lock the frame)", u8"切换串流、融合器模式(串流模式预设锁定帧萤幕更新率融合器模式不锁定帧)"));

			ImGui::PopItemWidth();

			ImGui::SameLine(14, 0, 24);
			ImGui::Text(LOCALIZE_Menu(u8"Miscellaneous features", u8"杂项功能"));

			//ImGui::SameLine(8, 0, 24);
			//ImGui::Checkbox2(LOCALIZE_Menu(u8"EnableCollision(Suspend)", u8"無視碰撞(暫停)"), &Vars.Misc.EnableCollision);
			ImGui::SameLine(8, 0, 24);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Not Lock Frames", u8"不锁定帧率"), &Vars.Menu.LockFlame);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"SoftWare Info", u8"辅助资讯"), &Vars.Menu.DataInfo);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Highlight Blacklist", u8"高亮黑名单"), &Vars.Misc.BlackListShow);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Announcement", u8"公告"), &Vars.Menu.AnnoceOpen);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"All Player Info", u8"全体玩家资讯"), &Vars.Misc.BattlePlayerInfo);
			ImGui::SameLine(14, 0, 24);
			ImGui::Text(LOCALIZE_Menu(u8"Rank query server source", u8"段位查询伺服器来源"));
			ImGui::SameLine(14, 0, 24);
			ImGui::PushItemWidth(160);
			ImGui::Combo(("##rank"), &Vars.espRank.RankServer, Rank_items, IM_ARRAYSIZE(Rank_items));
			ImGui::PopItemWidth();
			ImGui::SameLine(275, 0, 0);
			if (ImGui::Button(LOCALIZE_Menu("Exit Paod", u8"一键退出"), ImVec2(140, 22)))
			{
				TerminateProcess(GetCurrentProcess(), -1);
			}

		}ImGui::EndChild();

		ImVec2 NextPos = ImGui::GetNextWindowPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(NextPos.x + 564.f, NextPos.y + 6.f), ImVec2(NextPos.x + 564.f + 180.f, NextPos.y + 6.f + 22.f), ImColor(42, 45, 54), 14.f);
		RenderText(OverlayEngine::Font16px, 16, 0, ImVec2(NextPos.x + 654.f, NextPos.y + 16.f), 0xFFFFFFFF, 0, FONT_CENTER, LOCALIZE_Menu(u8"Menu", u8"菜单"));

		ImGui::SameLine(545, 0, -190);
		ImGui::BeginChild(("##tabs4"), ImVec2(222, 450), false);
		{
			SaveHotKey.push_back(Vars.Menu.OpenedHotKey);
			SaveHotKey.push_back(Vars.Menu.SettingSwitchHotKey);
			SaveHotKey.push_back(Vars.Menu.FreshHotKey);
			ImGui::SameLine(14, 0, 10);
			add_hotkey(LOCALIZE_Menu(u8"Menu Keys", u8"菜单快捷键"), &Vars.Menu.OpenedHotKey, NameText9, &IsFind9);
			SetTooltip(LOCALIZE_Menu(u8"Hotkeys for calling out and hiding menus", u8"呼出、隐藏选单的快速热键"));
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Manual Refresh ", u8"手动刷新"), &Vars.Menu.FreshHotKey, NameText15, &IsFind15);
			SetTooltip(LOCALIZE_Menu(u8"Refresh All Data", u8"全部刷新数据"));
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Configuration Keys", u8"切换配置快捷键"), &Vars.Menu.SettingSwitchHotKey, NameText10, &IsFind10);
			SetTooltip(LOCALIZE_Menu(u8"Switch between the two saved configurations", u8"将已储存的2个配置进行切换"));
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Refresh the Physics Scene ", u8"刷新物理场景"), &Vars.Menu.FreshTrace, NameText18, &IsFind18);
			SetTooltip(LOCALIZE_Menu(u8"Clear the physics scene and reload it", u8"对物理场景进行清除之后重新载入"));
			SaveHotKey.clear();

			ImGui::SameLine(14, 0, 14);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.f, 1.0f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.32f, 0.32f, 0.32f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.f, 0.f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4039f, 0.5725f, 0.4039f, 1.0f));
			ImGui::Button(LOCALIZE_Menu(u8"Save", u8"保存"), ImVec2(193, 22));
			SetTooltip(LOCALIZE_Menu(u8"Save the menu settings", u8"将选单设定进行储存"));
			if (ImGui::IsItemClicked())
				Config->Save(Vars.Menu.config == 0 ? hudPath1 : hudPath2);

			ImGui::SameLine(14, 0, 30);
			ImGui::Button(LOCALIZE_Menu(u8"Reset", u8"重设"), ImVec2(193, 22));
			SetTooltip(LOCALIZE_Menu(u8"Reset the menu settings to the initial state", u8"将选单设定重置至初始化状态"));
			if (ImGui::IsItemClicked())
			{
				auto configsave = Vars.Menu.config;
				Config->Setup();
				Vars.Menu.config = configsave;
				Config->Save2(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
				Vars.Menu.config = configsave;
				Config->Load(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}
			ImGui::PopStyleColor(4);
			if (g_ServerResult.Version >= 2)
			{
				ImGui::SameLine(5, 0, 20);
				ImGui::PushFont(OverlayEngine::Font20px);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 1, 1));
				ImGui::Text(g_ServerResult.Version >= 2 ? LOCALIZE_Menu(u8"Physical Model Rendering", u8"实体模型渲染") : LOCALIZE_Menu(u8"Physical Model Rendering(Unauthorized)", u8"实体模型渲染(未授权)"));
				ImGui::PopStyleColor();
				ImGui::PopFont();
				ImGui::SameLine(10, 0, 25);
				ImGui::Text(LOCALIZE_Menu(u8"Max Fresh Distance", u8"最大刷新距离"));
				ImGui::SameLine(10, 0, 15);
				ImGui::PushItemWidth(180);
				ImGui::SliderFloat(("##Max Fresh Distance"), &Vars.Aimbot.MaxDynamicRigidDistance, 100.f, 1000.f, ("%.0f"));
				SetTooltip(LOCALIZE_Menu(u8"Adjust refresh to destroy entity range, adjust according to DMA speed", u8"调节刷新可破坏实体范围,根据DMA速度调节"));
				ImGui::PopItemWidth();

				ImGui::SameLine(10, 0, 25);
				ImGui::Text(LOCALIZE_Menu(u8"Max Update Barrier Count", u8"最大更新障碍物数量"));
				ImGui::SameLine(10, 0, 15);
				ImGui::PushItemWidth(180);
				ImGui::SliderInt(("##Max Update Count"), &Vars.Aimbot.MaxUpdateRigid, 1000, 3000);
				SetTooltip(LOCALIZE_Menu(u8"Adjust the number of entities updated each time. The larger the number, the faster the refresh speed (needs to be adjusted according to the DMA speed measurement)", u8"调节每次更新的实体数量,数量越大则刷新速度越快(需根据DMA的测速进行调整)"));
				ImGui::PopItemWidth();
				ImGui::SameLine(10, 0, 25);
				ImGui::InputText(LOCALIZE_Menu(u8"Cache Directory", u8"快取档案目录"), Vars.Aimbot.txt_CacheSave, 32);
				SetTooltip(LOCALIZE_Menu(u8"Set the path to store local models", u8"设定储存本地模型的路径"));
				ImGui::SameLine(10, 0, 25);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"Local Model Resources", u8"本地模型资源"), &Vars.Menu.UseCacheToTrace);
				SetTooltip(LOCALIZE_Menu(u8"Read resources from local hard disk to load game models faster", u8"从本机硬碟读取资源以更快的速度载入游戏模型"));
#ifdef ENABLE_CONSOLE
				ImGui::SameLine(10, 0, 25);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"Show Debug Mode", u8"开启调试模式"), &Vars.Aimbot.DebugMode);
				SetTooltip(LOCALIZE_Menu(u8"Enable debug mode to visualize entities", u8"开启调试模式以进行视觉化展示实体"));
				ImGui::SameLine(10, 0, 25);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"Show Ray Tracing", u8"显示光线追踪"), &Vars.Aimbot.RayTracing);
				SetTooltip(LOCALIZE_Menu(u8"Send a beam to the enemy's head. Green means no obstacles and red means obstacles.", u8"发射一道光线直至敌人头部绿色为无障碍红色为有障碍"));
				ImGui::SameLine(10, 0, 25);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"Show Skeletal", u8"显示骨骼网络"), &Vars.Aimbot.TriangleMesh);
				SetTooltip(LOCALIZE_Menu(u8"Draw the skeleton of the entity concretely", u8"将实体的骨骼具象化绘制"));
				ImGui::SameLine(10, 0, 25);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"Show Indestructible", u8"显示不可破坏的"), &Vars.Aimbot.StaticMesh);
				SetTooltip(LOCALIZE_Menu(u8"Show indestructible entities", u8"将不可破坏的实体显示出来"));
				ImGui::SameLine(10, 0, 25);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"Show Destructible", u8"显示可破坏的"), &Vars.Aimbot.DynamicMesh);
				SetTooltip(LOCALIZE_Menu(u8"Show destructible entities", u8"将可破坏的实体显示出来"));
#else
				Vars.Aimbot.DebugMode = 0;
				Vars.Aimbot.RayTracing = 0;
				Vars.Aimbot.TriangleMesh = 0;
				Vars.Aimbot.StaticMesh = 0;
				Vars.Aimbot.DynamicMesh = 0;
#endif
			}

		}ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void Menu::RenderAimbot()
	{
		ImGui::SameLine(290, 0, 0);
		RenderTabName(LOCALIZE_Menu(u8"Aimbot", u8"自瞄"));
		ImGui::SameLine(94, 0, 36);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16470f, 0.17647f, 0.20784f, 1.f));
		ImGui::BeginChild(("##tabs1"), ImVec2(220, 94), false);
		{
			ImGui::SameLine(8, 0, 8);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"AimBot", u8"自动瞄准"), &Vars.Aimbot.Aimbot);
			SetTooltip(LOCALIZE_Menu(u8"AimBot", u8"启动自动瞄准"));
			ImGui::SameLine(108, 0, 0);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"DeadZone", u8"死区自瞄"), &Vars.Aimbot.AimbotDrag);
			SetTooltip(LOCALIZE_Menu(u8"You can drag to switch to another target while aiming", u8"自瞄时可以拖曳切换另一个目标"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(!Vars.Aimbot.AimMode ? LOCALIZE_Menu(u8"PID Mode ", u8"演员模式") : LOCALIZE_Menu(u8"Violent Mode", u8"暴力模式"), &Vars.Aimbot.AimMode);
			SetTooltip(LOCALIZE_Menu(u8"Select the parameter type for auto aim", u8"选择自瞄准的参数类型"));
			if (HideFunction >= 10)
			{
				ImGui::SameLine(108, 0, 0);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"High Efficiency", u8"高效自瞄(Net)"), &Vars.Aimbot.HighAimbot);
				SetTooltip(LOCALIZE_Menu(u8"Only suitable for Kmbox Net controller,Gives the auto-aim higher efficiency and gives the enemy a more accurate attack", u8"只适合Kmbox Net控制器,赋予自瞄更高的效率,给予敌人更精准的打击"));
			}
			ImGui::SameLine(8, 0, 20);
			if (!Vars.Aimbot.AimMode)
			{
				if (Vars.Aimbot.MenuWeaponType == 0)
					ImGui::Checkbox2(LOCALIZE_Menu(u8"No Recoil", u8"算法压枪"), &Vars.Aimbot.AR_NoRecoil1);
				else if (Vars.Aimbot.MenuWeaponType == 1)
					ImGui::Checkbox2(LOCALIZE_Menu(u8"No Recoil", u8"算法压枪"), &Vars.Aimbot.SR_NoRecoil1);
				else
					ImGui::Checkbox2(LOCALIZE_Menu(u8"No Recoil", u8"算法压枪"), &Vars.Aimbot.DMR_NoRecoil1);
			}
			else
			{
				if (Vars.Aimbot.MenuWeaponType == 0)
					ImGui::Checkbox2(LOCALIZE_Menu(u8"No Recoil", u8"算法压枪"), &Vars.Aimbot.AR_NoRecoil2);
				else if (Vars.Aimbot.MenuWeaponType == 1)
					ImGui::Checkbox2(LOCALIZE_Menu(u8"No Recoil", u8"算法压枪"), &Vars.Aimbot.SR_NoRecoil2);
				else
					ImGui::Checkbox2(LOCALIZE_Menu(u8"No Recoil", u8"算法压枪"), &Vars.Aimbot.DMR_NoRecoil2);
			}
			SetTooltip(LOCALIZE_Menu(u8"Perform algorithm pressure", u8"进行演算法压枪"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Not Aim Through Walls", u8"掩体不瞄"), &Vars.Aimbot.VisibleCheck);
			SetTooltip(LOCALIZE_Menu(u8"Not Aim Through Walls", u8"启动掩体不瞄"));
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Crosshair"), Vars.Aimbot.CrosshairColor, 0);
		}ImGui::EndChild();
		ImGui::SameLine(320, 0, 0);
		ImGui::BeginChild(("##tabs2"), ImVec2(220, 94), false);
		{
			ImGui::SameLine(8, 0, 8);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Not-Aim-Teammate", u8"剔除队友"), &Vars.Aimbot.Team);
			SetTooltip(LOCALIZE_Menu(u8"Not-Aim-Teammate", u8"启动不瞄准队友"));
			ImGui::SameLine(8, 0, 20);
			int* KnockValue = Vars.Aimbot.MenuWeaponType == 0 ? &Vars.Aimbot.AR_Knock : Vars.Aimbot.MenuWeaponType == 1 ? &Vars.Aimbot.SR_Knock : &Vars.Aimbot.DMR_Knock;
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Aim-KnockDwonTarget", u8"瞄准倒地玩家"), KnockValue);
			SetTooltip(LOCALIZE_Menu(u8"Aim-KnockDwonTarget", u8"启动瞄准倒地玩家"));
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Draw FOV", u8"绘制FOV圈"), &Vars.Aimbot.FovShow);
			SetTooltip(LOCALIZE_Menu(u8"Draw FOV Circle", u8"启动绘制FOV圈"));
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##FOV"), Vars.Aimbot.FovColor, 0);
			ImGui::SameLine(8, 0, 20);
			int* PreditctOpen = Vars.Aimbot.MenuWeaponType == 0 ? &Vars.Aimbot.AR_PredictionPoint : Vars.Aimbot.MenuWeaponType ? &Vars.Aimbot.SR_PredictionPoint : &Vars.Aimbot.DMR_PredictionPoint;
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Prediction Point", u8"预瞄点"), PreditctOpen);
			SetTooltip(LOCALIZE_Menu(u8"Prediction Point", u8"启动预瞄点"));
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Prediction"), Vars.Aimbot.PredictionPointColor, 0);
		}ImGui::EndChild();

		ImGui::SameLine(94, 0, 100);
		ImGui::BeginChild(("##tabs3"), ImVec2(446, 150), false);
		{
			if (!Vars.Aimbot.AimMode)
			{
				ImGui::SameLine(2, 0, 0);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.5f, 0, 1));
				ImGui::Text(LOCALIZE_Menu(u8"PID&Settings", u8"超调控制器&基础设定"));
				ImGui::PopStyleColor();

				int RunColor = Vars.Aimbot.MenuWeaponType;
				for (int i = 0; i < 3; i++)
				{
					if (RunColor == i)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
					}
					switch (i)
					{
					case 0:
					{
						ImGui::SameLine(10, 0, 20);
						if (ImGui::Button(LOCALIZE_Menu(u8"AR Config", u8"步枪设定"), ImVec2(74, 20)))
							Vars.Aimbot.MenuWeaponType = 0;
						SetTooltip(LOCALIZE_Menu(u8"Switch AR Config", u8"切换步枪设定"));
						break;
					}
					case 1:
					{
						ImGui::SameLine(104, 0, 0);
						if (ImGui::Button(LOCALIZE_Menu(u8"SR Config", u8"狙击设定"), ImVec2(74, 20)))
							Vars.Aimbot.MenuWeaponType = 1;
						SetTooltip(LOCALIZE_Menu(u8"Switch SR Config", u8"切换狙击设定"));
						break;
					}
					case 2:
					{
						ImGui::SameLine(204, 0, 0);
						if (ImGui::Button(LOCALIZE_Menu(u8"DMR Config", u8"射手步枪设定"), ImVec2(74, 20)))
							Vars.Aimbot.MenuWeaponType = 2;
						SetTooltip(LOCALIZE_Menu(u8"Switch DMR Config", u8"切换射手步枪设定"));
						break;
					}
					default:
						break;
					}
					if (RunColor == i)
					{
						ImGui::PopStyleColor();
					}
				}
				ImGui::NewLine();

				if (Vars.Aimbot.MenuWeaponType == 0)
				{
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"P-X", u8"水平拉枪速度"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##P_X"), &Vars.Aimbot.AR_P_X1, 0.1f, 1.f, ("%.2f"));
					SetTooltip(LOCALIZE_Menu(u8"The horizontal axis gun pulling speed, the larger the value, the greater the gun pressing force", u8"横轴拉枪速度，数值越大则压枪力度越大"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"P-Y", u8"垂直拉枪速度"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##P_Y"), &Vars.Aimbot.AR_P_Y1, 0.1f, 1.f, ("%.2f"));
					SetTooltip(LOCALIZE_Menu(u8"The vertical axis gun pulling speed, the larger the value, the greater the gun pressing force", u8"纵轴拉枪速度，数值越大则压枪力度越大"));
					ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Speed", u8"平滑速度"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Speed"), &Vars.Aimbot.AR_SmoothValue1, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Smooth speed adjustment, the smaller the value, the smoother the speed", u8"平滑速度调节，数值越小则速度越平缓"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Recoil", u8"反冲强度"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Recoil"), &Vars.Aimbot.AR_RecoilValue1, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The larger the recoil strength adjustment value, the greater the amplitude.", u8"反冲强度调节值越大则幅度越大"));
					ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"FOV", u8"Fov范围"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##AR_FOVRange"), &Vars.Aimbot.AR_FovRange1, 1.f, 30.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"AR FOV range, the larger the value, the larger the locking range", u8"AR的FOV范围，数值越大锁定范围越大"));
					ImGui::PopItemWidth();
				}
				else if (Vars.Aimbot.MenuWeaponType == 1)
				{
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"BaseSmoothing", u8"锁定强度(强-弱)"));
					ImGui::SameLine(80, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##BaseSmoothing"), &Vars.Aimbot.SR_baseSmoothing1, 1.f, 5.f, ("%.1f"));
					SetTooltip(LOCALIZE_Menu(u8"Lock strength, the smaller the value, the stronger the lock strength", u8"锁定强度，数值越小则锁定强度越强"));
					ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Speed", u8"平滑速度"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Speed"), &Vars.Aimbot.SR_SmoothValue1, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Smooth speed adjustment, the smaller the value, the smoother the speed", u8"平滑速度调节，数值越小则速度越平缓"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Recoil", u8"反冲强度"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Recoil"), &Vars.Aimbot.SR_RecoilValue1, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The larger the recoil strength adjustment value, the greater the amplitude.", u8"反冲强度调节值越大则幅度越大"));
					ImGui::PopItemWidth();
					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"FOV", u8"Fov范围"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##FOVRange"), &Vars.Aimbot.SR_FovRange1, 1.f, 30.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"SR FOV range, the larger the value, the larger the locking range", u8"SR的FOV范围，数值越大锁定范围越大"));
					ImGui::PopItemWidth();
				}
				else
				{
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"BaseSmoothing", u8"锁定强度(强-弱)"));
					ImGui::SameLine(80, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##BaseSmoothing"), &Vars.Aimbot.DMR_baseSmoothing1, 1.f, 5.f, ("%.1f"));
					SetTooltip(LOCALIZE_Menu(u8"Lock strength, the smaller the value, the stronger the lock strength", u8"锁定强度，数值越小则锁定强度越强"));
					ImGui::PopItemWidth();

					//ImGui::SameLine(224, 0, 0);
					//ImGui::Text(LOCALIZE_Menu(u8"MaxSmoothIncrease", u8"鎖定平滑比例"));
					//ImGui::SameLine(284, 0, 0);
					//ImGui::PushItemWidth(120);
					//ImGui::SliderFloat(("##MaxSmoothIncrease"), &Vars.Aimbot.DMR_MaxSmoothIncrease, 0.1f, 1.f, ("%.1f"));
					//ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Speed", u8"平滑速度"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Speed"), &Vars.Aimbot.DMR_SmoothValue1, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Smooth speed adjustment, the smaller the value, the smoother the speed", u8"平滑速度调节，数值越小则速度越平缓"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Recoil", u8"反沖强度"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Recoil"), &Vars.Aimbot.DMR_RecoilValue1, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The larger the recoil strength adjustment value, the greater the amplitude.", u8"反冲强度调节值越大则幅度越大"));
					ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"FOV", u8"Fov范围"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##FOVRange"), &Vars.Aimbot.DMR_FovRange1, 1.f, 30.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"DMR FOV range, the larger the value, the larger the locking range", u8"DMR的FOV范围，数值越大锁定范围越大"));
					ImGui::PopItemWidth();
				}

				ImGui::SameLine(224, 0, 0);
				ImGui::Text(LOCALIZE_Menu(u8"Crosshair", u8"准星大小"));
				ImGui::SameLine(284, 0, 0);
				ImGui::PushItemWidth(120);
				ImGui::SliderFloat(("##Crosshair"), &Vars.Aimbot.CrosshairSize, 5.f, 20.f, ("%.0f"));
				SetTooltip(LOCALIZE_Menu(u8"Draw a crosshair in the center of the screen and adjust its length", u8"在萤幕中心画出一个准星，调整他的长度"));
				ImGui::PopItemWidth();

				if (Vars.Aimbot.MenuWeaponType == 0)
				{
					ImGui::NewLine();
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Muzzle press pixel", u8"枪口下压像素"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##FOVRange"), &Vars.Aimbot.DownValue, 0.f, 20.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Pixel push down when firing without aiming", u8"在不进行自瞄的时候开火时进行像素下压"));
					ImGui::PopItemWidth();
				}
				else if (Vars.Aimbot.SingleStep)
				{
					float* VariableX = !Vars.Aimbot.AimMode ? (Vars.Aimbot.MenuWeaponType == 1 ? &Vars.Aimbot.SR_MOVESTEP_X1 : &Vars.Aimbot.DMR_MOVESTEP_X1) : (Vars.Aimbot.MenuWeaponType == 1 ? &Vars.Aimbot.SR_MOVESTEP_X2 : &Vars.Aimbot.DMR_MOVESTEP_X2);
					float* VariableY = !Vars.Aimbot.AimMode ? (Vars.Aimbot.MenuWeaponType == 1 ? &Vars.Aimbot.SR_MOVESTEP_Y1 : &Vars.Aimbot.DMR_MOVESTEP_Y1) : (Vars.Aimbot.MenuWeaponType == 1 ? &Vars.Aimbot.SR_MOVESTEP_Y2 : &Vars.Aimbot.DMR_MOVESTEP_Y2);
					ImGui::NewLine();
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"X-axis limit", u8"X轴限制"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##X-axislimit"), VariableX, 1.f, 10.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The maximum number of pixels that can be moved at a time is N.", u8"单次最多移动N像素"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Y-axis limit", u8"Y轴限制"));
					ImGui::SameLine(284, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Y-axislimit"), VariableY, 1.f, 10.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The maximum number of pixels that can be moved at a time is N.", u8"单次最多移动N像素"));
					ImGui::PopItemWidth();
				}

				if (Vars.Aimbot.MenuWeaponType == 1)
				{
					ImGui::NewLine();
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Trigger:ADS Delay", u8"扳机:开镜延迟"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Trigger:ADS Delay"), &Vars.Aimbot.SRAutoShotDelay1, 100.f, 1000.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Shoot after a delay of x milliseconds after aiming.", u8"开镜后延迟x毫秒后开枪"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Trigger:Shooting Delay", u8"扳机:开枪延迟"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Trigger:Shooting Delay"), &Vars.Aimbot.SRAutoShotDelay2, 100.f, 1000.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"There should be x milliseconds between the previous shot and the next shot.", u8"在上一枪与下一枪之间应该间隔x毫秒"));
					ImGui::PopItemWidth();
				}
				//ImGui::PushFont(OverlayEngine::Font20px);
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0, 1.f));
				//ImGui::Text(LOCALIZE_Menu(u8"Shift priority lock head", u8"Shift優先鎖頭"));
				//ImGui::PopStyleColor();
				//ImGui::PopFont();
			}
			else
			{
				ImGui::SameLine(2, 0, 0);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.5f, 0, 1));
				ImGui::Text(LOCALIZE_Menu(u8"Aim&Settings", u8"自动瞄准&基础设定"));
				ImGui::PopStyleColor();

				int RunColor = Vars.Aimbot.MenuWeaponType;
				for (int i = 0; i < 3; i++)
				{
					if (RunColor == i)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
					}
					switch (i)
					{
					case 0:
					{
						ImGui::SameLine(10, 0, 20);
						if (ImGui::Button(LOCALIZE_Menu(u8"AR Config", u8"步枪设定"), ImVec2(74, 20)))
							Vars.Aimbot.MenuWeaponType = 0;
						SetTooltip(LOCALIZE_Menu(u8"Switch AR Config", u8"切换步枪设定"));
						break;
					}
					case 1:
					{
						ImGui::SameLine(104, 0, 0);
						if (ImGui::Button(LOCALIZE_Menu(u8"SR Config", u8"狙击设定"), ImVec2(74, 20)))
							Vars.Aimbot.MenuWeaponType = 1;
						SetTooltip(LOCALIZE_Menu(u8"Switch SR Config", u8"切换狙击设定"));
						break;
					}
					case 2:
					{
						ImGui::SameLine(204, 0, 0);
						if (ImGui::Button(LOCALIZE_Menu(u8"DMR Config", u8"射手步枪设定"), ImVec2(74, 20)))
							Vars.Aimbot.MenuWeaponType = 2;
						SetTooltip(LOCALIZE_Menu(u8"Switch DMR Config", u8"切换射手步枪设定"));
						break;
					}
					default:
						break;
					}
					if (RunColor == i)
					{
						ImGui::PopStyleColor();
					}
				}
				ImGui::NewLine();

				if (Vars.Aimbot.MenuWeaponType == 0)
				{
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"P-X", u8"水平拉枪速度"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##P_X"), &Vars.Aimbot.AR_P_X2, 0.1f, 1.f, ("%.2f"));
					SetTooltip(LOCALIZE_Menu(u8"The horizontal axis gun pulling speed, the larger the value, the greater the gun pressing force", u8"横轴拉枪速度，数值越大则压枪力度越大"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"P-Y", u8"垂直拉枪速度"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##P_Y"), &Vars.Aimbot.AR_P_Y2, 0.1f, 1.f, ("%.2f"));
					SetTooltip(LOCALIZE_Menu(u8"The vertical axis gun pulling speed, the larger the value, the greater the gun pressing force", u8"纵轴拉枪速度，数值越大则压枪力度越大"));
					ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Speed", u8"平滑速度"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Speed"), &Vars.Aimbot.AR_SmoothValue2, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Smooth speed adjustment, the smaller the value, the smoother the speed", u8"平滑速度调节，数值越小则速度越平缓"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Recoil", u8"反冲强度"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Recoil"), &Vars.Aimbot.AR_RecoilValue2, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The larger the recoil strength adjustment value, the greater the amplitude.", u8"反冲强度调节值越大则幅度越大"));
					ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"FOV", u8"Fov范围"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##AR_FOVRange"), &Vars.Aimbot.AR_FovRange2, 1.f, 30.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"AR FOV range, the larger the value, the larger the locking range", u8"AR的FOV范围，数值越大锁定范围越大"));
					ImGui::PopItemWidth();
				}
				else if (Vars.Aimbot.MenuWeaponType == 1)
				{
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"BaseSmoothing", u8"锁定强度(强-弱)"));
					ImGui::SameLine(80, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##BaseSmoothing"), &Vars.Aimbot.SR_baseSmoothing2, 1.f, 5.f, ("%.1f"));
					SetTooltip(LOCALIZE_Menu(u8"Lock strength, the smaller the value, the stronger the lock strength", u8"锁定强度，数值越小则锁定强度越强"));
					ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Speed", u8"平滑速度"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Speed"), &Vars.Aimbot.SR_SmoothValue2, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Smooth speed adjustment, the smaller the value, the smoother the speed", u8"平滑速度调节，数值越小则速度越平缓"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Recoil", u8"反冲强度"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Recoil"), &Vars.Aimbot.SR_RecoilValue2, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The larger the recoil strength adjustment value, the greater the amplitude.", u8"反冲强度调节值越大则幅度越大"));
					ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"FOV", u8"Fov范围"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##FOVRange"), &Vars.Aimbot.SR_FovRange2, 1.f, 30.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"SR FOV range, the larger the value, the larger the locking range", u8"SR的FOV范围，数值越大锁定范围越大"));
					ImGui::PopItemWidth();
				}
				else
				{
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"BaseSmoothing", u8"锁定强度(强-弱)"));
					ImGui::SameLine(80, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##BaseSmoothing"), &Vars.Aimbot.DMR_baseSmoothing2, 1.f, 5.f, ("%.1f"));
					SetTooltip(LOCALIZE_Menu(u8"Lock strength, the smaller the value, the stronger the lock strength", u8"锁定强度，数值越小则锁定强度越强"));
					ImGui::PopItemWidth();

					//ImGui::SameLine(224, 0, 0);
					//ImGui::Text(LOCALIZE_Menu(u8"MaxSmoothIncrease", u8"鎖定平滑比例"));
					//ImGui::SameLine(284, 0, 0);
					//ImGui::PushItemWidth(120);
					//ImGui::SliderFloat(("##MaxSmoothIncrease"), &Vars.Aimbot.DMR_MaxSmoothIncrease, 0.1f, 1.f, ("%.1f"));
					//ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Speed", u8"平滑速度"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Speed"), &Vars.Aimbot.DMR_SmoothValue2, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Smooth speed adjustment, the smaller the value, the smoother the speed", u8"平滑速度调节，数值越小则速度越平缓"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Recoil", u8"反冲强度"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Recoil"), &Vars.Aimbot.DMR_RecoilValue2, 1.f, 100.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The larger the recoil strength adjustment value, the greater the amplitude.", u8"反冲强度调节值越大则幅度越大"));
					ImGui::PopItemWidth();

					ImGui::NewLine();

					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"FOV", u8"Fov范围"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##FOVRange"), &Vars.Aimbot.DMR_FovRange2, 1.f, 30.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"DMR FOV range, the larger the value, the larger the locking range", u8"DMR的FOV范围，数值越大锁定范围越大"));
					ImGui::PopItemWidth();
				}


				ImGui::SameLine(224, 0, 0);
				ImGui::Text(LOCALIZE_Menu(u8"Crosshair", u8"准星大小"));
				ImGui::SameLine(284, 0, 0);
				ImGui::PushItemWidth(120);
				ImGui::SliderFloat(("##Crosshair"), &Vars.Aimbot.CrosshairSize, 5.f, 20.f, ("%.0f"));
				SetTooltip(LOCALIZE_Menu(u8"Draw a crosshair in the center of the screen and adjust its length", u8"在萤幕中心画出一个准星，调整他的长度"));
				ImGui::PopItemWidth();

				if (Vars.Aimbot.MenuWeaponType == 0)
				{
					ImGui::NewLine();
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Muzzle press pixel", u8"枪口下压像素"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##FOVRange"), &Vars.Aimbot.DownValue, 0.f, 20.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Pixel push down when firing without aiming", u8"在不进行自瞄的时候开火时进行像素下压"));
					ImGui::PopItemWidth();
				}
				else if (Vars.Aimbot.SingleStep)
				{
					float* VariableX = !Vars.Aimbot.AimMode ? (Vars.Aimbot.MenuWeaponType == 1 ? &Vars.Aimbot.SR_MOVESTEP_X1 : &Vars.Aimbot.DMR_MOVESTEP_X1) : (Vars.Aimbot.MenuWeaponType == 1 ? &Vars.Aimbot.SR_MOVESTEP_X2 : &Vars.Aimbot.DMR_MOVESTEP_X2);
					float* VariableY = !Vars.Aimbot.AimMode ? (Vars.Aimbot.MenuWeaponType == 1 ? &Vars.Aimbot.SR_MOVESTEP_Y1 : &Vars.Aimbot.DMR_MOVESTEP_Y1) : (Vars.Aimbot.MenuWeaponType == 1 ? &Vars.Aimbot.SR_MOVESTEP_Y2 : &Vars.Aimbot.DMR_MOVESTEP_Y2);
					ImGui::NewLine();
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"X-axis limit", u8"X轴限制"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##X-axislimit"), VariableX, 1.f, 10.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The maximum number of pixels that can be moved at a time is N.", u8"单次最多移动N像素"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Y-axis limit", u8"Y轴限制"));
					ImGui::SameLine(284, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Y-axislimit"), VariableY, 1.f, 10.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"The maximum number of pixels that can be moved at a time is N.", u8"单次最多移动N像素"));
					ImGui::PopItemWidth();
				}
				if (Vars.Aimbot.MenuWeaponType == 1)
				{
					ImGui::NewLine();
					ImGui::SameLine(10, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Trigger:ADS Delay", u8"扳机:开镜延迟"));
					ImGui::SameLine(68, 0, -4);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Trigger:ADS Delay"), &Vars.Aimbot.SRAutoShotDelay1, 100.f, 1000.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"Shoot after a delay of x milliseconds after aiming.", u8"开镜后延迟x毫秒后开枪"));
					ImGui::PopItemWidth();

					ImGui::SameLine(224, 0, 0);
					ImGui::Text(LOCALIZE_Menu(u8"Trigger:Shooting Delay", u8"扳机:开枪延迟"));
					ImGui::SameLine(284, 0, 0);
					ImGui::PushItemWidth(120);
					ImGui::SliderFloat(("##Trigger:Shooting Delay"), &Vars.Aimbot.SRAutoShotDelay2, 100.f, 1000.f, ("%.0f"));
					SetTooltip(LOCALIZE_Menu(u8"There should be x milliseconds between the previous shot and the next shot.", u8"在上一枪与下一枪之间应该间隔x毫秒"));
					ImGui::PopItemWidth();
				}

				//ImGui::PushFont(OverlayEngine::Font20px);
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0, 1.f));
				//ImGui::Text(LOCALIZE_Menu(u8"Shift priority lock head", u8"Shift優先鎖頭"));
				//ImGui::PopStyleColor();
				//ImGui::PopFont();
			}
		}ImGui::EndChild();

		ImGui::SameLine(94, 0, 154);
		ImGui::BeginChild(("##tabs4"), ImVec2(446, 196), false);
		{

			if (g_ServerResult.Version >= 2)
			{
				ImGui::SameLine(334, 0, 10);
				ImGui::Text(LOCALIZE_Menu(u8"Automatic trigger: (SR/SHOTGUN)", u8"自动扳机：霰弹/栓狙"));
				ImGui::SameLine(328, 0, 20);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"Trigger Bot", u8"自动扳机"), &Vars.Aimbot.AutoShot);
				SetTooltip(LOCALIZE_Menu(u8"Automatically fire when the muzzle crosshair moves to the enemy", u8"在枪口准星移动到敌人身上时进行自动开火"));
				ImGui::SameLine(328, 0, 20);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"Rifer", u8"步枪"), &Vars.Aimbot.RiferAutoShot);
				SetTooltip(LOCALIZE_Menu(u8"Automatically fire the rifer", u8"对步枪进行自动开火"));
				ImGui::SameLine(328, 0, 20);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"ShotGun", u8"霰弹"), &Vars.Aimbot.ShotGunAutoShot);
				SetTooltip(LOCALIZE_Menu(u8"Automatically fire the shotgun", u8"对霰弹枪进行自动开火"));
				ImGui::SameLine(328, 0, 20);
				ImGui::Checkbox2(LOCALIZE_Menu(u8"SR", u8"栓狙"), &Vars.Aimbot.SRGunAutoShot);
				SetTooltip(LOCALIZE_Menu(u8"Automatically fire sniper rifles", u8"对狙击枪进行自动开火"));
				ImGui::SameLine(328, 0, 20);
			}
			else
			{
				ImGui::SameLine(328, 0, 16);
			}
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Crosshair", u8"准星"), &Vars.Aimbot.Crosshair);
			SetTooltip(LOCALIZE_Menu(u8"Enable crosshair drawing switch", u8"启动准星绘制开关"));
			ImGui::SameLine(428, 0, 0);
			ImGui::ColorEdit3(("Color##Crosshair"), Vars.Aimbot.CrosshairColor, 0);
			ImGui::SameLine(328, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"GrenadeTime", u8"手雷倒计时"), &Vars.Misc.GrenadeCount);
			SetTooltip(LOCALIZE_Menu(u8"Start grenade countdown", u8"启动手雷倒数计时"));
			ImGui::SameLine(328, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Aim Only Scoping", u8"开镜自瞄"), &Vars.Aimbot.AimWhileScoping);
			SetTooltip(LOCALIZE_Menu(u8"Start Aim Only Scoping", u8"启动开镜自瞄"));
			ImGui::SameLine(328, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Single Step Limit", u8"单步移动限制"), &Vars.Aimbot.SingleStep);
			SetTooltip(LOCALIZE_Menu(u8"Single Step Limit", u8"启动单步移动限制"));
			ImGui::SameLine(208, 0, 0);
			if (ImGui::Button(LOCALIZE_Menu(u8"Reset Settings", u8"重置自瞄参数"), ImVec2(100, 20)))
			{
				SetDefaultAimSettings();
			}
			ImGui::SameLine(328, 0, -20);
			if (g_ServerResult.Version >= 2)
				ImGui::SameLine(14, 0, -144);
			else
				ImGui::SameLine(14, 0, -70);
			ImGui::Text(LOCALIZE_Menu(u8"Aimbot Mathod", u8"瞄准方式"));
			ImGui::SameLine(12, 0, 22);
			ImGui::PushItemWidth(120);
			ImGui::Combo(LOCALIZE_Menu(u8"Controller", u8"控制器"), &Vars.Aimbot.Controller_Current, Controller_items, IM_ARRAYSIZE(Controller_items));
			SetTooltip(LOCALIZE_Menu(u8"Select your controller to connect and start auto-aim", u8"选择你的控制器进行连接后开始自动瞄准"));
			ImGui::PopItemWidth();
			ImGui::SameLine(12, 0, 24);

			auto InitComboArray = [=](auto InArray, auto& OutArray)
				{
					int Numi = 0;
					for (auto iter : InArray)
					{
						if (!OutArray[Numi])
						{
							OutArray[Numi] = (char*)malloc(0x20);
							ZeroMemory(OutArray[Numi], 0x20);
							memcpy(OutArray[Numi], iter.first.data(), iter.first.size());
						}
						Numi++;
					}
				};

			if (Vars.Aimbot.Controller_Current == 0)
			{
				char* KmboxArray[20] = { 0 };
				InitComboArray(KmboxName, KmboxArray);
				ImGui::PushItemWidth(200);
				ImGui::Combo(LOCALIZE_Menu(u8"Serial Port", u8"串口"), &CurrentKmboxItem, KmboxArray, (int)KmboxName.size());
				//if (ImGui::IsItemHovered())
				//	ImGui::SetTooltip(LOCALIZE_Menu(u8"Select Serial Port", u8"選擇串口"));
				ImGui::SameLine(12, 0, 24);
				ImGui::InputText(LOCALIZE_Menu(u8"Baud Rate", u8"波特率"), Vars.Aimbot.txt_BPPort, 8);
				//if (ImGui::IsItemHovered())
				//	ImGui::SetTooltip(LOCALIZE_Menu(u8"Select Baud Rate", u8"選擇波特率"));
				ImGui::SameLine(12, 0, 24);
				ImGui::PopItemWidth();
				if (ImGui::Button(
					Vars.Aimbot.BProStatus == 0 ? LOCALIZE_Menu(u8"Connect", u8"连接") : LOCALIZE_Menu(u8"Disconnect", u8"断开")
					, ImVec2(120, 20)))
				{
					if (Vars.Aimbot.BProStatus == 0)
					{
						Utils::hasNullTerminator(KmboxArray[CurrentKmboxItem], 32);
						std::string PortName = std::string(KmboxArray[CurrentKmboxItem]);
						CONSOLE_INFO("%s", PortName.c_str());
						auto FindResult = KmboxName.find(PortName);
						if (FindResult != KmboxName.end())
						{
							auto NumBaud = atoi(Vars.Aimbot.txt_BPPort);
							if (GetKmbox().open(FindResult->second, NumBaud))
							{
								Vars.Aimbot.BProStatus = 1;
								Vars.Aimbot.AimBotMethod = 1;
							}
						}
					}
					else
					{
						GetKmbox().close();
						Vars.Aimbot.BProStatus = 0;
						Vars.Aimbot.AimBotMethod = 0;
					}
				}
				ImGui::SameLine(12, 0, 24);

				if (Vars.Aimbot.BProStatus == 1)
				{
					if (ImGui::Button(LOCALIZE_Menu(u8"Test Move", u8"测试移动"), ImVec2(120, 20)))
					{
						GetKmbox().SendMove(100, 100);
					}
					//if (ImGui::IsItemHovered())
					//	ImGui::SetTooltip(LOCALIZE_Menu(u8"Perform a mouse movement test", u8"測試移動"));
				}
			}
			else if (Vars.Aimbot.Controller_Current == 1)
			{
				ImGui::PushItemWidth(200);
				ImGui::InputText("IP", Vars.Aimbot.txt_IP, 16);
				ImGui::SameLine(12, 0, 24);
				ImGui::InputText("Port", Vars.Aimbot.txt_Port, 16);
				ImGui::SameLine(12, 0, 24);
				ImGui::InputText("UUID", Vars.Aimbot.txt_UUID, 16);
				ImGui::PopItemWidth();
				ImGui::SameLine(12, 0, 24);
				if (ImGui::Button(
					Vars.Aimbot.BNetStatus == 0 ? LOCALIZE_Menu(u8"Connect", u8"连接") : LOCALIZE_Menu(u8"Disconnect", u8"断开")
					, ImVec2(120, 20)))
				{
					if (Vars.Aimbot.BNetStatus == 0)
					{
						FirstInitKmbox = false;
						//if (kmNet_init(Vars.Aimbot.txt_IP, Vars.Aimbot.txt_Port, Vars.Aimbot.txt_UUID) == 0)
						//{
						//	Vars.Aimbot.BNetStatus = 1;
						//	Vars.Aimbot.AimBotMethod = 2;
						//	Config->Save(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
						//}
					}
					else
					{
						Vars.Aimbot.BNetStatus = 0;
						Vars.Aimbot.AimBotMethod = 0;
					}
					Config->Save(Vars.Menu.config == 0 ? hudPath1 : hudPath2);

				}
				ImGui::SameLine(12, 0, 24);

				if (Vars.Aimbot.BNetStatus == 1)
				{
					if (ImGui::Button(LOCALIZE_Menu(u8"Test Move", u8"测试移动"), ImVec2(120, 20)))
					{
						kmNet_mouse_move_auto(100, 100, 0);
					}
				}
			}
			else if (Vars.Aimbot.Controller_Current == 2)
			{
				char* KmboxArray[20] = { 0 };
				InitComboArray(KmboxName, KmboxArray);
				ImGui::PushItemWidth(200);
				ImGui::Combo(LOCALIZE_Menu(u8"Serial Port", u8"串口"), &CurrentKmboxItem, KmboxArray, (int)KmboxName.size());
				ImGui::SameLine(12, 0, 24);
				ImGui::InputText(LOCALIZE_Menu(u8"Baud Rate", u8"波特率"), Vars.Aimbot.txt_LKPort, 8);
				ImGui::SameLine(12, 0, 24);
				ImGui::PopItemWidth();
				if (ImGui::Button(
					Vars.Aimbot.LurkerStatus == 0 ? LOCALIZE_Menu(u8"Connect", u8"连接") : LOCALIZE_Menu(u8"Disconnect", u8"断开")
					, ImVec2(120, 20)))
				{
					if (Vars.Aimbot.LurkerStatus == 0)
					{
						Utils::hasNullTerminator(KmboxArray[CurrentKmboxItem], 32);
						std::string PortName = std::string(KmboxArray[CurrentKmboxItem]);
						auto FindResult = KmboxName.find(PortName);
						if (FindResult != KmboxName.end())
						{
							auto NumBaud = atoi(Vars.Aimbot.txt_LKPort);
							if (GetKmbox().open(FindResult->second, NumBaud))
							{
								Vars.Aimbot.LurkerStatus = 1;
								Vars.Aimbot.AimBotMethod = 3;
							}
						}
					}
					else
					{
						GetKmbox().close();
						Vars.Aimbot.LurkerStatus = 0;
						Vars.Aimbot.AimBotMethod = 0;
					}
				}
				ImGui::SameLine(12, 0, 24);

				if (Vars.Aimbot.LurkerStatus == 1)
				{
					if (ImGui::Button(LOCALIZE_Menu(u8"Test Move", u8"测试移动"), ImVec2(120, 20)))
					{
						GetKmbox().SendMoveLurker(100.f, 100.f);
					}
				}
			}
			else if (Vars.Aimbot.Controller_Current == 3)
			{
				char* KmboxArray[20] = { 0 };
				InitComboArray(KmboxName, KmboxArray);
				ImGui::PushItemWidth(200);
				ImGui::Combo(LOCALIZE_Menu(u8"Serial Port", u8"串口"), &CurrentKmboxItem, KmboxArray, (int)KmboxName.size());
				ImGui::SameLine(12, 0, 24);
				ImGui::InputText(LOCALIZE_Menu(u8"Baud Rate", u8"波特率"), Vars.Aimbot.txt_JSPort, 8);
				ImGui::SameLine(12, 0, 24);
				ImGui::PopItemWidth();
				if (ImGui::Button(
					Vars.Aimbot.JSMHStatus == 0 ? LOCALIZE_Menu(u8"Connect", u8"连接") : LOCALIZE_Menu(u8"Disconnect", u8"断开")
					, ImVec2(120, 20)))
				{
					if (Vars.Aimbot.JSMHStatus == 0)
					{
						Utils::hasNullTerminator(KmboxArray[CurrentKmboxItem], 32);
						std::string PortName = std::string(KmboxArray[CurrentKmboxItem]);
						auto FindResult = KmboxName.find(PortName);
						if (FindResult != KmboxName.end())
						{
							auto NumBaud = atoi(Vars.Aimbot.txt_JSPort);
							if (GetKmbox().open(FindResult->second, NumBaud))
							{
								Vars.Aimbot.JSMHStatus = 1;
								Vars.Aimbot.AimBotMethod = 4;
							}
						}
					}
					else
					{
						GetKmbox().close();
						Vars.Aimbot.JSMHStatus = 0;
						Vars.Aimbot.AimBotMethod = 0;
					}
				}
				ImGui::SameLine(12, 0, 24);

				if (Vars.Aimbot.JSMHStatus == 1)
				{
					if (ImGui::Button(LOCALIZE_Menu(u8"Test Move", u8"测试移动"), ImVec2(120, 20)))
					{
						GetKmbox().SendMoveJSMH(100.f, 100.f);
					}
				}
			}
		}ImGui::EndChild();

		ImVec2 NextPos = ImGui::GetNextWindowPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(NextPos.x + 564.f, NextPos.y + 6.f), ImVec2(NextPos.x + 564.f + 180.f, NextPos.y + 6.f + 22.f), ImColor(42, 45, 54), 14.f);
		RenderText(OverlayEngine::Font16px, 16, 0, ImVec2(NextPos.x + 654.f, NextPos.y + 16.f), 0xFFFFFFFF, 0, FONT_CENTER, LOCALIZE_Menu(u8"Key Settings", u8"按键设定"));

		ImGui::SameLine(545, 0, -254);
		ImGui::BeginChild(("##tabs5"), ImVec2(222, 450), false);
		{
			if (Vars.Aimbot.MenuWeaponType == 0)
			{
				SaveHotKey.push_back(Vars.Aimbot.AR_HotKey1);
				SaveHotKey.push_back(Vars.Aimbot.AR_HotKey2);
				SaveHotKey.push_back(Vars.Aimbot.AR_LockHeadHotKey);
			}
			else if (Vars.Aimbot.MenuWeaponType == 1)
			{
				SaveHotKey.push_back(Vars.Aimbot.SR_HotKey1);
				SaveHotKey.push_back(Vars.Aimbot.SR_HotKey2);
				SaveHotKey.push_back(Vars.Aimbot.SR_LockHeadHotKey);
			}
			else
			{
				SaveHotKey.push_back(Vars.Aimbot.DMR_HotKey1);
				SaveHotKey.push_back(Vars.Aimbot.DMR_HotKey2);
				SaveHotKey.push_back(Vars.Aimbot.DMR_LockHeadHotKey);
			}
			SaveHotKey.push_back(Vars.Aimbot.PauseLockKey);
			SaveHotKey.push_back(Vars.Aimbot.HotKeyCar);
			SaveHotKey.push_back(Vars.Aimbot.EnableHotKey);
			SaveHotKey.push_back(Vars.Aimbot.AutoShotHotKey);
			SaveHotKey.push_back(Vars.Menu.战斗模式HotKey);
			SaveHotKey.push_back(Vars.espItem.EnableHotKey);
			SaveHotKey.push_back(Vars.espVehicle.EnableHotKey);
			SaveHotKey.push_back(Vars.espItem.SmartItemHotKey);
			SaveHotKey.push_back(Vars.Misc.BattlefieldInfoHotKey);
			SaveHotKey.push_back(Vars.Misc.BattlefieldUnlockHotKey);
			SaveHotKey.push_back(Vars.Misc.BattlePlayerInfoHotKey);
			SaveHotKey.push_back(Vars.Aimbot.InKnockDownHotKey);
			ImGui::SameLine(14, 0, 10);
			if (Vars.Aimbot.MenuWeaponType == 0)
			{
				add_hotkey(LOCALIZE_Menu(u8"AR Aim Key1", u8"步枪自瞄按键1"), &Vars.Aimbot.AR_HotKey1, NameText1, &IsFind1);
				ImGui::SameLine(14, 0, 8);
				add_hotkey(LOCALIZE_Menu(u8"AR Aim Key2", u8"步枪自瞄按键2"), &Vars.Aimbot.AR_HotKey2, NameText13, &IsFind13);
				ImGui::SameLine(14, 0, 8);
				add_hotkey(LOCALIZE_Menu(u8"AR LockHead", u8"步枪强制锁头"), &Vars.Aimbot.AR_LockHeadHotKey, NameText16, &IsFind16);
				ImGui::SameLine(14, 0, 8);
			}
			else if (Vars.Aimbot.MenuWeaponType == 1)
			{
				add_hotkey(LOCALIZE_Menu(u8"SR Aim Key1", u8"狙击自瞄按键1"), &Vars.Aimbot.SR_HotKey1, NameText1, &IsFind1);
				ImGui::SameLine(14, 0, 8);
				add_hotkey(LOCALIZE_Menu(u8"SR Aim Key2", u8"狙击自瞄按键2"), &Vars.Aimbot.SR_HotKey2, NameText13, &IsFind13);
				ImGui::SameLine(14, 0, 8);
				add_hotkey(LOCALIZE_Menu(u8"SR LockHead", u8"狙击强制锁头"), &Vars.Aimbot.SR_LockHeadHotKey, NameText16, &IsFind16);
				ImGui::SameLine(14, 0, 8);
			}
			else
			{
				add_hotkey(LOCALIZE_Menu(u8"DMR Aim Key1", u8"连狙自瞄按键1"), &Vars.Aimbot.DMR_HotKey1, NameText1, &IsFind1);
				ImGui::SameLine(14, 0, 8);
				add_hotkey(LOCALIZE_Menu(u8"DMR Aim Key2", u8"连狙自瞄按键2"), &Vars.Aimbot.DMR_HotKey2, NameText13, &IsFind13);
				ImGui::SameLine(14, 0, 8);
				add_hotkey(LOCALIZE_Menu(u8"DMR LockHead", u8"连狙强制锁头"), &Vars.Aimbot.DMR_LockHeadHotKey, NameText16, &IsFind16);
				ImGui::SameLine(14, 0, 8);
			}
			add_hotkey(LOCALIZE_Menu(u8"Pause Aim", u8"停滞自瞄"), &Vars.Aimbot.PauseLockKey, NameText14, &IsFind14);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Lock Car-Key", u8"扫车模式"), &Vars.Aimbot.HotKeyCar, NameText2, &IsFind2);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Aimbot Enable", u8"自瞄开关快速键"), &Vars.Aimbot.EnableHotKey, NameText3, &IsFind3);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Trigger Key", u8"自动扳机热键"), &Vars.Aimbot.AutoShotHotKey, NameText4, &IsFind4);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Battle Mode", u8"对战模式热键"), &Vars.Menu.战斗模式HotKey, NameText5, &IsFind5);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Item Enable", u8"物资透视热键"), &Vars.espItem.EnableHotKey, NameText6, &IsFind6);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Vehicle Enable", u8"载具透视热键"), &Vars.espVehicle.EnableHotKey, NameText7, &IsFind7);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Intellect Item Enable", u8"智慧物资热键"), &Vars.espItem.SmartItemHotKey, NameText8, &IsFind8);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"BattlefieldInfo Key", u8"战局信息热键"), &Vars.Misc.BattlefieldInfoHotKey, NameText11, &IsFind11);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Battlefield Opera Key", u8"战局信息操作热键"), &Vars.Misc.BattlefieldUnlockHotKey, NameText12, &IsFind12);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Global Information HotKey ", u8"全域资讯"), &Vars.Misc.BattlePlayerInfoHotKey, NameText19, &IsFind19);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Active KnockDown HotKey ", u8"强锁倒地热键"), &Vars.Aimbot.InKnockDownHotKey, NameText20, &IsFind20);
			ImGui::SameLine(14, 0, 8);
			add_hotkey(LOCALIZE_Menu(u8"Switch KnockDown HotKey ", u8"开启击倒热键"), &Vars.Aimbot.KnockDownHotKey, NameText17, &IsFind17);

			SaveHotKey.clear();
		}ImGui::EndChild();
		ImGui::PopStyleColor();

	}

	void Menu::RenderEsp()
	{
		ImGui::SameLine(290, 0, 0);
		RenderTabName(LOCALIZE_Menu(u8"Visuals", u8"透视"));
		ImGui::SameLine(94, 0, 36);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16470f, 0.17647f, 0.20784f, 1.f));
		ImGui::BeginChild(("##tabs1"), ImVec2(220, 272), false);
		{
			ImGui::SameLine(8, 0, 8);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"2D Box", u8"2D方框"), &Vars.espHuman.Box);
			ImGui::SameLine(64, 0, 0);
			ImGui::ColorEdit3((u8"未锁定##2DBox"), Vars.espHuman.BoxVis, 0);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3((u8"已锁定##2DBox"), Vars.espHuman.AllLockColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Name", u8"姓名"), &Vars.espHuman.PlayerName);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Name"), Vars.espHuman.PlayerNameColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Health", u8"血条"), &Vars.espHuman.HealthBar);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Health"), Vars.espHuman.HealthBarColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Kills", u8"击杀数"), &Vars.espHuman.KillCount);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Health2"), Vars.espHuman.HealthBarOutLineColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"HandWeapon", u8"手持武器"), &Vars.espHuman.PlayerWeapon);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Weapon"), Vars.espHuman.PlayerWeaponColor, 0);

			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"TeamNum", u8"队标"), &Vars.espHuman.PlayerTeam);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Distance", u8"距离"), &Vars.espHuman.Distance);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Distance"), Vars.espHuman.DistanceColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"HeadCircle", u8"头部圆圈"), &Vars.espHuman.HeadCircle);
			//ImGui::Checkbox2(LOCALIZE_Menu(u8"Player Sight", u8"視角射線"), &Vars.espHuman.PlayerSight);
			//ImGui::SameLine(128, 0, 0);
			//ImGui::ColorEdit3(("Color##Sight"), Vars.espHuman.PlayerSightColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Level", u8"生存等级"), &Vars.espHuman.SuvivalLevel);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Level"), Vars.espHuman.SuvivalLevelColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(u8"KD", &Vars.espRank.KD);
			ImGui::SameLine(148, 0, 0);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Team", u8"队友"), &Vars.espRank.Team);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Ranked", u8"段位"), &Vars.espRank.Rank);
			ImGui::SameLine(148, 0, 0);
			ImGui::Checkbox2(Vars.espRank.RankTexture ? LOCALIZE_Menu(u8"Texture", u8"贴图") : LOCALIZE_Menu(u8"Text", u8"文字"), &Vars.espRank.RankTexture);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Viewers", u8"观战数"), &Vars.espHuman.Observer);
			ImGui::SameLine(148, 0, 0);
			ImGui::Checkbox2(Vars.espRank.Mode ? LOCALIZE_Menu(u8"Solo", u8"单排") : LOCALIZE_Menu(u8"Squad", u8"多排"), &Vars.espRank.Mode);

			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Collaborator", u8"合作者识别"), &Vars.espHuman.Collaborator);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##CollaboratorColor"), Vars.espHuman.CollaboratorColor, 0);

		}ImGui::EndChild();
		ImGui::SameLine(94, 0, 276);
		ImGui::BeginChild(("##tabs2"), ImVec2(220, 174), false);
		{
			ImGui::SameLine(8, 0, 8);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Item", u8"物资"), &Vars.espItem.Item);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Ammo", u8"子弹"), &Vars.espItem.ALL_Ammo);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Grenade", u8"投掷物"), &Vars.espItem.ALL_Grenade);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Weapon", u8"武器"), &Vars.espItem.ALL_Weapon);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Armor", u8"防具"), &Vars.espItem.ALL_Armor);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Medicine", u8"药品"), &Vars.espItem.ALL_Medicine);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Attachment", u8"武器配件"), &Vars.espItem.ALL_Attachment);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"SmartItem", u8"智慧物资"), &Vars.espItem.SmartItem);
		}ImGui::EndChild();
		ImGui::SameLine(320, 0, -276);
		ImGui::BeginChild(("##tabs3"), ImVec2(220, 120), false);
		{
			ImGui::SameLine(8, 0, 8);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Skeleton", u8"骨骼"), &Vars.espHuman.Skeleton);
			ImGui::SameLine(14, 0, 20);
			ImGui::Text(LOCALIZE_Menu(u8"Line Thickness", u8"线条粗细调节"));
			ImGui::SameLine(14, 0, 20);
			ImGui::PushItemWidth(140);
			ImGui::SliderFloat(("##ThickNess"), &Vars.espHuman.Thinkness, 1.f, 5.f, ("%.1f"));
			ImGui::PopItemWidth();
			ImGui::SameLine(14, 0, 24);
			ImGui::Text(LOCALIZE_Menu(u8"Leak out color", u8"掩体前颜色"));
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Leak out"), Vars.espHuman.SkeletonVis, 0);
			ImGui::SameLine(14, 0, 20);
			ImGui::Text(LOCALIZE_Menu(u8"Color behind", u8"掩体后颜色"));
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##behind"), Vars.espHuman.SkeletonInVis, 0);

		}ImGui::EndChild();
		ImGui::SameLine(320, 0, 124);
		ImGui::BeginChild(("##tabs4"), ImVec2(220, 148), false);
		{
			ImGui::SameLine(8, 0, 8);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Spectator", u8"观察者"), &Vars.espHuman.Spectator);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Warning Line", u8"被瞄预警线"), &Vars.Misc.Warning);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Map Radar", u8"大地图雷达"), &Vars.Misc.MRadar);
			ImGui::SameLine(100, 0, -4);
			ImGui::PushItemWidth(100);
			ImGui::SliderFloat("##MRadarRadius", &Vars.Misc.MRadarRadius, 5.f, 20.f, u8"%.0f");
			ImGui::PopItemWidth();
			ImGui::SameLine(8, 0, 24);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"MiniMap", u8"小地图雷达"), &Vars.Misc.Radar);
			ImGui::SameLine(100, 0, -4);
			ImGui::PushItemWidth(100);
			ImGui::SliderFloat("##RadarRadius", &Vars.Misc.RadarRadius, 5.f, 20.f, u8"%.0f");
			ImGui::PopItemWidth();
			ImGui::SameLine(8, 0, 24);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Vehicle Radar", u8"载具雷达"), &Vars.Misc.VehicleMap);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Airdrop Radar", u8"空投雷达"), &Vars.Misc.AirDropsMap);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Name Radar", u8"雷达显示名字"), &Vars.Misc.NameMap);
		}ImGui::EndChild();
		ImGui::SameLine(320, 0, 152);
		ImGui::BeginChild(("##tabs5"), ImVec2(220, 175), false);
		{
			ImGui::SameLine(8, 0, 8);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Loot", u8"战利品"), &Vars.espAirDrop.Loot);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Loot"), Vars.espAirDrop.LootColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"AirDrops", u8"空投箱"), &Vars.espAirDrop.AirDrop);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##AirDrops"), Vars.espAirDrop.AirDropColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Vehicle", u8"载具开关"), &Vars.espVehicle.Vehicle);
			ImGui::SameLine(128, 0, 0);
			ImGui::ColorEdit3(("Color##Vehicle"), Vars.espVehicle.VehicleColor, 0);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Vehicle Health", u8"载具血量"), &Vars.espVehicle.VehicleHealth);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Item(Pictures)", u8"贴图透视"), &Vars.espItem.ItemPictures);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Item(Characters)", u8"文字透视"), &Vars.espItem.ItemText);
			ImGui::SameLine(8, 0, 20);
			ImGui::Checkbox2(LOCALIZE_Menu(u8"Items inside the Loot", u8"战利品内物品"), &Vars.espAirDrop.LootInside);
			ImGui::SameLine(14, 0, 20);
			ImGui::Text(LOCALIZE_Menu(u8"Texture Resize", u8"贴图大小调节"));
			ImGui::SameLine(94, 0, 0);
			ImGui::PushItemWidth(100);
			ImGui::SliderFloat(("##Texture Resize"), &Vars.espItem.ImageSize, 30.f, 60.f, ("%.0f"));
			ImGui::PopItemWidth();
		}ImGui::EndChild();
		ImGui::SameLine(546, 0, -308);
		ImGui::BeginChild(("##tabs6"), ImVec2(220, 304), false);
		{
			ImVec2 NextPos = ImGui::GetNextWindowPos();
			ImGui::GetCurrentWindow()->DrawList->AddImage(HumanMoveable, ImVec2(NextPos.x + 73, NextPos.y + 40), ImVec2(NextPos.x + 153, NextPos.y + 265));
			if (Vars.espHuman.HealthBar)
			{
				ImGui::GetCurrentWindow()->DrawList->AddRect(ImVec2(NextPos.x + 73, NextPos.y + 40), ImVec2(NextPos.x + 153, NextPos.y + 45), 0xFFFFFFFF);
				ImGui::GetCurrentWindow()->DrawList->AddRectFilled(ImVec2(NextPos.x + 73, NextPos.y + 40), ImVec2(NextPos.x + 153, NextPos.y + 45), 0xFFFFFFFF);
			}
			if (Vars.espHuman.PlayerName)
				ImGui::GetCurrentWindow()->DrawList->AddText(ImVec2(NextPos.x + 80, NextPos.y + 22), 0xFFFFFFFF, ("PlayerName"));
			if (Vars.espHuman.PlayerWeapon)
				ImGui::GetCurrentWindow()->DrawList->AddImage(WeaponHold, ImVec2(NextPos.x + 93, NextPos.y + 5), ImVec2(NextPos.x + 135, NextPos.y + 25));
			if (Vars.espHuman.PlayerTeam)
				ImGui::GetCurrentWindow()->DrawList->AddText(ImVec2(NextPos.x + 48, NextPos.y + 26), 0xFF00FFFF, ("26-"));
			if (Vars.espHuman.Observer)
				ImGui::GetCurrentWindow()->DrawList->AddText(ImVec2(NextPos.x + 158, NextPos.y + 26), 0xFFFFFFFF, ("2"));
			ImVec2 DrawPos = ImVec2(NextPos.x + 113, NextPos.y + 266);
			int TextPos = 0;
			std::string FormatText;
			if (Vars.espHuman.KillCount)
				FormatText += LOCALIZE_Menu("Kill:2  ", u8"击杀:2  ");
			if (Vars.espRank.Rank && !Vars.espRank.RankTexture)
				FormatText += LOCALIZE_Menu("Master", u8"大师");
			if (Vars.espRank.KD)
			{
				if (Vars.espRank.Rank)
					FormatText += LOCALIZE_Menu("/0.8", u8"/0.8");
				else
					FormatText += LOCALIZE_Menu("0.8", u8"0.8");
			}

			if (FormatText.size())
			{
				auto font = ImGui::GetCurrentWindow()->DrawList->_Data->Font;
				ImVec2 text_size = font->CalcTextSizeA(font->FontSize, FLT_MAX, -1.0f, FormatText.c_str(), nullptr, NULL);
				text_size.x = (float)(int)(text_size.x + 0.95f);
				DrawPos.x -= text_size.x / 2;
				ImGui::GetCurrentWindow()->DrawList->AddText(DrawPos + ImVec2(0, TextPos), 0xFFFFFFFF, FormatText.c_str());
				TextPos += (int)font->FontSize;
			}
			DrawPos = ImVec2(NextPos.x + 113, NextPos.y + 266);
			FormatText = "";
			if (Vars.espHuman.SuvivalLevel)
				FormatText += LOCALIZE_Menu("1Tier-500", u8"1阶-500");
			if (Vars.espHuman.Distance)
				FormatText += LOCALIZE_Menu("[26M]", u8"[26M]");
			if (FormatText.size())
			{
				auto font = ImGui::GetCurrentWindow()->DrawList->_Data->Font;
				ImVec2 text_size = font->CalcTextSizeA(font->FontSize, FLT_MAX, -1.0f, FormatText.c_str(), nullptr, NULL);
				text_size.x = (float)(int)(text_size.x + 0.95f);
				DrawPos.x -= text_size.x / 2;
				ImGui::GetCurrentWindow()->DrawList->AddText(DrawPos + ImVec2(0, (float)TextPos), 0xFFFFFFFF, FormatText.c_str());
				TextPos += (int)font->FontSize;
			}
		}ImGui::EndChild();
		ImGui::SameLine(546, 0, 308);
		ImGui::BeginChild(("##tabs7"), ImVec2(220, 175), false);
		{
			ImGui::SameLine(14, 0, 8);
			ImGui::Text(LOCALIZE_Menu(u8"Player Distance", u8"玩家透视距离调节"));
			ImGui::SameLine(14, 0, 14);
			ImGui::PushItemWidth(140);
			ImGui::SliderFloat(("##Player Distance"), &Vars.espHuman.DistanceSlider, 100.f, 1000.f, ("%.0f"));
			ImGui::PopItemWidth();
			ImGui::SameLine(14, 0, 18);
			ImGui::Text(LOCALIZE_Menu(u8"Full Info", u8"详细资讯显示距离调节"));
			ImGui::SameLine(14, 0, 14);
			ImGui::PushItemWidth(140);
			ImGui::SliderFloat(("##Full Info"), &Vars.espHuman.SimpleInfoSlider, 100.f, 1000.f, ("%.0f"));
			ImGui::PopItemWidth();
			ImGui::SameLine(14, 0, 18);
			ImGui::Text(LOCALIZE_Menu(u8"Item Distance", u8"物资透视距离调节"));
			ImGui::SameLine(14, 0, 14);
			ImGui::PushItemWidth(140);
			ImGui::SliderFloat(("##ItemDistance"), &Vars.espItem.ItemDistance, 10.f, 100.f, ("%.0f"));
			ImGui::PopItemWidth();
			ImGui::SameLine(14, 0, 18);
			//ImGui::Text(LOCALIZE_Menu(u8"Vehicle Distance", u8"載具透視距離調節"));
			//ImGui::SameLine(14, 0, 14);
			//ImGui::PushItemWidth(140);
			//ImGui::SliderFloat(("##VehicleDistance"), &Vars.espVehicle.VehicleDistance, 100.f, 1000.f, ("%.0f"));
			//ImGui::PopItemWidth();
			ImGui::Text(LOCALIZE_Menu(u8"Item FontSize", u8"物品文字尺寸"));
			ImGui::SameLine(14, 0, 14);
			ImGui::PushItemWidth(140);
			ImGui::SliderFloat(("##Item FontSize"), &Vars.espItem.TextSize, 10.f, 24.f, ("%.0f"));
			ImGui::PopItemWidth();

			ImGui::SameLine(14, 0, 18);
			ImGui::Text(LOCALIZE_Menu(u8"Player FontSize", u8"玩家文字尺寸"));
			ImGui::SameLine(14, 0, 14);
			ImGui::PushItemWidth(140);
			ImGui::SliderFloat(("##PlayerFontSize"), &Vars.espHuman.TextSize, 10.f, 24.f, ("%.0f"));
			ImGui::PopItemWidth();

		}ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void Menu::RenderAnnoce()
	{
		static bool FirstRelease = false;
		if (!Vars.Menu.AnnoceOpen)
		{
			if (!FirstRelease)
			{
				FirstRelease = true;
				OverlayEngine::Transparent();
			}
			return;
		}
		auto& style = ImGui::GetStyle();
		ImVec4 backcolor[48];
		memcpy(backcolor, style.Colors, sizeof(ImVec4) * 48);
		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.215f, 0.215f, 0.254f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.215f, 0.215f, 0.254f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.160f, 0.190f, 0.290f, 1.000f);
		colors[ImGuiCol_Border] = ImVec4(0.215f, 0.215f, 0.215f, 0.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.454f, 0.508f, 0.610f, 1.000f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
		static bool bIsTransparent = false;
		// Main body of the Demo window starts here.
		ImGui::SetNextWindowPos(ImVec2(Drawing::GetViewPort().X / 2.f - 335.f, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(770, 670.f), ImGuiCond_FirstUseEver);
		ImGui::PushFont(OverlayEngine::Font20px);
		if (!ImGui::Begin(LOCALIZE_Menu(u8"PaoD Announcement", u8"PaoD更新公告"), (bool*)&Vars.Menu.AnnoceOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			memcpy(style.Colors, backcolor, sizeof(ImVec4) * 48);
			// Early out if the window is collapsed, as an optimization.
			ImGui::End();
			ImGui::PopFont();
			return;
		}
		ImVec2 NextPos = ImGui::GetNextWindowPos();

		if (Vars.Misc.DisplayMode == 0)
		{
			if (MousePos.x >= NextPos.x && MousePos.y >= NextPos.y && MousePos.x <= NextPos.x + 770.f && MousePos.y <= NextPos.y + 670.f)
			{
				if (!bIsTransparent)
				{
					OverlayEngine::UnTransparent();
					bIsTransparent = true;
				}
			}
			else
			{
				if (bIsTransparent)
				{
					OverlayEngine::Transparent();
					bIsTransparent = false;
				}
			}
		}
		else
		{
			OverlayEngine::UnTransparent();
			bIsTransparent = true;
		}
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
		ImGui::Text(LOCALIZE_Menu(" PaoD announcement will be released in the support", u8" PaoD公告将在辅助中发布"));
		ImGui::Text(LOCALIZE_Menu(" If you have any questions about using PaoD, please send them to the email address", u8" PaoD开放了客户回馈通道，如有任何PaoD使用的问题请回馈至邮箱"));
		ImGui::Text(LOCALIZE_Menu(" quaresmavickey@gmail.com", u8" quaresmavickey@gmail.com"));
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 1, 1));
		ImGui::Text(LOCALIZE_Menu(" Notice：", u8" 注意："));
		ImGui::Text(LOCALIZE_Menu(" When using modeling, be sure to download the offline model!The latest version is 0719", u8" 使用建模漏打务必下载离线模型！最新版本为0719"));
		ImGui::Text(LOCALIZE_Menu(" Not using the offline model will increase the chance of crashes,It is not recommended to enable shared radar for hosts with low configuration", u8" 不使用离线模型将增加闪退几率,配置低的主机不建议开启共享雷达"));
		ImGui::Text(LOCALIZE_Menu(" Shared radar will occupy 10-20% of CPU usage, that is, modeling missed will reduce the corresponding performance", u8" 共享雷达会占用10-20%的CPU使用率,即建模漏打会减少相应性能！"));
		ImGui::Text(LOCALIZE_Menu(" Use the model to quickly transfer position: drive a vehicle/take a plane to transfer position,", u8" 使用模型漏打快速转移位置：驾驶载具/坐飞机转移位置,"));
		ImGui::Text(LOCALIZE_Menu(" You need to wait for 5-10 seconds for the model to refresh after reaching the position (to see if the enemy changes color correctly), otherwise it will be impossible to judge!", u8" 到达位置需要等待模型刷新5-10秒(看敌方是否正确变色),否则无法判断！"));
		ImGui::PopStyleColor();
		ImGui::Text(LOCALIZE_Menu("Local model download address1:<www.123pan.com/s/lLJjjv-G4neh.html> Password:6666", u8"本地模型下载位址1:<www.123pan.com/s/lLJjjv-G4neh.html> 密码:6666"));
		ImGui::Text(LOCALIZE_Menu("Local model download address2:<https://easyupload.io/83dqds> Password:6666", u8"本地模型下载位址2:<https://easyupload.io/83dqds> 密码:6666"));

		if (ImGui::CollapsingHeader(LOCALIZE_Menu("2024/11/20 Notice", u8"2024/11/20 通知", ImGuiTreeNodeFlags_DefaultOpen)))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			ImGui::BulletText(LOCALIZE_Menu(u8"1、随游戏更新", u8"1、随游戏更新"));
			ImGui::PopStyleColor();
			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader(LOCALIZE_Menu("2024/11/15 Notice", u8"2024/11/15 通知")))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			ImGui::BulletText(LOCALIZE_Menu(u8"1、更新全自动步枪扳机", u8"1、更新全自动步枪扳机"));
			ImGui::PopStyleColor();
			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader(LOCALIZE_Menu("2024/11/6 Notice", u8"2024/11/6 通知")))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			ImGui::BulletText(LOCALIZE_Menu(u8"11.6随游戏更新", u8"11.6随游戏更新"));
			ImGui::BulletText(LOCALIZE_Menu(u8"1、全部辅助文本将以中文简体展示", u8"1、全部辅助文本将以中文简体展示"));
			ImGui::BulletText(LOCALIZE_Menu(u8"2、兼容24H2系统", u8"2、兼容24H2系统"));
			ImGui::PopStyleColor();
			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader(LOCALIZE_Menu("2024/10/26 Notice", u8"2024/10/26 通知")))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			ImGui::BulletText(LOCALIZE_Menu(u8"Optimize the prediction and aiming again", u8"再次優化預判自瞄"));
			ImGui::PopStyleColor();
			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader(LOCALIZE_Menu("2024/10/23 Notice", u8"2024/10/23 通知")))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			ImGui::BulletText(LOCALIZE_Menu(u8"2024/10/23 Follow the game update", u8"2024/10/23 跟隨遊戲更新"));
			ImGui::BulletText(LOCALIZE_Menu(u8"Smart Item Updates", u8"智慧物品更新"));
			ImGui::BulletText(LOCALIZE_Menu(u8"1.When equipping an electromagnetic backpack, the normal secondary bag is not displayed.", u8"1.在裝備電磁背包的時候，不顯示普通二級包"));
			ImGui::BulletText(LOCALIZE_Menu(u8"2.When the durability of the third-level armor is higher than 30%, the second-level armor will not be displayed.", u8"2.三級甲耐久高於30%的時候不顯示二級甲"));
			ImGui::BulletText(LOCALIZE_Menu(u8"3.When the durability of the level 3 helmet is higher than 10%, the level 2 helmet will not be displayed.", u8"3.三級頭耐久高於10%的時候不顯示二級頭"));
			ImGui::BulletText(LOCALIZE_Menu(u8"General Updates", u8"通用更新"));
			ImGui::BulletText(LOCALIZE_Menu(u8"1.Added automatic trigger to the Dragon rifle.", u8"1.為德拉貢步槍添加自動扳機"));
			ImGui::BulletText(LOCALIZE_Menu(u8"2.Added customizable lock target color.", u8"2.新增可自訂鎖定目標顏色"));
			ImGui::BulletText(LOCALIZE_Menu(u8"3.Added customizable handheld weapon colors.", u8"3.添加可自訂手持武器顏色"));
			ImGui::BulletText(LOCALIZE_Menu(u8"AimBot Update", u8"自瞄更新"));
			ImGui::BulletText(LOCALIZE_Menu(u8"1.Adjust the anticipatory aiming parameters.", u8"1.調整預判自瞄準參數"));
			ImGui::BulletText(LOCALIZE_Menu(u8"Trigger Update", u8"扳機更新"));
			ImGui::BulletText(LOCALIZE_Menu(u8"1.Added Trigger: Scope Delay and Trigger: Shot Delay in SR auto-aim options.", u8"1.在SR自瞄选项中增加 扳機:開鏡延遲 和 扳機:開槍延遲"));
			ImGui::BulletText(LOCALIZE_Menu(u8"Trigger: scope opening delay -> delay x milliseconds before shooting after opening the scope.", u8"扳機:開鏡延遲->開鏡後延遲x毫秒後開槍"));
			ImGui::BulletText(LOCALIZE_Menu(u8"Trigger: Fire delay -> There should be x milliseconds between the last shot and the next shot.", u8"扳機:開槍延遲->在上一槍與下一槍之間應該間隔x毫秒"));

			ImGui::BulletText(LOCALIZE_Menu(u8"", u8""));

			ImGui::BulletText(LOCALIZE_Menu(u8"Cancel the previous batch of dealers' email addresses and change to the official email address.", u8"撤銷上批經銷商信箱,更換官方信箱"));
			ImGui::BulletText(LOCALIZE_Menu(u8"If you have any questions, you can report to the new email address. : quaresmavickey@gmail.com", u8"任何問題可以回饋新信箱 : quaresmavickey@gmail.com"));
			ImGui::PopStyleColor();
			ImGui::Separator();
		}

		ImGui::Text(LOCALIZE_Menu(" Previous announcements can be found on the official website", u8" 以前的通告可以透过官网查询"));
		ImGui::End();
		ImGui::PopFont();
		memcpy(style.Colors, backcolor, sizeof(ImVec4) * 48);
	}

	void Menu::SetTooltip(std::string Text)
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::PushFont(OverlayEngine::Font18px);
			ImGui::SetTooltip(Text.c_str());
			ImGui::PopFont();
		}
	}

	void Menu::Shutdown()
	{
		exit(-1);
	}
}