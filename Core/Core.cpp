#include "Include/WinHeaders.hpp"
#include "PatternLoader.h"
#include "DirectX/DirectX11.h"
#include "Menu.h"
#include "Helper/Misc/Utils.hpp"
#include "SDK/UEncrypt.h"
#include "SDK/UnrealNames.h"
#include "ThirdParty/cpplinq.hpp"
#include "Visuals.h"
#include "Config.h"
#include "ItemManager.h"
#include "FilePak.h"
#include "Core/Auth.h"
#include <ShlObj.h>
#include "Core/DriverControl.h"
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")
#define InternalBypass
#define AnnoceVersionDefine 66
extern std::string hudPath1;
extern std::string hudPath2;
extern std::string ServerIP;
namespace IronMan::Core
{
	using namespace SDK;
	extern long long g_expireDate;
	std::string pakPath;
	std::string itemConfigPath;
	std::string itemDefConfigPath;

	bool ReInit = false;
	bool ThreadExit = false;
	HANDLE ResHandle = nullptr;

	int DGErrorCount = 0;
	bool DGRuning = false;
	HANDLE DetectedGameHandle = nullptr;
	DWORD AttachGamePid = 0;

	HANDLE UpdateThreadHandle = NULL;
	ULONG GameTimeTick = 0;
	int DebugTime = 0;

	inline HMODULE GetModuleFromAddr(PVOID p)
	{
		MEMORY_BASIC_INFORMATION m = { 0 };
		VirtualQuery(p, &m, sizeof(MEMORY_BASIC_INFORMATION));
		return (HMODULE)m.AllocationBase;
	}

	DWORD ProcessThread(LPVOID S)
	{
		sdk::updateHandle();
		return 0;
	}
	//http://154.12.95.202:9999/getuser/offset  http://113.215.232.229:80
	bool GetPatternFromHost()
	{
		int loopCount = 0;
	retry:
		httplib::Client cli("http://113.215.232.234:80");
		auto res = cli.Get("/getuser/offset");
		//auto split = Utils::SplitString(res->body, "|");
		//CONSOLE_INFO2("size:%d", split.size());
		/*if (split.size() > 100)
		{
			PatternLoader::SetupOffset(split);
			return true;
		}*/
		if (res && res->status == 200) {
			auto split = Utils::SplitString(res->body, "|");
			if (split.size() > 100)
			{
				PatternLoader::SetupOffset(split);
				return true;
			}
		}
		else {
			if (loopCount < 3)
			{
				loopCount++;
				goto retry;
			}
			CONSOLE_INFO2(u8"连接服务器失败...请检查你的网络是否可用!");
			TerminateProcess(GetCurrentProcess(), -1);
			return false;
		}
		return false;
	}

	void UpdateThreadVEH()
	{
		if (!ThreadExit)
		{
			GetDMA().FreshAll();
			if (g_PatternData.ModuleHandle != 0)
			{
				if (GetDMA().Read<ptr_t>(g_PatternData.ModuleHandle) == 0)
				{
					Sleep(3000);
					CONSOLE_INFO("%p|%p|Exit1", g_PatternData.ModuleHandle, GetDMA().Read<ptr_t>(g_PatternData.ModuleHandle));
					g_PatternData.ModuleHandle = 0;
					AttachGamePid = 0;
					DGErrorCount = 0;
					ReInit = true;
					ThreadExit = false;
				}
			}
			Sleep(500);
		}
		else
		{
			if (g_PatternData.ModuleHandle != 0)
			{
				if (GetDMA().Read<ptr_t>(g_PatternData.ModuleHandle) == 0)
				{
					Sleep(3000);
					CONSOLE_INFO("%p|%p|Exit2", g_PatternData.ModuleHandle, GetDMA().Read<ptr_t>(g_PatternData.ModuleHandle));
					g_PatternData.ModuleHandle = 0;
					AttachGamePid = 0;
					DGErrorCount = 0;
					ReInit = true;
					ThreadExit = false;
				}
			}
			Sleep(1000);
		}
	}

	DWORD InitUpdateThread(LPVOID ss)
	{
		do
		{
			try
			{
				UpdateThreadVEH();
			}
			catch (...)
			{
				CONSOLE_INFO2("InitUpdateThread Exception!");
			}
		} while (true);
		return 0;
	}

	void UpdateOffset()
	{
		std::string SendtoHost =
			"CharacterState:4368|ptr_GetMousePostion:268668736|offset_PhysScene:504|offset_PxScene:268659856|GameTimePtr:261267520|InputAxisProperties:304|retaddrofLineTrace:24632676|CursorPosPtr:261405592|offset_HealthFlag1:528|offset_HealthFlag2:2656|offset_HealthFlag3:2645|offset_HealthIndex1:2644|offset_HealthIndex2:2672|offset_HealthXorKeys:2640|offset_HealthDecryptFunction:38990160|UObjectSort[0]:3|UObjectSort[1]:1|UObjectSort[2]:0|UObjectSort[3]:2|UObjectSort[4]:4|offset_JmpR14:302439681|offset_JmpR15:9137980|offset_bIsCanted:3860|offset_bSpawnBulletFromBarrel:3848|ComponentLocationOffset:576|offset_UFunction_Func:248|offset_ProcessEvent:219733704|offset_ComponentSpaceTransforms:2752|offset_ComponentToWorld:560|fnGetNames:39132224|GNames:271147992|UObjectArray:268407296|UObjectArrayNum:268407288|ElementsPerChunk:16840|UWrold_Ptr:181836200|UWorld:268675800|offset_OwningGameInstance:352|offset_LocalPlayers:184|offset_ViewportClient:160|offset_PlayerController:48|offsetAActors:368|offsetPersistentLevel:336|offset_UWorld:0|vtindex_GetWorld:38|decryptUWorld:5035962|decryptOwningGameInstance:4268799|decryptLocalPlayers:5035962|decryptViewportClient:3977934|decryptPlayerController:5049079|decryptPersistentLevel:4679560|encryAActors:2973360|decryptAActors:2974701|decryptNames:0|decryptUObject:2399884|decryptUObjectIndex:7674366|decryptUObjectArray:8886730|offset_UObjectFlags:36|offset_UObjectIndex:32|offset_UObjectNameNumber:20|offset_UObjectNameComparisonIndex:16|offset_UObjectOuter:8|offset_UObjectClass:24|UObjectSize:40|offset_UStruct_SuperStruct:0|offset_UStruct_Children:176|offset_UStruct_PropertiesSize:164|offset_UClassBaseChainArray:216|offset_UClassNumClassBasesInChainMinusOne:224|offset_UFunction_FunctionFlags:296|SteamResizeBuffer:0|UnrealEngineRender:0|EmptyMemory:0|fnFindTextInLocalizationTable:201196036|ExtFindTextInLocalizationTableRcx:268971760|ExtFindTextInLocalizationTableCall:201194852|fnGetComponentsByClass:0|offset_OwnedComponents:320|offset_OwnedActorComponent:0|fnGetCurrentWeapon:0|fnGetCurrentSway:0|fnLineTraceSingle:212393968|fnLineTraceSingleByChannel:8159676|pDefaultResponseParam:268621608|fnFCollisionQueryParamsConstructible:6945556|fnFCollisionQueryParamsDestructor:6947016|fnGetTslSettingsInstance:57366960|offset_TslSettings:488|offset_ATslCharacterUnk1:6972|offset_ATslCharacterUnk2:6680|offset_WorldPhysicsScene:0|fnGetPhysXScene:0|fnSetHitResultFromShapeAndFaceIndex:0|offset_Is_Scope:2812|offset_WorldLocation:2484|offset_JudgeMap:1480|offset_MapBaseAddr:263497192|offset_FireState:6660|offset_bThirdPerson:4285|offset_BackPackState:9480732|offset_Language:262022944|offset_ScopeCamera_Breath:0|offset_GameState:272|offset_UFont_LegacyFontSize:316|offset_UCanvas_SizeX:64|offset_UCanvas_SizeY:68|offset_UCurveVector_FloatCurves:48|offset_ULvel_WorldSettings:784|offset_SkeletalMeshSocket_SocketName:40|offset_SkeletalMeshSocket_BoneName:48|offset_SkeletalMeshSocket_RelativeLocation:56|offset_SkeletalMeshSocket_RelativeRotation:68|offset_SkeletalMeshSocket_RelativeScale:80|offset_SkeletalMeshSocket_bForceAlwaysAnimated:92|offset_USkeleton_Sockets:400|offset_USkeletalMesh_Skeleton:72|offset_USkeletalMesh_Sockets:704|offset_USkeletalMesh_RefSkeleton:288|offset_Actor_RootComponent:608|offset_Actor_Owner:80|offset_Actor_Instigator:784|offset_Actor_CustomTimeDilation:532|offset_Actor_LinearVelocity:96|offset_AController_Pawn:1128|offset_ATslGameState_NumAliveTeams:1244|offset_ATslGameState_LerpSafetyZonePosition:1380|offset_ATslGameState_LerpSafetyZoneRadius:1132|offset_ATslGameState_LerpBlueZonePosition:1460|offset_ATslGameState_LerpBlueZoneRadius:1484|offset_ATslGameState_RedZonePosition:1368|offset_ATslGameState_RedZoneRadius:1476|offset_ATslGameState_BlackZonePosition:2968|offset_ATslGameState_BlackZoneRadius:2980|offset_ATslGameState_SafetyZonePosition:1380|offset_ATslGameState_SafetyZoneRadius:1132|offset_AController_StateName:1112|offset_AController_ControlRotation:1088|offset_APawn_PlayerState:1080|offset_APawn_Controller:1056|offset_APawn_BaseEyeHeight:1028|offset_APlayerCameraManager_PCOwner:1016|offset_APlayerCameraManager_ViewTarget:4064|offset_APlayerCameraManager_CameraCache:1088|offset_MinimalViewInfo_FOV:1416|offset_MinimalViewInfo_Location:1444|offset_MinimalViewInfo_Rotation:1424|offset_TViewTarget_POV:16|offset_CameraCacheEntry_Timestamp:0|offset_CameraCacheEntry_POV:16|offset_AHUD_BlockInputWidgetList:1472|offset_AHUD_WidgetStateMap:1352|offset_AHUD_DebugCanvas:1096|offset_AHUD_Canvas:1088|offset_UActorComponent_bRegistered:392|offset_UTslVehicleCommonComponent_FuelMax:724|offset_UTslVehicleCommonComponent_Fuel:720|offset_UTslVehicleCommonComponent_HealthMax:716|offset_UTslVehicleCommonComponent_Health:712|offset_USceneComponent_ComponentVelocity:508|offset_USceneComponent_RelativeLocation:824|offset_USceneComponent_RelativeRotation:856|offset_USceneComponent_AttachParent:880|offset_UPrimitiveComponent_LastRenderTimeOnScreen:1872|offset_UPrimitiveComponent_LastRenderTime:1868|offset_UPrimitiveComponent_LastSubmitTime:1864|offset_UPrimitiveComponent_bRenderCustomDepth:1164|offset_UPrimitiveComponent_BodyInstance:1216|offset_USkinnedMeshComponent_SkeletalMesh:2736|offset_USkinnedMeshComponent_MasterPoseComponent:2744|offset_USkinnedMeshComponent_MasterBoneMap:2816|offset_USkinnedMeshComponent_CurrentEditableComponentTransforms:2784|offset_USkinnedMeshComponent_CurrentReadComponentTransforms:2788|offset_UTslAnimInstance_ControlRotation_CP:2036|offset_UTslAnimInstance_RecoilADSRotation_CP:2716|offset_UTslAnimInstance_ScopingAlpha_CP:3528|offset_UTslAnimInstance_LeanRightAlpha_CP:3760|offset_UTslAnimInstance_LeanLeftAlpha_CP:3756|offset_UTslAnimInstance_bIsScoping_CP:3526|offset_UTslAnimInstance_bIsTPPAiming:3525|offset_UTslAnimInstance_bIsAiming_CP:3524|offset_UTslAnimInstance_bIsReloading_CP:2440|offset_USkeletalMeshComponent_AnimScriptInstance:3544|offset_UStaticMeshSocket_SocketName:40|offset_UStaticMeshSocket_RelativeLocation:48|offset_UStaticMeshSocket_RelativeRotation:60|offset_UStaticMeshSocket_RelativeScale:72|offset_UStaticMesh_Sockets:192|offset_UStaticMeshComponent_StaticMesh:2752|offset_UWeaponMeshComponent_AttachedStaticComponentMap:4928|offset_UWeaponMeshComponent_ReplicatedSkinParam:4856|offset_UCharacterMovementComponent_Velocity:516|offset_UCharacterMovementComponent_JumpZVelocity:676|offset_UCharacterMovementComponent_WalkableFloorZ:688|offset_UCharacterMovementComponent_LastUpdateLocation:948|offset_UCharacterMovementComponent_LastUpdateRotation:960|offset_UCharacterMovementComponent_LastUpdateVelocity:976|offset_UCharacterMovementComponent_CurrentFloor:1120|offset_UCharacterMovementComponent_bit4:1276|offset_UCharacterMovementComponent_StanceMode:1280|offset_ACharacter_CharacterMovement:1496|offset_ACharacter_Mesh:1552|offset_UVehicleRiderComponent_SeatIndex:544|offset_UVehicleRiderComponent_LastVehiclePawn:608|offset_UWeaponProcessorComponent_EquippedWeapons:504|offset_UWeaponProcessorComponent_WeaponIndex:768|offset_ATslCharacterBase_Health:2624|offset_ATslCharacterBase_HealthMax:2676|offset_ATslCharacterBase_WeaponProcessor:2464|offset_ATslCharacter_Team:8240|offset_ATslCharacter_InventoryFacade:4064|offset_ATslCharacter_VehicleRiderComponent:8528|offset_ATslCharacter_GroggyHealth:6240|offset_ATslCharacter_GroggyHealthMax:5064|offset_ATslCharacter_AimOffsets:6920|offset_ATslCharacter_LastAimOffsets:7008|offset_ATslCharacter_ScopingCamera:4008|offset_ATslCharacter_SpectatedCount:4704|offset_ATslCharacter_LastTeamNum:8256|offset_ATslCharacter_CharacterName:2456|offset_APlayerController_AcknowledgedPawn:1192|offset_APlayerController_PlayerCameraManager:1232|offset_APlayerController_MyHUD:1224|offset_APlayerController_MapSig:1776|offset_APlayerController_RotationInput:1600|offset_APlayerController_InputYawScale:1612|offset_APlayerController_InputPitchScale:1616|offset_APlayerController_InputRollScale:1620|offset_ATslPlayerController_MaxValidPing:4012|offset_ATslPlayerController_PlayerInput:1352|offset_APlayerState_PlayerName:1080|offset_APlayerState_Ping:1104|offset_ATslPlayerState_PlayerStatistics:1240|offset_ATslPlayerState_PubgIdData:3008|offset_ATslPlayerState_DamageDealtOnEnemy:1160|offset_ATslPlayerState_PartnerLevel:1722|offset_AWheeledVehicle_Mesh:1128|offset_AWheeledVehicle_VehicleMovement:1136|offset_ATslFloatingVehicle_Mesh:1216|offset_ATslFloatingVehicle_VehicleCommonComponent:1248|offset_ATslWheeledVehicle_VehicleCommonComponent:3104|offset_ADroppedItem_Item:1112|offset_UItem_StackCount:56|offset_UItem_ItemTableRowPtr:168|offset_ItemTableRowBase_ItemName:24|offset_ItemTableRowBase_ItemCategory:48|offset_ItemTableRowBase_ItemDetailedName:72|offset_ItemTableRowBase_ItemDescription:96|offset_ItemTableRowBase_ItemID:616|offset_UDroppedItemInteractionComponent_Item:2008|offset_AInventory_Items:1176|offset_UEquipableItem_EquipSlotID:352|offset_UEquipableItem_DurabilityMax:528|offset_UEquipableItem_Durability:532|offset_UEquipableItem_ReplicatedSkinItem:376|offset_AEquipment_Items:1152|offset_AItemPackage_Items:1424|offset_ATslWeapon_Mesh3P:2000|offset_ATslWeapon_MuzzleAttachPoint:2168|offset_ATslWeapon_FiringAttachPoint:2176|offset_ATslWeapon_AttachedItem:2088|offset_ATslWeapon_Gun_ScopingAttachPoint:3232|offset_ATslWeapon_Gun_CurrentAmmoData:2744|offset_ATslWeapon_Gun_CurrentZeroLevel:2748|offset_ATslWeapon_Trajectory_TrajectoryGravityZ:4092|offset_ATslWeapon_Trajectory_WeaponTrajectoryData:4304|offset_WeaponTrajectoryData_TrajectoryConfig:256|offset_ATslProjectile_ProjectileConfig:1600|offset_ATslProjectile_CollisionComp:1200|offset_ATslProjectile_ExplosionDelay:1592|offset_ATslProjectile_TimeTillExplosion:1964|offset_UTslSettings_BallisticDragScale:1244|offset_UTslSettings_BallisticDropScale:1248|offset_UBodySetup_WalkableSlopeOverride:720|offset_UWidget_Visibility:161|offset_UWidget_Slot:48|offset_UCanvasPanelSlot_LayoutData:56|offset_UTslNewWorldMapWidget_MapGrid:1496|offset_UMinimapCanvasWidget_SelectMinimapSizeIndex:1456|offset_UMinimapCanvasWidget_MinimapSizeOffset:1292|offset_UTslGameUserSettings_SelectMiniMapIndex:424|offset_AInventoryFacade_Inventory:1040|offset_AInventoryFacade_Equipment:1024|";
		httplib::Client cli("http://127.0.12.103:9999");
		httplib::Params params;
		params.emplace("name", "offset");
		params.emplace("uid", SendtoHost.c_str());
		auto res = cli.Post("/user", params);
		if (res && res->status == 200) {
			CONSOLE_INFO2(u8"UpdateOffset Success!");
		}
		else {
			CONSOLE_INFO2(u8"UpdateOffset Failed!");
		}
	}

	DWORD LoadResourceThread(LPVOID ss)
	{
		try
		{
			//sdk::Initialize();
			CONSOLE_INFO("SDK init Success");
			if (!Vars.Menu.firstInit)
			{
#ifndef NoWriteMem
				if (!sdk::LocalizationInitalize())
				{
					MessageBoxA(0, "Localization Failed!", 0, 0);
					TerminateProcess(GetCurrentProcess(), 1);
				}
#endif
				CONSOLE_INFO2(u8"加载资源中 1/3");
				Visuals::Initialize();
				CONSOLE_INFO2(u8"加载资源中 2/3");
				Menu::Initialize(g_ServerResult.English);
				CONSOLE_INFO2(u8"加载资源中 3/3");
#ifndef NoWriteMem
				sdk::Localization();
				CONSOLE_INFO2(u8"加载资源完成");
				sdk::ClearLocalizationBuff();
#endif
				CONSOLE_INFO2(u8"启动更新线程");
				CreateThread(NULL, NULL, ProcessThread, NULL, NULL, NULL);
				Vars.Menu.firstInit = true;
			}
#ifndef NoWriteMem
			CONSOLE_INFO2(u8"正在初始化离线皮肤资源...");
			Visuals::InitSkinData();
#endif
			CONSOLE_INFO2(u8"正在初始化离线资源...");
			g_PatternData.SteamClient64 = GetDMA().getModuleHandle(("gameoverlayrenderer64.dll"));
			ThreadExit = true;
			Vars.Menu.PauseEveryThing = false;
			Vars.Menu.bNeedReFresh = false;
		}
		catch (...)
		{
			MessageBoxA(0, "載入資源發生錯誤，請重新開啟輔助", "Error", 0);
			TerminateProcess(GetCurrentProcess(), 1);
		}
		return 1;
	}

	DWORD ThreadStartRadar(LPVOID ss)
	{
		CHAR my_documents[MAX_PATH];
		SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
		system((std::string(my_documents) + "\\PaoD-Radar\\bin\\java.exe -jar  " + std::string(my_documents) + "\\NeoDMA\\5.bin").c_str());
		return 1;
	}

	DWORD ThreadGetGame(LPVOID s)
	{
		do
		{
			AttachGamePid = 0;
			if (!DGRuning)
			{
				DGRuning = true;
			}
			auto PidList = GetDMA().GetProcessidByEnum("TslGame.exe");
			if (PidList.size() < 2)
			{
				Sleep(3000);
				continue;
			}
			for (int i = 0; i < PidList.size(); i++)
			{
				if (g_PatternData.ModuleHandle != 0)
					PatternLoader::ReFixOffset();
				GetDMA().AttachProcess(PidList[i], "TslGame.exe");
				g_PatternData.ModuleHandle = GetDMA().getBaseAddress();
				if (!g_PatternData.ModuleHandle)
					continue;
				PatternLoader::FixOffset();
				if (!GetDMA().Read<ptr_t>(g_PatternData.fnGetNames))
					continue;
				std::string NeedModuleName = "discord_game_sdk.dll";
				ptr_t discordModule = GetDMA().getModuleHandle(NeedModuleName);
				if (!discordModule)
					discordModule = GetDMA().GetProcessModule(PidList[i], NeedModuleName);
				if (discordModule > 0)
				{
					AttachGamePid = PidList[i];
					break;
				}
			}
			if (AttachGamePid)
				break;
			Sleep(3000);
		} while (true);
		return 1;
	}

	void printStackTrace() {
		void* stack[100];
		unsigned short frames;
		SYMBOL_INFO* symbol;
		HANDLE process = GetCurrentProcess();

		SymInitialize(process, NULL, TRUE);

		frames = CaptureStackBackTrace(0, 100, stack, NULL);
		symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

		for (unsigned short i = 0; i < frames; i++) {
			SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
			CONSOLE_INFO("Exception From:%d:%s,%p", frames - i - 1, symbol->Name, symbol->Address);
		}

		free(symbol);
	}

	void sehTranslator(unsigned int code, _EXCEPTION_POINTERS* pExp) {
		CONSOLE_INFO2(u8"程式發生中斷,請聯絡管理員傳送訊息不要關閉窗口!");
		CONSOLE_INFO2("Structured exception occurred!");
		CONSOLE_INFO2("Exception code: 0x%x", code);

		if (pExp && pExp->ExceptionRecord) {
			CONSOLE_INFO2("Exception address: %p", pExp->ExceptionRecord->ExceptionAddress);
			auto dll4 = GetModuleHandleA("4.dll");
			auto module = GetModuleHandleA(nullptr);
			CONSOLE_INFO2("Exception offset: %p", dll4 ? ((ptr_t)pExp->ExceptionRecord->ExceptionAddress - (ptr_t)dll4) : ((ptr_t)pExp->ExceptionRecord->ExceptionAddress - (ptr_t)module));
			CONSOLE_INFO2("Exception flags: %p", pExp->ExceptionRecord->ExceptionFlags);
		}

		printStackTrace();
		//std::abort(); // 强制程序崩溃以便调试
	}

	void printCurrentException() {
		try {
			if (std::exception_ptr eptr = std::current_exception()) {
				std::rethrow_exception(eptr);
			}
		}
		catch (const std::exception& e) {
			CONSOLE_INFO2("Caught exception: %s", e.what());
		}
		catch (...) {
			CONSOLE_INFO2("Caught unknown exception");
		}
	}

	void terminateHandler() {
		CONSOLE_INFO2(u8"程式發生中斷,請聯絡管理員傳送訊息不要關閉窗口!");
		CONSOLE_INFO2("Unhandled exception occurred!");
		printStackTrace();
		printCurrentException();
		//std::abort(); // 强制程序崩溃以便调试
	}

	/*std::string GetPlayerIDTest(std::string Name, std::string APIKEY)
	{
		std::string APIKey = APIKEY;
		httplib::Client cli("https://api.pubg.com");
		httplib::Headers headers = {
			{ "Accept", "application/vnd.api+json" },
			{ "Authorization", "Bearer " + APIKey}
		};
	Retry:
		auto res = cli.Get("/shards/steam/players?filter[playerNames]=" + Name, headers);
		if (res && res->status == 200) {
			return Utils::GetTextBetween(res->body, "\"id\":\"", "\",\"");
		}
		else if (res) {
			if (res->status == 429)
			{
				goto Retry;
			}
			else if (res->status == 401)
			{
				CONSOLE_INFO2(u8"%d Un|%s", res->status, APIKEY.c_str());
				return "";
			}
			else if (res->status == 404)
			{
				CONSOLE_INFO2(u8"%d not found", res->status);
				return "";
			}
			else
			{
				CONSOLE_INFO2(u8"%d Error", res->status);
				return "";
			}
		}
		else if (!res)
		{
			goto Retry;
		}
		return "null";
	}*/

	std::string GetServerIP()
	{
		int loopCount = 0;
	retry:
		auto Result = Utils::SimpleHttpGet("https://note.youdao.com/yws/api/personal/file/WEB6ebb3b1974a0bfb5002cac2fbfa9eecc?method=download&inline=true&shareKey=255c69463015f6a0cd82cad5872599c4", "");
		if (Result.find("http://") != std::string::npos) {
			//MessageBoxA(0, Result.c_str(), 0, 0);
			
			return Result;
		}
		else {
			if (loopCount < 3)
			{
				loopCount++;
				goto retry;
			}
			CONSOLE_INFO2(u8"無法從伺服器取得資料!");
			return "http://127.0.12.103:9999";
		}
		return "http://127.0.12.103:9999";
	}


	bool Initialize(ptr_t imageBase)
	{
		//GetPatternFromHost();
		//UpdateOffset();
		//MessageBoxA(0, "test", 0, 0);
		//auto FileContent = Utils::readFileToByteArray("C:\\APIKEY.bin");
		//auto SpiltText = Utils::split(FileContent, "|");
		//CONSOLE_INFO2("size:%d",SpiltText.size());
		//for (int i = 0; i < SpiltText.size(); i++)
		//{
		//	auto Result = Utils::SimpleHttpGet("https://api.pubg.com/shards/steam/players?filter[playerNames]=TobyPao_-", "accept: application/vnd.api+json\r\nAuthorization:Bearer " + SpiltText[i]);
		//	//auto Result = GetPlayerIDTest("TobyPao_-", SpiltText[i]);
		//	if (Result.find("account.") != std::string::npos)
		//	{
		//		CONSOLE_INFO2(u8"account Find!");
		//	}
		//	else
		//	{
		//		CONSOLE_INFO2(u8"%d Error : %s", i, SpiltText[i].c_str());
		//	}
		//}
		//return 0;
		_set_se_translator(sehTranslator);
		// 设置 C++ 未处理异常处理程序、
	
		std::set_terminate(terminateHandler);
		Vars.Misc.RankServer = 1;
		MSG msg = { 0 };
		CONSOLE_INFO2(u8"启动中,正在进行初始化");

//		if (!NT_SUCCESS(Driver().EnsureLoaded())
//#ifndef ENABLE_CONSOLE
//			|| !NT_SUCCESS(Driver().ProtectProcess())
//#endif
//			//|| !NT_SUCCESS(Driver().ReadMem(4, 0x500, sizeof(ServerResult), &g_ServerResult))
//			)
//		{
//			TerminateProcess(GetCurrentProcess(), (UINT)0);
//		}
		CONSOLE_INFO2(u8"正在验证用户数据");
		//g_expireDate = g_ServerResult.expireDate;
		/*if (Auth::Check())
		{
			ServerIP = GetServerIP();
			CHAR my_documents[MAX_PATH];
			SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
			if (Utils::FileExists(std::string(my_documents) + ("\\NeoDMA\\3.bin")))
			{
				hudPath1 = std::string(my_documents) + ("\\NeoDMA\\5298.ini");
				hudPath2 = std::string(my_documents) + ("\\NeoDMA\\7469.ini");
				pakPath = std::string(my_documents) + ("\\NeoDMA\\3.bin");
				itemConfigPath = std::string(my_documents) + ("\\NeoDMA\\ItemConfig.ini");
				itemDefConfigPath = std::string(my_documents) + ("\\NeoDMA\\ItemDefaultConfig.ini");
			}
			else
				return 1;

			CONSOLE_INFO2(u8"加载资源包中");
			if (Package::GetPackage()->Load(pakPath))
			{
				Config = new CConfig();
				Config->Load(hudPath1);
				if (Vars.Menu.AnnoceVersion != AnnoceVersionDefine)
				{
					Vars.Menu.AnnoceOpen = true;
					Vars.Menu.AnnoceVersion = AnnoceVersionDefine;
				}
				CONSOLE_INFO2(u8"初始化 绘制 & 雷达");
				CreateThread(NULL, NULL, ThreadStartRadar, NULL, NULL, NULL);
				if (!OverlayEngine::Initialize())
				{
					MessageBoxA(NULL, "无法创建绘制!", "ERROR", MB_OK | MB_ICONHAND);
					TerminateProcess(GetCurrentProcess(), 0);
					return false;
				}
			}
			else
			{
				MessageBoxA(0, ("资源包获取失败"), 0, 0);
				TerminateProcess(GetCurrentProcess(), 0);
			}

		}
		else
		{
			MessageBoxA(0, ("Error4"), 0, 0);
			TerminateProcess(GetCurrentProcess(), 0);
		}*/

		//ServerIP = "http://113.215.232.229:80"; //http://154.12.95.202:9999

		CHAR my_documents[MAX_PATH];
		SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
		if (Utils::FileExists(std::string(my_documents) + ("\\NeoDMA\\3.bin")))
		{
			hudPath1 = std::string(my_documents) + ("\\NeoDMA\\5298.ini");
			hudPath2 = std::string(my_documents) + ("\\NeoDMA\\7469.ini");
			pakPath = std::string(my_documents) + ("\\NeoDMA\\3.bin");
			itemConfigPath = std::string(my_documents) + ("\\NeoDMA\\ItemConfig.ini");
			itemDefConfigPath = std::string(my_documents) + ("\\NeoDMA\\ItemDefaultConfig.ini");
		}
		else
			return 1;

		CONSOLE_INFO2(u8"加载资源包中");
		if (Package::GetPackage()->Load(pakPath))
		{
			Config = new CConfig();
			Config->Load(hudPath1);
			if (Vars.Menu.AnnoceVersion != AnnoceVersionDefine)
			{
				Vars.Menu.AnnoceOpen = true;
				Vars.Menu.AnnoceVersion = AnnoceVersionDefine;
			}
			CONSOLE_INFO2(u8"初始化 绘制 & 雷达");
			CreateThread(NULL, NULL, ThreadStartRadar, NULL, NULL, NULL);
			if (!OverlayEngine::Initialize())
			{
				MessageBoxA(NULL, "无法创建绘制!", "ERROR", MB_OK | MB_ICONHAND);
				TerminateProcess(GetCurrentProcess(), 0);
				return false;
			}
		}
		else
		{
			MessageBoxA(0, ("资源包获取失败"), 0, 0);
			TerminateProcess(GetCurrentProcess(), 0);
		}
		CONSOLE_INFO2(u8"正在初始化DMA");
		GetDMA().GetHandle();

		if (!GetDMA().InitKeyboard())
		{
			MessageBoxA(0, "無法取得主機系統版本信息，請重新啟動主機Explorer.exe進程\n", 0, 0);
			return 1;
		}
	ReDetectedGame:
		if (g_ServerResult.Version >= 2)
			Vars.Menu.PhysxTrace = true;
		else
			Vars.Menu.PhysxTrace = false;
		CONSOLE_INFO2(u8"正在连接服务器.");
		Vars.Menu.Opened = true;
		//Vars.Menu.战斗模式 = false;
		OverlayEngine::HideWindow();
		Vars.Menu.PauseEveryThing = true;
		//GetPatternFromHost();

		if (!UpdateThreadHandle)
		{
			UpdateThreadHandle = CreateThread(NULL, NULL, InitUpdateThread, NULL, NULL, NULL);
		}
		g_PatternData.UObjectArray = std::min(g_PatternData.UObjectArray, g_PatternData.UObjectArrayNum);
		CONSOLE_INFO2(u8"服务器连接成功!");


		DGErrorCount = 0;
		DetectedGameHandle = CreateThread(NULL, NULL, ThreadGetGame, NULL, NULL, NULL);
		if (DetectedGameHandle == NULL) {
			CONSOLE_INFO2(u8"无法启动监视游戏线程!");
			return 1;
		}
		ReInit = false;
		CONSOLE_INFO2(u8"正在等待游戏启动");

		do
		{
			if (DGRuning)
			{
				DGRuning = false;
				DGErrorCount = 0;
			}
			else
			{
				DGErrorCount++;
			}
			if (DGErrorCount >= 10)
			{
				CONSOLE_INFO("Detect Thread Exception!");
				TerminateThread(DetectedGameHandle, -1);
				DetectedGameHandle = nullptr;
				ReInit = false;
				goto ReDetectedGame;
			}
			DWORD dwWaitResult = WaitForSingleObject(DetectedGameHandle, 1000);
			if (dwWaitResult == WAIT_TIMEOUT) {
				continue;
			}
			else if (dwWaitResult == WAIT_OBJECT_0) {
				break;
			}
		} while (true);
		if (!AttachGamePid)
			goto ReDetectedGame;


		ResHandle = CreateThread(NULL, NULL, LoadResourceThread, NULL, NULL, NULL);
		if (ResHandle == NULL) {
			CONSOLE_INFO2(u8"无法创建资源加载线程");
			return 1;
		}
		ReInit = false;
		do
		{
			if (ReInit)
			{
				CONSOLE_INFO("Detect Game Exit,Need To ReSet1-%d", ReInit);
				TerminateThread(ResHandle, -1);
				ResHandle = nullptr;
				ReInit = false;
				goto ReDetectedGame;
			}
			DWORD dwWaitResult = WaitForSingleObject(ResHandle, 1000);
			if (dwWaitResult == WAIT_TIMEOUT) {
				continue;
			}
			else if (dwWaitResult == WAIT_OBJECT_0) {
				break;
			}
		} while (true);
		while (OverlayEngine::bInitSuccess() == false)
		{
			Sleep(1);
			//CONSOLE_INFO("IMGUI Init Not Finished!");
		}
		OverlayEngine::InitWindow();
		CONSOLE_INFO2(u8"开启成功...请开始游玩!");
		while (true)
		{
			try
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					continue;
				}
				OverlayEngine::Start();
				if (ReInit)
				{
					CONSOLE_INFO("Detect Game Exit,Need To ReSet2-%d", ReInit);
					ReInit = false;
					goto ReDetectedGame;
				}
			}
			catch (...)
			{

			}
		}
		CONSOLE_INFO2("Process Has Exit");
		return true;
	}
}