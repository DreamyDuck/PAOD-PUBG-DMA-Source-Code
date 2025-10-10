#include "Imgui/imgui.h"

namespace IronMan::Core
{

	class Menu
	{
	public:
		static bool Initialize(bool isEng);
		static void Shutdown();
	private:
		//void 

	private:
		static void OnRender();
		static void ProcessInputMessage();
		static void ChineseMenu();
		static void add_hotkey(const char* text, int* option_toggle, char* NameText, bool* IsFind);
		static void RenderEsp();
		static void RenderAimbot();
		static void RenderMisc();
		static void RenderWeapon();
		static void RenderEquipment();
		static void RenderEquipment2();
		static void RenderAnnoce();
		static void SetTooltip(std::string Text);

		static bool m_English;
		static ImTextureID VisualNormal;
		static ImTextureID VisualActive;
		static ImTextureID AimbotNormal;
		static ImTextureID AimbotActive;
		static ImTextureID MiscNormal;
		static ImTextureID MiscActive;
		static ImTextureID WeaponNormal;
		static ImTextureID WeaponActive;
		static ImTextureID BoxNormal1;
		static ImTextureID BoxActive1;
		static ImTextureID BoxNormal2;
		static ImTextureID BoxActive2;
		static ImTextureID Project1Normal;
		static ImTextureID Project1Active;
		static ImTextureID Project2Normal;
		static ImTextureID Project2Active;
		static ImTextureID ChineseNormal;
		static ImTextureID ChineseActive;
		static ImTextureID EnglishNormal;
		static ImTextureID EnglishActive;
		static ImTextureID HumanMoveable;
		static ImTextureID WeaponHold;
	};

}