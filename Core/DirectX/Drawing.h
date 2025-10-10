#pragma once
#include "Core/Math/Vector.hpp"
#include "Core/Imgui/imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "Core/Imgui/imgui_internal.h"

namespace IronMan::Core
{
	using namespace Math;

	enum FontRenderFlag_t
	{
		FONT_LEFT = 0,
		FONT_RIGHT = 1,
		FONT_CENTER = 2
	};

	namespace Settings
	{
		constexpr uint32_t ColorWhite = 0xFFfffffd;
		constexpr uint32_t ColorRed = 0xFF0000ff;
		constexpr uint32_t ColorGreen = 0xFF02ff00;
		constexpr uint32_t ColorPurple = 0xFFA020F0;
		constexpr uint32_t ColorGreyDim = 0xFF696969;
		constexpr uint32_t ColorBlack = 0xFF000000;
		constexpr uint32_t ColorSkyBlue = 0xFF00BFFF;
		constexpr uint32_t ColorLightBlue = 0xff00FFFF;
		constexpr uint32_t ColorGold = 0xffFFD700;
		constexpr uint32_t ColorOrangeRed = 0xffFF4500;
		constexpr uint32_t ColorLightGreen = 0xff90EE90;
		constexpr uint32_t ColorOrange = 0xffFFA500;
		constexpr uint32_t ColorDarkCyan = 0xff008B8B;
		constexpr uint32_t ColorMagenta1 = 0xffFF00FF;
		constexpr uint32_t ColorYellow = 0xffFFFF00;
		constexpr uint32_t Äµµ¤ºì = 0xffCC00ff;
		constexpr uint32_t É­ÂÌÉ« = 0xff4Dff4D;
		constexpr uint32_t ÄÞºçÀ¶ = 0xff00FFFF;
		constexpr uint32_t º£ÂÌÉ« = 0xff68C823;
		constexpr uint32_t ÌÒºì = 0xffde4383;
	}

	class Drawing
	{
	public:
		static FVector2D GetViewPort();
		static void DrawLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f);
		static void DrawOutlinedRect(const ImVec2& a, const ImVec2& b, ImU32 col, ImU32 outlineCol, float thickness = 1.0f, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);
		static void DrawRect(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);
		static void DrawRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);                     // a: upper-left, b: lower-right
		static void DrawRectFilledMultiColor(const ImVec2& a, const ImVec2& b, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
		static void DrawOutlinedQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col, ImU32 outlineCol, float thickness = 1.0f);
		static void DrawQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col, float thickness = 1.0f);
		static void DrawQuadFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col);
		static void DrawTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col, float thickness = 1.0f);
		static void DrawTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col);
		static void DrawCircle(const ImVec2& centre, float radius, uint32_t col, int num_segments = 12, float thickness = 1.0f);
		static void DrawCircleFilled(const ImVec2& centre, float radius, uint32_t col, int num_segments = 12);
		static ImVec2 CalcTextSize(const ImFont* font, float fontSize, float clip_w, ImVec2 pos, ImU32 col, ImU32 outlineCol, DWORD alignment, const char* text, ...);
		static ImVec2 DrawTextOutline(const ImFont* font, float fontSize, float clip_w, ImVec2 pos, ImU32 col, ImU32 outlineCol, DWORD alignment, const char* text, ...);
		static void DrawTextV(const ImVec2& pos, ImU32 col, std::string text);
		static void DrawTextV(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = NULL);
		static void DrawImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 1), ImU32 col = 0xFFFFFFFF);
		static void DrawImageQuad(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 0), const ImVec2& uv_c = ImVec2(1, 1), const ImVec2& uv_d = ImVec2(0, 1), ImU32 col = 0xFFFFFFFF);
		static void DrawImageRounded(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col, float rounding, int rounding_corners = ImDrawCornerFlags_All);
		static void DrawPolyline(const ImVec2* points, const int num_points, ImU32 col, bool closed, float thickness);
		static void DrawConvexPolyFilled(const ImVec2* points, const int num_points, ImU32 col); // Note: Anti-aliased filling requires points to be in clockwise order.
		static void DrawBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, int num_segments = 0);
		static void DrawBox3D(FVector origin, FVector extends, ImU32 col, float thickness = 1.0f);


		static void DrawBox(const ImVec2& pos, const ImVec2& headPosition, uint32_t color);
		static void DrawBox_S(const ImVec2& pos, const ImVec2& headPosition, uint32_t color, float Div, float Siz);
		static void DrawHealthBar(const ImVec2& pos, const ImVec2& headPosition, float Health, float HealthMax, bool IsDown, ImVec4 color, ImVec4 outlineColor);
		static float DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& position, float size, uint32_t color, bool center);

		static uint32_t StreamColor;
		static uint32_t CombatColor;
	};

}