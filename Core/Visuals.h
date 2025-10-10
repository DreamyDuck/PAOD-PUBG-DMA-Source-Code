#pragma once
#include "SDK/SDK.h"
#include "Core/DirectX/DirectX11.h"
#include "Core/DirectX/Drawing.h"
#include "Helper/Misc/JsonHelper.h"
#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "ws2_32.lib")
#include <chrono>
namespace IronMan::Core
{
	using namespace SDK;
	using namespace physx;



	class PIDController {
	public:
		PIDController(double kp, double ki, double kd) : Kp(kp), Ki(ki), Kd(kd), integral(0.0f), last_error(0.0f), init_pid(false), dt(0.f) {}

		void init(double kp, double ki, double kd)
		{
			Kp = kp; Ki = ki; Kd = kd;
			integral = 0.0f;
			last_error = 0.0f;
			init_pid = false;
		}

		void clear()
		{
			integral = 0.0f;
			last_error = 0.0f;
			dt = 0.f;
		}

		double compute(double setpoint, double process_value) {
			if (last_error == 0.0f)
			{
				lastTime = std::chrono::high_resolution_clock::now();
			}
			auto now = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);
			dt = float(duration.count()) / 1000.f;
			//std::cout << "执行时间: " << dt << std::endl;

			lastTime = std::chrono::high_resolution_clock::now();
			double error = setpoint - process_value;
			double proportional = Kp * error;
			integral += (Ki * 10.f) * error * dt;
			if (abs(integral) > process_value)
			{
				clear();
				return 0;
			}
			double derivate = 0.0;
			if (dt == 0.f || std::abs(error) > 250.f) {
				derivate = 0.0;
			}
			else if (last_error != 0.f) {
				derivate = (Kd * 0.01f) * (error - last_error) / (float)dt;
			}
			else {
				derivate = 0.0;
				last_error = 0.f;
			}

			double output = proportional + integral + derivate;
			last_error = error;

			if (init_pid) {
				output = 0;
				init_pid = false;
			}

			return round(output);
		}

	private:
		double Kp;
		double Ki;
		double Kd;
		double integral;
		double last_error;
		float dt;
		std::chrono::steady_clock::time_point lastTime;
	public:
		bool init_pid;
	};

	const uint32_t TEAM_COLOR[100] =
	{
		ImColor(35,43,46),
		ImColor(133,76,190),
		ImColor(133,141,82),
		ImColor(152,113,152),
		ImColor(251,119,137),
		ImColor(119,47,180),
		ImColor(142,115,88),
		ImColor(26,240,204),
		ImColor(15,248,148),
		ImColor(101,244,117),
		ImColor(213,189,221),
		ImColor(36,201,33),
		ImColor(52,216,197),
		ImColor(32,154,186),
		ImColor(35,64,208),
		ImColor(152,66,239),
		ImColor(216,128,207),
		ImColor(188,87,142),
		ImColor(67,199,62),
		ImColor(2,173,118),
		ImColor(169,149,178),
		ImColor(106,8,14),
		ImColor(210,178,124),
		ImColor(16,237,62),
		ImColor(132,54,5),
		ImColor(150,216,98),
		ImColor(69,204,149),
		ImColor(81,66,174),
		ImColor(155,115,130),
		ImColor(65,51,250),
		ImColor(59,107,246),
		ImColor(49,92,240),
		ImColor(234,16,209),
		ImColor(160,237,13),
		ImColor(3,206,87),
		ImColor(52,159,3),
		ImColor(219,122,153),
		ImColor(244,174,79),
		ImColor(112,178,235),
		ImColor(114,179,33),
		ImColor(55,9,196),
		ImColor(171,161,241),
		ImColor(220,53,203),
		ImColor(83,79,82),
		ImColor(79,120,18),
		ImColor(183,92,222),
		ImColor(122,240,160),
		ImColor(139,70,81),
		ImColor(107,161,224),
		ImColor(46,17,51),
		ImColor(224,79,144),
		ImColor(87,210,158),
		ImColor(114,124,57),
		ImColor(35,245,254),
		ImColor(212,50,106),
		ImColor(248,31,227),
		ImColor(219,90,29),
		ImColor(195,240,59),
		ImColor(85,201,19),
		ImColor(112,26,140),
		ImColor(211,93,16),
		ImColor(178,134,11),
		ImColor(236,172,51),
		ImColor(123,123,161),
		ImColor(175,58,230),
		ImColor(167,5,54),
		ImColor(52,57,27),
		ImColor(208,16,230),
		ImColor(147,50,205),
		ImColor(70,240,227),
		ImColor(167,50,250),
		ImColor(152,76,52),
		ImColor(99,78,155),
		ImColor(61,170,149),
		ImColor(171,150,64),
		ImColor(47,15,25),
		ImColor(234,217,88),
		ImColor(161,102,105),
		ImColor(220,191,81),
		ImColor(25,182,175),
		ImColor(72,35,14),
		ImColor(138,78,142),
		ImColor(155,188,109),
		ImColor(71,101,185),
		ImColor(160,115,208),
		ImColor(236,241,4),
		ImColor(214,240,223),
		ImColor(138,148,3),
		ImColor(2,76,8),
		ImColor(184,242,152),
		ImColor(25,236,100),
		ImColor(87,215,250),
		ImColor(150,228,5),
		ImColor(51,238,31),
		ImColor(213,136,222),
		ImColor(189,242,138),
		ImColor(10,226,195),
		ImColor(149,173,241),
		ImColor(239,71,56),
		ImColor(163,28,55),
	};

	struct boneVecSave
	{
		FVector UpdateOrigin;
		FVector Origin;
		std::unordered_map<int, FVector> BoneArray;
	};

	struct FZeroingInfo {
		float BaseIron;
		float BaseScope;
		float Increment;
		float BaseHolo;
		inline FZeroingInfo()
			: BaseIron(100.0f), BaseScope(100.0f), Increment(100.0f), BaseHolo(BaseScope) {
		}
		inline FZeroingInfo(float _BaseIron, float _BaseScope, float _Increment, float _BaseHolo)
			: BaseIron(_BaseIron), BaseScope(_BaseScope), Increment(_Increment), BaseHolo(_BaseHolo) {
		}
	};

	struct pRankIcon
	{
	public:
		ImTextureID Icon;
		ImVec2 iconSize;
		inline pRankIcon()
			: Icon(nullptr), iconSize(0, 0) {
		}
		inline pRankIcon(ImTextureID icon, ImVec2 iconsize)
			: Icon(icon),
			iconSize(iconsize) {
		}
	};

	struct NetPlayerInfo
	{
	public:
		int currentRankPoint;
		float kda;
		std::string rank_tier;
		std::string rank_tier_icon;
		int damage;
		inline NetPlayerInfo()
			: rank_tier(("Unranked")), rank_tier_icon(("Unranked")), currentRankPoint(0), kda(0.f), damage(0) {
		}
	};

	struct RankGameMode
	{
	public:
		NetPlayerInfo solo;
		NetPlayerInfo squad;
		inline RankGameMode()
			:solo(), squad() {
		}
	};

	struct AllRankModeInfo
	{
	public:
		RankGameMode FPP;
		RankGameMode TPP;
		inline AllRankModeInfo()
			:FPP(), TPP() {
		}
	};

	struct ScoreBoard
	{
	public:
		int TeamNumber;
		std::string PlayerName;
		std::string TeamName;
		int Kill;
		int Assist;
		int Ranking;
		int Level;
		float KDA;
		float Damage;
		float AvgDamage;
		bool Partner;
		bool bFog;
		inline ScoreBoard()
			:TeamNumber(0), PlayerName(""), TeamName(""), Kill(0), Assist(0), Level(0), KDA(0.f), Partner(false), Damage(0.f), AvgDamage(0.f), Ranking(0), bFog(false){
		}
	};

	struct Item_Container
	{
		bool* open;
		float* color;
		inline Item_Container()
			: open(nullptr), color(nullptr) {
		}
		inline Item_Container(bool* _open, float* _color)
			: open(_open),
			color(_color) {
		}
	};

	struct Pivot
	{
		Pivot() :x(), y() {}
		Pivot(float x, float y) :x(x), y(y) {}

		inline JsonReader& Deserialize(JsonReader& ar)
		{
			ar.StartObject();
			ar.Member("x")& x;
			ar.Member("y")& y;
			return ar.EndObject();
		}
		float x;
		float y;
	};

	struct Size
	{
		Size() :w(), h() {}
		Size(float w, float h) :w(w), h(h) {}
		inline JsonReader& Deserialize(JsonReader& ar)
		{
			ar.StartObject();
			ar.Member("w")& w;
			ar.Member("h")& h;
			return ar.EndObject();
		}
		float w;
		float h;
	};
	struct Rect
	{
		Rect() :x(), y(), w(), h() {}
		Rect(float x, float y, float w, float h) :x(x), y(y), w(w), h(h) {}
		inline JsonReader& Deserialize(JsonReader& ar)
		{
			ar.StartObject();
			ar.Member("x")& x;
			ar.Member("y")& y;
			ar.Member("w")& w;
			ar.Member("h")& h;
			return ar.EndObject();
		}
		float x;
		float y;
		float w;
		float h;
	};
	class Sprite
	{
	public:
		Sprite() : initialized(false)
		{
		}
		Sprite(std::string name, Rect frame, bool rotated, bool trimmed, Rect spriteSourceSize, Size sourceSize, Pivot pivot)
			:frame(frame), rotated(rotated), trimmed(trimmed), spriteSourceSize(spriteSourceSize), sourceSize(sourceSize), pivot(pivot), initialized(false)
		{
		}
		inline JsonReader& Deserialize(JsonReader& ar)
		{
			ar.StartObject();


			if (ar.Member(("frame")))
				frame.Deserialize(ar);

			ar.Member(("rotated"))& rotated;
			ar.Member(("trimmed"))& trimmed;

			if (ar.HasMember(("spriteSourceSize")))
				spriteSourceSize.Deserialize(ar);

			if (ar.HasMember(("sourceSize")))
				sourceSize.Deserialize(ar);

			if (ar.HasMember(("pivot")))
				pivot.Deserialize(ar);

			if (ar)	initialized = true;


			return ar.EndObject();
		}
		operator bool() const { return initialized; }

		Rect frame;
		bool rotated;
		bool trimmed;
		Rect spriteSourceSize;
		Size sourceSize;
		Pivot pivot;
	private:
		bool initialized;
	};
	class Meta
	{
	public:
		Meta() {}
		Meta(Size size, std::string scale)
			:size(size), scale(scale)
		{
		}

		inline JsonReader& Deserialize(JsonReader& ar)
		{
			ar.Member(("meta"));
			ar.StartObject();
			//ar.Member(("version")) & version;
			//ar.Member(("target")) & target;
			//ar.Member(("image")) & image;
			//ar.Member(("format")) & format;

			ar.Member(("size"));
			size.Deserialize(ar);

			ar.Member(("scale"))& scale;
			//ar.Member(("smartupdate")) & smartupdate;
			return ar.EndObject();
		}
		//std::string version;
		//std::string target;
		//std::string image;
		//std::string format;
		Size size;
		std::string scale;
		//std::string smartupdate;
	};
	class Sprites
	{
	public:
		Sprites() {}

		inline JsonReader& Deserialize(JsonReader& ar)
		{
			for (auto iter = frames.begin(); iter != frames.end();) {
				frames.erase(iter++);
			}

			ar.StartObject();
			ar.Member(("frames"));
			size_t frameCount = 0;
			ar.StartObject(&frameCount);
			for (int i = 0; i < frameCount; i++)
			{
				std::string objectName;
				ar.Member(i, objectName);
				if (!objectName.empty())
					frames[hash_(objectName.c_str())].Deserialize(ar);
			}
			ar.EndObject();
			meta.Deserialize(ar);
			return ar.EndObject();
		}
		std::unordered_map<hash_t, Sprite> frames;
		Meta meta;
	};


	struct BaseUv
	{
		BaseUv() :farme() {}
		virtual ~BaseUv() {}
		virtual const char* GetType() const = 0;

		inline virtual JsonReader& Deserialize(JsonReader& ar)
		{
			ar.Member("farme");
			return farme.Deserialize(ar);
		}
		Rect farme;
	};

	struct Bar : public BaseUv
	{
		Bar() :BaseUv() {}
		const char* GetType() const { return "bar"; }
		inline JsonReader& Deserialize(JsonReader& ar)
		{
			return BaseUv::Deserialize(ar);
		}
	};
	struct Icon : public BaseUv
	{
		Icon() :BaseUv(), scale() {}
		Icon(float scale) :scale(scale) {}

		const char* GetType() const { return "icon"; }

		inline JsonReader& Deserialize(JsonReader& ar)
		{
			ar.Member("scale")& scale;
			return BaseUv::Deserialize(ar);
		}
		float scale;
	};
	struct Font : public BaseUv
	{
		Font() :BaseUv(), fontSize() {}
		Font(float fontSize) :fontSize(fontSize) {}
		const char* GetType() const { return "font"; }

		inline JsonReader& Deserialize(JsonReader& ar)
		{
			ar.Member("fontSize")& fontSize;
			return BaseUv::Deserialize(ar);
		}

		float fontSize;
	};

	class SpritesUV
	{
	public:
		SpritesUV() :frames() {}

		class Sprite
		{
		public:
			Sprite() :initialized(false), uvs() {}
			~Sprite()
			{
				for (auto itr : uvs)
					delete itr.second;
			}

			operator bool() const { return initialized; }

			inline JsonReader& Deserialize(JsonReader& ar)
			{
				size_t uvCount = 0;
				ar.StartObject(&uvCount);
				for (int i = 0; i < uvCount; i++)
				{
					std::string objectName;
					ar.Member(i, objectName);
					if (!objectName.empty())
					{
						ar.StartObject();
						std::string type;
						ar.Member("type")& type;

						BaseUv* uv = nullptr;
						if (type == "bar")
							uv = new Bar;
						else if (type == "icon")
							uv = new Icon;
						else if (type == "font")
							uv = new Font;

						if (uv)
						{
							uv->Deserialize(ar);
							uvs[hash_(objectName.c_str())] = uv;
						}
						ar.EndObject();
					}
				}
				if (ar) initialized = true;
				return ar.EndObject();
			}
			std::map<hash_t, BaseUv*> uvs;
		private:
			bool initialized;
		};

		inline JsonReader& Deserialize(JsonReader& ar)
		{
			for (auto iter = frames.begin(); iter != frames.end();) {
				frames.erase(iter++);
			}

			size_t frameCount = 0;
			ar.StartObject(&frameCount);
			for (int i = 0; i < frameCount; i++)
			{
				std::string objectName;
				ar.Member(i, objectName);
				if (!objectName.empty())
					frames[hash_(objectName.c_str())].Deserialize(ar);
			}
			return ar.EndObject();
		}
		std::map<hash_t, Sprite> frames;
	};

	struct CapSuleInfo
	{
		ptr_t Actor;
		PxTransform BodyTransform;
		float Radius;
		float halfHigh;
		inline CapSuleInfo() :Radius(0.f), halfHigh(0.f), BodyTransform(PxTransform()), Actor(0) {}
		inline CapSuleInfo(float a, float b, PxTransform c, ptr_t f) : Radius(a), halfHigh(b), BodyTransform(c), Actor(f) {}
	};

	class HealthBar
	{
		friend class Visuals;
	private:
		HealthBar();
		~HealthBar();
		bool Load();
		inline bool IsLoaded() const { return m_Loaded; }
	private:
		Sprites* m_sprites;
		SpritesUV* m_uvs;
		ImTextureID m_Tex;
		ImTextureID m_CarePackage;
		ImTextureID m_MapCarePackage;
		ImTextureID m_MapVehicle;
		ImTextureID m_MapBoat;
		ImTextureID m_Disconnected;
		ImTextureID m_Partners;
		ImTextureID m_Bronze1;
		ImTextureID m_Bronze2;
		ImTextureID m_Bronze3;
		ImTextureID m_Bronze4;
		ImTextureID m_Bronze5;
		ImTextureID m_Silver1;
		ImTextureID m_Silver2;
		ImTextureID m_Silver3;
		ImTextureID m_Silver4;
		ImTextureID m_Silver5;
		ImTextureID m_Gold1;
		ImTextureID m_Gold2;
		ImTextureID m_Gold3;
		ImTextureID m_Gold4;
		ImTextureID m_Gold5;
		ImTextureID m_Platinum1;
		ImTextureID m_Platinum2;
		ImTextureID m_Platinum3;
		ImTextureID m_Platinum4;
		ImTextureID m_Platinum5;
		ImTextureID m_Diamond1;
		ImTextureID m_Diamond2;
		ImTextureID m_Diamond3;
		ImTextureID m_Diamond4;
		ImTextureID m_Diamond5;
		ImTextureID m_Master1;
		ImTextureID m_Unranked;
		bool	m_Loaded;
	};

	class Visuals
	{
	public:
		static bool Initialize();
		static void InitSkinData();
		static void PlayerTrigger(ATslCharacter& player);
	private:
		static void Update();
		static void BeginDraw();
		static void EndDraw();
		/// Player
		static void ActorESP(AActor& Actor);
		static void PlayerESP(ATslCharacter& player, std::unordered_map<ptr_t, std::shared_ptr<CapSuleInfo>>& inCapsule, std::unordered_map<ptr_t, std::vector<FVector>>& inArray);
		static void PlayerSkeleton(ATslCharacter& player, float headWidth, float, ImColor color, ImColor InvisibleColor, bool bIsVisible);
		static void PlayerHealthBar(ATslCharacter& player, float center, float distance, float h, ImVec2 FootPos, int TeamID);
		static void PlayerBox(ATslCharacter& player, float x, float y, float w, float h, float dis, ImColor color);
		static void PlayerAimPosition(ATslCharacter& player, FVector HeadPosition, float t, ImColor color, float Distance);

		static void ProjectESP(std::vector<std::shared_ptr<AActor>>& projects);
		static bool 定义雷达图(FVector deltaFromLocal, int TeamNum, FRotator LastAimOffset, int Drawtype = 1);
		static bool MapRadar2(FVector PlayerLocation, float  ObjectFov, int TeamNum, int Type, ImTextureID Texture = nullptr, ImVec2 uv0 = ImVec2(0, 0), ImVec2 uv1 = ImVec2(0, 0), std::string PlayerName = "");
		static void DrawScopeBoard();
		static void DrawScopeBoard2();
		/// LootGroup
		static void LootGroupESP(ADroppedItemGroup& lootGroup);

		/// Loot
		static void LootESP(ADroppedItem& loot);

		/// Airdrop
		static void AirdropESP(ACarePackageItem& airdrop);

		/// DeathBox
		static void DeathBoxESP(AFloorSnapItemPackage& deathbox);

		///Vehicle
		static void VehicleESP(ATslWheeledVehicle& vehicle);

		///Boat
		static void BoatESP(ATslFloatingVehicle& boat);

		static pRankIcon getRankIcon(std::string rank_tier);
	public:
		static HealthBar* m_HealthBar;
		static std::unordered_map <ptr_t, std::shared_ptr<std::vector<FVector>>> ProjectPath;
		static std::unordered_map <hash_t, std::shared_ptr<pRankIcon>> RankingIcon;
		static std::vector<ScoreBoard>m_ScoreBoard;
		static std::vector<ScoreBoard>m_ScoreBoard2;
	};

	namespace Aimbot
	{
		extern float deltaTime;
		extern float curAimTime;
		FVector GetPlayerVelocity(ATslCharacter& p);
		FVector GetPrediction(ATslCharacter& target, FVector& aimPos, float& dropPitchOffset, FVector PlayerVelocity);
		void AimToTarget();
		float GetNextPos(ATslCharacter& player, float dist, FVector PlayerVelocity);
		bool isDMR();
		bool isShotGun();
		bool isSR();
		bool isGrenade();
		void mouseleftdown();
		void mouseleftup();
	}
}
