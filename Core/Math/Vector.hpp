#pragma once
#include "Include/Macro.hpp"
#include "Matrix.hpp"
#include <math.h>
#include <cmath>
#include <float.h>
#include <emmintrin.h>
#include <PxPhysicsAPI.h>

#define MIN_flt			(1.175494351e-38F)			/* min positive value */
#define MAX_flt			(3.402823466e+38F)

#define M_PI		3.1415926535897932
#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#define SMALL_NUMBER		(1.e-8f)
// Aux constants.
#define INV_PI			(0.31830988618f)
#define HALF_PI			(1.57079632679f)

#define DEG2RAD(x)  ( (float)(x) * (float)(M_PI_F / 180.f) )
#define RAD2DEG(x)  ( (float)(x) * (float)(180.f / M_PI_F) )

#define CHECK_VALID( _v ) 0

#define KINDA_SMALL_NUMBER	(1.e-4f)
#define THRESH_VECTOR_NORMALIZED		(0.01f)		/** Allowed error for a normalized vector (against squared magnitude) */
#define THRESH_QUAT_NORMALIZED			(0.01f)		/** Allowed error for a normalized quaternion (against squared magnitude) */
typedef __m128	VectorRegister;
typedef __m128i VectorRegisterInt;
typedef __m128d VectorRegisterDouble;

/**
 * @param A0	Selects which element (0-3) from 'A' into 1st slot in the result
 * @param A1	Selects which element (0-3) from 'A' into 2nd slot in the result
 * @param B2	Selects which element (0-3) from 'B' into 3rd slot in the result
 * @param B3	Selects which element (0-3) from 'B' into 4th slot in the result
 */
#define SHUFFLEMASK(A0,A1,B2,B3) ( (A0) | ((A1)<<2) | ((B2)<<4) | ((B3)<<6) )

#define VectorLoadFloat3_W0( Ptr )		MakeVectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], 0.0f )
 /**
 * Loads XYZ and sets W=0
 *
 * @param Vector	VectorRegister
 * @return		VectorRegister(X, Y, Z, 0.0f)
 */
#define VectorSet_W0( Vec )		_mm_and_ps( Vec, Math::XYZMask )

 /**
  * Creates a vector out of four FLOATs.
  *
  * @param X		1st float component
  * @param Y		2nd float component
  * @param Z		3rd float component
  * @param W		4th float component
  * @return		VectorRegister(X, Y, Z, W)
  */
#define VectorLoadFloat4( Ptr )			MakeVectorRegister( ((const float*)(Ptr))[0], ((const float*)(Ptr))[1], ((const float*)(Ptr))[2], ((const float*)(Ptr))[3] )

  /**
   * Adds two vectors (component-wise) and returns the result.
   *
   * @param Vec1	1st vector
   * @param Vec2	2nd vector
   * @return		VectorRegister( Vec1.x+Vec2.x, Vec1.y+Vec2.y, Vec1.z+Vec2.z, Vec1.w+Vec2.w )
   */
#define VectorAdd( Vec1, Vec2 )			_mm_add_ps( Vec1, Vec2 )

   /**
	* Multiplies two vectors (component-wise), adds in the third vector and returns the result.
	*
	* @param Vec1	1st vector
	* @param Vec2	2nd vector
	* @param Vec3	3rd vector
	* @return		VectorRegister( Vec1.x*Vec2.x + Vec3.x, Vec1.y*Vec2.y + Vec3.y, Vec1.z*Vec2.z + Vec3.z, Vec1.w*Vec2.w + Vec3.w )
	*/
#define VectorMultiplyAdd( Vec1, Vec2, Vec3 )	_mm_add_ps( _mm_mul_ps(Vec1, Vec2), Vec3 )

	/**
	 * Subtracts a vector from another (component-wise) and returns the result.
	 *
	 * @param Vec1	1st vector
	 * @param Vec2	2nd vector
	 * @return		VectorRegister( Vec1.x-Vec2.x, Vec1.y-Vec2.y, Vec1.z-Vec2.z, Vec1.w-Vec2.w )
	 */
#define VectorSubtract( Vec1, Vec2 )	_mm_sub_ps( Vec1, Vec2 )

	 /**
	  * Multiplies two vectors (component-wise) and returns the result.
	  *
	  * @param Vec1	1st vector
	  * @param Vec2	2nd vector
	  * @return		VectorRegister( Vec1.x*Vec2.x, Vec1.y*Vec2.y, Vec1.z*Vec2.z, Vec1.w*Vec2.w )
	  */
#define VectorMultiply( Vec1, Vec2 )	_mm_mul_ps( Vec1, Vec2 )
	  /**
	   * Replicates one element into all four elements and returns the new vector.
	   *
	   * @param Vec			Source vector
	   * @param ElementIndex	Index (0-3) of the element to replicate
	   * @return				VectorRegister( Vec[ElementIndex], Vec[ElementIndex], Vec[ElementIndex], Vec[ElementIndex] )
	   */
#define VectorReplicate( Vec, ElementIndex )	_mm_shuffle_ps( Vec, Vec, SHUFFLEMASK(ElementIndex,ElementIndex,ElementIndex,ElementIndex) )



namespace IronMan::Core::Math
{
	/**
	 * Returns a bitwise equivalent vector based on 4 DWORDs.
	 *
	 * @param X		1st uint32 component
	 * @param Y		2nd uint32 component
	 * @param Z		3rd uint32 component
	 * @param W		4th uint32 component
	 * @return		Bitwise equivalent vector with 4 floats
	 */
	FORCEINLINE VectorRegister MakeVectorRegister(uint32_t X, uint32_t Y, uint32_t Z, uint32_t W)
	{
		union { VectorRegister v; VectorRegisterInt i; } Tmp;
		Tmp.i = _mm_setr_epi32(X, Y, Z, W);
		return Tmp.v;
	}

	/**
	 * Returns a vector based on 4 FLOATs.
	 *
	 * @param X		1st float component
	 * @param Y		2nd float component
	 * @param Z		3rd float component
	 * @param W		4th float component
	 * @return		Vector of the 4 FLOATs
	 */
	FORCEINLINE VectorRegister MakeVectorRegister(float X, float Y, float Z, float W)
	{
		return _mm_setr_ps(X, Y, Z, W);
	}


	/**
	* Returns a vector based on 4 int32.
	*
	* @param X		1st int32 component
	* @param Y		2nd int32 component
	* @param Z		3rd int32 component
	* @param W		4th int32 component
	* @return		Vector of the 4 int32
	*/
	FORCEINLINE VectorRegisterInt MakeVectorRegisterInt(int32_t X, int32_t Y, int32_t Z, int32_t W)
	{
		return _mm_setr_epi32(X, Y, Z, W);
	}

	/** Bitmask to AND out the XYZ components in a vector */
	static const VectorRegister XYZMask = MakeVectorRegister((uint32_t)0xffffffff, (uint32_t)0xffffffff, (uint32_t)0xffffffff, (uint32_t)0x00000000);
	/** This is to speed up Quaternion Inverse. Static variable to keep sign of inverse **/
	static const VectorRegister QINV_SIGN_MASK = MakeVectorRegister(-1.f, -1.f, -1.f, 1.f);



	/**
	 * Inverse quaternion ( -X, -Y, -Z, W)
	 */
	FORCEINLINE VectorRegister VectorQuaternionInverse(const VectorRegister& NormalizedQuat)
	{
		return VectorMultiply(Math::QINV_SIGN_MASK, NormalizedQuat);
	}

	/**
 * Calculates the cross product of two vectors (XYZ components). W is set to 0.
 *
 * @param Vec1	1st vector
 * @param Vec2	2nd vector
 * @return		cross(Vec1.xyz, Vec2.xyz). W is set to 0.
 */
	FORCEINLINE VectorRegister VectorCross(const VectorRegister& Vec1, const VectorRegister& Vec2)
	{
		VectorRegister A_YZXW = _mm_shuffle_ps(Vec1, Vec1, SHUFFLEMASK(1, 2, 0, 3));//C9
		VectorRegister B_ZXYW = _mm_shuffle_ps(Vec2, Vec2, SHUFFLEMASK(2, 0, 1, 3));//D2
		VectorRegister A_ZXYW = _mm_shuffle_ps(Vec1, Vec1, SHUFFLEMASK(2, 0, 1, 3));
		VectorRegister B_YZXW = _mm_shuffle_ps(Vec2, Vec2, SHUFFLEMASK(1, 2, 0, 3));
		return VectorSubtract(VectorMultiply(A_YZXW, B_ZXYW), VectorMultiply(A_ZXYW, B_YZXW));
	}

	/**
	 * Rotate a vector using a unit Quaternion.
	 *
	 * @param Quat Unit Quaternion to use for rotation.
	 * @param VectorW0 Vector to rotate. W component must be zero.
	 * @return Vector after rotation by Quat.
	 */
	FORCEINLINE VectorRegister VectorQuaternionRotateVector(const VectorRegister& Quat, const VectorRegister& VectorW0)
	{
		// Q * V * Q.Inverse
		//const VectorRegister InverseRotation = VectorQuaternionInverse(Quat);
		//const VectorRegister Temp = VectorQuaternionMultiply2(Quat, VectorW0);
		//const VectorRegister Rotated = VectorQuaternionMultiply2(Temp, InverseRotation);

		// Equivalence of above can be shown to be:
		// http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
		// V' = V + 2w(Q x V) + (2Q x (Q x V))
		// refactor:
		// V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
		// T = 2(Q x V);
		// V' = V + w*(T) + (Q x T)

		//constexpr auto aa = SHUFFLEMASK(2, 0, 1, 3);

		const VectorRegister QW = VectorReplicate(Quat, 3);//0xFF
		VectorRegister T = VectorCross(Quat, VectorW0);
		T = VectorAdd(T, T);
		const VectorRegister VTemp0 = VectorMultiplyAdd(QW, T, VectorW0);
		const VectorRegister VTemp1 = VectorCross(Quat, T);
		const VectorRegister Rotated = VectorAdd(VTemp0, VTemp1);
		return Rotated;
	}

	/**
	 * Rotate a vector using the inverse of a unit Quaternion (rotation in the opposite direction).
	 *
	 * @param Quat Unit Quaternion to use for rotation.
	 * @param VectorW0 Vector to rotate. W component must be zero.
	 * @return Vector after rotation by the inverse of Quat.
	 */
	FORCEINLINE VectorRegister VectorQuaternionInverseRotateVector(const VectorRegister& Quat, const VectorRegister& VectorW0)
	{
		// Q.Inverse * V * Q
		//const VectorRegister InverseRotation = VectorQuaternionInverse(Quat);
		//const VectorRegister Temp = VectorQuaternionMultiply2(InverseRotation, VectorW0);
		//const VectorRegister Rotated = VectorQuaternionMultiply2(Temp, Quat);

		const VectorRegister QInv = VectorQuaternionInverse(Quat);
		return VectorQuaternionRotateVector(QInv, VectorW0);
	}

	/**
	 * Stores the XYZ components of a vector to unaligned memory.
	 *
	 * @param Vec	Vector to store XYZ
	 * @param Ptr	Unaligned memory pointer
	 */
	FORCEINLINE void VectorStoreFloat3(const VectorRegister& Vec, void* Ptr)
	{
		union { VectorRegister v; float f[4]; } Tmp;
		Tmp.v = Vec;
		float* FloatPtr = (float*)(Ptr);
		FloatPtr[0] = Tmp.f[0];
		FloatPtr[1] = Tmp.f[1];
		FloatPtr[2] = Tmp.f[2];
	}


	// Generic axis enum (mirrored for property use in Object.h)
	namespace EAxis
	{
		enum Type
		{
			None,
			X,
			Y,
			Z,
		};
	}

	/**
 * Converts a float to an integer with truncation towards zero.
 * @param F		Floating point value to convert
 * @return		Truncated integer.
 */
	static constexpr FORCEINLINE int32_t TruncToInt(float F)
	{
		return (int32_t)F;
	}

	/**
	 * Converts a float to an integer value with truncation towards zero.
	 * @param F		Floating point value to convert
	 * @return		Truncated integer value.
	 */
	static constexpr FORCEINLINE float TruncToFloat(float F)
	{
		return (float)TruncToInt(F);
	}

	/** Multiples value by itself */
	template< class T >
	static FORCEINLINE T Square(const T A)
	{
		return A * A;
	}

	/** Computes a fully accurate inverse square root */
	static FORCEINLINE float InvSqrt(float F)
	{
		/*return 1.0f / sqrtf(F);*/

		// Performs two passes of Newton-Raphson iteration on the hardware estimate
		//    v^-0.5 = x
		// => x^2 = v^-1
		// => 1/(x^2) = v
		// => F(x) = x^-2 - v
		//    F'(x) = -2x^-3

		//    x1 = x0 - F(x0)/F'(x0)
		// => x1 = x0 + 0.5 * (x0^-2 - Vec) * x0^3
		// => x1 = x0 + 0.5 * (x0 - Vec * x0^3)
		// => x1 = x0 + x0 * (0.5 - 0.5 * Vec * x0^2)
		//
		// This final form has one more operation than the legacy factorization (X1 = 0.5*X0*(3-(Y*X0)*X0)
		// but retains better accuracy (namely InvSqrt(1) = 1 exactly).

		const __m128 fOneHalf = _mm_set_ss(0.5f);
		__m128 Y0, X0, X1, X2, FOver2;
		float temp;

		Y0 = _mm_set_ss(F);
		X0 = _mm_rsqrt_ss(Y0);	// 1/sqrt estimate (12 bits)
		FOver2 = _mm_mul_ss(Y0, fOneHalf);

		// 1st Newton-Raphson iteration
		X1 = _mm_mul_ss(X0, X0);
		X1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X1));
		X1 = _mm_add_ss(X0, _mm_mul_ss(X0, X1));

		// 2nd Newton-Raphson iteration
		X2 = _mm_mul_ss(X1, X1);
		X2 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X2));
		X2 = _mm_add_ss(X1, _mm_mul_ss(X1, X2));

		_mm_store_ss(&temp, X2);
		return temp;
	}

	/** Computes a faster but less accurate inverse square root */
	static FORCEINLINE float InvSqrtEst(float F)
	{
		return InvSqrt(F);
	}

	static FORCEINLINE float Sqrt(float Value) { return sqrtf(Value); }

	/**
	 * Returns value based on comparand. The main purpose of this function is to avoid
	 * branching based on floating point comparison which can be avoided via compiler
	 * intrinsics.
	 *
	 * Please note that we don't define what happens in the case of NaNs as there might
	 * be platform specific differences.
	 *
	 * @param	Comparand		Comparand the results are based on
	 * @param	ValueGEZero		Return value if Comparand >= 0
	 * @param	ValueLTZero		Return value if Comparand < 0
	 *
	 * @return	ValueGEZero if Comparand >= 0, ValueLTZero otherwise
	 */
	static constexpr FORCEINLINE float FloatSelect(float Comparand, float ValueGEZero, float ValueLTZero)
	{
		return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
	}

	/**
	 * Returns value based on comparand. The main purpose of this function is to avoid
	 * branching based on floating point comparison which can be avoided via compiler
	 * intrinsics.
	 *
	 * Please note that we don't define what happens in the case of NaNs as there might
	 * be platform specific differences.
	 *
	 * @param	Comparand		Comparand the results are based on
	 * @param	ValueGEZero		Return value if Comparand >= 0
	 * @param	ValueLTZero		Return value if Comparand < 0
	 *
	 * @return	ValueGEZero if Comparand >= 0, ValueLTZero otherwise
	 */
	static constexpr FORCEINLINE double FloatSelect(double Comparand, double ValueGEZero, double ValueLTZero)
	{
		return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
	}

	/**
	 * Converts degrees to radians.
	 * @param	DegVal			Value in degrees.
	 * @return					Value in radians.
	 */
	template<class T>
	static FORCEINLINE auto DegreesToRadians(T const& DegVal) -> decltype(DegVal* (PI / 180.f))
	{
		return DegVal * (PI / 180.f);
	}

	static FORCEINLINE int32_t FloorToInt(float F)
	{
		return _mm_cvt_ss2si(_mm_set_ss(F + F - 0.5f)) >> 1;
	}

	/** Computes absolute value in a generic way */
	template< class T >
	static constexpr FORCEINLINE T Abs(const T A)
	{
		return (A >= (T)0) ? A : -A;
	}

	/**
	 *	Checks if a floating point number is nearly zero.
	 *	@param Value			Number to compare
	 *	@param ErrorTolerance	Maximum allowed difference for considering Value as 'nearly zero'
	 *	@return					true if Value is nearly zero
	 */
	static FORCEINLINE bool IsNearlyZero(float Value, float ErrorTolerance = SMALL_NUMBER)
	{
		return Abs<float>(Value) <= ErrorTolerance;
	}

	/** Performs a linear interpolation between two values, Alpha ranges from 0-1 */
	template< class T, class U >
	static FORCEINLINE T Lerp(const T& A, const T& B, const U& Alpha)
	{
		return (T)(A + Alpha * (B - A));
	}

	static FORCEINLINE void SinCos(float* ScalarSin, float* ScalarCos, float  Value)
	{
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float quotient = (INV_PI * 0.5f) * Value;
		if (Value >= 0.0f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient - 0.5f));
		}
		float y = Value - (2.0f * PI) * quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > HALF_PI)
		{
			y = PI - y;
			sign = -1.0f;
		}
		else if (y < -HALF_PI)
		{
			y = -PI - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float y2 = y * y;

		// 11-degree minimax approximation
		*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*ScalarCos = sign * p;
	}

	/**
	 * Multiplies two 4x4 matrices.
	 *
	 * @param Result	Pointer to where the result should be stored
	 * @param Matrix1	Pointer to the first matrix
	 * @param Matrix2	Pointer to the second matrix
	 */
	FORCEINLINE void VectorMatrixMultiply(void* Result, const void* Matrix1, const void* Matrix2)
	{
		typedef float Float4x4[4][4];
		const Float4x4& A = *((const Float4x4*)Matrix1);
		const Float4x4& B = *((const Float4x4*)Matrix2);
		Float4x4 Temp;
		Temp[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0] + A[0][3] * B[3][0];
		Temp[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1] + A[0][3] * B[3][1];
		Temp[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2] + A[0][3] * B[3][2];
		Temp[0][3] = A[0][0] * B[0][3] + A[0][1] * B[1][3] + A[0][2] * B[2][3] + A[0][3] * B[3][3];

		Temp[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0] + A[1][3] * B[3][0];
		Temp[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1] + A[1][3] * B[3][1];
		Temp[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2] + A[1][3] * B[3][2];
		Temp[1][3] = A[1][0] * B[0][3] + A[1][1] * B[1][3] + A[1][2] * B[2][3] + A[1][3] * B[3][3];

		Temp[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0] + A[2][3] * B[3][0];
		Temp[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1] + A[2][3] * B[3][1];
		Temp[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2] + A[2][3] * B[3][2];
		Temp[2][3] = A[2][0] * B[0][3] + A[2][1] * B[1][3] + A[2][2] * B[2][3] + A[2][3] * B[3][3];

		Temp[3][0] = A[3][0] * B[0][0] + A[3][1] * B[1][0] + A[3][2] * B[2][0] + A[3][3] * B[3][0];
		Temp[3][1] = A[3][0] * B[0][1] + A[3][1] * B[1][1] + A[3][2] * B[2][1] + A[3][3] * B[3][1];
		Temp[3][2] = A[3][0] * B[0][2] + A[3][1] * B[1][2] + A[3][2] * B[2][2] + A[3][3] * B[3][2];
		Temp[3][3] = A[3][0] * B[0][3] + A[3][1] * B[1][3] + A[3][2] * B[2][3] + A[3][3] * B[3][3];
		memcpy(Result, &Temp, 16 * sizeof(float));
	}

	/**
	 * Multiplies two quaternions; the order matters.
	 *
	 * When composing quaternions: VectorQuaternionMultiply(C, A, B) will yield a quaternion C = A * B
	 * that logically first applies B then A to any subsequent transformation (right first, then left).
	 *
	 * @param Result	Pointer to where the result Quat1 * Quat2 should be stored
	 * @param Quat1	Pointer to the first quaternion (must not be the destination)
	 * @param Quat2	Pointer to the second quaternion (must not be the destination)
	 */
	FORCEINLINE void VectorQuaternionMultiply(void* Result, const void* Quat1, const void* Quat2)
	{
		typedef float Float4[4];
		const Float4& A = *((const Float4*)Quat1);
		const Float4& B = *((const Float4*)Quat2);
		Float4& R = *((Float4*)Result);

#if USE_FAST_QUAT_MUL
		const float T0 = (A[2] - A[1]) * (B[1] - B[2]);
		const float T1 = (A[3] + A[0]) * (B[3] + B[0]);
		const float T2 = (A[3] - A[0]) * (B[1] + B[2]);
		const float T3 = (A[1] + A[2]) * (B[3] - B[0]);
		const float T4 = (A[2] - A[0]) * (B[0] - B[1]);
		const float T5 = (A[2] + A[0]) * (B[0] + B[1]);
		const float T6 = (A[3] + A[1]) * (B[3] - B[2]);
		const float T7 = (A[3] - A[1]) * (B[3] + B[2]);
		const float T8 = T5 + T6 + T7;
		const float T9 = 0.5f * (T4 + T8);

		R[0] = T1 + T9 - T8;
		R[1] = T2 + T9 - T7;
		R[2] = T3 + T9 - T6;
		R[3] = T0 + T9 - T5;
#else
		// store intermediate results in temporaries
		const float TX = A[3] * B[0] + A[0] * B[3] + A[1] * B[2] - A[2] * B[1];
		const float TY = A[3] * B[1] - A[0] * B[2] + A[1] * B[3] + A[2] * B[0];
		const float TZ = A[3] * B[2] + A[0] * B[1] - A[1] * B[0] + A[2] * B[3];
		const float TW = A[3] * B[3] - A[0] * B[0] - A[1] * B[1] - A[2] * B[2];

		// copy intermediate result to *this
		R[0] = TX;
		R[1] = TY;
		R[2] = TZ;
		R[3] = TW;
#endif
	}

	/**
 * Calculate the inverse of an FMatrix.
 *
 * @param DstMatrix		FMatrix pointer to where the result should be stored
 * @param SrcMatrix		FMatrix pointer to the Matrix to be inversed
 */
	FORCEINLINE void VectorMatrixInverse(void* DstMatrix, const void* SrcMatrix)
	{
		typedef float Float4x4[4][4];
		const Float4x4& M = *((const Float4x4*)SrcMatrix);
		Float4x4 Result;
		float Det[4];
		Float4x4 Tmp;

		Tmp[0][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
		Tmp[0][1] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
		Tmp[0][2] = M[1][2] * M[2][3] - M[1][3] * M[2][2];

		Tmp[1][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
		Tmp[1][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
		Tmp[1][2] = M[0][2] * M[2][3] - M[0][3] * M[2][2];

		Tmp[2][0] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
		Tmp[2][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
		Tmp[2][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

		Tmp[3][0] = M[1][2] * M[2][3] - M[1][3] * M[2][2];
		Tmp[3][1] = M[0][2] * M[2][3] - M[0][3] * M[2][2];
		Tmp[3][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

		Det[0] = M[1][1] * Tmp[0][0] - M[2][1] * Tmp[0][1] + M[3][1] * Tmp[0][2];
		Det[1] = M[0][1] * Tmp[1][0] - M[2][1] * Tmp[1][1] + M[3][1] * Tmp[1][2];
		Det[2] = M[0][1] * Tmp[2][0] - M[1][1] * Tmp[2][1] + M[3][1] * Tmp[2][2];
		Det[3] = M[0][1] * Tmp[3][0] - M[1][1] * Tmp[3][1] + M[2][1] * Tmp[3][2];

		float Determinant = M[0][0] * Det[0] - M[1][0] * Det[1] + M[2][0] * Det[2] - M[3][0] * Det[3];
		const float	RDet = 1.0f / Determinant;

		Result[0][0] = RDet * Det[0];
		Result[0][1] = -RDet * Det[1];
		Result[0][2] = RDet * Det[2];
		Result[0][3] = -RDet * Det[3];
		Result[1][0] = -RDet * (M[1][0] * Tmp[0][0] - M[2][0] * Tmp[0][1] + M[3][0] * Tmp[0][2]);
		Result[1][1] = RDet * (M[0][0] * Tmp[1][0] - M[2][0] * Tmp[1][1] + M[3][0] * Tmp[1][2]);
		Result[1][2] = -RDet * (M[0][0] * Tmp[2][0] - M[1][0] * Tmp[2][1] + M[3][0] * Tmp[2][2]);
		Result[1][3] = RDet * (M[0][0] * Tmp[3][0] - M[1][0] * Tmp[3][1] + M[2][0] * Tmp[3][2]);
		Result[2][0] = RDet * (
			M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
			M[2][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) +
			M[3][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1])
			);
		Result[2][1] = -RDet * (
			M[0][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
			M[2][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
			M[3][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1])
			);
		Result[2][2] = RDet * (
			M[0][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) -
			M[1][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
			M[3][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
			);
		Result[2][3] = -RDet * (
			M[0][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) -
			M[1][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1]) +
			M[2][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
			);
		Result[3][0] = -RDet * (
			M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
			M[2][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) +
			M[3][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
			);
		Result[3][1] = RDet * (
			M[0][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
			M[2][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
			M[3][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1])
			);
		Result[3][2] = -RDet * (
			M[0][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) -
			M[1][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
			M[3][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
			);
		Result[3][3] = RDet * (
			M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
			M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
			M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
			);

		memcpy(DstMatrix, &Result, 16 * sizeof(float));
	}

	FORCEINLINE float Atan2(float Y, float X)
	{
		//return atan2f(Y,X);
		// atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
		// We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to the C library function.
		// On PC this has been measured to be 2x faster than the std C version.

		const float absX = Math::Abs(X);
		const float absY = Math::Abs(Y);
		const bool yAbsBigger = (absY > absX);
		float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
		float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

		if (t0 == 0.f)
			return 0.f;

		float t3 = t1 / t0;
		float t4 = t3 * t3;

		static const float c[7] = {
			+7.2128853633444123e-03f,
			-3.5059680836411644e-02f,
			+8.1675882859940430e-02f,
			-1.3374657325451267e-01f,
			+1.9856563505717162e-01f,
			-3.3324998579202170e-01f,
			+1.0f
		};

		t0 = c[0];
		t0 = t0 * t4 + c[1];
		t0 = t0 * t4 + c[2];
		t0 = t0 * t4 + c[3];
		t0 = t0 * t4 + c[4];
		t0 = t0 * t4 + c[5];
		t0 = t0 * t4 + c[6];
		t3 = t0 * t3;

		t3 = yAbsBigger ? (0.5f * PI) - t3 : t3;
		t3 = (X < 0.0f) ? PI - t3 : t3;
		t3 = (Y < 0.0f) ? -t3 : t3;

		return t3;
	}

	static FORCEINLINE float Fmod(float X, float Y)
	{
		if (fabsf(Y) <= 1.e-8f)
		{
			//FmodReportError(X, Y);
			return 0.f;
		}
		const float Quotient = TruncToFloat(X / Y);
		float IntPortion = Y * Quotient;

		// Rounding and imprecision could cause IntPortion to exceed X and cause the result to be outside the expected range.
		// For example Fmod(55.8, 9.3) would result in a very small negative value!
		if (fabsf(IntPortion) > fabsf(X))
		{
			IntPortion = X;
		}

		const float Result = X - IntPortion;
		return Result;
	}

#define FASTASIN_HALF_PI (1.5707963050f)
	/**
	* Computes the ASin of a scalar float.
	*
	* @param Value  input angle
	* @return ASin of Value
	*/
	static FORCEINLINE float FastAsin(float Value)
	{
		// Clamp input to [-1,1].
		bool nonnegative = (Value >= 0.0f);
		float x = Math::Abs(Value);
		float omx = 1.0f - x;
		if (omx < 0.0f)
		{
			omx = 0.0f;
		}
		float root = Math::Sqrt(omx);
		// 7-degree minimax approximation
		float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
		result *= root;  // acos(|x|)
		// acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
		return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
	}
#undef FASTASIN_HALF_PI
	struct FQuat;

	// ScriptStruct CoreUObject.Vector4
	// 0x0010
	struct alignas(16) FVector4
	{
		float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              W;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	};

	struct FVector
	{
	public:
		float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

		inline FVector()
			: X(0), Y(0), Z(0) {
		}
		inline FVector(float f)
			: X(f), Y(f), Z(f) {
		}

		inline FVector(float x, float y, float z)
			: X(x),
			Y(y),
			Z(z) {
		}

		__forceinline FVector FVector::operator-(const FVector& V) {
			return FVector(X - V.X, Y - V.Y, Z - V.Z);
		}

		__forceinline FVector FVector::operator-(const FVector& V) const {
			return FVector(X - V.X, Y - V.Y, Z - V.Z);
		}

		__forceinline FVector FVector::operator+(const FVector& V) {
			return FVector(X + V.X, Y + V.Y, Z + V.Z);
		}
		__forceinline FVector FVector::operator+(const FVector& V) const {
			return FVector(X + V.X, Y + V.Y, Z + V.Z);
		}


		__forceinline FVector FVector::operator*(float Scale) const {
			return FVector(X * Scale, Y * Scale, Z * Scale);
		}

		__forceinline FVector FVector::operator/(float Scale) const {
			const float RScale = 1.f / Scale;
			return FVector(X * RScale, Y * RScale, Z * RScale);
		}

		__forceinline FVector FVector::operator+(float A) const {
			return FVector(X + A, Y + A, Z + A);
		}

		__forceinline FVector FVector::operator-(float A) const {
			return FVector(X - A, Y - A, Z - A);
		}

		__forceinline FVector FVector::operator*(const FVector& V) const {
			return FVector(X * V.X, Y * V.Y, Z * V.Z);
		}

		__forceinline FVector FVector::operator/(const FVector& V) const {
			return FVector(X / V.X, Y / V.Y, Z / V.Z);
		}

		__forceinline float FVector::operator|(const FVector& V) const {
			return X * V.X + Y * V.Y + Z * V.Z;
		}

		__forceinline FVector FVector::operator^(const FVector& V) const {
			return FVector
			(
				Y * V.Z - Z * V.Y,
				Z * V.X - X * V.Z,
				X * V.Y - Y * V.X
			);
		}

		__forceinline FVector& FVector::operator+=(const FVector& v) {
			CHECK_VALID(*this);
			CHECK_VALID(v);
			X += v.X;
			Y += v.Y;
			Z += v.Z;
			return *this;
		}

		__forceinline FVector& FVector::operator-=(const FVector& v) {
			CHECK_VALID(*this);
			CHECK_VALID(v);
			X -= v.X;
			Y -= v.Y;
			Z -= v.Z;
			return *this;
		}

		__forceinline FVector& FVector::operator*=(const FVector& v) {
			CHECK_VALID(*this);
			CHECK_VALID(v);
			X *= v.X;
			Y *= v.Y;
			Z *= v.Z;
			return *this;
		}

		__forceinline FVector& FVector::operator/=(const FVector& v) {
			CHECK_VALID(*this);
			CHECK_VALID(v);
			X /= v.X;
			Y /= v.Y;
			Z /= v.Z;
			return *this;
		}

		__forceinline bool FVector::operator==(const FVector& src) const {
			CHECK_VALID(src);
			CHECK_VALID(*this);
			return (src.X == X) && (src.Y == Y) && (src.Z == Z);
		}

		__forceinline bool FVector::operator!=(const FVector& src) const {
			CHECK_VALID(src);
			CHECK_VALID(*this);
			return (src.X != X) || (src.Y != Y) || (src.Z != Z);
		}

		__forceinline float FVector::Size() const {
			return sqrt(X * X + Y * Y + Z * Z);
		}

		__forceinline float FVector::Size2D() const {
			return sqrt(X * X + Y * Y);
		}

		__forceinline float FVector::SizeSquared() const {
			return X * X + Y * Y + Z * Z;
		}

		__forceinline float FVector::SizeSquared2D() const {
			return X * X + Y * Y;
		}

		__forceinline float FVector::Dot(const FVector& vOther) const {
			const FVector& a = *this;

			return (a.X * vOther.X + a.Y * vOther.Y + a.Z * vOther.Z);
		}

		/**
		 * Normalize this vector in-place if it is larger than a given tolerance. Leaves it unchanged if not.
		 *
		 * @param Tolerance Minimum squared length of vector for normalization.
		 * @return true if the vector was normalized correctly, false otherwise.
		 */
		bool Normalize(float Tolerance = SMALL_NUMBER);
		/**
		 * Calculates normalized version of vector without checking for zero length.
		 *
		 * @return Normalized version of vector.
		 * @see GetSafeNormal()
		 */
		FORCEINLINE FVector GetUnsafeNormal() const;

		/**
		 * Gets a normalized copy of the vector, checking it is safe to do so based on the length.
		 * Returns zero vector if vector length is too small to safely normalize.
		 *
		 * @param Tolerance Minimum squared vector length.
		 * @return A normalized copy if safe, (0,0,0) otherwise.
		 */
		FVector GetSafeNormal(float Tolerance = SMALL_NUMBER) const;

		/**
		 * Gets a normalized copy of the 2D components of the vector, checking it is safe to do so. Z is set to zero.
		 * Returns zero vector if vector length is too small to normalize.
		 *
		 * @param Tolerance Minimum squared vector length.
		 * @return Normalized copy if safe, otherwise returns zero vector.
		 */
		FVector GetSafeNormal2D(float Tolerance = SMALL_NUMBER) const;

		__forceinline FVector FVector::Normalized() {
			FVector vector;
			float length = this->Size();

			if (length != 0) {
				vector.X = X * (1.0f / length);
				vector.Y = Y * (1.0f / length);
				vector.Z = Z * (1.0f / length);
			}
			else
			{
				vector.X = vector.Y = 0.0f;
				vector.Z = 1.0f;
			}
			return vector;
		}

		__forceinline float Distance(const FVector& vOther) const
		{
			return float(sqrtf(powf(vOther.X - X, 2.0) + powf(vOther.Y - Y, 2.0) + powf(vOther.Z - Z, 2.0)));
		}
		__forceinline bool IsNumber() const {
			return X == X && Y == Y && Z == Z;
		}
		__forceinline bool IsFiniteNumber() const {
			return (X <= DBL_MAX && X >= -DBL_MAX) && (Y <= DBL_MAX && Y >= -DBL_MAX) && (Z <= DBL_MAX && Z >= -DBL_MAX);
		}
		__forceinline bool IsVaild() const {
			return IsNumber() && IsFiniteNumber();
		}
		__forceinline bool IsZero()
		{
			if (this->X == 0.f && this->Y == 0.f && this->Z == 0.f)
				return true;

			return false;
		}

		__forceinline bool HasZero()
		{
			if (this->X == 0.f || this->Y == 0.f || this->Z == 0.f)
				return true;

			return false;
		}

		/**
		 * Checks whether vector is near to zero within a specified tolerance.
		 *
		 * @param Tolerance Error tolerance.
		 * @return true if the vector is near to zero, false otherwise.
		 */
		bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;

		/**
		 * Get a copy of the vector as sign only.
		 * Each component is set to +1 or -1, with the sign of zero treated as +1.
		 *
		 * @param A copy of the vector with each component set to +1 or -1
		 */
		FORCEINLINE FVector GetSignVector() const;

		/**
		 * Calculate the cross product of two vectors.
		 *
		 * @param A The first vector.
		 * @param B The second vector.
		 * @return The cross product.
		 */
		FORCEINLINE static FVector CrossProduct(const FVector& A, const FVector& B);


		/**
		 * Calculate the dot product of two vectors.
		 *
		 * @param A The first vector.
		 * @param B The second vector.
		 * @return The dot product.
		 */
		FORCEINLINE static float DotProduct(const FVector& A, const FVector& B);

		/** A zero vector (0,0,0) */
		static const FVector ZeroVector;

		/** One vector (1,1,1) */
		static const FVector OneVector;

	};

	FORCEINLINE bool FVector::IsNearlyZero(float Tolerance) const
	{
		return
			Math::Abs(X) <= Tolerance
			&& Math::Abs(Y) <= Tolerance
			&& Math::Abs(Z) <= Tolerance;
	}

	FORCEINLINE FVector FVector::GetSignVector() const
	{
		return FVector
		(
			Math::FloatSelect(X, 1.f, -1.f),
			Math::FloatSelect(Y, 1.f, -1.f),
			Math::FloatSelect(Z, 1.f, -1.f)
		);
	}

	FORCEINLINE FVector FVector::CrossProduct(const FVector& A, const FVector& B)
	{
		return A ^ B;
	}

	FORCEINLINE float FVector::DotProduct(const FVector& A, const FVector& B)
	{
		return A | B;
	}


	FORCEINLINE bool FVector::Normalize(float Tolerance)
	{
		const float SquareSum = X * X + Y * Y + Z * Z;
		if (SquareSum > Tolerance)
		{
			const float Scale = Math::InvSqrt(SquareSum);
			X *= Scale; Y *= Scale; Z *= Scale;
			return true;
		}
		return false;
	}

	FORCEINLINE FVector FVector::GetUnsafeNormal() const
	{
		const float Scale = Math::InvSqrt(X * X + Y * Y + Z * Z);
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}

	FORCEINLINE FVector FVector::GetSafeNormal(float Tolerance) const
	{
		const float SquareSum = X * X + Y * Y + Z * Z;

		// Not sure if it's safe to add tolerance in there. Might introduce too many errors
		if (SquareSum == 1.f)
		{
			return *this;
		}
		else if (SquareSum < Tolerance)
		{
			return FVector::ZeroVector;
		}
		const float Scale = Math::InvSqrt(SquareSum);
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}

	FORCEINLINE FVector FVector::GetSafeNormal2D(float Tolerance) const
	{
		const float SquareSum = X * X + Y * Y;

		// Not sure if it's safe to add tolerance in there. Might introduce too many errors
		if (SquareSum == 1.f)
		{
			if (Z == 0.f)
			{
				return *this;
			}
			else
			{
				return FVector(X, Y, 0.f);
			}
		}
		else if (SquareSum < Tolerance)
		{
			return FVector::ZeroVector;
		}

		const float Scale = Math::InvSqrt(SquareSum);
		return FVector(X * Scale, Y * Scale, 0.f);
	}

	// ScriptStruct CoreUObject.Vector2D
	// 0x0008
	struct FVector2D
	{
		float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

		inline FVector2D()
			: X(0), Y(0) {
		}

		inline FVector2D(float x, float y)
			: X(x),
			Y(y) {
		}
		inline FVector2D(float value)
			: X(value),
			Y(value) {
		}

		__forceinline FVector2D FVector2D::operator-(const FVector2D& V) {
			return FVector2D(X - V.X, Y - V.Y);
		}

		__forceinline FVector2D FVector2D::operator+(const FVector2D& V) {
			return FVector2D(X + V.X, Y + V.Y);
		}

		__forceinline FVector2D FVector2D::operator-(const FVector2D& V) const {
			return FVector2D(X - V.X, Y - V.Y);
		}

		__forceinline FVector2D FVector2D::operator+(const FVector2D& V) const {
			return FVector2D(X + V.X, Y + V.Y);
		}



		__forceinline FVector2D FVector2D::operator*(float Scale) const {
			return FVector2D(X * Scale, Y * Scale);
		}

		__forceinline FVector2D FVector2D::operator/(float Scale) const {
			const float RScale = 1.f / Scale;
			return FVector2D(X * RScale, Y * RScale);
		}

		__forceinline FVector2D FVector2D::operator+(float A) const {
			return FVector2D(X + A, Y + A);
		}

		__forceinline FVector2D FVector2D::operator+(float A) {
			return FVector2D(X + A, Y + A);
		}

		__forceinline FVector2D FVector2D::operator-(float A) const {
			return FVector2D(X - A, Y - A);
		}

		__forceinline FVector2D FVector2D::operator*(const FVector2D& V) const {
			return FVector2D(X * V.X, Y * V.Y);
		}

		__forceinline FVector2D FVector2D::operator/(const FVector2D& V) const {
			return FVector2D(X / V.X, Y / V.Y);
		}

		__forceinline float FVector2D::operator|(const FVector2D& V) const {
			return X * V.X + Y * V.Y;
		}

		__forceinline float FVector2D::operator^(const FVector2D& V) const {
			return X * V.Y - Y * V.X;
		}

		__forceinline FVector2D& FVector2D::operator+=(const FVector2D& v) {
			CHECK_VALID(*this);
			CHECK_VALID(v);
			X += v.X;
			Y += v.Y;
			return *this;
		}

		__forceinline FVector2D& FVector2D::operator-=(const FVector2D& v) {
			CHECK_VALID(*this);
			CHECK_VALID(v);
			X -= v.X;
			Y -= v.Y;
			return *this;
		}

		__forceinline FVector2D& FVector2D::operator*=(const FVector2D& v) {
			CHECK_VALID(*this);
			CHECK_VALID(v);
			X *= v.X;
			Y *= v.Y;
			return *this;
		}

		__forceinline FVector2D& FVector2D::operator/=(const FVector2D& v) {
			CHECK_VALID(*this);
			CHECK_VALID(v);
			X /= v.X;
			Y /= v.Y;
			return *this;
		}

		__forceinline bool FVector2D::operator==(const FVector2D& src) const {
			CHECK_VALID(src);
			CHECK_VALID(*this);
			return (src.X == X) && (src.Y == Y);
		}

		__forceinline bool FVector2D::operator!=(const FVector2D& src) const {
			CHECK_VALID(src);
			CHECK_VALID(*this);
			return (src.X != X) || (src.Y != Y);
		}

		__forceinline float FVector2D::Size() const {
			return sqrt(X * X + Y * Y);
		}

		__forceinline float FVector2D::SizeSquared() const {
			return X * X + Y * Y;
		}

		__forceinline float FVector2D::Dot(const FVector2D& vOther) const {
			const FVector2D& a = *this;

			return (a.X * vOther.X + a.Y * vOther.Y);
		}
		__forceinline float FVector2D::Distance(const FVector2D& vOther) const {

			return float(sqrtf(powf(vOther.X - X, 2.0) + powf(vOther.Y - Y, 2.0)));
		}
		__forceinline bool FVector2D::IsNumber() const {
			return X == X && Y == Y;
		}
		__forceinline bool FVector2D::IsFiniteNumber() const {
			return (X <= DBL_MAX && X >= -DBL_MAX) && (Y <= DBL_MAX && Y >= -DBL_MAX);
		}
		__forceinline bool FVector2D::IsVaild() const {
			return IsNumber() && IsFiniteNumber();
		}
		__forceinline FVector2D FVector2D::Normalize() {
			FVector2D vector;
			float length = this->Size();

			if (length != 0) {
				vector.X = X / length;
				vector.Y = Y / length;
			}
			else
				vector.X = vector.Y = 0.0f;

			return vector;
		}
	};





	// 0x000C
	struct FRotator
	{
		float                                              Pitch;                                                    // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              Yaw;                                                      // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              Roll;                                                     // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

		inline FRotator(float f)
			: Pitch(f), Yaw(f), Roll(f) {
		}

		inline FRotator()
			: Pitch(0), Yaw(0), Roll(0) {
		}

		inline FRotator(float x, float y, float z)
			: Pitch(x),
			Yaw(y),
			Roll(z) {
		}
		inline FRotator(FVector v)
			: Pitch(v.X),
			Yaw(v.Y),
			Roll(v.Z) {
		}
		__forceinline FRotator FRotator::operator+(const FRotator& V) {
			return FRotator(Pitch + V.Pitch, Yaw + V.Yaw, Roll + V.Roll);
		}

		__forceinline FRotator FRotator::operator+(const FRotator& V) const {
			return FRotator(Pitch + V.Pitch, Yaw + V.Yaw, Roll + V.Roll);
		}

		__forceinline FRotator FRotator::operator-(const FRotator& V) {
			return FRotator(Pitch - V.Pitch, Yaw - V.Yaw, Roll - V.Roll);
		}

		__forceinline FRotator FRotator::operator-(const FRotator& V) const {
			return FRotator(Pitch - V.Pitch, Yaw - V.Yaw, Roll - V.Roll);
		}

		__forceinline FRotator FRotator::operator*(float Scale) const {
			return FRotator(Pitch * Scale, Yaw * Scale, Roll * Scale);
		}

		__forceinline FRotator FRotator::operator/(float Scale) const {
			const float RScale = 1.f / Scale;
			return FRotator(Pitch * RScale, Yaw * RScale, Roll * RScale);
		}

		__forceinline FRotator FRotator::operator+(float A) const {
			return FRotator(Pitch + A, Yaw + A, Roll + A);
		}

		__forceinline FRotator FRotator::operator-(float A) const {
			return FRotator(Pitch - A, Yaw - A, Roll - A);
		}

		__forceinline FRotator FRotator::operator*(const FRotator& V) const {
			return FRotator(Pitch * V.Pitch, Yaw * V.Yaw, Roll * V.Roll);
		}

		__forceinline FRotator FRotator::operator/(const FRotator& V) const {
			return FRotator(Pitch / V.Pitch, Yaw / V.Yaw, Roll / V.Roll);
		}

		__forceinline float FRotator::operator|(const FRotator& V) const {
			return Pitch * V.Pitch + Yaw * V.Yaw + Roll * V.Roll;
		}

		__forceinline FRotator& FRotator::operator+=(const FRotator& v) {
			Pitch += v.Pitch;
			Yaw += v.Yaw;
			Roll += v.Roll;
			return *this;
		}

		__forceinline FRotator& FRotator::operator-=(const FRotator& v) {
			Pitch -= v.Pitch;
			Yaw -= v.Yaw;
			Roll -= v.Roll;
			return *this;
		}

		__forceinline FRotator& FRotator::operator*=(const FRotator& v) {
			Pitch *= v.Pitch;
			Yaw *= v.Yaw;
			Roll *= v.Roll;
			return *this;
		}

		__forceinline FRotator& FRotator::operator/=(const FRotator& v) {
			Pitch /= v.Pitch;
			Yaw /= v.Yaw;
			Roll /= v.Roll;
			return *this;
		}

		__forceinline float FRotator::operator^(const FRotator& V) const {
			return Pitch * V.Yaw - Yaw * V.Pitch - Roll * V.Roll;
		}

		__forceinline bool FRotator::operator==(const FRotator& src) const {
			return (src.Pitch == Pitch) && (src.Yaw == Yaw) && (src.Roll == Roll);
		}

		__forceinline bool FRotator::operator!=(const FRotator& src) const {
			return (src.Pitch != Pitch) || (src.Yaw != Yaw) || (src.Roll != Roll);
		}

		__forceinline float FRotator::Size() const {
			return sqrt(Pitch * Pitch + Yaw * Yaw + Roll * Roll);
		}

		__forceinline float FRotator::SizeSquared() const {
			return Pitch * Pitch + Yaw * Yaw + Roll * Roll;
		}

		__forceinline float FRotator::Dot(const FRotator& vOther) const {
			const FRotator& a = *this;

			return (a.Pitch * vOther.Pitch + a.Yaw * vOther.Yaw + a.Roll * vOther.Roll);
		}

		/**
		 * Checks whether rotator is nearly zero within specified tolerance, when treated as an orientation.
		 * This means that FRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator.
		 *
		 * @param Tolerance Error Tolerance.
		 * @return true if rotator is nearly zero, within specified tolerance, otherwise false.
		 */
		bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;

		/**
		 * Gets the rotation values so they fall within the range [0,360]
		 *
		 * @return Clamped version of rotator.
		 */
		FRotator Clamp() const;

		/**
		 * Clamps an angle to the range of [0, 360).
		 *
		 * @param Angle The angle to clamp.
		 * @return The clamped angle.
		 */
		static float ClampAxis(float Angle);

		/**
		 * Clamps an angle to the range of (-180, 180].
		 *
		 * @param Angle The Angle to clamp.
		 * @return The clamped angle.
		 */
		static float NormalizeAxis(float Angle);



		__forceinline void FRotator::Normalize() {
			Pitch = NormalizeAxis(Pitch);
			Yaw = NormalizeAxis(Yaw);
			Roll = NormalizeAxis(Roll);
		}

		__forceinline FRotator FRotator::GetNormalized() const {
			FRotator Rot = *this;
			Rot.Normalize();
			return Rot;
		}

		__forceinline FVector FRotator::Vector() const {
			float CP, SP, CY, SY;
			Math::SinCos(&SP, &CP, Math::DegreesToRadians(Pitch));
			Math::SinCos(&SY, &CY, Math::DegreesToRadians(Yaw));
			FVector V = FVector(CP * CY, CP * SY, SP);
			return V;
		}

		__forceinline bool IsNumber() const {
			return Pitch == Pitch && Yaw == Yaw && Roll == Roll;
		}
		__forceinline bool IsFiniteNumber() const {
			return (Pitch <= DBL_MAX && Pitch >= -DBL_MAX) && (Yaw <= DBL_MAX && Yaw >= -DBL_MAX) && (Roll <= DBL_MAX && Roll >= -DBL_MAX);
		}
		__forceinline bool IsVaild() const {
			return IsNumber() && IsFiniteNumber();
		}

		__forceinline bool IsZero()
		{
			if (this->Pitch == 0.f && this->Yaw == 0.f && this->Roll == 0.f)
				return true;

			return false;
		}

		/**
		 * Checks whether two rotators are equal within specified tolerance, when treated as an orientation.
		 * This means that FRotator(0, 0, 360).Equals(FRotator(0,0,0)) is true, because they represent the same final orientation.
		 *
		 * @param R The other rotator.
		 * @param Tolerance Error Tolerance.
		 * @return true if two rotators are equal, within specified tolerance, otherwise false.
		 */
		bool Equals(const FRotator& R, float Tolerance = KINDA_SMALL_NUMBER) const;

		__forceinline FRotator EqualsValue(const FRotator& R) const
		{
#if PLATFORM_ENABLE_VECTORINTRINSICS
			const VectorRegister RegA = VectorLoadFloat3_W0(this);
			const VectorRegister RegB = VectorLoadFloat3_W0(&R);
			const VectorRegister NormDelta = VectorNormalizeRotator(VectorSubtract(RegA, RegB));
			const VectorRegister AbsNormDelta = VectorAbs(NormDelta);
			return !VectorAnyGreaterThan(AbsNormDelta, VectorLoadFloat1(&Tolerance));
#else
			return FRotator((Math::Abs(NormalizeAxis(Pitch - R.Pitch)))
				, (Math::Abs(NormalizeAxis(Yaw - R.Yaw)))
				, (Math::Abs(NormalizeAxis(Roll - R.Roll))));
#endif
		}

		/**
		 * Get Rotation as a quaternion.
		 *
		 * @return Rotation as a quaternion.
		 */
		struct FQuat Quaternion() const;

		/** Builds a rotation matrix given only a XAxis. Y and Z are unspecified but will be orthonormal. XAxis need not be normalized. */
		static struct FMatrix MakeFromX(FVector const& XAxis);

		/** Builds a rotation matrix given only a YAxis. X and Z are unspecified but will be orthonormal. YAxis need not be normalized. */
		static struct FMatrix MakeFromY(FVector const& YAxis);

		/** Builds a rotation matrix given only a ZAxis. X and Y are unspecified but will be orthonormal. ZAxis need not be normalized. */
		static struct FMatrix MakeFromZ(FVector const& ZAxis);

	};
	FORCEINLINE bool FRotator::IsNearlyZero(float Tolerance) const
	{
		return
			Math::Abs(NormalizeAxis(Pitch)) <= Tolerance
			&& Math::Abs(NormalizeAxis(Yaw)) <= Tolerance
			&& Math::Abs(NormalizeAxis(Roll)) <= Tolerance;
	}

	FORCEINLINE FRotator FRotator::Clamp() const
	{
		return FRotator(ClampAxis(Pitch), ClampAxis(Yaw), ClampAxis(Roll));
	}


	FORCEINLINE float FRotator::ClampAxis(float Angle)
	{
		// returns Angle in the range (-360,360)
		Angle = Math::Fmod(Angle, 360.f);

		if (Angle < 0.f)
		{
			// shift to [0,360) range
			Angle += 360.f;
		}

		return Angle;
	}

	FORCEINLINE float FRotator::NormalizeAxis(float Angle)
	{
		// returns Angle in the range [0,360)
		Angle = ClampAxis(Angle);

		if (Angle > 180.f)
		{
			// shift to (-180,180]
			Angle -= 360.f;
		}

		return Angle;
	}


	FORCEINLINE bool FRotator::Equals(const FRotator& R, float Tolerance) const
	{
#if PLATFORM_ENABLE_VECTORINTRINSICS
		const VectorRegister RegA = VectorLoadFloat3_W0(this);
		const VectorRegister RegB = VectorLoadFloat3_W0(&R);
		const VectorRegister NormDelta = VectorNormalizeRotator(VectorSubtract(RegA, RegB));
		const VectorRegister AbsNormDelta = VectorAbs(NormDelta);
		return !VectorAnyGreaterThan(AbsNormDelta, VectorLoadFloat1(&Tolerance));
#else
		return (Math::Abs(NormalizeAxis(Pitch - R.Pitch)) <= Tolerance)
			&& (Math::Abs(NormalizeAxis(Yaw - R.Yaw)) <= Tolerance)
			&& (Math::Abs(NormalizeAxis(Roll - R.Roll)) <= Tolerance);
#endif
	}


	// ScriptStruct CoreUObject.Quat
	// 0x0010
	struct FQuat
	{
	public:

		/** Default constructor (no initialization). */
		FORCEINLINE FQuat() { }

		/**
		 * Constructor.
		 *
		 * @param InX X component of the quaternion
		 * @param InY Y component of the quaternion
		 * @param InZ Z component of the quaternion
		 * @param InW W component of the quaternion
		 */
		FORCEINLINE FQuat(float InX, float InY, float InZ, float InW)
			: X(InX)
			, Y(InY)
			, Z(InZ)
			, W(InW)
		{
		}

		/**
		 * Copy constructor.
		 *
		 * @param Q A FQuat object to use to create new quaternion from.
		 */
		 //FORCEINLINE FQuat(const struct FQuat& Q);

		 /**
		  * Creates and initializes a new quaternion from the given matrix.
		  *
		  * @param M The rotation matrix to initialize from.
		  */
		explicit FQuat(const struct FMatrix& M);


		/**
		 * Creates and initializes a new quaternion from the given rotator.
		 *
		 * @param R The rotator to initialize from.
		 */
		explicit FQuat(const struct FRotator& R);


		/** Get the FRotator representation of this Quaternion. */
		FRotator Rotator() const;

		/**
		 * Get the length squared of this quaternion.
		 *
		 * @return The length of this quaternion.
		 */
		FORCEINLINE float SizeSquared() const;

		// Return true if this quaternion is normalized
		bool IsNormalized() const;

		/**
		 * @return inverse of this quaternion
		 */
		FORCEINLINE FQuat Inverse() const;

		/**
		 * Normalize this quaternion if it is large enough.
		 * If it is too small, returns an identity quaternion.
		 *
		 * @param Tolerance Minimum squared length of quaternion for normalization.
		 */
		FORCEINLINE void Normalize(float Tolerance = SMALL_NUMBER);

		FVector Rotate(FVector V) const;
		/**
		 * Rotate a vector by this quaternion.
		 *
		 * @param V the vector to be rotated
		 * @return vector after rotation
		 */
		FVector RotateVector(FVector V) const;

		/**
		 * Rotate a vector by the inverse of this quaternion.
		 *
		 * @param V the vector to be rotated
		 * @return vector after rotation by the inverse of this quaternion.
		 */
		FVector UnrotateVector(FVector V) const;
		/**
		 * Gets the result of multiplying this by another quaternion (this * Q).
		 *
		 * Order matters when composing quaternions: C = A * B will yield a quaternion C that logically
		 * first applies B then A to any subsequent transformation (right first, then left).
		 *
		 * @param Q The Quaternion to multiply this by.
		 * @return The result of multiplication (this * Q).
		 */
		FORCEINLINE FQuat operator*(const FQuat& Q) const;

		/**
		 * Rotate a vector by this quaternion.
		 *
		 * @param V the vector to be rotated
		 * @return vector after rotation
		 * @see RotateVector
		 */
		FVector operator*(const FVector& V) const;

		float                                              X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		float                                              W;                                                        // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

	public:

		/** Identity quaternion. */
		static const FQuat Identity;
	};




	enum EForceInit
	{
		ForceInit,
		ForceInitToZero
	};

	// ScriptStruct CoreUObject.Plane
	// 0x0004 (0x0010 - 0x000C)
	struct alignas(16) FPlane : public FVector
	{
	public:
		/** Default constructor (no initialization). */
		FORCEINLINE FPlane();

		/**
		 * Copy Constructor.
		 *
		 * @param P Plane to copy from.
		 */
		FORCEINLINE FPlane(const FPlane & P);

		/**
		 * Constructor.
		 *
		 * @param V 4D vector to set up plane.
		 */
		 //FORCEINLINE FPlane(const FVector4 & V);
		 /**
		  * Constructor.
		  *
		  * @param InX X-coefficient.
		  * @param InY Y-coefficient.
		  * @param InZ Z-coefficient.
		  * @param InW W-coefficient.
		  */
		FORCEINLINE FPlane(float InX, float InY, float InZ, float InW);

		/**
		 * Constructor.
		 *
		 * @param InNormal Plane Normal Vector.
		 * @param InW Plane W-coefficient.
		 */
		FORCEINLINE FPlane(FVector InNormal, float InW);

		/**
		 * Constructor.
		 *
		 * @param InBase Base point in plane.
		 * @param InNormal Plane Normal Vector.
		 */
		FORCEINLINE FPlane(FVector InBase, const FVector & InNormal);

		/**
		 * Constructor.
		 *
		 * @param A First point in the plane.
		 * @param B Second point in the plane.
		 * @param C Third point in the plane.
		 */
		FPlane(FVector A, FVector B, FVector C);

		/**
		 * Constructor
		 *
		 * @param EForceInit Force Init Enum.
		 */
		explicit FORCEINLINE FPlane(EForceInit);
		float W;
	};

	/* FPlane inline functions
	 *****************************************************************************/

	FORCEINLINE FPlane::FPlane()
	{}


	FORCEINLINE FPlane::FPlane(const FPlane& P)
		: FVector(P)
		, W(P.W)
	{}


	/*FORCEINLINE FPlane::FPlane(const FVector4& V)
		: FVector(V)
		, W(V.W)
	{}*/


	FORCEINLINE FPlane::FPlane(float InX, float InY, float InZ, float InW)
		: FVector(InX, InY, InZ)
		, W(InW)
	{}


	FORCEINLINE FPlane::FPlane(FVector InNormal, float InW)
		: FVector(InNormal), W(InW)
	{}


	FORCEINLINE FPlane::FPlane(FVector InBase, const FVector& InNormal)
		: FVector(InNormal)
		, W(InBase | InNormal)
	{}


	FORCEINLINE FPlane::FPlane(FVector A, FVector B, FVector C)
		: FVector(((B - A) ^ (C - A)).GetSafeNormal())
	{
		W = A | (FVector)(*this);
	}


	FORCEINLINE FPlane::FPlane(EForceInit)
		: FVector(ForceInit), W(0.f)
	{}


	// ScriptStruct CoreUObject.Matrix
	// 0x0040
	struct FMatrix
	{

	public:

		// Constructors.
		FORCEINLINE FMatrix();

		/**
		 * Constructor.
		 *
		 * @param InX X plane
		 * @param InY Y plane
		 * @param InZ Z plane
		 * @param InW W plane
		 */
		FORCEINLINE FMatrix(const FPlane& InX, const FPlane& InY, const FPlane& InZ, const FPlane& InW);

		/**
		 * Constructor.
		 *
		 * @param InX X vector
		 * @param InY Y vector
		 * @param InZ Z vector
		 * @param InW W vector
		 */
		FORCEINLINE FMatrix(const FVector& InX, const FVector& InY, const FVector& InZ, const FVector& InW);


		// @return determinant of this matrix.

		inline float Determinant() const;

		/** Fast path, and handles nil matrices. */
		inline FMatrix Inverse() const;


		// NOTE: There is some compiler optimization issues with WIN64 that cause FORCEINLINE to cause a crash
		// Remove any scaling from this matrix (ie magnitude of each row is 1) with error Tolerance
		inline void RemoveScaling(float Tolerance = SMALL_NUMBER);

		/**
		 * set an axis of this matrix
		 *
		 * @param i index into the axis of the matrix
		 * @param Axis vector of the axis
		 */
		inline void SetAxis(int32_t i, const FVector& Axis);

		/**
		 * get axis of this matrix scaled by the scale of the matrix
		 *
		 * @param i index into the axis of the matrix
		 * @ return vector of the axis
		 */
		inline FVector GetScaledAxis(EAxis::Type Axis) const;

		// @return the origin of the co-ordinate system
		inline FVector GetOrigin() const;

		/** @return rotator representation of this matrix */
		inline FRotator Rotator() const;
		/**
		 * Gets the result of multiplying a Matrix to this.
		 *
		 * @param Other The matrix to multiply this by.
		 * @return The result of multiplication.
		 */
		FORCEINLINE FMatrix operator* (const FMatrix& Other) const;
	public:
		union
		{
			alignas(16) float M[4][4] alignas(16);
		};
		FPlane                                      XPlane;                                                   // 0x0000(0x0010) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		FPlane                                      YPlane;                                                   // 0x0010(0x0010) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		FPlane                                      ZPlane;                                                   // 0x0020(0x0010) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		FPlane                                      WPlane;                                                   // 0x0030(0x0010) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		alignas(16) static const FMatrix Identity alignas(16);
	};

	/** Combined rotation and translation matrix */
	class FRotationTranslationMatrix
		: public FMatrix
	{
	public:

		/**
		 * Constructor.
		 *
		 * @param Rot rotation
		 * @param Origin translation to apply
		 */
		FRotationTranslationMatrix(const FRotator& Rot, const FVector& Origin);

		/** Matrix factory. Return an FMatrix so we don't have type conversion issues in expressions. */
		static FMatrix Make(const FRotator& Rot, const FVector& Origin)
		{
			return FRotationTranslationMatrix(Rot, Origin);
		}
	};


	FORCEINLINE FMatrix::FMatrix(const FPlane& InX, const FPlane& InY, const FPlane& InZ, const FPlane& InW)
	{
		M[0][0] = InX.X; M[0][1] = InX.Y;  M[0][2] = InX.Z;  M[0][3] = InX.W;
		M[1][0] = InY.X; M[1][1] = InY.Y;  M[1][2] = InY.Z;  M[1][3] = InY.W;
		M[2][0] = InZ.X; M[2][1] = InZ.Y;  M[2][2] = InZ.Z;  M[2][3] = InZ.W;
		M[3][0] = InW.X; M[3][1] = InW.Y;  M[3][2] = InW.Z;  M[3][3] = InW.W;
	}

	FORCEINLINE FMatrix::FMatrix(const FVector& InX, const FVector& InY, const FVector& InZ, const FVector& InW)
	{
		M[0][0] = InX.X; M[0][1] = InX.Y;  M[0][2] = InX.Z;  M[0][3] = 0.0f;
		M[1][0] = InY.X; M[1][1] = InY.Y;  M[1][2] = InY.Z;  M[1][3] = 0.0f;
		M[2][0] = InZ.X; M[2][1] = InZ.Y;  M[2][2] = InZ.Z;  M[2][3] = 0.0f;
		M[3][0] = InW.X; M[3][1] = InW.Y;  M[3][2] = InW.Z;  M[3][3] = 1.0f;
	}

	// Constructors.

	FORCEINLINE FMatrix::FMatrix()
	{
	}

	// NOTE: There is some compiler optimization issues with WIN64 that cause FORCEINLINE to cause a crash
	// Remove any scaling from this matrix (ie magnitude of each row is 1)
	inline void FMatrix::RemoveScaling(float Tolerance/*=SMALL_NUMBER*/)
	{
		// For each row, find magnitude, and if its non-zero re-scale so its unit length.
		const float SquareSum0 = (M[0][0] * M[0][0]) + (M[0][1] * M[0][1]) + (M[0][2] * M[0][2]);
		const float SquareSum1 = (M[1][0] * M[1][0]) + (M[1][1] * M[1][1]) + (M[1][2] * M[1][2]);
		const float SquareSum2 = (M[2][0] * M[2][0]) + (M[2][1] * M[2][1]) + (M[2][2] * M[2][2]);
		const float Scale0 = Math::FloatSelect(SquareSum0 - Tolerance, Math::InvSqrt(SquareSum0), 1.0f);
		const float Scale1 = Math::FloatSelect(SquareSum1 - Tolerance, Math::InvSqrt(SquareSum1), 1.0f);
		const float Scale2 = Math::FloatSelect(SquareSum2 - Tolerance, Math::InvSqrt(SquareSum2), 1.0f);
		M[0][0] *= Scale0;
		M[0][1] *= Scale0;
		M[0][2] *= Scale0;
		M[1][0] *= Scale1;
		M[1][1] *= Scale1;
		M[1][2] *= Scale1;
		M[2][0] *= Scale2;
		M[2][1] *= Scale2;
		M[2][2] *= Scale2;
	}

	inline void FMatrix::SetAxis(int32_t i, const FVector& Axis)
	{
		//checkSlow(i >= 0 && i <= 2);
		M[i][0] = Axis.X;
		M[i][1] = Axis.Y;
		M[i][2] = Axis.Z;
	}

	inline FVector FMatrix::GetScaledAxis(EAxis::Type InAxis) const
	{
		switch (InAxis)
		{
		case EAxis::X:
			return FVector(M[0][0], M[0][1], M[0][2]);

		case EAxis::Y:
			return FVector(M[1][0], M[1][1], M[1][2]);

		case EAxis::Z:
			return FVector(M[2][0], M[2][1], M[2][2]);

		default:
			//ensure(0);
			return FVector::ZeroVector;
		}
	}

	// Inverse.
	inline FMatrix FMatrix::Inverse() const
	{
		FMatrix Result;

		// Check for zero scale matrix to invert
		if (GetScaledAxis(EAxis::X).IsNearlyZero(SMALL_NUMBER) &&
			GetScaledAxis(EAxis::Y).IsNearlyZero(SMALL_NUMBER) &&
			GetScaledAxis(EAxis::Z).IsNearlyZero(SMALL_NUMBER))
		{
			// just set to zero - avoids unsafe inverse of zero and duplicates what QNANs were resulting in before (scaling away all children)
			Result = FMatrix::Identity;
		}
		else
		{
			const float	Det = Determinant();

			if (Det == 0.0f)
			{
				Result = FMatrix::Identity;
			}
			else
			{
				VectorMatrixInverse(&Result, this);
			}
		}

		return Result;
	}

	// Determinant.

	inline float FMatrix::Determinant() const
	{
		return	M[0][0] * (
			M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
			M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
			M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
			) -
			M[1][0] * (
				M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
				M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
				M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
				) +
			M[2][0] * (
				M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
				M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
				M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
				) -
			M[3][0] * (
				M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
				M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
				M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
				);
	}

	// GetOrigin

	inline FVector FMatrix::GetOrigin() const
	{
		return FVector(M[3][0], M[3][1], M[3][2]);
	}

	inline FRotator FMatrix::Rotator() const
	{
		const FVector		XAxis = GetScaledAxis(EAxis::X);
		const FVector		YAxis = GetScaledAxis(EAxis::Y);
		const FVector		ZAxis = GetScaledAxis(EAxis::Z);

		FRotator	Rotator = FRotator(
			Math::Atan2(XAxis.Z, Math::Sqrt(Math::Square(XAxis.X) + Math::Square(XAxis.Y))) * 180.f / PI,
			Math::Atan2(XAxis.Y, XAxis.X) * 180.f / PI,
			0
		);

		const FVector		SYAxis = FRotationTranslationMatrix(Rotator, FVector::ZeroVector).GetScaledAxis(EAxis::Y);
		Rotator.Roll = Math::Atan2(ZAxis | SYAxis, YAxis | SYAxis) * 180.f / PI;
		return Rotator;
	}


	FORCEINLINE FMatrix FMatrix::operator*(const FMatrix& Other) const
	{
		FMatrix Result;
		VectorMatrixMultiply(&Result, this, &Other);
		return Result;
	}



	struct alignas(16) FTransform
	{
	public:

		/** Default constructor. */
		FORCEINLINE FTransform()
			: Rotation(0.f, 0.f, 0.f, 1.f)
			, Translation(0.f)
			, Scale3D(FVector::OneVector)
		{
		}

		/**
		* Constructor with all components initialized
		*
		* @param InRotation The value to use for rotation component
		* @param InTranslation The value to use for the translation component
		* @param InScale3D The value to use for the scale component
		*/
		FORCEINLINE FTransform(const FQuat & InRotation, const FVector & InTranslation, const FVector & InScale3D = FVector::OneVector)
			: Rotation(InRotation),
			Translation(InTranslation),
			Scale3D(InScale3D)
		{
		}

		/**
		 * Constructor with all components initialized, taking a FRotator as the rotation component
		 *
		 * @param InRotation The value to use for rotation component (after being converted to a quaternion)
		 * @param InTranslation The value to use for the translation component
		 * @param InScale3D The value to use for the scale component
		 */
		FORCEINLINE FTransform(const FRotator & InRotation, const FVector & InTranslation, const FVector & InScale3D = FVector::OneVector)
			: Rotation(InRotation),
			Translation(InTranslation),
			Scale3D(InScale3D)
		{

		}

		/**
		 * Return a transform that is the result of this multiplied by another transform.
		 * Order matters when composing transforms : C = A * B will yield a transform C that logically first applies A then B to any subsequent transformation.
		 *
		 * @param  Other other transform by which to multiply.
		 * @return new transform: this * Other
		 */
		FORCEINLINE FTransform operator*(const FTransform & Other) const;

		FTransform GetRelativeTransform(const FTransform & Other) const;
		/** Inverts the transform and then transforms V - correctly handles scaling in this transform. */
		FORCEINLINE FVector InverseTransformPositionNoScale(const FVector & V) const;
		FORCEINLINE static bool AnyHasNegativeScale(const FVector & InScale3D, const  FVector & InOtherScale3D);

		/**
		 * Create a new transform: OutTransform = A * B.
		 *
		 * Order matters when composing transforms : A * B will yield a transform that logically first applies A then B to any subsequent transformation.
		 *
		 * @param  OutTransform pointer to transform that will store the result of A * B.
		 * @param  A Transform A.
		 * @param  B Transform B.
		 */
		FORCEINLINE static void Multiply(FTransform * OutTransform, const FTransform * A, const FTransform * B);

		/**
		 * Returns the Scale3D component
		 *
		 * @return The Scale3D component
		 */
		FORCEINLINE FVector GetScale3D() const
		{
			//DiagnosticCheckNaN_Scale3D();
			return Scale3D;
		}

		/**
		 * Returns the translation component
		 *
		 * @return The translation component
		 */
		FORCEINLINE FVector GetTranslation() const
		{
			//DiagnosticCheckNaN_Translate();
			return Translation;
		}

		/**
		 * Returns the rotation component
		 *
		 * @return The rotation component
		 */
		FORCEINLINE FQuat GetRotation() const
		{
			//DiagnosticCheckNaN_Rotate();
			return Rotation;
		}
	private:
		/**
		* Create a new transform: OutTransform = A * B using the matrix while keeping the scale that's given by A and B
		* Please note that this operation is a lot more expensive than normal Multiply
		*
		* Order matters when composing transforms : A * B will yield a transform that logically first applies A then B to any subsequent transformation.
		*
		* @param  OutTransform pointer to transform that will store the result of A * B.
		* @param  A Transform A.
		* @param  B Transform B.
		*/

		FORCEINLINE static void MultiplyUsingMatrixWithScale(FTransform * OutTransform, const FTransform * A, const FTransform * B);

		/**
		 * Create a new transform from multiplications of given to matrices (AMatrix*BMatrix) using desired scale
		 * This is used by MultiplyUsingMatrixWithScale and GetRelativeTransformUsingMatrixWithScale
		 * This is only used to handle negative scale
		 *
		 * @param	AMatrix first Matrix of operation
		 * @param	BMatrix second Matrix of operation
		 * @param	DesiredScale - there is no check on if the magnitude is correct here. It assumes that is correct.
		 * @param	OutTransform the constructed transform
		 */
		FORCEINLINE static void ConstructTransformFromMatrixWithDesiredScale(const FMatrix & AMatrix, const FMatrix & BMatrix, const FVector & DesiredScale, FTransform & OutTransform);

		/**
		* Create a new transform: OutTransform = Base * Relative(-1) using the matrix while keeping the scale that's given by Base and Relative
		* Please note that this operation is a lot more expensive than normal GetRelativeTrnasform
		*
		* @param  OutTransform pointer to transform that will store the result of Base * Relative(-1).
		* @param  BAse Transform Base.
		* @param  Relative Transform Relative.
		*/
		static void GetRelativeTransformUsingMatrixWithScale(FTransform * OutTransform, const FTransform * Base, const FTransform * Relative);

		FORCEINLINE static FVector GetSafeScaleReciprocal(const FVector & InScale, float Tolerance = SMALL_NUMBER);
	public:
		struct FQuat                                       Rotation;                                                 // 0x0000(0x0010) (Edit, BlueprintVisible, SaveGame, IsPlainOldData)
		struct FVector                                     Translation;                                              // 0x0010(0x000C) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		unsigned char                                      UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
		struct FVector                                     Scale3D;                                                  // 0x0020(0x000C) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		unsigned char                                      UnknownData01[0x4];                                       // 0x002C(0x0004) MISSED OFFSET

		static const FTransform Identity;
		FORCEINLINE FMatrix ToMatrixWithScale() const
		{
			FMatrix OutMatrix;
			OutMatrix.M[3][0] = Translation.X;
			OutMatrix.M[3][1] = Translation.Y;
			OutMatrix.M[3][2] = Translation.Z;

			const float x2 = Rotation.X + Rotation.X;
			const float y2 = Rotation.Y + Rotation.Y;
			const float z2 = Rotation.Z + Rotation.Z;
			{
				const float xx2 = Rotation.X * x2;
				const float yy2 = Rotation.Y * y2;
				const float zz2 = Rotation.Z * z2;

				OutMatrix.M[0][0] = (1.0f - (yy2 + zz2)) * Scale3D.X;
				OutMatrix.M[1][1] = (1.0f - (xx2 + zz2)) * Scale3D.Y;
				OutMatrix.M[2][2] = (1.0f - (xx2 + yy2)) * Scale3D.Z;
			}
			{
				const float yz2 = Rotation.Y * z2;
				const float wx2 = Rotation.W * x2;

				OutMatrix.M[2][1] = (yz2 - wx2) * Scale3D.Z;
				OutMatrix.M[1][2] = (yz2 + wx2) * Scale3D.Y;
			}
			{
				const float xy2 = Rotation.X * y2;
				const float wz2 = Rotation.W * z2;

				OutMatrix.M[1][0] = (xy2 - wz2) * Scale3D.Y;
				OutMatrix.M[0][1] = (xy2 + wz2) * Scale3D.X;
			}
			{
				const float xz2 = Rotation.X * z2;
				const float wy2 = Rotation.W * y2;

				OutMatrix.M[2][0] = (xz2 + wy2) * Scale3D.Z;
				OutMatrix.M[0][2] = (xz2 - wy2) * Scale3D.X;
			}

			OutMatrix.M[0][3] = 0.0f;
			OutMatrix.M[1][3] = 0.0f;
			OutMatrix.M[2][3] = 0.0f;
			OutMatrix.M[3][3] = 1.0f;

			return OutMatrix;
		}
	};

	// do backward operation when inverse, translation -> rotation
	FORCEINLINE FVector FTransform::InverseTransformPositionNoScale(const FVector& V) const
	{
		const VectorRegister InputVector = VectorLoadFloat3_W0(&V);
		const VectorRegister TranslationVector = VectorLoadFloat3_W0(&Translation);

		// (V-Translation)
		const VectorRegister TranslatedVec = VectorSet_W0(VectorSubtract(InputVector, TranslationVector));

		// ( Rotation.Inverse() * (V-Translation) )
		const VectorRegister VResult = VectorQuaternionInverseRotateVector(VectorLoadFloat4(&Rotation), TranslatedVec);

		FVector Result;
		VectorStoreFloat3(VResult, &Result);
		return Result;
	}

	FORCEINLINE bool FTransform::AnyHasNegativeScale(const FVector& InScale3D, const  FVector& InOtherScale3D)
	{
		return  (InScale3D.X < 0.f || InScale3D.Y < 0.f || InScale3D.Z < 0.f
			|| InOtherScale3D.X < 0.f || InOtherScale3D.Y < 0.f || InOtherScale3D.Z < 0.f);
	}

	/** Returns Multiplied Transform of 2 FTransforms **/
	FORCEINLINE void FTransform::Multiply(FTransform* OutTransform, const FTransform* A, const FTransform* B)
	{
		//A->DiagnosticCheckNaN_All();
		//B->DiagnosticCheckNaN_All();

		//checkSlow(A->IsRotationNormalized());
		//checkSlow(B->IsRotationNormalized());

		//	When Q = quaternion, S = single scalar scale, and T = translation
		//	QST(A) = Q(A), S(A), T(A), and QST(B) = Q(B), S(B), T(B)

		//	QST (AxB) 

		// QST(A) = Q(A)*S(A)*P*-Q(A) + T(A)
		// QST(AxB) = Q(B)*S(B)*QST(A)*-Q(B) + T(B)
		// QST(AxB) = Q(B)*S(B)*[Q(A)*S(A)*P*-Q(A) + T(A)]*-Q(B) + T(B)
		// QST(AxB) = Q(B)*S(B)*Q(A)*S(A)*P*-Q(A)*-Q(B) + Q(B)*S(B)*T(A)*-Q(B) + T(B)
		// QST(AxB) = [Q(B)*Q(A)]*[S(B)*S(A)]*P*-[Q(B)*Q(A)] + Q(B)*S(B)*T(A)*-Q(B) + T(B)

		//	Q(AxB) = Q(B)*Q(A)
		//	S(AxB) = S(A)*S(B)
		//	T(AxB) = Q(B)*S(B)*T(A)*-Q(B) + T(B)

		if (AnyHasNegativeScale(A->Scale3D, B->Scale3D))
		{
			// @note, if you have 0 scale with negative, you're going to lose rotation as it can't convert back to quat
			MultiplyUsingMatrixWithScale(OutTransform, A, B);
		}
		else
		{
			OutTransform->Rotation = B->Rotation * A->Rotation;
			OutTransform->Scale3D = A->Scale3D * B->Scale3D;
			OutTransform->Translation = B->Rotation * (B->Scale3D * A->Translation) + B->Translation;
		}

		// we do not support matrix transform when non-uniform
		// that was removed at rev 21 with UE4
		//OutTransform->DiagnosticCheckNaN_All();
	}

	FORCEINLINE FTransform FTransform::operator*(const FTransform& Other) const
	{
		FTransform Output;
		Multiply(&Output, this, &Other);
		return Output;
	}

	FORCEINLINE void FTransform::MultiplyUsingMatrixWithScale(FTransform* OutTransform, const FTransform* A, const FTransform* B)
	{
		// the goal of using M is to get the correct orientation
		// but for translation, we still need scale
		ConstructTransformFromMatrixWithDesiredScale(A->ToMatrixWithScale(), B->ToMatrixWithScale(), A->Scale3D * B->Scale3D, *OutTransform);
	}

	FORCEINLINE void FTransform::ConstructTransformFromMatrixWithDesiredScale(const FMatrix& AMatrix, const FMatrix& BMatrix, const FVector& DesiredScale, FTransform& OutTransform)
	{
		// the goal of using M is to get the correct orientation
		// but for translation, we still need scale
		FMatrix M = AMatrix * BMatrix;
		M.RemoveScaling();

		// apply negative scale back to axes
		FVector SignedScale = DesiredScale.GetSignVector();

		M.SetAxis(0, M.GetScaledAxis(EAxis::X) * SignedScale.X);
		M.SetAxis(1, M.GetScaledAxis(EAxis::Y) * SignedScale.Y);
		M.SetAxis(2, M.GetScaledAxis(EAxis::Z) * SignedScale.Z);

		// @note: if you have negative with 0 scale, this will return rotation that is identity
		// since matrix loses that axes
		FQuat Rotation = FQuat(M);
		Rotation.Normalize();

		// set values back to output
		OutTransform.Scale3D = DesiredScale;
		OutTransform.Rotation = Rotation;

		// technically I could calculate this using FTransform but then it does more quat multiplication 
		// instead of using Scale in matrix multiplication
		// it's a question of between RemoveScaling vs using FTransform to move translation
		OutTransform.Translation = M.GetOrigin();
	}

	// mathematically if you have 0 scale, it should be infinite, 
	// however, in practice if you have 0 scale, and relative transform doesn't make much sense 
	// anymore because you should be instead of showing gigantic infinite mesh
	// also returning BIG_NUMBER causes sequential NaN issues by multiplying 
	// so we hardcode as 0
	FORCEINLINE FVector FTransform::GetSafeScaleReciprocal(const FVector& InScale, float Tolerance)
	{
		FVector SafeReciprocalScale;
		if (Math::Abs(InScale.X) <= Tolerance)
		{
			SafeReciprocalScale.X = 0.f;
		}
		else
		{
			SafeReciprocalScale.X = 1 / InScale.X;
		}

		if (Math::Abs(InScale.Y) <= Tolerance)
		{
			SafeReciprocalScale.Y = 0.f;
		}
		else
		{
			SafeReciprocalScale.Y = 1 / InScale.Y;
		}

		if (Math::Abs(InScale.Z) <= Tolerance)
		{
			SafeReciprocalScale.Z = 0.f;
		}
		else
		{
			SafeReciprocalScale.Z = 1 / InScale.Z;
		}

		return SafeReciprocalScale;
	}



	/* FQuat inline functions
 *****************************************************************************/

	inline FQuat::FQuat(const FMatrix& M)
	{
		// If Matrix is NULL, return Identity quaternion. If any of them is 0, you won't be able to construct rotation
		// if you have two plane at least, we can reconstruct the frame using cross product, but that's a bit expensive op to do here
		// for now, if you convert to matrix from 0 scale and convert back, you'll lose rotation. Don't do that. 
		if (M.GetScaledAxis(EAxis::X).IsNearlyZero() || M.GetScaledAxis(EAxis::Y).IsNearlyZero() || M.GetScaledAxis(EAxis::Z).IsNearlyZero())
		{
			*this = FQuat::Identity;
			return;
		}


		//const MeReal *const t = (MeReal *) tm;
		float	s;

		// Check diagonal (trace)
		const float tr = M.M[0][0] + M.M[1][1] + M.M[2][2];

		if (tr > 0.0f)
		{
			float InvS = Math::InvSqrt(tr + 1.f);
			this->W = 0.5f * (1.f / InvS);
			s = 0.5f * InvS;

			this->X = (M.M[1][2] - M.M[2][1]) * s;
			this->Y = (M.M[2][0] - M.M[0][2]) * s;
			this->Z = (M.M[0][1] - M.M[1][0]) * s;
		}
		else
		{
			// diagonal is negative
			int32_t i = 0;

			if (M.M[1][1] > M.M[0][0])
				i = 1;

			if (M.M[2][2] > M.M[i][i])
				i = 2;

			static const int32_t nxt[3] = { 1, 2, 0 };
			const int32_t j = nxt[i];
			const int32_t k = nxt[j];

			s = M.M[i][i] - M.M[j][j] - M.M[k][k] + 1.0f;

			float InvS = Math::InvSqrt(s);

			float qt[4];
			qt[i] = 0.5f * (1.f / InvS);

			s = 0.5f * InvS;

			qt[3] = (M.M[j][k] - M.M[k][j]) * s;
			qt[j] = (M.M[i][j] + M.M[j][i]) * s;
			qt[k] = (M.M[i][k] + M.M[k][i]) * s;

			this->X = qt[0];
			this->Y = qt[1];
			this->Z = qt[2];
			this->W = qt[3];

			//DiagnosticCheckNaN();
		}
	}


	FORCEINLINE FQuat::FQuat(const FRotator& R)
	{
		*this = R.Quaternion();
		//DiagnosticCheckNaN();
	}

	FORCEINLINE void FQuat::Normalize(float Tolerance)
	{
		const float SquareSum = X * X + Y * Y + Z * Z + W * W;

		if (SquareSum >= Tolerance)
		{
			const float Scale = Math::InvSqrt(SquareSum);

			X *= Scale;
			Y *= Scale;
			Z *= Scale;
			W *= Scale;
		}
		else
		{
			*this = FQuat::Identity;
		}
	}

	FORCEINLINE FVector FQuat::Rotate(FVector V) const
	{
		// http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
		// V' = V + 2w(Q x V) + (2Q x (Q x V))
		// refactor:
		// V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
		// T = 2(Q x V);
		// V' = V + w*(T) + (Q x T)

		const float vx = 2.0f * V.X;
		const float vy = 2.0f * V.Y;
		const float vz = 2.0f * V.Z;
		const float w2 = W * W - 0.5f;
		const float dot2 = (X * vx + Y * vy + Z * vz);
		return FVector((vx * w2 + (Y * vz - Z * vy) * W + X * dot2), (vy * w2 + (Z * vx - X * vz) * W + Y * dot2),
			(vz * w2 + (X * vy - Y * vx) * W + Z * dot2));
	}

	FORCEINLINE FVector FQuat::RotateVector(FVector V) const
	{
		// http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
		// V' = V + 2w(Q x V) + (2Q x (Q x V))
		// refactor:
		// V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
		// T = 2(Q x V);
		// V' = V + w*(T) + (Q x T)

		const FVector Q(X, Y, Z);
		const FVector T = FVector::CrossProduct(Q, V) * 2.f;
		const FVector Result = V + (T * W) + FVector::CrossProduct(Q, T);
		return Result;
	}

	FORCEINLINE FVector FQuat::UnrotateVector(FVector V) const
	{
		//return Inverse().RotateVector(V);

		const FVector Q(-X, -Y, -Z); // Inverse
		const FVector T = FVector::CrossProduct(Q, V) * 2.f;
		const FVector Result = V + (T * W) + FVector::CrossProduct(Q, T);
		return Result;
	}

	FORCEINLINE FQuat FQuat::operator*(const FQuat& Q) const
	{
		FQuat Result;
		VectorQuaternionMultiply(&Result, this, &Q);

		//Result.DiagnosticCheckNaN();

		return Result;
	}

	FORCEINLINE FVector FQuat::operator*(const FVector& V) const
	{
		return RotateVector(V);
	}

	FORCEINLINE bool FQuat::IsNormalized() const
	{
		return (Math::Abs(1.f - SizeSquared()) < THRESH_QUAT_NORMALIZED);
	}

	FORCEINLINE FQuat FQuat::Inverse() const
	{
		//checkSlow(IsNormalized());

		return FQuat(-X, -Y, -Z, W);
	}


	FORCEINLINE float FQuat::SizeSquared() const
	{
		return (X * X + Y * Y + Z * Z + W * W);
	}

	static FRotator Vec2Rot(FVector vec)
	{
		FRotator rot;

		rot.Yaw = RAD2DEG(std::atan2f(vec.Y, vec.X));
		rot.Pitch = RAD2DEG(std::atan2f(vec.Z, std::sqrtf(vec.X * vec.X + vec.Y * vec.Y)));
		rot.Roll = 0.f;

		return rot;
	}

	static FRotator Clamp(FRotator r, bool CP = false)
	{
		if (CP)
		{
			// CP
			while (r.Yaw < 0.0f)
				r.Yaw += 360.0f;
			while (r.Yaw > 360.0f)
				r.Yaw -= 360.0f;

			while (r.Pitch < 0.0f)
				r.Pitch += 360.0f;
			while (r.Pitch > 360.0f)
				r.Pitch -= 360.0f;

			if (r.Pitch > 75.0f && r.Pitch <= 180.0f)//87
				r.Pitch = 75.0f;
			else if (r.Pitch > 180.0f && r.Pitch < 273.0f)
				r.Pitch = 273.0f;

			r.Roll = 0.0f;
			return r;
		}

		if (r.Yaw > 180.f)
			r.Yaw -= 360.f;
		else if (r.Yaw < -180.f)
			r.Yaw += 360.f;

		if (r.Pitch > 180.f)
			r.Pitch -= 360.f;
		else if (r.Pitch < -180.f)
			r.Pitch += 360.f;

		if (r.Pitch < -75.f)
			r.Pitch = -75.f;
		else if (r.Pitch > 75.f)
			r.Pitch = 75.f;

		r.Roll = 0.f;
		return r;

	}

}