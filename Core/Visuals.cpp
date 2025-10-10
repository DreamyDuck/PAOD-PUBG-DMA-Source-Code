#include "Include/WinHeaders.hpp"
#include "Visuals.h"
#include "Config.h"
#include "FilePak.h"
#include "SDK/ObjectManager.h"
#include "Core/DirectX/DirectX11.h"
#include "ItemManager.h"
#include "VehicleManager.h"
#include "HandWeaponManager.h"
#include "SkinWeaponManager.h"
#include "ThirdParty/cpplinq.hpp"
#include <random>
#include "Auth.h"
#include "Image.h"
#include "PhysicsManager/PhysicsManager.h"
#define LOCALIZE_Visual(en,cn) (Vars.Menu.language ? en: cn)
#define ServerVersion 3
#define NEWRANK

int SpectatedCount;
extern bool bMMapRadarIsOpen;
extern std::string hudPath1;
extern std::string hudPath2;
extern IronMan::ServerResult g_ServerResult;
extern bool setInitPID;
extern ImVec2 MousePos;
extern ImVec2 BMenuPos;
extern ImVec2 BMenuSize;
extern int HideFunction;
//更新黑名单
std::unordered_map<hash_t, int>BalltModeStateMap;
std::unordered_map<hash_t, std::shared_ptr<int>>WhiteListMapArray;
WfirstRWLock WhiteListMapArrayLock;
std::string ServerIP;

std::unordered_map<hash_t, std::shared_ptr<int>> GetWhiteListMap()
{
	unique_readguard<WfirstRWLock> rlock(WhiteListMapArrayLock);
	return WhiteListMapArray;
}

static void SetWhiteListMap(std::unordered_map<hash_t, std::shared_ptr<int>>& objects)
{
	unique_writeguard<WfirstRWLock> wlock(WhiteListMapArrayLock);
	WhiteListMapArray = objects;
	objects.clear();
}

static void EraseWhiteListMap(hash_t Name)
{
	unique_writeguard<WfirstRWLock> wlock(WhiteListMapArrayLock);
	auto FindResult = WhiteListMapArray.find(Name);
	if (FindResult != WhiteListMapArray.end())
		WhiteListMapArray.erase(FindResult);
}

static void AddWhiteListMap(hash_t Name, std::shared_ptr<int> Value)
{
	unique_writeguard<WfirstRWLock> wlock(WhiteListMapArrayLock);
	WhiteListMapArray.emplace(Name, Value);
}

bool CheckPlayerIsInWhiteListMap(hash_t Name)
{
	unique_readguard<WfirstRWLock> rlock(WhiteListMapArrayLock);
	return WhiteListMapArray.find(Name) != WhiteListMapArray.end();
}

WfirstRWLock BlackListMapArrayLock;
std::unordered_map<hash_t, std::shared_ptr<int>>BlackListMapArray;

std::unordered_map<hash_t, std::shared_ptr<int>> GetBlackListMap()
{
	unique_readguard<WfirstRWLock> rlock(BlackListMapArrayLock);
	return BlackListMapArray;
}

static void SetBlackListMap(std::unordered_map<hash_t, std::shared_ptr<int>>& objects)
{
	unique_writeguard<WfirstRWLock> wlock(BlackListMapArrayLock);
	BlackListMapArray = objects;
	objects.clear();
}

static void EraseBlackListMap(hash_t Name)
{
	unique_writeguard<WfirstRWLock> wlock(BlackListMapArrayLock);
	auto FindResult = BlackListMapArray.find(Name);
	if (FindResult != BlackListMapArray.end())
		BlackListMapArray.erase(FindResult);
}

static void AddBlackListMap(hash_t Name, std::shared_ptr<int> Value)
{
	unique_writeguard<WfirstRWLock> wlock(BlackListMapArrayLock);
	BlackListMapArray.emplace(Name, Value);
}

bool CheckPlayerIsInBlackListMap(hash_t Name)
{
	unique_readguard<WfirstRWLock> rlock(BlackListMapArrayLock);
	return BlackListMapArray.find(Name) != BlackListMapArray.end();
}

std::unordered_map<std::string, int> WaitUpload;
bool BlackListUpLoad = false;
bool BlackListRemove = false;
std::string BlackListName;

namespace IronMan::Core
{
	HANDLE AimbotThreadHandle = nullptr;
	struct TeamMateStruct
	{
		std::string Name;
		std::string RankTier;
		float KDA;
		bool BlackList;
		bool Partner;
		int Team;
		inline TeamMateStruct(std::string a, std::string b, bool c, bool d, float e, int Team)
		{
			Name = a;
			RankTier = b;
			BlackList = c;
			Partner = d;
			KDA = e;
		}
	};
	std::vector<TeamMateStruct>TeamMateKDList;
	std::vector<std::string> APIKeys;
	extern int RunTimeStart;
	extern int RenderCount;
	void* RayCastBuff;
	PhysicsManager* g_physic = nullptr;
	HealthBar* Visuals::m_HealthBar = nullptr;
	std::unordered_map <ptr_t, std::shared_ptr<std::vector<FVector>>> Visuals::ProjectPath;
	std::unordered_map <hash_t, std::shared_ptr<pRankIcon>> Visuals::RankingIcon;
	std::vector<ScoreBoard>Visuals::m_ScoreBoard;
	std::vector<ScoreBoard>Visuals::m_ScoreBoard2;

	int InFogPlayer = 0;
	//预判点坐标
	FVector2D PredictionPoint = { 0,0 };
	std::string GlobalHost = "";

	ULONGLONG WatchKeyTime = 0;

	std::shared_ptr<std::unordered_map<hash_t, std::vector<FSkinMapData>>> WeapSkinMap;
	std::unordered_map<hash_t, int*>WeapSaveSettings;
	std::vector<FWeaponAttachmentData>_EquipmentData1;
	Inventory_Equipment _InventoryEquipment[3];

	bool bControllerPress = false;
	auto bPressLogTime = std::chrono::high_resolution_clock::now();

	bool RaderType = false;

	ULONGLONG ServerInfoTime = 0;
	int MacCount = 0;
	bool bMacChange = false;
	int NetWorkCount1 = 0;
	int NetWorkCount2 = 0;
	bool bNetWorkError = false;
	int VersionCount = 0;
	bool bVersionChange = false;

	std::string ThisSeason;

	std::unordered_map<hash_t, std::shared_ptr<AllRankModeInfo>> NetAlreadyCache;//已刷新的玩家
	std::unordered_map<hash_t, std::shared_ptr<std::string>> NetUnCache;//未刷新的名称
#ifdef NEWRANK
	WfirstRWLock _AlreadyGetAccountIDLock;
	std::unordered_map<hash_t, std::shared_ptr<std::string>> AlreadyGetAccountID;

	DECLSPEC_NOINLINE static std::unordered_map<hash_t, std::shared_ptr<std::string>> GetAlreadyGetAccountID()
	{
		unique_readguard<WfirstRWLock> rlock(_AlreadyGetAccountIDLock);
		return AlreadyGetAccountID;
	}

	DECLSPEC_NOINLINE static void SetAlreadyGetAccountID(std::unordered_map<hash_t, std::shared_ptr<std::string>>& objects)
	{
		unique_writeguard<WfirstRWLock> wlock(_AlreadyGetAccountIDLock);
		AlreadyGetAccountID = objects;
		objects.clear();
	}

	DECLSPEC_NOINLINE static void AddAlreadyGetAccountID(hash_t Name, std::string AccountID)
	{
		unique_writeguard<WfirstRWLock> wlock(_AlreadyGetAccountIDLock);
		AlreadyGetAccountID.emplace(Name, std::make_shared<std::string>(AccountID));
	}

	static bool IsInAlreadyGetAccountID(hash_t Name)
	{
		unique_readguard<WfirstRWLock> rlock(_AlreadyGetAccountIDLock);
		return AlreadyGetAccountID.find(Name) != AlreadyGetAccountID.end();
	}

	static std::shared_ptr<std::string> GetAlreadyGetAccountID(hash_t Name)
	{
		unique_readguard<WfirstRWLock> rlock(_AlreadyGetAccountIDLock);
		auto FindResult = AlreadyGetAccountID.find(Name);
		if (FindResult != AlreadyGetAccountID.end())
			return FindResult->second;
		return std::make_shared<std::string>("");
	}
#endif
	WfirstRWLock _NetPlayerCacheLock;

	static bool IsInAlreadyCache(hash_t Name)
	{
		unique_readguard<WfirstRWLock> rlock(_NetPlayerCacheLock);
		return NetAlreadyCache.find(Name) != NetAlreadyCache.end();
	}

	DECLSPEC_NOINLINE static std::unordered_map<hash_t, std::shared_ptr<AllRankModeInfo>> GetAlreadyCachePlayer()
	{
		unique_readguard<WfirstRWLock> rlock(_NetPlayerCacheLock);
		return NetAlreadyCache;
	}

	DECLSPEC_NOINLINE static void SetAlreadyCachePlayer(std::unordered_map<hash_t, std::shared_ptr<AllRankModeInfo>>& objects)
	{
		unique_writeguard<WfirstRWLock> wlock(_NetPlayerCacheLock);
		NetAlreadyCache = objects;
		objects.clear();
	}

	WfirstRWLock _NetPlayerNameCacheLock;

	DECLSPEC_NOINLINE static std::unordered_map<hash_t, std::shared_ptr<std::string>> GetUnCachePlayer()
	{
		unique_readguard<WfirstRWLock> rlock(_NetPlayerNameCacheLock);
		return NetUnCache;
	}

	DECLSPEC_NOINLINE static void SetUnCachePlayer(std::unordered_map<hash_t, std::shared_ptr<std::string>>& objects)
	{
		unique_writeguard<WfirstRWLock> wlock(_NetPlayerNameCacheLock);
		NetUnCache = objects;
		objects.clear();
	}

	DECLSPEC_NOINLINE static void EraseUnCachePlayer(hash_t Name)
	{
		unique_writeguard<WfirstRWLock> wlock(_NetPlayerNameCacheLock);
		auto FindResult = NetUnCache.find(Name);
		if (FindResult != NetUnCache.end())
			NetUnCache.erase(FindResult);
	}

	std::shared_ptr<AllRankModeInfo> getNetPlayerInfo(std::string playername)
	{
		auto AlreadyCache = GetAlreadyCachePlayer();
		if (!AlreadyCache.size())
			return std::make_shared<AllRankModeInfo>();
		auto NetInfo = AlreadyCache.find(hash_(playername));
		if (NetInfo != AlreadyCache.end())
			return NetInfo->second;
		return std::make_shared<AllRankModeInfo>();
	}

	bool bHasNetPlayerInfo(std::string playername)
	{
		auto AlreadyCache = GetAlreadyCachePlayer();
		if (!AlreadyCache.size())
			return false;
		auto NetInfo = AlreadyCache.find(hash_(playername));
		return NetInfo != AlreadyCache.end();
	}

	std::unordered_map<int, std::shared_ptr<Item_Container>> ItemContainer;//包含物品

	std::vector<int>AimSelfTeam;
	ptr_t LogTimeToShot = 0;
	ptr_t ARLogTimeToShot = 0;
	ptr_t LogTimeToSkin = 0;

	void* m_Target = nullptr;
	static int ColorOffset = -1;

	std::string GetApiKeyFromServer()
	{
		int loopCount = 0;
	retry:
		auto Result = Utils::SimpleHttpGet("https://note.youdao.com/yws/api/personal/file/WEB36f241d05b1c5ba8feddbca0a76ed28b?method=download&shareKey=8f7db3a9fc4a1c66a0e46879cdbdca74", "");
		if (Result.find("eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9") != std::string::npos) {
			return Result;
		}
		else {
			if (loopCount < 3)
			{
				loopCount++;
				goto retry;
			}
			CONSOLE_INFO2(u8"無法從伺服器取得段位資料,請開啟愛加速後重試!");
			return "";
		}
		return "";
	}

	std::string GetPlayerID(std::string Name)
	{
		if (!APIKeys.size())
			return "null";
		int RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
		std::string APIKey = APIKeys[RandNum];
		httplib::Client cli("https://api.pubg.com");
		httplib::Headers headers = {
			{ "Accept", "application/vnd.api+json" },
			{ "Authorization", "Bearer " + APIKey}
		};
	Retry:
		std::string ServerName = "";
		switch (Vars.espRank.RankServer)
		{
		case 0:
			ServerName = "steam";
			break;
		case 1:
			ServerName = "kakao";
			break;
		case 2:
			ServerName = "psn";
			break;
		case 3:
			ServerName = "stadia";
			break;
		default:
			ServerName = "steam";
			break;
		}
		auto res = cli.Get("/shards/" + ServerName + "/players?filter[playerNames]=" + Name, headers);
		if (res && res->status == 200) {
			return Utils::GetTextBetween(res->body, "\"id\":\"", "\",\"");
		}
		else if (res) {
			if (res->status == 429)
			{
				CONSOLE_INFO(u8"%d too many|%d", res->status, RandNum);
				RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
				APIKey = APIKeys[RandNum];
				goto Retry;
			}
			else if (res->status == 401)
			{
				CONSOLE_INFO(u8"%d Un|%d", res->status, RandNum);
				RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
				APIKey = APIKeys[RandNum];
				goto Retry;
			}
			else if (res->status == 404)
			{
				CONSOLE_INFO(u8"%d not found", res->status);
			}
			else
			{
				CONSOLE_INFO(u8"%d Error", res->status);
			}
		}
		else if (!res)
		{
			Sleep(2000);
			CONSOLE_INFO(u8"key not valid|%d", RandNum);
			RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
			APIKey = APIKeys[RandNum];
			goto Retry;
		}
		return "null";
	}

	std::string GetPlayerIDMuti(std::string Name)
	{
		if (!APIKeys.size())
			return "null";
		int RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
		std::string APIKey = APIKeys[RandNum];
		httplib::Client cli("https://api.pubg.com");
		httplib::Headers headers = {
			{ "Accept", "application/vnd.api+json" },
			{ "Authorization", "Bearer " + APIKey}
		};
	Retry:
		if (!sdk::InGame())
			return "null";
		std::string ServerName = "";
		switch (Vars.espRank.RankServer)
		{
		case 0:
			ServerName = "steam";
			break;
		case 1:
			ServerName = "kakao";
			break;
		case 2:
			ServerName = "psn";
			break;
		case 3:
			ServerName = "stadia";
			break;
		default:
			ServerName = "steam";
			break;
		}
		auto res = cli.Get("/shards/" + ServerName + "/players?filter[playerNames]=" + Name, headers);
		if (res && res->status == 200) {
			return res->body;
		}
		else if (res) {
			if (res->status == 429)
			{
				CONSOLE_INFO(u8"%d too many|%d", res->status, RandNum);
				RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
				APIKey = APIKeys[RandNum];
				goto Retry;
			}
			else if (res->status == 401)
			{
				CONSOLE_INFO(u8"%d Un|%d", res->status, RandNum);
				RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
				APIKey = APIKeys[RandNum];
				goto Retry;
			}
			else if (res->status == 404)
			{
				CONSOLE_INFO(u8"%d not found", res->status);
			}
			else
			{
				CONSOLE_INFO(u8"%d Error", res->status);
			}
		}
		else if (!res)
		{
			auto RequestResult = Utils::SimpleHttpGet("https://api.pubg.com/shards/steam/players?filter[playerNames]=" + Name, "accept: application/vnd.api+json\r\nAuthorization:Bearer " + APIKey);
			if (RequestResult.find("\"data\"") != std::string::npos)
				return RequestResult;
			Sleep(2000);
			CONSOLE_INFO(u8"key not valid|%d", RandNum);
			RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
			APIKey = APIKeys[RandNum];
			goto Retry;
		}
		return "null";
	}

	std::string GetRankStats(std::string SeasonId, std::string AccountId)
	{
		if (!APIKeys.size())
			return "null";
		int RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
		std::string APIKey = APIKeys[RandNum];
		httplib::Client cli("https://api.pubg.com");
		httplib::Headers headers = {
			{ "Accept", "application/vnd.api+json" },
			{ "Authorization", "Bearer " + APIKey}
		};
	Retry:
		if (!sdk::InGame())
			return "null";
		std::string ServerName = "";
		switch (Vars.espRank.RankServer)
		{
		case 0:
			ServerName = "steam";
			break;
		case 1:
			ServerName = "kakao";
			break;
		case 2:
			ServerName = "psn";
			break;
		case 3:
			ServerName = "stadia";
			break;
		default:
			ServerName = "steam";
			break;
		}
		auto res = cli.Get("/shards/" + ServerName + "/players/" + AccountId + "/seasons/" + SeasonId + "/ranked", headers);
		if (res && res->status == 200) {
			return res->body;
		}
		else if (res) {
			if (res->status == 429)
			{
				CONSOLE_INFO(u8"%d too many|%d", res->status, RandNum);
				RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
				APIKey = APIKeys[RandNum];
				goto Retry;
			}
			else if (res->status == 401)
			{
				CONSOLE_INFO(u8"%d Un|%d", res->status, RandNum);
				RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
				APIKey = APIKeys[RandNum];
				goto Retry;
			}
			else if (res->status == 404)
			{
				CONSOLE_INFO(u8"%d not found", res->status);
			}
			else
			{
				CONSOLE_INFO(u8"%d Error", res->status);
			}
		}
		else if (!res)
		{
			auto RequestResult = Utils::SimpleHttpGet("https://api.pubg.com/shards/steam/players/" + AccountId + "/seasons/" + SeasonId + "/ranked", "accept: application/vnd.api+json\r\nAuthorization:Bearer " + APIKey);
			if (RequestResult.find("\"data\"") != std::string::npos)
				return RequestResult;
			Sleep(2000);
			CONSOLE_INFO(u8"key not valid|%d", RandNum);
			RandNum = (int)Utils::RandomRange(0, (float)APIKeys.size());
			APIKey = APIKeys[RandNum];
			goto Retry;
		}
		return "null";
	}

	int GetVisBoneIndex(int ENum)
	{
		switch (ENum)
		{
		case Bone::forehead:
			return 0;
		case Bone::Head:
			return 1;
		case Bone::neck_01:
			return 2;
		case Bone::spine_03:
			return 3;
		case Bone::spine_02:
			return 4;
		case Bone::spine_01:
			return 5;
		case Bone::upperarm_l:
			return 6;
		case Bone::upperarm_r:
			return 7;
		case Bone::lowerarm_l:
			return 8;
		case Bone::lowerarm_r:
			return 9;
		case Bone::hand_l:
			return 10;
		case Bone::hand_r:
			return 11;
		case Bone::thumb_01_l:
			return 12;
		case Bone::thumb_01_r:
			return 13;
		case Bone::thigh_l:
			return 14;
		case Bone::thigh_r:
			return 15;
		case Bone::calf_l:
			return 16;
		case Bone::calf_r:
			return 17;
		case Bone::foot_l:
			return 18;
		case Bone::foot_r:
			return 19;
		case Bone::ik_foot_root:
			return 20;
		case Bone::ik_foot_l:
			return 21;
		case Bone::ik_foot_r:
			return 22;
		default:
			break;
		}
		return 0;
	}

	std::string GetVisBoneName(int ENum)
	{
		switch (ENum)
		{
		case Bone::forehead:
			return ("forehead");
		case Bone::Head:
			return ("Head");
		case Bone::neck_01:
			return ("neck_01");
		case Bone::spine_03:
			return ("spine_03");
		case Bone::spine_02:
			return ("spine_02");
		case Bone::spine_01:
			return ("spine_01");
		case Bone::upperarm_l:
			return ("upperarm_l");
		case Bone::upperarm_r:
			return ("upperarm_r");
		case Bone::lowerarm_l:
			return ("lowerarm_l");
		case Bone::lowerarm_r:
			return ("lowerarm_r");
		case Bone::hand_l:
			return ("hand_l");
		case Bone::hand_r:
			return ("hand_r");
		case Bone::thumb_01_l:
			return ("thumb_01_l");
		case Bone::thumb_01_r:
			return ("thumb_01_r");
		case Bone::thigh_l:
			return ("thigh_l");
		case Bone::thigh_r:
			return ("thigh_r");
		case Bone::calf_l:
			return ("calf_l");
		case Bone::calf_r:
			return ("calf_r");
		case Bone::foot_l:
			return ("foot_l");
		case Bone::foot_r:
			return ("foot_r");
		case Bone::ik_foot_root:
			return ("ik_foot_root");
		case Bone::ik_foot_l:
			return ("ik_foot_l");
		case Bone::ik_foot_r:
			return ("ik_foot_r");
		default:
			break;
		}
		return ("0");
	}

	std::string GetEnRankTier(int Ranked)
	{
		std::string rank_tire;
		if (Ranked < 1500)
		{
			rank_tire = "Bronze";
			if (Ranked < 1100)
				rank_tire += ("5");
			else
			{
				int sub_tire = (Ranked - 1000) / 100;
				rank_tire += std::to_string(sub_tire);
			}
		}
		else if (Ranked >= 1500 && Ranked < 2000)
		{
			rank_tire = "Silver";
			int sub_tire = 5 - (Ranked - 1500) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else if (Ranked >= 2000 && Ranked < 2500)
		{
			rank_tire = "Gold";
			int sub_tire = 5 - (Ranked - 2000) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else if (Ranked >= 2500 && Ranked < 3000)
		{
			rank_tire = "Platinum";
			int sub_tire = 5 - (Ranked - 2500) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else if (Ranked >= 3000 && Ranked < 3500)
		{
			rank_tire = "Diamond";
			int sub_tire = 5 - (Ranked - 3000) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else
			rank_tire = "Master1";
		return rank_tire;
	}

	std::string GetCnRankTier(int Ranked)
	{
		std::string rank_tire;
		if (Ranked < 1500)
		{
			rank_tire = u8"銅牌";
			if (Ranked < 1100)
				rank_tire += ("5");
			else
			{
				int sub_tire = (Ranked - 1000) / 100;
				rank_tire += std::to_string(sub_tire);
			}
		}
		else if (Ranked >= 1500 && Ranked < 2000)
		{
			rank_tire = u8"銀牌";
			int sub_tire = 5 - (Ranked - 1500) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else if (Ranked >= 2000 && Ranked < 2500)
		{
			rank_tire = u8"金牌";
			int sub_tire = 5 - (Ranked - 2000) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else if (Ranked >= 2500 && Ranked < 3000)
		{
			rank_tire = u8"鉑金";
			int sub_tire = 5 - (Ranked - 2500) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else if (Ranked >= 3000 && Ranked < 3500)
		{
			rank_tire = u8"磚石";
			int sub_tire = 5 - (Ranked - 3000) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else
			rank_tire += u8"大師I";
		return rank_tire;
	}

	std::string GetRankTier(int Ranked)
	{
		std::string rank_tire;
		if (Ranked < 1500)
		{
			rank_tire = LOCALIZE_Visual("Bronze", u8"銅牌");
			if (Ranked < 1100)
				rank_tire += ("5");
			else
			{
				int sub_tire = (Ranked - 1000) / 100;
				rank_tire += std::to_string(sub_tire);
			}
		}
		else if (Ranked >= 1500 && Ranked < 2000)
		{
			rank_tire = LOCALIZE_Visual("Silver", u8"銀牌");
			int sub_tire = 5 - (Ranked - 1500) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else if (Ranked >= 2000 && Ranked < 2500)
		{
			rank_tire = LOCALIZE_Visual("Gold", u8"金牌");
			int sub_tire = 5 - (Ranked - 2000) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else if (Ranked >= 2500 && Ranked < 3000)
		{
			rank_tire = LOCALIZE_Visual("Platinum", u8"鉑金");
			int sub_tire = 5 - (Ranked - 2500) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else if (Ranked >= 3000 && Ranked < 3500)
		{
			rank_tire = LOCALIZE_Visual("Diamond", u8"磚石");
			int sub_tire = 5 - (Ranked - 3000) / 100;
			rank_tire += std::to_string(sub_tire);
		}
		else
			rank_tire = LOCALIZE_Visual("Master1", u8"大師I");
		return rank_tire;
	}

	std::string floatToString(float value) {
		std::ostringstream out;
		out << std::fixed << std::setprecision(1) << value;
		return out.str();
	}

	bool SetLastCommunicateTime(std::string username, ptr_t Time)
	{
		int loopCount = 0;
	retry:
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cli(NameHost);
			auto res = cli.Get("/user/" + username + "COMUNICATE" + "/" + std::to_string(Time));
			if (res && res->status == 200) {
				return true;
			}
		}
		catch (...)
		{
		}
		if (loopCount < 3)
		{
			loopCount++;
			goto retry;
		}
		return false;
	}

	ptr_t GetLastCommunicateTime(std::string username)
	{
		int loopCount = 0;
	retry:
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cli(NameHost);
			auto res = cli.Get("/getuser/" + username + "COMUNICATE");
			if (res && res->status == 200) {
				return atoi(res->body.c_str());
			}
		}
		catch (...)
		{
		}
		if (loopCount < 3)
		{
			loopCount++;
			goto retry;
		}
		return false;
	}

	unsigned long long GetServerTimer()
	{
		int loopCount = 0;
	retry:
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cli(NameHost);
			auto res = cli.Get("/AA8B");
			if (res && res->status == 200) {
				return atoi(res->body.c_str());
			}
		}
		catch (...)
		{

		}
		if (loopCount < 3)
		{
			loopCount++;
			goto retry;
		}
		return 0;
	}

	std::string GetSeason()
	{
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cli(NameHost);
			auto res = cli.Get("/getuser/season");
			if (res && res->status == 200) {
				return res->body;
			}
			else {
				return "31";
			}
		}
		catch (...)
		{

		}
		return "31";
	}

	bool UploadMachineCode(std::string username, std::string password, ULONGLONG ServerNotifyTime)
	{
		int loopCount = 0;
	retry:
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cliSNT(NameHost);
			auto resSNT = cliSNT.Get("/getuser/" + username + "SNT");
			if (resSNT && resSNT->status == 200 || resSNT->status == 500) {
				auto res_text = resSNT->status == 200 ? resSNT->body : "0";
				if (ServerNotifyTime > atoi(res_text.c_str()))
				{
					char timer[100];
					_itoa_s((int)ServerNotifyTime, timer, 10);
					auto resSNTSET = cliSNT.Get("/user/" + username + "SNT/" + std::string(timer));
					if (resSNTSET && resSNTSET->status == 200)
					{
						auto resMACSET = cliSNT.Get("/user/" + username + "MAC/" + password);
						if (resMACSET && resSNTSET->status == 200)
						{
							return true;
						}
					}
				}
			}
		}
		catch (...)
		{

		}

		if (loopCount < 3)
		{
			loopCount++;
			goto retry;
		}
		return false;
	}

	bool QueryUserMac(std::string username, std::string password)
	{
		int loopCount = 0;
	retry:
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cli(NameHost);
			auto res = cli.Get("/getuser/" + username + "MAC");
			if (res && res->status == 200) {
				if (res->body == password)
					return false;
			}
		}
		catch (...)
		{
		}
		if (loopCount < 3)
		{
			loopCount++;
			goto retry;
		}
		return true;
	}

	bool ClearBlackList(std::string InName)
	{
		int loopCount = 0;
	retry:
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cli(NameHost);
			auto res = cli.Get("/leave/" + InName);
			if (res && res->status == 200) {
				if (res->body == "ok")
					return true;
			}
		}
		catch (...)
		{
		}
		if (loopCount < 3)
		{
			loopCount++;
			goto retry;
		}
		return false;
	}

	bool UpdateBlackList(std::string InName)
	{
		int loopCount = 0;
	retry:
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cli(NameHost);
			auto res = cli.Get("/last/" + InName);
			if (res && res->status == 200) {
				if (res->body == "ok")
					return true;
			}
		}
		catch (...) {
		}
		if (loopCount < 3)
		{
			loopCount++;
			goto retry;
		}
		return false;
	}

	bool GetBlackList(std::vector<std::string>& Out)
	{
		int loopCount = 0;
	retry:
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cli(NameHost);
			auto res = cli.Get("/list");
			if (res && res->status == 200) {
				Out = Utils::SplitText(res->body, ("name\": \""), ("\", "));
				if (Out.size())
					return true;
				return false;
			}
		}
		catch (...)
		{

		}

		if (loopCount < 3)
		{
			loopCount++;
			goto retry;
		}
		return false;
	}

	int GetServerStatus()
	{
		int loopCount = 0;
	retry:
		try
		{
			std::string NameHost = GlobalHost;
			NameHost.pop_back();
			httplib::Client cli(NameHost);
			auto res = cli.Get("/getuser/serverstatusExtNeo");
			if (res && res->status == 200) {
				return atoi(res->body.c_str());
			}
		}
		catch (...)
		{

		}

		if (loopCount < 3)
		{
			loopCount++;
			goto retry;
		}
		return 0;
	}

	DWORD UpdateServerData()
	{
		int lastestInfo = GetServerStatus();
		if (lastestInfo != ServerVersion && lastestInfo != 0)
		{
			VersionCount++;
			if (VersionCount >= 5)
			{
				bVersionChange = true;
			}
		}
		else
		{
			VersionCount = 0;
		}
		return 1;
	}

	void bUseHost(std::string& hostname)
	{
		hostname = ServerIP + "/";
		return;
	}

	std::string execCommand(const char* cmd) {
		std::array<char, 128> buffer;
		std::string result;
		// 使用 _popen 而不是 popen
		std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
		if (!pipe) {
			throw std::runtime_error("popen() failed!");
		}
		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}
		return result;
	}

	std::string GetSystemRoot() {
		char buffer[MAX_PATH];
		DWORD length = GetEnvironmentVariableA("SystemRoot", buffer, MAX_PATH);
		if (length == 0) {
			CONSOLE_INFO2("Failed to get SystemRoot environment variable.");
			CONSOLE_INFO2("Failed to get SystemRoot environment variable.");
			CONSOLE_INFO2("Failed to get SystemRoot environment variable.");
			CONSOLE_INFO2("Failed to get SystemRoot environment variable.");
			CONSOLE_INFO2("Failed to get SystemRoot environment variable.");
			return "";
		}
		return std::string(buffer, length);
	}

	void AddToSystemPath(const std::string& newPath) {
		// 获取现有的系统Path环境变量
		char* buffer;
		size_t bufferSize;
		_dupenv_s(&buffer, &bufferSize, "Path");

		if (buffer == nullptr) {
			return;
		}

		std::string currentPath(buffer);
		free(buffer);

		// 检查新路径是否已经在Path变量中
		if (currentPath.find(newPath) != std::string::npos) {
			if (!SetEnvironmentVariableA("Path", currentPath.c_str())) {
				{
					CONSOLE_INFO2("Failed to set new Path environment variable.");
					CONSOLE_INFO2("Failed to set new Path environment variable.");
					CONSOLE_INFO2("Failed to set new Path environment variable.");
					CONSOLE_INFO2("Failed to set new Path environment variable.");
					CONSOLE_INFO2("Failed to set new Path environment variable.");

				}
			}
			return;
		}

		// 添加新的路径到现有的Path变量中
		std::string updatedPath = currentPath + ";" + newPath;

		// 设置新的Path环境变量
		if (!SetEnvironmentVariableA("Path", updatedPath.c_str())) {
			CONSOLE_INFO2("Failed to set new Path environment variable.");
			CONSOLE_INFO2("Failed to set new Path environment variable.");
			CONSOLE_INFO2("Failed to set new Path environment variable.");
			CONSOLE_INFO2("Failed to set new Path environment variable.");
			CONSOLE_INFO2("Failed to set new Path environment variable.");

		}
	}

	DWORD QueryNetInfoThread(LPVOID ss)
	{
		VMM();
#ifndef NEWRANK
		httplib::Client clientHttp("https://006.api.cloudbypass.com");
		httplib::Headers headers = {
			{ "x-cb-apikey", "1338ed48f6ef4c16bf9a39bf43940931" },
			{ "x-cb-host", "pubg.op.gg" }
		};
#endif
		static bool InGameState = false;
		int Num = 0;
		while (true)
		{
			try
			{
				//if (GlobalHost == "")
				//{
				//	AddToSystemPath(GetSystemRoot() + "\\System32\\Wbem");
				//	bUseHost(GlobalHost);
				//	CONSOLE_INFO("GlobalHost:%s", GlobalHost.c_str());
				//}
				//if (!ServerInfoTime)
				//{
				//	std::unordered_map<hash_t, std::shared_ptr<int>>TempList;
				//	std::vector<std::string> Out;
				//	GetBlackList(Out);
				//	for (int i = 0; i < Out.size(); i++)
				//	{
				//		TempList.emplace(hash_(Out[i]), std::make_shared<int>(i));
				//	}
				//	Out.clear();
				//	SetBlackListMap(TempList);
				//	UpdateServerData();
				//	std::string SID = execCommand("wmic userAccount where \"Name = '%userName%'\" get SID /value");
				//	SID = Utils::GetTextBetween(SID, "SID=", "\r\n");
				//	if (SID.empty())
				//		SID = g_ServerResult.password;
				//	std::string UUID = execCommand("wmic path Win32_ComputerSystemProduct get uuid /value");
				//	UUID = Utils::GetTextBetween(UUID, "UUID=", "\r\n");
				//	if (UUID.empty())
				//		UUID = g_ServerResult.password;
				//	hash_t CurrentHash = hash_(SID) ^ hash_(UUID) ^ hash_(g_ServerResult.password) ^ Utils::GetMachineCode();
				//	//UploadMachineCode(g_ServerResult.username, std::to_string(CurrentHash), g_ServerResult.beijingTime);
				//	ThisSeason = GetSeason();
				//	ServerInfoTime = GetTickCount64();
				//	auto tempTime = GetServerTimer();
				//	if (tempTime > 0)
				//		Auth::ServerTime = tempTime;
				//	SetLastCommunicateTime(g_ServerResult.username, tempTime);
				//}
				//else if (GetTickCount64() - ServerInfoTime > 60000 && !bVersionChange)
				//{
				//	ptr_t LastTime = 0;
				//	ptr_t tempTime = 0;
				//	for (int i = 0; i < 5; i++)
				//	{
				//		LastTime = GetLastCommunicateTime(g_ServerResult.username);
				//		if (LastTime > 0)
				//			break;
				//	}
				//	for (int i = 0; i < 5; i++)
				//	{
				//		tempTime = GetServerTimer();
				//		if (tempTime > 0)
				//			break;
				//	}
				//	if (LastTime == 0 || tempTime == 0)
				//	{
				//		NetWorkCount1++;
				//		if (NetWorkCount1 >= 5)
				//		{
				//			bNetWorkError = true;
				//		}
				//	}
				//	else
				//	{
				//		NetWorkCount1 = 0;
				//	}

				//	if (tempTime - LastTime >= 50)
				//	{
				//		UpdateServerData();
				//		std::string SID = execCommand("wmic userAccount where \"Name = '%userName%'\" get SID /value");
				//		SID = Utils::GetTextBetween(SID, "SID=", "\r\n");
				//		if (SID.empty())
				//			SID = g_ServerResult.password;
				//		std::string UUID = execCommand("wmic path Win32_ComputerSystemProduct get uuid /value");
				//		UUID = Utils::GetTextBetween(UUID, "UUID=", "\r\n");
				//		if (UUID.empty())
				//			UUID = g_ServerResult.password;
				//		hash_t CurrentHash = hash_(SID) ^ hash_(UUID) ^ hash_(g_ServerResult.password) ^ Utils::GetMachineCode();
				//		auto MacChange = QueryUserMac(g_ServerResult.username, std::to_string(CurrentHash));
				//		if (MacChange == true)
				//		{
				//			MacCount++;
				//			if (MacCount >= 5)
				//			{
				//				bMacChange = true;
				//			}
				//		}
				//		else
				//		{
				//			MacCount = 0;
				//		}
				//		auto tempTime = GetServerTimer();
				//		if (tempTime > 0)
				//			Auth::ServerTime = tempTime;
				//		SetLastCommunicateTime(g_ServerResult.username, tempTime);
				//		ServerInfoTime = GetTickCount64();
				//	}
				//	else
				//	{
				//		NetWorkCount2++;
				//		if (NetWorkCount2 >= 5)
				//		{
				//			bNetWorkError = true;
				//		}
				//		else
				//		{
				//			NetWorkCount2 = 0;
				//		}
				//		break;
				//	}
				//}

				if (BlackListUpLoad && BlackListName != "")
				{
					UpdateBlackList(BlackListName);
					BlackListName = "";
					BlackListUpLoad = false;
				}
				if (BlackListRemove && BlackListName != "")
				{
					ClearBlackList(BlackListName);
					BlackListName = "";
					BlackListRemove = false;
				}

				if (!Vars.espRank.Rank)
				{
					Sleep((DWORD)100);
					continue;
				}
				if (!sdk::InGame())
				{
					if (InGameState)
					{
						InGameState = false;
						std::unordered_map<hash_t, std::shared_ptr<int>>TempList;
						std::vector<std::string> Out;
						GetBlackList(Out);
						auto m_BlackListMap = GetBlackListMap();
						for (int i = 0; i < Out.size(); i++)
						{
							auto FindResult = m_BlackListMap.find(hash_(Out[i]));
							if (FindResult == m_BlackListMap.end())
								TempList.emplace(hash_(Out[i]), std::make_shared<int>(i));
						}
						Out.clear();
						if (TempList.size())
						{
							for (auto iter : TempList)
							{
								AddBlackListMap(iter.first, iter.second);
							}
							TempList.clear();
						}
					}
					auto AlreadyCache = GetAlreadyCachePlayer();
					if (AlreadyCache.size())
					{
						AlreadyCache.clear();
						SetAlreadyCachePlayer(std::unordered_map<hash_t, std::shared_ptr<AllRankModeInfo>>());
					}
					auto UnCachePlayer = GetUnCachePlayer();
					if (UnCachePlayer.size())
					{
						UnCachePlayer.clear();
						SetUnCachePlayer(std::unordered_map<hash_t, std::shared_ptr<std::string>>());
					}
#ifdef NEWRANK
					auto AlreadyAccountID = GetAlreadyGetAccountID();
					if (AlreadyAccountID.size())
					{
						AlreadyAccountID.clear();
						SetAlreadyGetAccountID(std::unordered_map<hash_t, std::shared_ptr<std::string>>());
					}
#endif
					Num = 0;
					Sleep(1000);
					continue;
				}
				else
				{
					InGameState = true;
				}
#ifdef NEWRANK
				std::string QueryString;
				int QueryCount = 0;
				auto cacheNetObject = GetUnCachePlayer();
				for (auto iter = cacheNetObject.begin(); iter != cacheNetObject.end(); ++iter)
				{
					if (!iter->second)
						continue;
					if (!sdk::InGame())
						break;
					auto targetName = iter->second;
					if (!Utils::StringIsValid(*targetName))
					{
						QueryCount++;
						EraseUnCachePlayer(iter->first);
						continue;
					}
					if (IsInAlreadyGetAccountID(hash_(*targetName)))
					{
						QueryCount++;
						continue;
					}
					if (QueryCount < 10)
					{
						QueryString += *targetName + ",";
						QueryCount++;
						if (QueryCount == cacheNetObject.size())
						{
							QueryCount = 0;
							QueryString.pop_back();
							auto ResData = GetPlayerIDMuti(QueryString);
							if (ResData != "null")
							{
								JsonReader AccountJson(ResData.data(), ResData.size());
								AccountJson.StartObject();
								//ar.Member(("version")) & version;

								AccountJson.Member(("data"));
								size_t AccountSize = 0;
								AccountJson.StartArray(&AccountSize);

								for (int i = 0; i < AccountSize; i++)
								{
									AccountJson.StartObject();
									std::string AccountID;
									std::string ActorName;
									if (AccountJson.HasMember("id"))
										AccountJson& AccountID;
									if (AccountJson.HasMember("attributes"))
									{
										AccountJson.StartObject();
										if (AccountJson.HasMember("name"))
											AccountJson& ActorName;
										AddAlreadyGetAccountID(hash_(ActorName), AccountID);
										CONSOLE_INFO("%s|%s", ActorName.c_str(), AccountID.c_str());
										AccountJson.EndObject();
									}
									AccountJson.EndObject();
								}
								AccountJson.EndArray();
								AccountJson.EndObject();
							}
						}
					}
					else
					{
						QueryCount = 0;
						QueryString.pop_back();
						auto ResData = GetPlayerIDMuti(QueryString);
						if (ResData != "null")
						{
							JsonReader AccountJson(ResData.data(), ResData.size());
							AccountJson.StartObject();
							//ar.Member(("version")) & version;

							AccountJson.Member(("data"));
							size_t AccountSize = 0;
							AccountJson.StartArray(&AccountSize);

							for (int i = 0; i < AccountSize; i++)
							{
								AccountJson.StartObject();
								std::string AccountID;
								std::string ActorName;
								if (AccountJson.HasMember("id"))
									AccountJson& AccountID;
								if (AccountJson.HasMember("attributes"))
								{
									AccountJson.StartObject();
									if (AccountJson.HasMember("name"))
										AccountJson& ActorName;
									AddAlreadyGetAccountID(hash_(ActorName), AccountID);
									CONSOLE_INFO("%s|%s", ActorName.c_str(), AccountID.c_str());
									AccountJson.EndObject();
								}
								AccountJson.EndObject();
							}
							AccountJson.EndArray();
							AccountJson.EndObject();
						}
					}
				}
				int currentRankPoint = 0;
				int kills = 0;
				int assists = 0;
				int deaths = 0;
				int roundsPlayed = 0;
				float damageDealt = 0;
				for (auto iter = cacheNetObject.begin(); iter != cacheNetObject.end(); ++iter)
				{
					if (!iter->second)
						continue;
					if (!sdk::InGame())
						break;
					auto targetName = iter->second;
					if (!Utils::StringIsValid(*targetName))
					{
						EraseUnCachePlayer(iter->first);
						continue;
					}

					if (!IsInAlreadyGetAccountID(hash_(*targetName)))
						continue;
					auto targetNameHash = hash_(*targetName);
					AllRankModeInfo ModeInfo = AllRankModeInfo();

					auto AccountId = GetAlreadyGetAccountID(targetNameHash);
					if (!AccountId || AccountId->find("account") == std::string::npos)
						continue;
					std::string RankData = "";
					JsonReader RankDataReader(nullptr);
					if (*AccountId == "null")
						goto DefineOk;
					RankData = GetRankStats("division.bro.official.pc-2018-" + ThisSeason, *AccountId);
					if (RankData == "null")
						goto DefineOk;
					RankDataReader.Build((const char*)RankData.data(), RankData.size());
					size_t Count = 0;
					RankDataReader.StartObject(&Count);

					// 依次进入到目标节点
					if (!RankDataReader.HasMember("data"))
					{
						RankDataReader.EndObject();
						goto DefineOk;
					}
					RankDataReader.StartObject(&Count);
					if (!RankDataReader.HasMember("attributes"))
					{
						RankDataReader.EndObject();
						RankDataReader.EndObject();
						goto DefineOk;
					}
					RankDataReader.StartObject(&Count);
					if (!RankDataReader.HasMember("rankedGameModeStats"))
					{
						RankDataReader.EndObject();
						RankDataReader.EndObject();
						RankDataReader.EndObject();
						goto DefineOk;
					}
					RankDataReader.StartObject(&Count);
					if (RankDataReader.HasMember("squad"))
					{
						RankDataReader.StartObject(&Count);
						// 读取 currentRankPoint

						RankDataReader.Member("currentRankPoint");
						RankDataReader& currentRankPoint;
						RankDataReader.Member("kills");
						RankDataReader& kills;
						RankDataReader.Member("assists");
						RankDataReader& assists;
						RankDataReader.Member("deaths");
						RankDataReader& deaths;
						RankDataReader.Member("roundsPlayed");
						RankDataReader& roundsPlayed;
						RankDataReader.Member("damageDealt");
						RankDataReader& damageDealt;
						RankDataReader.EndObject();

						ModeInfo.TPP.squad.currentRankPoint = currentRankPoint;
						ModeInfo.TPP.squad.kda = (deaths == 0 ? (float)(kills + assists) : (float)(kills + assists) / (float)deaths);
						ModeInfo.TPP.squad.rank_tier = GetRankTier(currentRankPoint);
						ModeInfo.TPP.squad.rank_tier_icon = GetEnRankTier(currentRankPoint);
						ModeInfo.TPP.squad.damage = (int)damageDealt / roundsPlayed;
						CONSOLE_INFO("%s:squad-tpp: currentRankPoint:%d---kda:%.2f---rank_tier:%s\n", targetName->c_str(), ModeInfo.TPP.squad.currentRankPoint, ModeInfo.TPP.squad.kda, ModeInfo.TPP.squad.rank_tier.c_str());
					}
					if (RankDataReader.HasMember("squad-fpp"))
					{
						RankDataReader.StartObject(&Count);
						// 读取 currentRankPoint
						RankDataReader.Member("currentRankPoint");
						RankDataReader& currentRankPoint;
						RankDataReader.Member("kills");
						RankDataReader& kills;
						RankDataReader.Member("assists");
						RankDataReader& assists;
						RankDataReader.Member("deaths");
						RankDataReader& deaths;
						RankDataReader.Member("roundsPlayed");
						RankDataReader& roundsPlayed;
						RankDataReader.Member("damageDealt");
						RankDataReader& damageDealt;
						RankDataReader.EndObject();
						ModeInfo.FPP.squad.currentRankPoint = currentRankPoint;
						ModeInfo.FPP.squad.kda = (deaths == 0 ? (float)(kills + assists) : (float)(kills + assists) / (float)deaths);
						ModeInfo.FPP.squad.rank_tier = GetRankTier(currentRankPoint);
						ModeInfo.FPP.squad.rank_tier_icon = GetEnRankTier(currentRankPoint);
						ModeInfo.FPP.squad.damage = (int)damageDealt / roundsPlayed;
						CONSOLE_INFO("%s:squad-Fpp: currentRankPoint:%d---kda:%.2f---rank_tier:%s\n", targetName->c_str(), ModeInfo.FPP.squad.currentRankPoint, ModeInfo.FPP.squad.kda, ModeInfo.FPP.squad.rank_tier.c_str());
					}
					if (RankDataReader.HasMember("solo"))
					{
						RankDataReader.StartObject(&Count);
						RankDataReader.Member("currentRankPoint");
						RankDataReader& currentRankPoint;
						RankDataReader.Member("kills");
						RankDataReader& kills;
						RankDataReader.Member("assists");
						RankDataReader& assists;
						RankDataReader.Member("deaths");
						RankDataReader& deaths;
						RankDataReader.Member("roundsPlayed");
						RankDataReader& roundsPlayed;
						RankDataReader.Member("damageDealt");
						RankDataReader& damageDealt;
						RankDataReader.EndObject();

						ModeInfo.TPP.solo.currentRankPoint = currentRankPoint;
						ModeInfo.TPP.solo.kda = (deaths == 0 ? (float)(kills + assists) : (float)(kills + assists) / (float)deaths);
						ModeInfo.TPP.solo.rank_tier = GetRankTier(currentRankPoint);
						ModeInfo.TPP.solo.rank_tier_icon = GetEnRankTier(currentRankPoint);
						ModeInfo.TPP.solo.damage = (int)damageDealt / roundsPlayed;
						CONSOLE_INFO("%s:solo-Tpp: currentRankPoint:%d---kda:%.2f---rank_tier:%s\n", targetName->c_str(), ModeInfo.TPP.solo.currentRankPoint, ModeInfo.TPP.solo.kda, ModeInfo.TPP.solo.rank_tier.c_str());
					}
					if (RankDataReader.HasMember("solo-fpp"))
					{
						RankDataReader.StartObject(&Count);
						RankDataReader.Member("currentRankPoint");
						RankDataReader& currentRankPoint;
						RankDataReader.Member("kills");
						RankDataReader& kills;
						RankDataReader.Member("assists");
						RankDataReader& assists;
						RankDataReader.Member("deaths");
						RankDataReader& deaths;
						RankDataReader.Member("roundsPlayed");
						RankDataReader& roundsPlayed;
						RankDataReader.Member("damageDealt");
						RankDataReader& damageDealt;
						RankDataReader.EndObject();

						ModeInfo.FPP.solo.currentRankPoint = currentRankPoint;
						ModeInfo.FPP.solo.kda = (deaths == 0 ? (float)(kills + assists) : (float)(kills + assists) / (float)deaths);
						ModeInfo.FPP.solo.rank_tier = GetRankTier(currentRankPoint);
						ModeInfo.FPP.solo.rank_tier_icon = GetEnRankTier(currentRankPoint);
						ModeInfo.FPP.solo.damage = (int)damageDealt / roundsPlayed;
						CONSOLE_INFO("%s:solo-Fpp: currentRankPoint:%d---kda:%.2f---rank_tier:%s\n", targetName->c_str(), ModeInfo.FPP.solo.currentRankPoint, ModeInfo.FPP.solo.kda, ModeInfo.FPP.solo.rank_tier.c_str());
					}
					RankDataReader.EndObject();
					RankDataReader.EndObject();
					RankDataReader.EndObject();
					RankDataReader.EndObject();

				DefineOk:
					if (sdk::InGame())
					{
						auto UnCachePlayer = GetUnCachePlayer();
						if (UnCachePlayer.size())
						{
							EraseUnCachePlayer(iter->first);
							//CONSOLE_INFO("%s success,erase iter!\n", targetName.c_str());
							auto AlreadyCache = GetAlreadyCachePlayer();
							AlreadyCache.emplace(hash_(targetName->c_str()), std::make_shared<AllRankModeInfo>(ModeInfo));
							SetAlreadyCachePlayer(AlreadyCache);
						}
					}
					else
					{
						//CONSOLE_INFO("%s unsuccessful,Game Ending!\n", targetName.c_str());
					}
					Num++;
				}
#else
				auto cacheNetObject = GetUnCachePlayer();
				for (auto iter = cacheNetObject.begin(); iter != cacheNetObject.end(); ++iter)
				{
					if (!sdk::InGame())
					{
						break;
					}

					auto targetName = iter->second;
					if (!Utils::StringIsValid(targetName))
					{
						EraseUnCachePlayer(iter->first);
						continue;
					}
					auto targetNameHash = hash_(targetName);

					AllRankModeInfo ModeInfo = AllRankModeInfo();

					std::string strbuff = "";
					std::string rescookies = "";
					int UIDCount = 0;
				retryGetUID:
					auto ResHttp = clientHttp.Get("/user/" + targetName, headers);
					if (!ResHttp || ResHttp->status != 200)
					{
						if (UIDCount < 3)
						{
							UIDCount++;
							goto retryGetUID;
						}
						goto DefineOk;
					}
					for (const auto& header : ResHttp->headers) {
						if (header.first == "Set-Cookie") {
							if (!rescookies.empty()) {
								rescookies += "; ";
							}
							rescookies += header.second;
						}
					}
					if (rescookies == "")
						goto DefineOk;
					strbuff = ResHttp->body;
					if (strbuff.find(("not-found__desc")) == std::string::npos)
					{
						std::string UID = Utils::GetTextBetween(strbuff, ("data-user_id=\""), ("\""));
						//*h << ("Content-Type:application/json");
						httplib::Headers params = {
							{ "x-cb-apikey", "1338ed48f6ef4c16bf9a39bf43940931" },
							{ "x-cb-host", "pubg.op.gg" },
							{ "cookie", rescookies },
						};
						std::string json_data = R"({"_method":"PATCH","type":"matches"})";
						auto PostRes = clientHttp.Post("/api/users/" + UID + "/renew", params, json_data, "application/json");
						int RenewCount = 0;
						while (!PostRes || PostRes->status != 200)
						{
							PostRes = clientHttp.Post("/api/users/" + UID + "/renew", params, json_data, "application/json");
							if (RenewCount >= 3)
								goto DefineOk;
							else
								RenewCount++;
							Sleep(500);
						}

						for (auto i = 0; i < 4; i++)
						{
							std::string RankMode;
							std::string RankPeople;
							float deaths = 0;
							float kills = 0;
							int damage_dealt_avg = 0;
							switch (i)
							{
							case 0:
								RankPeople = ("1");
								RankMode = ("fpp");
								break;
							case 1:
								RankPeople = ("4");
								RankMode = ("fpp");
								break;
							case 2:
								RankPeople = ("1");
								RankMode = ("tpp");
								break;
							case 3:
								RankPeople = ("4");
								RankMode = ("tpp");
								break;
							}
							int RankdataCount = 0;
							std::string urlName;
						retryGetRankdata:
							urlName += UID;
							urlName += ("/ranked-stats?season=pc-2018-");
							urlName += ThisSeason;
							urlName += ("&queue_size=");
							urlName += RankPeople;
							urlName += ("&mode=competitive-");
							urlName += RankMode;
							auto res_rankdata = clientHttp.Get("/api/users/" + urlName, headers);
							if (!res_rankdata || (res_rankdata->status != 200 && res_rankdata->status != 404))
							{
								if (RankdataCount < 3)
								{
									RankdataCount++;
									goto retryGetRankdata;
								}
								continue;
							}
							if (BlackListUpLoad && BlackListName != "")
							{
								UpdateBlackList(BlackListName);
								BlackListName = "";
								BlackListUpLoad = false;
							}
							if (BlackListRemove && BlackListName != "")
							{
								ClearBlackList(BlackListName);
								BlackListName = "";
								BlackListRemove = false;
							}
							strbuff = res_rankdata->body;
							if (strbuff == ("{\n    \"message\": \"\"\n}") || strbuff.empty())
							{
								continue;
							}
							switch (i)
							{
							case 0:
								ModeInfo.FPP.solo.currentRankPoint = atoi(Utils::GetTextBetween(strbuff, ("rank_points\":"), (",\"")).c_str());
								deaths = atoi(Utils::GetTextBetween(strbuff, ("deaths_sum\":"), (",\"")).c_str());
								kills = atoi(Utils::GetTextBetween(strbuff, ("kills_sum\":"), (",\"")).c_str());
								damage_dealt_avg = atoi(Utils::GetTextBetween(strbuff, ("damage_dealt_avg\":"), (",\"")).c_str());
								ModeInfo.FPP.solo.kda = (deaths == 0 ? kills : kills / deaths);
								ModeInfo.FPP.solo.rank_tier = GetRankTier(ModeInfo.FPP.solo.currentRankPoint);
								ModeInfo.FPP.solo.rank_tier_icon = GetRankTier2(ModeInfo.FPP.solo.currentRankPoint);
								ModeInfo.FPP.solo.damage = damage_dealt_avg;
								CONSOLE_INFO("%s:solo-fpp: currentRankPoint:%d---kda:%.2f---rank_tier:%s\n", targetName.c_str(), ModeInfo.FPP.solo.currentRankPoint, ModeInfo.FPP.solo.kda, ModeInfo.FPP.solo.rank_tier.c_str());
								break;
							case 1:
								ModeInfo.FPP.squad.currentRankPoint = atoi(Utils::GetTextBetween(strbuff, ("rank_points\":"), (",\"")).c_str());
								deaths = atof(Utils::GetTextBetween(strbuff, ("deaths_sum\":"), (",\"")).c_str());
								kills = atof(Utils::GetTextBetween(strbuff, ("kills_sum\":"), (",\"")).c_str());
								damage_dealt_avg = atoi(Utils::GetTextBetween(strbuff, ("damage_dealt_avg\":"), (",\"")).c_str());
								ModeInfo.FPP.squad.kda = (deaths == 0 ? kills : kills / deaths);
								ModeInfo.FPP.squad.rank_tier = GetRankTier(ModeInfo.FPP.squad.currentRankPoint);
								ModeInfo.FPP.squad.rank_tier_icon = GetRankTier2(ModeInfo.FPP.squad.currentRankPoint);
								ModeInfo.FPP.squad.damage = damage_dealt_avg;
								CONSOLE_INFO("%s:squad-fpp: currentRankPoint:%d---kda:%.2f---rank_tier:%s\n", targetName.c_str(), ModeInfo.FPP.squad.currentRankPoint, ModeInfo.FPP.squad.kda, ModeInfo.FPP.squad.rank_tier.c_str());
								break;
							case 2:
								ModeInfo.TPP.solo.currentRankPoint = atoi(Utils::GetTextBetween(strbuff, ("rank_points\":"), (",\"")).c_str());
								deaths = atof(Utils::GetTextBetween(strbuff, ("deaths_sum\":"), (",\"")).c_str());
								kills = atof(Utils::GetTextBetween(strbuff, ("kills_sum\":"), (",\"")).c_str());
								damage_dealt_avg = atoi(Utils::GetTextBetween(strbuff, ("damage_dealt_avg\":"), (",\"")).c_str());
								ModeInfo.TPP.solo.kda = (deaths == 0 ? kills : kills / deaths);
								ModeInfo.TPP.solo.rank_tier = GetRankTier(ModeInfo.TPP.solo.currentRankPoint);
								ModeInfo.TPP.solo.rank_tier_icon = GetRankTier2(ModeInfo.TPP.solo.currentRankPoint);
								ModeInfo.TPP.solo.damage = damage_dealt_avg;
								CONSOLE_INFO("%s:solo-tpp: currentRankPoint:%d---kda:%.2f---rank_tier:%s\n", targetName.c_str(), ModeInfo.TPP.solo.currentRankPoint, ModeInfo.TPP.solo.kda, ModeInfo.TPP.solo.rank_tier.c_str());
								break;
							case 3:
								ModeInfo.TPP.squad.currentRankPoint = atoi(Utils::GetTextBetween(strbuff, ("rank_points\":"), (",\"")).c_str());
								deaths = atof(Utils::GetTextBetween(strbuff, ("deaths_sum\":"), (",\"")).c_str());
								kills = atof(Utils::GetTextBetween(strbuff, ("kills_sum\":"), (",\"")).c_str());
								damage_dealt_avg = atoi(Utils::GetTextBetween(strbuff, ("damage_dealt_avg\":"), (",\"")).c_str());
								ModeInfo.TPP.squad.kda = (deaths == 0 ? kills : kills / deaths);
								ModeInfo.TPP.squad.rank_tier = GetRankTier(ModeInfo.TPP.squad.currentRankPoint);
								ModeInfo.TPP.squad.rank_tier_icon = GetRankTier2(ModeInfo.TPP.squad.currentRankPoint);
								ModeInfo.TPP.squad.damage = damage_dealt_avg;
								CONSOLE_INFO("%s:squad-tpp: currentRankPoint:%d---kda:%.2f---rank_tier:%s\n", targetName.c_str(), ModeInfo.TPP.squad.currentRankPoint, ModeInfo.TPP.squad.kda, ModeInfo.TPP.squad.rank_tier.c_str());
								break;
							}
						}
					}
				DefineOk:
					if (sdk::InGame())
					{
						auto UnCachePlayer = GetUnCachePlayer();
						if (UnCachePlayer.size())
						{
							//CONSOLE_INFO("%s success,erase iter!\n", targetName.c_str());
							UnCachePlayer.erase(UnCachePlayer.find(iter->first));
							SetUnCachePlayer(UnCachePlayer);
							auto AlreadyCache = GetAlreadyCachePlayer();
							AlreadyCache.emplace(hash_(targetName.c_str()), ModeInfo);
							SetAlreadyCachePlayer(AlreadyCache);
						}
					}
					else
					{
						//CONSOLE_INFO("%s unsuccessful,Game Ending!\n", targetName.c_str());
					}
					Num++;
				}
#endif
				Sleep((DWORD)1000);
			}
			catch (...)
			{
				CONSOLE_INFO("Thread Expection!\n", );
			}
		}
		VMEND();
		return 0;
	}

	namespace Aimbot
	{
		float Aim_DeltaTime = 1.0f / 80.0f;
		static float finalTime = 0.0f;
		static float curAimTime = 0.0f;
		int LastHeadKey = 0;
		float GrenadeZAdd = 0.f;
		PIDController PidX(Vars.Aimbot.AR_P_X1, Vars.Aimbot.I_X, Vars.Aimbot.D_X);
		PIDController PidY(Vars.Aimbot.AR_P_Y1, Vars.Aimbot.I_Y, Vars.Aimbot.D_Y);

		float ScreenDistantTo(FVector2D Screen, FVector2D EnemyScreen)
		{
			float benrenX = Screen.X - EnemyScreen.X;
			float benrenY = Screen.Y - EnemyScreen.Y;
			return sqrt(benrenX * benrenX + benrenY * benrenY);
		}

		float GetNextPos(ATslCharacter& player, float dist, FVector PlayerVelocity)
		{
			float offset = 0.04f;




			/*Drawing::DrawTextOutline(OverlayEngine::Font18px, 18, 0, ImVec2(100, 100), 0xFFFFFFFF, 0xFF000000, FONT_LEFT,
				"%f\n%f|%f\n%f|%f\n%f", offset, predictionPos.X, predictionPos.Y, (GetPlayerVelocity(target)* offset).X, (GetPlayerVelocity(target)* offset).Y, Pix);
			if (OverlayEngine::WasKeyPressed(VK_F6))
				Pix -= 0.0002f;
			if (OverlayEngine::WasKeyPressed(VK_F7))
				Pix += 0.0002f;*/

			if (sdk::GetIsSR() || sdk::GetIsDMR())
			{
				if (dist <= 20.f)
					offset = 0.015f;
				else if (dist <= 30.f)
					offset = 0.011f;
				else if (dist <= 50.f)
					offset = 0.013f;
				else if (dist <= 70.f)
					offset = 0.0116f;
				else if (dist <= 90.f)
					offset = 0.012f;
				else if (dist <= 120.f)
					offset = 0.0124f - 0.001f;
				else if (dist <= 150.f)
					offset = 0.0132f - 0.001f;
				else if (dist <= 180.f)
					offset = 0.0144f - 0.001f;
				else if (dist <= 210.f)
					offset = 0.015f - 0.001f;
				else if (dist <= 250.f)
					offset = 0.016f - 0.001f;
				else if (dist <= 280.f)
					offset = 0.0166f - 0.001f;
				else if (dist <= 310.f)
					offset = 0.0172f - 0.001f;
				else if (dist <= 350.f)
					offset = 0.018f - 0.001f;
				else if (dist <= 380.f)
					offset = 0.0185f - 0.001f;
				else if (dist <= 410.f)
					offset = 0.0186f - 0.001f;
				else if (dist <= 450.f)
					offset = 0.019f - 0.001f;
				else if (dist <= 480.f)
					offset = 0.0196f - 0.001f;
				else if (dist <= 510.f)
					offset = 0.02f - 0.001f;
				else if (dist <= 550.f)
					offset = 0.0206f - 0.001f;
				else if (dist <= 600.f)
					offset = 0.0218f - 0.001f;
				else if (dist <= 640.f)
					offset = 0.0224f - 0.001f;
				else if (dist <= 670.f)
					offset = 0.0232f - 0.001f;
				else if (dist <= 700.f)
					offset = 0.024f - 0.001f;
				static float Pix = 0.01f;

				auto VelocityXY = Math::Abs(PlayerVelocity.X) + Math::Abs(PlayerVelocity.Y);
				if (VelocityXY > 1200.f)
					offset += offset * ((VelocityXY - 1200.f) / VelocityXY) * Pix;
			}
			else
			{
				if (dist <= 20.f)
					offset = 0.008f;
				else if (dist <= 30.f)
					offset = 0.01f;
				else if (dist <= 50.f)
					offset = 0.011f;
				else if (dist <= 70.f)
					offset = 0.012f;
				else if (dist <= 90.f)
					offset = 0.0123f;
				else if (dist <= 120.f)
					offset = 0.0125f;
				else if (dist <= 150.f)
					offset = 0.0128f;
				else if (dist <= 180.f)
					offset = 0.0132f;
				else if (dist <= 210.f)
					offset = 0.0135f;
				else if (dist <= 250.f)
					offset = 0.014f;
				else if (dist <= 280.f)
					offset = 0.0143f;
				else if (dist <= 310.f)
					offset = 0.0146f;
				else if (dist <= 350.f)
					offset = 0.015f;
				else if (dist <= 380.f)
					offset = 0.0155f;
				else if (dist <= 410.f)
					offset = 0.016f;
				else if (dist <= 450.f)
					offset = 0.0165f;
				else if (dist <= 480.f)
					offset = 0.017f;
				else if (dist <= 510.f)
					offset = 0.0175f;
				else if (dist <= 550.f)
					offset = 0.018f;
				else if (dist <= 600.f)
					offset = 0.0185f;
				else if (dist <= 640.f)
					offset = 0.019f;
				else if (dist <= 670.f)
					offset = 0.0195f;
				else if (dist <= 700.f)
					offset = 0.02f;
				else if (dist <= 800.f)
					offset = 0.021f;
				else if (dist <= 900.f)
					offset = 0.022f;
				else
					offset = 0.023f;
				static float Pix = 1.25f;
				if (player->_LastVehiclePawn)
				{
					auto VelocityXY = Math::Abs(PlayerVelocity.X) + Math::Abs(PlayerVelocity.Y);
					if (VelocityXY > 1200.f)
						offset += offset * ((VelocityXY - 1200.f) / VelocityXY) * Pix;
				}
			}


			return offset;
		}

		bool isDMR()
		{
			auto weaponData = hash_(sdk::GetWeaponName());
			switch (weaponData)
			{
			case "WeapVSS_C"_hash:
			case "WeapQBU88_C"_hash:
			case "WeapMadsQBU88_C"_hash:
			case "WeapMini14_C"_hash:
			case "WeapSKS_C"_hash:
			case "WeapFNFal_C"_hash:
			case "WeapMk47Mutant_C"_hash:
			case "WeapDragunov_C"_hash:
			case "WeapMk12_C"_hash:
			case "WeapMk14_C"_hash:
				return true;
			default:
				break;
			}
			return false;
		}

		bool isSR()
		{
			auto weaponData = hash_(sdk::GetWeaponName());
			switch (weaponData)
			{
			case "WeapMosinNagant_C"_hash:
			case "WeapM24_C"_hash:
			case "WeapAWM_C"_hash:
			case "WeapKar98k_C"_hash:
			case "WeapJuliesKar98k_C"_hash:
			case "WeapJuliesM24_C"_hash:
			case "WeapL6_C"_hash:
			case "WeapCrossbow_1_C"_hash:
			case "WeapWin1894_C"_hash:
			case "WeapWin94_C"_hash:
			case "WeapDragunov_C"_hash:
				return true;
			default:
				break;
			}
			return false;
		}

		bool isShotGun()
		{
			auto weaponData = hash_(sdk::GetWeaponName());
			switch (weaponData)
			{
			case "WeapBerreta686_C"_hash:
			case "WeapSaiga12_C"_hash:
			case "WeapWinchester_C"_hash:
			case "WeapDP12_C"_hash:
			case "WeapSawnoff_C"_hash:
			case "WeapOriginS12_C"_hash:
				return true;
			default:
				break;
			}
			return false;
		}

		bool isDBS()
		{
			auto weaponData = hash_(sdk::GetWeaponName());
			switch (weaponData)
			{
			case "WeapDP12_C"_hash:
				return true;
			default:
				break;
			}
			return false;
		}

		bool isGrenade()
		{
			auto weaponData = hash_(sdk::GetWeaponName());
			switch (weaponData)
			{
			case "WeapFlashBang_C"_hash:
			case "WeapGrenade_C"_hash:
			case "WeapSmokeBomb_C"_hash:
			case "WeapStickyGrenade_C"_hash:
			case "WeapBluezoneGrenade_C"_hash:
			case "WeapDecoyGrenade_C"_hash:
			case "WeapMolotov_C"_hash:
			case "WeapC4_C"_hash:
			case "WeapRock_C"_hash:
			case "WeapFlareGun_C"_hash:
			case "WeapApple_C"_hash:
			case "WeapSnowball_C"_hash:
			case "WeapCupcake_C"_hash:
				return true;
			default:
				break;
			}
			return false;
		}

		FVector GetPlayerVelocity(ATslCharacter& p)
		{
			if (p->IsValid())
			{
				if (IsAddrValid(p->_VehicleRiderComponent) && p->_SeatIndex >= 0)
					if (IsAddrValid(p->_LastVehiclePawn))
						return p->_LinearVelocity;

				UCharacterMovementComponent CharacterMovement = p->_CharacterMovement;

				if (CharacterMovement->IsValid())
				{
					const FVector InVelocity = p->_Velocity;
					const FVector Delta = FVector(InVelocity.X, InVelocity.Y, 0.f) * Aim_DeltaTime;
					auto CurrentFloor = p->_CurrentFloor;
					if (CurrentFloor.IsWalkableFloor())
					{
						FVector RampVector = CharacterMovement->ComputeGroundMovementDelta(Delta, CurrentFloor.HitResult,
							CurrentFloor.bLineTrace, p->_bMaintainHorizontalGroundVelocity, p->_WalkableFloorZ, p->_CurrentFloor_HitResult_Component, p->_CurrentFloor_HitResult_Component_BodyInstance_WalkableSlopeOverride) / Aim_DeltaTime;
						return RampVector + FVector(0.f, 0.f, InVelocity.Z);
					}
					return InVelocity;
				}
			}

			return { 0.f, 0.f, 0.f };
		}

		FZeroingInfo GetWeaponZeroInfo(hash_t HashName)
		{
			static FZeroingInfo ZeroingSG = FZeroingInfo(50.0f, 100.0f, 0.0f, 100.0f);
			static FZeroingInfo ZeroingCBow = FZeroingInfo(25.0f, 25.0f, 0.0f, 25.0f);
			static FZeroingInfo ZeroingVSS = FZeroingInfo(100.0f, 100.0f, 25.0f, 100.0f);
			static FZeroingInfo ZeroingPistol = FZeroingInfo(50.0f, 50.0f, 0.0f, 100.0f);
			static FZeroingInfo ZeroingDeagle = FZeroingInfo(50.0f, 100.0f, 0.0f, 100.0f);
			static FZeroingInfo ZeroingSkorpion = FZeroingInfo(100.0f, 50.0f, 0.0, 100.0f);
			static FZeroingInfo DefaultZeroing = FZeroingInfo();
			switch (HashName)
			{
			case "WeapCrossbow_1_C"_hash:
				return ZeroingCBow;
			case "WeapBerreta686_C"_hash:
			case "WeapSaiga12_C"_hash:
			case "WeapWinchester_C"_hash:
			case "WeapDP12_C"_hash:
			case "WeapSawnoff_C"_hash:
				return ZeroingSG;
			case "WeapG18_C"_hash:
			case "WeapM1911_C"_hash:
			case "WeapM9_C"_hash:
			case "WeapNagantM1895_C"_hash:
			case "WeapRhino_C"_hash:
				return ZeroingPistol;
			case "WeapDesertEagle_C"_hash:
				return ZeroingDeagle;
			case "Weapvz61Skorpion_C"_hash:
				return ZeroingSkorpion;
			case "WeapVSS_C"_hash:
				return ZeroingVSS;
			default:
				return DefaultZeroing;
			}
		}

		ESight GetSight(ptr_t TslWeapon)
		{
			if (!IsAddrValid(TslWeapon))
				return ESight::Iron;
			//auto AttachedItem = TslWeapon->AttachedItem;
			//for (int i = 0; i < AttachedItem.Num(); i++)
			//{
			//	auto AttachableItem = UAttachableItem(AttachedItem[i]);
			//	if (!AttachableItem.IsValid())
			//		continue;
			//	switch (hash_(AttachableItem.GetItemID().GetName()))
			//	{
			//	case "Item_Attach_Weapon_Upper_PM2_01_C"_hash:
			//		return ESight::X15;
			//	case "Item_Attach_Weapon_Upper_CQBSS_C"_hash:
			//		return ESight::X8;
			//	case "Item_Attach_Weapon_Upper_Scope6x_C"_hash:
			//		return ESight::X6;
			//	case "Item_Attach_Weapon_Upper_ACOG_01_C"_hash:
			//		return ESight::X4;
			//	case "Item_Attach_Weapon_Upper_Scope3x_C"_hash:
			//		return ESight::X3;
			//	case "Item_Attach_Weapon_Upper_Aimpoint_C"_hash:
			//		return ESight::X2;
			//	case "Item_Attach_Weapon_Upper_DotSight_01_C"_hash:
			//		return ESight::RedDot;
			//	case "Item_Attach_Weapon_Upper_Holosight_C"_hash:
			//		return ESight::Holo;
			//	default:
			//		break;
			//	}
			//}
			auto AttachmentData = sdk::GetWeponAttachmentData();
			for (int i = 0; i < AttachmentData.size(); i++)
			{
				FName ItemName = FName(AttachmentData[i].itemID);
				std::unique_ptr<std::string> IName = std::make_unique<std::string>(ItemName.GetName2());
				switch (hash_(*IName))
				{
				case "Item_Attach_Weapon_Upper_PM2_01_C"_hash:
					return ESight::X15;
				case "Item_Attach_Weapon_Upper_CQBSS_C"_hash:
					return ESight::X8;
				case "Item_Attach_Weapon_Upper_Scope6x_C"_hash:
					return ESight::X6;
				case "Item_Attach_Weapon_Upper_ACOG_01_C"_hash:
					return ESight::X4;
				case "Item_Attach_Weapon_Upper_Scope3x_C"_hash:
					return ESight::X3;
				case "Item_Attach_Weapon_Upper_Aimpoint_C"_hash:
					return ESight::X2;
				case "Item_Attach_Weapon_Upper_DotSight_01_C"_hash:
					return ESight::RedDot;
				case "Item_Attach_Weapon_Upper_Holosight_C"_hash:
					return ESight::Holo;
				default:
					break;
				}
			}
			return ESight::Iron;
		}

		void SimulateWeaponTrajectory(FWeaponTrajectoryConfig config, FVector Direction, float Distance, float& BulletDrop, float& TravelTime)
		{
			float TravelDistance = 0.0f;
			float CurrentDrop = 0.0f;
			BulletDrop = 0.0f;
			TravelTime = 0.0f;

			Direction.Normalize();
			Direction = Direction * 100.0f;

			auto getBulletVelocity = [=](auto distance, auto BallisticDragScale, FWeaponTrajectoryConfig config, FRichCurve_3 FloatCurve, std::vector<std::vector<FRichCurveKey>> Keys)
				{
					float bulletVelocity = config.InitialSpeed;
					UCurveVector ballisticCurve(config.BallisticCurve);
					if (ballisticCurve->IsValid())
					{
						bulletVelocity = ballisticCurve->GetVectorValue(distance * config.BDS * BallisticDragScale, FloatCurve, Keys).X;
					}
					return bulletVelocity;
				};

			auto BallisticDragScale = sdk::GetBallisticDragScale();
			auto BallisticDropScale = sdk::GetBallisticDropScale();
			auto weapon_TrajectoryGravityZ = sdk::GetTrajectoryGravityZ();
			auto FloatCurve = sdk::GetFloatCurves();
			auto FloatCurveKeys = sdk::GetCurvesKeysArray();
			auto TrajectoryGravityZ = sdk::GetTrajectoryGravityZ();
			while (1)
			{
				float BulletSpeed = getBulletVelocity(TravelDistance, BallisticDragScale, config, FloatCurve, FloatCurveKeys);

				FVector Velocity = Direction * BulletSpeed;
				Velocity.Z += CurrentDrop;

				FVector Acceleration = Velocity * config.SimulationSubstepTime;
				float AccelerationLen = Acceleration.Size() / 100.0f;
				if (TravelDistance + AccelerationLen > Distance)
				{
					float RemainDistance = Distance - TravelDistance;
					float AccelerationSpeed = AccelerationLen / config.SimulationSubstepTime;
					float RemainTime = RemainDistance / AccelerationSpeed;

					TravelTime += RemainTime;
					BulletDrop += RemainTime * CurrentDrop;
					break;
				}
				TravelDistance += AccelerationLen;
				TravelTime += config.SimulationSubstepTime;
				BulletDrop += config.SimulationSubstepTime * CurrentDrop;
				CurrentDrop += config.SimulationSubstepTime * TrajectoryGravityZ * 100.f * config.VDragCoefficient * BallisticDropScale;
			}
		}

		std::pair<float, float> GetBulletDropAndTravelTime(FWeaponTrajectoryConfig config,
			const FVector& GunLocation, const FRotator& GunRotation, const FVector& TargetPos,
			float ZeroingDistance, float BulletDropAdd)
		{
			auto TrajectoryGravityZ = sdk::GetTrajectoryGravityZ();
			const float ZDistanceToTarget = TargetPos.Z - GunLocation.Z;
			const float DistanceToTarget = (GunLocation - TargetPos).Size() / 100.0f;
			float TravelTime = DistanceToTarget / config.InitialSpeed;
			float BulletDrop = 0.5f * TrajectoryGravityZ * TravelTime * TravelTime * 100.0f;

			float TravelTimeZero = ZeroingDistance / config.InitialSpeed;
			float BulletDropZero = 0.5f * TrajectoryGravityZ * TravelTimeZero * TravelTimeZero * 100.0f;
			//CONSOLE_INFO("ZDistanceToTarget:%f\nDistanceToTarget:%f\nTravelTime:%f\nBulletDrop:%f\nTravelTimeZero:%f\nBulletDropZero:%f\nBulletDropAdd:%f",
				//ZDistanceToTarget, DistanceToTarget, TravelTime, BulletDrop,
				//TravelTimeZero, BulletDropZero, BulletDropAdd);
			SimulateWeaponTrajectory(config, GunRotation.Vector(), DistanceToTarget, BulletDrop, TravelTime);

			SimulateWeaponTrajectory(config, FVector(1.0f, 0.0f, 0.0f), ZeroingDistance, TravelTimeZero, BulletDropZero);

			BulletDrop = fabsf(BulletDrop) - fabsf(BulletDropAdd);
			if (BulletDrop < 0.0f)
				BulletDrop = 0.0f;
			BulletDropZero = fabsf(BulletDropZero) + fabsf(BulletDropAdd);

			const float TargetPitch = asinf((ZDistanceToTarget + BulletDrop) / 100.0f / DistanceToTarget);
			const float ZeroPitch = IsNearlyZero(ZeroingDistance) ? 0.0f : atan2f(BulletDropZero / 100.0f, ZeroingDistance);
			const float FinalPitch = TargetPitch - ZeroPitch;
			const float AdditiveZ = DistanceToTarget * sinf(FinalPitch) * 100.0f - ZDistanceToTarget;
			//CONSOLE_INFO("3-BulletDrop:%f|TravelTime:%f", AdditiveZ, TravelTime);

			return std::pair(AdditiveZ, TravelTime);
		}

		float GetZeroingDistance(ptr_t TslWeapon, bool IsScoping)
		{
			auto WeaponHashName = hash_(sdk::GetWeaponName());
			auto bCantedSighted = sdk::GetbIsCanted();
			auto CurrentZeroLevel = sdk::GetCurrentZeroLevel();
			if (bCantedSighted)
				return WeaponHashName == "WeapP90_C"_hash ? 100.0f : 50.0f;
			auto ZeroingInfo = GetWeaponZeroInfo(WeaponHashName);
			int ZeroLevel = IsScoping ? CurrentZeroLevel : 0;
			auto ScopeSight = GetSight(TslWeapon);

			switch (ScopeSight)
			{
			case ESight::X15:
			case ESight::X8:
				return ZeroingInfo.BaseScope + ZeroingInfo.Increment * ZeroLevel;
			case ESight::X6:
			case ESight::X4:
			case ESight::X3:
			case ESight::X2:
			case ESight::RedDot:
				return ZeroingInfo.BaseScope;
			case ESight::Holo:
				return ZeroingInfo.BaseHolo;
			}
			return ZeroingInfo.BaseIron + ZeroingInfo.Increment * ZeroLevel;
		}

		FVector GetPrediction(ATslCharacter& target, FVector& aimPos, float& dropPitchOffset, FVector Velocity)
		{
			auto& player = sdk::GetPlayer();
			if (!IsAddrValid(player))
			{
				return aimPos;
			}
			const float RAD_TO_DEG = (180.f) / PI;
			auto getBulletVelocity = [=](auto distance, auto BallisticDragScale, FWeaponTrajectoryConfig config, FRichCurve_3 FloatCurve, std::vector<std::vector<FRichCurveKey>> Keys)
				{
					float bulletVelocity = config.InitialSpeed * 100.0f;
					UCurveVector ballisticCurve(config.BallisticCurve);
					if (ballisticCurve->IsValid())
					{
						bulletVelocity = ballisticCurve->GetVectorValue(distance * 0.01f * BallisticDragScale * config.BDS, FloatCurve, Keys).X * 100.0f;
					}
					return bulletVelocity;
				};

			auto& weapon = sdk::GetWeapon();
			if (IsAddrValid(weapon))
			{
				auto config = sdk::GetWeaponTrajectoryConfig();
				if (config.InitialSpeed == 0.0f || config.VDragCoefficient == 0.0f)
					return aimPos;
				dropPitchOffset = 0.0f;
				auto muzzleOrigin = sdk::GetFiringLocation();
				float distanceMax = (aimPos - muzzleOrigin).Size();
				float zeroingDistance = GetZeroingDistance(weapon, sdk::GetIsScoping_CP()) * 100.f;
				float dropOffset = 0.0f;
				float drop = 0.0f;
				float flyTime = distanceMax / config.InitialSpeed / 100.f;
				if (config.bUseAdvancedBallistics)
				{
					float ballisticSubstepDropVelocity = 0.0f;
					float travelDistance = 0.0f;
					flyTime = 0.0f;
					auto BallisticDragScale = sdk::GetBallisticDragScale();
					auto BallisticDropScale = sdk::GetBallisticDropScale();
					auto weapon_TrajectoryGravityZ = sdk::GetTrajectoryGravityZ();
					auto FloatCurve = sdk::GetFloatCurves();
					auto FloatCurveKeys = sdk::GetCurvesKeysArray();
					for (int i = 0; i < 500; i++)
					{
						float v = getBulletVelocity(travelDistance, BallisticDragScale, config, FloatCurve, FloatCurveKeys);
						drop += ballisticSubstepDropVelocity * config.SimulationSubstepTime;
						travelDistance += config.SimulationSubstepTime * v;
						flyTime += config.SimulationSubstepTime;
						if (travelDistance >= zeroingDistance && dropOffset == 0.0f)
							dropOffset = drop;
						if (travelDistance >= distanceMax)
						{
							flyTime -= (travelDistance - distanceMax) / v;
							drop -= ((travelDistance - distanceMax) / v) * ballisticSubstepDropVelocity;
							break;
						}
						ballisticSubstepDropVelocity += config.SimulationSubstepTime * Math::Abs(weapon_TrajectoryGravityZ) * 100.0f * config.VDragCoefficient * BallisticDropScale;
					}

					if (distanceMax > zeroingDistance)
					{
						FTransform GunTransformZ = sdk::GetFiringTransform();
						auto fireP = GunTransformZ.GetTranslation();
						auto GunZ = GunTransformZ.GetRelativeTransform(sdk::GetFiringLocationTransform()).Translation.Z;
						float scopZ = 0.0f;
						//if (!sdk::GetAttachedStaticComponentMapValid())
						{
							auto scopP = sdk::GetScopingTransform().GetTranslation();
							scopZ = (scopP.Z - fireP.Z);
						}
						//else
						//{
						//	auto componentTransform = sdk::GetAttachedComponentTransform();
						//	auto scopPointLocation = componentTransform.Translation;
						//	FVector componentPosition = componentTransform.InverseTransformPositionNoScale(scopPointLocation);
						//	FTransform SocketTransform = sdk::GetFiringLocationTransform();
						//	FVector scopingPosition = SocketTransform.InverseTransformPositionNoScale(componentTransform.GetTranslation());
						//	scopZ = scopingPosition.Z + componentPosition.Z - GunZ;
						//	//CONSOLE_INFO("Normal:%f|%f", scopZ, componentPosition.Z);
						//	//auto MeshComponentToWorld = weapon->Mesh3P->GetComponentTransform();
						//	//float RelativeZ_1 = component->GetSocketTransform(weapon->ScopingAttachPoint, RTS_Component).GetTranslation().Z;
						//	//float RelativeZ_2 = componentTransform.GetRelativeTransform(MeshComponentToWorld).GetTranslation().Z;
						//	//scopZ = RelativeZ_1 + RelativeZ_2 - fireP.Z;
						//	//CONSOLE_INFO("After:%f", scopZ);
						//}

						//float BulletDrop = 0;
						//float TravelTime = 0;
						//zeroingDistance /= 100.f;
						//auto LocalComponentToWorld = sdk::GetFiringLocationTransform();
						//auto ControlRotation_CP = sdk::GetControlRotation_CP();
						//auto ControlRotation = ControlRotation_CP/* + Recoil*/;
						//float BulletDropAdd = scopZ;
						//auto GunLocation = GunTransformZ.Translation;
						//FRotator GunRotation = GunTransformZ.Rotation.Rotator();
						//auto AimLocation = GunLocation.Size() > 0.0f ? GunLocation : LocalComponentToWorld.Translation;
						//auto AimRotation = sdk::GetIsScoping_CP() ? GunRotation : ControlRotation;
						//auto Result = GetBulletDropAndTravelTime(
						//	config,
						//	AimLocation,
						//	AimRotation,
						//	aimPos,
						//	zeroingDistance,
						//	BulletDropAdd);
						//BulletDrop = Result.first;
						//TravelTime = Result.second;
						//return  FVector(aimPos.X, aimPos.Y, aimPos.Z + Math::Abs(BulletDrop)) + Velocity * (TravelTime /sdk::GetCustomTimeDilation());

						dropPitchOffset = (((Math::Abs(scopZ) + drop) / distanceMax) * RAD_TO_DEG) -
							(((Math::Abs(scopZ) + dropOffset) / zeroingDistance) * RAD_TO_DEG);
						//CONSOLE_INFO("dropPitchOffset:%f|%f", dropPitchOffset, distanceMax / 100.f);
						if (sdk::GetCurrentZeroLevel() == 0)
						{
							//值越大下压越大 这里是做一个计算后的Z值的补偿 因为不精准。
							static float BL200 = 0.015f;
							static float HI200 = 0.0115f;
							static float HI300 = 0.0175f;
							static float HI400 = 0.020f;
							static float HI500 = 0.0222f;
							static float HI600 = 0.0245f;
							static float HI700 = 0.0265f;
							static float HI800 = 0.0285f;
							static float HI900 = 0.0310f;

							if (distanceMax >= 90000.0f)
								dropPitchOffset -= HI900;
							else if (distanceMax >= 80000.0f)
								dropPitchOffset -= HI800;
							else if (distanceMax >= 70000.0f)
								dropPitchOffset -= HI700;
							else if (distanceMax >= 60000.0f)
								dropPitchOffset -= HI600;
							else if (distanceMax >= 50000.0f)
								dropPitchOffset -= HI500;
							else if (distanceMax >= 40000.0f)
								dropPitchOffset -= HI400;
							else if (distanceMax >= 30000.0f)
								dropPitchOffset -= HI300;
							else if (distanceMax >= 20000.0f)
								dropPitchOffset -= HI200;
							else
								dropPitchOffset -= BL200;
						}
					}
				}
				else
				{
					flyTime = 0.0f;
					dropPitchOffset = 0.5f * Math::Abs(sdk::GetTrajectoryGravityZ()) * powf(flyTime, 2);
				}
				return aimPos + Velocity * (flyTime / sdk::GetCustomTimeDilation());
			}

			return aimPos;
		}

		float SetUpFpsSmooth(float Fov)
		{
			uint32_t temp = uint32_t(Fov);
			if (temp >= 52)//紅點全息
				return (ImGui::GetIO().Framerate >= 140.f ? 2.2f : ImGui::GetIO().Framerate >= 120.f ? 2.7f : ImGui::GetIO().Framerate >= 100.f ? 3.2f : ImGui::GetIO().Framerate >= 80.f ? 3.7f : 4.2f);
			else if (temp >= 38)//二倍
				return (ImGui::GetIO().Framerate >= 140.f ? 2.0f : ImGui::GetIO().Framerate >= 120.f ? 2.5f : ImGui::GetIO().Framerate >= 100.f ? 3.0f : ImGui::GetIO().Framerate >= 80.f ? 3.5f : 4.0f);
			else if (temp >= 26)//三倍
				return (ImGui::GetIO().Framerate >= 140.f ? 1.8f : ImGui::GetIO().Framerate >= 120.f ? 2.3f : ImGui::GetIO().Framerate >= 100.f ? 2.8f : ImGui::GetIO().Framerate >= 80.f ? 3.3f : 3.8f);
			else if (temp >= 19)//四倍
				return (ImGui::GetIO().Framerate >= 140.f ? 1.6f : ImGui::GetIO().Framerate >= 120.f ? 2.1f : ImGui::GetIO().Framerate >= 100.f ? 2.6f : ImGui::GetIO().Framerate >= 80.f ? 3.1f : 3.6f);
			else if (temp >= 13)//六倍
				return (ImGui::GetIO().Framerate >= 140.f ? 1.4f : ImGui::GetIO().Framerate >= 120.f ? 1.9f : ImGui::GetIO().Framerate >= 100.f ? 2.4f : ImGui::GetIO().Framerate >= 80.f ? 2.9f : 3.4f);
			else if (temp >= 10)//八倍
				return (ImGui::GetIO().Framerate >= 140.f ? 1.2f : ImGui::GetIO().Framerate >= 120.f ? 1.7f : ImGui::GetIO().Framerate >= 100.f ? 2.2f : ImGui::GetIO().Framerate >= 80.f ? 2.7f : 3.2f);
			else
				return (ImGui::GetIO().Framerate >= 140.f ? 1.0f : ImGui::GetIO().Framerate >= 120.f ? 1.5f : ImGui::GetIO().Framerate >= 100.f ? 2.0f : ImGui::GetIO().Framerate >= 80.f ? 2.5f : 3.0f);
			return 2.f;
		}

		void mouseleftdown()
		{
			INPUT input = { 0 };
			input.type = INPUT_MOUSE;
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN/* | MOUSEEVENTF_ABSOLUTE*/;
			SendInput((UINT)1, (LPINPUT)&input, (int)sizeof(input));
		}

		void mouseleftup()
		{
			INPUT input = { 0 };
			input.type = INPUT_MOUSE;
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP/* | MOUSEEVENTF_ABSOLUTE*/;
			SendInput((UINT)1, (LPINPUT)&input, (int)sizeof(input));
		}

		void mymouse_event(LONG dx, LONG dy)
		{
			INPUT input = { 0 };
			input.type = INPUT_MOUSE;
			input.mi.dx = dx;
			input.mi.dy = dy;
			input.mi.dwFlags = MOUSEEVENTF_MOVE/* | MOUSEEVENTF_ABSOLUTE*/;
			SendInput((UINT)1, (LPINPUT)&input, (int)sizeof(input));
		}

		void DownPressMuzzle()
		{
			auto& weapon = sdk::GetWeapon();
			if (!IsAddrValid(weapon) || sdk::GetWeaponIndex() == 255)
				return;
			auto WeaponType = sdk::GetWeaponType();
			if (Vars.Aimbot.DownValue > 0.f && WeaponType == 0 && OverlayEngine::IsKeyDown(VK_LBUTTON) && !sdk::GetIsReloading_CP() && sdk::GetIsAiming_CP() && sdk::GetScopingAlpha_CP() >= 0.5f)
			{
				if (Vars.Aimbot.AimBotMethod == 1 && Vars.Aimbot.BProStatus == 1)
				{
					GetKmbox().SendMove(0, (int)Vars.Aimbot.DownValue, 0);
				}
				else if (Vars.Aimbot.AimBotMethod == 2 && Vars.Aimbot.BNetStatus == 1)
				{
					kmNet_mouse_move_auto(0, (int)Vars.Aimbot.DownValue, 0);
				}
				else if (Vars.Aimbot.AimBotMethod == 3 && Vars.Aimbot.LurkerStatus == 1)
				{
					GetKmbox().SendMoveLurker(0, (float)Vars.Aimbot.DownValue);
				}
				else if (Vars.Aimbot.AimBotMethod == 4 && Vars.Aimbot.JSMHStatus == 1)
				{
					GetKmbox().SendMoveJSMH(0, (float)Vars.Aimbot.DownValue);
				}
			}
		}

		float GetGrenadePredict(float Dist, float EnemyZ, float LocalZ)
		{
			if (Dist < 35.f)
			{
				return 0;
			}
			else if (Dist >= 35.f && Dist < 40.f)
			{
				float Min = 55.f;
				float Max = 200.f;
				float PerValue = (Max - Min) / 5.f;
				float retValue = Min + (Dist - 35.f) * PerValue;
				float ZValue = (EnemyZ - LocalZ);
				if (ZValue <= 30.f)
					return retValue + ZValue;
				else
					return retValue + ZValue * 0.3f;
			}
			else if (Dist >= 40.f && Dist < 45.f)
			{
				float Min = 200.f;
				float Max = 500.f;
				float PerValue = (Max - Min) / 5.f;
				float retValue = Min + (Dist - 40.f) * PerValue;
				float ZValue = (EnemyZ - LocalZ);
				if (ZValue <= 30.f)
					return retValue + ZValue;
				else
					return retValue + ZValue * 0.3f;
			}
			else if (Dist >= 45.f && Dist < 50.f)
			{
				float Min = 500.f;
				float Max = 900.f;
				float PerValue = (Max - Min) / 5.f;
				float retValue = Min + (Dist - 45.f) * PerValue;
				float ZValue = (EnemyZ - LocalZ);
				if (ZValue <= 30.f)
					return retValue + ZValue;
				else
					return retValue + ZValue * 0.3f;
			}
			else if (Dist >= 50.f && Dist < 55.f)
			{
				float Min = 900.f;
				float Max = 1500.f;
				float PerValue = (Max - Min) / 5.f;
				float retValue = Min + (Dist - 50.f) * PerValue;
				float ZValue = (EnemyZ - LocalZ);
				if (ZValue <= 30.f)
					return retValue + ZValue;
				else
					return retValue + ZValue * 0.3f;
			}
			else
			{
				float Min = 1500.f;
				float Max = 2500.f;
				float PerValue = (Max - Min) / 5.f;
				float retValue = Min + (Dist - 55.f) * PerValue;
				float ZValue = (EnemyZ - LocalZ);
				if (ZValue <= 30.f)
					return retValue + ZValue;
				else
					return retValue + ZValue * 0.3f;
			}
			return 0.f;
		}

		void AimToTarget()
		{
			if (!setInitPID)
			{
				Aimbot::curAimTime = 0.0f;
				Aimbot::finalTime = 0.0f;
				if (!Vars.Aimbot.AimMode)
				{
					PidX.init(Vars.Aimbot.AR_P_X1, Vars.Aimbot.I_X, Vars.Aimbot.D_X);
					PidY.init(Vars.Aimbot.AR_P_Y1, Vars.Aimbot.I_Y, Vars.Aimbot.D_Y);
				}
				else
				{
					PidX.init(Vars.Aimbot.AR_P_X2, Vars.Aimbot.I_X, Vars.Aimbot.D_X);
					PidY.init(Vars.Aimbot.AR_P_Y2, Vars.Aimbot.I_Y, Vars.Aimbot.D_Y);
				}
				setInitPID = true;
			}
			if (!sdk::InGame())
			{
				Aimbot::curAimTime = 0.0f;
				Aimbot::finalTime = 0.0f;
				PidX.clear();
				PidY.clear();
				return;
			}
			static bool bRecoil = false;
			auto lb = OverlayEngine::IsKeyDown(VK_LBUTTON);

			bool shiftb = false;
			auto WeaponType = SDK::sdk::GetWeaponType();
			if (WeaponType == 0)
				shiftb = OverlayEngine::IsKeyDown(Vars.Aimbot.AR_LockHeadHotKey);
			else if (WeaponType == 1)
				shiftb = OverlayEngine::IsKeyDown(Vars.Aimbot.SR_LockHeadHotKey);
			else
				shiftb = OverlayEngine::IsKeyDown(Vars.Aimbot.DMR_LockHeadHotKey);

			auto ctrl = OverlayEngine::IsKeyDown(VK_CONTROL);
			auto& player = sdk::GetPlayer();
			ATslCharacter* target = nullptr;
			ptr_t targetPtr = sdk::GetAimTarget();
			if (!IsAddrValid(targetPtr) || !sdk::IsLocalPlayerAlive())
			{
				if (sdk::IsLocalPlayerAlive())
					DownPressMuzzle();
				Aimbot::curAimTime = 0.0f;
				Aimbot::finalTime = 0.0f;
				m_Target = nullptr;
				PidX.clear();
				PidY.clear();
				return;
			}

			for (auto actor : sdk::m_cacheObjectsAimbot[EPlayer])
			{
				auto AimPlayer = *actor.get()->CCast<ATslCharacter>();
				if (!AimPlayer || !AimPlayer->IsValid())
				{
					continue;
				}
				if (AimPlayer->GetPtr() == (void*)targetPtr)
				{
					target = actor.get()->CCast<ATslCharacter>();
					break;
				}
			}

			if (OverlayEngine::IsKeyDown(Vars.Aimbot.HotKeyCar))
				Vars.Aimbot.StartLockCar = true;
			else
				Vars.Aimbot.StartLockCar = false;

			if (!LastHeadKey)
				LastHeadKey = ctrl ? 777 : shiftb ? 999 : 888;

			if ((ctrl ? 777 : shiftb ? 999 : 888) != LastHeadKey)
			{
				LastHeadKey = ctrl ? 777 : shiftb ? 999 : 888;
				Aimbot::curAimTime = 0.0f;
				Aimbot::finalTime = 0.0f;
			}

			if (Vars.Aimbot.CheckReloading)
			{
				if (sdk::GetIsReloading_CP())
				{
					Aimbot::curAimTime = 0.0f;
					Aimbot::finalTime = 0.0f;
					m_Target = nullptr;
					sdk::ClearAimTarget();
					PidX.clear();
					PidY.clear();
					return;
				}
			}

			if (Vars.Aimbot.AimWhileScoping)
			{
				if (!sdk::GetIsAiming_CP() || sdk::GetScopingAlpha_CP() <= 0.5f)
				{
					Aimbot::curAimTime = 0.0f;
					Aimbot::finalTime = 0.0f;
					m_Target = nullptr;
					sdk::ClearAimTarget();
					PidX.clear();
					PidY.clear();
					return;
				}
			}
			bool bIsMortar = IsAddrValid(sdk::GetMortarPlayer());
			auto& weapon = sdk::GetWeapon();
			if (!bIsMortar && !Vars.Aimbot.HandNotLock ? (!IsAddrValid(weapon) || sdk::GetWeaponIndex() == 255 || sdk::GetWeaponIndex() == 3) : false)
				return;
			bool bIsGrenade = false;
			if (Vars.Aimbot.GrenadePredict && sdk::GetIsGrenade())
				bIsGrenade = true;
			if (target && IsAddrValid(player) && target->IsValid() && sdk::IsLocalPlayerAlive() && target->_isInFog)
			{
				m_Target = target->GetPtr();
				{
					float dropPitchOffset = 0.0f;
					FVector aimPos(.0f, .0f, .0f);
					FVector muzzleOrigin;
					if (Vars.Aimbot.VisibleCheck && target->IsValid() && !bIsMortar && !bIsGrenade)
					{
						static int boneScans_headPriority[] = {
							forehead,Head,neck_01,spine_03,spine_02,spine_01,upperarm_l,upperarm_r,lowerarm_l,lowerarm_r,hand_l,hand_r,thumb_01_l,thumb_01_r,thigh_l,thigh_r,calf_l,calf_r,foot_l,foot_r,ik_foot_root,ik_foot_l,ik_foot_r
						};
						static int boneScans[] = {
							spine_03,spine_02,spine_01,upperarm_l,upperarm_r,lowerarm_l,lowerarm_r,hand_l,hand_r,thumb_01_l,thumb_01_r ,thigh_l,thigh_r,calf_l,calf_r,foot_l,foot_r,ik_foot_root,ik_foot_l,ik_foot_r,forehead,Head,neck_01
						};
						for (int i = 0; i < ARRAY_COUNT(boneScans); i++)
						{
							auto loop = shiftb ? boneScans_headPriority[i] : boneScans[i];
							auto CheckPos = sdk::GetBonePosition(*target, loop);
							if ((!g_physic->IsSceneCreate() ? sdk::PlayerVisibleCheck(*target) : g_physic->GetActorVisible(targetPtr, loop)))
							{
								aimPos = CheckPos;
								break;
							}
						}
					}
					else
					{
						aimPos = shiftb ? sdk::GetBonePosition(*target, Bone::forehead) : sdk::GetBonePosition(*target, Bone::spine_03);
					}

					if (aimPos.IsZero())
					{
						UVehicleRiderComponent VehicleRiderComponent = target->_VehicleRiderComponent;
						//if (VehicleRiderComponent.IsValid() && target->_SeatIndex >= 0 && sdk::PlayerVisibleCheck(*target))//in car
						if (VehicleRiderComponent.IsValid() && target->_SeatIndex >= 0 &&
							(!g_physic->IsSceneCreate() ?
								sdk::PlayerVisibleCheck(*target) :
								g_physic->WeaponCanHit(sdk::GetLocalPos(), sdk::GetBonePosition(*target, Bone::forehead), 200000.f, false, false, true)))//in car
						{
							aimPos = shiftb ? sdk::GetBonePosition(*target, Bone::forehead) : sdk::GetBonePosition(*target, Bone::spine_03);
						}
						else
						{
							DownPressMuzzle();
							Aimbot::curAimTime = 0.0f;
							Aimbot::finalTime = 0.0f;
							m_Target = nullptr;
							sdk::ClearAimTarget();
							PidX.clear();
							PidY.clear();
							return;
						}
					}
					if (aimPos.IsZero())
					{
						DownPressMuzzle();
					}

					//Tire hit
					if (Vars.Aimbot.StartLockCar && OverlayEngine::IsKeyDown(Vars.Aimbot.HotKeyCar) && sdk::PlayerVisibleCheck(*target))
					{
						UVehicleRiderComponent VehicleRiderComponent = target->_VehicleRiderComponent;
						if (VehicleRiderComponent->IsValid())
						{
							APawn LastVehiclePawn = target->_LastVehiclePawn;
							if (LastVehiclePawn->IsValid() && target->_LastVehiclePawnEncryptIndex != 0)
							{
								auto vehicle = LastVehiclePawn->Cast<ATslWheeledVehicle>();
								FName vehicleFName;
								vehicleFName.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(target->_LastVehiclePawnEncryptIndex);
								std::unique_ptr<std::string> vehicleName = std::make_unique<std::string>(vehicleFName.GetName2());
								auto VehicleBoneNameHash = sdk::GetVehicleBoneMapName();
								auto BoneResult = VehicleBoneNameHash.find(hash_(*vehicleName));
								if (BoneResult != VehicleBoneNameHash.end())
								{
									std::vector<FVector>FindBonePos;
									std::vector<int>FindBoneArray;
									for (int i = 0; i < BoneResult->second.size(); i++)
									{
										auto BoneStr = BoneResult->second[i];
										if (BoneStr.find(("Tire_")) != std::string::npos || BoneStr.find(("WheelF_Tire")) != std::string::npos ||
											BoneStr.find(("WheelR_Tire")) != std::string::npos || BoneStr.find(("Helper_")) != std::string::npos ||
											BoneStr.find(("PhysicsWheel_")) != std::string::npos || (BoneStr.find(("_Tire")) != std::string::npos && BoneStr != ("Spare_Tire"))
											|| BoneStr.find(("PhysWheel")) != std::string::npos
											)
										{
											FindBoneArray.push_back(i);
										}
									}

									if (FindBoneArray.size() && FindBoneArray.size() < 300 && target->_Vehicle_BoneTable.size())
									{
										float MostClosely = FLT_MAX;
										FVector MostVector(0.f, 0.f, 0.f);
										FindBonePos.resize(FindBoneArray.size());
										for (int i = 0; i < FindBoneArray.size(); i++)
										{
											FindBonePos[i] = target->_Vehicle_BoneTable[FindBoneArray[i]];
											if (FindBonePos[i].IsNearlyZero() || !FindBonePos[i].IsVaild()/* || target->_VehicleBoneInfo[FindBoneArray[i]].Name.ComparisonIndex == 0
												|| target->_VehicleBoneName[FindBoneArray[i]] == "None"*/)
												continue;
											if (!FindBonePos[i].IsZero())
											{
												auto delta = FindBonePos[i] - sdk::GetLocalPos();
												float dist = delta.Size() / 100.0f;
												if (dist < MostClosely)
												{
													MostClosely = dist;
													MostVector = FindBonePos[i];
													//CONSOLE_INFO("%s|%s|%d|%f|%f|%f", target->_VehicleBoneName[FindBoneArray[i]].c_str(), target->_VehicleBoneInfo[FindBoneArray[i]].Name.GetName(), target->_VehicleBoneInfo[FindBoneArray[i]].Name.ComparisonIndex, FindBonePos[i].X, FindBonePos[i].Y, FindBonePos[i].Z);
												}
											}

										}
										if (!MostVector.IsZero() && MostClosely != FLT_MAX)
											aimPos = MostVector;
									}
								}
							}
						}
					}

					if (bIsGrenade)
					{
						aimPos = sdk::GetBonePosition(*target, Bone::forehead);
					}
					if (!aimPos.IsZero())
					{
						auto PlayerVelocity = GetPlayerVelocity(*target);
						FVector predictionPos = GetPrediction(*target, aimPos, dropPitchOffset, PlayerVelocity);
						auto dist = (aimPos - sdk::GetLocalPos()).Size() / 100.f;
						bool isOpen = sdk::GetbSpawnBulletFromBarrel();
						if (Vars.Aimbot.Aimbot != 0)
						{
							float z = predictionPos.Z;
							predictionPos += PlayerVelocity * GetNextPos(*target, dist, PlayerVelocity);
							predictionPos.Z = z;
							if (bIsGrenade)
							{
								if (dist >= 70.f)
									return;
								predictionPos = aimPos;
								predictionPos.Z += GetGrenadePredict(dist, predictionPos.Z, sdk::GetLocalHeadPos().Z);
							}
							FRotator dst = Clamp(Vec2Rot(predictionPos - sdk::GetLocalPos()));
							FRotator src = isOpen ? Clamp(sdk::GetControlRotation_CP()) : sdk::GetLocalRot();
							auto WeaponType = sdk::GetWeaponType();
							if (isOpen && (!Vars.Aimbot.AimMode ? (WeaponType == 0 ? Vars.Aimbot.AR_NoRecoil1 : WeaponType == 1 ? Vars.Aimbot.SR_NoRecoil1 : Vars.Aimbot.DMR_NoRecoil1) : (WeaponType == 0 ? Vars.Aimbot.AR_NoRecoil2 : WeaponType == 1 ? Vars.Aimbot.SR_NoRecoil2 : Vars.Aimbot.DMR_NoRecoil2)))
							{
								auto firingRot = sdk::GetFiringLocationTransform().GetRotation().Rotator();
								firingRot.Roll = 0.0f;
								if ((firingRot - src).Size() <= (lb ? 10.0f : 5.f))
									src = firingRot;
							}

							auto viewCenter = Drawing::GetViewPort() * 0.5f;
							FVector2D moveMouse;
							src = Clamp(src - FRotator(dropPitchOffset, 0, 0));
							auto angDelta = Clamp(dst - src);
							if ((angDelta.Size() > 1.0f || (finalTime && finalTime > curAimTime)) && (angDelta.Size() > 1.0f ? true : !lb))
							{
								if (!finalTime)
									finalTime = angDelta.Size() / (!Vars.Aimbot.AimMode ? (WeaponType == 0 ? Vars.Aimbot.AR_SmoothValue1 : WeaponType == 1 ? Vars.Aimbot.SR_SmoothValue1 : Vars.Aimbot.DMR_SmoothValue1) : (WeaponType == 0 ? Vars.Aimbot.AR_SmoothValue2 : WeaponType == 1 ? Vars.Aimbot.SR_SmoothValue2 : Vars.Aimbot.DMR_SmoothValue2));
								curAimTime += Aim_DeltaTime;
								if (curAimTime >= finalTime)
									curAimTime = finalTime;
								float percent = curAimTime / finalTime;
								angDelta = angDelta * percent;
								dst = Clamp(dst - angDelta);
								moveMouse = sdk::WorldToScreen(predictionPos, dst);
							}
							else
							{
								angDelta = angDelta * ((!Vars.Aimbot.AimMode ? ((WeaponType == 0 ? Vars.Aimbot.AR_RecoilValue1 : WeaponType == 1 ? Vars.Aimbot.SR_RecoilValue1 : Vars.Aimbot.DMR_RecoilValue1)) : ((WeaponType == 0 ? Vars.Aimbot.AR_RecoilValue2 : WeaponType == 1 ? Vars.Aimbot.SR_RecoilValue2 : Vars.Aimbot.DMR_RecoilValue2))) / 100.f);
								dst = Clamp(dst - angDelta);
								moveMouse = sdk::WorldToScreen(predictionPos, dst);
							}
							if ((moveMouse.X == 0.f && moveMouse.Y == 0.f) || !moveMouse.IsVaild())
							{
								Aimbot::curAimTime = 0.0f;
								Aimbot::finalTime = 0.0f;
								PidX.clear();
								PidY.clear();
								return;
							}
							if (WeaponType != 0)
							{
								FVector2D RelativePosition = { moveMouse.X - viewCenter.X, moveMouse.Y - viewCenter.Y };

								float baseSmoothing = !Vars.Aimbot.AimMode ? (WeaponType == 1 ? Vars.Aimbot.SR_baseSmoothing1 : Vars.Aimbot.DMR_baseSmoothing1) : (WeaponType == 1 ? Vars.Aimbot.SR_baseSmoothing2 : Vars.Aimbot.DMR_baseSmoothing2); // 基础平滑因子
								float baseFOV = WeaponType == 1 ? Vars.Aimbot.SR_baseFOV : Vars.Aimbot.DMR_baseFOV; // 基础视野
								float maxPercentageIncrease = WeaponType == 1 ? Vars.Aimbot.SR_MaxSmoothIncrease : Vars.Aimbot.DMR_MaxSmoothIncrease; // 最大增量为基础平滑的 20%
								float maxIncrease = baseSmoothing * maxPercentageIncrease; // 平滑的绝对最大增量
								// 在 [-0.05, +0.05] 范围内随机化基础平滑
								baseSmoothing += Utils::RandomRange(-0.05f, 0.05f);
								// 在 [-1, +1] 范围内随机化 FOV
								baseFOV = 10.f + Utils::RandomRange(-1.0f, 1.0f);
								// 距目标的距离，已计算
								float distance = moveMouse.Distance(viewCenter);
								float dynamicSmoothing = baseSmoothing;
								if (distance <= baseFOV) {
									// 线性缩放：从 fovDistance 处的基础平滑到 1 像素处的基础平滑 + maxIncrease
									float scale = 1.0f - (distance / baseFOV); // 将比例标准化为 0（在 fovDistance）和 1（在 1 像素）之间
									dynamicSmoothing += maxIncrease * scale; // 应用按比例增加
								}
								// 确保dynamicSmoothing不会降低到baseSmoothing以下
								dynamicSmoothing = (std::max)(dynamicSmoothing, baseSmoothing);

								// 计算该帧的移动步数
								FVector2D step = {
									(RelativePosition.X / dynamicSmoothing),
									(RelativePosition.Y / dynamicSmoothing)
								};
								moveMouse = step;
								if ((std::abs(moveMouse.X) < 1.0f) && (std::abs(moveMouse.Y) < 1.0f)) {
									moveMouse.X = round(moveMouse.X);
									moveMouse.Y = round(moveMouse.Y);
								}
								if ((std::abs(moveMouse.X) >= viewCenter.X * 2.f) && (std::abs(moveMouse.Y) >= viewCenter.Y * 2.f)) {
									Aimbot::curAimTime = 0.0f;
									Aimbot::finalTime = 0.0f;
									return;
								}
							}
							else
							{
								moveMouse.X = (float)PidX.compute((double)moveMouse.X, (double)viewCenter.X);
								moveMouse.Y = (float)PidY.compute((double)moveMouse.Y, (double)viewCenter.Y);

								if ((std::abs(moveMouse.X) < 1.0f) && (std::abs(moveMouse.Y) < 1.0f)) {
									moveMouse.X = round(moveMouse.X);
									moveMouse.Y = round(moveMouse.Y);
								}

								if ((std::abs(moveMouse.X) >= viewCenter.X * 2.f) && (std::abs(moveMouse.Y) >= viewCenter.Y * 2.f)) {
									Aimbot::curAimTime = 0.0f;
									Aimbot::finalTime = 0.0f;
									PidX.clear();
									PidY.clear();
									return;
								}
							}

							if (Vars.Aimbot.SingleStep && WeaponType != 0)
							{
								float MovePerTimeX = !Vars.Aimbot.AimMode ? (WeaponType == 1 ? Vars.Aimbot.SR_MOVESTEP_X1 : Vars.Aimbot.DMR_MOVESTEP_X1) : (WeaponType == 1 ? Vars.Aimbot.SR_MOVESTEP_X2 : Vars.Aimbot.DMR_MOVESTEP_X2);
								float MovePerTimeY = !Vars.Aimbot.AimMode ? (WeaponType == 1 ? Vars.Aimbot.SR_MOVESTEP_Y1 : Vars.Aimbot.DMR_MOVESTEP_Y1) : (WeaponType == 1 ? Vars.Aimbot.SR_MOVESTEP_Y2 : Vars.Aimbot.DMR_MOVESTEP_Y2);

								MovePerTimeX = MovePerTimeX / sdk::GetFov() * 80.f;
								MovePerTimeY = MovePerTimeY / sdk::GetFov() * 80.f;

								if (IsAddrValid(target->_LastVehiclePawn) && target->_SeatIndex >= 0)
								{
									MovePerTimeX *= 2.f;
									MovePerTimeY *= 2.f;
								}

								if (moveMouse.X > MovePerTimeX)
									moveMouse.X = MovePerTimeX;
								else if (moveMouse.X < -MovePerTimeX)
									moveMouse.X = -MovePerTimeX;
								if (moveMouse.Y > MovePerTimeY)
									moveMouse.Y = MovePerTimeY;
								else if (moveMouse.Y < -MovePerTimeY)
									moveMouse.Y = -MovePerTimeY;
							}

							if (Vars.Aimbot.AimBotMethod == 1 && Vars.Aimbot.BProStatus == 1)
							{
								GetKmbox().SendMove(moveMouse.X, (int)moveMouse.Y, 0);
							}
							else if (Vars.Aimbot.AimBotMethod == 2 && Vars.Aimbot.BNetStatus == 1)
							{
								kmNet_mouse_move_auto(moveMouse.X, (int)moveMouse.Y, 0);
							}
							else if (Vars.Aimbot.AimBotMethod == 3 && Vars.Aimbot.LurkerStatus == 1)
							{
								GetKmbox().SendMoveLurker(moveMouse.X, moveMouse.Y);
							}
							else if (Vars.Aimbot.AimBotMethod == 4 && Vars.Aimbot.JSMHStatus == 1)
							{
								GetKmbox().SendMoveJSMH(moveMouse.X, moveMouse.Y);
							}
						}
					}
					else
					{
						Aimbot::curAimTime = 0.0f;
						Aimbot::finalTime = 0.0f;
						PidX.clear();
						PidY.clear();
						return;
					}
				}
			}
			else
			{
				DownPressMuzzle();
				m_Target = nullptr;
				Aimbot::curAimTime = 0.0f;
				Aimbot::finalTime = 0.0f;
				PidX.clear();
				PidY.clear();
			}
			Vars.Aimbot.StartLockCar = false;
		}
	}

	bool PointIsInCirCle(ImVec2 center, float _r, ImVec2 point)
	{
		float dis = (point.x - center.x) * (point.x - center.x)
			+ (point.y - center.y) * (point.y - center.y);
		if (dis <= _r * _r)
			return true;
		else
			return false;
	};

	ImVec2 GetViewPos(ImVec2 Pos, float Ang, float Distant)
	{
		float radian = Ang * PI / 180.f;
		float xMargin = cos(radian) * Distant;
		float yMargin = sin(radian) * Distant;
		return ImVec2(Pos.x + xMargin, Pos.y + yMargin);
	}

	double get_angle(ImVec2 A, ImVec2 B, ImVec2 C)
	{
		double theta = atan2(A.x - C.x, A.y - C.y) - atan2(B.x - C.x, B.y - C.y);
		if (theta > M_PI)
			theta -= 2 * M_PI;
		if (theta < -M_PI)
			theta += 2 * M_PI;

		theta = abs(theta * 180.0 / M_PI);
		return theta;
	}

	// 计算叉积
	double crossProduct(ImVec2 p1, ImVec2 p2, ImVec2 p3) {
		return (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
	}

	// 判断点是否在四边形内部
	bool isInsideQuadrilateral(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImVec2 p) {
		double cross1 = crossProduct(p1, p2, p);
		double cross2 = crossProduct(p2, p3, p);
		double cross3 = crossProduct(p3, p4, p);
		double cross4 = crossProduct(p4, p1, p);

		if ((cross1 > 0 && cross2 > 0 && cross3 > 0 && cross4 > 0) ||
			(cross1 < 0 && cross2 < 0 && cross3 < 0 && cross4 < 0)) {
			return true;
		}

		return false;
	}

	ImVec2 GetMapRaderCenter()
	{
		ImVec2 ClientRect = Drawing::GetViewPort();
		if (!RaderType)
		{
			if (ClientRect.x == 1280.f && ClientRect.y == 720.f)
				return ImVec2(0.9136f, 0.8546f);
			else if (ClientRect.x == 1280.f && ClientRect.y == 768.f)
				return ImVec2(0.9078f, 0.8544f);
			else if (ClientRect.x == 1280.f && ClientRect.y == 800.f)
				return ImVec2(0.9038f, 0.8546f);
			else if (ClientRect.x == 1360.f && ClientRect.y == 768.f)
				return ImVec2(0.9136f, 0.8548f);
			else if (ClientRect.x == 1366.f && ClientRect.y == 768.f)
				return ImVec2(0.9136f, 0.8548f);
			else if (ClientRect.x == 1440.f && ClientRect.y == 900.f)
				return ImVec2(0.904f, 0.8546f);
			else if (ClientRect.x == 1600.f && ClientRect.y == 900.f)
				return ImVec2(0.9138f, 0.8546f);
			else if (ClientRect.x == 1680.f && ClientRect.y == 1050.f)
				return ImVec2(0.9038f, 0.8549f);
			else if (ClientRect.x == 1728.f && ClientRect.y == 1080.f)
				return ImVec2(0.9063f, 0.8539f);
			else if (ClientRect.x == 1920.f && ClientRect.y == 1080.f)
				return ImVec2(0.914f, 0.855f);
			else if (ClientRect.x == 1920.f && ClientRect.y == 1200.f)
				return ImVec2(0.90478f, 0.8535f);
			else if (ClientRect.x == 2560.f && ClientRect.y == 1080.f)
				return ImVec2(0.936585f, 0.8519f);
			else if (ClientRect.x == 2560.f && ClientRect.y == 1440.f)
				return ImVec2(0.91424f, 0.85559f);
			else if (ClientRect.x == 2560.f && ClientRect.y == 1600.f)
				return ImVec2(0.915490f, 0.852890f);
			else if (ClientRect.x == 3440.f && ClientRect.y == 1440.f)
				return ImVec2(0.937135f, 0.8525f);
			else if (ClientRect.x == 3840.f && ClientRect.y == 2160.f)
				return ImVec2(0.914402f, 0.8554f);
			else
				return ImVec2(0.914402f, 0.8554f);
		}
		else
		{
			if (ClientRect.x == 1280.f && ClientRect.y == 720.f)
				return ImVec2(0.861323f, 0.761643f);
			else if (ClientRect.x == 1280.f && ClientRect.y == 768.f)
				return ImVec2(0.851852f, 0.761865f);
			else if (ClientRect.x == 1280.f && ClientRect.y == 800.f)
				return ImVec2(0.8456486f, 0.761643f);
			else if (ClientRect.x == 1360.f && ClientRect.y == 768.f)
				return ImVec2(0.8613228f, 0.761821f);
			else if (ClientRect.x == 1366.f && ClientRect.y == 768.f)
				return ImVec2(0.8613428f, 0.761821f);
			else if (ClientRect.x == 1440.f && ClientRect.y == 900.f)
				return ImVec2(0.8455022f, 0.761643f);
			else if (ClientRect.x == 1600.f && ClientRect.y == 900.f)
				return ImVec2(0.8615114f, 0.761643f);
			else if (ClientRect.x == 1680.f && ClientRect.y == 1050.f)
				return ImVec2(0.8469486f, 0.761910f);
			else if (ClientRect.x == 1728.f && ClientRect.y == 1080.f)
				return ImVec2(0.8493f, 0.7607f);
			else if (ClientRect.x == 1920.f && ClientRect.y == 1080.f)
				return ImVec2(0.8617f, 0.762f);
			else if (ClientRect.x == 1920.f && ClientRect.y == 1200.f)
				return ImVec2(0.84677f, 0.762285f);
			else if (ClientRect.x == 2560.f && ClientRect.y == 1080.f)
				return ImVec2(0.897178f, 0.760035f);
			else if (ClientRect.x == 2560.f && ClientRect.y == 1440.f)
				return ImVec2(0.862473f, 0.762785f);
			else if (ClientRect.x == 2560.f && ClientRect.y == 1600.f)
				return ImVec2(0.863273f, 0.759935f);
			else if (ClientRect.x == 3440.f && ClientRect.y == 1440.f)
				return ImVec2(0.897929f, 0.759484f);
			else if (ClientRect.x == 3840.f && ClientRect.y == 2160.f)
				return ImVec2(0.862393f, 0.762185f);
			else
				return ImVec2(0.862393f, 0.762185f);
		}
	}

	float GetMapDiv()
	{
		ImVec2 ClientRect = Drawing::GetViewPort();
		if (!RaderType)
		{
			if (ClientRect.x == 1728.f && ClientRect.y == 1080.f)
				return 122.f;
		}
		else
		{
			if (ClientRect.x == 1728.f && ClientRect.y == 1080.f)
				return 132.f;
		}

		if (ClientRect.x == 1280.f && ClientRect.y == 720.f)
			return 86.f;
		else if (ClientRect.x == 1280.f && ClientRect.y == 768.f)
			return 92.f;
		else if (ClientRect.x == 1280.f && ClientRect.y == 800.f)
			return 96.f;
		else if (ClientRect.x == 1360.f && ClientRect.y == 768.f)
			return 92.f;
		else if (ClientRect.x == 1366.f && ClientRect.y == 768.f)
			return 92.f;
		else if (ClientRect.x == 1440.f && ClientRect.y == 900.f)
			return 104.f;
		else if (ClientRect.x == 1600.f && ClientRect.y == 900.f)
			return 104.f;
		else if (ClientRect.x == 1680.f && ClientRect.y == 1050.f)
			return 126.f;
		else if (ClientRect.x == 1920.f && ClientRect.y == 1080.f)
			return 126.f;
		else if (ClientRect.x == 1920.f && ClientRect.y == 1200.f)
			return 146.f;
		else if (ClientRect.x == 2560.f && ClientRect.y == 1080.f)
			return 128.f;
		else if (ClientRect.x == 2560.f && ClientRect.y == 1440.f)
			return 175.f;
		else if (ClientRect.x == 2560.f && ClientRect.y == 1600.f)
			return 172.f;
		else if (ClientRect.x == 3440.f && ClientRect.y == 1440.f)
			return 158.f;
		else if (ClientRect.x == 3840.f && ClientRect.y == 2160.f)
			return 258.f;
		else
			return 258.f;
	}

	BOOL ExPandLine(ImVec2 pt1, ImVec2 pt2, DOUBLE nLen, ImVec2& OutPt)
	{
		if (pt1.x - pt2.x == 0)
		{
			OutPt.x = pt1.x;
			if (pt1.y - pt2.y > 0)
			{
				OutPt.y = pt2.y - nLen;
			}
			else
			{
				OutPt.y = pt2.y + nLen;
			}
		}
		else if (pt1.y - pt2.y == 0)
		{
			OutPt.y = pt1.y;
			if (pt1.x - pt2.x > 0)
			{
				OutPt.x = pt2.x - nLen;
			}
			else
			{
				OutPt.x = pt2.x + nLen;
			}
		}
		else
		{
			DOUBLE k = 0.0;
			DOUBLE b = 0.0;
			k = (pt1.y - pt2.y) / (pt1.x - pt2.x);
			b = pt1.y - k * pt1.x;
			DOUBLE zoom = 0.0;
			zoom = nLen / sqrt((pt2.x - pt1.x) * (pt2.x - pt1.x) + (pt2.y - pt1.y) * (pt2.y - pt1.y));

			if (k > 0)
			{
				if (pt1.x - pt2.x > 0)
				{
					OutPt.x = pt2.x - zoom * (pt1.x - pt2.x);
					OutPt.y = k * OutPt.x + b;
				}
				else
				{
					OutPt.x = pt2.x + zoom * (pt2.x - pt1.x);
					OutPt.y = k * OutPt.x + b;
				}
			}
			else
			{
				if (pt1.x - pt2.x > 0)
				{
					OutPt.x = pt2.x - zoom * (pt1.x - pt2.x);
					OutPt.y = k * OutPt.x + b;
				}
				else
				{
					OutPt.x = pt2.x + zoom * (pt2.x - pt1.x);
					OutPt.y = k * OutPt.x + b;
				}
			}
		}
		return TRUE;
	}

	bool SmartSeleteEquipment(int LootId, float Durability, float DurabilityMax)
	{
		if (_InventoryEquipment[0].LootId == 0)//头部未装备
			if (LootId == 1412 || LootId == 1413 || LootId == 1414)//如果过滤的是头盔则通过
				return true;
		if (_InventoryEquipment[1].LootId == 0)//防弹衣未装备
			if (LootId == 1401 || LootId == 1402)//如果过滤的是防弹衣则通过
				return true;
		if (_InventoryEquipment[2].LootId == 0)//背包未装备
			if (LootId == 1404 || LootId == 1405 || LootId == 1408 || LootId == 1409 || LootId == 1415)//如果过滤的是背包则通过
				return true;

		if (_InventoryEquipment[0].LootId == 1410 || _InventoryEquipment[0].LootId == 1411)
		{
			if (LootId == 1412 || LootId == 1413 || LootId == 1414)
				return true;
		}

		if (_InventoryEquipment[1].LootId == 1403)
		{
			if (LootId == 1401 || LootId == 1402)
				return true;
		}

		if (_InventoryEquipment[2].LootId == 1406 || _InventoryEquipment[2].LootId == 1407)
		{
			if (LootId == 1408 || LootId == 1409 || LootId == 1404 || LootId == 1405)
				return true;
		}

		if (LootId == 1412 || LootId == 1413 || LootId == 1414)
		{
			if (_InventoryEquipment[0].LootId == 1412 || _InventoryEquipment[0].LootId == 1413)//二级头
			{
				if (_InventoryEquipment[0].Durability == _InventoryEquipment[0].DurabilityMax)//二级头耐久满的话只显示3级头
				{
					if (LootId == 1414 && _InventoryEquipment[0].Durability >= 30.f)//如果是三级头则显示
						return true;
					else//不是则不显示
						return false;
				}
				else if (LootId == 1412 || LootId == 1413 || LootId == 1414)//二级头耐久不满的话显示2级头和3级
				{
					if (LootId == 1414 && Durability >= 30.f)
						return true;
					else if ((LootId == 1412 || LootId == 1413) && Durability > _InventoryEquipment[0].Durability)
						return true;
					else
						return false;
				}
				else
					return false;
			}
			else if (_InventoryEquipment[0].LootId == 1414)//三级头
			{
				if (_InventoryEquipment[0].Durability < 10.f)//三级头耐久低于50%则显示二级头和三级头
				{
					if (LootId == 1412 || LootId == 1413 || LootId == 1414)
					{
						if (LootId == 1414 && Durability > _InventoryEquipment[0].Durability)
							return true;
						else if ((LootId == 1412 || LootId == 1413) && Durability > _InventoryEquipment[0].Durability)
							return true;
						else
							return false;
					}
					else
						return false;
				}
				else if (_InventoryEquipment[0].Durability != _InventoryEquipment[0].DurabilityMax)//三级头耐久不满 只显示三级头
				{
					if (LootId == 1414 && Durability > _InventoryEquipment[0].Durability)
						return true;
					else
						return false;
				}
				else
					return false;
			}
		}

		if (LootId == 1401 || LootId == 1402)
		{
			if (_InventoryEquipment[1].LootId == 1402)//二级甲
			{
				if (_InventoryEquipment[1].Durability == _InventoryEquipment[1].DurabilityMax)//二级甲耐久满的话只显示3级甲
				{
					if (LootId == 1401 && _InventoryEquipment[1].Durability >= 30.f)//3级甲
						return true;
					else
						return false;
				}
				else if (LootId == 1401 || LootId == 1402)//二级甲耐久不满的话显示2级甲和3级甲
				{
					if (LootId == 1401 && Durability >= 30.f)
						return true;
					else if (LootId == 1402 && Durability > _InventoryEquipment[1].Durability)
						return true;
					else
						return false;
				}
				else
					return false;
			}
			else if (_InventoryEquipment[1].LootId == 1401)//三级甲
			{
				if (_InventoryEquipment[1].Durability < 30.f)//三级甲耐久低于30%则显示二级甲和三级甲
				{
					if (LootId == 1401 && Durability > _InventoryEquipment[1].Durability)
						return true;
					else if (LootId == 1402 && Durability > _InventoryEquipment[1].Durability)
						return true;
					else
						return false;
				}
				else if (_InventoryEquipment[1].Durability != _InventoryEquipment[1].DurabilityMax)//三级头耐久不满 只显示三级头
				{
					if (LootId == 1401 && Durability > _InventoryEquipment[1].Durability)
						return true;
					else
						return false;
				}
				else
					return false;
			}
		}

		if (LootId == 1404 || LootId == 1405 || LootId == 1408 || LootId == 1409 || LootId == 1415)
		{
			if (_InventoryEquipment[2].LootId == 1415)
			{
				if (LootId == 1404 || LootId == 1405)
					return true;
				else
					return false;
			}
			else if (_InventoryEquipment[2].LootId == 1408 || _InventoryEquipment[2].LootId == 1409)//二级包
			{
				if (LootId == 1404 || LootId == 1405 || LootId == 1415)
					return true;
				else
					return false;
			}
			else if (_InventoryEquipment[2].LootId == 1404 || _InventoryEquipment[2].LootId == 1405)//三级包
				return false;
		}

		return true;
	}

	SmartItemID SmartWeapon()
	{
		auto WeaponValue = SmartItemID::NoWeapon;
		auto& player = sdk::GetPlayer();
		if (IsAddrValid(player))
		{
			bool bfirstWeaponEquip = false;
			bool bsecondWeaponEquip = false;
			auto& Weapon1 = sdk::GetWeaponNum1();
			if (IsAddrValid(Weapon1))
				bfirstWeaponEquip = true;
			auto& Weapon2 = sdk::GetWeaponNum2();
			if (IsAddrValid(Weapon2))
				bsecondWeaponEquip = true;
			if (!bfirstWeaponEquip && !bsecondWeaponEquip)
				WeaponValue = SmartItemID::NoWeapon;
			else if (bfirstWeaponEquip && bsecondWeaponEquip)
				WeaponValue = SmartItemID::AllArmed;
			else
				WeaponValue = SmartItemID::Armed;
		}
		return WeaponValue;
	}

	SmartItemID SmartSeleteAttachment(int LootId, hash_t itemHash)
	{
		int AttachNumber = 0;

		for (int i = 0; i < _EquipmentData1.size(); i++)
		{
			if (hash_("ItemName_SideRail_DotSight_RMR") != itemHash)
			{
				AttachNumber++;
			}
		}

		for (int i = 0; i < _EquipmentData1.size(); i++)
		{
			if (hash_(_EquipmentData1[i].itemName) == itemHash)//武器1如果装备有当前物品
				return SmartItemID::Equipped;
		}

		auto& weapon = sdk::GetWeapon();
		auto& WeaponName = sdk::GetWeaponName();
		if (IsAddrValid(weapon) && Utils::StringIsValid(WeaponName))
		{
			const auto weaponData = HandWeaponManager::GetHandWeapon(hash_(WeaponName));
			if (weaponData && weaponData->AttachAble.size() > 0)
			{
				if (AttachNumber == weaponData->AttachNum)
				{
					//CONSOLE_INFO("AttachNumber:%d|%d", AttachNumber, weaponData->AttachNum);
					return SmartItemID::Full;
				}
				auto Result = weaponData->AttachAble.find(LootId);
				if (Result != weaponData->AttachAble.end())
					return SmartItemID::SuitAttach;
				else
					return SmartItemID::UnSuit;
			}
			return SmartItemID::SuitAttach;
		}
		return SmartItemID::UnSuit;
	}

	void AimbotVEH()
	{
		if (!sdk::InGame())
		{
			if (sdk::m_cacheObjectsAimbot.size())
			{
				for (auto& objects : sdk::m_cacheObjectsAimbot)
					objects.second.clear();
				sdk::m_cacheObjectsAimbot.clear();
			}
			return;
		}
		auto AllObject = ObjectManager::GetObjects();
		for (auto& object : AllObject)
		{
			if (!object.second || !object.second->IsValid() || !IsAddrValid(object.second->GetPtr()))
				continue;
			auto type = object.second->GetObjectType();
			if (object.second->IsValid() && type == EPlayer)
			{
				sdk::m_cacheObjectsAimbot[type].push_back(object.second);
			}
		}
		if (!sdk::m_cacheObjectsAimbot[EPlayer].size())
			return;
		if (AllObject.size())
			AllObject.clear();
		try
		{
			if (g_ServerResult.Version >= 2)
			{
				if (bControllerPress)
				{
					auto end = std::chrono::high_resolution_clock::now();
					auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - bPressLogTime).count();
					bool NotRifer = Vars.Aimbot.RiferAutoShot && !sdk::GetIsShotGun() && !sdk::GetIsDMR() && !sdk::GetIsSR() && !sdk::GetIsGrenade() && sdk::GetWeaponIndex() != 255;
					int SleepValue = !NotRifer ? 250 : (200 / (sdk::GetFov() / sdk::GetMaxFov()));
					if (elapsed >= SleepValue)
					{
						if (Vars.Aimbot.AimBotMethod == 1 && Vars.Aimbot.BProStatus == 1)
						{
							GetKmbox().downLeft(0);
							bControllerPress = false;
							LogTimeToShot = 0;
						}
						else if (Vars.Aimbot.AimBotMethod == 2 && Vars.Aimbot.BNetStatus == 1)
						{
							kmNet_mouse_left(0);
							bControllerPress = false;
							LogTimeToShot = 0;
						}
						else if (Vars.Aimbot.AimBotMethod == 3 && Vars.Aimbot.LurkerStatus == 1)
						{
							bControllerPress = false;
							LogTimeToShot = 0;
						}
						else if (Vars.Aimbot.AimBotMethod == 4 && Vars.Aimbot.JSMHStatus == 1)
						{
							GetKmbox().LeftUpJSMH();
							bControllerPress = false;
							LogTimeToShot = 0;
						}
					}
				}
			}

			Aimbot::AimToTarget();
			if (g_ServerResult.Version >= 2)
			{
				if (Vars.Aimbot.AutoShot && (Vars.Aimbot.RiferAutoShot || Vars.Aimbot.ShotGunAutoShot || Vars.Aimbot.SRGunAutoShot))
				{
					for (auto actor : cpplinq::from(sdk::m_cacheObjectsAimbot[EPlayer])
						>> cpplinq::ref()
						>> cpplinq::where([](auto&& o) { return o.get() != nullptr && o.get()->IsValid(); })
						>> cpplinq::to_vector())
					{
						Visuals::PlayerTrigger(*actor.get()->CCast<ATslCharacter>());
					}
				}
			}


			auto WeaponType = sdk::GetWeaponType();
			bool PreditctOpen = WeaponType == 0 ? Vars.Aimbot.AR_PredictionPoint : WeaponType ? Vars.Aimbot.SR_PredictionPoint : Vars.Aimbot.DMR_PredictionPoint;
			if (PreditctOpen && sdk::GetTarget())
			{
				auto target = sdk::GetTarget()->Cast<ATslCharacter>();
				if (target->IsValid())
				{
					for (auto actor : sdk::m_cacheObjectsAimbot[EPlayer])
					{
						auto AimPlayer = *actor.get()->CCast<ATslCharacter>();
						if (!AimPlayer || !AimPlayer->IsValid())
						{
							continue;
						}
						if (AimPlayer->GetPtr() == target->GetPtr())
						{
							target = *actor.get()->CCast<ATslCharacter>();
							break;
						}
					}
				}
				bool PreditctOpen = WeaponType == 0 ? Vars.Aimbot.AR_PredictionPoint : WeaponType ? Vars.Aimbot.SR_PredictionPoint : Vars.Aimbot.DMR_PredictionPoint;
				if (target->IsValid() && PreditctOpen)
				{
					bool duiyou = !!target->_Team;
					if (!Vars.Aimbot.Team ? true : !duiyou)
					{
						auto headPos = sdk::GetBonePosition(target, Bone::forehead);
						auto origin = target->_Location;
						auto delta = target->_delta;
						float dist = target->_dist;
						if (dist <= (Vars.espHuman.DistanceSlider) && dist > 0)
						{
							float dropPitchOffset = 0.0f;
							auto PlayerVelocity = Aimbot::GetPlayerVelocity(target);
							auto predictionPos = Aimbot::GetPrediction(target, headPos, dropPitchOffset, PlayerVelocity);
							float z = predictionPos.Z;
							predictionPos += PlayerVelocity * Aimbot::GetNextPos(target, dist, PlayerVelocity);
							predictionPos.Z = z;
							FRotator curentRotation = sdk::GetControlRotation();
							curentRotation = Clamp(curentRotation - FRotator(dropPitchOffset, 0, 0));
							PredictionPoint = sdk::WorldToScreen(predictionPos, curentRotation);
						}
						else
						{
							PredictionPoint.X = 0;
							PredictionPoint.Y = 0;
						}
					}
					else
					{
						PredictionPoint.X = 0;
						PredictionPoint.Y = 0;
					}
				}
				else
				{
					PredictionPoint.X = 0;
					PredictionPoint.Y = 0;
				}
			}
			else
			{
				PredictionPoint.X = 0;
				PredictionPoint.Y = 0;
			}
		}
		catch (...)
		{
			CONSOLE_INFO2("AimBot Thread Cause VEH");
		}

		if (sdk::m_cacheObjectsAimbot.size())
		{
			for (auto& objects : sdk::m_cacheObjectsAimbot)
				objects.second.clear();
			sdk::m_cacheObjectsAimbot.clear();
		}
		return;
	}

	volatile long long lastUpdateTime = 0; // 共享变量，用于记录上次更新的时间戳
	int IPo = 0;

	DWORD AimbotThread(LPVOID ss)
	{
		static ptr_t WaitTime = 0;
		static ptr_t StartTime = 0;
		int Count = 0;
		static int LastAimbot = 0;
		bool ChangeBot = false;
		using framerate = std::chrono::duration<int, std::ratio<1, 80>>;
		auto tp = std::chrono::system_clock::now() + framerate{ 1 };
		using framerate2 = std::chrono::duration<int, std::ratio<1, 120>>;
		auto tp2 = std::chrono::system_clock::now() + framerate2{ 1 };
		do
		{
			lastUpdateTime = std::chrono::steady_clock::now().time_since_epoch().count();
			if (Vars.Aimbot.HighAimbot != LastAimbot)
			{
				LastAimbot = Vars.Aimbot.HighAimbot;
				Count = 0;
				ChangeBot = true;
				WaitTime = GetTickCount64();
			}
			try
			{
				auto& player = sdk::GetPlayer();
				if (sdk::InGame() && sdk::IsLocalPlayerAlive() && IsAddrValid(player))
				{
					if (!ChangeBot)
					{
						AimbotVEH();
					}
					else
					{
						if (GetTickCount64() - WaitTime >= 2000)
						{
							ChangeBot = false;
							//CONSOLE_INFO("start Aimbot");
						}
					}
					Count++;
					if (LastAimbot == 0)
					{
						std::this_thread::sleep_until(tp);
						tp += framerate{ 1 };
					}
					else
					{
						std::this_thread::sleep_until(tp2);
						tp2 += framerate2{ 1 };
					}
					//if (GetTickCount64() - StartTime >= 1000)
					//{
					//	StartTime = GetTickCount64();
					//	CONSOLE_INFO("Aimbot:%d", Count);
					//	Count = 0;
					//}
				}
				else
				{
					Sleep(1000);
				}
			}
			catch (...)
			{
				continue;
			}

		} while (true);
		return 1;
	}

	bool CheckThreadIsAlive() {
		long long currentTime = std::chrono::steady_clock::now().time_since_epoch().count();
		long long elapsedTime = currentTime - lastUpdateTime;
		return elapsedTime < 10000000000; // 设置一个超时时间，例如2秒
	}

	DWORD DetectedThread(LPVOID ss)
	{
		while (true)
		{
			if (!CheckThreadIsAlive()) {
				CONSOLE_INFO2("Thread seems to be stuck!");
				if (AimbotThreadHandle)
				{
					TerminateThread(AimbotThreadHandle, 0);
					AimbotThreadHandle = NULL;
				}
				AimbotThreadHandle = CreateThread((LPSECURITY_ATTRIBUTES)NULL, (SIZE_T)NULL, (LPTHREAD_START_ROUTINE)&AimbotThread, (LPVOID)NULL, (DWORD)NULL, (LPDWORD)NULL);
				Sleep(2000);
			}
			Sleep(1000);
		}

		return 0;
	}

	void AddScopeBoard2(ScoreBoard b)
	{
		if (b.Level <= 0 || b.Level > 2500 || b.PlayerName == "")
			return;
		bool isExist = false;
		for (auto iter : Visuals::m_ScoreBoard2)
		{
			if (b.PlayerName == iter.PlayerName)
			{
				isExist = true;
				break;
			}
		}
		if (!isExist)
		{
			Visuals::m_ScoreBoard2.push_back(b);
		}
		else
		{
			for (int i = 0; i < Visuals::m_ScoreBoard2.size(); i++)
			{
				if (b.PlayerName == Visuals::m_ScoreBoard2[i].PlayerName)
				{
					Visuals::m_ScoreBoard2[i] = b;
				}
			}
		}
	}

	bool Visuals::定义雷达图(FVector deltaFromLocal, int TeamNum, FRotator LastAimOffset, int Drawtype)
	{
		auto viewSize = Drawing::GetViewPort();

		bool IsSee = false;
		if (!RaderType)
		{
			if (deltaFromLocal.X > 20000.f || deltaFromLocal.X < -20000.f)
				return false;
			if (deltaFromLocal.Y > 20000.f || deltaFromLocal.Y < -20000.f)
				return false;
		}
		else
		{
			if (deltaFromLocal.X > 37000.f || deltaFromLocal.X < -37000.f)
				return false;
			if (deltaFromLocal.Y > 37000.f || deltaFromLocal.Y < -37000.f)
				return false;
		}

		ImVec2 MiniMapPos = GetMapRaderCenter();
		float MapDiv = GetMapDiv();

		//static ImVec2 MiniMapPos = ImVec2(0, 0);
		//static float MapDiv = 0;
		//if (MiniMapPos.x == 0.f && MiniMapPos.y == 0.f)
		//	MiniMapPos = ImVec2(0.862473f, 0.762785f);
		//if (MapDiv == 0.f)
		//	MapDiv = 172.f;
		////if ((viewSize.X > 1920.f || viewSize.Y > 1080.f))
		//{
		//	Drawing::DrawTextOutline(OverlayEngine::Font18px, 18, 0, ImVec2(50, 50), ImColor(0.39f, 0.58f, 0.9f), ImColor(0.0f, 0.0f, 0.0f, 1.0f), FONT_LEFT, ("MapDiv:%f|MiniMapPos.x:%f|MiniMapPos.y:%f"), MapDiv, MiniMapPos.x, MiniMapPos.y);
		//	if (OverlayEngine::WasKeyPressed(VK_F2))
		//		MapDiv -= 1.f;
		//	if (OverlayEngine::WasKeyPressed(VK_F3))
		//		MapDiv += 1.f;
		//	if (OverlayEngine::IsKeyDown(VK_F4))
		//		MiniMapPos.x -= 0.00005f;
		//	if (OverlayEngine::IsKeyDown(VK_F5))
		//		MiniMapPos.x += 0.00005f;
		//	if (OverlayEngine::IsKeyDown(VK_F6))
		//		MiniMapPos.y -= 0.00005f;
		//	if (OverlayEngine::IsKeyDown(VK_F7))
		//		MiniMapPos.y += 0.00005f;
		//}
		////else
		////{
		////	MiniMapPos = GetMapRaderCenter();
		////	MapDiv = GetMapDiv();
		////}
		float X = round(deltaFromLocal.X / 20000.f * MapDiv) + viewSize.X * MiniMapPos.x;
		float Y = round(deltaFromLocal.Y / 20000.f * MapDiv) + viewSize.Y * MiniMapPos.y;

		float MyX = viewSize.X * MiniMapPos.x;
		float MyY = viewSize.Y * MiniMapPos.y;
		//Drawing::DrawCircleFilled(ImVec2(MyX, MyY), 9.f, Settings::ColorGreen);
		//Drawing::DrawCircle(ImVec2(MyX, MyY), 10.f, IM_COL32(0, 0, 0, 255));
		float FovBegin = LastAimOffset.Yaw - 10.f;
		if (FovBegin < 0.f)
			FovBegin += 360.f;
		if (FovBegin > 360.f)
			FovBegin -= 360.f;
		float FovEnd = LastAimOffset.Yaw + 10.f;
		if (FovEnd < 0.f)
			FovEnd += 360.f;
		if (FovEnd > 360.f)
			FovEnd -= 360.f;
		ImVec2 LineBegin = GetViewPos(ImVec2(X, Y), FovBegin, 200.f);
		ImVec2 LineEnd = GetViewPos(ImVec2(X, Y), FovEnd, 200.f);
		ImVec2 A = ImVec2(X, Y), B = LineBegin, C = LineEnd, P = ImVec2(MyX, MyY);
		double angle1 = get_angle(C, A, B);
		double angle2 = get_angle(A, B, C);
		double angle3 = get_angle(B, C, A);
		double cangle1 = get_angle(P, A, C);
		double cangle2 = get_angle(P, A, B);
		double cangle3 = get_angle(P, B, A);
		double cangle4 = get_angle(P, B, C);
		double cangle5 = get_angle(P, C, A);
		double cangle6 = get_angle(P, C, B);
		if (round(cangle1 + cangle2 + cangle3 + cangle4 + cangle5 + cangle6) != round(angle1 + angle2 + angle3))
			IsSee = false;
		else
			IsSee = true;

		switch (Drawtype)
		{
		case 1:
		{
			auto TeamNumber = TeamNum;
			if (TeamNumber >= 100000)
				TeamNumber -= 100000;
			auto TO = TeamNumber + ColorOffset;
			if (TO >= 100)
				TO -= 100;
			if (abs(TO) >= 100000)
				TO = 0;
			float a = ((ImU32)TEAM_COLOR[TO] >> 24) & 0xff;
			float r = ((ImU32)TEAM_COLOR[TO] >> 16) & 0xff;
			float g = ((ImU32)TEAM_COLOR[TO] >> 8) & 0xff;
			float b = ((ImU32)TEAM_COLOR[TO]) & 0xff;

			Drawing::DrawCircleFilled(ImVec2(X, Y), Vars.Misc.RadarRadius, ImGui::FloatArrayGetColorU32(ImVec4(r / 255.f, g / 255.f, b / 255.f, 0.6f)), 50);
			Drawing::DrawCircle(ImVec2(X, Y), Vars.Misc.RadarRadius, 0xFF000000, 50);
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 16.f + (Vars.Misc.RadarRadius - 10.f), 0, ImVec2(X, Y), Settings::ColorWhite, IM_COL32(0, 0, 0, 255), FONT_CENTER, "%d", TeamNumber);
			float ts = LastAimOffset.Yaw;
			if (ts < 0.f)
				ts += 360.f;
			if (ts > 360.f)
				ts -= 360.f;
			ImVec2 ViewPortBegin = GetViewPos(ImVec2(X, Y), ts, Vars.Misc.RadarRadius + 3.5f);
			ImVec2 ViewPortEnd = GetViewPos(ImVec2(X, Y), ts, Vars.Misc.RadarRadius + 13.5f);

			ImVec2 Pos1 = ViewPortEnd;

			ImVec2 Pos = ImVec2(X, Y);
			ts = LastAimOffset.Yaw - 21.f;
			if (ts < 0.f)
				ts += 360.f;
			if (ts > 360.f)
				ts -= 360.f;
			ViewPortBegin = GetViewPos(ImVec2(Pos.x, Pos.y), ts, Vars.Misc.RadarRadius + 4.5f);
			ImVec2 Pos2 = ViewPortBegin;

			ts = LastAimOffset.Yaw + 21.f;
			if (ts < 0.f)
				ts += 360.f;
			if (ts > 360.f)
				ts -= 360.f;
			ViewPortBegin = GetViewPos(ImVec2(Pos.x, Pos.y), ts, Vars.Misc.RadarRadius + 4.5f);
			ImVec2 Pos3 = ViewPortBegin;
			Drawing::DrawTriangleFilled(Pos1, Pos2, Pos3, 0xFFFFFFFF);
			Drawing::DrawTriangle(Pos1, Pos2, Pos3, 0xFF000000, 2.f);

			//for (int il = 0; il < 40.f; il++)
			//{
			//	ts = LastAimOffset.Yaw - (float)il;
			//	if (ts < 0.f)
			//		ts += 360.f;
			//	if (ts > 360.f)
			//		ts -= 360.f;
			//	ViewPortEnd = GetViewPos(ImVec2(X, Y), ts, Vars.Misc.RadarRadius + 4.f);
			//	Drawing::DrawLine(ViewPortEnd, ViewPortEnd + ImVec2(.5f, .5f), 0xFFFFFFFF, 2);
			//	ts = LastAimOffset.Yaw + (float)il;
			//	if (ts < 0.f)
			//		ts += 360.f;
			//	if (ts > 360.f)
			//		ts -= 360.f;
			//	ViewPortEnd = GetViewPos(ImVec2(X, Y), ts, Vars.Misc.RadarRadius + 4.f);
			//	Drawing::DrawLine(ViewPortEnd, ViewPortEnd + ImVec2(.5f, .5f), 0xFFFFFFFF, 2);
			//}
			break;
		}
		case 2:
		{
			if (Vars.Misc.VehicleMap)
				Drawing::DrawImage(m_HealthBar->m_MapVehicle, ImVec2(X - 16.f * 0.8f, Y - 11.2f * 0.8f), ImVec2(X + 16.f * 0.8f, Y + 11.2f * 0.8f), ImVec2(0, 0), ImVec2(1, 1), 0xFF73B0FA);
			return true;
		}
		case 3:
		{
			float sizeToDrawX = 0, sizeToDrawY = 0;
			if (!RaderType)
			{
				sizeToDrawX = 26.f;
				sizeToDrawY = 24.7f;
			}
			else
			{
				sizeToDrawX = 28.f;
				sizeToDrawY = 26.6f;
			}
			if (Vars.Misc.AirDropsMap)
				Drawing::DrawImage(m_HealthBar->m_MapCarePackage, ImVec2(X - sizeToDrawX * 0.8f / 2.f, Y - sizeToDrawY * 0.8f / 2.f), ImVec2(X + sizeToDrawX * 0.8f / 2.f, Y + sizeToDrawY * 0.8f / 2.f));
			return true;
		}
		case 4:
		{
			if (Vars.Misc.VehicleMap)
				Drawing::DrawImage(m_HealthBar->m_MapBoat, ImVec2(X - 20.f * 0.8f, Y - 6.5f * 0.8f), ImVec2(X + 20.f * 0.8f, Y + 6.5f * 0.8f), ImVec2(0, 0), ImVec2(1, 1), 0xFF73B0FA);
			return true;
		}
		default:
			break;
		}


		return IsSee;
	}

	bool Visuals::MapRadar2(FVector PlayerLocation, float  ObjectFov, int TeamNum, int Type, ImTextureID Textur, ImVec2 uv0, ImVec2 uv1, std::string PlayerName)
	{
		static std::map<hash_t, float> WorldMapSize = {
			{hash_("Tiger_Main"), 408000.f},//泰戈
			{hash_("Desert_Main"), 408000.f},//米拉玛
			{hash_("Kiki_Main"), 408000.f},//帝斯顿 Heaven_Main
			{hash_("Range_Main"), 101175.0f},//训练场
			{hash_("Summerland_Main"), 101175.f},//卡拉金 Summerland_Main
			{hash_("Baltic_Main"), 406372.f},//艾伦格
			{hash_("Savage_Main"), 202387.5f},//萨诺
			{hash_("DihorOtok_Main"), 408000.f},//维寒迪 雪地 DihorOtok_Main
			{hash_("Chimera_Main"), 153003.f},//帕拉莫 Chimera_Main
			{hash_("Neon_Main"), 408000.f},//荣都 Neon_Main
			{hash_("Heaven_Main"), 101175.f}//褐湾 Heaven_Main
		};
		if (Vars.Misc.MRadar)
		{
			ptr_t UWorldClass = sdk::GetWorld();
			if (IsAddrValid(UWorldClass))
			{
				float ObjFov = ObjectFov;
				auto viewSize = Drawing::GetViewPort();
				auto Obj = sdk::m_HudBlockInputArray;
				if (UBlockInputUserWidget(Obj.obj)->IsValid())
				{
					UTslNewWorldMapWidget WorldMapWidget = UBlockInputUserWidget(Obj.obj)->Cast<UTslNewWorldMapWidget>();
					if (!WorldMapWidget.IsValid())
						return false;
					auto bWorldMap = Obj.bWorldMap;
					if (!bWorldMap)
						return false;

					auto WorldMapZoomFactor = Obj.WorldMapZoomFactor;
					auto WorldMapPosition = Obj.WorldMapPosition;

					int WorldLocationX = Obj.WorldLocationX;
					int WorldLocationY = Obj.WorldLocationY;

					float MapSize = WorldMapSize[hash_(Obj.WorldName)];
					//static float MainSize = 408000.0f;
					//if (OverlayEngine::IsKeyDown(VK_UP))
					//{
					//	MainSize += 1.f;
					//	CONSOLE_INFO("%f", MainSize);

					//}
					//if (OverlayEngine::IsKeyDown(VK_DOWN))
					//{
					//	MainSize -= 1.f;
					//	CONSOLE_INFO("%f", MainSize);
					//}
					//MapSize = MainSize;
					const float MapSizeFactored = MapSize / WorldMapZoomFactor;
					const FVector WorldLocation = PlayerLocation + FVector(WorldLocationX, WorldLocationY, 0);
					const FVector WorldCenterLocation = { MapSize * (1.0f + WorldMapPosition.X), MapSize * (1.0f + WorldMapPosition.Y), 0.0f };
					const FVector RadarPos = WorldLocation - WorldCenterLocation;
					const FVector RadarScreenCenterPos = { viewSize.X / 2.0f, viewSize.Y / 2.0f, 0.0f };
					const FVector RadarScreenPos = RadarScreenCenterPos + FVector{
						RadarPos.X / MapSizeFactored * viewSize.Y / 2.0f,
						RadarPos.Y / MapSizeFactored * viewSize.Y / 2.0f,
						0.0f
					};
					ULONG TeamID = TeamNum;
					if (TeamID >= 100000)
						TeamID -= 100000;
					auto TO = TeamID + ColorOffset;
					if (TO >= 100)
						TO -= 100;
					auto Radians = [=](double Angle)
						{
							return (Angle * 0.01745329251994f);
						};
					//if (OverlayEngine::IsKeyDown(VK_INSERT))
					//{
					//	CONSOLE_INFO("%f", WorldMapZoomFactor);
					//	CONSOLE_INFO("%s", UWorldClass.GetName().c_str());
					//}
					float X = RadarScreenPos.X;
					float Y = RadarScreenPos.Y;
					switch (Type)
					{
					case 1:
					{
						float a = ((ImU32)TEAM_COLOR[TO] >> 24) & 0xff;
						float r = ((ImU32)TEAM_COLOR[TO] >> 16) & 0xff;
						float g = ((ImU32)TEAM_COLOR[TO] >> 8) & 0xff;
						float b = ((ImU32)TEAM_COLOR[TO]) & 0xff;
						Drawing::DrawCircleFilled(ImVec2(X, Y), Vars.Misc.MRadarRadius, ImGui::FloatArrayGetColorU32(ImVec4(r / 255.f, g / 255.f, b / 255.f, 0.6f)), 50);
						Drawing::DrawCircle(ImVec2(X, Y), Vars.Misc.MRadarRadius, 0xFF000000, 50);
						Drawing::DrawTextOutline(OverlayEngine::Font20px, 16.f + (Vars.Misc.MRadarRadius - 10), 0, ImVec2(X, Y), Settings::ColorWhite, IM_COL32(0, 0, 0, 255), FONT_CENTER, "%d", TeamID);

						if (Vars.Misc.NameMap)
						{
							auto Fontsize = Drawing::CalcTextSize(OverlayEngine::Font20px, 16.f + (Vars.Misc.MRadarRadius - 10), 0, ImVec2(X, Y + 22), Settings::ColorWhite, IM_COL32(0, 0, 0, 255), FONT_CENTER, PlayerName.c_str());
							Drawing::DrawRectFilled(ImVec2(X - Fontsize.x / 2.f - 2.f, Y + 14), ImVec2(X + Fontsize.x / 2.f + 2.f, Y + 10 + Fontsize.y + 4.f), 0x5d000000);
							Drawing::DrawTextOutline(OverlayEngine::Font20px, 16.f + (Vars.Misc.MRadarRadius - 10), 0, ImVec2(X, Y + 22), Settings::ColorWhite, IM_COL32(0, 0, 0, 255), FONT_CENTER, PlayerName.c_str());
						}
						float ts = ObjectFov;
						if (ts < 0.f)
							ts += 360.f;
						if (ts > 360.f)
							ts -= 360.f;
						ImVec2 ViewPortBegin = GetViewPos(ImVec2(X, Y), ts, Vars.Misc.MRadarRadius + 3.5f);
						ImVec2 ViewPortEnd = GetViewPos(ImVec2(X, Y), ts, Vars.Misc.MRadarRadius + 13.5f);

						ImVec2 Pos1 = ViewPortEnd;

						ImVec2 Pos = ImVec2(X, Y);
						ts = ObjectFov - 21.f;
						if (ts < 0.f)
							ts += 360.f;
						if (ts > 360.f)
							ts -= 360.f;
						ViewPortBegin = GetViewPos(ImVec2(Pos.x, Pos.y), ts, Vars.Misc.MRadarRadius + 4.5f);
						ImVec2 Pos2 = ViewPortBegin;

						ts = ObjectFov + 21.f;
						if (ts < 0.f)
							ts += 360.f;
						if (ts > 360.f)
							ts -= 360.f;
						ViewPortBegin = GetViewPos(ImVec2(Pos.x, Pos.y), ts, Vars.Misc.MRadarRadius + 4.5f);
						ImVec2 Pos3 = ViewPortBegin;
						Drawing::DrawTriangleFilled(Pos1, Pos2, Pos3, 0xFFFFFFFF);
						Drawing::DrawTriangle(Pos1, Pos2, Pos3, 0xFF000000, 2.f);
						for (int il = 0; il < 40.f; il++)
						{
							ts = ObjectFov - (float)il;
							if (ts < 0.f)
								ts += 360.f;
							if (ts > 360.f)
								ts -= 360.f;
							ViewPortEnd = GetViewPos(ImVec2(X, Y), ts, Vars.Misc.MRadarRadius + 4.f);
							Drawing::DrawLine(ViewPortEnd, ViewPortEnd + ImVec2(.5f, .5f), 0xFFFFFFFF, 2);

							ts = ObjectFov + (float)il;
							if (ts < 0.f)
								ts += 360.f;
							if (ts > 360.f)
								ts -= 360.f;
							ViewPortEnd = GetViewPos(ImVec2(X, Y), ts, Vars.Misc.MRadarRadius + 4.f);
							Drawing::DrawLine(ViewPortEnd, ViewPortEnd + ImVec2(.5f, .5f), 0xFFFFFFFF, 2);
						}

						return true;
					}
					case 2:
					{
						if (Vars.Misc.VehicleMap)
							Drawing::DrawImage(m_HealthBar->m_MapVehicle, ImVec2(X - 16.f * 0.8f, Y - 11.2f * 0.8f), ImVec2(X + 16.f * 0.8f, Y + 11.2f * 0.8f));
						return true;
					}
					case 3:
					{
						auto LevelMap = (int)round(WorldMapZoomFactor);
						float sizeToDrawX = 0, sizeToDrawY = 0;
						if (LevelMap >= 1 && LevelMap < 2)
						{
							sizeToDrawX = 26.f;
							sizeToDrawY = 24.7f;
						}
						else if (LevelMap >= 2 && LevelMap < 4)
						{
							sizeToDrawX = 28.f;
							sizeToDrawY = 26.6f;
						}
						else if (LevelMap >= 4 && LevelMap < 8)
						{
							sizeToDrawX = 30.f;
							sizeToDrawY = 28.5f;
						}
						else if (LevelMap >= 8 && LevelMap < 16)
						{
							sizeToDrawX = 32.f;
							sizeToDrawY = 30.4f;
						}
						else
						{
							sizeToDrawX = 34.f;
							sizeToDrawY = 32.3f;
						}
						if (Vars.Misc.AirDropsMap)
							Drawing::DrawImage(m_HealthBar->m_MapCarePackage, ImVec2(X - sizeToDrawX * 0.8f / 2.f, Y - sizeToDrawY * 0.8f / 2.f), ImVec2(X + sizeToDrawX * 0.8f / 2.f, Y + sizeToDrawY * 0.8f / 2.f));
						return true;
					}
					case 4:
					{
						if (Vars.Misc.VehicleMap)
							Drawing::DrawImage(m_HealthBar->m_MapBoat, ImVec2(X - 20.f * 0.8f, Y - 6.5f * 0.8f), ImVec2(X + 20.f * 0.8f, Y + 6.5f * 0.8f));
						return true;
					}
					default:
						break;
					}
				}
			}
		}
		return false;
	}

	void Visuals::DrawScopeBoard()
	{
		static bool bIsTransparent = false;
		ImGuiStyle& style = ImGui::GetStyle();
		auto BackupRound = style.WindowRounding;
		auto BackupScrollbarSize = style.ScrollbarSize;
		style.WindowRounding = 5.f;
		style.ScrollbarSize = 10.f;
		static bool setOnce = false;
		static bool ShowScore = true;
		const ImVec2 ClientRect = Drawing::GetViewPort() / 2.f;
		ImVec2 WindowSize = ImVec2(570, ClientRect.y * 2.f * 0.15f);
		if (!setOnce)
		{
			ImGui::SetNextWindowSize(WindowSize);
			ImGui::SetNextWindowPos(ImVec2(ClientRect.x * 2.f - WindowSize.x - 20.f, ClientRect.y * 2.f * 0.1f));
			setOnce = true;
		}
		ImGui::SetNextWindowBgAlpha(0.f);
		if (!Vars.Misc.DisplayMode)
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.64f, 0.64f, 0.64f, 1.f));
		else
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 1.f));


		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
		ImGui::Begin(("Scoreboard"), &ShowScore, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		{
			auto WindowPos = ImGui::GetNextWindowPos();
			WindowSize = ImGui::GetWindowSize();

			BMenuPos = WindowPos;
			BMenuSize = WindowSize;
			if (MousePos.x >= WindowPos.x && MousePos.y >= WindowPos.y && MousePos.x <= WindowPos.x + WindowSize.x && MousePos.y <= WindowPos.y + WindowSize.y)
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

			//580 32
			//ImGui::GetCurrentWindow()->DrawList->AddRectFilled(WindowPos, ImVec2(WindowPos.x + 650.f, WindowPos.y + 35.f), 0x6D545454, 5);
			ImGui::PushFont(OverlayEngine::Font16px);
			ImGui::Columns(8, ("Scoreboardcolumns:"), false);
			ImGui::SetColumnOffset(1, 45);
			ImGui::SetColumnOffset(2, 170);
			ImGui::SetColumnOffset(3, 230);
			ImGui::SetColumnOffset(4, 280);
			ImGui::SetColumnOffset(5, 340);
			ImGui::SetColumnOffset(6, 400);
			ImGui::SetColumnOffset(7, 470);
			ImGui::SetColumnOffset(8, 650);
			ImGui::PushFont(OverlayEngine::Font30px);
			ImGui::Text(LOCALIZE_Visual(" Team", u8" 队伍")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("     Name", u8"    名称")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("Rank", u8"段位")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("RP", u8"等级")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("Kill", u8"击杀")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("Assist", u8"  助攻")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("KDA", u8"KDA")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("Opera", u8"操作")); ImGui::NextColumn();
			ImGui::PopFont();
			ImGui::Separator();
			static int selected = -1;
			for (int i = 0; i < m_ScoreBoard.size(); i++)
			{
				ImGui::Text(("         %d"), m_ScoreBoard[i].TeamNumber); ImGui::NextColumn();
				if (m_ScoreBoard[i].Partner)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.843f, 0, 1));
				if (m_ScoreBoard[i].bFog)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0, 1));
				ImGui::Text(LOCALIZE_Visual("          %s", u8"        %s"), m_ScoreBoard[i].PlayerName.c_str()); ImGui::NextColumn();
				if (m_ScoreBoard[i].bFog)
					ImGui::PopStyleColor();
				if (m_ScoreBoard[i].Partner)
					ImGui::PopStyleColor();
				ImGui::Text(LOCALIZE_Visual("     %d", u8"   %d"), m_ScoreBoard[i].Ranking); ImGui::NextColumn();
				ImGui::Text(LOCALIZE_Visual(" %d", u8"    %d"), m_ScoreBoard[i].Level); ImGui::NextColumn();
				if (m_ScoreBoard[i].Kill >= 3)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
				if (m_ScoreBoard[i].Damage == 0)
					ImGui::Text(LOCALIZE_Visual("  %d", "     %d"), m_ScoreBoard[i].Kill);
				else
					ImGui::Text(LOCALIZE_Visual("  %d  -  %.0f", "     %d  -  %.0f"), m_ScoreBoard[i].Kill, m_ScoreBoard[i].Damage);
				ImGui::NextColumn();
				if (m_ScoreBoard[i].Kill >= 3)
					ImGui::PopStyleColor(1);
				ImGui::Text(("          %d"), m_ScoreBoard[i].Assist); ImGui::NextColumn();
				ImVec4 KDColor = ImVec4(0.f, 0.f, 0.f, 1.f);
				if (m_ScoreBoard[i].KDA <= 1.f)
					KDColor = ImVec4(1.f, 1.f, 1.f, 1.f);
				else if (m_ScoreBoard[i].KDA <= 2.f)
					KDColor = ImVec4(0.f, 1.f, 0.f, 1.f);
				else
					KDColor = ImVec4(1.f, 1.f, 0.f, 1.f);
				ImGui::PushStyleColor(ImGuiCol_Text, KDColor);
				ImGui::Text(("     %.1f"), m_ScoreBoard[i].KDA); ImGui::NextColumn();

				auto FindState = BalltModeStateMap.find(hash_(m_ScoreBoard[i].PlayerName));
				if (FindState == BalltModeStateMap.end())
				{
					BalltModeStateMap.emplace(hash_(m_ScoreBoard[i].PlayerName), 0);
					FindState = BalltModeStateMap.find(hash_(m_ScoreBoard[i].PlayerName));
					if (FindState == BalltModeStateMap.end())
						continue;
				}
				if (CheckPlayerIsInBlackListMap(hash_(m_ScoreBoard[i].PlayerName)))
				{
					if (FindState->second != 1)
						FindState->second = 1;
				}
				if (m_ScoreBoard[i].TeamName != "")
				{
					if (CheckPlayerIsInBlackListMap(hash_(m_ScoreBoard[i].TeamName)))
					{
						if (FindState->second != 2)
							FindState->second = 2;
					}
				}
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
				if (FindState->second == 0)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
				else if (FindState->second == 3)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
				else
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
				std::string FormatString;
				if (FindState->second == 0)
					FormatString = Utils::FormatString(LOCALIZE_Visual("Default##%d", u8"未设定##%d"), i).c_str();
				else if (FindState->second == 1)
				{
					if (BlackListUpLoad && m_ScoreBoard[i].PlayerName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("UpLoad##%d", u8"上传中##%d"), i);
					else
						FormatString = Utils::FormatString(LOCALIZE_Visual("Blacklist##%d", u8"黑名单##%d"), i);
				}
				else if (FindState->second == 2)
				{
					if (BlackListRemove && m_ScoreBoard[i].PlayerName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("Removing##%d", u8"移除中##%d"), i);
					else if (BlackListUpLoad && m_ScoreBoard[i].TeamName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("UpLoadTeam##%d", u8"上传战队中##%d"), i);
					else
						FormatString = Utils::FormatString(LOCALIZE_Visual("BlacklistTeam##%d", u8"战队黑名单##%d"), i);

				}
				else if (FindState->second == 3)
				{
					if (BlackListRemove && m_ScoreBoard[i].PlayerName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("Removing##%d", u8"移除中##%d"), i);
					if (BlackListRemove && m_ScoreBoard[i].TeamName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("RemovingTeam##%d", u8"移除战队中##%d"), i);
					else
						FormatString = Utils::FormatString(LOCALIZE_Visual("Whitelist##%d", u8"白名单##%d"), i);
				}
				auto UpdateName = WaitUpload.find(m_ScoreBoard[i].PlayerName);
				auto UpdateTeam = WaitUpload.find(m_ScoreBoard[i].TeamName);
				if (ImGui::Button(FormatString.c_str(), ImVec2(70, 20)) || UpdateName != WaitUpload.end() || UpdateTeam != WaitUpload.end())
				{
					if (!BlackListUpLoad && !BlackListRemove)
					{
						if (UpdateName == WaitUpload.end() && UpdateTeam == WaitUpload.end())
						{
							if (FindState->second < 3)
							{
								FindState->second++;
								if (FindState->second == 2 && m_ScoreBoard[i].TeamName == "")
									FindState->second++;
							}
							else
								FindState->second = 0;
						}
						switch (FindState->second)
						{
						case 0:
						{
							if (CheckPlayerIsInWhiteListMap(hash_(m_ScoreBoard[i].PlayerName)))
							{
								EraseWhiteListMap(hash_(m_ScoreBoard[i].PlayerName));
							}
							break;
						}
						case 1:
						{
							if (UpdateTeam != WaitUpload.end())
								WaitUpload.erase(UpdateTeam);
							if (UpdateName != WaitUpload.end())
								WaitUpload.erase(UpdateName);
							AddBlackListMap(hash_(m_ScoreBoard[i].PlayerName), std::make_shared<int>(1));
							BlackListName = m_ScoreBoard[i].PlayerName;
							BlackListUpLoad = true;
							break;
						}
						case 2:
						{
							if (CheckPlayerIsInBlackListMap(hash_(m_ScoreBoard[i].PlayerName)))
							{
								EraseBlackListMap(hash_(m_ScoreBoard[i].PlayerName));
								BlackListName = m_ScoreBoard[i].PlayerName;
								BlackListRemove = true;
							}
							if (!BlackListRemove)
							{
								if (UpdateTeam != WaitUpload.end())
									WaitUpload.erase(UpdateTeam);
								AddBlackListMap(hash_(m_ScoreBoard[i].TeamName), std::make_shared<int>(1));
								BlackListName = m_ScoreBoard[i].TeamName;
								BlackListUpLoad = true;
							}
							else
							{
								WaitUpload.emplace(m_ScoreBoard[i].TeamName, 1);
							}
							break;
						}
						case 3:
						{
							if (CheckPlayerIsInBlackListMap(hash_(m_ScoreBoard[i].TeamName == "" ? m_ScoreBoard[i].PlayerName : m_ScoreBoard[i].TeamName)))
							{
								EraseBlackListMap(hash_(m_ScoreBoard[i].TeamName == "" ? m_ScoreBoard[i].PlayerName : m_ScoreBoard[i].TeamName));
								BlackListName = m_ScoreBoard[i].TeamName == "" ? m_ScoreBoard[i].PlayerName : m_ScoreBoard[i].TeamName;
								BlackListRemove = true;
							}
							if (!BlackListRemove)
							{
								if (UpdateName != WaitUpload.end())
									WaitUpload.erase(UpdateName);
								if (!CheckPlayerIsInWhiteListMap(hash_(m_ScoreBoard[i].PlayerName)))
								{
									AddWhiteListMap(hash_(m_ScoreBoard[i].PlayerName), std::make_shared<int>(1));
								}
							}
							else
							{
								WaitUpload.emplace(m_ScoreBoard[i].PlayerName, 1);
							}
							break;
						}

						default:
							break;
						}
					}
				}
				ImGui::PopStyleColor(4);
				ImGui::NextColumn();

				ImGui::PopStyleColor();
				ImGui::Separator();


			}
			ImGui::Columns(1);
			ImGui::PopFont();

		}
		ImGui::End();
		ImGui::PopStyleColor(2);


		style.WindowRounding = BackupRound;
		style.ScrollbarSize = BackupScrollbarSize;

	}

	void Visuals::DrawScopeBoard2()
	{
		static bool bIsTransparent = false;
		ImGuiStyle& style = ImGui::GetStyle();
		auto BackupRound = style.WindowRounding;
		auto BackupScrollbarSize = style.ScrollbarSize;
		style.WindowRounding = 5.f;
		style.ScrollbarSize = 10.f;
		static bool setOnce = false;
		static bool ShowScore = true;
		const ImVec2 ClientRect = Drawing::GetViewPort() / 2.f;
		ImVec2 WindowSize = ImVec2(570, ClientRect.y * 2.f * 0.4f);
		if (!setOnce)
		{
			ImGui::SetNextWindowSize(WindowSize);
			ImGui::SetNextWindowPos(ImVec2(ClientRect.x - WindowSize.x / 2.f, ClientRect.y - WindowSize.y / 2.f));
			setOnce = true;
		}
		ImGui::SetNextWindowBgAlpha(0.f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.5f, 0.5f, 0.5f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
		ImGui::Begin(("Scoreboard2"), &ShowScore, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		{
			auto WindowPos = ImGui::GetNextWindowPos();
			WindowSize = ImGui::GetWindowSize();

			BMenuPos = WindowPos;
			BMenuSize = WindowSize;
			if (MousePos.x >= WindowPos.x && MousePos.y >= WindowPos.y && MousePos.x <= WindowPos.x + WindowSize.x && MousePos.y <= WindowPos.y + WindowSize.y)
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

			ImGui::PushFont(OverlayEngine::Font16px);
			ImGui::Columns(8, ("Scoreboardcolumns2:"), false);
			ImGui::SetColumnOffset(1, 45);
			ImGui::SetColumnOffset(2, 170);
			ImGui::SetColumnOffset(3, 230);
			ImGui::SetColumnOffset(4, 280);
			ImGui::SetColumnOffset(5, 340);
			ImGui::SetColumnOffset(6, 400);
			ImGui::SetColumnOffset(7, 470);
			ImGui::SetColumnOffset(8, 650);
			ImGui::PushFont(OverlayEngine::Font30px);
			ImGui::Text(LOCALIZE_Visual(" Team", u8" 隊伍")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("     Name", u8"    名稱")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("Rank", u8"段位")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("RP", u8"等級")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("Avg", u8"場傷")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("KDA", u8"KDA")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("Partner", u8"合作者")); ImGui::NextColumn();
			ImGui::Text(LOCALIZE_Visual("Opera", u8"操作")); ImGui::NextColumn();
			ImGui::PopFont();
			ImGui::Separator();
			static int selected = -1;
			for (int i = 0; i < m_ScoreBoard2.size(); i++)
			{
				ImGui::Text(("         %d"), m_ScoreBoard2[i].TeamNumber); ImGui::NextColumn();
				if (m_ScoreBoard2[i].Partner)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.843f, 0, 1));
				ImGui::Text(LOCALIZE_Visual("          %s", u8"        %s"), m_ScoreBoard2[i].PlayerName.c_str()); ImGui::NextColumn();
				if (m_ScoreBoard2[i].Partner)
					ImGui::PopStyleColor();
				ImGui::Text(LOCALIZE_Visual("     %d", u8"   %d"), m_ScoreBoard2[i].Ranking); ImGui::NextColumn();
				ImGui::Text(LOCALIZE_Visual(" %d", u8"    %d"), m_ScoreBoard2[i].Level); ImGui::NextColumn();
				ImGui::Text(LOCALIZE_Visual("  %.0f", "     %.0f"), m_ScoreBoard2[i].AvgDamage); ImGui::NextColumn();

				ImVec4 KDColor = ImVec4(0.f, 0.f, 0.f, 1.f);
				if (m_ScoreBoard2[i].KDA <= 1.f)
					KDColor = ImVec4(1.f, 1.f, 1.f, 1.f);
				else if (m_ScoreBoard2[i].KDA <= 2.f)
					KDColor = ImVec4(0.f, 1.f, 0.f, 1.f);
				else
					KDColor = ImVec4(1.f, 1.f, 0.f, 1.f);
				ImGui::Text(("     %.1f"), m_ScoreBoard2[i].KDA); ImGui::NextColumn();

				ImGui::Text(("        %s"), m_ScoreBoard2[i].Partner ? LOCALIZE_Visual("Yes", u8"是") : LOCALIZE_Visual("No", u8"否")); ImGui::NextColumn();

				auto FindState = BalltModeStateMap.find(hash_(m_ScoreBoard2[i].PlayerName));
				if (FindState == BalltModeStateMap.end())
				{
					BalltModeStateMap.emplace(hash_(m_ScoreBoard2[i].PlayerName), 0);
					FindState = BalltModeStateMap.find(hash_(m_ScoreBoard2[i].PlayerName));
					if (FindState == BalltModeStateMap.end())
						continue;
				}
				if (CheckPlayerIsInBlackListMap(hash_(m_ScoreBoard2[i].PlayerName)))
				{
					if (FindState->second != 1)
						FindState->second = 1;
				}
				if (m_ScoreBoard2[i].TeamName != "")
				{
					if (CheckPlayerIsInBlackListMap(hash_(m_ScoreBoard2[i].TeamName)))
					{
						if (FindState->second != 2)
							FindState->second = 2;
					}
				}
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
				if (FindState->second == 0)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
				else if (FindState->second == 3)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
				else
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
				std::string FormatString;
				if (FindState->second == 0)
					FormatString = Utils::FormatString(LOCALIZE_Visual("Default##%d", u8"未設定##%d"), i).c_str();
				else if (FindState->second == 1)
				{
					if (BlackListUpLoad && m_ScoreBoard2[i].PlayerName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("UpLoad##%d", u8"上傳中##%d"), i);
					else
						FormatString = Utils::FormatString(LOCALIZE_Visual("Blacklist##%d", u8"黑名單##%d"), i);
				}
				else if (FindState->second == 2)
				{
					if (BlackListRemove && m_ScoreBoard2[i].PlayerName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("Removing##%d", u8"移除中##%d"), i);
					else if (BlackListUpLoad && m_ScoreBoard2[i].TeamName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("UpLoadTeam##%d", u8"上傳戰隊中##%d"), i);
					else
						FormatString = Utils::FormatString(LOCALIZE_Visual("BlacklistTeam##%d", u8"戰隊黑名單##%d"), i);

				}
				else if (FindState->second == 3)
				{
					if (BlackListRemove && m_ScoreBoard2[i].PlayerName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("Removing##%d", u8"移除中##%d"), i);
					if (BlackListRemove && m_ScoreBoard2[i].TeamName == BlackListName)
						FormatString = Utils::FormatString(LOCALIZE_Visual("RemovingTeam##%d", u8"移除戰隊中##%d"), i);
					else
						FormatString = Utils::FormatString(LOCALIZE_Visual("Whitelist##%d", u8"白名單##%d"), i);
				}
				auto UpdateName = WaitUpload.find(m_ScoreBoard2[i].PlayerName);
				auto UpdateTeam = WaitUpload.find(m_ScoreBoard2[i].TeamName);
				if (ImGui::Button(FormatString.c_str(), ImVec2(70, 20)) || UpdateName != WaitUpload.end() || UpdateTeam != WaitUpload.end())
				{
					if (!BlackListUpLoad && !BlackListRemove)
					{
						if (UpdateName == WaitUpload.end() && UpdateTeam == WaitUpload.end())
						{
							if (FindState->second < 3)
							{
								FindState->second++;
								if (FindState->second == 2 && m_ScoreBoard2[i].TeamName == "")
									FindState->second++;
							}
							else
								FindState->second = 0;
						}
						switch (FindState->second)
						{
						case 0:
						{
							if (CheckPlayerIsInWhiteListMap(hash_(m_ScoreBoard2[i].PlayerName)))
							{
								EraseWhiteListMap(hash_(m_ScoreBoard2[i].PlayerName));
							}
							break;
						}
						case 1:
						{
							if (UpdateTeam != WaitUpload.end())
								WaitUpload.erase(UpdateTeam);
							if (UpdateName != WaitUpload.end())
								WaitUpload.erase(UpdateName);
							AddBlackListMap(hash_(m_ScoreBoard2[i].PlayerName), std::make_shared<int>(1));
							BlackListName = m_ScoreBoard2[i].PlayerName;
							BlackListUpLoad = true;
							break;
						}
						case 2:
						{
							if (CheckPlayerIsInBlackListMap(hash_(m_ScoreBoard2[i].PlayerName)))
							{
								EraseBlackListMap(hash_(m_ScoreBoard2[i].PlayerName));
								BlackListName = m_ScoreBoard2[i].PlayerName;
								BlackListRemove = true;
							}
							if (!BlackListRemove)
							{
								if (UpdateTeam != WaitUpload.end())
									WaitUpload.erase(UpdateTeam);
								AddBlackListMap(hash_(m_ScoreBoard2[i].TeamName), std::make_shared<int>(1));
								BlackListName = m_ScoreBoard2[i].TeamName;
								BlackListUpLoad = true;
							}
							else
							{
								WaitUpload.emplace(m_ScoreBoard2[i].TeamName, 1);
							}
							break;
						}
						case 3:
						{
							if (CheckPlayerIsInBlackListMap(hash_(m_ScoreBoard2[i].TeamName == "" ? m_ScoreBoard2[i].PlayerName : m_ScoreBoard2[i].TeamName)))
							{
								EraseBlackListMap(hash_(m_ScoreBoard2[i].TeamName == "" ? m_ScoreBoard2[i].PlayerName : m_ScoreBoard2[i].TeamName));
								BlackListName = m_ScoreBoard2[i].TeamName == "" ? m_ScoreBoard2[i].PlayerName : m_ScoreBoard2[i].TeamName;
								BlackListRemove = true;
							}
							if (!BlackListRemove)
							{
								if (UpdateName != WaitUpload.end())
									WaitUpload.erase(UpdateName);
								if (!CheckPlayerIsInWhiteListMap(hash_(m_ScoreBoard2[i].PlayerName)))
								{
									AddWhiteListMap(hash_(m_ScoreBoard2[i].PlayerName), std::make_shared<int>(1));
								}
							}
							else
							{
								WaitUpload.emplace(m_ScoreBoard2[i].PlayerName, 1);
							}
							break;
						}

						default:
							break;
						}
					}
				}
				ImGui::PopStyleColor(3);
				ImGui::NextColumn();

				ImGui::PopStyleColor();
				ImGui::Separator();


			}
			ImGui::Columns(1);
			ImGui::PopFont();

		}
		ImGui::End();
		ImGui::PopStyleColor(2);

		style.WindowRounding = BackupRound;
		style.ScrollbarSize = BackupScrollbarSize;

	}

	bool Visuals::Initialize()
	{
		m_HealthBar = new HealthBar();
		OverlayEngine::OnUpdate += Visuals::Update;
		OverlayEngine::OnPresent += Visuals::BeginDraw;
		OverlayEngine::OnPostPresent += Visuals::EndDraw;
		return VehicleManager::GetManager()->Load() && HandWeaponManager::GetManager()->Load() && m_HealthBar->Load()
#ifndef NoWriteMem
			&& SkinWeaponManager::GetManager()->Load()
#endif
			;
	}

	void Visuals::InitSkinData()
	{
		WeapSkinMap->clear();
		std::unordered_map<hash_t, int>GNameStore;
		//NamesStore::GetSkinName(WeapSkinMap);
		auto start = GetTickCount64();
		CONSOLE_INFO("GetSkinName");

		struct FillName
		{
			int32_t IndexID;
			std::string Name;
			std::string FullName;
			inline FillName(int a, std::string b, std::string c)
			{
				IndexID = a;
				Name = b;
				FullName = c;
			}
		};

		std::unordered_map<std::string, std::vector<FillName>>tMap;

		const std::string itemSkinStr = ("Item_Skin_");
		const std::string weapStr = ("Weap");

		for (int i = 500000; i < 1000000; i++)
		{
			try
			{
				auto Name = std::string(ScatterNameArray[i].buffer);
				//if (Name == "Item_Jacket_17Shou_01_C")
				//{
				//	CONSOLE_INFO("Item_Jacket_17Shou_01_C:%d", i);
				//}
				auto pos = Name.find(itemSkinStr);
				if (pos == 0)
				{
					auto ObjName = Name.substr(10, Name.length() - 12);
					pos = ObjName.find("_");
					if (pos != std::string::npos)
					{
						auto WeapName = ObjName.substr(0, pos);
						WeapName = ("Weap") + WeapName + ("_C");
						tMap[WeapName].push_back(FillName(i, ObjName, Name));
					}
				}
				GNameStore.emplace(hash_(Name), i);
			}
			catch (...)
			{
			}
		}
		CONSOLE_INFO("Cache Time:%d", GetTickCount64() - start);
		start = GetTickCount64();
		CONSOLE_INFO("tMapSize:%d", tMap.size());
		for (auto iter : tMap)
		{
			hash_t SetName = 0;
			std::string OutName = "";
			switch (hash_(iter.first))
			{
			case "WeapBacklv3_C"_hash:
			case "WeapBackLV3_C"_hash:
			case "WeapBackLv3_C"_hash:
			case "WeapBack_C"_hash:
			case "WeapBackLv2_C"_hash:
			case "WeapBacklv2_C"_hash:
			case "WeapBackLV2_C"_hash:
			case "WeapBackLv1_C"_hash:
			case "WeapBacklv1_C"_hash:
			case "WeapBackLV1_C"_hash:
				SetName = hash_("Back");
				OutName = ("Back");
				break;
			case "WeapArmor_C"_hash:
				SetName = hash_("Armor");
				OutName = ("Armor");
				break;
			case "WeapHelmet_C"_hash:
				SetName = hash_("Helmet");
				OutName = ("Helmet");
				break;
			case "WeapDeagle_C"_hash:
			case "WeapDESERTEAGLE_C"_hash:
				SetName = hash_("WeapDesertEagle_C");
				OutName = ("WeapDesertEagle_C");
				break;
			case "WeapG18_C"_hash:
			case "WeapGlock18C_C"_hash:
			case "WeapGLOCK18C_C"_hash:
				SetName = hash_("WeapG18_C");
				OutName = ("WeapG18_C");
				break;
			case "WeapNagant_C"_hash:
			case "WeapNagantM1895_C"_hash:
				SetName = hash_("WeapNagantM1895_C");
				OutName = ("WeapNagantM1895_C");
				break;
			case "WeapSawedOff_C"_hash:
			case "WeapSawedoff_C"_hash:
			case "WeapSawnoff_C"_hash:
				SetName = hash_("WeapSawnoff_C");
				OutName = ("WeapSawnoff_C");
				break;
			case "WeapAK47_C"_hash:
			case "WeapAKM_C"_hash:
				SetName = hash_("WeapAK47_C");
				OutName = ("WeapAK47_C");
				break;
			case "WeapAUG_C"_hash:
			case "WeapAUGA3_C"_hash:
				SetName = hash_("WeapAUG_C");
				OutName = ("WeapAUG_C");
				break;
			case "WeapAWM_C"_hash:
			case "WeapAwm_C"_hash:
				SetName = hash_("WeapAWM_C");
				OutName = ("WeapAWM_C");
				break;
			case "WeapBeraetta686_C"_hash:
			case "WeapBeretta686_C"_hash:
			case "WeapBeretta_C"_hash:
			case "WeapBerreta686_C"_hash:
			case "WeapBeretta_M9_C"_hash:
				SetName = hash_("WeapBerreta686_C");
				OutName = ("WeapBerreta686_C");
				break;
			case "WeapM762_C"_hash:
			case "WeapBerylM762_C"_hash:
				SetName = hash_("WeapBerylM762_C");
				OutName = ("WeapBerylM762_C");
				break;
			case "WeapBizon_C"_hash:
			case "WeapBizonPP19_C"_hash:
				SetName = hash_("WeapBizonPP19_C");
				OutName = ("WeapBizonPP19_C");
				break;
			case "WeapCrossBow_C"_hash:
			case "WeapCrossbow_C"_hash:
				SetName = hash_("WeapCrossbow_1_C");
				OutName = ("WeapCrossbow_1_C");
				break;
			case "WeapFNFAL_C"_hash:
			case "WeapFnFal_C"_hash:
				SetName = hash_("WeapFNFal_C");
				OutName = ("WeapFNFal_C");
				break;
			case "WeapGroza_C"_hash:
			case "WeapGROZA_C"_hash:
				SetName = hash_("WeapGroza_C");
				OutName = ("WeapGroza_C");
				break;
			case "WeapKar98k_C"_hash:
			case "Weapkar98k_C"_hash:
				SetName = hash_("WeapKar98k_C");
				OutName = ("WeapKar98k_C");
				break;
			case "WeapM16_C"_hash:
			case "WeapM16A1_C"_hash:
			case "WeapM16A4_C"_hash:
				SetName = hash_("WeapM16A4_C");
				OutName = ("WeapM16A4_C");
				break;
			case "WeapMK47Mutant_C"_hash:
			case "WeapMutant_C"_hash:
				SetName = hash_("WeapMk47Mutant_C");
				OutName = ("WeapMk47Mutant_C");
				break;
			case "WeapMosin_C"_hash:
				SetName = hash_("WeapMosinNagant_C");
				OutName = ("WeapMosinNagant_C");
				break;
			case "WeapAce32_C"_hash:
			case "WeapGalilAce32_C"_hash:
				SetName = hash_("WeapACE32_C");
				OutName = ("WeapACE32_C");
				break;
			case "WeapSaiga12_C"_hash:
			case "WeapSaiga_C"_hash:
				SetName = hash_("WeapSaiga12_C");
				OutName = ("WeapSaiga12_C");
				break;
			case "WeapScar-L_C"_hash:
			case "WeapSCAR-L_C"_hash:
				SetName = hash_("WeapSCAR-L_C");
				OutName = ("WeapSCAR-L_C");
				break;
			case "WeapUzi_C"_hash:
			case "WeapUZI_C"_hash:
			case "WeapUziPro_C"_hash:
				SetName = hash_("WeapUZI_C");
				OutName = ("WeapUZI_C");
				break;
			case "WeapVss_C"_hash:
			case "WeapVSS_C"_hash:
				SetName = hash_("WeapVSS_C");
				OutName = ("WeapVSS_C");
				break;
			case "WeapCowbar_C"_hash:
			case "WeapCrowbar_C"_hash:
				SetName = hash_("WeapCowbar_C");
				OutName = ("WeapCowbar_C");
				break;
			case "WeapMK12_C"_hash:
			case "WeapMk12_C"_hash:
				SetName = hash_("WeapMk12_C");
				OutName = ("WeapMk12_C");
				break;
			case "WeapMk14_C"_hash:
			case "WeapMK14_C"_hash:
				SetName = hash_("WeapMk14_C");
				OutName = ("WeapMk14_C");
				break;
			case "WeapFAMASG2_C"_hash:
				SetName = hash_("WeapFamasG2_C");
				OutName = ("WeapFamasG2_C");
				break;
			case "WeapM1897_C"_hash:
			case "WeapWinchester_C"_hash:
				SetName = hash_("WeapWinchester_C");
				OutName = ("WeapWinchester_C");
				break;
			case "WeapWin1894_C"_hash:
			case "WeapWin94_C"_hash:
				SetName = hash_("WeapWin94_C");
				OutName = ("WeapWin94_C");
				break;
			case "WeapM18_C"_hash:
				SetName = hash_("WeapSmokeBomb_C");
				OutName = ("WeapSmokeBomb_C");
				break;
			case "WeapSkorpion_C"_hash:
				SetName = hash_("Weapvz61Skorpion_C");
				OutName = ("Weapvz61Skorpion_C");
				break;
			case "WeapQBZ_C"_hash:
			case "WeapQBZ95_C"_hash:
				SetName = hash_("WeapQBZ95_C");
				OutName = ("WeapQBZ95_C");
				break;
			default:
				SetName = hash_(iter.first);
				OutName = iter.first;
				break;
			}
			auto FindResult = WeapSkinMap->find(SetName);
			if (FindResult == WeapSkinMap->end())
			{
				if (OutName == "Armor")
				{
					WeapSkinMap->emplace(hash_("Armor_Lv1"), std::vector<FSkinMapData>());
					WeapSkinMap->emplace(hash_("Armor_Lv2"), std::vector<FSkinMapData>());
					WeapSkinMap->emplace(hash_("Armor_Lv3"), std::vector<FSkinMapData>());
				}
				else if (OutName == "Helmet")
				{
					WeapSkinMap->emplace(hash_("Helmet_Lv1"), std::vector<FSkinMapData>());
					WeapSkinMap->emplace(hash_("Helmet_Lv2"), std::vector<FSkinMapData>());
					WeapSkinMap->emplace(hash_("Helmet_Lv3"), std::vector<FSkinMapData>());
				}
				else if (OutName == "Back")
				{
					WeapSkinMap->emplace(hash_("Back_Lv1"), std::vector<FSkinMapData>());
					WeapSkinMap->emplace(hash_("Back_Lv2"), std::vector<FSkinMapData>());
					WeapSkinMap->emplace(hash_("Back_Lv3"), std::vector<FSkinMapData>());
				}
				WeapSkinMap->emplace(SetName, std::vector<FSkinMapData>());
				//CONSOLE_INFO("Initliazed:%s|%p", OutName.c_str(), SetName);
			}
			FindResult = WeapSkinMap->find(SetName);
			if (FindResult != WeapSkinMap->end())
			{
				for (auto SkinName : iter.second)
				{
					if (OutName.find("Back") != std::string::npos || OutName.find("Armor") != std::string::npos || OutName.find("Helmet") != std::string::npos)
					{
						if (OutName.find("Back") != std::string::npos)
						{
							try
							{
								if (SkinName.FullName.find("Item_Skin_Backlv3") != std::string::npos || SkinName.FullName.find("Item_Skin_BackLV3") != std::string::npos
									|| SkinName.FullName.find("Item_Skin_BackLv3") != std::string::npos || SkinName.FullName.find("Item_Skin_Back_A") != std::string::npos
									|| SkinName.FullName.find("Item_Skin_Back_C") != std::string::npos)
									(*WeapSkinMap)[hash_("Back_Lv3")].push_back(FSkinMapData(FName(SkinName.IndexID), SkinName.FullName, 0));
								else if (SkinName.FullName.find("Item_Skin_BackLv2") != std::string::npos || SkinName.FullName.find("Item_Skin_BackLV2") != std::string::npos
									|| SkinName.FullName.find("Item_Skin_Backlv2") != std::string::npos || SkinName.FullName.find("Item_Skin_BackPack") != std::string::npos
									|| SkinName.FullName.find("Item_Skin_Back_BlueBlocker_01_C") != std::string::npos || SkinName.FullName.find("Item_Skin_Back_Scientist_01_C") != std::string::npos)
									(*WeapSkinMap)[hash_("Back_Lv2")].push_back(FSkinMapData(FName(SkinName.IndexID), SkinName.FullName, 0));
								else if (SkinName.FullName.find("Item_Skin_Back_E") != std::string::npos || SkinName.FullName.find("Item_Skin_BackLv1") != std::string::npos
									|| SkinName.FullName.find("Item_Skin_Backlv1") != std::string::npos || SkinName.FullName.find("Item_Skin_BackLv1") != std::string::npos)
									(*WeapSkinMap)[hash_("Back_Lv1")].push_back(FSkinMapData(FName(SkinName.IndexID), SkinName.FullName, 0));
							}
							catch (...)
							{

							}
						}
						else if (OutName.find("Armor") != std::string::npos)
						{
							try
							{
								if (SkinName.FullName.find("Armor_E") != std::string::npos)
									(*WeapSkinMap)[hash_("Armor_Lv1")].push_back(FSkinMapData(FName(SkinName.IndexID), SkinName.FullName, 0));
								else if (SkinName.FullName.find("Armor_D") != std::string::npos)
									(*WeapSkinMap)[hash_("Armor_Lv2")].push_back(FSkinMapData(FName(SkinName.IndexID), SkinName.FullName, 0));
								else if (SkinName.FullName.find("Armor_C") != std::string::npos)
									(*WeapSkinMap)[hash_("Armor_Lv3")].push_back(FSkinMapData(FName(SkinName.IndexID), SkinName.FullName, 0));
							}
							catch (...)
							{

							}

						}
						else if (OutName.find("Helmet") != std::string::npos)
						{
							try
							{
								if (SkinName.FullName.find("Helmet_E") != std::string::npos
									|| SkinName.FullName.find("Helmet_Viking") != std::string::npos || SkinName.FullName.find("Helmet_Biker") != std::string::npos
									|| SkinName.FullName.find("Helmet_999") != std::string::npos || SkinName.FullName.find("Helmet_Motorcross") != std::string::npos)
									(*WeapSkinMap)[hash_("Helmet_Lv1")].push_back(FSkinMapData(FName(SkinName.IndexID), SkinName.FullName, 0));
								else if (SkinName.FullName.find("Helmet_F") != std::string::npos || SkinName.FullName.find("Helmet_LV2") != std::string::npos
									|| SkinName.FullName.find("Helmet_Pillar") != std::string::npos)
									(*WeapSkinMap)[hash_("Helmet_Lv2")].push_back(FSkinMapData(FName(SkinName.IndexID), SkinName.FullName, 0));
								else if (SkinName.FullName.find("Helmet_G") != std::string::npos)
									(*WeapSkinMap)[hash_("Helmet_Lv3")].push_back(FSkinMapData(FName(SkinName.IndexID), SkinName.FullName, 0));
							}
							catch (...)
							{

							}
						}
					}
					else
					{
						auto IndexResult = GNameStore.find(hash_(SkinName.Name));
						if (IndexResult != GNameStore.end())
						{
							FindResult->second.push_back(FSkinMapData(FName(IndexResult->second), SkinName.Name, FName(SkinName.IndexID)));
						}
					}
				}
				//CONSOLE_INFO("%s|size:%d", OutName.c_str(), FindResult->second.size());
			}
			else
			{
				CONSOLE_INFO("CreateVector Failed!");
			}
			//CONSOLE_INFO("%s|%p|%d", OutName.c_str(), SetName, FindResult->second.size());
		}
		CONSOLE_INFO("skin map finish %d", GetTickCount64() - start);
	}

	void DrawHalfCircle(const FVector2D& Center, float Radius, const ImU32& Color, bool bTop)
	{
		const int32_t NumSides = 16;
		float AngleStep = PI / NumSides;  // 半圆
		FVector2D LastVertex = Center + FVector2D(Radius, 0);

		for (int32_t i = 1; i <= NumSides; ++i)
		{
			float Angle = i * AngleStep;
			FVector2D ThisVertex = Center + FVector2D(std::cos(Angle) * Radius, std::sin(Angle) * Radius * (bTop ? -1 : 1));
			Drawing::DrawLine(ImVec2(LastVertex.X, LastVertex.Y), ImVec2(ThisVertex.X, ThisVertex.Y), Color, 1);
			LastVertex = ThisVertex;
		}
	}

	void DrawCapsule(const FVector2D& Top, const FVector2D& Bottom, float Radius, const ImU32& Color)
	{
		// 画出连接顶端和底端的线
		Drawing::DrawLine(Top, Bottom, Color);

		// 画出顶端和底端的半圆
		DrawHalfCircle(Top, Radius, Color, true);
		DrawHalfCircle(Bottom, Radius, Color, false);

		// 画出中间的连接线
		FVector2D TopLeft = Top + FVector2D(-Radius, 0);
		FVector2D TopRight = Top + FVector2D(Radius, 0);
		FVector2D BottomLeft = Bottom + FVector2D(-Radius, 0);
		FVector2D BottomRight = Bottom + FVector2D(Radius, 0);

		Drawing::DrawLine(TopLeft, BottomLeft, Color);
		Drawing::DrawLine(TopRight, BottomRight, Color);
	}

	void Visuals::Update()
	{
		if (Vars.Menu.DataInfo)
		{
			ImGuiIO& io = ImGui::GetIO();
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, 25), 0xFFFFFFFF, 0xFF101010, 0, LOCALIZE_Visual(u8"FPS:%.0f", u8"帧率:%.0f"), !Vars.Misc.DisplayMode ? io.Framerate : (!sdk::InGame() ? io.Framerate : io.Framerate * 1.5f));
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, 40), Vars.Aimbot.Aimbot ? 0xFFFFFFFF : 0xFF00FFFF, 0xFF101010, 0, LOCALIZE_Visual(u8"Aimbot:%s", u8"自动瞄准:%s"), Vars.Aimbot.Aimbot ? LOCALIZE_Visual(u8"ON", u8"启动") : LOCALIZE_Visual(u8"OFF", u8"关闭"));
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, 55), Vars.Aimbot.AutoShot ? 0xFFFFFFFF : 0xFF00FFFF, 0xFF101010, 0, LOCALIZE_Visual(u8"Trigger Bot:%s", u8"自动扳机:%s"), Vars.Aimbot.AutoShot ? LOCALIZE_Visual(u8"ON", u8"启动") : LOCALIZE_Visual(u8"OFF", u8"关闭"));
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, 70), 0xFFFFFFFF, 0xFF101010, 0, LOCALIZE_Visual(u8"Config Set:%s", u8"选定配置:%s"), Vars.Menu.config == 0 ? u8"A" : u8"B");
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, 85), 0xFFFFFFFF, 0xFF101010, 0, LOCALIZE_Visual(u8"AliveTeams:%d", u8"存活队伍:%d"), sdk::GetNumAliveTeams());
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, 100), 0xFFFFFFFF, 0xFF101010, 0, LOCALIZE_Visual(u8"FogPlayer:%d", u8"迷雾中玩家:%d"), InFogPlayer);
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, 115), !Vars.Menu.bNeedReFresh ? 0xFFFFFFFF : 0xFF0000FF, 0xFF101010, 0, LOCALIZE_Visual(u8"ReFresh:%s", u8"刷新状态:%s"), Vars.Menu.bNeedReFresh ? LOCALIZE_Visual(u8"Freshing", u8"刷新中") : LOCALIZE_Visual(u8"Off", u8"关闭"));
			if (g_ServerResult.Version < 2)
				Vars.Menu.PhysxTrace = false;
			if (Vars.Menu.PhysxTrace)
				Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, 130), RunTimeStart == 0 ? 0xFF00FFFF : RunTimeStart < 10 ? 0xFF00FF00 : 0xFFFFFFFF, 0xFF101010, 0, LOCALIZE_Visual(u8"PhysicTrace:%s", u8"物理追踪:%s"), RunTimeStart == 0 ? LOCALIZE_Visual(u8"Not In Match", u8"未检测对局") : RunTimeStart < 10 ? LOCALIZE_Visual(u8"Loading", u8"加载中") : LOCALIZE_Visual(u8"Loaded", u8"已载入"));

			auto DamageDelta = sdk::GetDamageDelta();
			if (DamageDelta > 0.f)
				Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, !Vars.Menu.PhysxTrace ? 130 : 145), DamageDelta < 100.f ? 0xFFFFFFFF : DamageDelta < 200.f ? 0xFF00FF00 : 0xFF00FFFF, 0xFF101010, 0, LOCALIZE_Visual(u8"Damage:%.0f", u8"造成伤害:%.0f"), DamageDelta);
			InFogPlayer = 0;
#ifdef ENABLE_CONSOLE
			auto CursorPos = sdk::GetCursorPosition();
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(10, 160), 0xFFFFFFFF, 0xFF000000, 0, u8"X:%d|Y:%d", CursorPos.x, CursorPos.y);
#endif
		}

		if (!sdk::InGame())
		{
			TeamMateKDList.clear();
			m_ScoreBoard2.clear();
			bMMapRadarIsOpen = false;
			SpectatedCount = 0;
			return;
		}
		std::vector<TeamMateStruct>CopyTeam;
		auto LastTeamNum = sdk::GetLastTeamNum();
		for (int i = 0; i < TeamMateKDList.size(); i++)
		{
			if (TeamMateKDList[i].Team == LastTeamNum)
			{
				CopyTeam.push_back(TeamMateKDList[i]);
			}
		}
		TeamMateKDList.clear();
		for (int i = 0; i < CopyTeam.size(); i++)
		{
			TeamMateKDList.push_back(CopyTeam[i]);
		}


		if (Vars.Aimbot.DebugMode)
		{
			g_physic->DrawRigidBox();
		}

		bMMapRadarIsOpen = sdk::GetbMapOpen();
		ItemManager::GetGroupManager()->GetGroupByIndex(0).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(1).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(2).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(3).enable = Vars.espItem.ALL_Grenade;
		ItemManager::GetGroupManager()->GetGroupByIndex(4).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(5).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(6).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(7).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(8).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(9).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(10).enable = Vars.espItem.ALL_Weapon;
		ItemManager::GetGroupManager()->GetGroupByIndex(11).enable = Vars.espItem.ALL_Armor;
		ItemManager::GetGroupManager()->GetGroupByIndex(12).enable = Vars.espItem.ALL_Armor;
		ItemManager::GetGroupManager()->GetGroupByIndex(13).enable = Vars.espItem.ALL_Attachment;
		ItemManager::GetGroupManager()->GetGroupByIndex(14).enable = Vars.espItem.ALL_Attachment;
		ItemManager::GetGroupManager()->GetGroupByIndex(15).enable = Vars.espItem.ALL_Ammo;
		ItemManager::GetGroupManager()->GetGroupByIndex(16).enable = Vars.espItem.ALL_Attachment;
		ItemManager::GetGroupManager()->GetGroupByIndex(17).enable = Vars.espItem.ALL_Attachment;
		ItemManager::GetGroupManager()->GetGroupByIndex(18).enable = Vars.espItem.ALL_Attachment;
		ItemManager::GetGroupManager()->GetGroupByIndex(19).enable = Vars.espItem.ALL_Medicine;
		auto& player = sdk::GetPlayer();
		if (!IsAddrValid(player) || !sdk::IsLocalPlayerAlive())
		{
			SpectatedCount = 0;
			return;
		}
		SpectatedCount = sdk::GetSpectatedCount();

		if (Vars.espItem.SmartItem && sdk::IsLocalPlayerAlive())
		{
			//使用 Player->PlayerState->ReplicatedEquipableItems[]（0=head，1=armor，2=backpack）。如果 ReplicatedEquipableItems[0].Durability 大于 0，则有一个（未损坏的）头盔。
			if (_EquipmentData1.size())
				_EquipmentData1.clear();
			auto& Weapon1 = sdk::GetWeapon();
			if (IsAddrValid(Weapon1))
				_EquipmentData1 = sdk::GetWeponAttachmentData();
			{
				//ZeroMemory(_InventoryEquipment, sizeof(_InventoryEquipment));
				auto TempBuff = sdk::GetInventory_Equipment();
				for (int i = 0; i < TempBuff.size(); i++)
				{
					if (i >= 3)
						break;
					_InventoryEquipment[i] = TempBuff[i];
				}
			}
		}
#ifndef NoWriteMem
		if (Vars.Misc.WeaponSkin && sdk::IsLocalPlayerAlive())
		{
			if (!LogTimeToSkin)
				LogTimeToSkin = GetTickCount64();
			auto EquimentData = sdk::GetHumanEuqiment();
			if (GetTickCount64() - LogTimeToSkin >= 100)
			{
				LogTimeToSkin = GetTickCount64();
				if (g_ServerResult.Version == 3)
				{
					if (IsAddrValid(EquimentData[(int)EEquipSlotID::Backpack].Addr))
					{
						try
						{
							if (EquimentData[(int)EEquipSlotID::Backpack].EquimentName.find("Back_E") != std::string::npos && Vars.Misc.BackLv1_SetID != 0)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::Backpack];
								if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != Vars.Misc.BackLv1_SetID)
								{
									GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, Vars.Misc.BackLv1_SetID);
								}
							}
							else if (EquimentData[(int)EEquipSlotID::Backpack].EquimentName.find("Back_F") != std::string::npos && Vars.Misc.BackLv2_SetID != 0)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::Backpack];
								if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != Vars.Misc.BackLv2_SetID)
								{
									GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, Vars.Misc.BackLv2_SetID);
								}
							}
							else if (EquimentData[(int)EEquipSlotID::Backpack].EquimentName.find("Back_C") != std::string::npos && Vars.Misc.BackLv3_SetID != 0)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::Backpack];
								if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != Vars.Misc.BackLv3_SetID)
								{
									GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, Vars.Misc.BackLv3_SetID);
								}
							}
							else if (EquimentData[(int)EEquipSlotID::Backpack].EquimentName.find("BlueBlocker") != std::string::npos)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::Backpack];
								auto FindBackPack = WeapSkinMap.find(hash_("Back_Lv2"));
								if (FindBackPack != WeapSkinMap.end())
								{
									for (auto iter : FindBackPack->second)
									{
										if (iter.SkinItemName == "Item_Skin_Back_BlueBlocker_01_C")
										{
											if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != iter.SkinItemID.ComparisonIndex)
											{
												GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, iter.SkinItemID.ComparisonIndex);
											}
										}
									}
								}
							}
						}
						catch (...)
						{

						}
					}
					if (IsAddrValid(EquimentData[(int)EEquipSlotID::Head].Addr))
					{
						try
						{
							if (EquimentData[(int)EEquipSlotID::Head].EquimentName.find("Head_E") != std::string::npos && Vars.Misc.HelmetLv1_SetID != 0)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::Head];
								if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != Vars.Misc.HelmetLv1_SetID)
								{
									GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, Vars.Misc.HelmetLv1_SetID);
								}
							}
							else if (EquimentData[(int)EEquipSlotID::Head].EquimentName.find("Head_F") != std::string::npos && Vars.Misc.HelmetLv2_SetID != 0)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::Head];
								if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != Vars.Misc.HelmetLv2_SetID)
								{
									GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, Vars.Misc.HelmetLv2_SetID);
								}
							}
							else if (EquimentData[(int)EEquipSlotID::Head].EquimentName.find("Head_G") != std::string::npos && Vars.Misc.HelmetLv3_SetID != 0)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::Head];
								if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != Vars.Misc.HelmetLv3_SetID)
								{
									GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, Vars.Misc.HelmetLv3_SetID);
								}
							}
						}
						catch (...)
						{

						}
					}
					if (IsAddrValid(EquimentData[(int)EEquipSlotID::TorsoArmor].Addr))
					{
						try
						{
							if (EquimentData[(int)EEquipSlotID::TorsoArmor].EquimentName.find("Armor_E") != std::string::npos && Vars.Misc.ArmorLv1_SetID != 0)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::TorsoArmor];
								if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != Vars.Misc.ArmorLv1_SetID)
								{
									GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, Vars.Misc.ArmorLv1_SetID);
								}
							}
							else if (EquimentData[(int)EEquipSlotID::TorsoArmor].EquimentName.find("Armor_D") != std::string::npos && Vars.Misc.ArmorLv2_SetID != 0)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::TorsoArmor];
								if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != Vars.Misc.ArmorLv2_SetID)
								{
									GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, Vars.Misc.ArmorLv2_SetID);
								}
							}
							else if (EquimentData[(int)EEquipSlotID::TorsoArmor].EquimentName.find("Armor_C") != std::string::npos && Vars.Misc.ArmorLv3_SetID != 0)
							{
								auto EquimentGroup = EquimentData[(int)EEquipSlotID::TorsoArmor];
								if (EquimentGroup.Skin.SkinItemID.ComparisonIndex != Vars.Misc.ArmorLv3_SetID)
								{
									GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, Vars.Misc.ArmorLv3_SetID);
								}
							}
						}
						catch (...)
						{

						}
					}
				}
			}

			ptr_t WeaponMesh = sdk::GetMesh3P();
			if (!IsAddrValid(WeaponMesh))
			{
				return;
			}
			auto CurrentWeaponName = sdk::GetWeaponName();
			auto FindResult = WeapSkinMap.find(hash_(CurrentWeaponName));
			if (FindResult != WeapSkinMap.end())
			{
				auto SaveResult = WeapSaveSettings.find(hash_(CurrentWeaponName));
				if (SaveResult != WeapSaveSettings.end())
				{
					auto FNameArray = FindResult->second;
					int optionToggle = *SaveResult->second;
					int SkinCategoryType = (int)3;

					auto WeaponSkinParam = sdk::GetWeaponSkinParam();

					std::string DisplayName = "";
					std::string LevelName = "";
					auto WeaponNotFormatName = WeaponSkinParam.SkinParam.SkinName.GetName2();
					if (StringIsValid(WeaponNotFormatName))
					{
						auto weaponData = SkinWeaponManager::GetHandWeapon(hash_(WeaponNotFormatName));
						if (weaponData && !weaponData->formatName_.empty())
						{
							DisplayName = weaponData->formatName_;
							auto Pos = WeaponNotFormatName.find("_lv");
							if (Pos != std::string::npos)
							{
								LevelName = "LV" + WeaponNotFormatName.substr(Pos + 3);
							}

						}
						else
							DisplayName = WeaponNotFormatName;
					}
					else
						DisplayName = WeaponNotFormatName;

					if (LevelName.empty())
						Drawing::DrawTextOutline(OverlayEngine::Font16px, 16, 0, ImVec2(Drawing::GetViewPort().X / 2.f - 100, 120), Settings::ColorGold, 0xFF000000, FONT_CENTER,
							LOCALIZE_Visual(u8"<--->Switch Weapon's Skin %d / %d Current: %s", u8"<--->切換武器的特效 %d / %d 目前特效: %s"), optionToggle, FNameArray.size() - 1, DisplayName.c_str());
					else
						Drawing::DrawTextOutline(OverlayEngine::Font16px, 16, 0, ImVec2(Drawing::GetViewPort().X / 2.f - 100, 120), Settings::ColorGold, 0xFF000000, FONT_CENTER,
							LOCALIZE_Visual(u8"<--->Switch Weapon's Skin %d / %d Current: %s %s", u8"<--->切換武器的特效 %d / %d 目前特效: %s %s"), optionToggle, FNameArray.size() - 1, DisplayName.c_str(), LevelName.c_str());
					static ULONGLONG tickRun = 0;
					if (!tickRun)
						tickRun = GetTickCount64();
					if (GetTickCount64() - tickRun >= 20)
					{
						tickRun = GetTickCount64();
						if (optionToggle <= FNameArray.size() && optionToggle != -1 && WeaponSkinParam.SkinParam.SkinName.ComparisonIndex != FNameArray[optionToggle].SkinItemID.ComparisonIndex)
						{
							GetDMA().write<int>((ptr_t)WeaponMesh + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam + 8, SkinCategoryType);
							GetDMA().write<ptr_t>((ptr_t)WeaponMesh + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam + 16, FNameArray[optionToggle].SkinItemID.ComparisonIndex);
							GetDMA().write<ptr_t>((ptr_t)WeaponMesh + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam, FNameArray[optionToggle].ItemSkinID.ComparisonIndex);
							FEuqimentReplicatedSkinItem EquimentGroup;
							switch (sdk::GetWeaponIndex())
							{
							case 0:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponPrimary];
								break;
							case 1:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponSecondary];
								break;
							case 2:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponMelee];
								break;
							case 3:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponThrowable];
								break;
							case 4:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponTacticalGe];
								break;
							default:
								ZeroMemory(&EquimentGroup, sizeof(EquimentGroup));
								break;
							}
							if (IsAddrValid(EquimentGroup.Addr))
							{
								GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, FNameArray[optionToggle].ItemSkinID.ComparisonIndex);
							}
						}

						if (OverlayEngine::WasKeyPressed(39) && (optionToggle < 0 || optionToggle < FNameArray.size() - 1))
						{
							if (optionToggle < 0)
								optionToggle = 0;
							else
								optionToggle = optionToggle + 1;
							*SaveResult->second = optionToggle;
							GetDMA().write<int>((ptr_t)WeaponMesh + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam + 8, SkinCategoryType);
							GetDMA().write<ptr_t>((ptr_t)WeaponMesh + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam + 16, FNameArray[optionToggle].SkinItemID.ComparisonIndex);
							GetDMA().write<ptr_t>((ptr_t)WeaponMesh + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam, FNameArray[optionToggle].ItemSkinID.ComparisonIndex);
							FEuqimentReplicatedSkinItem EquimentGroup;
							switch (sdk::GetWeaponIndex())
							{
							case 0:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponPrimary];
								break;
							case 1:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponSecondary];
								break;
							case 2:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponMelee];
								break;
							case 3:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponThrowable];
								break;
							case 4:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponTacticalGe];
								break;
							default:
								ZeroMemory(&EquimentGroup, sizeof(EquimentGroup));
								break;
							}
							if (IsAddrValid(EquimentGroup.Addr))
							{
								GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, FNameArray[optionToggle].ItemSkinID.ComparisonIndex);
							}
						}
						if (OverlayEngine::WasKeyPressed(37) && optionToggle >= 0)
						{
							if (optionToggle <= 0)
								optionToggle = -1;
							else
								optionToggle = optionToggle - 1;
							*SaveResult->second = optionToggle;
							GetDMA().write<int>((ptr_t)WeaponMesh + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam + 8, SkinCategoryType);
							GetDMA().write<ptr_t>((ptr_t)WeaponMesh + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam + 16, optionToggle == -1 ? 0 : FNameArray[optionToggle].SkinItemID.ComparisonIndex);
							GetDMA().write<ptr_t>((ptr_t)WeaponMesh + g_PatternData.offset_UWeaponMeshComponent_ReplicatedSkinParam, FNameArray[optionToggle].ItemSkinID.ComparisonIndex);
							FEuqimentReplicatedSkinItem EquimentGroup;
							switch (sdk::GetWeaponIndex())
							{
							case 0:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponPrimary];
								break;
							case 1:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponSecondary];
								break;
							case 2:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponMelee];
								break;
							case 3:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponThrowable];
								break;
							case 4:
								EquimentGroup = EquimentData[(int)EEquipSlotID::WeaponTacticalGe];
								break;
							default:
								ZeroMemory(&EquimentGroup, sizeof(EquimentGroup));
								break;
							}
							if (IsAddrValid(EquimentGroup.Addr))
							{
								GetDMA().write<int>(EquimentGroup.Addr + g_PatternData.offset_UEquipableItem_ReplicatedSkinItem, FNameArray[optionToggle].ItemSkinID.ComparisonIndex);
							}
						}
					}
				}
			}
		}
#endif
	}

	void Visuals::BeginDraw()
	{
		if (!sdk::InGame())
			return;

		RaderType = sdk::GetSelectMinimapSizeIndex();

		if (Vars.espHuman.Spectator && SpectatedCount > 0 && SpectatedCount < 100 && sdk::IsLocalPlayerAlive())
			Drawing::DrawTextOutline(OverlayEngine::Font30px, 26, 0, ImVec2(150, 140), IM_COL32(255, 0, 0, 255), IM_COL32(0, 0, 0, 255), FONT_LEFT, LOCALIZE_Visual(u8"Observers:%d", u8"观察人数:%d"), SpectatedCount);

		auto WeaponType = sdk::GetWeaponType();
		if (Vars.Aimbot.FovShow && sdk::IsLocalPlayerAlive())
			Drawing::DrawCircle(Drawing::GetViewPort() / 2,
				(!Vars.Aimbot.AimMode ? (WeaponType == 0 ? Vars.Aimbot.AR_FovRange1 : WeaponType == 1 ? Vars.Aimbot.SR_FovRange1 : Vars.Aimbot.DMR_FovRange1) : (WeaponType == 0 ? Vars.Aimbot.AR_FovRange2 : WeaponType == 1 ? Vars.Aimbot.SR_FovRange2 : Vars.Aimbot.DMR_FovRange2)) * 14.f, ImGui::FloatArrayGetColorU32(Vars.Aimbot.FovColor), 80, 1);

		if (Vars.Aimbot.Crosshair && sdk::IsLocalPlayerAlive())
		{
			auto FullScreen = Drawing::GetViewPort();
			auto CenterOfScreen = Drawing::GetViewPort() / 2;
			Drawing::DrawLine(
				ImVec2(CenterOfScreen.X, CenterOfScreen.Y - FullScreen.Y * 0.014f),
				ImVec2(CenterOfScreen.X, CenterOfScreen.Y - FullScreen.Y * 0.014f - Vars.Aimbot.CrosshairSize),
				ImGui::FloatArrayGetColorU32(Vars.Aimbot.CrosshairColor), 2);

			Drawing::DrawLine(
				ImVec2(CenterOfScreen.X - FullScreen.X * 0.007875f, CenterOfScreen.Y),
				ImVec2(CenterOfScreen.X - FullScreen.X * 0.007875f - Vars.Aimbot.CrosshairSize, CenterOfScreen.Y),
				ImGui::FloatArrayGetColorU32(Vars.Aimbot.CrosshairColor), 2);

			Drawing::DrawLine(
				ImVec2(CenterOfScreen.X, CenterOfScreen.Y + FullScreen.Y * 0.014f),
				ImVec2(CenterOfScreen.X, CenterOfScreen.Y + FullScreen.Y * 0.014f + Vars.Aimbot.CrosshairSize),
				ImGui::FloatArrayGetColorU32(Vars.Aimbot.CrosshairColor), 2);

			Drawing::DrawLine(
				ImVec2(CenterOfScreen.X + FullScreen.X * 0.007875f, CenterOfScreen.Y),
				ImVec2(CenterOfScreen.X + FullScreen.X * 0.007875f + Vars.Aimbot.CrosshairSize, CenterOfScreen.Y),
				ImGui::FloatArrayGetColorU32(Vars.Aimbot.CrosshairColor), 2);
		}

		if (Vars.Misc.Projectile)
			ProjectESP(sdk::GetActors<EProjectile>());

		if (!Vars.Menu.战斗模式)
		{
			for (auto& actor : sdk::GetActors<EVehicle>())
			{
				if (actor && actor->IsValid())
				{
					//auto ReadGuard = actor->getReadLock();
					VehicleESP(*(actor->CCast<ATslWheeledVehicle>()));
					//ReadGuard.reset();
				}
			}

			for (auto& actor : sdk::GetActors<EBoat>())
			{
				if (actor && actor->IsValid())
				{
					//auto ReadGuard = actor->getReadLock();
					BoatESP(*(actor->CCast<ATslFloatingVehicle>()));
					//ReadGuard.reset();
				}
			}

			//for (auto& groupMap : sdk::GetItemGroups())
			//{
			//	if (groupMap.first && groupMap.first->IsValid())
			//		LootGroupESP(*(groupMap.first->CCast<ADroppedItemGroup>()), groupMap.second);
			//}

			for (auto& actor : sdk::GetActors<EItemGroup>())
			{
				if (actor && actor->IsValid())
				{
					//auto ReadGuard = actor->getReadLock();
					LootGroupESP(*(actor->CCast<ADroppedItemGroup>()));
					//ReadGuard.reset();
				}
			}

			for (auto& actor : sdk::GetActors<EItem>())
			{
				if (actor && actor->IsValid())
				{
					//auto ReadGuard = actor->getReadLock();
					LootESP(*actor->CCast<ADroppedItem>());
					//ReadGuard.reset();
				}
			}

			for (auto& actor : sdk::GetActors<EDeathBox>())
			{
				if (actor && actor->IsValid())
				{
					//auto ReadGuard = actor->getReadLock();
					DeathBoxESP(*actor->CCast<AFloorSnapItemPackage>());
					//ReadGuard.reset();
				}
			}

			for (auto& actor : sdk::GetActors<EAirdrop>())
			{
				if (actor && actor->IsValid())
				{
					//auto ReadGuard = actor->getReadLock();
					AirdropESP(*actor->CCast<ACarePackageItem>());
					//ReadGuard.reset();
				}
			}
		}
		std::unique_ptr<std::unordered_map<ptr_t, std::vector<FVector>>> RayCastArray(new std::unordered_map<ptr_t, std::vector<FVector>>());
		std::unique_ptr <std::unordered_map<ptr_t, std::shared_ptr<CapSuleInfo>>>TempCapSuleInfo(new std::unordered_map<ptr_t, std::shared_ptr<CapSuleInfo>>());
		for (auto actor : cpplinq::from(sdk::GetActors<EPlayer>())
			>> cpplinq::ref()
			>> cpplinq::where([](auto&& o) { return o.get() != nullptr && o.get()->IsValid(); })
			>> cpplinq::to_vector())
		{
			if (!actor.get() || !actor.get()->CCast<ATslCharacter>()->IsValid())
				continue;
			//auto ReadGuard = actor.get()->getReadLock();
			PlayerESP(*actor.get()->CCast<ATslCharacter>(), *TempCapSuleInfo, *RayCastArray);					//ReadGuard.reset();
		}

		if (Vars.Menu.PhysxTrace && g_physic->IsSceneCreate())
		{
			g_physic->SetRigidActor(*TempCapSuleInfo);
			TempCapSuleInfo.reset();
			g_physic->SetRayCast(*RayCastArray);
			RayCastArray.reset();
			RenderCount = 0;
		}
		else
		{
			RayCastArray.reset();
			TempCapSuleInfo.reset();
		}
#define ShowEOther

#ifdef ShowEOther
		for (auto& actor : sdk::GetActors<EOther>())
			if (actor && actor->IsValid())
				ActorESP(*actor->CCast<AActor>());
#endif
		auto PreditctOpen = WeaponType == 0 ? Vars.Aimbot.AR_PredictionPoint : WeaponType ? Vars.Aimbot.SR_PredictionPoint : Vars.Aimbot.DMR_PredictionPoint;
		if (PreditctOpen && !bMMapRadarIsOpen && PredictionPoint.X != 0.f && PredictionPoint.Y != 0.f && sdk::IsLocalPlayerAlive())
		{
			Drawing::DrawCircleFilled(PredictionPoint, 4.f, ImGui::FloatArrayGetColorU32(Vars.Aimbot.PredictionPointColor));
		}
		static bool bRingGrenade = false;
		static ptr_t TickGrenade = 0;
		static ptr_t WeaponEntity = 0;
		auto& HandleWeapon = sdk::GetWeapon();
		if (Vars.Misc.GrenadeCount && sdk::IsLocalPlayerAlive() && IsAddrValid(HandleWeapon))
		{
			if (sdk::GetIsGrenade() && hash_(sdk::GetWeaponName()) == "WeapGrenade_C"_hash)
			{
				if (OverlayEngine::IsKeyDown(VK_LBUTTON) && !bRingGrenade)
				{
					if (OverlayEngine::WasKeyPressed('R'))
					{
						CONSOLE_INFO("Start");
						bRingGrenade = true;
						TickGrenade = GetTickCount64();
						WeaponEntity = (ptr_t)HandleWeapon;
						OverlayEngine::SetKeyPressed('R', false);
					}
				}
			}
			if (bRingGrenade && GetTickCount64() - TickGrenade < 5000 && WeaponEntity == HandleWeapon && sdk::IsLocalPlayerAlive())
			{
				auto actualTime = 5000.f - ((float)(GetTickCount64() - TickGrenade));
				auto viewPort = Drawing::GetViewPort();
				auto length = viewPort.Y * 0.035f;
				auto DrawPos = viewPort / 2.f + ImVec2(viewPort.X * 0.085f, length / 2.f);
				Drawing::DrawRectFilled(DrawPos, DrawPos + ImVec2(7.f, -(length * (actualTime / 5000.f))), actualTime > 3000.f ? Settings::ColorGreen : actualTime > 1500.f ? Settings::ColorYellow : Settings::ColorRed, 3.f);
				Drawing::DrawRect(DrawPos, DrawPos + ImVec2(7.f, -length), 0xFF050505, 3.f);
				Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, DrawPos + ImVec2(4, 6), 0xFF00FF00, 0xFF000000, FONT_CENTER, "%.1f", ((float)(actualTime / 1000.f)));
			}
			else
			{
				TickGrenade = 0;
				bRingGrenade = false;
				WeaponEntity = 0;
			}
		}
		else
		{
			TickGrenade = 0;
			bRingGrenade = false;
			WeaponEntity = 0;
		}
	}

	void Visuals::EndDraw()
	{
		auto tick = GetTickCount64();
		if (/*tick - WatchKeyTime >= 20*/true)
		{
			WatchKeyTime = tick;
			if (OverlayEngine::WasKeyPressed(Vars.Menu.OpenedHotKey))
			{
				Vars.Menu.Opened = !Vars.Menu.Opened;
				if (!Vars.Menu.Opened)
				{
					if (Vars.Misc.DisplayMode)
						SetCursorPos(0, 0);
					OverlayEngine::Transparent();
					OverlayEngine::UpdateTargetWindow();
					Config->ThreadSave(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
					ItemManager::SaveConfig();
				}
				else
				{
					OverlayEngine::UpdateTargetWindow();
				}
			}

			if (OverlayEngine::WasKeyPressed(Vars.Menu.战斗模式HotKey))
				Vars.Menu.战斗模式 = !Vars.Menu.战斗模式;

			if (OverlayEngine::WasKeyPressed(Vars.espItem.EnableHotKey))
			{
				Vars.espItem.Item = !Vars.espItem.Item;
				Config->ThreadSave(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}

			if (OverlayEngine::WasKeyPressed(Vars.espItem.SmartItemHotKey))
			{
				Vars.espItem.SmartItem = !Vars.espItem.SmartItem;
				Config->ThreadSave(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}

			if (OverlayEngine::WasKeyPressed(Vars.espVehicle.EnableHotKey))
			{
				Vars.espVehicle.Vehicle = !Vars.espVehicle.Vehicle;
				Config->ThreadSave(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}

			if (OverlayEngine::WasKeyPressed(Vars.Aimbot.EnableHotKey))
			{
				Vars.Aimbot.Aimbot = !Vars.Aimbot.Aimbot;
				Config->ThreadSave(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}

			if (OverlayEngine::WasKeyPressed(Vars.Misc.BattlefieldInfoHotKey))
			{
				Vars.Misc.BattlefieldInfo = !Vars.Misc.BattlefieldInfo;
				Config->ThreadSave(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}

			if (OverlayEngine::WasKeyPressed(Vars.Misc.BattlePlayerInfoHotKey))
			{
				Vars.Misc.BattlePlayerInfo = !Vars.Misc.BattlePlayerInfo;
				Config->ThreadSave(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}

			if (OverlayEngine::WasKeyPressed(Vars.Misc.BattlefieldUnlockHotKey))
			{
				Vars.Misc.BattlefieldUnlock = !Vars.Misc.BattlefieldUnlock;
				if (Vars.Misc.DisplayMode && !Vars.Misc.BattlefieldUnlock)
				{
					auto ViewPort = Drawing::GetViewPort();
					SetCursorPos(ViewPort.X, ViewPort.Y);
				}
			}

			if (OverlayEngine::WasKeyPressed(Vars.Aimbot.AutoShotHotKey))
			{
				Vars.Aimbot.AutoShot = !Vars.Aimbot.AutoShot;
				Config->ThreadSave(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}

			if (OverlayEngine::WasKeyPressed(Vars.Aimbot.KnockDownHotKey))
			{
				auto WeaponType = sdk::GetWeaponType();
				if (WeaponType == 0)
				{
					Vars.Aimbot.AR_Knock = !Vars.Aimbot.AR_Knock;
				}
				else if (WeaponType == 1)
				{
					Vars.Aimbot.SR_Knock = !Vars.Aimbot.SR_Knock;
				}
				else
				{
					Vars.Aimbot.DMR_Knock = !Vars.Aimbot.DMR_Knock;
				}
				Config->ThreadSave(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
			}

			if (OverlayEngine::WasKeyPressed(Vars.Menu.SettingSwitchHotKey))
			{
				auto logConfig = !Vars.Menu.config;
				Config->Save(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
				Vars.Menu.config = logConfig;
				Config->Load(Vars.Menu.config == 0 ? hudPath1 : hudPath2);
				Vars.Menu.config = logConfig;
			}

			if (OverlayEngine::WasKeyPressed(Vars.Menu.FreshHotKey))
			{
				Vars.Menu.bNeedReFresh = true;
				return;
			}

			if (OverlayEngine::WasKeyPressed(VK_F11))
			{
				HideFunction++;
			}
			if (HideFunction == 0)
			{
				Vars.Aimbot.HighAimbot = 0;
			}
		}
		if (ColorOffset == -1)
		{
			srand((int)time(0));
			ColorOffset = rand() % 100;
		}
#ifndef ENABLE_CONSOLE
		if (bVersionChange)
		{
			static ULONGLONG offlinetime = 0;
			if (!offlinetime)
				offlinetime = GetTickCount64();
			ULONGLONG timetoend = GetTickCount64() - offlinetime;
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 28, 0, Drawing::GetViewPort() / 2.f, IM_COL32(0, 255, 0, 255), IM_COL32(0, 0, 0, 255), FONT_CENTER, LOCALIZE_Visual("Detected that a new version has been pushed, please restart", u8"侦测到有新版本推送，请重启"));

			if (timetoend >= 300000)
				TerminateProcess(GetCurrentProcess(), 0);
		}
		if (bMacChange)
		{
			static ULONGLONG offlinetime = 0;
			if (!offlinetime)
				offlinetime = GetTickCount64();
			ULONGLONG timetoend = GetTickCount64() - offlinetime;
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 28, 0, Drawing::GetViewPort() / 2.f, IM_COL32(0, 255, 0, 255), IM_COL32(0, 0, 0, 255), FONT_CENTER, LOCALIZE_Visual("Detected that your account has been logged in from a different location. Please check if the card password has been stolen", u8"侦测到您的帐户已从其他位置登入。请检查卡片密码是否被盗"));
			if (timetoend >= 60000)
				TerminateProcess(GetCurrentProcess(), 0);
		}
		if (bNetWorkError)
		{
			static ULONGLONG offlinetime = 0;
			if (!offlinetime)
				offlinetime = GetTickCount64();
			ULONGLONG timetoend = GetTickCount64() - offlinetime;
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 28, 0, Drawing::GetViewPort() / 2.f, IM_COL32(0, 255, 0, 255), IM_COL32(0, 0, 0, 255), FONT_CENTER, LOCALIZE_Visual("Your network seems to be disconnected, assistance will be exited soon", u8"你的网路似乎已经断开,辅助即将退出"));
			if (timetoend >= 60000)
				TerminateProcess(GetCurrentProcess(), 0);
		}
		if (!Auth::Check(true))
		{
			static ULONGLONG offlinetime = 0;
			if (!offlinetime)
				offlinetime = GetTickCount64();
			ULONGLONG timetoend = GetTickCount64() - offlinetime;
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 28, 0, ImVec2(120, 40), IM_COL32(255, 255, 0, 255), IM_COL32(0, 0, 0, 255), FONT_LEFT, LOCALIZE_Visual("The software has expired and will not be updated any more. Please renew your subscription before using it.", u8"软体使用期限已到,物件将不会继续更新。请续费后使用"));
		}
#endif	
		if (!sdk::InGame())
			return;
		auto CacheSize = GetUnCachePlayer().size();
		if (CacheSize)
			Drawing::DrawTextOutline(OverlayEngine::Font18px, 18, 0, ImVec2(Drawing::GetViewPort().X / 2.f, 10), 0xFFFFFFFF, 0xFF000000, FONT_CENTER, CacheSize ? u8"段位刷新中" : u8"段位刷新完成");

		if (Vars.Misc.BattlePlayerInfo)
		{
			if (m_ScoreBoard2.capacity() > 0)
			{
				std::sort(m_ScoreBoard2.begin(), m_ScoreBoard2.end(),
					[](ScoreBoard a, ScoreBoard b)
					{
						return (int)a.TeamNumber < (int)b.TeamNumber;
					}
				);
				Visuals::DrawScopeBoard2();
			}
		}

		if (Vars.Misc.BattlefieldInfo)
		{
			if (m_ScoreBoard.capacity() > 0)
			{
				std::sort(m_ScoreBoard.begin(), m_ScoreBoard.end(),
					[](ScoreBoard a, ScoreBoard b)
					{
						return a.Ranking > b.Ranking;
					}
				);
				std::sort(m_ScoreBoard.begin(), m_ScoreBoard.end(),
					[](ScoreBoard a, ScoreBoard b)
					{
						return a.KDA > b.KDA;
					}
				);
				std::sort(m_ScoreBoard.begin(), m_ScoreBoard.end(),
					[](ScoreBoard a, ScoreBoard b)
					{
						return (int)a.bFog < (int)b.bFog;
					}
				);
			}

			Visuals::DrawScopeBoard();
		}
		if (m_ScoreBoard.size())
			m_ScoreBoard.clear();

		if (Vars.Misc.Warning && AimSelfTeam.size() > 0)
		{
			auto ScreenSize = Drawing::GetViewPort();
			std::string formatText;
			for (int i = 0; i < AimSelfTeam.size(); i++)
			{
				char num1[32];
				ZeroMemory(num1, 32);
				_itoa(AimSelfTeam[i], num1, 10);
				if (i != AimSelfTeam.size() - 1)
					formatText += ("  ") + std::string(num1) + ("/  ");
				else
					formatText += std::string(num1);
			}
			Drawing::DrawTextOutline(OverlayEngine::Font20px, 25, 0, ImVec2(ScreenSize.X * 0.5f, ScreenSize.Y - ScreenSize.Y * 0.15f), Settings::ColorRed, 0xFF000000, FONT_LEFT, LOCALIZE_Visual("Locked Warning Team:%s", u8"被锁定警告 敌方队伍:%s"), formatText.c_str());
		}
		if (AimSelfTeam.size())
			AimSelfTeam.clear();
		if (Vars.espRank.Team && Vars.espRank.Rank && !TeamMateKDList.empty())
		{
			float LeftPos = 20;
			for (int i = 0; i < TeamMateKDList.size(); i++)
			{
				auto RankColor = TeamMateKDList[i].KDA <= 1.f ? Settings::ColorWhite : TeamMateKDList[i].KDA < 2.f ? Settings::ColorGreen : TeamMateKDList[i].KDA < 3.f ? Settings::ColorLightBlue : TeamMateKDList[i].KDA <= 3.5f ? Settings::ColorYellow : Settings::ColorRed;
				if (TeamMateKDList[i].BlackList)
					RankColor = Settings::ColorRed;
				if (TeamMateKDList[i].Partner)
					RankColor = Settings::ColorGold;
				auto DrawPos = Drawing::DrawTextOutline(OverlayEngine::Font20px, 20, 0, ImVec2(LeftPos, Drawing::GetViewPort().Y - 20.f), RankColor, 0xFF000000, FONT_LEFT, u8"%s:%s(%.1f)", TeamMateKDList[i].Name.c_str(), TeamMateKDList[i].RankTier.c_str(), TeamMateKDList[i].KDA);
				LeftPos += DrawPos.x + 15.f;
			}
		}
	}

	pRankIcon Visuals::getRankIcon(std::string rank_tier)
	{
		auto rankIcon = RankingIcon.find(hash_(rank_tier));
		if (rankIcon != RankingIcon.end() && rankIcon->second)
			return *rankIcon->second;
		return pRankIcon();
	}

	void Visuals::PlayerSkeleton(ATslCharacter& player, float headWidth, float dist, ImColor color, ImColor InvisibleColor, bool bIsVisible)
	{
		static std::vector<std::vector<int>> boneVec =
		{
			{ Bone::hand_l, Bone::lowerarm_l, Bone::upperarm_l, Bone::neck_01 },//左手 左下手臂 左上臂 颈部(喉结)
			{ Bone::hand_r, Bone::lowerarm_r, Bone::upperarm_r, Bone::neck_01 },
			{ Bone::foot_l, Bone::calf_l, Bone::thigh_l, Bone::spine_01 },//左脚  左小腿   左大腿   裆部
			{ Bone::foot_r, Bone::calf_r, Bone::thigh_r, Bone::spine_01 },
			{ Bone::spine_01, /*Bone::spine_02,*/ Bone::neck_01/*, Bone::forehead*/ }//裆部  胃  喉结 头
		};
		//FVector Min, Max;
		////sdk::GetLocalBounds(player->_CacheLocalBound, player->_CacheLocalBound2, Min, Max);
		//Min = player->_Location;
		//FTransform Trans = FTransform(player->_ComponentToWorld.Rotation, player->_Location);
		////Max += player->_ComponentToWorld.Translation;

		//FVector2D _2DPos;
		////auto Pspine_02 = (player->_ComponentToWorld.Translation + sdk::GetBonePosition(player, Bone::neck_01)) / 2.f;
		//auto Pspine_02 = Min;
		//if (sdk::WorldToScreen2(Pspine_02, _2DPos))
		//{
		//	Drawing::DrawCircleFilled(_2DPos, 1.5f, 0xFFFFFFFF);
		//}
		//auto Pspine_01 = Max;
		//if (sdk::WorldToScreen(Pspine_01, _2DPos))
		//{
		//	Drawing::DrawCircleFilled(_2DPos, 2.5f, 0xFF00FFFF);
		//}


		for (auto limb : boneVec)
		{
			FVector2D prev, cur, clip;
			FVector prev3D, cur3D;
			prev3D = sdk::GetBonePosition(player, limb.at(0));
			if (prev3D.IsZero())
				continue;
			if (!sdk::WorldToScreen(prev3D, prev))
				break;
			for (size_t bone = 1; bone < limb.size(); bone++)
			{
				int visible = bIsVisible ? 2 : 0;
				cur3D = sdk::GetBonePosition(player, limb.at(bone));
				if (cur3D.IsZero())
					continue;
				if (!sdk::WorldToScreen(cur3D, cur))
					break;

				bool preVisiable = false;
				bool postVisiable = false;

				if (bIsVisible)
				{
					if (!g_physic->IsSceneCreate())
					{
						preVisiable = sdk::PlayerVisibleCheck(player);
						postVisiable = sdk::PlayerVisibleCheck(player);
					}
					else
					{
						preVisiable = g_physic->GetActorVisible((ptr_t)player->GetPtr(), limb.at(0));
						postVisiable = g_physic->GetActorVisible((ptr_t)player->GetPtr(), limb.at(bone));
					}

					if (preVisiable || postVisiable)
					{
						visible = 2;
						goto draw;
					}

					if (!preVisiable && !postVisiable)
					{
						visible = 0;
						goto draw;
					}
				}

				//Settings::ColorWhite
			draw:
				// all visible
				if (visible == 2)
				{
					Drawing::DrawLine(prev, cur, color, headWidth);
				}
				else if (visible == 1)
				{
					Drawing::DrawLine(prev, clip, preVisiable ? color : InvisibleColor, headWidth);
					Drawing::DrawLine(clip, cur, preVisiable ? InvisibleColor : color, headWidth);
				}
				else if (visible == 0)
				{
					Drawing::DrawLine(prev, cur, InvisibleColor, headWidth);
				}
				prev = cur;
				prev3D = cur3D;
			}
		}
	}

	void Visuals::PlayerHealthBar(ATslCharacter& player, float center, float dist, float h, ImVec2 FootPos, int TeamID)
	{
		FVector foreheadPos = sdk::GetBonePosition(player, Bone::forehead);
		foreheadPos.Z += dist < 20.0f ? 20.0f : 35.0f /*dist * 2.0f*/;

		FVector2D screenPos;
		if (!sdk::WorldToScreen(foreheadPos, screenPos))
			return;

		static const auto character = m_HealthBar->m_sprites->frames["character.png"_hash];
		static const auto characterUv = m_HealthBar->m_uvs->frames["character.png"_hash];
		if (character && characterUv)
		{
			static auto killNum = reinterpret_cast<Font*>(characterUv.uvs.at("killNum"_hash));
			static auto characterName = reinterpret_cast<Font*>(characterUv.uvs.at("characterName"_hash));
			static auto distance = reinterpret_cast<Font*>(characterUv.uvs.at("distance"_hash));
			static auto healthbar = reinterpret_cast<Bar*>(characterUv.uvs.at("healthbar"_hash));

			static const ImVec2 spriteSize(m_HealthBar->m_sprites->meta.size.w, m_HealthBar->m_sprites->meta.size.h);
			const ImVec2 characterSize(character.frame.w /** zoom*/, character.frame.h /** zoom*/);
			const ImVec2 minRegion(character.frame.x, character.frame.y);
			const ImVec2 maxRegion(minRegion + characterSize);
			const ImVec2 basePosition = ImVec2(center - characterSize.x / 2.0f, screenPos.Y - characterSize.y);
			const ImRect bb(basePosition, basePosition + characterSize);

			static const ImVec2 uv0 = ImVec2(minRegion.x / spriteSize.x, minRegion.y / spriteSize.y);
			static const ImVec2 uv1 = ImVec2(maxRegion.x / spriteSize.x, maxRegion.y / spriteSize.y);


			/*
			 * characterName
			 */
			if (Vars.espHuman.PlayerName)
			{
				const ImVec2 characterNamePos(characterName->farme.x, characterName->farme.y + 3.0f);
				Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 86.f, bb.Min + characterNamePos + ImVec2(0, 0), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : ImGui::FloatArrayGetColorU32(Vars.espHuman.PlayerNameColor), 0xFF000000, FONT_LEFT, player->_ShowName.c_str());
			}

			/*
			 * healthBar
			 */
			const ImRect healthbarFarme(healthbar->farme.x, healthbar->farme.y, healthbar->farme.w - 24.f, healthbar->farme.h - 2.f);
			if (Vars.espHuman.HealthBar)
			{
				auto _health = player->_Health;
				auto _healthMax = player->_HealthMax;
				auto _groggyHealth = player->_GroggyHealth;
				auto _groggyHealthMax = player->_GroggyHealthMax;
				if (_health < 0.f || _health > 1000.f)
					_health = 100.f;
				if (_healthMax < 0.f || _healthMax > 1000.f)
					_healthMax = 100.f;
				if (_health == 0.f && _healthMax == 0.f)
				{
					_health = 100.f;
					_healthMax = 100.f;
				}
				if (_groggyHealth == 0.f && _groggyHealthMax == 0.f)
				{
					_groggyHealth = 100.f;
					_groggyHealthMax = 100.f;
				}
				bool _isGroggy = !(_health > 0.0f);
				auto _barW = _isGroggy ? (_groggyHealth / _groggyHealthMax) * healthbar->farme.w
					: (_health / _healthMax) * healthbar->farme.w;

				Drawing::DrawHealthBar(bb.Min + healthbarFarme.Min, healthbarFarme.Max, _isGroggy ? _groggyHealth : _health, _isGroggy ? _groggyHealthMax : _healthMax, _isGroggy, ImVec4(Vars.espHuman.HealthBarColor), ImVec4(Vars.espHuman.HealthBarOutLineColor));
			}
			if (player->_isParthers)
				Drawing::DrawLine((bb.Min + healthbarFarme.Min) + ImVec2(0, healthbarFarme.Max.y + 2), (bb.Min + healthbarFarme.Min) + healthbarFarme.Max + ImVec2(0, 2), 0xff00D7FF);

			if (player->_isDisconnected)
				Drawing::DrawImage(m_HealthBar->m_Disconnected, screenPos - ImVec2(12.5, 12.5), screenPos + ImVec2(12.5, 12.5));
			/*
			 * Team
			 */
			if (Vars.espHuman.PlayerTeam)
			{
				auto TO = TeamID + ColorOffset;
				while (TO >= 100) { TO -= 100; }
				if (TeamID < 200)
					Drawing::DrawCircleFilled(bb.Min + healthbarFarme.Min - ImVec2(TeamID > 9 ? 12 : 7, 7), TeamID > 9 ? 10 : 8, TEAM_COLOR[TO]);
				Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize + 2, 0, bb.Min + healthbarFarme.Min - ImVec2(TeamID > 9 ? 12 : 7, 8), 0xFFFFFFFF, 0xFF000000, FONT_CENTER, ("%d"), TeamID);
			}

			if (Vars.espHuman.Observer && player->_SpectatedCount > 0)
			{
				Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, bb.Min + healthbarFarme.Min + healthbarFarme.Max + (!Vars.espRank.RankTexture ? ImVec2(10, -14) : ImVec2(10, -34)), Settings::ColorYellow, 0xFF000000, FONT_CENTER, ("%d"), player->_SpectatedCount);
			}

			std::unique_ptr<ATslWeapon> weapon(new ATslWeapon(player->_CurrentWeapon));
			if (Vars.espHuman.PlayerWeapon && weapon->IsValid())
			{
				FName WeaponFName;
				WeaponFName.ComparisonIndex = UEEncryptedObjectProperty<int32_t, DecryptFunc::UObjectNameComparisonIndex>::STATIC_Get(player->_EncryptWeaponID);
				std::unique_ptr<std::string> WeaponName = std::make_unique<std::string>(WeaponFName.GetName2());
				if (Utils::StringIsValid(*WeaponName))
				{
					const auto weaponData = HandWeaponManager::GetHandWeapon(hash_(*WeaponName));
					if (weaponData && weaponData->icon.size() != 0 && weaponData->icon[0].iconSize.x != 0.f && weaponData->icon[0].iconSize.y != 0.f)
					{
						ImRect db;
						const ImVec2 ImagebasePosition = ImVec2(center - weaponData->icon[0].iconSize.x / 2.f * 0.8f, (screenPos.Y - characterSize.y + 4.f) - weaponData->icon[0].iconSize.y * 0.8f);
						const ImRect Imagebb(ImagebasePosition, ImagebasePosition + weaponData->icon[0].iconSize * 0.8f);
						if (!bMMapRadarIsOpen)
							Drawing::DrawImage(HandWeaponManager::GetTexture()[0], Imagebb.Min, Imagebb.Max, weaponData->icon[0].uv0, weaponData->icon[0].uv1, player->_IsReloading_CP ? ImColor(255, 0, 0, 255) : ImGui::FloatArrayGetColorU32(Vars.espHuman.PlayerWeaponColor));
					}
				}
			}

			if (Vars.espRank.Rank && player->_CurrentRankPoint > 0)
			{
				if (player->_Health > 0.f || player->_GroggyHealth > 0.f)
				{
					if (Vars.espRank.RankTexture)
					{
						pRankIcon Icon_rank = getRankIcon(player->_RankIcon);
						if (!Utils::IsSafeReadPtr(Icon_rank.Icon, 8))
						{
							auto RankPos = bb.Min + healthbarFarme.Min + healthbarFarme.Max;
							Drawing::DrawImage(Icon_rank.Icon, RankPos - ImVec2(0, 22), RankPos + ImVec2(22, 0));
							auto RankColor = player->_KDA <= 1.f ? Settings::ColorWhite : player->_KDA < 2.f ? Settings::ColorGreen : player->_KDA < 3.f ? Settings::ColorLightBlue : player->_KDA <= 3.5f ? Settings::ColorYellow : Settings::ColorRed;
							Drawing::DrawTextOutline(OverlayEngine::Font16px, 15, 0, RankPos + ImVec2(5.f, 8.f), RankColor, IM_COL32(0, 0, 0, 200), FONT_LEFT, "%.1f", player->_KDA);
						}
					}
				}
			}
		}
	}

	void Visuals::PlayerBox(ATslCharacter& player, float x, float y, float w, float h, float dis, ImColor color)
	{
		ImColor outline = ImColor(0.0f, 0.0f, 0.0f, color.Value.w);
		if (h < 5.0f || dis > 250.0f) outline.Value.w = 0;//远距离 描边透明

		Drawing::DrawOutlinedRect({ x - w, y }, { x + w, y + h }, color, outline);
	}

	void Visuals::PlayerAimPosition(ATslCharacter& player, FVector HeadPosition, float t, ImColor color, float Distance)
	{
		if (!player || !player->IsValid())
			return;
		bool IsSee1 = false;
		bool IsSee2 = false;
		bool IsOffLine = false;
		FVector 视线结束点 = { 0,0,0 };
		FVector2D 起始点 = { 0,0 }, 结束点 = { 0,0 }, 自己位置 = { 0,0 };
		auto selfLocation = sdk::GetComponentLocation();
		FRotator Rotator = player->_AimOffsets;
		if (player->_LastAimOffsets.Yaw == 0.f && player->_LastAimOffsets == 0.f)
			IsOffLine = true;

		Rotator = Clamp(Rotator);

		float 弧度X = Rotator.Yaw * M_PI / 180.0f;
		float 弧度Y = Rotator.Pitch * M_PI / 180.0f;
		视线结束点.X = HeadPosition.X + cos(弧度X) * t;
		视线结束点.Y = HeadPosition.Y + sin(弧度X) * t;
		视线结束点.Z = HeadPosition.Z + sin(弧度Y) * t;	// 这里要看游戏Y角度数据是什么样子而调整是[+]还是[-]

		auto viewSize = Drawing::GetViewPort();

		if (player->_IsAiming_CP && player->_ScopingAlpha_CP != 0.f)
		{
			auto origin = player->_Location;
			auto deltaFromLocal = origin - selfLocation;
			float MapDiv = GetMapDiv();
			float X = round(deltaFromLocal.X / 20000.f * MapDiv) + viewSize.X / 2.f;
			float Y = round(deltaFromLocal.Y / 20000.f * MapDiv) + viewSize.Y / 2.f;
			float MyX = viewSize.X / 2.f;
			float MyY = viewSize.Y / 2.f;
			float FovBegin = Rotator.Yaw - 10.f;
			if (FovBegin < 0.f)
				FovBegin += 360.f;
			if (FovBegin > 360.f)
				FovBegin -= 360.f;
			float FovEnd = Rotator.Yaw + 10.f;
			if (FovEnd < 0.f)
				FovEnd += 360.f;
			if (FovEnd > 360.f)
				FovEnd -= 360.f;
			ImVec2 LineLeft = GetViewPos(ImVec2(X, Y), FovBegin, 15.f);
			ImVec2 LineRight = GetViewPos(ImVec2(X, Y), FovEnd, 15.f);

			auto TeamNumber = player->_LastTeamNum;
			if (TeamNumber >= 100000)
				TeamNumber -= 100000;
			auto TO = TeamNumber + ColorOffset;
			if (TO >= 100)
				TO -= 100;
			if (TO > 100 || TO < 0)
				TO = 0;
			ImVec2 LineBegin = GetViewPos(LineLeft, Rotator.Yaw, 500.f);
			ImVec2 LineEnd = GetViewPos(LineRight, Rotator.Yaw, 500.f);

			ImVec2 A = ImVec2(X, Y), B = LineLeft, C = LineRight, P = ImVec2(MyX, MyY);
			double angle1 = get_angle(C, A, B);
			double angle2 = get_angle(A, B, C);
			double angle3 = get_angle(B, C, A);
			double cangle1 = get_angle(P, A, C);
			double cangle2 = get_angle(P, A, B);
			double cangle3 = get_angle(P, B, A);
			double cangle4 = get_angle(P, B, C);
			double cangle5 = get_angle(P, C, A);
			double cangle6 = get_angle(P, C, B);

			if (isInsideQuadrilateral(LineLeft, LineBegin, LineEnd, LineRight, ImVec2(MyX, MyY)) || round(cangle1 + cangle2 + cangle3 + cangle4 + cangle5 + cangle6) == round(angle1 + angle2 + angle3))
				IsSee1 = true;
			else
				IsSee1 = false;
			if (!IsSee1)
				goto printLine;
			IsSee2 = g_physic->GetActorVisible((ptr_t)player->GetPtr(), 23);
		}

		auto TeamNumber = player->_LastTeamNum;
		if (TeamNumber >= 100000)
			TeamNumber -= 100000;
		auto TO = TeamNumber + ColorOffset;
		if (TO >= 100)
			TO -= 100;

	printLine:
		FVector2D sccrenCenter = viewSize / 2.f;
		if (sdk::WorldToScreen2(视线结束点, 结束点) && sdk::WorldToScreen2(HeadPosition, 起始点))
		{
			if (起始点.X == 0.f && 起始点.Y == 0.f)
				return;
			if (IsSee1)
			{
				if (Vars.Misc.WarningExt)
				{
					if (abs(sccrenCenter.X - 起始点.X) <= viewSize.X * 0.075f && abs(sccrenCenter.Y - 起始点.Y) <= viewSize.X * 0.075f)
						Drawing::DrawLine(sccrenCenter, 起始点, IsSee2 ? 0xFF0000ff : 0xFFfffffd, 1.25f);
					else
					{
						auto F = atan2((起始点.Y - sccrenCenter.Y), (起始点.X - sccrenCenter.X));
						ImVec2 ViewPortBegin = ImVec2(sccrenCenter.X + 60.f * cos(F), sccrenCenter.Y + 60.f * sin(F));
						ImVec2 ViewPortEnd = ImVec2(sccrenCenter.X + 140.f * cos(F), sccrenCenter.Y + 140.f * sin(F));
						Drawing::DrawCircle(sccrenCenter, 120.f, 0xFF00FFFF, 140, 1.5f);
						//Drawing::DrawLine(ViewPortBegin, ViewPortEnd, IsSee2 ? Settings::ColorRed : Settings::ColorYellow, 2.5f);
						ImVec2 Pos1 = ViewPortEnd;
						auto Theta = F * (180.f / M_PI);
						auto ts = Theta - 145.f;
						if (ts < 0.f)
							ts += 360.f;
						if (ts > 360.f)
							ts -= 360.f;
						ViewPortBegin = GetViewPos(ViewPortEnd, ts, 20.f);
						ImVec2 Pos2 = ViewPortBegin;

						//Drawing::DrawLine(ViewPortEnd, ViewPortBegin, IsSee2 ? Settings::ColorRed : Settings::ColorYellow, 2.f);
						ts = Theta + 145.f;
						if (ts < 0.f)
							ts += 360.f;
						if (ts > 360.f)
							ts -= 360.f;
						ViewPortBegin = GetViewPos(ViewPortEnd, ts, 20.f);
						ImVec2 Pos3 = ViewPortBegin;
						//Drawing::DrawLine(ViewPortEnd, ViewPortBegin, IsSee2 ? Settings::ColorRed : Settings::ColorYellow, 2.f);
						Drawing::DrawTriangleFilled(Pos1, Pos2, Pos3, IsSee2 ? 0xFF0000FF : 0xfffffffd);
						Drawing::DrawTriangle(Pos1, Pos2, Pos3, IsSee2 ? 0xFF000000 : 0xfffffffd);
					}
				}

				{
					auto origin = player->_Location;
					auto deltaFromLocal = origin - (!IsAddrValid(sdk::GetPlayer()) ? sdk::GetLocalPos() : selfLocation);
					auto viewSize = Drawing::GetViewPort();
					if (!RaderType)
					{
						if (deltaFromLocal.X > 20000.f || deltaFromLocal.X < -20000.f)
							return;
						if (deltaFromLocal.Y > 20000.f || deltaFromLocal.Y < -20000.f)
							return;
					}
					else
					{
						if (deltaFromLocal.X > 37000.f || deltaFromLocal.X < -37000.f)
							return;
						if (deltaFromLocal.Y > 37000.f || deltaFromLocal.Y < -37000.f)
							return;
					}

					ImVec2 MiniMapPos = GetMapRaderCenter();
					float MapDiv = GetMapDiv();
					float X = round(deltaFromLocal.X / 20000.f * MapDiv) + viewSize.X * MiniMapPos.x;
					float Y = round(deltaFromLocal.Y / 20000.f * MapDiv) + viewSize.Y * MiniMapPos.y;

					float MyX = viewSize.X * MiniMapPos.x;
					float MyY = viewSize.Y * MiniMapPos.y;

					if (TO > 100 || TO < 0)
						return;

					ImVec2 OutLine;
					ExPandLine(ImVec2(X, Y), ImVec2(MyX, MyY), 150.f, OutLine);
					Drawing::DrawLine(ImVec2(X, Y), OutLine, IsSee2 ? TEAM_COLOR[TO] : 0xA0fffffd, 2);
					ExPandLine(ImVec2(MyX, MyY), ImVec2(X, Y), 10.f, OutLine);
					Drawing::DrawLine(ImVec2(MyX, MyY), OutLine, IsSee2 ? TEAM_COLOR[TO] : 0xA0fffffd, 2);
					AimSelfTeam.push_back(TeamNumber);
				}
			}
		}
		else if (IsSee1)
		{
			//float sub = 0.f;
			//if (player->IsValid())
			//{
			//	if (sdk::GetLastRenderTimeOnScreen() == -1000.f || player->_LastRenderTimeOnScreen == -1000.f)
			//		return;
			//	sub = abs(sdk::GetLastRenderTimeOnScreen() - player->_LastRenderTimeOnScreen) <= 0.1f;
			//}
			//if (sub <= 15.f)
			//	IsSee2 = true;
			//else
			//	IsSee2 = false;
			if (结束点.X != 0.f || 结束点.Y != 0.f)
			{
				if (Vars.Misc.WarningExt)
				{
					//CONSOLE_INFO("%d|%f", player->_IsAiming_CP, player->_ScopingAlpha_CP);
					if (abs(sccrenCenter.X - 结束点.X) <= viewSize.X * 0.075f && abs(sccrenCenter.Y - 结束点.Y) <= viewSize.X * 0.075f)
					{
						Drawing::DrawLine(sccrenCenter, 结束点, IsSee2 ? 0x700000ff : 0x80fffffd, 1.0f);
					}
					else
					{
						auto F = atan2((结束点.Y - sccrenCenter.Y), (结束点.X - sccrenCenter.X));
						ImVec2 ViewPortBegin = ImVec2(sccrenCenter.X + 60.f * cos(F), sccrenCenter.Y + 60.f * sin(F));
						ImVec2 ViewPortEnd = ImVec2(sccrenCenter.X + 140.f * cos(F), sccrenCenter.Y + 140.f * sin(F));
						Drawing::DrawCircle(sccrenCenter, 120.f, 0xFF00FFFF, 140, 1.5f);
						//Drawing::DrawLine(ViewPortBegin, ViewPortEnd, IsSee2 ? Settings::ColorRed : Settings::ColorYellow, 2.5f);
						ImVec2 Pos1 = ViewPortEnd;
						auto Theta = F * (180.f / M_PI);
						auto ts = Theta - 145.f;
						if (ts < 0.f)
							ts += 360.f;
						if (ts > 360.f)
							ts -= 360.f;
						ViewPortBegin = GetViewPos(ViewPortEnd, ts, 20.f);
						ImVec2 Pos2 = ViewPortBegin;

						//Drawing::DrawLine(ViewPortEnd, ViewPortBegin, IsSee2 ? Settings::ColorRed : Settings::ColorYellow, 2.f);
						ts = Theta + 145.f;
						if (ts < 0.f)
							ts += 360.f;
						if (ts > 360.f)
							ts -= 360.f;
						ViewPortBegin = GetViewPos(ViewPortEnd, ts, 20.f);
						ImVec2 Pos3 = ViewPortBegin;
						//Drawing::DrawLine(ViewPortEnd, ViewPortBegin, IsSee2 ? Settings::ColorRed : Settings::ColorYellow, 2.f);
						Drawing::DrawTriangleFilled(Pos1, Pos2, Pos3, IsSee2 ? 0xFF0000FF : 0xFFFFFFFD);
						Drawing::DrawTriangle(Pos1, Pos2, Pos3, IsSee2 ? 0xFF0000FF : 0xFFFFFFFD);
					}
				}
			}

			{
				auto origin = player->_Location;
				auto deltaFromLocal = origin - (!IsAddrValid(sdk::GetPlayer()) ? sdk::GetLocalPos() : selfLocation);

				auto viewSize = Drawing::GetViewPort();

				if (!RaderType)
				{
					if (deltaFromLocal.X > 20000.f || deltaFromLocal.X < -20000.f)
						return;
					if (deltaFromLocal.Y > 20000.f || deltaFromLocal.Y < -20000.f)
						return;
				}
				else
				{
					if (deltaFromLocal.X > 37000.f || deltaFromLocal.X < -37000.f)
						return;
					if (deltaFromLocal.Y > 37000.f || deltaFromLocal.Y < -37000.f)
						return;
				}

				ImVec2 MiniMapPos = GetMapRaderCenter();
				float MapDiv = GetMapDiv();
				float X = round(deltaFromLocal.X / 20000.f * MapDiv) + viewSize.X * MiniMapPos.x;
				float Y = round(deltaFromLocal.Y / 20000.f * MapDiv) + viewSize.Y * MiniMapPos.y;

				float MyX = viewSize.X * MiniMapPos.x;
				float MyY = viewSize.Y * MiniMapPos.y;
				ImVec2 OutLine;
				ExPandLine(ImVec2(X, Y), ImVec2(MyX, MyY), 150.f, OutLine);
				Drawing::DrawLine(ImVec2(X, Y), OutLine, IsSee2 ? TEAM_COLOR[TO] : 0xA0fffffd, 2);
				ExPandLine(ImVec2(MyX, MyY), ImVec2(X, Y), 10.f, OutLine);
				Drawing::DrawLine(ImVec2(MyX, MyY), OutLine, IsSee2 ? TEAM_COLOR[TO] : 0xA0fffffd, 2);
				AimSelfTeam.push_back(TeamNumber);
			}
		}
	}

	void Visuals::ProjectESP(std::vector<std::shared_ptr<AActor>>& projects)
	{
		int count = 0;
		float TimeTillExplosion = -0.1f;
		float ExplosionDelay = -0.1f;
		float TimePercent = -0.1f;
		std::unordered_map <ptr_t, std::shared_ptr<std::vector<FVector>>> ProjectPathCopy;
		for (auto& actor : projects)
		{
			if (!actor || !actor->IsValid()) continue;
			auto proj = actor->CCast<ATslProjectile>();
			auto root = (USceneComponent)proj->_RootComponent;
			if (!root->IsValid())
				continue;
			auto aa = proj->_Location;
			if (aa.IsZero())
				continue;
			//Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, ImVec2(100, 100 + count * 18),0xFFFFFFFF,0xFF000000,FONT_LEFT,"%p",actor->GetPtr());
			//count++;

			auto tickFrame = GetTickCount64();

			if (proj->_ExistExplorTime == 0)
				proj->_ExistExplorTime = tickFrame;
			else
			{
				if (tickFrame - proj->_ExistExplorTime >= 100)
				{
					proj->_ExistExplorTime = tickFrame;
					if (proj->_LogTimeTillExplosion == 0)
					{
						proj->_LogTimeTillExplosion = proj->_TimeTillExplosion;
						proj->_bProjectleValid = true;
					}
					else
					{
						if (proj->_LogTimeTillExplosion == proj->_TimeTillExplosion)
						{
							proj->_bProjectleValid = false;
							if (ProjectPath.find((ptr_t)proj->GetPtr()) != ProjectPath.end())
							{
								ProjectPath[(ptr_t)proj->GetPtr()]->clear();
							}
						}
						else
						{
							proj->_LogTimeTillExplosion = proj->_TimeTillExplosion;
							proj->_bProjectleValid = true;
						}
					}
				}
			}
			if (!proj->_bProjectleValid)
				continue;
			auto name = proj->_ProjectileName;
			std::string ItemDisplayName;

			if (name.find("StickyGrenade") != name.npos)
			{
				if (!Vars.Misc.multiProject_num[1])
					continue;
				ItemDisplayName = LOCALIZE_Visual("StickyGrenade", u8"粘性炸弹");
				name = "K_UXU_02";
			}
			else if (name.find("DecoyGrenade") != name.npos)
			{
				if (!Vars.Misc.multiProject_num[3])
					continue;
				ItemDisplayName = LOCALIZE_Visual("DecoyGrenade", u8"诱饵手雷");
				name = "K_WDU_07";
			}
			else if (name.find("Grenade") != name.npos)
			{
				if (!Vars.Misc.multiProject_num[0])
					continue;
				ItemDisplayName = LOCALIZE_Visual("Grenade", u8"破片手榴彈");
				name = "ItemName_Weapon_Grenade";
			}
			else if (name.find("ProjC4") != name.npos)
			{
				if (!Vars.Misc.multiProject_num[6])
					continue;
				ItemDisplayName = LOCALIZE_Visual("C4", u8"C4");
				name = ("K_UXU_06");
			}
			else if (name.find("Molotov") != name.npos)
			{
				if (!Vars.Misc.multiProject_num[7])
					continue;
				ItemDisplayName = LOCALIZE_Visual("Molotov", u8"燃烧瓶");
				name = "ItemName_Weapon_Molotov";
			}
			else if (name.find("FlashBang") != name.npos)
			{
				if (!Vars.Misc.multiProject_num[5])
					continue;
				ItemDisplayName = LOCALIZE_Visual("FlashBang", u8"闪光弹");
				name = "ItemName_Weapon_FlashBang";
			}
			else if (name.find("Smoke") != name.npos)
			{
				if (!Vars.Misc.multiProject_num[4])
					continue;
				ItemDisplayName = LOCALIZE_Visual("SmokeBomb", u8"烟雾弹");
				name = "ItemName_Weapon_SmokeBomb";
			}
			else if (name.find("Bluezone") != name.npos)
			{
				if (!Vars.Misc.multiProject_num[2])
					continue;
				ItemDisplayName = LOCALIZE_Visual("BluezoneGrenade", u8"蓝圈手榴弹");
				name = "K_WDU_19";
			}
			else
				continue;
			if (Vars.Misc.ProjectileType)
				ItemDisplayName = ("Gren");
			auto ItemHashName = hash_(name);

			if (ProjectPath.find((ptr_t)proj->GetPtr()) != ProjectPath.end())
			{
				ProjectPathCopy[(ptr_t)proj->GetPtr()] = ProjectPath[(ptr_t)proj->GetPtr()];
			}

			FVector2D sLoot;
			auto bSee = sdk::WorldToScreen2(aa, sLoot);
			if (true)
			{
				auto distance = proj->_dist;
				if (distance >= 150.f)
					continue;
				{
					auto& path = ProjectPathCopy[(ptr_t)proj->GetPtr()];
					if (!path)
						path = std::make_shared<std::vector<FVector>>();
					if (proj->_TimeTillExplosion > 0.0f)
					{
						if (path->empty() && !aa.IsZero())
							path->push_back(aa);
						else if (!aa.IsZero() && (aa - path->back()).Size() > 50.0f)
						{
							path->push_back(aa);
						}
					}
					else
					{
						path->clear();
					}
				}

				TimeTillExplosion = proj->_TimeTillExplosion;
				if (TimeTillExplosion <= 0.f)
					TimeTillExplosion = 0.f;
				ExplosionDelay = proj->_ExplosionDelay;
				TimePercent = TimeTillExplosion / ExplosionDelay;
				const auto& loots = ItemManager::GetGroupManager()->GetLoots()[LootCategory::Throwable];
				if (loots.empty())
					continue;
				auto lootInfo = loots.find(ItemHashName);
				if (lootInfo == loots.end())
					continue;
				auto loot = lootInfo->second;
				if (loot == nullptr)
					continue;
				if (TimeTillExplosion > 0.f)
				{
					auto Radius = proj->_ProjectileConfig.ExplosionRadius;
					float Percent = 1.f;
					if (Vars.Misc.ProjectDamageDistance >= 10.f && Vars.Misc.ProjectDamageDistance <= 20.f)
						Percent = 0.9f - (Vars.Misc.ProjectDamageDistance - 10.f) * 0.01f;
					if (Vars.Misc.ProjectDamageDistance >= 20.f && Vars.Misc.ProjectDamageDistance <= 30.f)
						Percent = 0.8f - (Vars.Misc.ProjectDamageDistance - 20.f) * 0.01f;
					if (Vars.Misc.ProjectDamageDistance >= 30.f && Vars.Misc.ProjectDamageDistance <= 40.f)
						Percent = 0.7f - (Vars.Misc.ProjectDamageDistance - 30.f) * 0.01f;
					if (Vars.Misc.ProjectDamageDistance >= 40.f && Vars.Misc.ProjectDamageDistance <= 60.f)
						Percent = 0.6f - (Vars.Misc.ProjectDamageDistance - 40.f) * 0.005f;
					if (Vars.Misc.ProjectDamageDistance >= 60.f && Vars.Misc.ProjectDamageDistance <= 100.f)
						Percent = 0.5f - (Vars.Misc.ProjectDamageDistance - 60.f) * 0.005f;
					Radius = Radius * Percent;
					if (Radius != 0.f && Vars.Misc.ProjectRadius && distance < Vars.Misc.ProjectRadiusDistance)
					{
						std::vector<ImVec2> points;
						float step = 6.2831f / 50.f;
						float theta = 0.f;
						FVector world_pos(aa.X, aa.Y, aa.Z);
						auto viewport = Drawing::GetViewPort();
						for (int i = 0; i < 50; i++, theta += step) {
							FVector worldSpace = { world_pos.X + Radius * cosf(theta), world_pos.Y - Radius * sinf(theta), world_pos.Z };
							FVector2D screenSpace;
							sdk::WorldToScreen2(worldSpace, screenSpace);
							points.push_back(screenSpace);
						}
						//ImGui::GetOverlayDrawList()->AddConvexPolyFilled(points.data(), points.size(), ImGui::GetColorU32(Vars.Misc.ProjectRadiusColor));
						Drawing::DrawPolyline(points.data(), points.size(), ImGui::GetColorU32(Vars.Misc.ProjectRadiusColor), true, 2.5f);
					}
					if (bSee)
					{
						if (Vars.Misc.ProjectileType)
						{
							ImRect bb2(sLoot - FVector2D(14.5, 22), sLoot + FVector2D(14.5, 7));
							if (!bMMapRadarIsOpen)
							{
								if (loot->icon.size() == 0)
									continue;
								Drawing::DrawImage(ItemManager::GetTexture()[1], bb2.Min, bb2.Max, loot->icon[1].uv0, loot->icon[1].uv1, ImColor(1.0f, 1.0f, 1.0f, 1.f));
							}
							Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, sLoot + ImVec2(0, 12), TimePercent >= 0.8f ? ImColor(1.0f, 1.0f, 1.0f, 1.0f) : TimePercent > 0.5f ? ImColor(0.0f, 1.0f, 0.0f, 1.0f) : ImColor(0.2f, 0.8f, 0.8f, 1.0f), 0xFF000000, FONT_CENTER, (u8"[%dM](%.1f)"), (int)distance, TimeTillExplosion);
						}
						else
						{
							Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, sLoot + ImVec2(0, -4), TimePercent >= 0.8f ? ImColor(1.0f, 1.0f, 1.0f, 1.0f) : TimePercent > 0.5f ? ImColor(0.0f, 1.0f, 0.0f, 1.0f) : ImColor(0.2f, 0.8f, 0.8f, 1.0f), 0xFF000000, FONT_CENTER, (u8"%s(%.1f)"), ItemDisplayName.c_str(), TimeTillExplosion);
							Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, sLoot + ImVec2(0, 12), TimePercent >= 0.8f ? ImColor(1.0f, 1.0f, 1.0f, 1.0f) : TimePercent > 0.5f ? ImColor(0.0f, 1.0f, 0.0f, 1.0f) : ImColor(0.2f, 0.8f, 0.8f, 1.0f), 0xFF000000, FONT_CENTER, (u8"[%dM]"), (int)distance);
						}
					}
				}
			}
			//CONSOLE_INFO("ExplosionRadius:%f",proj->ProjectileConfig.ExplosionRadius);
			//Drawing::DrawCircleFilled(sLoot, proj->ProjectileConfig.ExplosionRadius,0x40FFFFFF,100);
			//Drawing::DrawCircle(sLoot, proj->ProjectileConfig.ExplosionRadius, TimePercent >= 0.8f ? ImColor(0.0f, 1.0f, 0.0f, 1.0f) : TimePercent > 0.5f ? ImColor(0.0f, 1.0f, 1.0f, 1.0f) : ImColor(1.0f, 0.0f, 0.0f, 1.0f),100);
		}
		if (Vars.Misc.PorjectileLine && TimeTillExplosion >= 0.f)
		{
			ProjectPath = std::move(ProjectPathCopy);
			for (auto& p : ProjectPath)
			{
				if (!p.second)
					continue;
				std::vector<ImVec2> lines;
				for (auto& v : *p.second)
				{
					FVector2D outPos;
					sdk::WorldToScreen2(v, outPos);
					lines.push_back(outPos);
				}
				Drawing::DrawPolyline(lines.data(), (int)lines.size(), ImGui::GetColorU32(Vars.Misc.ProjectileColor), false, 1.0f);
				//int Times = 0;
				//ImVec2 lines1;
				//ImVec2 lines2;
				//for (auto& v : p.second)
				//{
				//	lines1 = sdk::WorldToScreen(v);
				//	if (lines2.x && lines2.y)
				//		Drawing::DrawLine(lines2, lines1, ImGui::FloatArrayGetColorU32(Vars.Misc.ProjectileColor), 1);
				//	lines2 = lines1;
				//}
				//Drawing::DrawPolyline(lines.data(), lines.size(), TimePercent >= 0.8f ? ImColor(0.0f, 1.0f, 0.0f, 0.548f) : TimePercent > 0.5f ? ImColor(0.0f, 1.0f, 1.0f, 0.548f) : ImColor(1.0f, 0.0f, 0.0f, 0.548f), false, 3.0f);
			}
		}
	}

	void Visuals::PlayerESP(ATslCharacter& player,
		std::unordered_map<ptr_t, std::shared_ptr<CapSuleInfo>>& inCapsule,
		std::unordered_map<ptr_t, std::vector<FVector>>& inArray)
	{
		if (!player || !player->IsValid())
			return;
		if (!player->_bInit)
			return;
		if (!player->_isInFog || player->_dist >= Vars.espHuman.DistanceSlider)
		{
			ScoreBoard LogScore;
			LogScore.TeamNumber = player->_LastTeamNum >= 100000 ? player->_LastTeamNum - 100000 : player->_LastTeamNum;
			LogScore.PlayerName = player->_ShowName;
			LogScore.TeamName = player->_TeamName;
			LogScore.Kill = player->_KillNum;
			LogScore.Assist = player->_AssistsNum;
			auto Level = (player->_SurvivalTier - 1) * 500 + player->_SurvivalLevel;
			LogScore.Level = Level;
			LogScore.Partner = player->_isParthers;
			LogScore.Damage = player->_DamageDealtOnEnemy;
			LogScore.KDA = player->_KDA;
			LogScore.Ranking = player->_CurrentRankPoint;
			LogScore.AvgDamage = player->_AvgDamage;
			LogScore.bFog = true;
			m_ScoreBoard.push_back(LogScore);
			AddScopeBoard2(LogScore);
			InFogPlayer++;
			return;
		}

		FVector2D _2DAxis, _2TAxis, _2NAxis;
		auto origin = player->_Location;
		auto delta = player->_delta;
		int 距离 = (int)player->_dist;
		bool duiyou = !!player->_Team;
		FVector FootPose;

		if (player->_Health > 0.f || player->_GroggyHealth > 0.f)
		{
			if (player->_LastTeamNum < 0 || player->_LastTeamNum > 100300)
				player->_LastTeamNum = 0;
			int TeamNumber = player->_LastTeamNum >= 100000 ? player->_LastTeamNum - 100000 : player->_LastTeamNum;
			int SurvivalLevel = (player->_SurvivalTier - 1) * 500 + player->_SurvivalLevel;
			FVector Lfoot = sdk::GetBonePosition(player, Bone::foot_l);
			FVector Rfoot = sdk::GetBonePosition(player, Bone::foot_r);
			FootPose = FVector((Lfoot.X + Rfoot.X) / 2.f, (Lfoot.Y + Rfoot.Y) / 2.f, (Lfoot.Z + Rfoot.Z) / 2.f);

			auto LastAim = player->_AimOffsets;
			if (Vars.Aimbot.Team ? !duiyou : true)
			{
				if (Vars.Misc.MRadar)
				{
					MapRadar2(player->_Location, LastAim.Yaw, player->_LastTeamNum, 1, nullptr, ImVec2(0, 0), ImVec2(0, 0), player->_ShowName);
				}
				if (Vars.Misc.Radar && !bMMapRadarIsOpen)
				{
					auto LocalHead = sdk::IsLocalPlayerAlive() ? sdk::GetLocalHeadPos() : sdk::GetLocalPos();
					auto EnemyHead = sdk::GetBonePosition(player, Bone::forehead);
					FVector ThisDelta = EnemyHead - LocalHead;
					定义雷达图(ThisDelta, player->_LastTeamNum, LastAim);
				}
			}

			if (Vars.espRank.Rank)
			{
				if (TeamNumber < 300)
				{
					if (Utils::StringIsValid(player->_PlayerName))
					{
						auto targetName = player->_RankName;
						if (Utils::StringIsValid(targetName))
						{
							auto targetHashName = hash_(targetName);
							if (SurvivalLevel > 0)
							{
								if (TeamNumber >= 200 || SurvivalLevel < 80)
								{
									auto AreadyCache = GetAlreadyCachePlayer();
									if (AreadyCache.find(targetHashName) == AreadyCache.end())
									{
										AreadyCache.emplace(targetHashName, std::make_shared<AllRankModeInfo>());
										SetAlreadyCachePlayer(AreadyCache);
									}
								}
								else
								{
									auto UnCache = GetUnCachePlayer();
									if (UnCache.find(targetHashName) == UnCache.end())//未在缓存列表继续下行
									{
										if (!IsInAlreadyCache(targetHashName))//是否缓存完成
										{
											UnCache.emplace(hash_(targetName.c_str()), std::make_shared<std::string>(targetName));
											SetUnCachePlayer(UnCache);
										}
									}
								}
							}
						}
					}
				}
				bool bHasInfo = bHasNetPlayerInfo(player->_RankName);
				if (bHasInfo)
				{
					static bool bThridPerson = false;
					auto netInfo = getNetPlayerInfo(player->_RankName);
					if (netInfo)
					{
						bool ThisThridPerson = sdk::GetbThirdPerson() == 1 ? true : false;
						bool PlayerAimState = sdk::GetFireState() == 2 ? true : false;
						if (!PlayerAimState)
							bThridPerson = ThisThridPerson;
						if (!sdk::IsLocalPlayerAlive())
							bThridPerson = true;
						auto SeleteInfo = !bThridPerson ? netInfo->FPP : netInfo->TPP;
						auto Info_Main = !Vars.espRank.Mode ? SeleteInfo.squad : SeleteInfo.solo;
						player->_KDA = Info_Main.kda;
						player->_CurrentRankPoint = Info_Main.currentRankPoint;
						player->_AvgDamage = Info_Main.damage;
						player->_RankText = GetRankTier(Info_Main.currentRankPoint);
						player->_RankIcon = GetEnRankTier(Info_Main.currentRankPoint);
					}
					else
					{
						player->_KDA = 0.f;
						player->_CurrentRankPoint = 0;
						player->_AvgDamage = 0.f;
						player->_RankText = "";
						player->_RankIcon = "";
					}
				}
				else
				{
					player->_KDA = 0.f;
					player->_CurrentRankPoint = 0;
					player->_RankText = "";
					player->_RankIcon = "";
				}

				if (duiyou)
				{
					if (player->_CurrentRankPoint > 0)
					{
						bool IsExist = false;
						for (int i = 0; i < TeamMateKDList.size(); i++)
						{
							if (hash_(TeamMateKDList[i].Name) == hash_(player->_RankName))
							{
								TeamMateKDList[i] = TeamMateStruct(player->_RankName, GetRankTier(player->_CurrentRankPoint), player->_bBlackList, player->_isParthers, player->_KDA, player->_LastTeamNum);
								IsExist = true;
								break;
							}
						}
						if (!IsExist && !player->_RankName.empty())
							TeamMateKDList.push_back(TeamMateStruct(player->_RankName, GetRankTier(player->_CurrentRankPoint), player->_bBlackList, player->_isParthers, player->_KDA, player->_LastTeamNum));
					}
					else
					{
						bool IsExist = false;
						for (int i = 0; i < TeamMateKDList.size(); i++)
						{
							if (hash_(TeamMateKDList[i].Name) == hash_(player->_RankName))
							{
								TeamMateKDList[i] = TeamMateStruct(player->_RankName, LOCALIZE_Visual("UnRanked", u8"未定级"), player->_bBlackList, player->_isParthers, 0.f, player->_LastTeamNum);
								IsExist = true;
								break;
							}
						}
						if (!IsExist && !player->_RankName.empty())
							TeamMateKDList.push_back(TeamMateStruct(player->_RankName, LOCALIZE_Visual("UnRanked", u8"未定级"), player->_bBlackList, player->_isParthers, 0.f, player->_LastTeamNum));
					}
				}
			}


			if (!Vars.Aimbot.Team ? false : duiyou)
				return;

			bool 可视 = sdk::PlayerVisibleCheck(player);
			if (!sdk::IsLocalPlayerAlive())
				可视 = true;
			g_physic->AddActorResult((ptr_t)player->GetPtr());
			if (Vars.Menu.PhysxTrace && g_physic->IsSceneCreate())
			{
				PxQuat NormalRot = g_physic->FQuatToPxQuat(player->_ComponentToWorld.Rotation);
				static int boneScans[] = {
					forehead,Head,neck_01,spine_03,spine_02,spine_01,upperarm_l,upperarm_r,lowerarm_l,lowerarm_r,hand_l,hand_r,thumb_01_l,thumb_01_r,thigh_l,thigh_r,calf_l,calf_r,foot_l,foot_r,ik_foot_root,ik_foot_l,ik_foot_r,23
				};
				for (auto iter : boneScans)
				{
					if (iter == 23)
						break;
					inCapsule.emplace((ptr_t)player->GetPtr() + iter, std::make_shared<CapSuleInfo>(CapSuleInfo(iter == 0 ? 24.f : 12.f, 0.f, PxTransform(g_physic->FVectorToPxVec3(sdk::GetBonePosition(player, iter)), NormalRot), (ptr_t)player->GetPtr() + iter)));
				}
				if (可视)
				{

					std::vector<FVector>HitPos;
					HitPos.resize(ARRAY_COUNT(boneScans));
					for (int i = 0; i < ARRAY_COUNT(boneScans); i++)
					{
						if (i == 23)
							HitPos[i] = sdk::GetBonePosition(player, forehead);
						else
							HitPos[i] = sdk::GetBonePosition(player, boneScans[i]);
					}
					inArray.emplace((ptr_t)player->GetPtr(), HitPos);
					HitPos.clear();
				}
				else
				{
					static int boneScans[] = {
						forehead,Head,neck_01,spine_03,spine_02,spine_01,upperarm_l,upperarm_r,lowerarm_l,lowerarm_r,hand_l,hand_r,thumb_01_l,thumb_01_r,thigh_l,thigh_r,calf_l,calf_r,foot_l,foot_r,ik_foot_root,ik_foot_l,ik_foot_r,23
					};
					std::vector<FVector>HitPos;
					HitPos.resize(ARRAY_COUNT(boneScans));
					for (int i = 0; i < ARRAY_COUNT(boneScans); i++)
					{
						if (i == 23)
							HitPos[i] = sdk::GetBonePosition(player, forehead);
						else
							HitPos[i] = FVector();
					}
					inArray.emplace((ptr_t)player->GetPtr(), HitPos);
					HitPos.clear();
				}
			}

			ScoreBoard LogScore;
			LogScore.TeamNumber = player->_LastTeamNum >= 100000 ? player->_LastTeamNum - 100000 : player->_LastTeamNum;
			LogScore.PlayerName = player->_ShowName;
			LogScore.TeamName = player->_TeamName;
			LogScore.Kill = player->_KillNum;
			LogScore.Assist = player->_AssistsNum;
			auto Level = (player->_SurvivalTier - 1) * 500 + player->_SurvivalLevel;
			LogScore.Level = Level;
			LogScore.Partner = player->_isParthers;
			LogScore.Damage = player->_DamageDealtOnEnemy;
			LogScore.KDA = player->_KDA;
			LogScore.Ranking = player->_CurrentRankPoint;
			LogScore.AvgDamage = player->_AvgDamage;
			LogScore.bFog = false;
			m_ScoreBoard.push_back(LogScore);
			AddScopeBoard2(LogScore);

			FVector Poses = sdk::GetBonePosition(player, Bone::forehead);
			if (Vars.Misc.Warning && 距离 <= (int)(Vars.espHuman.DistanceSlider) && sdk::IsLocalPlayerAlive())
				PlayerAimPosition(player, Poses, 80.f - delta.Size() / 100.0f / 40.f, Settings::ColorGreen, (float)距离);

			if (!sdk::WorldToScreen(player->_CacheLocation, _2NAxis))
				return;
			if (!sdk::WorldToScreen(FootPose, _2DAxis))
				return;
			if (!sdk::WorldToScreen(Poses, _2TAxis))
				return;
			if (Vars.Aimbot.DebugMode && Vars.Aimbot.RayTracing && 可视 && g_physic->IsSceneCreate())
			{
				FVector LocalPos;
				if (!sdk::GetIsAiming_CP())
					LocalPos = sdk::GetLocalPos();
				else
					LocalPos = sdk::GetFiringLocation();
				g_physic->WeaponCanHit(LocalPos, Poses, 200000.f, true);
			}
			if (距离 <= (int)(Vars.espHuman.DistanceSlider) && 距离 >= 0)
			{
				int X = _2TAxis.X; int Y = _2TAxis.Y; int vX = _2DAxis.X; int vY = _2DAxis.Y; int BoxX = X - ((vY - Y) / 4); int BoxY = Y; int BoxW = (vY - Y) / 2; int BoxH = vY - Y;
				bool InternalVisibleCheck = false;
				if (g_physic->IsSceneCreate() && sdk::IsLocalPlayerAlive() && 可视)
					InternalVisibleCheck = g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::forehead);
				if (Vars.espHuman.Box && !bMMapRadarIsOpen)//方框
				{
					Drawing::DrawOutlinedRect(ImVec2(BoxX, BoxY), ImVec2(BoxX + BoxW, BoxY + BoxH), InternalVisibleCheck ? (m_Target == player.GetPtr() ? ImGui::FloatArrayGetColorU32(Vars.espHuman.AllLockColor) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonVis)) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonInVis), 2.f);
					//Drawing::DrawOutlinedRect(ImVec2(BoxX, BoxY), ImVec2(BoxX + BoxW, BoxY + BoxH), sdk::PlayerVisibleCheck(player) ? (m_Target == player.GetPtr() ? ImGui::FloatArrayGetColorU32(Vars.espHuman.AllLockColor) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonVis)) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonInVis), 2.f);
					//Drawing::DrawBox_S(ImVec2(BoxX, BoxY), ImVec2(BoxW, BoxH), InternalVisibleCheck ? (m_Target == player.GetPtr() ? ImGui::FloatArrayGetColorU32(Vars.espHuman.AllLockColor) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonVis)) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonInVis), 5.f, 2.f);
				}

				if (Vars.espHuman.Skeleton && !bMMapRadarIsOpen)
#ifdef DebugTrace
					PlayerSkeleton(player, Vars.espHuman.Thinkness, 距离, m_Target == player.GetPtr() ? ImGui::FloatArrayGetColorU32(Vars.espHuman.AllLockColor) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonVis), ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonInVis), true);
#else	
					PlayerSkeleton(player, Vars.espHuman.Thinkness, 距离, m_Target == player.GetPtr() ? ImGui::FloatArrayGetColorU32(Vars.espHuman.AllLockColor) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonVis), ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonInVis), 可视);
#endif
				if (Vars.espHuman.HeadCircle && !bMMapRadarIsOpen)
				{
					FVector2D screen;
					if (sdk::WorldToScreen(Poses, screen))//鼻子
						Drawing::DrawCircle(screen, ceil(BoxH / (距离 < 20.0f ? 10.0f : 8.0f)),
							(!g_physic->IsSceneCreate() ? sdk::PlayerVisibleCheck(player) : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::forehead)) ? (m_Target == player.GetPtr() ? ImGui::FloatArrayGetColorU32(Vars.espHuman.AllLockColor) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonVis)) : ImGui::FloatArrayGetColorU32(Vars.espHuman.SkeletonInVis), 50, Vars.espHuman.Thinkness);
				}

				bool bSpawnBulletFromBarrel = sdk::GetbSpawnBulletFromBarrel();
				if ((距离 <= Vars.espHuman.SimpleInfoSlider || bSpawnBulletFromBarrel) && !bMMapRadarIsOpen)
				{
					PlayerHealthBar(player, X, 距离, 0, _2DAxis, TeamNumber);
					bool ShowNextLine = false;
					auto KillCountColor = player->_KillNum <= 1 ? Settings::ColorWhite : player->_KillNum < 5 ? Settings::ColorGreen : Settings::ColorYellow;
					auto RankColor = player->_KDA <= 1.f ? Settings::ColorWhite : player->_KDA < 2.f ? Settings::ColorGreen : player->_KDA < 3.f ? Settings::ColorLightBlue : player->_KDA <= 3.5f ? Settings::ColorYellow : Settings::ColorRed;
					if (Vars.espHuman.KillCount)
					{
						if (player->_RankText.size() && ("Unranked") != player->_RankText && player->_KDA > 0.0f)
						{
							if ((!Vars.espRank.Rank && !Vars.espRank.KD) || Vars.espRank.RankTexture)
								Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, _2DAxis + ImVec2(0, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : KillCountColor, 0xFF000000, FONT_CENTER, LOCALIZE_Visual("Kill:%d", u8"击杀:%d"), player->_KillNum);
							else if (Vars.espRank.Rank && Vars.espRank.KD)
							{
								Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize - 1, 0, _2DAxis + ImVec2(-2, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : KillCountColor, 0xFF000000, FONT_RIGHT, LOCALIZE_Visual("Kill:%d", u8"击杀:%d"), player->_KillNum);
								Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize - 1, 0, _2DAxis + ImVec2(2, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : RankColor, 0xFF000000, FONT_LEFT, u8"%s/%.1f", player->_RankText.c_str(), player->_KDA);
							}
							else if (Vars.espRank.Rank && !Vars.espRank.KD)
							{
								Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize - 1, 0, _2DAxis + ImVec2(-2, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : KillCountColor, 0xFF000000, FONT_RIGHT, LOCALIZE_Visual("Kill:%d", u8"击杀:%d"), player->_KillNum);
								Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize - 1, 0, _2DAxis + ImVec2(2, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : RankColor, 0xFF000000, FONT_LEFT, LOCALIZE_Visual("%s", u8"%s"), player->_RankText.c_str());
							}
							else
							{
								Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize - 1, 0, _2DAxis + ImVec2(-2, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : KillCountColor, 0xFF000000, FONT_RIGHT, LOCALIZE_Visual("Kill:%d", u8"击杀:%d"), player->_KillNum);
								Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize - 1, 0, _2DAxis + ImVec2(2, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : RankColor, 0xFF000000, FONT_LEFT, LOCALIZE_Visual("%.1f", u8"%.1f"), player->_KDA);
							}
						}
						else
							Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, _2DAxis + ImVec2(0, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : KillCountColor, 0xFF000000, FONT_CENTER, LOCALIZE_Visual("Kill:%d", u8"击杀:%d"), player->_KillNum);
						ShowNextLine = true;
					}
					else if ((Vars.espRank.Rank || Vars.espRank.KD) && player->_RankText.size() && ("Unranked") != player->_RankText)
					{
						if (Vars.espRank.Rank && Vars.espRank.KD)
							Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, _2DAxis + ImVec2(0, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : RankColor, 0xFF000000, FONT_CENTER, LOCALIZE_Visual("%s/%.1f", u8"%s/%.1f"), player->_RankText.c_str(), player->_KDA);
						else if (Vars.espRank.Rank && !Vars.espRank.KD)
							Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, _2DAxis + ImVec2(0, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : RankColor, 0xFF000000, FONT_CENTER, LOCALIZE_Visual("%s", u8"%s"), player->_RankText.c_str());
						else
							Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, _2DAxis + ImVec2(0, 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : RankColor, 0xFF000000, FONT_CENTER, LOCALIZE_Visual("%.1f", u8"%.1f"), player->_KDA);
						ShowNextLine = true;
					}

					if (Vars.espHuman.Distance)
					{
						if (!Vars.espHuman.SuvivalLevel)
						{
							Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, _2DAxis + (ShowNextLine ? ImVec2(0, 23.0f + Vars.espHuman.TextSize - 16) : ImVec2(0, 10.0f)), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : ImGui::FloatArrayGetColorU32(Vars.espHuman.DistanceColor), 0xFF000000, FONT_CENTER, ("[%dM]"), 距离);
						}
						else
						{
							Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, _2DAxis + (ShowNextLine ? ImVec2(-12.f, 23.0f + Vars.espHuman.TextSize - 16) : ImVec2(-12.f, 10.0f)), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : ImGui::FloatArrayGetColorU32(Vars.espHuman.DistanceColor), 0xFF000000, FONT_RIGHT, ("[%dM]"), 距离);
							Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, _2DAxis + (ShowNextLine ? ImVec2(0, 23.0f + Vars.espHuman.TextSize - 16) : ImVec2(0, 10.0f)), player->_bBlackList ? 0xFF0000FF : ImGui::FloatArrayGetColorU32(Vars.espHuman.SuvivalLevelColor), 0xFF000000, FONT_LEFT, LOCALIZE_Visual("%dTier-%d", u8"%d阶-%d"), player->_SurvivalTier, player->_SurvivalLevel);
						}
					}
					else if (Vars.espHuman.SuvivalLevel)
					{
						Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, _2DAxis + (ShowNextLine ? ImVec2(0, 23.0f + Vars.espHuman.TextSize - 16) : ImVec2(0, 10.0f)), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : ImGui::FloatArrayGetColorU32(Vars.espHuman.SuvivalLevelColor), 0xFF000000, FONT_CENTER, LOCALIZE_Visual("%dTier-%d", u8"%d阶-%d"), player->_SurvivalTier, player->_SurvivalLevel);
					}
				}
				else
				{
					Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espHuman.TextSize, 0, ImVec2(X, Y - 10.0f), player->_isParthers ? ImGui::FloatArrayGetColorU32(Vars.espHuman.CollaboratorColor) : player->_bBlackList ? 0xFF0000FF : ImGui::GetColorU32(Vars.espHuman.DistanceColor), 0xFF000000, FONT_CENTER, ("[%dM]"), 距离);
				}
			}
		}
	}

	void ShotFire()
	{
		if (!bControllerPress)
		{
			if (Vars.Aimbot.AimBotMethod == 1 && Vars.Aimbot.BProStatus == 1)
			{
				GetKmbox().downLeft(1);
				bControllerPress = true;
			}
			else if (Vars.Aimbot.AimBotMethod == 2 && Vars.Aimbot.BNetStatus == 1)
			{
				kmNet_mouse_left(1);
				bControllerPress = true;
			}
			else if (Vars.Aimbot.AimBotMethod == 3 && Vars.Aimbot.LurkerStatus == 1)
			{
				GetKmbox().ClickLurker(VK_LBUTTON);
				bControllerPress = true;
			}
			else if (Vars.Aimbot.AimBotMethod == 4 && Vars.Aimbot.JSMHStatus == 1)
			{
				GetKmbox().LeftDownJSMH();
				bControllerPress = true;
			}
			bPressLogTime = std::chrono::high_resolution_clock::now();
		}
		return;
	}

	void ARShotFire()
	{
		if (!bControllerPress)
		{
			if (Vars.Aimbot.AimBotMethod == 1 && Vars.Aimbot.BProStatus == 1)
			{
				GetKmbox().downLeft(1);
				bControllerPress = true;
			}
			else if (Vars.Aimbot.AimBotMethod == 2 && Vars.Aimbot.BNetStatus == 1)
			{
				kmNet_mouse_left(1);
				bControllerPress = true;
			}
			else if (Vars.Aimbot.AimBotMethod == 3 && Vars.Aimbot.LurkerStatus == 1)
			{
				GetKmbox().ClickLurker(VK_LBUTTON);
				bControllerPress = true;
			}
			else if (Vars.Aimbot.AimBotMethod == 4 && Vars.Aimbot.JSMHStatus == 1)
			{
				GetKmbox().LeftDownJSMH();
				bControllerPress = true;
			}
			bPressLogTime = std::chrono::high_resolution_clock::now();
		}
		return;
	}

	void Visuals::PlayerTrigger(ATslCharacter& player)
	{
		if (!player || !player->IsValid())
			return;
		if (player->_Health == 0.f && player->_GroggyHealth == 0.f)
			return;
		bool duiyou = !!player->_Team;
		if (!Vars.Aimbot.Team ? false : duiyou)
			return;
		if (CheckPlayerIsInWhiteListMap(hash_(player->_PlayerName)))
			return;
		int 距离 = (int)player->_dist;
		ATslCharacter target = player->GetPtr();
		FVector2D screenPos;
		if (距离 <= (int)(Vars.espHuman.DistanceSlider) && 距离 > 0 && sdk::WorldToScreen(player->_CacheLocation, screenPos))
		{
			auto ViewPort = sdk::GetbSpawnBulletFromBarrel() ? sdk::GetFiringLocation() : sdk::GetLocalPos();
			auto bVisible = sdk::PlayerVisibleCheck(player);

			if (bVisible && Vars.Aimbot.ShotGunAutoShot && sdk::GetIsShotGun() && sdk::GetWeaponIndex() != 255 && 距离 <= 40)
			{
				auto foreheadPos = sdk::GetBonePosition(player, Bone::forehead);
				auto neckPos = sdk::GetBonePosition(player, Bone::neck_01);
				auto upperarmlPos = sdk::GetBonePosition(player, Bone::upperarm_l);
				auto upperarmrPos = sdk::GetBonePosition(player, Bone::upperarm_r);
				auto dangbuPos = sdk::GetBonePosition(player, Bone::spine_01);
				auto thighlPos = sdk::GetBonePosition(player, Bone::thigh_l);
				auto thighrPos = sdk::GetBonePosition(player, Bone::thigh_r);
				auto chestPos = (dangbuPos + neckPos) / 2.f;

				if (!foreheadPos.IsZero() && !neckPos.IsZero() && !upperarmlPos.IsZero() && !upperarmrPos.IsZero() && !chestPos.IsZero() && !dangbuPos.IsZero() && !thighlPos.IsZero() && !thighrPos.IsZero())
				{
					auto drawScreen = sdk::WorldToScreen(chestPos);
					auto neckScreen = sdk::WorldToScreen(neckPos);
					auto upperarmlScreen = sdk::WorldToScreen(upperarmlPos);
					auto upperarmrScreen = sdk::WorldToScreen(upperarmrPos);
					auto dangbuPosScreen = sdk::WorldToScreen(dangbuPos);
					auto thighlScreen = sdk::WorldToScreen(thighlPos);
					auto thighrScreen = sdk::WorldToScreen(thighrPos);
					auto Radius = std::max(Math::Abs(drawScreen.Y - neckScreen.Y), Math::Abs(neckScreen.X - upperarmlScreen.X));
					Radius = std::max(Radius, Math::Abs(neckScreen.X - upperarmrScreen.X));
					auto inCircle = PointIsInCirCle(drawScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), spine_01)))
					{
						ShotFire();
						return;
					}
					auto foreheadScreen = sdk::WorldToScreen(foreheadPos);
					Radius = Math::Abs(neckScreen.Y - foreheadScreen.Y);
					inCircle = PointIsInCirCle(foreheadScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), forehead)))
					{
						ShotFire();
						return;
					}
					inCircle = PointIsInCirCle(neckScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), neck_01)))
					{
						ShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(neckScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);
					Radius = std::max(Math::Abs(thighrScreen.Y - dangbuPosScreen.Y), Math::Abs(thighlScreen.Y - dangbuPosScreen.Y));
					inCircle = PointIsInCirCle(dangbuPosScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), spine_01)))
					{
						ShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(foreheadScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);
				}
			}

			if (bVisible && sdk::GetIsScoping_CP() && Vars.Aimbot.RiferAutoShot && !sdk::GetIsShotGun() && !sdk::GetIsDMR() && !sdk::GetIsSR() && !sdk::GetIsGrenade() && sdk::GetWeaponIndex() != 255)
			{
				float dropPitchOffset = 0.f;
				auto PlayerVelocity = Aimbot::GetPlayerVelocity(player);
				auto OriginalForeHead = sdk::GetBonePosition(player, Bone::forehead);
				auto foreheadPos = Aimbot::GetPrediction(player, OriginalForeHead, dropPitchOffset, PlayerVelocity);
				float dist = (player->_Location - sdk::GetLocalPos()).Size() / 100.0f;
				float z = foreheadPos.Z;
				foreheadPos += PlayerVelocity * Aimbot::GetNextPos(player, dist, PlayerVelocity);
				foreheadPos.Z = z;
				FVector SubPos = foreheadPos - OriginalForeHead;
				auto neckPos = sdk::GetBonePosition(player, Bone::neck_01) + SubPos;
				auto upperarmlPos = sdk::GetBonePosition(player, Bone::upperarm_l) + SubPos;
				auto upperarmrPos = sdk::GetBonePosition(player, Bone::upperarm_r) + SubPos;
				auto dangbuPos = sdk::GetBonePosition(player, Bone::spine_01) + SubPos;
				auto ElbowLPos = sdk::GetBonePosition(player, Bone::lowerarm_l) + SubPos;
				auto ElbowRPos = sdk::GetBonePosition(player, Bone::lowerarm_r) + SubPos;
				auto handLPos = sdk::GetBonePosition(player, Bone::hand_l) + SubPos;
				auto handRPos = sdk::GetBonePosition(player, Bone::hand_r) + SubPos;

				auto thighlPos = sdk::GetBonePosition(player, Bone::thigh_l) + SubPos;
				auto thighrPos = sdk::GetBonePosition(player, Bone::thigh_r) + SubPos;
				auto KneelPos = sdk::GetBonePosition(player, Bone::calf_l) + SubPos;
				auto KneerPos = sdk::GetBonePosition(player, Bone::calf_r) + SubPos;
				auto FootlPos = sdk::GetBonePosition(player, Bone::foot_l) + SubPos;
				auto FootrPos = sdk::GetBonePosition(player, Bone::foot_r) + SubPos;
				auto chestPos = (dangbuPos + neckPos) / 2.f;
				if (!foreheadPos.IsZero() && !neckPos.IsZero() && !upperarmlPos.IsZero() && !upperarmrPos.IsZero() && !chestPos.IsZero())
				{
					auto drawScreen = sdk::WorldToScreen(chestPos);
					auto neckScreen = sdk::WorldToScreen(neckPos);
					auto upperarmlScreen = sdk::WorldToScreen(upperarmlPos);
					auto upperarmrScreen = sdk::WorldToScreen(upperarmrPos);

					auto ElbowLScreen = sdk::WorldToScreen(ElbowLPos);
					auto ElbowRScreen = sdk::WorldToScreen(ElbowRPos);

					auto MiddlearmLScreen = (upperarmlScreen + ElbowLScreen) / 2.f;
					auto MiddlearmRScreen = (upperarmrScreen + ElbowRScreen) / 2.f;

					auto handLScreen = sdk::WorldToScreen(handLPos);
					auto handRScreen = sdk::WorldToScreen(handRPos);

					auto MiddleHandLScreen = (ElbowLScreen + handLScreen) / 2.f;
					auto MiddleHandRScreen = (ElbowRScreen + handRScreen) / 2.f;

					auto dangbuPosScreen = sdk::WorldToScreen(dangbuPos);
					auto thighlScreen = sdk::WorldToScreen(thighlPos);
					auto thighrScreen = sdk::WorldToScreen(thighrPos);

					auto KneelScreen = sdk::WorldToScreen(KneelPos);
					auto KneerScreen = sdk::WorldToScreen(KneerPos);

					auto FootlScreen = sdk::WorldToScreen(FootlPos);
					auto FootrScreen = sdk::WorldToScreen(FootrPos);

					auto MiddlethighLScreen = (thighlScreen + KneelScreen) / 2.f;
					auto MiddlethighRScreen = (thighrScreen + KneerScreen) / 2.f;

					auto MiddlefootLScreen = (KneelScreen + FootlScreen) / 2.f;
					auto MiddlefootRScreen = (KneerScreen + FootrScreen) / 2.f;

					//胸部
					auto Radius = std::max(Math::Abs(drawScreen.Y - neckScreen.Y), Math::Abs(neckScreen.X - upperarmlScreen.X));
					Radius = std::max(Radius, Math::Abs(neckScreen.X - upperarmrScreen.X)) * 1.25f;
					auto inCircle = PointIsInCirCle(drawScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::spine_01)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(drawScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//头部
					auto foreheadScreen = sdk::WorldToScreen(foreheadPos);
					Radius = Math::Abs(neckScreen.Y - foreheadScreen.Y) * 1.15f;
					inCircle = PointIsInCirCle(foreheadScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::Head)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(foreheadScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//颈部
					inCircle = PointIsInCirCle(neckScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::neck_01)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(neckScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//裆部
					Radius = std::max(Math::Abs(thighrScreen.Y - dangbuPosScreen.Y), Math::Abs(thighlScreen.Y - dangbuPosScreen.Y)) * 1.75f;
					inCircle = PointIsInCirCle(dangbuPosScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::spine_02)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(dangbuPosScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//左肩膀
					Radius = Math::Abs(neckScreen.Y - upperarmlScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(upperarmlScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::upperarm_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(upperarmlScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//右肩膀
					Radius = Math::Abs(neckScreen.Y - upperarmrScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(upperarmrScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::upperarm_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(upperarmrScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);


					//左手肘
					Radius = Math::Abs(upperarmlScreen.Y - ElbowLScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(ElbowLScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::lowerarm_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(ElbowLScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//左手肘-左肩膀之间
					inCircle = PointIsInCirCle(MiddlearmLScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::lowerarm_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(MiddlearmLScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//右手肘-右肩膀之间
					inCircle = PointIsInCirCle(MiddlearmRScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::lowerarm_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(MiddlearmRScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//右手肘
					Radius = Math::Abs(upperarmrScreen.Y - ElbowRScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(ElbowRScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::lowerarm_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(ElbowRScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//左手指
					Radius = Math::Abs(ElbowLScreen.Y - handLScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(handLScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::hand_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(handLScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//右手指
					Radius = Math::Abs(ElbowRScreen.Y - handRScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(handRScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::hand_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(handRScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);


					//左手肘-左手指之间
					inCircle = PointIsInCirCle(MiddleHandLScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::hand_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(MiddleHandLScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//右手肘-右手指之间
					inCircle = PointIsInCirCle(MiddleHandRScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::hand_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(MiddleHandRScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//左裆部到大腿
					Radius = Math::Abs(thighlScreen.Y - dangbuPosScreen.Y) * 1.75f;
					inCircle = PointIsInCirCle(thighlScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::thigh_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(thighlScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//左大腿到膝盖
					Radius = Math::Abs(thighlScreen.Y - KneelScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(KneelScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::calf_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(KneelScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					inCircle = PointIsInCirCle(MiddlethighLScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::calf_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(MiddlethighLScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//左膝盖 - 左脚趾
					Radius = Math::Abs(KneelScreen.Y - FootlScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(FootlScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::foot_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(FootlScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					inCircle = PointIsInCirCle(MiddlefootLScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::calf_l)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(MiddlefootLScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//右裆部到大腿
					Radius = Math::Abs(thighrScreen.Y - dangbuPosScreen.Y) * 1.75f;
					inCircle = PointIsInCirCle(thighrScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::thigh_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(thighrScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//右大腿到膝盖
					Radius = Math::Abs(thighrScreen.Y - KneerScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(KneerScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::calf_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(KneerScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					inCircle = PointIsInCirCle(MiddlethighRScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::calf_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(MiddlethighRScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					//右膝盖 - 右脚趾
					Radius = Math::Abs(KneerScreen.Y - FootrScreen.Y) * 1.25f;
					inCircle = PointIsInCirCle(FootrScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::foot_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(FootrScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);

					inCircle = PointIsInCirCle(MiddlefootRScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), Bone::calf_r)))
					{
						ARShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(MiddlefootRScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);
				}

			}
			else
				ARLogTimeToShot = 0;

			if (sdk::GetIsSR() && sdk::GetIsScoping_CP())
			{
				if (LogTimeToShot == 0 || !LogTimeToShot)
				{
					LogTimeToShot = GetTickCount64();
				}
				if (GetTickCount64() - LogTimeToShot < Vars.Aimbot.SRAutoShotDelay1)
					return;
			}
			else
				LogTimeToShot = 0;

			if (bVisible && Vars.Aimbot.SRGunAutoShot /*&& 距离 <= 250*/ && !sdk::GetIsReloading_CP()
				&& GetTickCount64() - LogTimeToShot > Vars.Aimbot.SRAutoShotDelay2 && LogTimeToShot != 0)
			{
				float dropPitchOffset = 0.f;
				auto PlayerVelocity = Aimbot::GetPlayerVelocity(player);
				auto OriginalForeHead = sdk::GetBonePosition(player, Bone::forehead);
				auto foreheadPos = Aimbot::GetPrediction(player, OriginalForeHead, dropPitchOffset, PlayerVelocity);
				float dist = (player->_Location - sdk::GetLocalPos()).Size() / 100.0f;
				float z = foreheadPos.Z;
				foreheadPos += PlayerVelocity * Aimbot::GetNextPos(player, dist, PlayerVelocity);
				foreheadPos.Z = z;
				FVector SubPos = foreheadPos - OriginalForeHead;
				auto neckPos = sdk::GetBonePosition(player, Bone::neck_01) + SubPos;
				auto upperarmlPos = sdk::GetBonePosition(player, Bone::upperarm_l) + SubPos;
				auto upperarmrPos = sdk::GetBonePosition(player, Bone::upperarm_r) + SubPos;
				auto dangbuPos = sdk::GetBonePosition(player, Bone::spine_01) + SubPos;
				auto chestPos = (dangbuPos + neckPos) / 2.f;
				if (!foreheadPos.IsZero() && !neckPos.IsZero() && !upperarmlPos.IsZero() && !upperarmrPos.IsZero() && !chestPos.IsZero())
				{
					auto drawScreen = sdk::WorldToScreen(chestPos);
					auto neckScreen = sdk::WorldToScreen(neckPos);
					auto upperarmlScreen = sdk::WorldToScreen(upperarmlPos);
					auto upperarmrScreen = sdk::WorldToScreen(upperarmrPos);
					auto Radius = std::max(Math::Abs(drawScreen.Y - neckScreen.Y), Math::Abs(neckScreen.X - upperarmlScreen.X));
					Radius = std::max(Radius, Math::Abs(neckScreen.X - upperarmrScreen.X));
					auto inCircle = PointIsInCirCle(drawScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), spine_01)))
					{
						ShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(drawScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);
					auto foreheadScreen = sdk::WorldToScreen(foreheadPos);
					Radius = Math::Abs(neckScreen.Y - foreheadScreen.Y);
					inCircle = PointIsInCirCle(foreheadScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), forehead)))
					{
						ShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(foreheadScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);
					inCircle = PointIsInCirCle(neckScreen, Radius, Drawing::GetViewPort() / 2.f);
					if (inCircle && Radius < 1000.f && (!g_physic->IsSceneCreate() ? bVisible : g_physic->GetActorVisible((ptr_t)player->GetPtr(), neck_01)))
					{
						ShotFire();
						return;
					}
					//Drawing::DrawCircleFilled(neckScreen, Radius, inCircle ? Settings::ColorRed : Settings::ColorWhite);
				}
			}

		}
	}

	void Visuals::LootGroupESP(ADroppedItemGroup& lootGroup)
	{
		if (!lootGroup || !lootGroup->IsValid())
		{
			CONSOLE_INFO("lootGroup");
			return;
		}
		if (!Vars.espItem.Item)
			return;
		if (!Vars.espItem.ItemPictures && !Vars.espItem.ItemText)
			return;

		if (!lootGroup->_ValidItem)
		{
			CONSOLE_INFO("validitem");
			return;
		}
		auto origin = lootGroup->_Location;
		auto distanceItem = (origin - sdk::GetLocalPos()).Size() / 100.0f;

		int i = 1, n = 0;

		if (lootGroup->_ItemArray.empty() || lootGroup->_ItemArray.size() > 100 || lootGroup->_digaArray.size() > 100)
			return;

		float iconRadius = Vars.espItem.ImageSize * 0.5f;
		for (auto& Item : lootGroup->_ItemArray)
		{
			static FVector previousPos;
			if (!IsAddrValid(Item.obj) || !IsAddrValid(Item.UItem) || !IsAddrValid(Item.ItemTableRow) || !IsAddrValid(Item.TempUItem))
				continue;
			if (Item.RelativeLocation.IsZero() || origin.IsZero())
				continue;
			FVector2D sLoot;
			auto location = origin + Item.RelativeLocation;
			auto distance = (location - sdk::GetLocalPos()).Size() / 100.0f;
			if (distance > Vars.espItem.ItemDistance)
				continue;
			if (Item.ItemIDName.empty() || Item.ItemIDName == "")
				continue;
			if (distance < 5.f)
				Sleep(0);
			auto ItemAllInfo = ItemManager::FindItemAllInfo(hash_(Item.ItemIDName));

			std::string itemName = ItemAllInfo.ItemName;

			std::string itemCategoryName = ItemAllInfo.ItemCategory;

			if (itemName.empty() || itemCategoryName.empty() || itemName == "" || itemCategoryName == "")
			{
				//if (sdk::WorldToScreen(location, sLoot))
				//	Drawing::DrawTextOutline(OverlayEngine::Font14px, 18.f, 0, sLoot, 0xFF0000FF, 0xFF000000, 0, "%s", Item.ItemIDName.c_str());
				continue;
			}

			LootCategory itemCategory = GetLootCategory(itemCategoryName);
			hash_t itemHashName = hash_(itemName);
			float iconRadius = Vars.espItem.ImageSize * 0.5f;
			if (itemCategory != LootCategory::Unknown)
			{
				const auto& loots = ItemManager::GetGroupManager()->GetLoots()[itemCategory];
				if (loots.empty())
					continue;
				auto lootInfo = loots.find(itemHashName);
				if (lootInfo == loots.end())
				{
					if (itemName.find(("M_ItemLegacy_1226")) != std::string::npos)
						itemHashName = HASH("M_ItemLegacy_1126");
					else if (Item.ItemIDName.find("Ghillie") != std::string::npos)
						itemHashName = HASH("M_ItemLegacy_1126");
					else
						continue;

					lootInfo = loots.find(itemHashName);
					if (lootInfo == loots.end())
						continue;
				}
				auto lootItem = lootInfo->second;
				if (lootItem == nullptr)
					continue;

				bool SkipNext = false;
				auto result = SmartWeapon();
				if (Vars.espItem.SmartItem && result != SmartItemID::NoWeapon)
				{
					if (lootItem->id >= 1100 && lootItem->id < 1200)
					{
						if (SmartSeleteAttachment(lootItem->id, itemHashName) == SmartItemID::SuitAttach)
							SkipNext = true;
						else
							continue;
					}
				}
				if (/*Vars.Visuals.LootInfo.LootSort && */distance > 10.0f)// 10m
				{
					if (i == 0) ///first item
						previousPos = location;
					else if ((location - previousPos).Size() > 300.0f)///  More than 3 meters
					{
						previousPos = location;///update items location
						n = 0;
					}
					if (!sdk::WorldToScreen(previousPos, sLoot))///	Check inside the screen
						continue;
					sLoot.Y += (n++) * iconRadius;
				}
				else if (!sdk::WorldToScreen(location, sLoot))///	Check inside the screen
					continue;


				auto group = lootItem->GetGroup();
				if (SkipNext)
					goto NextStep;
				if (group == nullptr || !group->enable || group->color.Value.w <= 0.0f)
				{
					if (n > 0)
						n--;
					continue;
				}
			NextStep:
				if (Vars.espItem.SmartItem)
				{
					if (result == SmartItemID::NoWeapon)
					{
						if (lootItem->id >= 1100 && lootItem->id < 1400)
						{
							if (n > 0)
								n--;
							continue;
						}
					}
					if (result == SmartItemID::AllArmed)
					{
						if (lootItem->id >= 1000 && lootItem->id < 1100)
							if (lootItem->id != 1012 && lootItem->id != 1016 && lootItem->id != 1029 && lootItem->id != 1032 && lootItem->id != 1041 && lootItem->id != 1053 && lootItem->id != 1056 && lootItem->id != 1065)
							{
								if (n > 0)
									n--;
								continue;
							}
					}
					if (lootItem->id >= 1400 && lootItem->id < 1500 && !SmartSeleteEquipment(lootItem->id, 100.f, 100.f))
					{
						if (n > 0)
							n--;
						continue;
					}
				}

				auto FindResult = ItemContainer.find(lootItem->id);
				if (FindResult == ItemContainer.end())
				{
					if (n > 0)
						n--;
					continue;
				}
				if (!*FindResult->second->open)
				{
					if (n > 0)
						n--;
					continue;
				}
				if (bMMapRadarIsOpen)
					continue;
				if (n == 1)
				{
					FVector2D disPosition = sLoot - FVector2D(0, 20.0f);
					disPosition.X += iconRadius;
					Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espItem.TextSize, 0, disPosition, ImGui::FloatArrayGetColorU32(FindResult->second->color), 0xFF000000,
						FONT_LEFT, (u8"[%dM]"), (int)distance);
				}
				if (Vars.espItem.ItemPictures)
				{
					if (lootItem->icon.size() == 0)
						continue;
					ImRect bb(sLoot - FVector2D(iconRadius), sLoot + FVector2D(iconRadius));
					Drawing::DrawImage(ItemManager::GetTexture()[1], bb.Min, bb.Max, lootItem->icon[1].uv0, lootItem->icon[1].uv1, ImColor(1.0f, 1.0f, 1.0f, group->color.Value.w));
					sLoot.X += iconRadius;
				}
				if (Vars.espItem.ItemText)
				{
					std::string name = "";
					auto LanguageType = sdk::GetLanguageType();
					switch (LanguageType)
					{
					case 0:
						name = lootItem->name_cnsimple;
						break;
					case 1:
						name = lootItem->name_cntradit;
						break;
					case 2:
						name = lootItem->name_english;
						break;
					case 3:
						name = lootItem->name_japanese;
						break;
					case 4:
						name = lootItem->name_korean;
						break;
					default:
						name = lootItem->name_cnsimple;
						break;
					}
					if (lootItem->id < 1400 && lootItem->id > 1500)
					{
						size_t pos = name.find("(");
						if (pos != name.npos)
						{
							name = name.substr(0, pos - 1);
						}
						pos = name.find("（");
						if (pos != name.npos)
						{
							name = name.substr(0, pos - 2);
						}
					}
					ImVec2 fontSize;
					fontSize = Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espItem.TextSize, 0, sLoot + ImVec2(2.0f, 0.0f), ImGui::FloatArrayGetColorU32(FindResult->second->color), 0xFF000000/*ImColor(0.0f, 0.0f, 0.0f, group->color.Value.w)*/, FONT_LEFT, name.c_str());
					sLoot.X += fontSize.x;
				}

				if (n == 0)
				{
					Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espItem.TextSize, 0, sLoot + ImVec2(1.0f, 0.0f), ImGui::FloatArrayGetColorU32(FindResult->second->color), 0xFF000000,
						FONT_LEFT, (u8"(%dM)"), (int)distance);
				}
			}
			i++;
		}
	}

	void Visuals::LootESP(ADroppedItem& loot)
	{
		if (!loot || !loot->IsValid())
			return;
		if (!Vars.espItem.Item)
			return;
		if (!Vars.espItem.ItemPictures && !Vars.espItem.ItemText)
			return;
		UItem item = (UItem)loot->_Item;
		if (!item->IsValid())
			return;
		if (loot->_ItemName.empty() || loot->_ItemName == "")
			return;
		FVector2D sLoot;
		auto location = loot->_Location;
		auto distance = loot->_dist;
		if (distance > Vars.espItem.ItemDistance)
			return;

		if (!sdk::WorldToScreen(location, sLoot))///Check inside the screen
			return;

		auto ItemAllInfo = ItemManager::FindItemAllInfo(hash_(loot->_ItemName));

		std::string itemName = ItemAllInfo.ItemName;

		std::string itemCategoryName = ItemAllInfo.ItemCategory;

		if (itemName.empty() || itemCategoryName.empty() || itemName == "" || itemCategoryName == "")
		{
#ifdef ENABLE_CONSOLE
			if (OverlayEngine::WasKeyPressed(VK_UP))
			{
				CONSOLE_INFO("%s|%s|%s", item->GetItemID().GetName2(false).c_str(), item->GetItemName().ToString().c_str(), item->GetItemCategory().ToString().c_str());
			}
#endif
			return;
		}

		//Drawing::DrawTextOutline(0, 0, 0, sLoot, IM_COL32(255, 255, 0, 255), IM_COL32(0, 0, 0, 255), FONT_CENTER, "Name:%s\nItemName:%s\nCategory:%s", loot->_ItemName.c_str(), itemName.c_str(), itemCategoryName.c_str());

		//return;
		LootCategory itemCategory = GetLootCategory(itemCategoryName);
		hash_t itemHashName = hash_(itemName);
		float iconRadius = Vars.espItem.ImageSize * 0.5f;
		//Drawing::DrawTextOutline(0, 0, 0, sLoot, IM_COL32(255, 255, 0, 255), IM_COL32(0, 0, 0, 255), FONT_CENTER, "Name:%s\nDescription:%s\nCategory:%s\nDetailedName:%s\nitemCategory:%d", itemName.c_str(), itemDescription.c_str(), itemCategoryName.c_str(), itemDetailedName.c_str(), itemCategory);

		if (itemCategory != LootCategory::Unknown)
		{
			const auto& loots = ItemManager::GetGroupManager()->GetLoots()[itemCategory];
			if (loots.empty())
				return;

			auto lootInfo = loots.find(itemHashName);
			if (lootInfo == loots.end())
			{
				if (loot->_ItemName.find("Apple") != itemName.npos)
					itemHashName = HASH("ItemName_Weapon_Apple");
				else if (loot->_ItemName.find("Snowball") != itemName.npos)
					itemHashName = HASH("ItemName_Weapon_Snowball");
				else if (loot->_ItemName.find("Ghillie") != itemName.npos)
					itemHashName = HASH("M_ItemLegacy_1226");
				else if (loot->_ItemName.find("M_ItemLegacy_1226") != itemName.npos)
					itemHashName = HASH("M_ItemLegacy_1226");
				else
				{
					return;
				}

				lootInfo = loots.find(itemHashName);
				if (lootInfo == loots.end())
				{
					return;
				}
			}
			auto loott = lootInfo->second;
			if (loott == nullptr)
				return;
			bool SkipNext = false;
			auto result = SmartWeapon();
			if (Vars.espItem.SmartItem && result != SmartItemID::NoWeapon)
			{
				if (loott->id >= 1100 && loott->id < 1200)
				{
					if (SmartSeleteAttachment(loott->id, itemHashName) != SmartItemID::SuitAttach)
						return;
					else
						SkipNext = true;
				}
			}
			auto group = loott->GetGroup();
			if (group == nullptr || !group->enable || group->color.Value.w <= 0.0f)
				return;


			if (Vars.espItem.SmartItem)
			{
				if (result == SmartItemID::NoWeapon)
				{
					if (loott->id >= 1100 && loott->id < 1400)
						return;
				}
				if (result == SmartItemID::AllArmed)
				{
					if (loott->id >= 1000 && loott->id < 1100)
						if (loott->id != 1012 && loott->id != 1016 && loott->id != 1029 && loott->id != 1032 && loott->id != 1041 && loott->id != 1053 && loott->id != 1056 && loott->id != 1065)
							return;
				}

				//UEquipableItem EquipableItem = item->SafeCast<UEquipableItem>();
				if (loott->id >= 1400 && loott->id < 1500 && !SmartSeleteEquipment(loott->id, loot->_Durability, loot->_DurabilityMax))
					return;
			}

			auto FindResult = ItemContainer.find(loott->id);
			if (FindResult == ItemContainer.end())
				return;
			if (!*FindResult->second->open)
				return;

			if (bMMapRadarIsOpen)
				return;
			if (Vars.espItem.ItemPictures && loott->icon.size() != 0)
			{
				ImRect bb(sLoot - FVector2D(iconRadius), sLoot + FVector2D(iconRadius));
				Drawing::DrawImage(ItemManager::GetTexture()[1], bb.Min, bb.Max, loott->icon[1].uv0, loott->icon[1].uv1, ImColor(1.0f, 1.0f, 1.0f, 1.0f));
				sLoot.X += iconRadius;
			}
			if (Vars.espItem.ItemText)
			{
				std::string name = "";
				auto LanguageType = sdk::GetLanguageType();
				switch (LanguageType)
				{
				case 0:
					name = loott->name_cnsimple;
					break;
				case 1:
					name = loott->name_cntradit;
					break;
				case 2:
					name = loott->name_english;
					break;
				case 3:
					name = loott->name_japanese;
					break;
				case 4:
					name = loott->name_korean;
					break;
				default:
					name = loott->name_cnsimple;
					break;
				}
				if (loott->id < 1400 && loott->id > 1500)
				{
					size_t pos = name.find("(");
					if (pos != name.npos)
					{
						name = name.substr(0, pos - 1);
					}
					pos = name.find("（");
					if (pos != name.npos)
					{
						name = name.substr(0, pos - 2);
					}
				}
				ImVec2 fontSize;
				fontSize = Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espItem.TextSize, 0, sLoot + ImVec2(2.0f, 0.0f), ImGui::FloatArrayGetColorU32(FindResult->second->color), 0xFF000000, FONT_LEFT, ("%s"), name.c_str()/*, loot->id*/);
				sLoot.X += fontSize.x;
			}
			Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espItem.TextSize, 0, sLoot + ImVec2(1.0f, 0.0f), ImGui::FloatArrayGetColorU32(FindResult->second->color), 0xFF000000, FONT_LEFT, (u8"[%dM]"), (int)distance);
		}
		else
		{
			//CONSOLE_INFO(" Name:%s  itemCategory:%s  ItemDetailedName:%s	ItemDescription:%s", item.GetItemName().ToString().c_str(), item.GetItemCategory().ToString().c_str(), item.GetItemDetailedName().ToString().c_str(), item.GetItemDescription().ToString().c_str());
		}
	}

	void Visuals::AirdropESP(ACarePackageItem& airdrop)
	{
		if (!airdrop || !airdrop->IsValid())
			return;
		auto root = airdrop->_RootComponent;
		if (!root || airdrop->_Location.IsZero())
			return;
		auto origin = airdrop->_Location;
		auto delta = airdrop->_delta;
		int dist = (int)airdrop->_dist;

		auto Show = MapRadar2(origin, 0, 0, 3);
		if (!bMMapRadarIsOpen)
		{
			auto LocalHead = sdk::IsLocalPlayerAlive() ? sdk::GetLocalHeadPos() : sdk::GetLocalPos();
			auto EnemyHead = origin;
			FVector ThisDelta = EnemyHead - LocalHead;
			定义雷达图(ThisDelta, 0, FRotator(), 3);
		}
		if (!Vars.espAirDrop.AirDrop)
			return;
		FVector2D screen;
		if (!sdk::WorldToScreen(origin, screen) || Show)
			return;

		if (!bMMapRadarIsOpen)
			Drawing::DrawImage(m_HealthBar->m_CarePackage, ImVec2(screen.X - 20.f, screen.Y - 28.f), ImVec2(screen.X + 20.f, screen.Y + 28.f));

		auto fontsize = Drawing::DrawTextOutline(OverlayEngine::Font18px, 16, 0, ImVec2(screen.X + 20.f, screen.Y + 10.f), ImGui::FloatArrayGetColorU32(Vars.espAirDrop.AirDropColor), 0xFF000000, FONT_LEFT, LOCALIZE_Visual(u8"Airdrop[%dM]", u8"空投(%dM)"), dist);
		screen.Y += fontsize.y + 20.f;

		if (!Vars.espItem.ItemPictures && !Vars.espItem.ItemText)
			return;

		if (airdrop->_ItemsNum >= 40 || airdrop->_ItemsNum <= 0)
			return;

		if (airdrop->_ArrayData._itemFreshArray.empty())
			return;

		auto items = airdrop->_ArrayData._itemArray;

		for (int i = 0; i < (int)airdrop->_ItemsNum; i++)
		{
			auto item = UItem(items[i]);

			if (!item || !item.IsValid() || airdrop->_ArrayData._itemFreshArray[i] == 0
				|| airdrop->_ArrayData._ItemsIDNameArray[i].empty() || airdrop->_ArrayData._ItemsIDNameArray[i] == ""
				|| Utils::IsSafeReadPtr(airdrop->_ArrayData._ItemsIDNameArray[i].data(), airdrop->_ArrayData._ItemsIDNameArray[i].size() + 1))
				continue;
			Utils::hasNullTerminator(airdrop->_ArrayData._ItemsIDNameArray[i].data(), airdrop->_ArrayData._ItemsIDNameArray[i].size() + 1);
			char TestString[256];
			ZeroMemory(TestString, 256);
			memcpy(TestString, airdrop->_ArrayData._ItemsIDNameArray[i].data(), airdrop->_ArrayData._ItemsIDNameArray[i].size());
			std::string SafeString = std::string(TestString);
			auto ItemAllInfo = ItemManager::FindItemAllInfo(hash_(SafeString));

			std::string itemName = ItemAllInfo.ItemName;

			std::string itemCategoryName = ItemAllInfo.ItemCategory;

			if (itemName.empty() || itemCategoryName.empty())
				continue;
			hash_t itemHashName = hash_(itemName);
			LootCategory itemCategory = GetLootCategory(itemCategoryName);
			float iconRadius = (Vars.espItem.ImageSize - 15.f) * 0.5f;
			if (itemCategory != LootCategory::Unknown)
			{
				const auto& loots = ItemManager::GetGroupManager()->GetLoots()[itemCategory];
				if (loots.empty())
					continue;
				auto lootInfo = loots.find(itemHashName);
				if (lootInfo == loots.end())
				{
					if (airdrop->_ArrayData._ItemsIDNameArray[i].find("Ghillie") != itemName.npos)
						itemHashName = HASH("M_ItemLegacy_1226");
					else continue;

					lootInfo = loots.find(itemHashName);
					if (lootInfo == loots.end())
						continue;
				}

				auto loot = lootInfo->second;
				if (loot == nullptr)
					continue;

				auto group = loot->GetGroup();
				if (group == nullptr || !group->enable || group->color.Value.w <= 0.0f)
					continue;

				auto FindResult = ItemContainer.find(loot->id);
				if (FindResult == ItemContainer.end())
					continue;
				if (!*FindResult->second->open)
					continue;

				screen.Y += i > 0 ? iconRadius + 3.0f : 0.0f;
				auto tempScreen = screen;

				if (Vars.espItem.ItemPictures)
				{
					if (loot->icon.size() == 0)
						return;
					ImRect bb(tempScreen - FVector2D(iconRadius), tempScreen + FVector2D(iconRadius));
					if (!bMMapRadarIsOpen)
						Drawing::DrawImage(ItemManager::GetTexture()[1], bb.Min, bb.Max, loot->icon[1].uv0, loot->icon[1].uv1, ImColor(1.0f, 1.0f, 1.0f, group->color.Value.w));
					tempScreen.X += iconRadius;
				}
				if (Vars.espItem.ItemText)
				{
					std::string name = "";
					auto LanguageType = sdk::GetLanguageType();
					switch (LanguageType)
					{
					case 0:
						name = loot->name_cnsimple;
						break;
					case 1:
						name = loot->name_cntradit;
						break;
					case 2:
						name = loot->name_english;
						break;
					case 3:
						name = loot->name_japanese;
						break;
					case 4:
						name = loot->name_korean;
						break;
					default:
						name = loot->name_cnsimple;
						break;
					}
					if (loot->id < 1400 && loot->id > 1500)
					{
						size_t pos = name.find("(");
						if (pos != name.npos)
						{
							name = name.substr(0, pos - 1);
						}
						pos = name.find("（");
						if (pos != name.npos)
						{
							name = name.substr(0, pos - 2);
						}
					}
					Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espItem.TextSize, 0, tempScreen + ImVec2(2.0f, 0.0f), ImGui::FloatArrayGetColorU32(FindResult->second->color), 0xFF000000, FONT_LEFT, name.c_str());
				}
			}
			else
			{
				CONSOLE_INFO("itemCategory:%s", item->GetItemCategory().ToString().c_str());
			}
		}
	}

	void Visuals::DeathBoxESP(AFloorSnapItemPackage& deathbox)
	{
		if (!deathbox || !deathbox->IsValid())
			return;
		if (!Vars.espAirDrop.Loot)
			return;
		auto root = deathbox->_RootComponent;
		if (!root || deathbox->_Location.IsZero())
			return;

		auto origin = deathbox->_Location;
		auto delta = deathbox->_delta;
		int dist = (int)deathbox->_dist;

		FVector2D screen;
		if (!sdk::WorldToScreen(origin, screen) || dist > 200)
			return;

		auto fontsize = Drawing::DrawTextOutline(OverlayEngine::Font18px, 15, 0, screen, ImGui::FloatArrayGetColorU32(Vars.espAirDrop.LootColor), 0xFF000000, FONT_LEFT, LOCALIZE_Visual(u8"Loot [%dM]", u8"戰利品 [%dM]"), dist);
		screen.Y += fontsize.y;

		if (!Vars.espAirDrop.LootInside)
			return;

		if (!Vars.espItem.ItemPictures && !Vars.espItem.ItemText)
			return;

		if (deathbox->_ItemsNum >= 40 || deathbox->_ItemsNum <= 0)
			return;

		if (deathbox->_ArrayData._itemFreshArray.empty())
			return;

		auto items = deathbox->_ArrayData._itemArray;

		for (int i = 0; i < (int)deathbox->_ItemsNum; i++)
		{
			auto item = UItem(items[i]);

			if (!item || !item.IsValid() || deathbox->_ArrayData._itemFreshArray[i] == 0
				|| deathbox->_ArrayData._ItemsIDNameArray[i].empty() || deathbox->_ArrayData._ItemsIDNameArray[i] == ""
				|| Utils::IsSafeReadPtr(deathbox->_ArrayData._ItemsIDNameArray[i].data(), deathbox->_ArrayData._ItemsIDNameArray[i].size() + 1))
				continue;
			Utils::hasNullTerminator(deathbox->_ArrayData._ItemsIDNameArray[i].data(), deathbox->_ArrayData._ItemsIDNameArray[i].size() + 1);
			char TestString[256];
			ZeroMemory(TestString, 256);
			memcpy(TestString, deathbox->_ArrayData._ItemsIDNameArray[i].data(), deathbox->_ArrayData._ItemsIDNameArray[i].size());
			std::string SafeString = std::string(TestString);
			auto ItemAllInfo = ItemManager::FindItemAllInfo(hash_(SafeString));

			std::string itemName = ItemAllInfo.ItemName;

			std::string itemCategoryName = ItemAllInfo.ItemCategory;

			if (itemName.empty() || itemCategoryName.empty())
				continue;

			hash_t itemHashName = hash_(itemName);
			float iconRadius = (Vars.espItem.ImageSize - 15.f) * 0.5f;
			LootCategory itemCategory = GetLootCategory(itemCategoryName);
			if (itemCategory != LootCategory::Unknown)
			{
				const auto& loots = ItemManager::GetGroupManager()->GetLoots()[itemCategory];
				if (loots.empty())
					continue;


				auto lootInfo = loots.find(itemHashName);
				if (lootInfo == loots.end())
				{
					if (deathbox->_ArrayData._ItemsIDNameArray[i].find("Ghillie") != itemName.npos)
						itemHashName = HASH("M_ItemLegacy_1226");
					else
						continue;


					lootInfo = loots.find(itemHashName);
					if (lootInfo == loots.end())
					{
						CONSOLE_INFO("Not found item data:%s     %s", item->GetItemCategory().ToString().c_str());
						continue;
					}
				}

				auto loot = lootInfo->second;
				if (loot == nullptr)
					continue;

				auto group = loot->GetGroup();
				if (group == nullptr || !group->enable || group->color.Value.w <= 0.0f)
					continue;

				auto FindResult = ItemContainer.find(loot->id);
				if (FindResult == ItemContainer.end())
					continue;
				if (!*FindResult->second->open)
					continue;

				screen.Y += i > 0 ? iconRadius + 3.0f : 0.0f;
				auto tempScreen = screen;
				if (Vars.espItem.ItemPictures)
				{
					if (loot->icon.size() == 0)
						continue;
					ImRect bb(tempScreen - FVector2D(iconRadius), tempScreen + FVector2D(iconRadius));
					if (!bMMapRadarIsOpen)
						Drawing::DrawImage(ItemManager::GetTexture()[1], bb.Min, bb.Max, loot->icon[1].uv0, loot->icon[1].uv1, ImColor(1.0f, 1.0f, 1.0f, group->color.Value.w));
					tempScreen.X += iconRadius;
				}
				if (Vars.espItem.ItemText)
				{
					std::string name = "";
					auto LanguageType = sdk::GetLanguageType();
					switch (LanguageType)
					{
					case 0:
						name = loot->name_cnsimple;
						break;
					case 1:
						name = loot->name_cntradit;
						break;
					case 2:
						name = loot->name_english;
						break;
					case 3:
						name = loot->name_japanese;
						break;
					case 4:
						name = loot->name_korean;
						break;
					default:
						name = loot->name_cnsimple;
						break;
					}
					if (loot->id < 1400 && loot->id > 1500)
					{
						size_t pos = name.find("(");
						if (pos != name.npos)
						{
							name = name.substr(0, pos - 1);
						}
						pos = name.find("（");
						if (pos != name.npos)
						{
							name = name.substr(0, pos - 2);
						}
					}
					Drawing::DrawTextOutline(OverlayEngine::Font20px, Vars.espItem.TextSize, 0, tempScreen + ImVec2(2.0f, 0.0f), ImGui::FloatArrayGetColorU32(FindResult->second->color), 0xFF000000, FONT_LEFT, name.c_str());
				}
			}
		}
	}

	void Visuals::VehicleESP(ATslWheeledVehicle& vehicle)
	{
		if (!vehicle || !vehicle->IsValid())
			return;
		FVector2D _2NAxis;
		if (!vehicle->IsValid() || !vehicle->_VehicleCommonComponent)
			return;
		auto VehicleCommonComponent = (UTslVehicleCommonComponent)vehicle->_VehicleCommonComponent;
		auto root = vehicle->_RootComponent;
		if (!root)
			return;
		auto origin = vehicle->_Location;
		auto delta = vehicle->_delta;
		float dist = vehicle->_dist;
		bool Show = MapRadar2(origin, 0.f, 0, 2);
		if (!bMMapRadarIsOpen)
		{
			auto LocalHead = sdk::IsLocalPlayerAlive() ? sdk::GetLocalHeadPos() : sdk::GetLocalPos();
			auto EnemyHead = origin;
			FVector ThisDelta = EnemyHead - LocalHead;
			定义雷达图(ThisDelta, 0, FRotator(), 2);
		}

		if (dist < Vars.espVehicle.VehicleDistance && Vars.espVehicle.Vehicle && sdk::WorldToScreen(origin, _2NAxis) && !Show)
		{
			if (!Utils::StringIsValid(vehicle->_VehicleName))
				return;
			std::string HashNameStr = vehicle->_VehicleName;
			const auto vehicleData = VehicleManager::GetVehicle(hash_(HashNameStr));
			std::string displayName = LOCALIZE_Visual("Vehicle", u8"载具");
			if (vehicleData)
			{
				std::string DisName = "";
				auto LanguageType = sdk::GetLanguageType();
				switch (LanguageType)
				{
				case 0:
					DisName = vehicleData->name_cnsimple;
					break;
				case 1:
					DisName = vehicleData->name_cntradit;
					break;
				case 2:
					DisName = vehicleData->name_english;
					break;
				case 3:
					DisName = vehicleData->name_japanese;
					break;
				case 4:
					DisName = vehicleData->name_korean;
					break;
				default:
					DisName = vehicleData->name_cnsimple;
					break;
				}
				displayName = DisName;
			}
			else
			{
				//CONSOLE_INFO("No vehicle data found:%s", vehicle->GetName().c_str());
			}

			char formatVehicle[256] = { 0 };
			ZeroMemory(formatVehicle, 256);
			if (Vars.espVehicle.VehicleHealth)
				if (vehicle->_VehicleCommonComponentHealth == 0.f && vehicle->_VehicleCommonComponentHealthMax == 0.f)
					sprintf_s(formatVehicle, LOCALIZE_Visual("%s:0\n", u8"%s:0\n"), displayName.c_str());
				else
					sprintf_s(formatVehicle, LOCALIZE_Visual("%s:%.0f\n", u8"%s:%.0f\n"), displayName.c_str(), vehicle->_VehicleCommonComponentHealth / vehicle->_VehicleCommonComponentHealthMax * 100.f);
			else
				sprintf_s(formatVehicle, LOCALIZE_Visual("%s\n", u8"%s\n"), displayName.c_str());

			std::string InfoPrint = std::string(formatVehicle);
			sprintf_s(formatVehicle, LOCALIZE_Visual("[%dM]", u8"[%dM]"), (int)dist);
			InfoPrint += std::string(formatVehicle);
			Drawing::DrawTextOutline(OverlayEngine::Font16px, 16, 0, _2NAxis, ImGui::FloatArrayGetColorU32(Vars.espVehicle.VehicleColor), 0xFF000000, FONT_LEFT, InfoPrint.c_str());
		}
	}

	void Visuals::BoatESP(ATslFloatingVehicle& boat)
	{
		if (!boat || !boat->IsValid())
			return;
		FVector2D _2NAxis;
		if (!boat->IsValid() || !boat->_VehicleCommonComponent)
			return;
		auto VehicleCommonComponent = (UTslVehicleCommonComponent)boat->_VehicleCommonComponent;
		auto root = boat->_RootComponent;
		if (!root)
			return;
		auto origin = boat->_Location;
		auto delta = boat->_delta;
		float dist = boat->_dist;

		bool Show = MapRadar2(origin, 0.f, 0, 4);
		if (!bMMapRadarIsOpen)
		{
			auto LocalHead = sdk::IsLocalPlayerAlive() ? sdk::GetLocalHeadPos() : sdk::GetLocalPos();
			auto EnemyHead = origin;
			FVector ThisDelta = EnemyHead - LocalHead;
			定义雷达图(ThisDelta, 0, FRotator(), 4);
		}

		if (dist < Vars.espVehicle.VehicleDistance && Vars.espVehicle.Vehicle && sdk::WorldToScreen(origin, _2NAxis) && !Show)
		{
			if (!Utils::StringIsValid(boat->_VehicleName))
				return;
			std::string HashNameStr = boat->_VehicleName;
			const auto vehicleData = VehicleManager::GetVehicle(hash_(HashNameStr));
			std::string displayName = LOCALIZE_Visual("Boat", u8"船只");
			if (vehicleData)
			{
				std::string DisName = "";
				auto LanguageType = sdk::GetLanguageType();
				switch (LanguageType)
				{
				case 0:
					DisName = vehicleData->name_cnsimple;
					break;
				case 1:
					DisName = vehicleData->name_cntradit;
					break;
				case 2:
					DisName = vehicleData->name_english;
					break;
				case 3:
					DisName = vehicleData->name_japanese;
					break;
				case 4:
					DisName = vehicleData->name_korean;
					break;
				default:
					DisName = vehicleData->name_cnsimple;
					break;
				}
				displayName = DisName;
			}
			else
			{
				//CONSOLE_INFO("No vehicle data found:%s", vehicle->GetName().c_str());
			}
			char formatVehicle[256] = { 0 };
			ZeroMemory(formatVehicle, 256);
			if (Vars.espVehicle.VehicleHealth)
				if (boat->_VehicleCommonComponentHealth == 0.f && boat->_VehicleCommonComponentHealthMax == 0.f)
					sprintf_s(formatVehicle, LOCALIZE_Visual("%s:0\n", u8"%s:0\n"), displayName.c_str());
				else
					sprintf_s(formatVehicle, LOCALIZE_Visual("%s:%.0f\n", u8"%s:%.0f\n"), displayName.c_str(), boat->_VehicleCommonComponentHealth / boat->_VehicleCommonComponentHealthMax * 100.f);
			else
				sprintf_s(formatVehicle, LOCALIZE_Visual("%s\n", u8"%s\n"), displayName.c_str());

			std::string InfoPrint = std::string(formatVehicle);
			sprintf_s(formatVehicle, LOCALIZE_Visual("[%dM]", u8"[%dM]"), (int)dist);
			InfoPrint += std::string(formatVehicle);
			Drawing::DrawTextOutline(OverlayEngine::Font16px, 16, 0, _2NAxis, ImGui::FloatArrayGetColorU32(Vars.espVehicle.VehicleColor), 0xFF000000, FONT_LEFT, InfoPrint.c_str());
		}
	}

	void Visuals::ActorESP(AActor& Actor)
	{
		if (!Actor || !Actor->IsValid())
			return;
		FVector2D _2NAxis;
		if (!Actor->IsValid() || !Actor->_RootComponent)
			return;
		if (Actor->_Location.IsZero())
			return;
		auto origin = Actor->_Location;
		auto delta = Actor->_delta;
		float dist = Actor->_dist;

		if (sdk::WorldToScreen(origin, _2NAxis))
		{
			//if (Actor->_VehicleName == "SLBZombie_Burning_C")
			//{
			//	CONSOLE_INFO("%s",Actor->GetClass()->GetSuperClassName().c_str());
			//}
			Drawing::DrawTextOutline(OverlayEngine::Font14px, 14, 0, _2NAxis, 0xFFFFFFFF, 0xFF000000, FONT_LEFT, Actor->_VehicleName.c_str());
		}
	}

	HealthBar::HealthBar()
		:m_Tex(nullptr),
		m_CarePackage(nullptr),
		m_MapCarePackage(nullptr),
		m_MapVehicle(nullptr),
		m_MapBoat(nullptr),
		m_Partners(nullptr),
		m_Bronze1(nullptr),
		m_Bronze2(nullptr),
		m_Bronze3(nullptr),
		m_Bronze4(nullptr),
		m_Bronze5(nullptr),
		m_Silver1(nullptr),
		m_Silver2(nullptr),
		m_Silver3(nullptr),
		m_Silver4(nullptr),
		m_Silver5(nullptr),
		m_Gold1(nullptr),
		m_Gold2(nullptr),
		m_Gold3(nullptr),
		m_Gold4(nullptr),
		m_Gold5(nullptr),
		m_Platinum1(nullptr),
		m_Platinum2(nullptr),
		m_Platinum3(nullptr),
		m_Platinum4(nullptr),
		m_Platinum5(nullptr),
		m_Diamond1(nullptr),
		m_Diamond2(nullptr),
		m_Diamond3(nullptr),
		m_Diamond4(nullptr),
		m_Diamond5(nullptr),
		m_Master1(nullptr),
		m_Unranked(nullptr),
		m_Loaded(false)
	{
		m_sprites = new Sprites();
		m_uvs = new SpritesUV();
	}

	HealthBar::~HealthBar()
	{
		if (m_sprites) delete m_sprites;

		if (m_uvs) delete m_uvs;


		if (m_Tex)
		{
			// TODO: safe release texture
		}

	}

	bool HealthBar::Load()
	{
		auto ApiKeysData = GetApiKeyFromServer();
		if (ApiKeysData != "")
		{
			auto KeysArray = Utils::split(ApiKeysData, "|");
			for (int i = 0; i < KeysArray.size(); i++)
			{
				APIKeys.push_back(KeysArray[i]);
			}
		}
		g_physic = new PhysicsManager(g_ServerResult.Version >= 2);
		WeapSkinMap = std::make_shared<std::unordered_map<hash_t, std::vector<FSkinMapData>>>();
		const void* hudSprite = nullptr;
		size_t spriteSize = Package::GetPackage()->FindFile((const hash_t)("hud.png"_hash), (void**)&hudSprite);

		const char* hudConfig = nullptr;
		size_t configLength = Package::GetPackage()->FindFile((const hash_t)("hud.paper2dsprites"_hash), (void**)&hudConfig);

		const char* huduvConfig = nullptr;
		size_t uvConfigLength = Package::GetPackage()->FindFile((const hash_t)("huduv.json"_hash), (void**)&huduvConfig);

		const char* hudCarePackage = nullptr;
		size_t CarePackageLength = Package::GetPackage()->FindFile(HASH("CarePackage_Flying.png"), (void**)&hudCarePackage);

		const char* hudMapCarePackage = nullptr;
		size_t MapCarePackageLength = Package::GetPackage()->FindFile(HASH("CarePackage_Normal.png"), (void**)&hudMapCarePackage);

		const char* hudVehicle = nullptr;
		size_t VehicleLength = Package::GetPackage()->FindFile(HASH("MapVehicle.png"), (void**)&hudVehicle);

		const char* hudBoat = nullptr;
		size_t BoatLength = Package::GetPackage()->FindFile(HASH("MapBoat.png"), (void**)&hudBoat);

		const char* hudPartners = nullptr;
		size_t PartnersLength = Package::GetPackage()->FindFile(HASH("PARTNERS.png"), (void**)&hudPartners);

		if (hudSprite && hudConfig && huduvConfig
			&& hudCarePackage && hudMapCarePackage && hudVehicle && hudBoat
			&& hudPartners)
		{
			m_Loaded = true;
			if (!(m_Tex = OverlayEngine::CreateTexture((const char*)hudSprite, spriteSize)))
				m_Loaded = false;

			if (!(m_CarePackage = OverlayEngine::CreateTexture((const char*)hudCarePackage, CarePackageLength)))
				m_Loaded = false;

			if (!(m_Disconnected = OverlayEngine::CreateTexture((const char*)DisconnectImage, sizeof(DisconnectImage))))
				m_Loaded = false;

			if (!(m_MapCarePackage = OverlayEngine::CreateTexture((const char*)hudMapCarePackage, MapCarePackageLength)))
				m_Loaded = false;

			if (!(m_MapVehicle = OverlayEngine::CreateTexture((const char*)hudVehicle, VehicleLength)))
				m_Loaded = false;

			if (!(m_MapBoat = OverlayEngine::CreateTexture((const char*)hudBoat, BoatLength)))
				m_Loaded = false;

			if (!(m_Partners = OverlayEngine::CreateTexture((const char*)hudPartners, PartnersLength)))
				m_Loaded = false;

			JsonReader frameReader(hudConfig, configLength);
			if (!m_sprites->Deserialize(frameReader))
				m_Loaded = false;

			JsonReader configReader(huduvConfig, uvConfigLength);
			if (!m_uvs->Deserialize(configReader))
				m_Loaded = false;

			if (!m_Loaded)
			{
				MessageBoxA(NULL, ("laoded res failed! - mirodo"), "ERROR", MB_OK | MB_ICONHAND);
				return false;
			}
		}
		else
		{
			MessageBoxA(NULL, ("res NotFind! - Muti"), "ERROR", MB_OK | MB_ICONHAND);
			return false;
		}

		if (hudSprite) { memset((void*)hudSprite, 0, spriteSize);	free((void*)hudSprite); }
		if (hudConfig) { memset((void*)hudConfig, 0, configLength);	free((void*)hudConfig); }
		if (huduvConfig) { memset((void*)huduvConfig, 0, uvConfigLength);	free((void*)huduvConfig); }
		if (hudCarePackage) { memset((void*)hudCarePackage, 0, CarePackageLength);	free((void*)hudCarePackage); }
		if (hudMapCarePackage) { memset((void*)hudMapCarePackage, 0, MapCarePackageLength);	free((void*)hudMapCarePackage); }
		if (hudVehicle) { memset((void*)hudVehicle, 0, VehicleLength);	free((void*)hudVehicle); }
		if (hudBoat) { memset((void*)hudBoat, 0, BoatLength);	free((void*)hudBoat); }
		if (hudPartners) { memset((void*)hudPartners, 0, PartnersLength);	free((void*)hudPartners); }
		const void* Bronze_1, * Bronze_2, * Bronze_3, * Bronze_4, * Bronze_5, * Silver_1, * Silver_2, * Silver_3, * Silver_4, * Silver_5,
			* Gold_1, * Gold_2, * Gold_3, * Gold_4, * Gold_5, * Platinum_1, * Platinum_2, * Platinum_3, * Platinum_4, * Platinum_5,
			* Diamond_1, * Diamond_2, * Diamond_3, * Diamond_4, * Diamond_5, * Master_1, * UnRanked_1;

		size_t Bronze1 = Package::GetPackage()->FindFile(HASH("Bronze-1.png"), (void**)&Bronze_1);
		size_t Bronze2 = Package::GetPackage()->FindFile(HASH("Bronze-2.png"), (void**)&Bronze_2);
		size_t Bronze3 = Package::GetPackage()->FindFile(HASH("Bronze-3.png"), (void**)&Bronze_3);
		size_t Bronze4 = Package::GetPackage()->FindFile(HASH("Bronze-4.png"), (void**)&Bronze_4);
		size_t Bronze5 = Package::GetPackage()->FindFile(HASH("Bronze-5.png"), (void**)&Bronze_5);

		size_t Silver1 = Package::GetPackage()->FindFile(HASH("Silver-1.png"), (void**)&Silver_1);
		size_t Silver2 = Package::GetPackage()->FindFile(HASH("Silver-2.png"), (void**)&Silver_2);
		size_t Silver3 = Package::GetPackage()->FindFile(HASH("Silver-3.png"), (void**)&Silver_3);
		size_t Silver4 = Package::GetPackage()->FindFile(HASH("Silver-4.png"), (void**)&Silver_4);
		size_t Silver5 = Package::GetPackage()->FindFile(HASH("Silver-5.png"), (void**)&Silver_5);

		size_t Gold1 = Package::GetPackage()->FindFile(HASH("gold-1.png"), (void**)&Gold_1);
		size_t Gold2 = Package::GetPackage()->FindFile(HASH("gold-2.png"), (void**)&Gold_2);
		size_t Gold3 = Package::GetPackage()->FindFile(HASH("gold-3.png"), (void**)&Gold_3);
		size_t Gold4 = Package::GetPackage()->FindFile(HASH("gold-4.png"), (void**)&Gold_4);
		size_t Gold5 = Package::GetPackage()->FindFile(HASH("gold-5.png"), (void**)&Gold_5);

		size_t Platinum1 = Package::GetPackage()->FindFile(HASH("Platinum-1.png"), (void**)&Platinum_1);
		size_t Platinum2 = Package::GetPackage()->FindFile(HASH("Platinum-2.png"), (void**)&Platinum_2);
		size_t Platinum3 = Package::GetPackage()->FindFile(HASH("Platinum-3.png"), (void**)&Platinum_3);
		size_t Platinum4 = Package::GetPackage()->FindFile(HASH("Platinum-4.png"), (void**)&Platinum_4);
		size_t Platinum5 = Package::GetPackage()->FindFile(HASH("Platinum-5.png"), (void**)&Platinum_5);

		size_t Diamond1 = Package::GetPackage()->FindFile(HASH("Diamond-1.png"), (void**)&Diamond_1);
		size_t Diamond2 = Package::GetPackage()->FindFile(HASH("Diamond-2.png"), (void**)&Diamond_2);
		size_t Diamond3 = Package::GetPackage()->FindFile(HASH("Diamond-3.png"), (void**)&Diamond_3);
		size_t Diamond4 = Package::GetPackage()->FindFile(HASH("Diamond-4.png"), (void**)&Diamond_4);
		size_t Diamond5 = Package::GetPackage()->FindFile(HASH("Diamond-5.png"), (void**)&Diamond_5);

		size_t Master1 = Package::GetPackage()->FindFile(HASH("Master-1.png"), (void**)&Master_1);
		size_t UnRanked = Package::GetPackage()->FindFile(HASH("Unranked.png"), (void**)&UnRanked_1);

		if (Bronze_1 && Bronze_2 && Bronze_3 && Bronze_4 && Bronze_5 && Silver_1 && Silver_2 && Silver_3 && Silver_4 && Silver_5
			&& Gold_1 && Gold_2 && Gold_3 && Gold_4 && Gold_5 && Platinum_1 && Platinum_2 && Platinum_3 && Platinum_4 && Platinum_5
			&& Diamond_1 && Diamond_2 && Diamond_3 && Diamond_4 && Diamond_5 && Master_1 && UnRanked_1)
		{
			m_Loaded = true;
			if (!(m_Bronze1 = OverlayEngine::CreateTexture((const char*)Bronze_1, Bronze1)))
				m_Loaded = false;
			if (!(m_Bronze2 = OverlayEngine::CreateTexture((const char*)Bronze_2, Bronze2)))
				m_Loaded = false;
			if (!(m_Bronze3 = OverlayEngine::CreateTexture((const char*)Bronze_3, Bronze3)))
				m_Loaded = false;
			if (!(m_Bronze4 = OverlayEngine::CreateTexture((const char*)Bronze_4, Bronze4)))
				m_Loaded = false;
			if (!(m_Bronze5 = OverlayEngine::CreateTexture((const char*)Bronze_5, Bronze5)))
				m_Loaded = false;
			if (!(m_Silver1 = OverlayEngine::CreateTexture((const char*)Silver_1, Silver1)))
				m_Loaded = false;
			if (!(m_Silver2 = OverlayEngine::CreateTexture((const char*)Silver_2, Silver2)))
				m_Loaded = false;
			if (!(m_Silver3 = OverlayEngine::CreateTexture((const char*)Silver_3, Silver3)))
				m_Loaded = false;
			if (!(m_Silver4 = OverlayEngine::CreateTexture((const char*)Silver_4, Silver4)))
				m_Loaded = false;
			if (!(m_Silver5 = OverlayEngine::CreateTexture((const char*)Silver_5, Silver5)))
				m_Loaded = false;
			if (!(m_Gold1 = OverlayEngine::CreateTexture((const char*)Gold_1, Gold1)))
				m_Loaded = false;
			if (!(m_Gold2 = OverlayEngine::CreateTexture((const char*)Gold_2, Gold2)))
				m_Loaded = false;
			if (!(m_Gold3 = OverlayEngine::CreateTexture((const char*)Gold_3, Gold3)))
				m_Loaded = false;
			if (!(m_Gold4 = OverlayEngine::CreateTexture((const char*)Gold_4, Gold4)))
				m_Loaded = false;
			if (!(m_Gold5 = OverlayEngine::CreateTexture((const char*)Gold_5, Gold5)))
				m_Loaded = false;
			if (!(m_Platinum1 = OverlayEngine::CreateTexture((const char*)Platinum_1, Platinum1)))
				m_Loaded = false;
			if (!(m_Platinum2 = OverlayEngine::CreateTexture((const char*)Platinum_2, Platinum2)))
				m_Loaded = false;
			if (!(m_Platinum3 = OverlayEngine::CreateTexture((const char*)Platinum_3, Platinum3)))
				m_Loaded = false;
			if (!(m_Platinum4 = OverlayEngine::CreateTexture((const char*)Platinum_4, Platinum4)))
				m_Loaded = false;
			if (!(m_Platinum5 = OverlayEngine::CreateTexture((const char*)Platinum_5, Platinum5)))
				m_Loaded = false;
			if (!(m_Diamond1 = OverlayEngine::CreateTexture((const char*)Diamond_1, Diamond1)))
				m_Loaded = false;
			if (!(m_Diamond2 = OverlayEngine::CreateTexture((const char*)Diamond_2, Diamond2)))
				m_Loaded = false;
			if (!(m_Diamond3 = OverlayEngine::CreateTexture((const char*)Diamond_3, Diamond3)))
				m_Loaded = false;
			if (!(m_Diamond4 = OverlayEngine::CreateTexture((const char*)Diamond_4, Diamond4)))
				m_Loaded = false;
			if (!(m_Diamond5 = OverlayEngine::CreateTexture((const char*)Diamond_5, Diamond5)))
				m_Loaded = false;
			if (!(m_Master1 = OverlayEngine::CreateTexture((const char*)Master_1, Master1)))
				m_Loaded = false;
			if (!(m_Unranked = OverlayEngine::CreateTexture((const char*)UnRanked_1, UnRanked)))
				m_Loaded = false;
		}
		else
		{
			MessageBoxA(NULL, ("res NotFind!"), "ERROR", MB_OK | MB_ICONHAND);
			return false;
		}
		Visuals::RankingIcon.emplace(hash_(("Bronze1")), std::make_shared<pRankIcon>(pRankIcon(m_Bronze1, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Bronze2")), std::make_shared<pRankIcon>(pRankIcon(m_Bronze2, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Bronze3")), std::make_shared<pRankIcon>(pRankIcon(m_Bronze3, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Bronze4")), std::make_shared<pRankIcon>(pRankIcon(m_Bronze4, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Bronze5")), std::make_shared<pRankIcon>(pRankIcon(m_Bronze5, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Silver1")), std::make_shared<pRankIcon>(pRankIcon(m_Silver1, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Silver2")), std::make_shared<pRankIcon>(pRankIcon(m_Silver2, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Silver3")), std::make_shared<pRankIcon>(pRankIcon(m_Silver3, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Silver4")), std::make_shared<pRankIcon>(pRankIcon(m_Silver4, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Silver5")), std::make_shared<pRankIcon>(pRankIcon(m_Silver5, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Gold1")), std::make_shared<pRankIcon>(pRankIcon(m_Gold1, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Gold2")), std::make_shared<pRankIcon>(pRankIcon(m_Gold2, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Gold3")), std::make_shared<pRankIcon>(pRankIcon(m_Gold3, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Gold4")), std::make_shared<pRankIcon>(pRankIcon(m_Gold4, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Gold5")), std::make_shared<pRankIcon>(pRankIcon(m_Gold5, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Platinum1")), std::make_shared<pRankIcon>(pRankIcon(m_Platinum1, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Platinum2")), std::make_shared<pRankIcon>(pRankIcon(m_Platinum2, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Platinum3")), std::make_shared<pRankIcon>(pRankIcon(m_Platinum3, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Platinum4")), std::make_shared<pRankIcon>(pRankIcon(m_Platinum4, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Platinum5")), std::make_shared<pRankIcon>(pRankIcon(m_Platinum5, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Diamond1")), std::make_shared<pRankIcon>(pRankIcon(m_Diamond1, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Diamond2")), std::make_shared<pRankIcon>(pRankIcon(m_Diamond2, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Diamond3")), std::make_shared<pRankIcon>(pRankIcon(m_Diamond3, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Diamond4")), std::make_shared<pRankIcon>(pRankIcon(m_Diamond4, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Diamond5")), std::make_shared<pRankIcon>(pRankIcon(m_Diamond5, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Master1")), std::make_shared<pRankIcon>(pRankIcon(m_Master1, ImVec2(25, 25))));
		Visuals::RankingIcon.emplace(hash_(("Unranked")), std::make_shared<pRankIcon>(pRankIcon(m_Unranked, ImVec2(25, 25))));

		if (Bronze_1) { memset((void*)Bronze_1, 0, Bronze1);	free((void*)Bronze_1); }
		if (Bronze_2) { memset((void*)Bronze_2, 0, Bronze2);	free((void*)Bronze_2); }
		if (Bronze_3) { memset((void*)Bronze_3, 0, Bronze3);	free((void*)Bronze_3); }
		if (Bronze_4) { memset((void*)Bronze_4, 0, Bronze4);	free((void*)Bronze_4); }
		if (Bronze_5) { memset((void*)Bronze_5, 0, Bronze5);	free((void*)Bronze_5); }

		if (Silver_1) { memset((void*)Silver_1, 0, Silver1);	free((void*)Silver_1); }
		if (Silver_2) { memset((void*)Silver_2, 0, Silver2);	free((void*)Silver_2); }
		if (Silver_3) { memset((void*)Silver_3, 0, Silver3);	free((void*)Silver_3); }
		if (Silver_4) { memset((void*)Silver_4, 0, Silver4);	free((void*)Silver_4); }
		if (Silver_5) { memset((void*)Silver_5, 0, Silver5);	free((void*)Silver_5); }

		if (Gold_1) { memset((void*)Gold_1, 0, Gold1);	free((void*)Gold_1); }
		if (Gold_2) { memset((void*)Gold_2, 0, Gold2);	free((void*)Gold_2); }
		if (Gold_3) { memset((void*)Gold_3, 0, Gold3);	free((void*)Gold_3); }
		if (Gold_4) { memset((void*)Gold_4, 0, Gold4);	free((void*)Gold_4); }
		if (Gold_5) { memset((void*)Gold_5, 0, Gold5);	free((void*)Gold_5); }

		if (Platinum_1) { memset((void*)Platinum_1, 0, Platinum1);	free((void*)Platinum_1); }
		if (Platinum_2) { memset((void*)Platinum_2, 0, Platinum2);	free((void*)Platinum_2); }
		if (Platinum_3) { memset((void*)Platinum_3, 0, Platinum3);	free((void*)Platinum_3); }
		if (Platinum_4) { memset((void*)Platinum_4, 0, Platinum4);	free((void*)Platinum_4); }
		if (Platinum_5) { memset((void*)Platinum_5, 0, Platinum5);	free((void*)Platinum_5); }

		if (Diamond_1) { memset((void*)Diamond_1, 0, Diamond1);	free((void*)Diamond_1); }
		if (Diamond_2) { memset((void*)Diamond_2, 0, Diamond2);	free((void*)Diamond_2); }
		if (Diamond_3) { memset((void*)Diamond_3, 0, Diamond3);	free((void*)Diamond_3); }
		if (Diamond_4) { memset((void*)Diamond_4, 0, Diamond4);	free((void*)Diamond_4); }
		if (Diamond_5) { memset((void*)Diamond_5, 0, Diamond5);	free((void*)Diamond_5); }

		if (Master_1) { memset((void*)Master_1, 0, Master1);	free((void*)Master_1); }
		if (UnRanked_1) { memset((void*)UnRanked_1, 0, UnRanked);	free((void*)UnRanked_1); }

		ItemContainer.emplace(1001, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.AKM, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1003, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.AUG, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1004, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.AWM, (float*)&Vars.espItem.SRColor)));
		ItemContainer.emplace(1005, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.S686, (float*)&Vars.espItem.ShotGunColor)));
		ItemContainer.emplace(1006, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.M762, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1007, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.PP19Bizon, (float*)&Vars.espItem.SMGColor)));
		ItemContainer.emplace(1008, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Crossbow, (float*)&Vars.espItem.ETCColor)));
		ItemContainer.emplace(1009, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Crowbar, (float*)&Vars.espItem.MelleColor)));
		ItemContainer.emplace(1010, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.DP28, (float*)&Vars.espItem.LMGColor)));
		ItemContainer.emplace(1011, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FlareGun, (float*)&Vars.espItem.ETCColor)));
		ItemContainer.emplace(1012, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FlashBang, (float*)&Vars.espItem.ThrowableColor)));
		ItemContainer.emplace(1013, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.SLR, (float*)&Vars.espItem.DMRColor)));
		ItemContainer.emplace(1014, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.P18C, (float*)&Vars.espItem.HandGunColor)));
		ItemContainer.emplace(1015, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.G36C, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1016, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FragGrenade, (float*)&Vars.espItem.ThrowableColor)));
		ItemContainer.emplace(1017, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Groza, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1018, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.M416, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1019, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Kar98k, (float*)&Vars.espItem.SRColor)));
		ItemContainer.emplace(1020, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.M16A4, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1021, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.P1911, (float*)&Vars.espItem.HandGunColor)));
		ItemContainer.emplace(1022, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.M24, (float*)&Vars.espItem.SRColor)));
		ItemContainer.emplace(1023, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.M249, (float*)&Vars.espItem.LMGColor)));
		ItemContainer.emplace(1024, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.P92, (float*)&Vars.espItem.HandGunColor)));
		ItemContainer.emplace(1025, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Machete, (float*)&Vars.espItem.MelleColor)));
		ItemContainer.emplace(1026, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Mini14, (float*)&Vars.espItem.DMRColor)));
		ItemContainer.emplace(1072, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Dragunov, (float*)&Vars.espItem.DMRColor)));
		ItemContainer.emplace(1027, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Mk14, (float*)&Vars.espItem.DMRColor)));
		ItemContainer.emplace(1028, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.MK47, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1029, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Molotov, (float*)&Vars.espItem.ThrowableColor)));
		ItemContainer.emplace(1030, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.MP5K, (float*)&Vars.espItem.SMGColor)));
		ItemContainer.emplace(1031, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.R1895, (float*)&Vars.espItem.HandGunColor)));
		ItemContainer.emplace(1032, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Pan, (float*)&Vars.espItem.MelleColor)));
		ItemContainer.emplace(1033, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.QBU, (float*)&Vars.espItem.DMRColor)));
		ItemContainer.emplace(1034, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.QBZ, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1035, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.R45, (float*)&Vars.espItem.HandGunColor)));
		ItemContainer.emplace(1036, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.S12K, (float*)&Vars.espItem.ShotGunColor)));
		ItemContainer.emplace(1037, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.SawedOff, (float*)&Vars.espItem.HandGunColor)));
		ItemContainer.emplace(1038, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.SCARL, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1039, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Sickle, (float*)&Vars.espItem.MelleColor)));
		ItemContainer.emplace(1040, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.SKS, (float*)&Vars.espItem.DMRColor)));
		ItemContainer.emplace(1041, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.SmokeBoom, (float*)&Vars.espItem.ThrowableColor)));
		ItemContainer.emplace(1043, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.TommyGun, (float*)&Vars.espItem.SMGColor)));
		ItemContainer.emplace(1073, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.JS9, (float*)&Vars.espItem.SMGColor)));
		ItemContainer.emplace(1044, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.UMP45, (float*)&Vars.espItem.SMGColor)));
		ItemContainer.emplace(1045, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.MicroUZI, (float*)&Vars.espItem.SMGColor)));
		ItemContainer.emplace(1046, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Vector, (float*)&Vars.espItem.SMGColor)));
		ItemContainer.emplace(1047, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.VSS, (float*)&Vars.espItem.DMRColor)));
		ItemContainer.emplace(1048, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Skoripion, (float*)&Vars.espItem.HandGunColor)));
		ItemContainer.emplace(1049, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Win94, (float*)&Vars.espItem.SRColor)));
		ItemContainer.emplace(1050, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.S1897, (float*)&Vars.espItem.ShotGunColor)));
		ItemContainer.emplace(1051, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Deagle, (float*)&Vars.espItem.HandGunColor)));
		ItemContainer.emplace(1052, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.DBS, (float*)&Vars.espItem.ShotGunColor)));
		ItemContainer.emplace(1055, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Panzerfaust, (float*)&Vars.espItem.TransmitterColor)));
		ItemContainer.emplace(1056, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.C4, (float*)&Vars.espItem.ThrowableColor)));
		ItemContainer.emplace(1057, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.MG3, (float*)&Vars.espItem.LMGColor)));
		ItemContainer.emplace(1058, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.MosinNagant, (float*)&Vars.espItem.SRColor)));
		ItemContainer.emplace(1059, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.LynxAMR, (float*)&Vars.espItem.SRColor)));
		ItemContainer.emplace(1060, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.K2, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1061, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Mortar, (float*)&Vars.espItem.TransmitterColor)));
		ItemContainer.emplace(1062, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.P90, (float*)&Vars.espItem.SMGColor)));
		ItemContainer.emplace(1063, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ACE32, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1064, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.M79, (float*)&Vars.espItem.ETCColor)));
		ItemContainer.emplace(1065, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.BlueZoneGrenade, (float*)&Vars.espItem.ThrowableColor)));
		ItemContainer.emplace(1068, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.O12, (float*)&Vars.espItem.ShotGunColor)));
		ItemContainer.emplace(1069, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Mk12, (float*)&Vars.espItem.DMRColor)));
		ItemContainer.emplace(1070, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FAMAS, (float*)&Vars.espItem.ARColor)));
		ItemContainer.emplace(1071, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.MP9, (float*)&Vars.espItem.SMGColor)));
		ItemContainer.emplace(1075, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.PickAex, (float*)&Vars.espItem.MelleColor)));
		ItemContainer.emplace(1101, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.AngledForegrip, (float*)&Vars.espItem.GripColor)));
		ItemContainer.emplace(1102, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.VerticalForegrip, (float*)&Vars.espItem.GripColor)));
		ItemContainer.emplace(1104, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Halfgrip, (float*)&Vars.espItem.GripColor)));
		ItemContainer.emplace(1105, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.LaserSight, (float*)&Vars.espItem.GripColor)));
		ItemContainer.emplace(1106, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.LightweightGrip, (float*)&Vars.espItem.GripColor)));
		ItemContainer.emplace(1108, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Thumbgrip, (float*)&Vars.espItem.GripColor)));
		ItemContainer.emplace(1109, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ExtendedMagARDMR, (float*)&Vars.espItem.MagazineColor)));
		ItemContainer.emplace(1110, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ExtendedMagSMG, (float*)&Vars.espItem.MagazineColor)));
		ItemContainer.emplace(1112, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ExtendedMagDMRSR, (float*)&Vars.espItem.MagazineColor)));
		ItemContainer.emplace(1113, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ExtQuickDrawMagARDMR, (float*)&Vars.espItem.MagazineColor)));
		ItemContainer.emplace(1114, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ExtQuickDrawMagSMG, (float*)&Vars.espItem.MagazineColor)));
		ItemContainer.emplace(1116, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ExtQuickDrawMagDMRSR, (float*)&Vars.espItem.MagazineColor)));
		ItemContainer.emplace(1121, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ChockSG, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1122, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CompensatorARDMR, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1123, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CompensatorSMG, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1124, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CompensatorDMRSR, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1149, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.MuzzleBrake, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1125, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.DuckbillSG, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1126, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FlashHiderARDMR, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1127, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FlashHiderSMG, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1128, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FlashHiderDMRSR, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1129, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.SuppressorARDMR, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1130, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.SuppressorSMG, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1132, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.SuppressorDMRSR, (float*)&Vars.espItem.MuzzleColor)));
		ItemContainer.emplace(1133, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CantedSight, (float*)&Vars.espItem.SightColor)));
		ItemContainer.emplace(1134, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.TacticalStock, (float*)&Vars.espItem.ButtstockColor)));
		ItemContainer.emplace(1136, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.BulletLoops, (float*)&Vars.espItem.ButtstockColor)));
		ItemContainer.emplace(1137, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CheekPad, (float*)&Vars.espItem.ButtstockColor)));
		ItemContainer.emplace(1138, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FoldingStock, (float*)&Vars.espItem.ButtstockColor)));
		ItemContainer.emplace(1139, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Scope4x, (float*)&Vars.espItem.SightColor)));
		ItemContainer.emplace(1140, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Scope2x, (float*)&Vars.espItem.SightColor)));
		ItemContainer.emplace(1141, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Scope8x, (float*)&Vars.espItem.SightColor)));
		ItemContainer.emplace(1142, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.RedDotSight, (float*)&Vars.espItem.SightColor)));
		ItemContainer.emplace(1143, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.HolographicSight, (float*)&Vars.espItem.SightColor)));
		ItemContainer.emplace(1144, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Scope15x, (float*)&Vars.espItem.SightColor)));
		ItemContainer.emplace(1145, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Scope3x, (float*)&Vars.espItem.SightColor)));
		ItemContainer.emplace(1146, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Scope6x, (float*)&Vars.espItem.SightColor)));
		ItemContainer.emplace(1147, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.HeaveStock, (float*)&Vars.espItem.ButtstockColor)));
		ItemContainer.emplace(1148, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Scope4xHot, (float*)&Vars.espItem.ButtstockColor)));
		ItemContainer.emplace(1201, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem._12Gauge, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1202, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem._300Magnum, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1203, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ACP45, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1204, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem._556mm, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1205, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem._762mm, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1206, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem._9mm, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1208, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FlareAmmo, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1209, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem._40mm, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1210, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem._57mm, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1211, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem._12GaugeSlug, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1212, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem._60mm, (float*)&Vars.espItem.AmmoColor)));
		ItemContainer.emplace(1301, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.JerryCan, (float*)&Vars.espItem.MelleColor)));
		ItemContainer.emplace(1302, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.AdrenalineSyringe, (float*)&Vars.espItem.MedicineColor)));
		ItemContainer.emplace(1303, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.EnergyDrink, (float*)&Vars.espItem.MedicineColor)));
		ItemContainer.emplace(1304, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.PainKiller, (float*)&Vars.espItem.MedicineColor)));
		ItemContainer.emplace(1305, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Bandage, (float*)&Vars.espItem.MedicineColor)));
		ItemContainer.emplace(1306, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.FirstAidKit, (float*)&Vars.espItem.MedicineColor)));
		ItemContainer.emplace(1307, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.MedKit, (float*)&Vars.espItem.MedicineColor)));

		ItemContainer.emplace(1401, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.VestLv3, (float*)&Vars.espItem.ArmorLv3Color)));
		ItemContainer.emplace(1402, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.VestLv2, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1403, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.VestLv2, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1404, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.BackpackLv3, (float*)&Vars.espItem.ArmorLv3Color)));
		ItemContainer.emplace(1405, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.BackpackLv3, (float*)&Vars.espItem.ArmorLv3Color)));
		ItemContainer.emplace(1408, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.BackpackLv2, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1409, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.BackpackLv2, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1412, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.HelmetLv2, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1413, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.HelmetLv2, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1414, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.HelmetLv3, (float*)&Vars.espItem.ArmorLv3Color)));
		ItemContainer.emplace(1415, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.InterferenceBackpack, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1501, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Item, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1502, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Item, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1601, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.EmergencyPickup, (float*)&Vars.espItem.ArmorLv2Color)));
		ItemContainer.emplace(1602, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.MountainBike, (float*)&Vars.espItem.MelleColor)));

		ItemContainer.emplace(1701, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.yingjichulizhuangbei, (float*)&Vars.espItem.TacticalColor)));
		ItemContainer.emplace(1702, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.guanchejing, (float*)&Vars.espItem.TacticalColor)));
		ItemContainer.emplace(1703, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.duoheyi, (float*)&Vars.espItem.TacticalColor)));
		ItemContainer.emplace(1704, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.wurenji, (float*)&Vars.espItem.TacticalColor)));
		ItemContainer.emplace(1706, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.zhanshubeibao, (float*)&Vars.espItem.TacticalColor)));
		ItemContainer.emplace(1707, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.NeonCoin, (float*)&Vars.espItem.TacticalColor)));
		ItemContainer.emplace(1603, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.BulletproofShield, (float*)&Vars.espItem.TacticalColor)));

		ItemContainer.emplace(1708, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CoverStructDropHandFlare, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1308, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.BattleReadyKit, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1709, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ChamberKeys, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1710, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ChamberKeys, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1711, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ChamberKeys, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1712, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ChamberKeys, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1713, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ChamberKeys, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1714, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.ChamberKeys, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1715, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.Revival_Transmitter, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1309, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.SelfRevive, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1716, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1717, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1718, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1719, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1720, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1721, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1722, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1723, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1724, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));
		ItemContainer.emplace(1725, std::make_shared<Item_Container>(Item_Container((bool*)&Vars.espItem.CraftMode, (float*)&Vars.espItem.UseableColor)));

		WeapSaveSettings.emplace(hash_("WeapDesertEagle_C"), &Vars.Misc.DesertEagle_Skin);
		WeapSaveSettings.emplace(hash_("WeapG18_C"), &Vars.Misc.G18_Skin);
		WeapSaveSettings.emplace(hash_("WeapM9_C"), &Vars.Misc.M9_Skin);
		WeapSaveSettings.emplace(hash_("WeapM1911_C"), &Vars.Misc.M1911_Skin);
		WeapSaveSettings.emplace(hash_("WeapNagantM1895_C"), &Vars.Misc.M1895_Skin);
		WeapSaveSettings.emplace(hash_("WeapRhino_C"), &Vars.Misc.Rhino_Skin);
		WeapSaveSettings.emplace(hash_("WeapSawnoff_C"), &Vars.Misc.SawedOff_Skin);
		WeapSaveSettings.emplace(hash_("Weapvz61Skorpion_C"), &Vars.Misc.Skorpion_Skin);
		WeapSaveSettings.emplace(hash_("WeapAK47_C"), &Vars.Misc.AK47_Skin);
		WeapSaveSettings.emplace(hash_("WeapAUG_C"), &Vars.Misc.AUG_Skin);
		WeapSaveSettings.emplace(hash_("WeapAWM_C"), &Vars.Misc.AWM_Skin);
		WeapSaveSettings.emplace(hash_("WeapBerreta686_C"), &Vars.Misc.Berreta686_Skin);
		WeapSaveSettings.emplace(hash_("WeapBerylM762_C"), &Vars.Misc.M762_Skin);
		WeapSaveSettings.emplace(hash_("WeapBizonPP19_C"), &Vars.Misc.PP19_Skin);
		WeapSaveSettings.emplace(hash_("WeapCrossbow_1_C"), &Vars.Misc.Crossbow_Skin);
		WeapSaveSettings.emplace(hash_("WeapDP12_C"), &Vars.Misc.DP12_Skin);
		WeapSaveSettings.emplace(hash_("WeapDP28_C"), &Vars.Misc.DP28_Skin);
		WeapSaveSettings.emplace(hash_("WeapFNFal_C"), &Vars.Misc.FNFAL_Skin);
		WeapSaveSettings.emplace(hash_("WeapG36C_C"), &Vars.Misc.G36C_Skin);
		WeapSaveSettings.emplace(hash_("WeapGroza_C"), &Vars.Misc.Groza_Skin);
		WeapSaveSettings.emplace(hash_("WeapHK416_C"), &Vars.Misc.HK416_Skin);
		WeapSaveSettings.emplace(hash_("WeapKar98k_C"), &Vars.Misc.Kar98k_Skin);
		WeapSaveSettings.emplace(hash_("WeapM16A4_C"), &Vars.Misc.M16A4_Skin);
		WeapSaveSettings.emplace(hash_("WeapM24_C"), &Vars.Misc.M24_Skin);
		WeapSaveSettings.emplace(hash_("WeapM249_C"), &Vars.Misc.M249_Skin);
		WeapSaveSettings.emplace(hash_("WeapMini14_C"), &Vars.Misc.Mini14_Skin);
		WeapSaveSettings.emplace(hash_("WeapMk14_C"), &Vars.Misc.MK14_Skin);
		WeapSaveSettings.emplace(hash_("WeapMk47Mutant_C"), &Vars.Misc.MK47_Skin);
		WeapSaveSettings.emplace(hash_("WeapMosinNagant_C"), &Vars.Misc.Mosin_Skin);
		WeapSaveSettings.emplace(hash_("WeapMP5K_C"), &Vars.Misc.MP5K_Skin);
		WeapSaveSettings.emplace(hash_("WeapQBU88_C"), &Vars.Misc.QBU88_Skin);
		WeapSaveSettings.emplace(hash_("WeapACE32_C"), &Vars.Misc.Ace32_Skin);
		WeapSaveSettings.emplace(hash_("WeapQBZ95_C"), &Vars.Misc.QBZ95_Skin);
		WeapSaveSettings.emplace(hash_("WeapSaiga12_C"), &Vars.Misc.Saiga12_Skin);
		WeapSaveSettings.emplace(hash_("WeapSCAR-L_C"), &Vars.Misc.SCARL_Skin);
		WeapSaveSettings.emplace(hash_("WeapSKS_C"), &Vars.Misc.SKS_Skin);
		WeapSaveSettings.emplace(hash_("WeapThompson_C"), &Vars.Misc.Thompson_Skin);
		WeapSaveSettings.emplace(hash_("WeapJS9_C"), &Vars.Misc.JS9_Skin);
		WeapSaveSettings.emplace(hash_("WeapUMP_C"), &Vars.Misc.UMP_Skin);
		WeapSaveSettings.emplace(hash_("WeapUZI_C"), &Vars.Misc.Uzi_Skin);
		WeapSaveSettings.emplace(hash_("WeapVector_C"), &Vars.Misc.Vector_Skin);
		WeapSaveSettings.emplace(hash_("WeapVSS_C"), &Vars.Misc.Vss_Skin);
		WeapSaveSettings.emplace(hash_("WeapWin1894_C"), &Vars.Misc.Win1894_Skin);
		WeapSaveSettings.emplace(hash_("WeapWin94_C"), &Vars.Misc.Win94_Skin);
		WeapSaveSettings.emplace(hash_("WeapWinchester_C"), &Vars.Misc.Winchester_Skin);
		WeapSaveSettings.emplace(hash_("WeapCowbar_C"), &Vars.Misc.Crowbar_Skin);
		WeapSaveSettings.emplace(hash_("WeapMachete_C"), &Vars.Misc.Machete_Skin);
		WeapSaveSettings.emplace(hash_("WeapPan_C"), &Vars.Misc.Pan_Skin);
		WeapSaveSettings.emplace(hash_("WeapSickle_C"), &Vars.Misc.Sickle_Skin);
		WeapSaveSettings.emplace(hash_("WeapK2_C"), &Vars.Misc.K2_Skin);
		WeapSaveSettings.emplace(hash_("WeapMk12_C"), &Vars.Misc.MK12_Skin);
		WeapSaveSettings.emplace(hash_("WeapP90_C"), &Vars.Misc.P90_Skin);
		WeapSaveSettings.emplace(hash_("WeapM1897_C"), &Vars.Misc.M1897_Skin);
		WeapSaveSettings.emplace(hash_("WeapMP9_C"), &Vars.Misc.MP9_Skin);
		WeapSaveSettings.emplace(hash_("WeapOriginS12_C"), &Vars.Misc.OriginS12_Skin);
		WeapSaveSettings.emplace(hash_("WeapDragunov_C"), &Vars.Misc.Dragunov_Skin);
		WeapSaveSettings.emplace(hash_("WeapFamasG2_C"), &Vars.Misc.FamasG2_Skin);
		WeapSaveSettings.emplace(hash_("WeapSmokeBomb_C"), &Vars.Misc.M18_Skin);
		WeapSaveSettings.emplace(hash_("WeapM79_C"), &Vars.Misc.M79_Skin);
		WeapSaveSettings.emplace(hash_("Back_Lv1"), &Vars.Misc.BackLv1_Skin);
		WeapSaveSettings.emplace(hash_("Back_Lv2"), &Vars.Misc.BackLv2_Skin);
		WeapSaveSettings.emplace(hash_("Back_Lv3"), &Vars.Misc.BackLv3_Skin);
		WeapSaveSettings.emplace(hash_("Armor_Lv1"), &Vars.Misc.ArmorLv1_Skin);
		WeapSaveSettings.emplace(hash_("Armor_Lv2"), &Vars.Misc.ArmorLv2_Skin);
		WeapSaveSettings.emplace(hash_("Armor_Lv3"), &Vars.Misc.ArmorLv3_Skin);
		WeapSaveSettings.emplace(hash_("Helmet_Lv1"), &Vars.Misc.HelmetLv1_Skin);
		WeapSaveSettings.emplace(hash_("Helmet_Lv2"), &Vars.Misc.HelmetLv2_Skin);
		WeapSaveSettings.emplace(hash_("Helmet_Lv3"), &Vars.Misc.HelmetLv3_Skin);
		CreateThread((LPSECURITY_ATTRIBUTES)NULL, (SIZE_T)NULL, (LPTHREAD_START_ROUTINE)&QueryNetInfoThread, (LPVOID)NULL, (DWORD)NULL, (LPDWORD)NULL);
		lastUpdateTime = std::chrono::steady_clock::now().time_since_epoch().count();
		AimbotThreadHandle = CreateThread((LPSECURITY_ATTRIBUTES)NULL, (SIZE_T)NULL, (LPTHREAD_START_ROUTINE)&AimbotThread, (LPVOID)NULL, (DWORD)NULL, (LPDWORD)NULL);
		CreateThread((LPSECURITY_ATTRIBUTES)NULL, (SIZE_T)NULL, (LPTHREAD_START_ROUTINE)&DetectedThread, (LPVOID)NULL, (DWORD)NULL, (LPDWORD)NULL);

		return m_Loaded;
	}
}