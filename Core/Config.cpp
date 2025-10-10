#include "Include/WinHeaders.hpp"
#include "Config.h"
#include "Core/DirectX/Drawing.h"
#include "Helper/Misc/Utils.hpp"
#define ConfigVersion 4
extern std::string hudPath1;
extern std::string hudPath2;
namespace IronMan::Core
{
#define SETUPVALUE( s )  HASH(s)

	Variables Vars = {};
	CConfig* Config = nullptr;

	HANDLE ThreadSaveHandle = nullptr;

	struct ThreadParams {
		CConfig* pConfig;
		std::string path;
	};

	void CConfig::Setup()
	{
		SetupValue(Vars.espHuman.Enable, true, SETUPVALUE("espHuman"), SETUPVALUE("Enable"));
		SetupValue(Vars.espHuman.Skeleton, true, SETUPVALUE("espHuman"), SETUPVALUE("Skeleton"));
		SetupValue(Vars.espHuman.HeadCircle, true, SETUPVALUE("espHuman"), SETUPVALUE("HeadCircle"));
		SetupValue(Vars.espHuman.SkeletonVis, std::vector<float>{1.f, 1.f, 0.f, 1.000000f}, SETUPVALUE("espBox"), SETUPVALUE("SkeletonVis"));
		SetupValue(Vars.espHuman.SkeletonInVis, std::vector<float>{0.972549f, 0.972549f, 1.000000f, 1.000000f}, SETUPVALUE("espBox"), SETUPVALUE("SkeletonInVis"));
		SetupValue(Vars.espHuman.Box, false, SETUPVALUE("espHuman"), SETUPVALUE("Box"));
		SetupValue(Vars.espHuman.BoxVis, std::vector<float>{0.02f, 1.f, 0.8f, 1.000000f}, SETUPVALUE("espBox"), SETUPVALUE("BoxVis"));
		SetupValue(Vars.espHuman.AllLockColor, std::vector<float>{1.f, 0.f, 0.f, 1.000000f}, SETUPVALUE("espBox"), SETUPVALUE("AllLockColor"));
		SetupValue(Vars.espHuman.PlayerName, true, SETUPVALUE("espHuman"), SETUPVALUE("PlayerName"));
		SetupValue(Vars.espHuman.PlayerNameColor, std::vector<float>{0.87f, 1.f, 0.f, 1.f}, SETUPVALUE("espBox"), SETUPVALUE("PlayerNameColor"));
		SetupValue(Vars.espHuman.PlayerWeapon, true, SETUPVALUE("espHuman"), SETUPVALUE("PlayerWeapon"));
		SetupValue(Vars.espHuman.PlayerWeaponColor, std::vector<float>{1.f, 1.f, 1.f, 1.f}, SETUPVALUE("espHuman"), SETUPVALUE("PlayerWeaponColor"));
		SetupValue(Vars.espHuman.HealthBar, true, SETUPVALUE("espHuman"), SETUPVALUE("HealthBar"));
		SetupValue(Vars.espHuman.HealthBarColor, std::vector<float>{1.f, 1.f, 1.f, 1.f}, SETUPVALUE("espBox"), SETUPVALUE("HealthBarColor"));
		SetupValue(Vars.espHuman.HealthBarOutLineColor, std::vector<float>{0.f, 1.f, 0.f, 0.4f}, SETUPVALUE("espBox"), SETUPVALUE("HealthBarOutLineColor"));
		SetupValue(Vars.espHuman.Distance, true, SETUPVALUE("espHuman"), SETUPVALUE("Distance"));
		SetupValue(Vars.espHuman.DistanceColor, std::vector<float>{1.f, 0.96f, 0.f, 1.f}, SETUPVALUE("espBox"), SETUPVALUE("DistanceColor"));
		SetupValue(Vars.espHuman.PlayerTeam, true, SETUPVALUE("espHuman"), SETUPVALUE("PlayerTeam"));
		SetupValue(Vars.espHuman.HeadCircle, true, SETUPVALUE("espHuman"), SETUPVALUE("HeadCircle"));
		SetupValue(Vars.espHuman.PlayerSight, true, SETUPVALUE("espHuman"), SETUPVALUE("PlayerSight"));
		SetupValue(Vars.espHuman.PlayerSightColor, std::vector<float>{1.f, 0.f, 0.f, 1.f}, SETUPVALUE("espHuman"), SETUPVALUE("PlayerSightColor"));
		SetupValue(Vars.espHuman.KillCount, true, SETUPVALUE("espHuman"), SETUPVALUE("KillCount"));
		SetupValue(Vars.espHuman.Observer, true, SETUPVALUE("espHuman"), SETUPVALUE("Observer"));
		SetupValue(Vars.espHuman.Spectator, true, SETUPVALUE("espHuman"), SETUPVALUE("Spectator"));
		SetupValue(Vars.espHuman.SuvivalLevel, false, SETUPVALUE("espHuman"), SETUPVALUE("SuvivalLevel"));
		SetupValue(Vars.espHuman.SuvivalLevelColor, std::vector<float>{0.f, 0.67f, 1.f, 1.f}, SETUPVALUE("espHuman"), SETUPVALUE("SuvivalLevelColor"));
		SetupValue(Vars.espHuman.Collaborator, true, SETUPVALUE("espHuman"), SETUPVALUE("Collaborator"));
		SetupValue(Vars.espHuman.CollaboratorColor, std::vector<float>{ 1.f, 1.f, 0.f, 1.f}, SETUPVALUE("Misc"), SETUPVALUE("CollaboratorColor1"));
		SetupValue(Vars.espHuman.SimpleInfoSlider, 300.f, SETUPVALUE("espHuman"), SETUPVALUE("SimpleInfoSlider"));
		SetupValue(Vars.espHuman.DistanceSlider, 600.f, SETUPVALUE("espHuman"), SETUPVALUE("DistanceSlider"));
		SetupValue(Vars.espHuman.Thinkness, 1.5f, SETUPVALUE("espHuman"), SETUPVALUE("Thinkness"));
		SetupValue(Vars.espHuman.TextSize, 16.f, SETUPVALUE("espHuman"), SETUPVALUE("TextSize"));

		SetupValue(Vars.espItem.EnableHotKey, VK_F1, SETUPVALUE("espItem"), SETUPVALUE("EnableHotKey"));
		SetupValue(Vars.espItem.SmartItem, true, SETUPVALUE("espItem"), SETUPVALUE("SmartItem"));
		SetupValue(Vars.espItem.SmartItemHotKey, VK_END, SETUPVALUE("espItem"), SETUPVALUE("SmartItemHotKey"));
		SetupValue(Vars.espItem.Item, true, SETUPVALUE("espItem"), SETUPVALUE("Item"));
		SetupValue(Vars.espItem.ItemPictures, true, SETUPVALUE("espItem"), SETUPVALUE("ItemPictures"));
		SetupValue(Vars.espItem.ItemText, false, SETUPVALUE("espItem"), SETUPVALUE("ItemText"));
		SetupValue(Vars.espItem.TextSize, 14.f, SETUPVALUE("espItem"), SETUPVALUE("TextSize"));
		SetupValue(Vars.espItem.ImageSize, 45.f, SETUPVALUE("espItem"), SETUPVALUE("ImageSize"));
		SetupValue(Vars.espItem.ItemDistance, 50.f, SETUPVALUE("espItem"), SETUPVALUE("ItemDistance"));
		SetupValue(Vars.espItem.ALL_Ammo, true, SETUPVALUE("espItem"), SETUPVALUE("ALL_Ammo"));
		SetupValue(Vars.espItem.ALL_Grenade, true, SETUPVALUE("espItem"), SETUPVALUE("ALL_Grenade"));
		SetupValue(Vars.espItem.ALL_Weapon, true, SETUPVALUE("espItem"), SETUPVALUE("ALL_Weapon"));
		SetupValue(Vars.espItem.ALL_Armor, true, SETUPVALUE("espItem"), SETUPVALUE("ALL_Armor"));
		SetupValue(Vars.espItem.ALL_Medicine, true, SETUPVALUE("espItem"), SETUPVALUE("ALL_Medicine"));
		SetupValue(Vars.espItem.ALL_Attachment, true, SETUPVALUE("espItem"), SETUPVALUE("ALL_Attachment"));
		SetupValue(Vars.espItem.ARColor, std::vector<float>{0.99f, 0.5f, 0.f, 1.f}, SETUPVALUE("espHuman"), SETUPVALUE("ARColor"));
		SetupValue(Vars.espItem.AKM, true, SETUPVALUE("espItem"), SETUPVALUE("AKM"));
		SetupValue(Vars.espItem.Groza, true, SETUPVALUE("espItem"), SETUPVALUE("Groza"));
		SetupValue(Vars.espItem.ACE32, true, SETUPVALUE("espItem"), SETUPVALUE("ACE32"));
		SetupValue(Vars.espItem.M762, true, SETUPVALUE("espItem"), SETUPVALUE("M762"));
		SetupValue(Vars.espItem.MK47, true, SETUPVALUE("espItem"), SETUPVALUE("MK47"));
		SetupValue(Vars.espItem.K2, true, SETUPVALUE("espItem"), SETUPVALUE("K2"));
		SetupValue(Vars.espItem.QBZ, true, SETUPVALUE("espItem"), SETUPVALUE("QBZ"));
		SetupValue(Vars.espItem.AUG, true, SETUPVALUE("espItem"), SETUPVALUE("AUG"));
		SetupValue(Vars.espItem.G36C, true, SETUPVALUE("espItem"), SETUPVALUE("G36C"));
		SetupValue(Vars.espItem.M416, true, SETUPVALUE("espItem"), SETUPVALUE("M416"));
		SetupValue(Vars.espItem.M16A4, true, SETUPVALUE("espItem"), SETUPVALUE("M16A4"));
		SetupValue(Vars.espItem.SCARL, true, SETUPVALUE("espItem"), SETUPVALUE("SCARL"));
		SetupValue(Vars.espItem.FAMAS, true, SETUPVALUE("espItem"), SETUPVALUE("FAMAS"));
		SetupValue(Vars.espItem.DMRColor, std::vector<float>{ 0.9f, 0.066f, 0.078f, 1.f }, SETUPVALUE("espHuman"), SETUPVALUE("DMRColor"));
		SetupValue(Vars.espItem.VSS, false, SETUPVALUE("espItem"), SETUPVALUE("VSS"));
		SetupValue(Vars.espItem.QBU, true, SETUPVALUE("espItem"), SETUPVALUE("QBU"));
		SetupValue(Vars.espItem.SLR, true, SETUPVALUE("espItem"), SETUPVALUE("SLR"));
		SetupValue(Vars.espItem.SKS, true, SETUPVALUE("espItem"), SETUPVALUE("SKS"));
		SetupValue(Vars.espItem.Mk12, true, SETUPVALUE("espItem"), SETUPVALUE("Mk12"));
		SetupValue(Vars.espItem.Mk14, true, SETUPVALUE("espItem"), SETUPVALUE("Mk14"));
		SetupValue(Vars.espItem.Mini14, true, SETUPVALUE("espItem"), SETUPVALUE("Mini14"));
		SetupValue(Vars.espItem.Dragunov, true, SETUPVALUE("espItem"), SETUPVALUE("Dragunov"));
		SetupValue(Vars.espItem.SRColor, std::vector<float>{  0.9f, 0.066f, 0.078f, 1.f  }, SETUPVALUE("espHuman"), SETUPVALUE("SRColor"));
		SetupValue(Vars.espItem.M24, true, SETUPVALUE("espItem"), SETUPVALUE("M24"));
		SetupValue(Vars.espItem.AWM, true, SETUPVALUE("espItem"), SETUPVALUE("AWM"));
		SetupValue(Vars.espItem.Win94, false, SETUPVALUE("espItem"), SETUPVALUE("Win94"));
		SetupValue(Vars.espItem.Kar98k, true, SETUPVALUE("espItem"), SETUPVALUE("Kar98k"));
		SetupValue(Vars.espItem.LynxAMR, true, SETUPVALUE("espItem"), SETUPVALUE("LynxAMR"));
		SetupValue(Vars.espItem.MosinNagant, true, SETUPVALUE("espItem"), SETUPVALUE("MosinNagant"));
		SetupValue(Vars.espItem.ThrowableColor, std::vector<float>{ 0.658f, 0.694f, 0.854f, 1.f }, SETUPVALUE("espHuman"), SETUPVALUE("ThrowableColor"));
		SetupValue(Vars.espItem.C4, true, SETUPVALUE("espItem"), SETUPVALUE("C4"));
		SetupValue(Vars.espItem.SmokeBoom, true, SETUPVALUE("espItem"), SETUPVALUE("SmokeBoom"));
		SetupValue(Vars.espItem.FlashBang, true, SETUPVALUE("espItem"), SETUPVALUE("FlashBang"));
		SetupValue(Vars.espItem.Molotov, true, SETUPVALUE("espItem"), SETUPVALUE("Molotov"));
		SetupValue(Vars.espItem.FragGrenade, true, SETUPVALUE("espItem"), SETUPVALUE("FragGrenade"));
		SetupValue(Vars.espItem.BlueZoneGrenade, true, SETUPVALUE("espItem"), SETUPVALUE("BlueZoneGrenade"));
		SetupValue(Vars.espItem.ShotGunColor, std::vector<float>{ 0.78f, 0.588f, 0.039f, 1 }, SETUPVALUE("espHuman"), SETUPVALUE("ShotGunColor"));
		SetupValue(Vars.espItem.O12, true, SETUPVALUE("espItem"), SETUPVALUE("O12"));
		SetupValue(Vars.espItem.DBS, true, SETUPVALUE("espItem"), SETUPVALUE("DBS"));
		SetupValue(Vars.espItem.S12K, true, SETUPVALUE("espItem"), SETUPVALUE("S12K"));
		SetupValue(Vars.espItem.S686, true, SETUPVALUE("espItem"), SETUPVALUE("S686"));
		SetupValue(Vars.espItem.S1897, true, SETUPVALUE("espItem"), SETUPVALUE("S1897"));
		SetupValue(Vars.espItem.SMGColor, std::vector<float>{ 0.78f, 0.588f, 0.039f, 1 }, SETUPVALUE("espHuman"), SETUPVALUE("SMGColor"));
		SetupValue(Vars.espItem.P90, true, SETUPVALUE("espItem"), SETUPVALUE("P90"));
		SetupValue(Vars.espItem.MP9, true, SETUPVALUE("espItem"), SETUPVALUE("MP9"));
		SetupValue(Vars.espItem.MP5K, false, SETUPVALUE("espItem"), SETUPVALUE("MP5K"));
		SetupValue(Vars.espItem.Vector, true, SETUPVALUE("espItem"), SETUPVALUE("Vector"));
		SetupValue(Vars.espItem.UMP45, true, SETUPVALUE("espItem"), SETUPVALUE("UMP45"));
		SetupValue(Vars.espItem.MicroUZI, false, SETUPVALUE("espItem"), SETUPVALUE("MicroUZI"));
		SetupValue(Vars.espItem.PP19Bizon, false, SETUPVALUE("espItem"), SETUPVALUE("PP19Bizon"));
		SetupValue(Vars.espItem.TommyGun, false, SETUPVALUE("espItem"), SETUPVALUE("TommyGun"));
		SetupValue(Vars.espItem.JS9, false, SETUPVALUE("espItem"), SETUPVALUE("JS9"));
		SetupValue(Vars.espItem.HandGunColor, std::vector<float>{ 0.78f, 0.588f, 0.039f, 1}, SETUPVALUE("espHuman"), SETUPVALUE("HandGunColor"));
		SetupValue(Vars.espItem.R45, false, SETUPVALUE("espItem"), SETUPVALUE("R45"));
		SetupValue(Vars.espItem.P92, false, SETUPVALUE("espItem"), SETUPVALUE("P92"));
		SetupValue(Vars.espItem.P18C, false, SETUPVALUE("espItem"), SETUPVALUE("P18C"));
		SetupValue(Vars.espItem.P1911, false, SETUPVALUE("espItem"), SETUPVALUE("P1911"));
		SetupValue(Vars.espItem.R1895, false, SETUPVALUE("espItem"), SETUPVALUE("R1895"));
		SetupValue(Vars.espItem.Skoripion, false, SETUPVALUE("espItem"), SETUPVALUE("Skoripion"));
		SetupValue(Vars.espItem.SawedOff, false, SETUPVALUE("espItem"), SETUPVALUE("SawedOff"));
		SetupValue(Vars.espItem.Deagle, false, SETUPVALUE("espItem"), SETUPVALUE("Deagle"));
		SetupValue(Vars.espItem.StunGun, true, SETUPVALUE("espItem"), SETUPVALUE("StunGun"));
		SetupValue(Vars.espItem.MelleColor, std::vector<float>{ 0.172f, 0.588f, 0.784f, 1}, SETUPVALUE("espHuman"), SETUPVALUE("MelleColor"));
		SetupValue(Vars.espItem.Crowbar, true, SETUPVALUE("espItem"), SETUPVALUE("Crowbar"));
		SetupValue(Vars.espItem.Sickle, true, SETUPVALUE("espItem"), SETUPVALUE("Sickle"));
		SetupValue(Vars.espItem.Machete, true, SETUPVALUE("espItem"), SETUPVALUE("Machete"));
		SetupValue(Vars.espItem.Pan, true, SETUPVALUE("espItem"), SETUPVALUE("Pan"));
		SetupValue(Vars.espItem.JerryCan, true, SETUPVALUE("espItem"), SETUPVALUE("JerryCan"));
		SetupValue(Vars.espItem.PickAex, true, SETUPVALUE("espItem"), SETUPVALUE("PickAex"));
		SetupValue(Vars.espItem.MountainBike, true, SETUPVALUE("espItem"), SETUPVALUE("MountainBike"));
		SetupValue(Vars.espItem.yingjichulizhuangbei, true, SETUPVALUE("espItem"), SETUPVALUE("yingjichulizhuangbei"));
		SetupValue(Vars.espItem.guanchejing, true, SETUPVALUE("espItem"), SETUPVALUE("guanchejing"));
		SetupValue(Vars.espItem.duoheyi, true, SETUPVALUE("espItem"), SETUPVALUE("duoheyi"));
		SetupValue(Vars.espItem.wurenji, true, SETUPVALUE("espItem"), SETUPVALUE("wurenji"));
		SetupValue(Vars.espItem.zhanshubeibao, true, SETUPVALUE("espItem"), SETUPVALUE("zhanshubeibao"));
		SetupValue(Vars.espItem.NeonCoin, true, SETUPVALUE("espItem"), SETUPVALUE("NeonCoin"));
		SetupValue(Vars.espItem.CoverStructDropHandFlare, true, SETUPVALUE("espItem"), SETUPVALUE("CoverStructDropHandFlare"));
		SetupValue(Vars.espItem.LMGColor, std::vector<float>{  0.78f, 0.588f, 0.039f, 1 }, SETUPVALUE("espHuman"), SETUPVALUE("LMGColor"));
		SetupValue(Vars.espItem.MG3, true, SETUPVALUE("espItem"), SETUPVALUE("MG3"));
		SetupValue(Vars.espItem.M249, true, SETUPVALUE("espItem"), SETUPVALUE("M249"));
		SetupValue(Vars.espItem.DP28, true, SETUPVALUE("espItem"), SETUPVALUE("DP28"));
		SetupValue(Vars.espItem.ETCColor, std::vector<float>{  0.f, 0.572f, 0.513f, 1.f  }, SETUPVALUE("espHuman"), SETUPVALUE("ETCColor"));
		SetupValue(Vars.espItem.Crossbow, true, SETUPVALUE("espItem"), SETUPVALUE("Crossbow"));
		SetupValue(Vars.espItem.FlareGun, true, SETUPVALUE("espItem"), SETUPVALUE("FlareGun"));
		SetupValue(Vars.espItem.M79, true, SETUPVALUE("espItem"), SETUPVALUE("M79"));
		SetupValue(Vars.espItem.TransmitterColor, std::vector<float>{  0.294f, 0.596f, 0.698f, 1.f  }, SETUPVALUE("espHuman"), SETUPVALUE("TransmitterColor"));
		SetupValue(Vars.espItem.Mortar, false, SETUPVALUE("espItem"), SETUPVALUE("Mortar"));
		SetupValue(Vars.espItem.Panzerfaust, true, SETUPVALUE("espItem"), SETUPVALUE("Panzerfaust"));
		SetupValue(Vars.espItem.ArmorLv2Color, std::vector<float>{  0.035f, 0.556f, 0.878f, 1  }, SETUPVALUE("espHuman"), SETUPVALUE("ArmorLv2Color"));
		SetupValue(Vars.espItem.HelmetLv2, true, SETUPVALUE("espItem"), SETUPVALUE("HelmetLv2"));
		SetupValue(Vars.espItem.VestLv2, true, SETUPVALUE("espItem"), SETUPVALUE("VestLv2"));
		SetupValue(Vars.espItem.BackpackLv2, true, SETUPVALUE("espItem"), SETUPVALUE("BackpackLv2"));
		SetupValue(Vars.espItem.InterferenceBackpack, true, SETUPVALUE("espItem"), SETUPVALUE("InterferenceBackpack"));
		SetupValue(Vars.espItem.EmergencyPickup, true, SETUPVALUE("espItem"), SETUPVALUE("EmergencyPickup"));
		SetupValue(Vars.espItem.BulletproofShield, false, SETUPVALUE("espItem"), SETUPVALUE("BulletproofShield"));
		SetupValue(Vars.espItem.ArmorLv3Color, std::vector<float>{  1.f, 1.f, 0.f, 1.f  }, SETUPVALUE("espHuman"), SETUPVALUE("ArmorLv3Color"));
		SetupValue(Vars.espItem.HelmetLv3, true, SETUPVALUE("espItem"), SETUPVALUE("HelmetLv3"));
		SetupValue(Vars.espItem.VestLv3, true, SETUPVALUE("espItem"), SETUPVALUE("VestLv3"));
		SetupValue(Vars.espItem.BackpackLv3, true, SETUPVALUE("espItem"), SETUPVALUE("BackpackLv3"));
		SetupValue(Vars.espItem.SightColor, std::vector<float>{  0.9f, 0.231f, 0.494f, 1.f }, SETUPVALUE("espHuman"), SETUPVALUE("SightColor"));
		SetupValue(Vars.espItem.CantedSight, true, SETUPVALUE("espItem"), SETUPVALUE("CantedSight"));
		SetupValue(Vars.espItem.Scope6x, true, SETUPVALUE("espItem"), SETUPVALUE("Scope6x"));
		SetupValue(Vars.espItem.RedDotSight, true, SETUPVALUE("espItem"), SETUPVALUE("RedDotSight"));
		SetupValue(Vars.espItem.Scope8x, true, SETUPVALUE("espItem"), SETUPVALUE("Scope8x"));
		SetupValue(Vars.espItem.HolographicSight, true, SETUPVALUE("espItem"), SETUPVALUE("HolographicSight"));
		SetupValue(Vars.espItem.Scope15x, true, SETUPVALUE("espItem"), SETUPVALUE("Scope15x"));
		SetupValue(Vars.espItem.Scope2x, true, SETUPVALUE("espItem"), SETUPVALUE("Scope2x"));
		SetupValue(Vars.espItem.Thermal4xScope, true, SETUPVALUE("espItem"), SETUPVALUE("Thermal4xScope"));
		SetupValue(Vars.espItem.Scope3x, true, SETUPVALUE("espItem"), SETUPVALUE("Scope3x"));
		SetupValue(Vars.espItem.Scope4x, true, SETUPVALUE("espItem"), SETUPVALUE("Scope4x"));
		SetupValue(Vars.espItem.Scope4xHot, true, SETUPVALUE("espItem"), SETUPVALUE("Scope4xHot"));
		SetupValue(Vars.espItem.MagazineColor, std::vector<float>{  0.9f, 0.815f, 0.f, 1  }, SETUPVALUE("espHuman"), SETUPVALUE("MagazineColor"));
		SetupValue(Vars.espItem.ExtQuickDrawMagSMG, true, SETUPVALUE("espItem"), SETUPVALUE("ExtQuickDrawMagSMG"));
		SetupValue(Vars.espItem.ExtQuickDrawMagARDMR, true, SETUPVALUE("espItem"), SETUPVALUE("ExtQuickDrawMagARDMR"));
		SetupValue(Vars.espItem.ExtQuickDrawMagDMRSR, true, SETUPVALUE("espItem"), SETUPVALUE("ExtQuickDrawMagDMRSR"));
		SetupValue(Vars.espItem.ExtendedMagSMG, true, SETUPVALUE("espItem"), SETUPVALUE("ExtendedMagSMG"));
		SetupValue(Vars.espItem.ExtendedMagARDMR, true, SETUPVALUE("espItem"), SETUPVALUE("ExtendedMagARDMR"));
		SetupValue(Vars.espItem.ExtendedMagDMRSR, true, SETUPVALUE("espItem"), SETUPVALUE("ExtendedMagDMRSR"));
		SetupValue(Vars.espItem.AmmoColor, std::vector<float>{  0.784f, 0.f, 0.462f, 1  }, SETUPVALUE("espHuman"), SETUPVALUE("AmmoColor"));
		SetupValue(Vars.espItem.ACP45, false, SETUPVALUE("espItem"), SETUPVALUE("ACP45"));
		SetupValue(Vars.espItem._556mm, true, SETUPVALUE("espItem"), SETUPVALUE("_556mm"));
		SetupValue(Vars.espItem._762mm, true, SETUPVALUE("espItem"), SETUPVALUE("_762mm"));
		SetupValue(Vars.espItem._57mm, true, SETUPVALUE("espItem"), SETUPVALUE("_57mm"));
		SetupValue(Vars.espItem._9mm, false, SETUPVALUE("espItem"), SETUPVALUE("_9mm"));
		SetupValue(Vars.espItem._40mm, false, SETUPVALUE("espItem"), SETUPVALUE("_40mm"));
		SetupValue(Vars.espItem._60mm, false, SETUPVALUE("espItem"), SETUPVALUE("_60mm"));
		SetupValue(Vars.espItem._12Gauge, false, SETUPVALUE("espItem"), SETUPVALUE("_12Gauge"));
		SetupValue(Vars.espItem._12GaugeSlug, false, SETUPVALUE("espItem"), SETUPVALUE("_12GaugeSlug"));
		SetupValue(Vars.espItem._300Magnum, false, SETUPVALUE("espItem"), SETUPVALUE("_300Magnum"));
		SetupValue(Vars.espItem.FlareAmmo, false, SETUPVALUE("espItem"), SETUPVALUE("FlareAmmo"));
		SetupValue(Vars.espItem.MuzzleColor, std::vector<float>{ 0.274f, 0.654f, 0.66f, 1}, SETUPVALUE("espHuman"), SETUPVALUE("MuzzleColor"));
		SetupValue(Vars.espItem.ChockSG, true, SETUPVALUE("espItem"), SETUPVALUE("ChockSG"));
		SetupValue(Vars.espItem.SuppressorSMG, true, SETUPVALUE("espItem"), SETUPVALUE("SuppressorSMG"));
		SetupValue(Vars.espItem.DuckbillSG, true, SETUPVALUE("espItem"), SETUPVALUE("DuckbillSG"));
		SetupValue(Vars.espItem.SuppressorARDMR, true, SETUPVALUE("espItem"), SETUPVALUE("SuppressorARDMR"));
		SetupValue(Vars.espItem.FlashHiderSMG, true, SETUPVALUE("espItem"), SETUPVALUE("FlashHiderSMG"));
		SetupValue(Vars.espItem.SuppressorDMRSR, true, SETUPVALUE("espItem"), SETUPVALUE("SuppressorDMRSR"));
		SetupValue(Vars.espItem.FlashHiderARDMR, true, SETUPVALUE("espItem"), SETUPVALUE("FlashHiderARDMR"));
		SetupValue(Vars.espItem.FlashHiderDMRSR, true, SETUPVALUE("espItem"), SETUPVALUE("FlashHiderDMRSR"));
		SetupValue(Vars.espItem.CompensatorSMG, true, SETUPVALUE("espItem"), SETUPVALUE("CompensatorSMG"));
		SetupValue(Vars.espItem.CompensatorARDMR, true, SETUPVALUE("espItem"), SETUPVALUE("CompensatorARDMR"));
		SetupValue(Vars.espItem.CompensatorDMRSR, true, SETUPVALUE("espItem"), SETUPVALUE("CompensatorDMRSR"));
		SetupValue(Vars.espItem.MuzzleBrake, true, SETUPVALUE("espItem"), SETUPVALUE("MuzzleBrake"));
		SetupValue(Vars.espItem.GripColor, std::vector<float>{ 0.9f, 0.815f, 0.f, 1}, SETUPVALUE("espHuman"), SETUPVALUE("GripColor"));
		SetupValue(Vars.espItem.LightweightGrip, true, SETUPVALUE("espItem"), SETUPVALUE("LightweightGrip"));
		SetupValue(Vars.espItem.Thumbgrip, true, SETUPVALUE("espItem"), SETUPVALUE("Thumbgrip"));
		SetupValue(Vars.espItem.VerticalForegrip, true, SETUPVALUE("espItem"), SETUPVALUE("VerticalForegrip"));
		SetupValue(Vars.espItem.AngledForegrip, true, SETUPVALUE("espItem"), SETUPVALUE("AngledForegrip"));
		SetupValue(Vars.espItem.Halfgrip, true, SETUPVALUE("espItem"), SETUPVALUE("Halfgrip"));
		SetupValue(Vars.espItem.LaserSight, true, SETUPVALUE("espItem"), SETUPVALUE("LaserSight"));
		SetupValue(Vars.espItem.ButtstockColor, std::vector<float>{ 0.776f, 0.592f, 0.05f, 1}, SETUPVALUE("espHuman"), SETUPVALUE("ButtstockColor"));
		SetupValue(Vars.espItem.CheekPad, true, SETUPVALUE("espItem"), SETUPVALUE("CheekPad"));
		SetupValue(Vars.espItem.TacticalStock, true, SETUPVALUE("espItem"), SETUPVALUE("TacticalStock"));
		SetupValue(Vars.espItem.FoldingStock, true, SETUPVALUE("espItem"), SETUPVALUE("FoldingStock"));
		SetupValue(Vars.espItem.BulletLoops, true, SETUPVALUE("espItem"), SETUPVALUE("BulletLoops"));
		SetupValue(Vars.espItem.HeaveStock, true, SETUPVALUE("espItem"), SETUPVALUE("HeaveStock"));
		SetupValue(Vars.espItem.MedicineColor, std::vector<float>{ 0.776f, 0.592f, 0.05f, 1}, SETUPVALUE("espHuman"), SETUPVALUE("MedicineColor"));
		SetupValue(Vars.espItem.MedKit, true, SETUPVALUE("espItem"), SETUPVALUE("MedKit"));
		SetupValue(Vars.espItem.FirstAidKit, true, SETUPVALUE("espItem"), SETUPVALUE("FirstAidKit"));
		SetupValue(Vars.espItem.Bandage, true, SETUPVALUE("espItem"), SETUPVALUE("Bandage"));
		SetupValue(Vars.espItem.PainKiller, true, SETUPVALUE("espItem"), SETUPVALUE("PainKiller"));
		SetupValue(Vars.espItem.EnergyDrink, true, SETUPVALUE("espItem"), SETUPVALUE("EnergyDrink"));
		SetupValue(Vars.espItem.AdrenalineSyringe, true, SETUPVALUE("espItem"), SETUPVALUE("AdrenalineSyringe"));
		SetupValue(Vars.espItem.UseableColor, std::vector<float>{ 0.476f, 0.492f, 0.25f, 1}, SETUPVALUE("espHuman"), SETUPVALUE("UseableColor1"));
		SetupValue(Vars.espItem.SelfRevive, true, SETUPVALUE("espItem"), SETUPVALUE("SelfRevive"));
		SetupValue(Vars.espItem.BattleReadyKit, true, SETUPVALUE("espItem"), SETUPVALUE("BattleReadyKit"));
		SetupValue(Vars.espItem.ChamberKeys, true, SETUPVALUE("espItem"), SETUPVALUE("ChamberKeys"));
		SetupValue(Vars.espItem.Revival_Transmitter, true, SETUPVALUE("espItem"), SETUPVALUE("Revival_Transmitter"));
		SetupValue(Vars.espItem.TacticalColor, std::vector<float>{ 0.75f, 0.75f, 0.75f, 1}, SETUPVALUE("espHuman"), SETUPVALUE("TacticalColor1"));
		SetupValue(Vars.espItem.CraftMode, true, SETUPVALUE("espItem"), SETUPVALUE("CraftMode"));

		SetupValue(Vars.espAirDrop.AirDrop, true, SETUPVALUE("espAirDrop"), SETUPVALUE("AirDrop"));
		SetupValue(Vars.espAirDrop.AirDropColor, std::vector<float>{ 0.619f, 0.819f, 0.56f, 1.f}, SETUPVALUE("espHuman"), SETUPVALUE("AirDropColor"));
		SetupValue(Vars.espAirDrop.Loot, true, SETUPVALUE("espAirDrop"), SETUPVALUE("Loot"));
		SetupValue(Vars.espAirDrop.LootColor, std::vector<float>{ 0.529f, 0.8f, 0.36f, 1.f}, SETUPVALUE("espHuman"), SETUPVALUE("LootColor"));
		SetupValue(Vars.espAirDrop.LootInside, true, SETUPVALUE("espAirDrop"), SETUPVALUE("LootInside"));

		SetupValue(Vars.espVehicle.Enable, true, SETUPVALUE("espVehicle"), SETUPVALUE("Enable"));
		SetupValue(Vars.espVehicle.EnableHotKey, VK_F3, SETUPVALUE("espVehicle"), SETUPVALUE("EnableHotKey"));
		SetupValue(Vars.espVehicle.Vehicle, true, SETUPVALUE("espVehicle"), SETUPVALUE("Vehicle"));
		SetupValue(Vars.espVehicle.VehicleColor, std::vector<float>{ 1, 1, 1, 1}, SETUPVALUE("espHuman"), SETUPVALUE("VehicleColor"));
		SetupValue(Vars.espVehicle.VehicleHealth, true, SETUPVALUE("espVehicle"), SETUPVALUE("VehicleHealth"));
		SetupValue(Vars.espVehicle.VehicleDistance, 1000.f, SETUPVALUE("espVehicle"), SETUPVALUE("VehicleDistance"));

		SetupValue(Vars.espRank.KD, true, SETUPVALUE("espRank"), SETUPVALUE("KD"));
		SetupValue(Vars.espRank.Rank, true, SETUPVALUE("espRank"), SETUPVALUE("Rank"));
		SetupValue(Vars.espRank.RankTexture, true, SETUPVALUE("espRank"), SETUPVALUE("RankTexture"));
		SetupValue(Vars.espRank.Mode, false, SETUPVALUE("espRank"), SETUPVALUE("Mode"));
		SetupValue(Vars.espRank.Team, true, SETUPVALUE("espRank"), SETUPVALUE("Team"));
		SetupValue(Vars.espRank.RankServer, 0, SETUPVALUE("espRank"), SETUPVALUE("RankServer"));

		SetupValue(Vars.Aimbot.HighAimbot, false, SETUPVALUE("Aimbot"), SETUPVALUE("HighAimbot"));
		SetupValue(Vars.Aimbot.AimbotDrag, false, SETUPVALUE("Aimbot"), SETUPVALUE("AimbotDrag"));
		SetupValue(Vars.Aimbot.DownValue, 0.f, SETUPVALUE("Aimbot"), SETUPVALUE("DownValue"));
		SetupValue(Vars.Aimbot.Aimbot, true, SETUPVALUE("Aimbot"), SETUPVALUE("Aimbot"));
		SetupValue(Vars.Aimbot.CheckReloading, true, SETUPVALUE("Aimbot"), SETUPVALUE("CheckReloading"));

		SetupValue(Vars.Aimbot.GrenadePredict, true, SETUPVALUE("Aimbot"), SETUPVALUE("GrenadePredict"));
		SetupValue(Vars.Aimbot.HandNotLock, true, SETUPVALUE("Aimbot"), SETUPVALUE("HandNotLock"));
		SetupValue(Vars.Aimbot.VisibleCheck, true, SETUPVALUE("Aimbot"), SETUPVALUE("VisibleCheck"));
		SetupValue(Vars.Aimbot.AR_Knock, true, SETUPVALUE("Aimbot"), SETUPVALUE("AR_Knock"));
		SetupValue(Vars.Aimbot.SR_Knock, true, SETUPVALUE("Aimbot"), SETUPVALUE("SR_Knock"));
		SetupValue(Vars.Aimbot.DMR_Knock, true, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_Knock"));
		SetupValue(Vars.Aimbot.AR_PredictionPoint, true, SETUPVALUE("Aimbot"), SETUPVALUE("AR_PredictionPoint"));
		SetupValue(Vars.Aimbot.SR_PredictionPoint, true, SETUPVALUE("Aimbot"), SETUPVALUE("SR_PredictionPoint"));
		SetupValue(Vars.Aimbot.DMR_PredictionPoint, true, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_PredictionPoint"));
		SetupValue(Vars.Aimbot.PredictionPointColor, std::vector<float>{ 1, 0, 0, 1}, SETUPVALUE("espHuman"), SETUPVALUE("PredictionPointColor"));
		SetupValue(Vars.Aimbot.Team, true, SETUPVALUE("Aimbot"), SETUPVALUE("Team"));
		SetupValue(Vars.Aimbot.FovShow, true, SETUPVALUE("Aimbot"), SETUPVALUE("FovShow"));
		SetupValue(Vars.Aimbot.FovColor, std::vector<float>{ 1, 0, 0, 1}, SETUPVALUE("espHuman"), SETUPVALUE("FovColor"));
		SetupValue(Vars.Aimbot.Crosshair, true, SETUPVALUE("Aimbot"), SETUPVALUE("Crosshair"));
		SetupValue(Vars.Aimbot.CrosshairColor, std::vector<float>{ 0, 1, 0, 1}, SETUPVALUE("espHuman"), SETUPVALUE("CrosshairColor"));
		SetupValue(Vars.Aimbot.CrosshairSize, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("CrosshairSize"));

		SetupValue(Vars.Aimbot.SingleStep, true, SETUPVALUE("Aimbot"), SETUPVALUE("SingleStep1"));
		SetupValue(Vars.Aimbot.SR_MOVESTEP_X1, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("SR_MOVESTEP_X1"));
		SetupValue(Vars.Aimbot.SR_MOVESTEP_Y1, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("SR_MOVESTEP_Y1"));
		SetupValue(Vars.Aimbot.DMR_MOVESTEP_X1, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_MOVESTEP_X1"));
		SetupValue(Vars.Aimbot.DMR_MOVESTEP_Y1, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_MOVESTEP_Y1"));

		SetupValue(Vars.Aimbot.SR_MOVESTEP_X2, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("SR_MOVESTEP_X2"));
		SetupValue(Vars.Aimbot.SR_MOVESTEP_Y2, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("SR_MOVESTEP_Y2"));
		SetupValue(Vars.Aimbot.DMR_MOVESTEP_X2, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_MOVESTEP_X2"));
		SetupValue(Vars.Aimbot.DMR_MOVESTEP_Y2, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_MOVESTEP_Y2"));

		SetupValue(Vars.Aimbot.InKnockDownHotKey, 27, SETUPVALUE("Aimbot"), SETUPVALUE("InKnockDownHotKey"));
		SetupValue(Vars.Aimbot.KnockDownHotKey, VK_INSERT, SETUPVALUE("Aimbot"), SETUPVALUE("KnockDownHotKey"));
		SetupValue(Vars.Aimbot.AR_LockHeadHotKey, VK_SHIFT, SETUPVALUE("Aimbot"), SETUPVALUE("AR_LockHeadHotKey"));
		SetupValue(Vars.Aimbot.SR_LockHeadHotKey, VK_SHIFT, SETUPVALUE("Aimbot"), SETUPVALUE("SR_LockHeadHotKey"));
		SetupValue(Vars.Aimbot.DMR_LockHeadHotKey, VK_SHIFT, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_LockHeadHotKey"));
		SetupValue(Vars.Aimbot.AR_HotKey1, VK_RBUTTON, SETUPVALUE("Aimbot"), SETUPVALUE("AR_HotKey1"));
		SetupValue(Vars.Aimbot.AR_HotKey2, 20, SETUPVALUE("Aimbot"), SETUPVALUE("AR_HotKey2"));
		SetupValue(Vars.Aimbot.SR_HotKey1, VK_RBUTTON, SETUPVALUE("Aimbot"), SETUPVALUE("SR_HotKey1"));
		SetupValue(Vars.Aimbot.SR_HotKey2, 20, SETUPVALUE("Aimbot"), SETUPVALUE("SR_HotKey2"));
		SetupValue(Vars.Aimbot.DMR_HotKey1, VK_RBUTTON, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_HotKey1"));
		SetupValue(Vars.Aimbot.DMR_HotKey2, 20, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_HotKey2"));
		SetupValue(Vars.Aimbot.PauseLockKey, 'F', SETUPVALUE("Aimbot"), SETUPVALUE("PauseLockKey"));
		SetupValue(Vars.Aimbot.HotKeyCar, 17, SETUPVALUE("Aimbot"), SETUPVALUE("HotKeyCar"));
		SetupValue(Vars.Aimbot.EnableHotKey, VK_F2, SETUPVALUE("Aimbot"), SETUPVALUE("EnableHotKey"));
		SetupValue(Vars.Aimbot.RiferAutoShot, true, SETUPVALUE("Aimbot"), SETUPVALUE("RiferAutoShot"));
		SetupValue(Vars.Aimbot.ShotGunAutoShot, true, SETUPVALUE("Aimbot"), SETUPVALUE("ShotGunAutoShot"));
		SetupValue(Vars.Aimbot.SRAutoShotDelay1, 800.f, SETUPVALUE("Aimbot"), SETUPVALUE("SRAutoShotDelay1"));
		SetupValue(Vars.Aimbot.SRAutoShotDelay2, 450.f, SETUPVALUE("Aimbot"), SETUPVALUE("SRAutoShotDelay2"));
		SetupValue(Vars.Aimbot.SRGunAutoShot, false, SETUPVALUE("Aimbot"), SETUPVALUE("SRGunAutoShot2"));
		SetupValue(Vars.Aimbot.AutoShot, false, SETUPVALUE("Aimbot"), SETUPVALUE("AutoShot"));
		SetupValue(Vars.Aimbot.AutoShotHotKey, VK_F5, SETUPVALUE("Aimbot"), SETUPVALUE("AutoShotHotKey"));
		SetupValue(Vars.Aimbot.AimWhileScoping, false, SETUPVALUE("Aimbot"), SETUPVALUE("AimWhileScoping"));

		SetupValue(Vars.Aimbot.MenuWeaponType, 0, SETUPVALUE("Aimbot"), SETUPVALUE("MenuWeaponType"));
		SetupValue(Vars.Aimbot.AimMode, 0, SETUPVALUE("Aimbot"), SETUPVALUE("AimMode"));
		SetupValue(Vars.Aimbot.Controller_Current, 0, SETUPVALUE("Aimbot"), SETUPVALUE("Controller_Current"));

		SetupValue(Vars.Aimbot.AR_NoRecoil1, true, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_NoRecoil1"));
		SetupValue(Vars.Aimbot.AR_P_X1, 0.12f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_P_X1"));
		SetupValue(Vars.Aimbot.AR_P_Y1, 0.12f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_P_Y1"));
		SetupValue(Vars.Aimbot.AR_RecoilValue1, 100.f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_RecoilValue1"));
		SetupValue(Vars.Aimbot.AR_SmoothValue1, 25.f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_SmoothValue1"));
		SetupValue(Vars.Aimbot.AR_FovRange1, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_FovRange1"));

		SetupValue(Vars.Aimbot.AR_NoRecoil2, true, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_NoRecoil2"));
		SetupValue(Vars.Aimbot.AR_P_X2, 0.35f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_P_X2"));
		SetupValue(Vars.Aimbot.AR_P_Y2, 0.35f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_P_Y2"));
		SetupValue(Vars.Aimbot.AR_RecoilValue2, 100.f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_RecoilValue2"));
		SetupValue(Vars.Aimbot.AR_SmoothValue2, 35.f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_SmoothValue2"));
		SetupValue(Vars.Aimbot.AR_FovRange2, 5.f, SETUPVALUE("Aimbot"), SETUPVALUE("1AR_FovRange2"));

		SetupValue(Vars.Aimbot.SR_NoRecoil1, false, SETUPVALUE("Aimbot"), SETUPVALUE("1SR_NoRecoil1"));
		SetupValue(Vars.Aimbot.SR_baseSmoothing1, 2.5f, SETUPVALUE("Aimbot"), SETUPVALUE("3SR_baseSmoothing1"));
		SetupValue(Vars.Aimbot.SR_RecoilValue1, 100.f, SETUPVALUE("Aimbot"), SETUPVALUE("1SR_RecoilValue1"));
		SetupValue(Vars.Aimbot.SR_SmoothValue1, 15.f, SETUPVALUE("Aimbot"), SETUPVALUE("1SR_SmoothValue1"));
		SetupValue(Vars.Aimbot.SR_FovRange1, 6.f, SETUPVALUE("Aimbot"), SETUPVALUE("2SR_FovRange1"));

		SetupValue(Vars.Aimbot.SR_NoRecoil2, false, SETUPVALUE("Aimbot"), SETUPVALUE("1SR_NoRecoil2"));
		SetupValue(Vars.Aimbot.SR_baseSmoothing2, 2.2f, SETUPVALUE("Aimbot"), SETUPVALUE("3SR_baseSmoothing2"));
		SetupValue(Vars.Aimbot.SR_RecoilValue2, 100.f, SETUPVALUE("Aimbot"), SETUPVALUE("1SR_RecoilValue2"));
		SetupValue(Vars.Aimbot.SR_SmoothValue2, 25.f, SETUPVALUE("Aimbot"), SETUPVALUE("1SR_SmoothValue2"));
		SetupValue(Vars.Aimbot.SR_FovRange2, 8.f, SETUPVALUE("Aimbot"), SETUPVALUE("2SR_FovRange2"));

		SetupValue(Vars.Aimbot.DMR_NoRecoil1, false, SETUPVALUE("Aimbot"), SETUPVALUE("1DMR_NoRecoil1"));
		SetupValue(Vars.Aimbot.DMR_baseSmoothing1, 3.3f, SETUPVALUE("Aimbot"), SETUPVALUE("3DMR_baseSmoothing1"));
		SetupValue(Vars.Aimbot.DMR_RecoilValue1, 100.f, SETUPVALUE("Aimbot"), SETUPVALUE("1DMR_RecoilValue1"));
		SetupValue(Vars.Aimbot.DMR_SmoothValue1, 15.f, SETUPVALUE("Aimbot"), SETUPVALUE("1DMR_SmoothValue1"));
		SetupValue(Vars.Aimbot.DMR_FovRange1, 6.f, SETUPVALUE("Aimbot"), SETUPVALUE("1DMR_FovRange1"));

		SetupValue(Vars.Aimbot.DMR_NoRecoil2, true, SETUPVALUE("Aimbot"), SETUPVALUE("1DMR_NoRecoil2"));
		SetupValue(Vars.Aimbot.DMR_baseSmoothing2, 2.5f, SETUPVALUE("Aimbot"), SETUPVALUE("3DMR_baseSmoothing2"));
		SetupValue(Vars.Aimbot.DMR_RecoilValue2, 100.f, SETUPVALUE("Aimbot"), SETUPVALUE("1DMR_RecoilValue2"));
		SetupValue(Vars.Aimbot.DMR_SmoothValue2, 35.f, SETUPVALUE("Aimbot"), SETUPVALUE("1DMR_SmoothValue2"));
		SetupValue(Vars.Aimbot.DMR_FovRange2, 8.f, SETUPVALUE("Aimbot"), SETUPVALUE("2DMR_FovRange2"));

		SetupValue(Vars.Aimbot.SR_baseFOV, 10.f, SETUPVALUE("Aimbot"), SETUPVALUE("SR_baseFOV"));
		SetupValue(Vars.Aimbot.SR_MaxSmoothIncrease, 0.20f, SETUPVALUE("Aimbot"), SETUPVALUE("SR_MaxSmoothIncrease"));
		SetupValue(Vars.Aimbot.DMR_baseFOV, 10.f, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_baseFOV"));
		SetupValue(Vars.Aimbot.DMR_MaxSmoothIncrease, 0.20f, SETUPVALUE("Aimbot"), SETUPVALUE("DMR_MaxSmoothIncrease"));

		SetupValue(Vars.Aimbot.I_X, 0.10f, SETUPVALUE("Aimbot"), SETUPVALUE("I_X1"));
		SetupValue(Vars.Aimbot.D_X, 0.10f, SETUPVALUE("Aimbot"), SETUPVALUE("D_X1"));
		SetupValue(Vars.Aimbot.I_Y, 0.10f, SETUPVALUE("Aimbot"), SETUPVALUE("I_Y1"));
		SetupValue(Vars.Aimbot.D_Y, 0.10f, SETUPVALUE("Aimbot"), SETUPVALUE("D_Y1"));

		SetupValue(Vars.Aimbot.MaxDynamicRigidDistance, 500.f, SETUPVALUE("Aimbot"), SETUPVALUE("MaxDynamicRigidDistance"));
		SetupValue(Vars.Aimbot.MaxUpdateRigid, 1000, SETUPVALUE("Aimbot"), SETUPVALUE("MaxUpdateRigid"));
		SetupValue(Vars.Aimbot.DebugMode, false, SETUPVALUE("Aimbot"), SETUPVALUE("DebugMode"));
		SetupValue(Vars.Aimbot.RayTracing, false, SETUPVALUE("Aimbot"), SETUPVALUE("RayTracing"));
		SetupValue(Vars.Aimbot.TriangleMesh, false, SETUPVALUE("Aimbot"), SETUPVALUE("TriangleMesh"));
		SetupValue(Vars.Aimbot.StaticMesh, false, SETUPVALUE("Aimbot"), SETUPVALUE("StaticMesh"));
		SetupValue(Vars.Aimbot.DynamicMesh, false, SETUPVALUE("Aimbot"), SETUPVALUE("DynamicMesh"));

		char Path[] = "C:\\PaodCache\\";
		Vars.Aimbot.txt_CacheSave = (char*)malloc(0x30);
		memset(Vars.Aimbot.txt_CacheSave, 0, 0x20);
		SetupValue(Vars.Aimbot.txt_CacheSave, Path, SETUPVALUE("Aimbot"), SETUPVALUE("txt_CacheSave"));


		char Empty[] = "";
		char BPPort[] = "115200";
		char LKPort[] = "57600";
		char JSPort[] = "460800";
		char LocalHost[] = "http://127.0.0.1/";

		Vars.Aimbot.txt_IP = (char*)malloc(0x20);
		Vars.Aimbot.txt_Port = (char*)malloc(0x20);
		Vars.Aimbot.txt_UUID = (char*)malloc(0x20);
		Vars.Aimbot.txt_BPPort = (char*)malloc(0x20);
		Vars.Aimbot.txt_LKPort = (char*)malloc(0x20);
		Vars.Aimbot.txt_JSPort = (char*)malloc(0x20);
		Vars.Misc.txt_RadarIP = (char*)malloc(0x30);

		memset(Vars.Aimbot.txt_IP, 0, 0x20);
		memset(Vars.Aimbot.txt_Port, 0, 0x20);
		memset(Vars.Aimbot.txt_UUID, 0, 0x20);
		memset(Vars.Aimbot.txt_BPPort, 0, 0x20);
		memset(Vars.Aimbot.txt_LKPort, 0, 0x20);
		memset(Vars.Aimbot.txt_JSPort, 0, 0x20);
		memset(Vars.Misc.txt_RadarIP, 0, 0x20);

		SetupValue(Vars.Aimbot.txt_IP, (void*)(&Empty), SETUPVALUE("Aimbot"), SETUPVALUE("txt_IP"));
		SetupValue(Vars.Aimbot.txt_Port, (void*)(&Empty), SETUPVALUE("Aimbot"), SETUPVALUE("txt_Port"));
		SetupValue(Vars.Aimbot.txt_UUID, (void*)(&Empty), SETUPVALUE("Aimbot"), SETUPVALUE("txt_UUID"));
		SetupValue(Vars.Aimbot.txt_BPPort, (void*)(&BPPort), SETUPVALUE("Aimbot"), SETUPVALUE("txt_BPPort"));
		SetupValue(Vars.Aimbot.txt_LKPort, (void*)(&LKPort), SETUPVALUE("Aimbot"), SETUPVALUE("txt_LKPort"));
		SetupValue(Vars.Aimbot.txt_JSPort, (void*)(&JSPort), SETUPVALUE("Aimbot"), SETUPVALUE("txt_JSPort"));
		SetupValue(Vars.Misc.txt_RadarIP, LocalHost, SETUPVALUE("Misc"), SETUPVALUE("txt_RadarIP1"));

		SetupValue(Vars.Misc.ShareRadar, true, SETUPVALUE("Misc"), SETUPVALUE("ShareRadar"));
		SetupValue(Vars.Misc.Projectile, true, SETUPVALUE("Misc"), SETUPVALUE("Projectile"));
		SetupValue(Vars.Misc.BattlePlayerInfo, false, SETUPVALUE("Misc"), SETUPVALUE("BattlePlayerInfo"));
		SetupValue(Vars.Misc.BattlePlayerInfoHotKey, 27, SETUPVALUE("Misc"), SETUPVALUE("BattlePlayerInfoHotKey"));
		SetupValue(Vars.Misc.PorjectileTime, true, SETUPVALUE("Misc"), SETUPVALUE("PorjectileTime"));
		SetupValue(Vars.Misc.PorjectileLine, true, SETUPVALUE("Misc"), SETUPVALUE("PorjectileLine"));
		SetupValue(Vars.Misc.ProjectileType, true, SETUPVALUE("Misc"), SETUPVALUE("ProjectileType"));
		SetupValue(Vars.Misc.ProjectileColor, std::vector<float>{ 1.f, 1.f, 1.f, 1.f}, SETUPVALUE("Misc"), SETUPVALUE("ProjectileColor"));
		SetupValue(Vars.Misc.ProjectRadius, true, SETUPVALUE("Misc"), SETUPVALUE("ProjectRadius"));
		SetupValue(Vars.Misc.ProjectDamageDistance, 30.f, SETUPVALUE("Misc"), SETUPVALUE("ProjectDamageDistance"));
		SetupValue(Vars.Misc.ProjectRadiusDistance, 50.f, SETUPVALUE("Misc"), SETUPVALUE("ProjectRadiusDistance1"));
		SetupValue(Vars.Misc.ProjectRadiusColor, std::vector<float>{ 1.0f, 0.f, 0.f, 1.f}, SETUPVALUE("Misc"), SETUPVALUE("ProjectRadiusColor"));
		SetupValue(Vars.Misc.multiProject_num, std::vector<BYTE>{ true, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false }, SETUPVALUE("Misc"), SETUPVALUE("multiProject_num"));


		SetupValue(Vars.Misc.Radar, true, SETUPVALUE("Misc"), SETUPVALUE("Radar"));
		SetupValue(Vars.Misc.MRadar, true, SETUPVALUE("Misc"), SETUPVALUE("MRadar"));
		SetupValue(Vars.Misc.RadarRadius, 8.f, SETUPVALUE("Misc"), SETUPVALUE("RadarRadius"));
		SetupValue(Vars.Misc.MRadarRadius, 10.f, SETUPVALUE("Misc"), SETUPVALUE("MRadarRadius"));
		SetupValue(Vars.Misc.VehicleMap, true, SETUPVALUE("Misc"), SETUPVALUE("VehicleMap"));
		SetupValue(Vars.Misc.AirDropsMap, true, SETUPVALUE("Misc"), SETUPVALUE("AirDropsMap"));
		SetupValue(Vars.Misc.NameMap, true, SETUPVALUE("Misc"), SETUPVALUE("NameMap"));
		SetupValue(Vars.Misc.Warning, true, SETUPVALUE("Misc"), SETUPVALUE("Warning"));
		SetupValue(Vars.Misc.WarningExt, true, SETUPVALUE("Misc"), SETUPVALUE("WarningExt"));
		SetupValue(Vars.Misc.BattlefieldInfo, false, SETUPVALUE("Misc"), SETUPVALUE("BattlefieldInfo"));
		SetupValue(Vars.Misc.BattlefieldInfoHotKey, VK_DELETE, SETUPVALUE("Misc"), SETUPVALUE("BattlefieldInfoHotKey"));
		SetupValue(Vars.Misc.BattlefieldUnlock, false, SETUPVALUE("Misc"), SETUPVALUE("BattlefieldUnlock"));
		SetupValue(Vars.Misc.BattlefieldUnlockHotKey, VK_F6, SETUPVALUE("Misc"), SETUPVALUE("BattlefieldUnlockHotKey"));

		SetupValue(Vars.Misc.EnableCollision, false, SETUPVALUE("Misc"), SETUPVALUE("EnbleCollision"));
		SetupValue(Vars.Misc.WeaponSkin, false, SETUPVALUE("Misc"), SETUPVALUE("WeaponSkin"));
		SetupValue(Vars.Misc.DisplayMode, 1, SETUPVALUE("Misc"), SETUPVALUE("DisplayMode1"));
		SetupValue(Vars.Misc.GrenadeCount, true, SETUPVALUE("Misc"), SETUPVALUE("GrenadeCount"));

		SetupValue(Vars.Misc.DesertEagle_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("DesertEagle_Skin"));
		SetupValue(Vars.Misc.G18_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("G18_Skin"));
		SetupValue(Vars.Misc.M9_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M9_Skin"));
		SetupValue(Vars.Misc.M1911_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M1911_Skin"));
		SetupValue(Vars.Misc.M1895_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M1895_Skin"));
		SetupValue(Vars.Misc.Rhino_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Rhino_Skin"));
		SetupValue(Vars.Misc.SawedOff_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("SawedOff_Skin"));
		SetupValue(Vars.Misc.Skorpion_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Skorpion_Skin"));
		SetupValue(Vars.Misc.AK47_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("AK47_Skin"));
		SetupValue(Vars.Misc.AUG_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("AUG_Skin"));
		SetupValue(Vars.Misc.AWM_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("AWM_Skin")
		);
		SetupValue(Vars.Misc.Berreta686_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Berreta686_Skin"));
		SetupValue(Vars.Misc.M762_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M762_Skin"));
		SetupValue(Vars.Misc.PP19_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("PP19_Skin"));
		SetupValue(Vars.Misc.Crossbow_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Crossbow_Skin"));
		SetupValue(Vars.Misc.DP12_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("DP12_Skin"));
		SetupValue(Vars.Misc.DP28_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("DP28_Skin"));
		SetupValue(Vars.Misc.FNFAL_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("FNFAL_Skin"));
		SetupValue(Vars.Misc.G36C_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("G36C_Skin"));
		SetupValue(Vars.Misc.Groza_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Groza_Skin"));
		SetupValue(Vars.Misc.HK416_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("HK416_Skin"));
		SetupValue(Vars.Misc.Kar98k_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Kar98k_Skin"));
		SetupValue(Vars.Misc.M16A4_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M16A4_Skin"));
		SetupValue(Vars.Misc.M24_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M24_Skin"));
		SetupValue(Vars.Misc.M249_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M249_Skin"));
		SetupValue(Vars.Misc.Mini14_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Mini14_Skin"));
		SetupValue(Vars.Misc.MK14_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("MK14_Skin"));
		SetupValue(Vars.Misc.MK47_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("MK47_Skin"));
		SetupValue(Vars.Misc.Mosin_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Mosin_Skin"));
		SetupValue(Vars.Misc.MP5K_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("MP5K_Skin"));
		SetupValue(Vars.Misc.QBU88_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("QBU88_Skin"));
		SetupValue(Vars.Misc.Ace32_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Ace32_Skin"));
		SetupValue(Vars.Misc.QBZ95_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("QBZ95_Skin"));
		SetupValue(Vars.Misc.Saiga12_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Saiga12_Skin"));
		SetupValue(Vars.Misc.SCARL_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("SCARL_Skin"));
		SetupValue(Vars.Misc.SKS_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("SKS_Skin"));
		SetupValue(Vars.Misc.Thompson_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Thompson_Skin"));
		SetupValue(Vars.Misc.UMP_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("UMP_Skin"));
		SetupValue(Vars.Misc.Uzi_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Uzi_Skin"));
		SetupValue(Vars.Misc.Vector_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Vector_Skin"));
		SetupValue(Vars.Misc.Vss_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Vss_Skin"));
		SetupValue(Vars.Misc.Win1894_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Win1894_Skin"));
		SetupValue(Vars.Misc.Win94_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Win94_Skin"));
		SetupValue(Vars.Misc.Winchester_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Winchester_Skin"));
		SetupValue(Vars.Misc.Crowbar_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Crowbar_Skin"));
		SetupValue(Vars.Misc.Machete_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Machete_Skin"));
		SetupValue(Vars.Misc.Pan_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Pan_Skin"));
		SetupValue(Vars.Misc.Sickle_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Sickle_Skin"));
		SetupValue(Vars.Misc.K2_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("K2_Skin"));
		SetupValue(Vars.Misc.MK12_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("MK12_Skin"));
		SetupValue(Vars.Misc.P90_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("P90_Skin"));
		SetupValue(Vars.Misc.M1897_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M1897_Skin"));
		SetupValue(Vars.Misc.MP9_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("MP9_Skin"));
		SetupValue(Vars.Misc.OriginS12_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("OriginS12_Skin"));
		SetupValue(Vars.Misc.Dragunov_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("Dragunov_Skin"));
		SetupValue(Vars.Misc.FamasG2_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("FamasG2_Skin"));
		SetupValue(Vars.Misc.M18_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M18_Skin"));
		SetupValue(Vars.Misc.M79_Skin, -1, SETUPVALUE("Misc"), SETUPVALUE("M79_Skin"));

		SetupValue(Vars.Misc.BackLv1_Skin, 0, SETUPVALUE("Misc"), SETUPVALUE("BackLv1_Skin"));
		SetupValue(Vars.Misc.BackLv2_Skin, 0, SETUPVALUE("Misc"), SETUPVALUE("BackLv2_Skin"));
		SetupValue(Vars.Misc.BackLv3_Skin, 0, SETUPVALUE("Misc"), SETUPVALUE("BackLv3_Skin"));

		SetupValue(Vars.Misc.ArmorLv1_Skin, 0, SETUPVALUE("Misc"), SETUPVALUE("ArmorLv1_Skin"));
		SetupValue(Vars.Misc.ArmorLv2_Skin, 0, SETUPVALUE("Misc"), SETUPVALUE("ArmorLv2_Skin"));
		SetupValue(Vars.Misc.ArmorLv3_Skin, 0, SETUPVALUE("Misc"), SETUPVALUE("ArmorLv3_Skin"));

		SetupValue(Vars.Misc.HelmetLv1_Skin, 0, SETUPVALUE("Misc"), SETUPVALUE("HelmetLv1_Skin"));
		SetupValue(Vars.Misc.HelmetLv2_Skin, 0, SETUPVALUE("Misc"), SETUPVALUE("HelmetLv2_Skin"));
		SetupValue(Vars.Misc.HelmetLv3_Skin, 0, SETUPVALUE("Misc"), SETUPVALUE("HelmetLv3_Skin"));
		SetupValue(Vars.Misc.BlackListShow, 1, SETUPVALUE("Misc"), SETUPVALUE("BlackListShow"));

		SetupValue(Vars.Menu.战斗模式HotKey, 192, SETUPVALUE("Menu"), SETUPVALUE("战斗模式HotKey"));
		SetupValue(Vars.Menu.LockFlame, true, SETUPVALUE("Menu"), SETUPVALUE("LockFlame1"));
		SetupValue(Vars.Menu.OpenedHotKey, VK_HOME, SETUPVALUE("Menu"), SETUPVALUE("OpenedHotKey"));
		SetupValue(Vars.Menu.SettingSwitchHotKey, VK_F9, SETUPVALUE("Menu"), SETUPVALUE("SettingSwitchHotKey"));
		SetupValue(Vars.Menu.FreshHotKey, VK_F8, SETUPVALUE("Menu"), SETUPVALUE("FreshHotKey"));
		SetupValue(Vars.Menu.config, 0, SETUPVALUE("Menu"), SETUPVALUE("config"));
		SetupValue(Vars.Menu.language, 1, SETUPVALUE("Menu"), SETUPVALUE("language"));
		SetupValue(Vars.Menu.FreshTrace, VK_F10, SETUPVALUE("Menu"), SETUPVALUE("FreshTrace"));
		SetupValue(Vars.Menu.UseCacheToTrace, 1, SETUPVALUE("Menu"), SETUPVALUE("UseCacheToTrace2"));
		SetupValue(Vars.Menu.PhysxTrace, 1, SETUPVALUE("Menu"), SETUPVALUE("PhysxTrace2"));
		SetupValue(Vars.Menu.AnnoceOpen, 1, SETUPVALUE("Menu"), SETUPVALUE("AnnoceOpen"));
		SetupValue(Vars.Menu.AnnoceVersion, 1, SETUPVALUE("Menu"), SETUPVALUE("AnnoceVersion"));
		SetupValue(Vars.Menu.DataInfo, 1, SETUPVALUE("Menu"), SETUPVALUE("DataInfo"));
		SetupValue(Vars.ConfigVer, ConfigVersion, SETUPVALUE("Config"), SETUPVALUE("ConfigVer"));
	}

	void CConfig::SetupValue(int& value, int def, hash_t category, hash_t name)
	{
		value = def;
		ints.push_back(new ConfigValue< int >(category, name, &value));
	}

	void CConfig::SetupValue(float& value, float def, hash_t category, hash_t name)
	{
		value = def;
		floats.push_back(new ConfigValue< float >(category, name, &value));
	}

	void CConfig::SetupValue(float* value, std::vector<float> def, hash_t category, hash_t name)
	{
		if (!value || def.size() != 4)
			return;
		value[0] = def[0];
		value[1] = def[1];
		value[2] = def[2];
		value[3] = def[3];
		colors.push_back(new ConfigValue< float>(category, name, value));
	}

	void CConfig::SetupValue(BYTE* value, std::vector<BYTE> def, hash_t category, hash_t name)
	{
		if (!value || !def.size())
			return;
		for (int i = 0; i < def.size(); i++)
		{
			value[i] = def[i];
		}
		bytes.push_back(new ConfigValue<BYTE>(category, name, value));
	}

	void CConfig::SetupValue(char*& value, void* def, hash_t category, hash_t name)
	{
		strcat(value, (const char*)def);
		texts.push_back(new ConfigValue< char* >(category, name, &value));
	}

	void CConfig::SetupValue(bool& value, bool def, const hash_t category, const hash_t name)
	{
		value = def;
		bools.push_back(new ConfigValue< bool >(category, name, &value));
	}

	void CConfig::Save(std::string path)
	{
		for (auto value : ints)
			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), std::to_string(*value->value).c_str(), path.c_str());
		for (auto value : floats)
			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), std::to_string(*value->value).c_str(), path.c_str());

		for (auto value : bools)
			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), *value->value ? "1" : "0", path.c_str());

		for (auto value : texts)
			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), *value->value, path.c_str());

		for (auto value : colors)
		{
			std::string _value;
			for (int i = 0; i < 4; i++)
				_value += std::to_string(value->value[i]) + (i != 3 ? "," : "");

			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), _value.c_str(), path.c_str());
		}

		for (auto value : bytes)
		{
			std::string _value;
			for (int i = 0; i < 20; i++)
				_value += std::to_string(value->value[i]) + (i != 19 ? "," : "");

			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), _value.c_str(), path.c_str());
		}

		if (Vars.ConfigVer != ConfigVer)
		{
			if (DeleteFileA(path.c_str()))
			{
				ints.clear();
				floats.clear();
				bools.clear();
				colors.clear();
				bytes.clear();
				Setup();
			}
			Vars.ConfigVer = ConfigVer;
			Save(path);
		}
	}

	void CConfig::Save2(std::string path)
	{
		for (auto value : ints)
			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), std::to_string(*value->value).c_str(), path.c_str());
		for (auto value : floats)
			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), std::to_string(*value->value).c_str(), path.c_str());

		for (auto value : bools)
			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), *value->value ? "1" : "0", path.c_str());

		for (auto value : texts)
			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), *value->value, path.c_str());

		for (auto value : colors)
		{
			std::string _value;
			for (int i = 0; i < 4; i++)
				_value += std::to_string(value->value[i]) + (i != 3 ? "," : "");

			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), _value.c_str(), path.c_str());
		}

		for (auto value : bytes)
		{
			std::string _value;
			for (int i = 0; i < 20; i++)
				_value += std::to_string(value->value[i]) + (i != 19 ? "," : "");

			WritePrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), _value.c_str(), path.c_str());
		}
	}

	void CConfig::SaveThread(std::string path)
	{
		Save(path);
		return;
	}

	static DWORD WINAPI SaveThreadStatic(LPVOID lpParam) {
		try
		{
			ThreadParams* params = static_cast<ThreadParams*>(lpParam);
			params->pConfig->SaveThread(params->path);
			delete params; // 注意释放内存
			ThreadSaveHandle = NULL;
			CONSOLE_INFO("Save Success");
		}
		catch (const std::exception&)
		{
			CONSOLE_INFO2("SaveThreadStatic 出現異常!");
		}
		return 0;
	}

	bool CConfig::ThreadSave(std::string path)
	{
		try
		{
			if (ThreadSaveHandle == nullptr) {
				ThreadParams* params = new ThreadParams;
				params->pConfig = this;
				params->path = path;
				ThreadSaveHandle = CreateThread(NULL, NULL, &SaveThreadStatic, params, NULL, NULL);
				return ThreadSaveHandle != NULL;
			}
		}
		catch (...)
		{
			CONSOLE_INFO2("ThreadSave 出現異常!");
		}
		return false;
	}

	void CConfig::Load(std::string path)
	{
		char value_l[50] = { '\0' };

		for (auto value : ints)
		{
			GetPrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), "def", value_l, (DWORD)32, path.c_str());

			if (strcmp(value_l, "def") != 0)
				*value->value = atoi(value_l);
		}

		for (auto value : floats)
		{
			GetPrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), "def", value_l, (DWORD)32, path.c_str());
			if (strcmp(value_l, "def") != 0)
				*value->value = (float)atof(value_l);
		}

		for (auto value : bools)
		{
			GetPrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), "def", value_l, (DWORD)32, path.c_str());
			if (strcmp(value_l, "def") != 0)
			{
				*value->value = !strcmp(value_l, "1");
				if (*value->value)
				{
					*value->value = 3 + rand();
				}
			}
		}
		for (auto value : colors)
		{
			GetPrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), "def", value_l, (DWORD)32, path.c_str());
			if (strcmp(value_l, "def") != 0)
			{
				auto _colors = Utils::SplitString(value_l, ",");
				if (_colors.size() == 4)
				{
					value->value[0] = (float)atof(_colors[0].c_str());
					value->value[1] = (float)atof(_colors[1].c_str());
					value->value[2] = (float)atof(_colors[2].c_str());
					value->value[3] = (float)atof(_colors[3].c_str());
				}
			}
		}
		for (auto value : texts)
		{
			GetPrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), "def", value_l, (DWORD)32, path.c_str());
			if (strcmp(value_l, "def") != 0)
			{
				memset(*value->value, 0, 0x20);
				strcat(*value->value, value_l);
			}
		}
		for (auto value : bytes)
		{
			GetPrivateProfileStringA(std::to_string(value->category).c_str(), std::to_string(value->name).c_str(), "def", value_l, (DWORD)50, path.c_str());
			if (strcmp(value_l, "def") != 0)
			{
				auto _bones = Utils::SplitString(value_l, ",");
				if (_bones.size() == 20)
				{
					value->value[0] = (bool)atoi(_bones[0].c_str());
					value->value[1] = (bool)atoi(_bones[1].c_str());
					value->value[2] = (bool)atoi(_bones[2].c_str());
					value->value[3] = (bool)atoi(_bones[3].c_str());
					value->value[4] = (bool)atoi(_bones[4].c_str());
					value->value[5] = (bool)atoi(_bones[5].c_str());
					value->value[6] = (bool)atoi(_bones[6].c_str());
					value->value[7] = (bool)atoi(_bones[7].c_str());
					value->value[8] = (bool)atoi(_bones[8].c_str());
					value->value[9] = (bool)atoi(_bones[9].c_str());
					value->value[10] = (bool)atoi(_bones[10].c_str());
					value->value[11] = (bool)atoi(_bones[11].c_str());
					value->value[12] = (bool)atoi(_bones[12].c_str());
					value->value[13] = (bool)atoi(_bones[13].c_str());
					value->value[14] = (bool)atoi(_bones[14].c_str());
					value->value[15] = (bool)atoi(_bones[15].c_str());
					value->value[16] = (bool)atoi(_bones[16].c_str());
					value->value[17] = (bool)atoi(_bones[17].c_str());
					value->value[18] = (bool)atoi(_bones[18].c_str());
					value->value[19] = (bool)atoi(_bones[19].c_str());
				}
			}
		}
	}
}