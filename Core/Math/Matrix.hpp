#pragma once
#include "Include/Macro.hpp"
#include <assert.h>

typedef float vec_t;
inline vec_t BitsToFloat(unsigned long i)
{
	return *reinterpret_cast<vec_t*>(&i);
}

#define FLOAT32_NAN_BITS     (unsigned long)0x7FC00000	// not a number!
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )
#define D3DX10INLINE __forceinline
#define VEC_T_NAN FLOAT32_NAN

namespace IronMan::Core::Math
{
	struct FVector;

	typedef struct _D3DMATRIX {

		union {
			struct {
				float        _11, _12, _13, _14;
				float        _21, _22, _23, _24;
				float        _31, _32, _33, _34;
				float        _41, _42, _43, _44;

			};
			float m[4][4];
		};
		__forceinline _D3DMATRIX operator*(const _D3DMATRIX& Other) const
		{
			_D3DMATRIX pOut;
			pOut._11 = _11 * Other._11 + _12 * Other._21 + _13 * Other._31 + _14 * Other._41;
			pOut._12 = _11 * Other._12 + _12 * Other._22 + _13 * Other._32 + _14 * Other._42;
			pOut._13 = _11 * Other._13 + _12 * Other._23 + _13 * Other._33 + _14 * Other._43;
			pOut._14 = _11 * Other._14 + _12 * Other._24 + _13 * Other._34 + _14 * Other._44;
			pOut._21 = _21 * Other._11 + _22 * Other._21 + _23 * Other._31 + _24 * Other._41;
			pOut._22 = _21 * Other._12 + _22 * Other._22 + _23 * Other._32 + _24 * Other._42;
			pOut._23 = _21 * Other._13 + _22 * Other._23 + _23 * Other._33 + _24 * Other._43;
			pOut._24 = _21 * Other._14 + _22 * Other._24 + _23 * Other._34 + _24 * Other._44;
			pOut._31 = _31 * Other._11 + _32 * Other._21 + _33 * Other._31 + _34 * Other._41;
			pOut._32 = _31 * Other._12 + _32 * Other._22 + _33 * Other._32 + _34 * Other._42;
			pOut._33 = _31 * Other._13 + _32 * Other._23 + _33 * Other._33 + _34 * Other._43;
			pOut._34 = _31 * Other._14 + _32 * Other._24 + _33 * Other._34 + _34 * Other._44;
			pOut._41 = _41 * Other._11 + _42 * Other._21 + _43 * Other._31 + _44 * Other._41;
			pOut._42 = _41 * Other._12 + _42 * Other._22 + _43 * Other._32 + _44 * Other._42;
			pOut._43 = _41 * Other._13 + _42 * Other._23 + _43 * Other._33 + _44 * Other._43;
			pOut._44 = _41 * Other._14 + _42 * Other._24 + _43 * Other._34 + _44 * Other._44;

			return pOut;
		}
		struct FVector ToVector3();


	} D3DMATRIX;



	//struct matrix3x4_t
	//{
	//	matrix3x4_t() {}
	//	matrix3x4_t(
	//		float m00, float m01, float m02, float m03,
	//		float m10, float m11, float m12, float m13,
	//		float m20, float m21, float m22, float m23)
	//	{
	//		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
	//		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
	//		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	//	}

	//	//-----------------------------------------------------------------------------
	//	// Creates a matrix where the X axis = forward
	//	// the Y axis = left, and the Z axis = up
	//	//-----------------------------------------------------------------------------
	//	void Init(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3 &vecOrigin)
	//	{
	//		m_flMatVal[0][0] = xAxis.X; m_flMatVal[0][1] = yAxis.X; m_flMatVal[0][2] = zAxis.X; m_flMatVal[0][3] = vecOrigin.X;
	//		m_flMatVal[1][0] = xAxis.Y; m_flMatVal[1][1] = yAxis.Y; m_flMatVal[1][2] = zAxis.Y; m_flMatVal[1][3] = vecOrigin.Y;
	//		m_flMatVal[2][0] = xAxis.Z; m_flMatVal[2][1] = yAxis.Z; m_flMatVal[2][2] = zAxis.X; m_flMatVal[2][3] = vecOrigin.Z;
	//	}

	//	//-----------------------------------------------------------------------------
	//	// Creates a matrix where the X axis = forward
	//	// the Y axis = left, and the Z axis = up
	//	//-----------------------------------------------------------------------------
	//	matrix3x4_t(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3 &vecOrigin)
	//	{
	//		Init(xAxis, yAxis, zAxis, vecOrigin);
	//	}

	//	inline void Invalidate(void)
	//	{
	//		for (int i = 0; i < 3; i++)
	//		{
	//			for (int j = 0; j < 4; j++)
	//			{
	//				m_flMatVal[i][j] = VEC_T_NAN;
	//			}
	//		}
	//	}

	//	float *operator[](int i) { assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	//	const float *operator[](int i) const { assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	//	float *Base() { return &m_flMatVal[0][0]; }
	//	const float *Base() const { return &m_flMatVal[0][0]; }

	//	float m_flMatVal[3][4];
	//};
}