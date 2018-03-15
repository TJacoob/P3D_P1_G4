#ifndef _VEC3H_
#define _VEC3H_

struct Vec3
{
public:
	float x, y, z;

	// Construtores
	Vec3() : x(float(0)), y(float(0)), z(float(0)) {};
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {};
	
	// Operadores
	Vec3 operator + (const Vec3 &v) const { return Vec3(x + v.x, y + v.y, z + v.z); };
	Vec3 operator - (const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); };
	Vec3 operator * (const float &c) const { return Vec3(x*c, y*c, z*c); };

	// Methods
	float module() const { return sqrt( x*x + y*y + z*z ); };
	Vec3 normalize() const{
		float module = sqrt( x*x + y*y + z*z );
		return Vec3(x / module, y / module, z / module);
	};

};

#endif