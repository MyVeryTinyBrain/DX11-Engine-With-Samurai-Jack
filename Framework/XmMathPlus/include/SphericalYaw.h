#pragma once

#include "V3.h"
#include "Q.h"

namespace XmMathPlus
{
	struct XMMATHPLUS_API SphericalYaw
	{
		inline SphericalYaw() : Radius(0), PhiAngle(0), ThetaAngle(0) {}

		inline explicit SphericalYaw(float radius, float phiAngle, float thetaAngle) : Radius(radius), PhiAngle(phiAngle), ThetaAngle(thetaAngle) {}

		inline V3 ToCartesian() const
		{
			float phi = PhiAngle * XmMathPlus::Ex::Deg2Rad;
			float theta = ThetaAngle * XmMathPlus::Ex::Deg2Rad;
			float x = XmMathPlus::Ex::Sin(phi) * XmMathPlus::Ex::Cos(theta) * Radius;
			float y = XmMathPlus::Ex::Cos(phi) * Radius;
			float z = XmMathPlus::Ex::Sin(phi) * XmMathPlus::Ex::Sin(theta) * Radius;
			return V3(x, y, z);
		}

		inline Q ToQuaternion() const
		{
			V3 up = V3::up();
			return Q::LookRotation(ToCartesian(), up);
		}

		inline V3 GetDirection() const
		{
			float phi = PhiAngle * XmMathPlus::Ex::Deg2Rad;
			float theta = ThetaAngle * XmMathPlus::Ex::Deg2Rad;
			float x = XmMathPlus::Ex::Sin(phi) * XmMathPlus::Ex::Sin(theta);
			float y = XmMathPlus::Ex::Cos(phi);
			float z = XmMathPlus::Ex::Sin(phi) * XmMathPlus::Ex::Cos(theta);
			return V3(x, y, z);
		}

		inline static SphericalYaw FromCartesian(const V3& xyz)
		{
			float radius = sqrt(xyz.x * xyz.x * xyz.y * xyz.y + xyz.z * xyz.z);
			float phi = acos(xyz.y / radius) * XmMathPlus::Ex::Rad2Deg;
			float theta = atan2(xyz.x, xyz.z) * XmMathPlus::Ex::Rad2Deg;
			return SphericalYaw(radius, phi, theta);
		}

		inline static SphericalYaw FromQuaternion(const Q& q)
		{
			V3 forward(0, 0, 1);
			forward = q * V3::forward();
			return FromCartesian(forward);
		}

		inline static SphericalYaw Lerp(const SphericalYaw& a, const SphericalYaw& b, Real t)
		{
			float radius = XmMathPlus::Ex::Lerp(a.Radius, b.Radius, t);
			Q q = Q::Lerp(a.ToQuaternion(), b.ToQuaternion(), t);
			SphericalYaw result = FromQuaternion(q);
			result.Radius = radius;
			return result;
		}

		inline SphericalYaw& operator *= (Real rhs)
		{
			Radius *= rhs;
			PhiAngle *= rhs;
			ThetaAngle *= rhs;
			return *this;
		}

		inline SphericalYaw& operator /= (Real rhs)
		{
			Radius /= rhs;
			PhiAngle /= rhs;
			ThetaAngle /= rhs;
			return *this;
		}

		inline SphericalYaw& operator += (const SphericalYaw& rhs)
		{
			return (*this = FromCartesian(ToCartesian() + rhs.ToCartesian()));
		}

		inline SphericalYaw& operator -= (const SphericalYaw& rhs)
		{
			return (*this = FromCartesian(ToCartesian() - rhs.ToCartesian()));
		}

		inline SphericalYaw operator * (Real rhs) const
		{
			SphericalYaw lhs = *this;
			lhs *= rhs;
			return lhs;
		}

		inline SphericalYaw operator / (Real rhs) const
		{
			SphericalYaw lhs = *this;
			lhs /= rhs;
			return lhs;
		}

		inline SphericalYaw operator + (const SphericalYaw& rhs) const
		{
			SphericalYaw lhs = *this;
			lhs += rhs;
			return lhs;
		}

		inline SphericalYaw operator - (const SphericalYaw& rhs) const
		{
			SphericalYaw lhs = *this;
			lhs -= rhs;
			return lhs;
		}

		_declspec(property(get = ToCartesian)) V3 cartesian;
		_declspec(property(get = ToQuaternion)) Q quaternion;
		_declspec(property(get = GetDirection)) V3 direction;

		float Radius = 0.5f;

		float PhiAngle = 0;

		float ThetaAngle = 0;
	};
}
