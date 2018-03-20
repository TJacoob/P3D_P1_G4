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
	//Vec3 operator * (const Vec3 &v) const { return Vec3(x * v.x, y * v.y, z * v.z); };  // Isto � multiplica��o direta de cada elemento do vetor
	Vec3 operator * (const Vec3 &v) const { return Vec3((y*v.z) - (z*v.y), -(x*v.z) + (z*v.x), (x*v.y) - (y*v.x)); };  // Isto � produto externo, qual deles � que realmente queremos?
	Vec3 operator ^ (const float &c) const { return Vec3(pow(x, c), pow(y, c), pow(z, c)); };

	// Methods
	float module() const { return sqrt(x*x + y * y + z * z); };
	Vec3 normalize() const {
		float module = sqrt(x*x + y * y + z * z);
		return Vec3(x / module, y / module, z / module);
	};
	float dot(Vec3 v) const {
		return (x*v.x) + (y*v.y) + (z*v.z);
	}
	bool equals(Vec3 v) const { return x == v.x && y == v.y && z == v.z; };

};

#endif