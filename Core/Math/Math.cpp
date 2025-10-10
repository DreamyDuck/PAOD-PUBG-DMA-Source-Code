#include "Include/WinHeaders.hpp"

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Math.h"

namespace IronMan::Core::Math
{
	using namespace physx;
	const FVector FVector::ZeroVector(0.0f, 0.0f, 0.0f);
	const FVector FVector::OneVector(1.0f, 1.0f, 1.0f);
	const FTransform FTransform::Identity(FQuat(0.f, 0.f, 0.f, 1.f), FVector(0.f), FVector(1.f));
	const FQuat FQuat::Identity(0, 0, 0, 1);
	const FMatrix FMatrix::Identity(FPlane(1, 0, 0, 0), FPlane(0, 1, 0, 0), FPlane(0, 0, 1, 0), FPlane(0, 0, 0, 1));

	FVector _D3DMATRIX::ToVector3()
	{
		return struct FVector(_41, _42, _43);
	}

	FORCEINLINE FRotationTranslationMatrix::FRotationTranslationMatrix(const FRotator& Rot, const FVector& Origin)
	{
#ifdef PLATFORM_ENABLE_VECTORINTRINSICS

		const VectorRegister Angles = MakeVectorRegister(Rot.Pitch, Rot.Yaw, Rot.Roll, 0.0f);
		const VectorRegister HalfAngles = VectorMultiply(Angles, GlobalVectorConstants::DEG_TO_RAD);

		union { VectorRegister v; float f[4]; } SinAngles, CosAngles;
		VectorSinCos(&SinAngles.v, &CosAngles.v, &HalfAngles);

		const float	SP = SinAngles.f[0];
		const float	SY = SinAngles.f[1];
		const float	SR = SinAngles.f[2];
		const float	CP = CosAngles.f[0];
		const float	CY = CosAngles.f[1];
		const float	CR = CosAngles.f[2];

#else

		float SP, SY, SR;
		float CP, CY, CR;
		Math::SinCos(&SP, &CP, Math::DegreesToRadians(Rot.Pitch));
		Math::SinCos(&SY, &CY, Math::DegreesToRadians(Rot.Yaw));
		Math::SinCos(&SR, &CR, Math::DegreesToRadians(Rot.Roll));

#endif // PLATFORM_ENABLE_VECTORINTRINSICS

		M[0][0] = CP * CY;
		M[0][1] = CP * SY;
		M[0][2] = SP;
		M[0][3] = 0.f;

		M[1][0] = SR * SP * CY - CR * SY;
		M[1][1] = SR * SP * SY + CR * CY;
		M[1][2] = -SR * CP;
		M[1][3] = 0.f;

		M[2][0] = -(CR * SP * CY + SR * SY);
		M[2][1] = CY * SR - CR * SP * SY;
		M[2][2] = CR * CP;
		M[2][3] = 0.f;

		M[3][0] = Origin.X;
		M[3][1] = Origin.Y;
		M[3][2] = Origin.Z;
		M[3][3] = 1.f;
	}

	FRotator FQuat::Rotator() const
	{
		//SCOPE_CYCLE_COUNTER(STAT_MathConvertQuatToRotator);

		//DiagnosticCheckNaN();
		const float SingularityTest = Z * X - W * Y;
		const float YawY = 2.f * (W * Z + X * Y);
		const float YawX = (1.f - 2.f * (Math::Square(Y) + Math::Square(Z)));

		// reference 
		// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

		// this value was found from experience, the above websites recommend different values
		// but that isn't the case for us, so I went through different testing, and finally found the case 
		// where both of world lives happily. 
		const float SINGULARITY_THRESHOLD = 0.4999995f;
		const float RAD_TO_DEG = (180.f) / PI;
		FRotator RotatorFromQuat;

		if (SingularityTest < -SINGULARITY_THRESHOLD)
		{
			RotatorFromQuat.Pitch = -90.f;
			RotatorFromQuat.Yaw = Math::Atan2(YawY, YawX) * RAD_TO_DEG;
			RotatorFromQuat.Roll = FRotator::NormalizeAxis(-RotatorFromQuat.Yaw - (2.f * Math::Atan2(X, W) * RAD_TO_DEG));
		}
		else if (SingularityTest > SINGULARITY_THRESHOLD)
		{
			RotatorFromQuat.Pitch = 90.f;
			RotatorFromQuat.Yaw = Math::Atan2(YawY, YawX) * RAD_TO_DEG;
			RotatorFromQuat.Roll = FRotator::NormalizeAxis(RotatorFromQuat.Yaw - (2.f * Math::Atan2(X, W) * RAD_TO_DEG));
		}
		else
		{
			RotatorFromQuat.Pitch = Math::FastAsin(2.f * (SingularityTest)) * RAD_TO_DEG;
			RotatorFromQuat.Yaw = Math::Atan2(YawY, YawX) * RAD_TO_DEG;
			RotatorFromQuat.Roll = Math::Atan2(-2.f * (W * X + Y * Z), (1.f - 2.f * (Math::Square(X) + Math::Square(Y)))) * RAD_TO_DEG;
		}


		return RotatorFromQuat;
	}


	struct FQuat FRotator::Quaternion() const
	{
		const float DEG_TO_RAD = PI / (180.f);
		const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
		float SP, SY, SR;
		float CP, CY, CR;

		Math::SinCos(&SP, &CP, Pitch * DIVIDE_BY_2);
		Math::SinCos(&SY, &CY, Yaw * DIVIDE_BY_2);
		Math::SinCos(&SR, &CR, Roll * DIVIDE_BY_2);

		struct FQuat RotationQuat;
		RotationQuat.X = CR * SP * SY - SR * CP * CY;
		RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
		RotationQuat.Z = CR * CP * SY - SR * SP * CY;
		RotationQuat.W = CR * CP * CY + SR * SP * SY;

		return RotationQuat;
	}

	FMatrix FRotator::MakeFromX(FVector const& XAxis)
	{
		FVector const NewX = XAxis.GetSafeNormal();

		// try to use up if possible
		FVector const UpVector = (Math::Abs(NewX.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);

		const FVector NewY = (UpVector ^ NewX).GetSafeNormal();
		const FVector NewZ = NewX ^ NewY;

		return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
	}

	FMatrix FRotator::MakeFromY(FVector const& YAxis)
	{
		FVector const NewY = YAxis.GetSafeNormal();

		// try to use up if possible
		FVector const UpVector = (Math::Abs(NewY.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);

		const FVector NewZ = (UpVector ^ NewY).GetSafeNormal();
		const FVector NewX = NewY ^ NewZ;

		return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
	}

	FMatrix FRotator::MakeFromZ(FVector const& ZAxis)
	{
		FVector const NewZ = ZAxis.GetSafeNormal();

		// try to use up if possible
		FVector const UpVector = (Math::Abs(NewZ.Z) < (1.f - KINDA_SMALL_NUMBER)) ? FVector(0, 0, 1.f) : FVector(1.f, 0, 0);

		const FVector NewX = (UpVector ^ NewZ).GetSafeNormal();
		const FVector NewY = NewZ ^ NewX;

		return FMatrix(NewX, NewY, NewZ, FVector::ZeroVector);
	}




	FTransform FTransform::GetRelativeTransform(const FTransform& Other) const
	{
		// A * B(-1) = VQS(B)(-1) (VQS (A))
		// 
		// Scale = S(A)/S(B)
		// Rotation = Q(B)(-1) * Q(A)
		// Translation = 1/S(B) *[Q(B)(-1)*(T(A)-T(B))*Q(B)]
		// where A = this, B = Other
		FTransform Result;

		if (AnyHasNegativeScale(Scale3D, Other.GetScale3D()))
		{
			// @note, if you have 0 scale with negative, you're going to lose rotation as it can't convert back to quat
			GetRelativeTransformUsingMatrixWithScale(&Result, this, &Other);
		}
		else
		{
			FVector SafeRecipScale3D = GetSafeScaleReciprocal(Other.Scale3D, SMALL_NUMBER);
			Result.Scale3D = Scale3D * SafeRecipScale3D;

			if (Other.Rotation.IsNormalized() == false)
			{
				return FTransform::Identity;
			}

			FQuat Inverse = Other.Rotation.Inverse();
			Result.Rotation = Inverse * Rotation;

			Result.Translation = (Inverse * (Translation - Other.Translation)) * (SafeRecipScale3D);

#if DEBUG_INVERSE_TRANSFORM
			FMatrix AM = ToMatrixWithScale();
			FMatrix BM = Other.ToMatrixWithScale();

			Result.DebugEqualMatrix(AM * BM.InverseFast());

#endif
		}

		return Result;
	}
	void FTransform::GetRelativeTransformUsingMatrixWithScale(FTransform* OutTransform, const FTransform* Base, const FTransform* Relative)
	{
		// the goal of using M is to get the correct orientation
	// but for translation, we still need scale
		FMatrix AM = Base->ToMatrixWithScale();
		FMatrix BM = Relative->ToMatrixWithScale();
		// get combined scale
		FVector SafeRecipScale3D = GetSafeScaleReciprocal(Relative->Scale3D, SMALL_NUMBER);
		FVector DesiredScale3D = Base->Scale3D * SafeRecipScale3D;
		ConstructTransformFromMatrixWithDesiredScale(AM, BM.Inverse(), DesiredScale3D, *OutTransform);
	}

	FRotator FindLookAtRotation(const FVector& Start, const FVector& Target)
	{
		return MakeRotFromX(Target - Start);
	}

	FRotator MakeRotFromX(const FVector& X)
	{
		return FRotator::MakeFromX(X).Rotator();
	}

	FRotator MakeRotFromY(const FVector& Y)
	{
		return FRotator::MakeFromY(Y).Rotator();
	}

	FRotator MakeRotFromZ(const FVector& Z)
	{
		return FRotator::MakeFromZ(Z).Rotator();
	}
	FRotator RInterpTo(const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed)
	{
		// if DeltaTime is 0, do not perform any interpolation (Location was already calculated for that frame)
		if (DeltaTime == 0.f || Current == Target)
		{
			return Current;
		}

		// If no interp speed, jump to target value
		if (InterpSpeed <= 0.f)
		{
			return Target;
		}

		const float DeltaInterpSpeed = InterpSpeed * DeltaTime;

		const FRotator Delta = (Target - Current).GetNormalized();

		// If steps are too small, just return Target and assume we have reached our destination.
		if (Delta.IsNearlyZero())
		{
			return Target;
		}

		// Delta Move, Clamp so we do not over shoot.
		const FRotator DeltaMove = Delta * Math::Clamp<float>(DeltaInterpSpeed, 0.f, 1.f);
		return (Current + DeltaMove).GetNormalized();
	}

	FVector2D Vector2DInterpConstantTo(const FVector2D& Current, const FVector2D& Target, float DeltaTime, float InterpSpeed)
	{
		const FVector2D Delta = Target - Current;
		const float DeltaM = Delta.Size();
		const float MaxStep = InterpSpeed * DeltaTime;

		if (DeltaM > MaxStep)
		{
			if (MaxStep > 0.f)
			{
				const FVector2D DeltaN = Delta / DeltaM;
				return Current + DeltaN * MaxStep;
			}
			else
			{
				return Current;
			}
		}

		return Target;
	}

	FVector2D Vector2DInterpTo(const FVector2D& Current, const FVector2D& Target, float DeltaTime, float InterpSpeed)
	{
		if (InterpSpeed <= 0.f)
		{
			return Target;
		}

		const FVector2D Dist = Target - Current;
		if (Dist.SizeSquared() < KINDA_SMALL_NUMBER)
		{
			return Target;
		}

		const FVector2D DeltaMove = Dist * Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);
		return Current + DeltaMove;
	}

	PxMat33 QuaternionToMatrix(const FQuat& q) {
		// 提取四元数的组成部分
		float w = q.W;
		float x = q.X;
		float y = q.Y;
		float z = q.Z;

		// 计算重复使用的值以优化计算
		float xx = x * x;
		float yy = y * y;
		float zz = z * z;
		float xy = x * y;
		float xz = x * z;
		float yz = y * z;
		float wx = w * x;
		float wy = w * y;
		float wz = w * z;

		// 根据公式构建3x3矩阵
		PxMat33 rotMat;
		rotMat[0][0] = 1 - 2 * (yy + zz);
		rotMat[0][1] = 2 * (xy - wz);
		rotMat[0][2] = 2 * (xz + wy);

		rotMat[1][0] = 2 * (xy + wz);
		rotMat[1][1] = 1 - 2 * (xx + zz);
		rotMat[1][2] = 2 * (yz - wx);

		rotMat[2][0] = 2 * (xz - wy);
		rotMat[2][1] = 2 * (yz + wx);
		rotMat[2][2] = 1 - 2 * (xx + yy);

		return rotMat;
	}
}