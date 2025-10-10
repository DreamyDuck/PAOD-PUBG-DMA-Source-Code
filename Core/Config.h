#pragma once
#include <vector>
#include <string>

namespace IronMan::Core
{
	constexpr int ConfigVer = 2;

	enum ESP_PLAYER_TYPE :int
	{
		ESP_TYPE_2D,
		ESP_TYPE_2DB,
		ESP_TYPE_3D
	};


	struct Variables
	{
	public:
		Variables() { ZeroMemory(this, sizeof(Variables)); };
		~Variables() {}
		struct sEspHuman
		{
			int Enable;
			int Skeleton;
			int HeadCircle;
			float SkeletonVis[4];
			float SkeletonInVis[4];
			int Box;
			float BoxVis[4];
			float AllLockColor[4];
			int PlayerName;
			float PlayerNameColor[4];
			int PlayerWeapon;
			float PlayerWeaponColor[4];
			int HealthBar;
			float HealthBarColor[4];
			float HealthBarOutLineColor[4];
			int Distance;
			float DistanceColor[4];
			int PlayerTeam;
			int PlayerSight;
			float PlayerSightColor[4];
			int KillCount;
			int Observer;
			int Spectator;
			int SuvivalLevel;
			float SuvivalLevelColor[4];
			int Collaborator;
			float CollaboratorColor[4];

			float SimpleInfoSlider;
			float DistanceSlider;
			float Thinkness;
			float TextSize;
		}espHuman; //Esp
		struct sEspItem
		{
			int EnableHotKey;
			int SmartItem;
			int SmartItemHotKey;
			int Item;
			int ItemPictures;
			int ItemText;
			float ImageSize;
			float TextSize;
			float ItemDistance;
			int ALL_Ammo;
			int ALL_Grenade;
			int ALL_Weapon;
			int ALL_Armor;
			int ALL_Medicine;
			int ALL_Attachment;
			float ARColor[4];
			int AKM;
			int Groza = 0;
			int ACE32 = 0;
			int M762;
			int MK47;
			int K2;
			int QBZ;
			int AUG;
			int G36C;
			int M416;
			int M16A4;
			int SCARL;
			int FAMAS;
			float DMRColor[4];
			int VSS;
			int QBU;
			int SLR;
			int SKS;
			int Mk12;
			int Mk14;
			int Mini14;
			int Dragunov;
			float SRColor[4];
			int M24;
			int AWM;
			int Win94;
			int Kar98k;
			int LynxAMR;
			int MosinNagant;
			float ThrowableColor[4];
			int C4;
			int SmokeBoom;
			int FlashBang;
			int Molotov;
			int FragGrenade;
			int BlueZoneGrenade;
			int CoverStructDropHandFlare;
			float ShotGunColor[4];
			int O12;
			int DBS;
			int S12K;
			int S686;
			int S1897;
			float SMGColor[4];
			int P90;
			int MP9;
			int MP5K;
			int Vector;
			int UMP45;
			int MicroUZI;
			int PP19Bizon;
			int TommyGun;
			int JS9;
			float HandGunColor[4];
			int R45;
			int P92;
			int P18C;
			int P1911;
			int R1895;
			int Skoripion;
			int SawedOff;
			int Deagle;
			int StunGun;
			float MelleColor[4];
			int Crowbar;
			int Sickle;
			int Machete;
			int Pan;
			int JerryCan;
			int PickAex;
			int MountainBike;
			int NeonCoin;
			float LMGColor[4];
			int MG3;
			int M249;
			int DP28;
			float ETCColor[4];
			int Crossbow;
			int FlareGun;
			int M79;
			float TransmitterColor[4];
			int Mortar;
			int Panzerfaust;
			float ArmorLv2Color[4];
			int HelmetLv2;
			int VestLv2;
			int BackpackLv2;
			int InterferenceBackpack;
			int EmergencyPickup;
			int BulletproofShield;
			float ArmorLv3Color[4];
			int HelmetLv3;
			int VestLv3;
			int BackpackLv3;
			float SightColor[4];
			int CantedSight;
			int Scope6x;
			int RedDotSight;
			int Scope8x;
			int HolographicSight;
			int Scope15x;
			int Scope2x;
			int Thermal4xScope;
			int Scope3x;
			int Scope4x;
			int Scope4xHot;
			float MagazineColor[4];
			int ExtQuickDrawMagSMG;
			int ExtQuickDrawMagARDMR;
			int ExtQuickDrawMagDMRSR;
			int ExtendedMagSMG;
			int ExtendedMagARDMR;
			int ExtendedMagDMRSR;
			float AmmoColor[4];
			int ACP45;
			int _556mm;
			int _762mm;
			int _57mm;
			int _9mm;
			int _40mm;
			int _60mm;
			int _12Gauge;
			int _12GaugeSlug;
			int _300Magnum;
			int FlareAmmo;
			float MuzzleColor[4];
			int ChockSG;
			int SuppressorSMG;
			int DuckbillSG;
			int SuppressorARDMR;
			int FlashHiderSMG;
			int SuppressorDMRSR;
			int FlashHiderARDMR;
			int FlashHiderDMRSR;
			int CompensatorSMG;
			int CompensatorARDMR;
			int CompensatorDMRSR;
			int MuzzleBrake;
			float GripColor[4];
			int LightweightGrip;
			int Thumbgrip;
			int VerticalForegrip;
			int AngledForegrip;
			int Halfgrip;
			int LaserSight;
			float ButtstockColor[4];
			int CheekPad;
			int TacticalStock;
			int FoldingStock;
			int BulletLoops;
			int HeaveStock;
			float MedicineColor[4];
			int MedKit;
			int FirstAidKit;
			int Bandage;
			int PainKiller;
			int EnergyDrink;
			int AdrenalineSyringe;
			float UseableColor[4];
			int BattleReadyKit;
			int SelfRevive;
			int ChamberKeys;
			int Revival_Transmitter;
			float TacticalColor[4];
			int yingjichulizhuangbei;
			int guanchejing;
			int duoheyi;
			int wurenji;
			int zhanshubeibao;
			int CraftMode;
		}espItem; //Esp
		struct sEspAirDrop
		{
			int AirDrop;
			float AirDropColor[4];
			int Loot;
			float LootColor[4];
			int LootInside;
		}espAirDrop; //Esp
		struct sEspVehicle
		{
			int Enable;
			int EnableHotKey;
			int Vehicle;
			float VehicleColor[4];
			float VehicleDistance;
			int VehicleHealth;
		}espVehicle; //Esp
		struct sEspRank
		{
			int Rank;
			int RankTexture;
			int KD;
			int Mode;
			int Team;
			int RankServer;
		}espRank; //Esp
		struct sAimbot
		{
			int HighAimbot;
			int GrenadePredict;
			int HandNotLock;
			int Aimbot;
			int AimbotDrag;
			int CheckReloading;
			int VisibleCheck;
			int AR_Knock;
			int SR_Knock;
			int DMR_Knock;
			int AR_PredictionPoint;
			int SR_PredictionPoint;
			int DMR_PredictionPoint;
			float PredictionPointColor[4];
			int Team;
			int FovShow;
			float FovColor[4];
			int Crosshair;
			float CrosshairColor[4];
			float CrosshairSize;
			int AimWhileScoping;
			int AR_LockHeadHotKey;
			int SR_LockHeadHotKey;
			int DMR_LockHeadHotKey;
			int AR_HotKey1;
			int AR_HotKey2;
			int SR_HotKey1;
			int SR_HotKey2;
			int DMR_HotKey1;
			int DMR_HotKey2;
			int PauseLockKey;
			int HotKeyCar;
			bool StartLockCar;
			int EnableHotKey;
			int AutoShot;
			int RiferAutoShot;
			int ShotGunAutoShot;
			int SRGunAutoShot;
			float SRAutoShotDelay1;
			float SRAutoShotDelay2;
			int AutoShotHotKey;
			int InKnockDownHotKey;
			int KnockDownHotKey;
			int Controller_Current;

			int AimBotMethod;
			int BProStatus;
			int BNetStatus;
			int LurkerStatus;
			int JSMHStatus;

			float DownValue;

			int SingleStep;

			float SR_MOVESTEP_X1;
			float SR_MOVESTEP_Y1;
			float SR_MOVESTEP_X2;
			float SR_MOVESTEP_Y2;
			float DMR_MOVESTEP_X1;
			float DMR_MOVESTEP_Y1;
			float DMR_MOVESTEP_X2;
			float DMR_MOVESTEP_Y2;
			int MenuWeaponType;

			//步枪
			int AR_NoRecoil1;
			int AR_NoRecoil2;
			//演员
			float AR_P_X1;
			float AR_P_Y1;
			float AR_RecoilValue1;
			float AR_SmoothValue1;
			float AR_FovRange1;
			//暴力
			float AR_P_X2;
			float AR_P_Y2;
			float AR_RecoilValue2;
			float AR_SmoothValue2;
			float AR_FovRange2;


			int AimMode;
			//栓狙
			int SR_NoRecoil1;
			int SR_NoRecoil2;
			//演员
			float SR_baseSmoothing1;
			float SR_RecoilValue1;
			float SR_SmoothValue1;
			float SR_FovRange1;
			//暴力
			float SR_baseSmoothing2;
			float SR_RecoilValue2;
			float SR_SmoothValue2;
			float SR_FovRange2;
			//基础
			float SR_baseFOV;
			float SR_MaxSmoothIncrease;

			//连狙
			int DMR_NoRecoil1;
			int DMR_NoRecoil2;
			//演员
			float DMR_baseSmoothing1;
			float DMR_RecoilValue1;
			float DMR_SmoothValue1;
			float DMR_FovRange1;
			//暴力
			float DMR_baseSmoothing2;
			float DMR_RecoilValue2;
			float DMR_SmoothValue2;
			float DMR_FovRange2;
			//基础
			float DMR_baseFOV;
			float DMR_MaxSmoothIncrease;


			float I_X;
			float D_X;
			float I_Y;
			float D_Y;

			int DebugMode;
			int RayTracing;
			int TriangleMesh;
			int StaticMesh;
			int DynamicMesh;
			float MaxDynamicRigidDistance;
			int MaxUpdateRigid;
			char* txt_CacheSave;

			char* txt_IP;
			char* txt_Port;
			char* txt_UUID;
			char* txt_BPPort;
			char* txt_LKPort;
			char* txt_JSPort;
		}Aimbot;//自瞄
		struct sMisc
		{
			int ShareRadar;
			bool ShareRadarState;
			char* txt_RadarIP;

			int BattlePlayerInfo;
			int BattlePlayerInfoHotKey;

			int Projectile;
			int PorjectileTime;
			int PorjectileLine;
			int ProjectileType;
			float ProjectileColor[4];
			int ProjectRadius;
			float ProjectDamageDistance;
			float ProjectRadiusDistance;
			float ProjectRadiusColor[4];
			BYTE multiProject_num[20];

			int Radar;
			int MRadar;
			float RadarRadius;
			float MRadarRadius;
			int VehicleMap;
			int AirDropsMap;
			int NameMap;
			int Warning;
			int WarningExt;

			int BattlefieldInfo;
			int BattlefieldInfoHotKey;
			int BattlefieldUnlock;
			int BattlefieldUnlockHotKey;

			int EnableCollision;
			int WeaponSkin;

			int DesertEagle_Skin;
			int G18_Skin;
			int M9_Skin;
			int M1911_Skin;
			int M1895_Skin;
			int Rhino_Skin;
			int SawedOff_Skin;
			int Skorpion_Skin;
			int AK47_Skin;
			int AUG_Skin;
			int AWM_Skin;
			int Berreta686_Skin;
			int M762_Skin;
			int PP19_Skin;
			int Crossbow_Skin;
			int DP12_Skin;
			int DP28_Skin;
			int FNFAL_Skin;
			int G36C_Skin;
			int Groza_Skin;
			int HK416_Skin;
			int Kar98k_Skin;
			int M16A4_Skin;
			int M24_Skin;
			int M249_Skin;
			int Mini14_Skin;
			int MK14_Skin;
			int MK47_Skin;
			int Mosin_Skin;
			int MP5K_Skin;
			int QBU88_Skin;
			int Ace32_Skin;
			int QBZ95_Skin;
			int Saiga12_Skin;
			int SCARL_Skin;
			int SKS_Skin;
			int JS9_Skin;
			int Thompson_Skin;
			int UMP_Skin;
			int Uzi_Skin;
			int Vector_Skin;
			int Vss_Skin;
			int Win1894_Skin;
			int Win94_Skin;
			int Winchester_Skin;
			int Crowbar_Skin;
			int Machete_Skin;
			int Pan_Skin;
			int Sickle_Skin;
			int K2_Skin;
			int MK12_Skin;
			int P90_Skin;
			int M1897_Skin;
			int MP9_Skin;
			int OriginS12_Skin;
			int Dragunov_Skin;
			int FamasG2_Skin;
			int M18_Skin;
			int M79_Skin;


			int BackLv1_Skin;
			int BackLv2_Skin;
			int BackLv3_Skin;
			int BackLv1_SetID;
			int BackLv2_SetID;
			int BackLv3_SetID;

			int ArmorLv1_Skin;
			int ArmorLv2_Skin;
			int ArmorLv3_Skin;
			int ArmorLv1_SetID;
			int ArmorLv2_SetID;
			int ArmorLv3_SetID;


			int HelmetLv1_Skin;
			int HelmetLv2_Skin;
			int HelmetLv3_Skin;
			int HelmetLv1_SetID;
			int HelmetLv2_SetID;
			int HelmetLv3_SetID;

			//显示模式 
			int DisplayMode;
			int GrenadeCount;

			int RankServer;
			int BlackListShow;
		}Misc;//自瞄
		struct
		{
			int 战斗模式;
			int 战斗模式HotKey;
			int LockFlame;
			bool Opened;
			int OpenedHotKey;
			int SettingSwitchHotKey;
			int FreshHotKey;
			bool bNeedReFresh;
			int tab;
			int config;
			int language;
			bool firstInit;
			bool PauseEveryThing;
			int FreshTrace;
			int UseCacheToTrace;
			int PhysxTrace;
			int AnnoceOpen;
			int AnnoceVersion;
			int DataInfo;
		} Menu;

		int ConfigVer = 0;
	};

	extern Variables Vars;

	template< typename T >
	class ConfigItem
	{
		hash_t category, name;
		T* value;
	public:
		ConfigItem(hash_t category, hash_t name, T* value)
		{
			this->category = category;
			this->name = name;
			this->value = value;
		}
	};

	template< typename T >
	class ConfigValue
	{
	public:
		ConfigValue(hash_t category_, hash_t name_, T* value_)
		{
			category = category_;
			name = name_;
			value = value_;
		}

		hash_t category, name;
		T* value;
	};

	class CConfig
	{
	protected:
		std::vector<ConfigValue<int>*>ints;
		std::vector<ConfigValue<bool>*>bools;
		std::vector<ConfigValue<float>*>floats;
		std::vector<ConfigValue<float>*>colors;
		std::vector<ConfigValue<char*>*>texts;
		std::vector<ConfigValue<BYTE>*>bytes;
	private:
		void SetupValue(int&, int, hash_t, hash_t);
		void SetupValue(bool&, bool, const hash_t, const hash_t);
		void SetupValue(float&, float, hash_t, hash_t);
		void SetupValue(float*, std::vector<float>, hash_t, hash_t);
		void SetupValue(char*&, void*, hash_t, hash_t);
		void SetupValue(BYTE*, std::vector<BYTE>, hash_t, hash_t);

	public:
		CConfig()
		{
			Setup();
		}

		void Setup();

		void Save(std::string path);
		void Save2(std::string path);
		void Load(std::string path);
		void SaveThread(std::string path);
		bool ThreadSave(std::string path);
	};

	extern CConfig* Config;
}