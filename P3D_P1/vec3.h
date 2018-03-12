struct Vec3 {
	float x, y, z;
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3 operator + (const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
	Vec3 operator - (const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
	Vec3 operator * (float d) const { return Vec3(x*d, y*d, z*d); }
	Vec3 operator / (float d) const { return Vec3(x / d, y / d, z / d); }
	Vec3 normalize() const {
		float mg = sqrt(x*x + y * y + z * z);
		return Vec3(x / mg, y / mg, z / mg);
	}
} ;
