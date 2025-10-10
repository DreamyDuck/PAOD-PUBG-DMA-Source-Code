#include "Include/WinHeaders.hpp"
#include "Drawing.h"
#include "Core/SDK/SDK.h"
#include "Core/Config.h"
namespace IronMan::Core
{

	uint32_t Drawing::StreamColor = 0xFFFFFFFF;
	uint32_t Drawing::CombatColor = 0;

	FVector2D Drawing::GetViewPort()
	{
		auto displaySize = ImGui::GetIO().DisplaySize;
		return FVector2D(displaySize.x, displaySize.y);
	}

	void Drawing::DrawLine(const ImVec2& from, const ImVec2& to, ImU32 color, float thickness)
	{
		float a = (color >> 24) & 0xff;
		float r = (color) & 0xff;
		float g = (color >> 8) & 0xff;
		float b = (color >> 16) & 0xff;

		ImGui::GetOverlayDrawList()->AddLine(from, to, ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), thickness);
	}

	void Drawing::DrawOutlinedRect(const ImVec2& a, const ImVec2& b, ImU32 col, ImU32 outlineCol, float thickness, float rounding, int rounding_corners_flags)
	{
		ImGui::GetOverlayDrawList()->AddRect(a, b, ImGui::GetColorU32(col), rounding, rounding_corners_flags, thickness);
		{
			ImGui::GetOverlayDrawList()->AddRect(a - ImVec2(1.0f, 1.0f), b + ImVec2(1.0f, 1.0f), ImGui::GetColorU32(outlineCol), rounding, rounding_corners_flags, thickness);
			ImGui::GetOverlayDrawList()->AddRect(a + ImVec2(1.0f, 1.0f), b - ImVec2(1.0f, 1.0f), ImGui::GetColorU32(outlineCol), rounding, rounding_corners_flags, thickness);
		}
	}

	void Drawing::DrawRect(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness, float rounding, int rounding_corners_flags)
	{
		ImGui::GetOverlayDrawList()->AddRect(a, b, ImGui::GetColorU32(col), rounding, rounding_corners_flags, thickness);
	}

	void Drawing::DrawRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners_flags)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(a, b, ImGui::GetColorU32(col), rounding, rounding_corners_flags);
	}

	void Drawing::DrawRectFilledMultiColor(const ImVec2& a, const ImVec2& b, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
	{
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(a, b, ImGui::GetColorU32(col_upr_left), ImGui::GetColorU32(col_upr_right), ImGui::GetColorU32(col_bot_right), ImGui::GetColorU32(col_bot_left));
	}

	void Drawing::DrawOutlinedQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col, ImU32 outlineCol, float thickness)
	{
		ImGui::GetOverlayDrawList()->AddQuad(a, b, c, d, ImGui::GetColorU32(col), thickness);
		//ImGui::GetOverlayDrawList()->AddQuad(a, b, c, d, col, thickness);
		//ImGui::GetOverlayDrawList()->AddQuad(ImVec2(a.X - 1, a.Y - 1), ImVec2(b.X + 1, b.Y + 1), outlineCol, rounding, rounding_corners_flags, thickness);
		//ImGui::GetOverlayDrawList()->AddQuad(ImVec2(a.X + 1, a.Y + 1), ImVec2(b.X - 1, b.Y - 1), outlineCol, rounding, rounding_corners_flags, thickness);

	}

	void Drawing::DrawQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col, float thickness)
	{
		ImGui::GetOverlayDrawList()->AddQuad(a, b, c, d, ImGui::GetColorU32(col), thickness);
	}

	void Drawing::DrawQuadFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col)
	{
		ImGui::GetOverlayDrawList()->AddQuadFilled(a, b, c, d, ImGui::GetColorU32(col));
	}

	void Drawing::DrawTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col, float thickness)
	{
		ImGui::GetOverlayDrawList()->AddTriangle(a, b, c, ImGui::GetColorU32(col), thickness);
	}

	void Drawing::DrawTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col)
	{
		ImGui::GetOverlayDrawList()->AddTriangleFilled(a, b, c, ImGui::GetColorU32(col));
	}

	void Drawing::DrawCircle(const ImVec2& centre, float radius, uint32_t color, int num_segments, float thickness)
	{
		float a = (color >> 24) & 0xff;
		float r = (color) & 0xff;
		float g = (color >> 8) & 0xff;
		float b = (color >> 16) & 0xff;

		ImGui::GetOverlayDrawList()->AddCircle(centre, radius, ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), num_segments, thickness);
	}

	void Drawing::DrawCircleFilled(const ImVec2& position, float radius, uint32_t color, int num_segments)
	{
		float a = (color >> 24) & 0xff;
		float r = (color) & 0xff;
		float g = (color >> 8) & 0xff;
		float b = (color >> 16) & 0xff;

		ImGui::GetOverlayDrawList()->AddCircleFilled(position, radius, ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), 64);
	}

	ImVec2 Drawing::CalcTextSize(const ImFont* font, float fontSize, float clip_w, ImVec2 pos, ImU32 col, ImU32 outlineCol, DWORD alignment, const char* text, ...)
	{
		auto drawList = ImGui::GetOverlayDrawList();

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
		return text_size;
	}

	ImVec2 Drawing::DrawTextOutline(const ImFont* font, float fontSize, float clip_w, ImVec2 pos, ImU32 col, ImU32 outlineCol, DWORD alignment, const char* text, ...)
	{
		auto drawList = ImGui::GetOverlayDrawList();

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
			drawList->AddText(font, fontSize, pos - ImVec2(1.0f, 0.0f), /*ImGui::GetColorU32(outlineCol)*/0xFF101010, buf, nullptr, 0, cpu_fine_clip_rect);
			drawList->AddText(font, fontSize, pos - ImVec2(0.0f, 1.0f), /*ImGui::GetColorU32(outlineCol)*/0xFF101010, buf, nullptr, 0, cpu_fine_clip_rect);
			drawList->AddText(font, fontSize, pos + ImVec2(1.0f, 0.0f), /*ImGui::GetColorU32(outlineCol)*/0xFF101010, buf, nullptr, 0, cpu_fine_clip_rect);
			drawList->AddText(font, fontSize, pos + ImVec2(0.0f, 1.0f), /*ImGui::GetColorU32(outlineCol)*/0xFF101010, buf, nullptr, 0, cpu_fine_clip_rect);
		}
		drawList->AddText(font, fontSize, pos, ImGui::GetColorU32(col), buf, nullptr, 0, cpu_fine_clip_rect);
		return text_size;
	}

	void Drawing::DrawTextV(const ImVec2& pos, ImU32 col, std::string text)
	{
		ImGui::GetOverlayDrawList()->AddText(pos, ImGui::GetColorU32(col), text.c_str(), nullptr);
	}

	void Drawing::DrawTextV(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
	{
		ImGui::GetOverlayDrawList()->AddText(font, font_size, pos, ImGui::GetColorU32(col), text_begin, text_end, wrap_width, cpu_fine_clip_rect);
	}

	void Drawing::DrawImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col)
	{
		ImGui::GetOverlayDrawList()->AddImage(user_texture_id, a, b, uv_a, uv_b, ImGui::GetColorU32(col));
	}

	void Drawing::DrawImageQuad(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col)
	{
		ImGui::GetOverlayDrawList()->AddImageQuad(user_texture_id, a, b, c, d, uv_a, uv_b, uv_c, uv_d, ImGui::GetColorU32(col));
	}

	void Drawing::DrawImageRounded(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col, float rounding, int rounding_corners)
	{
		ImGui::GetOverlayDrawList()->AddImageRounded(user_texture_id, a, b, uv_a, uv_b, ImGui::GetColorU32(col), rounding, rounding_corners);

	}

	void Drawing::DrawPolyline(const ImVec2* points, const int num_points, ImU32 col, bool closed, float thickness)
	{
		ImGui::GetOverlayDrawList()->AddPolyline((ImVec2*)points, num_points, ImGui::GetColorU32(col), closed, thickness);
	}

	void Drawing::DrawConvexPolyFilled(const ImVec2* points, const int num_points, ImU32 col)
	{
		ImGui::GetOverlayDrawList()->AddConvexPolyFilled(points, num_points, ImGui::GetColorU32(col));
	}

	void Drawing::DrawBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, int num_segments)
	{
		ImGui::GetOverlayDrawList()->AddBezierCurve(pos0, cp0, cp1, pos1, ImGui::GetColorU32(col), thickness, num_segments);
	}

	void Drawing::DrawBox3D(FVector origin, FVector extends, ImU32 col, float thickness)
	{

		origin -= extends / 2.0f;

		// bottom plane
		FVector one = origin;
		FVector two = origin; two.X += extends.X;
		FVector three = origin; three.X += extends.X; three.Y += extends.Y;
		FVector four = origin; four.Y += extends.Y;

		FVector five = one; five.Z += extends.Z;
		FVector six = two; six.Z += extends.Z;
		FVector seven = three; seven.Z += extends.Z;
		FVector eight = four; eight.Z += extends.Z;

		FVector2D s1, s2, s3, s4, s5, s6, s7, s8;
		if (!SDK::sdk::WorldToScreen(one, s1) || !SDK::sdk::WorldToScreen(two, s2) || !SDK::sdk::WorldToScreen(three, s3) || !SDK::sdk::WorldToScreen(four, s4))
			return;

		if (!SDK::sdk::WorldToScreen(five, s5) || !SDK::sdk::WorldToScreen(six, s6) || !SDK::sdk::WorldToScreen(seven, s7) || !SDK::sdk::WorldToScreen(eight, s8))
			return;

		// 3 copies of each vertex? trash
		DrawLine(s1, s2, col, thickness);
		DrawLine(s2, s3, col, thickness);
		DrawLine(s3, s4, col, thickness);
		DrawLine(s4, s1, col, thickness);

		DrawLine(s5, s6, col, thickness);
		DrawLine(s6, s7, col, thickness);
		DrawLine(s7, s8, col, thickness);
		DrawLine(s8, s5, col, thickness);

		DrawLine(s1, s5, col, thickness);
		DrawLine(s2, s6, col, thickness);
		DrawLine(s3, s7, col, thickness);
		DrawLine(s4, s8, col, thickness);
	}

	void Drawing::DrawBox(const ImVec2& pos, const ImVec2& headPosition, uint32_t color)
	{
		DrawLine(ImVec2(pos.x, pos.y), ImVec2(pos.x, pos.y + headPosition.y), color, 1.5f);
		DrawLine(ImVec2(pos.x, pos.y), ImVec2(pos.x + headPosition.x, pos.y), color, 1.5f);

		DrawLine(ImVec2(pos.x, pos.y + headPosition.y), ImVec2(pos.x + headPosition.x, pos.y + headPosition.y), color, 1.f);
		DrawLine(ImVec2(pos.x + headPosition.x, pos.y), ImVec2(pos.x + headPosition.x, pos.y + headPosition.y), color, 1.f);

	}

	void Drawing::DrawBox_S(const ImVec2& pos, const ImVec2& headPosition, uint32_t color, float Div, float Siz)
	{
		{
			DrawLine(ImVec2(pos.x + 1.f, pos.y), ImVec2(pos.x + (headPosition.x / Div) + 1.f, pos.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//上左右--
			DrawLine(ImVec2(pos.x - 1.f, pos.y), ImVec2(pos.x + (headPosition.x / Div) - 1.f, pos.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//上左右--
			DrawLine(ImVec2(pos.x, pos.y + 1.f), ImVec2(pos.x + (headPosition.x / Div), pos.y + 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//上左右--
			DrawLine(ImVec2(pos.x, pos.y - 1.f), ImVec2(pos.x + (headPosition.x / Div), pos.y - 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//上左右--


			DrawLine(ImVec2(pos.x + headPosition.x - (headPosition.x / Div) + 1.f, pos.y), ImVec2(pos.x + headPosition.x + 1.f, pos.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//上左右--
			DrawLine(ImVec2(pos.x + headPosition.x - (headPosition.x / Div) - 1.f, pos.y), ImVec2(pos.x + headPosition.x - 1.f, pos.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//上左右--
			DrawLine(ImVec2(pos.x + headPosition.x - (headPosition.x / Div), pos.y + 1.f), ImVec2(pos.x + headPosition.x, pos.y + 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//上左右--
			DrawLine(ImVec2(pos.x + headPosition.x - (headPosition.x / Div), pos.y - 1.f), ImVec2(pos.x + headPosition.x, pos.y - 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//上左右--


			DrawLine(ImVec2(pos.x + 1.f, pos.y), ImVec2(pos.x + 1.f, pos.y + (headPosition.y / Div)), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//左上下|
			DrawLine(ImVec2(pos.x - 1.f, pos.y), ImVec2(pos.x - 1.f, pos.y + (headPosition.y / Div)), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//左上下|
			DrawLine(ImVec2(pos.x, pos.y + 1.f), ImVec2(pos.x, pos.y + (headPosition.y / Div) + 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//左上下|
			DrawLine(ImVec2(pos.x, pos.y - 1.f), ImVec2(pos.x, pos.y + (headPosition.y / Div) - 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//左上下|


			DrawLine(ImVec2(pos.x + 1.f, pos.y + headPosition.y - (headPosition.y / Div)), ImVec2(pos.x + 1.f, pos.y + headPosition.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//左上下|
			DrawLine(ImVec2(pos.x - 1.f, pos.y + headPosition.y - (headPosition.y / Div)), ImVec2(pos.x - 1.f, pos.y + headPosition.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//左上下|
			DrawLine(ImVec2(pos.x, pos.y + headPosition.y - (headPosition.y / Div) + 1.f), ImVec2(pos.x, pos.y + headPosition.y + 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//左上下|
			DrawLine(ImVec2(pos.x, pos.y + headPosition.y - (headPosition.y / Div) - 1.f), ImVec2(pos.x, pos.y + headPosition.y - 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//左上下|


			DrawLine(ImVec2(pos.x + 1.f, pos.y + headPosition.y), ImVec2(pos.x + (headPosition.x / Div) + 1.f, pos.y + headPosition.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//下左右
			DrawLine(ImVec2(pos.x - 1.f, pos.y + headPosition.y), ImVec2(pos.x + (headPosition.x / Div) - 1.f, pos.y + headPosition.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//下左右
			DrawLine(ImVec2(pos.x, pos.y + headPosition.y + 1.f), ImVec2(pos.x + (headPosition.x / Div), pos.y + headPosition.y + 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//下左右
			DrawLine(ImVec2(pos.x, pos.y + headPosition.y - 1.f), ImVec2(pos.x + (headPosition.x / Div), pos.y + headPosition.y - 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//下左右


			DrawLine(ImVec2(pos.x + headPosition.x - (headPosition.x / Div) + 1.f, pos.y + headPosition.y), ImVec2(pos.x + headPosition.x + 1.f, pos.y + headPosition.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//下左右
			DrawLine(ImVec2(pos.x + headPosition.x - (headPosition.x / Div) - 1.f, pos.y + headPosition.y), ImVec2(pos.x + headPosition.x - 1.f, pos.y + headPosition.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//下左右
			DrawLine(ImVec2(pos.x + headPosition.x - (headPosition.x / Div), pos.y + headPosition.y + 1.f), ImVec2(pos.x + headPosition.x, pos.y + headPosition.y + 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//下左右
			DrawLine(ImVec2(pos.x + headPosition.x - (headPosition.x / Div), pos.y + headPosition.y - 1.f), ImVec2(pos.x + headPosition.x, pos.y + headPosition.y - 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);//下左右


			DrawLine(ImVec2(pos.x + headPosition.x + 1.f, pos.y), ImVec2(pos.x + headPosition.x + 1.f, pos.y + (headPosition.y / Div)), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);
			DrawLine(ImVec2(pos.x + headPosition.x - 1.f, pos.y), ImVec2(pos.x + headPosition.x - 1.f, pos.y + (headPosition.y / Div)), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);
			DrawLine(ImVec2(pos.x + headPosition.x, pos.y + 1.f), ImVec2(pos.x + headPosition.x, pos.y + (headPosition.y / Div) + 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);
			DrawLine(ImVec2(pos.x + headPosition.x, pos.y - 1.f), ImVec2(pos.x + headPosition.x, pos.y + (headPosition.y / Div) - 1.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);


			DrawLine(ImVec2(pos.x + headPosition.x + 1.f, pos.y + headPosition.y - (headPosition.y / Div)), ImVec2(pos.x + headPosition.x + 1.f, pos.y + headPosition.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);
			DrawLine(ImVec2(pos.x + headPosition.x - 1.f, pos.y + headPosition.y - (headPosition.y / Div)), ImVec2(pos.x + headPosition.x - 1.f, pos.y + headPosition.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);
			DrawLine(ImVec2(pos.x + headPosition.x, pos.y + headPosition.y - (headPosition.y / Div) + 1.f), ImVec2(pos.x + headPosition.x, pos.y + headPosition.y + 2.f), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);
			DrawLine(ImVec2(pos.x + headPosition.x, pos.y + headPosition.y - (headPosition.y / Div) - 1.f), ImVec2(pos.x + headPosition.x, pos.y + headPosition.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), Siz);
		}

		DrawLine(ImVec2(pos.x, pos.y), ImVec2(pos.x + (headPosition.x / Div), pos.y), color, Siz);//上左右--
		DrawLine(ImVec2(pos.x + headPosition.x, pos.y), ImVec2(pos.x + headPosition.x - (headPosition.x / Div), pos.y), color, Siz);//上左右--

		DrawLine(ImVec2(pos.x, pos.y), ImVec2(pos.x, pos.y + (headPosition.y / Div)), color, Siz);//左上下|
		DrawLine(ImVec2(pos.x, pos.y + headPosition.y - (headPosition.y / Div)), ImVec2(pos.x, pos.y + headPosition.y), color, Siz);//左上下|

		DrawLine(ImVec2(pos.x, pos.y + headPosition.y), ImVec2(pos.x + (headPosition.x / Div), pos.y + headPosition.y), color, Siz);//下左右
		DrawLine(ImVec2(pos.x + headPosition.x - (headPosition.x / Div), pos.y + headPosition.y), ImVec2(pos.x + headPosition.x, pos.y + headPosition.y), color, Siz);//下左右

		DrawLine(ImVec2(pos.x + headPosition.x, pos.y), ImVec2(pos.x + headPosition.x, pos.y + (headPosition.y / Div)), color, Siz);
		DrawLine(ImVec2(pos.x + headPosition.x, pos.y + headPosition.y - (headPosition.y / Div)), ImVec2(pos.x + headPosition.x, pos.y + headPosition.y + 1), color, Siz);
	}

	void Drawing::DrawHealthBar(const ImVec2& pos, const ImVec2& headPosition, float Health, float HealthMax, bool IsDown, ImVec4 color,ImVec4 outlineColor)
	{
		ImColor outline = ImColor(0.0f, 0.0f, 0.0f, 0.3f);
		//DrawRectFilled(pos, ImVec2(pos.x + headPosition.x, pos.y + headPosition.y), 0x60050505, 3.f);
		DrawRectFilled(pos, ImVec2(pos.x + headPosition.x * (Health / HealthMax), pos.y + headPosition.y), ImGui::GetColorU32(!IsDown ? color : ImVec4(1.f, 0, 0, 1)), 5.f);
		//DrawOutlinedRect(pos - ImVec2(0.5f, 0.5f), ImVec2(pos.x + headPosition.x, pos.y + headPosition.y) + ImVec2(0.5f, 0.5f), outline, outline, 1.f, 5.f);
		DrawRect(pos - ImVec2(1.f, 1.f), pos + headPosition + ImVec2(1.f, 1.f), ImGui::GetColorU32(outlineColor), 1.f, 4.f);

	}

	float Drawing::DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, uint32_t color, bool center)
	{
		float a = (color >> 24) & 0xff;
		float r = (color >> 16) & 0xff;
		float g = (color >> 8) & 0xff;
		float b = (color) & 0xff;

		std::stringstream steam(text);
		std::string line;
		float y = 0.0f;
		int i = 0;

		while (std::getline(steam, line))
		{
			ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());
			if (center)
			{
				ImGui::GetOverlayDrawList()->AddText(pFont, size + 0.2f, ImVec2(pos.x - textSize.x / 2.0f - 1, pos.y + textSize.y * i - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, a / 255)), line.c_str());
				ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), line.c_str());
			}
			else
			{
				ImGui::GetOverlayDrawList()->AddText(pFont, size + 0.2f, ImVec2(pos.x - 1, pos.y + textSize.y * i - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, a / 255)), line.c_str());
				ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), line.c_str());
			}

			y = pos.y + textSize.y * (i + 1);
			i++;
		}

		return y;
	}
}