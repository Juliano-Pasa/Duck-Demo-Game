#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "glslprogram.hpp"

struct Transform
{
protected:
	vec3 _position = vec3(0.0f);
	vec3 _scale = vec3(1.0f);
	vec3 _angles = vec3(0.0f);
	quat _rotation = quat(vec3(0.0f));

	mat4 _modelMatrix = mat4(1.0f);

	bool _hasChanged = true;

	vec3 ClampAngles(const vec3& angles);

public:
	const vec3& position() const { return _position; }
	void position(const vec3& position) { _position = position; _hasChanged = true; }

	const vec3& scale() const { return _scale; }
	void scale(const vec3& scale) { _scale = scale; _hasChanged = true; }

	const vec3& angles() const { return _angles; }
	void angles(const vec3& angles) { _angles = ClampAngles(angles); _hasChanged = true; _rotation = quat(angles); }

	const quat& rotation() const { return _rotation; }
	void rotation(const quat& rotation) { _rotation = rotation; _hasChanged = true; _angles = glm::eulerAngles(rotation); }

	const mat4& modelMatrix() const { return _modelMatrix; }
	const bool hasChanged() const { return _hasChanged; }

	void ComputeModelMatrix();
	void ComputeModelMatrix(const mat4& parentMatrix);
};

#endif // __TRANSFORM_HPP__