#ifndef __TERRAINV2_HPP__
#define __TERRAINV2_HPP__

#include "entities/Entity.hpp"
#include "entities/Camera.hpp"
#include "entities/WorldLight.hpp"
#include <string>
#include <vector>

using namespace std;

class TerrainV2 : public Entity
{
private:
	int width;
	int height;
	int totalPatches;

	float maxHeight;
	float size;

	GLuint vaoID;
	GLuint primitiveRestartIndex;
	unsigned int handle[5];

	vector<vec3> vertices;
	vector<vec3> verticesColors;
	vector<vec3> normals;
	vector<vec2> uvs;
	vector<unsigned int> indices;

	Camera* camera;
	GLSLProgram shader;

	void GenerateVertices();
	void GenerateBuffers();

	vec3 dirtColor;
	vector<vec3> grassColors;
	void InitializeGrassColors();

	vec3 goalColor;
	vec3 goalPosition;
	void SetGoalPosition();

	//void SetRenderedPatches();
	vector<vector<float>> ReadHeightMap(string mapPath);
	vector<vector<vec3>> ReadNormalMap(string mapPath);
	vector<vector<vec3>> GenerateNormalMap(vector<vector<float>>& heightMap);

	string heightMapPath;
	string normalMapPath;

	int shells;
	float shellDistanceAttenuation;
	float shellLength;
	float minNoise;
	float maxNoise;

	float AOAttenuation;
	float AOBias;
	float density;

public:
	TerrainV2(int width, int height, int totalPatches, Camera* camera, string heightMapPath, string normalMapPath);

	void Initialize();
	void Update(double deltaTime);
	void Render(mat4 projection, mat4 view);

	vec3 CenterPosition();
	vec3 GoalPosition();
	vector<vec3> GetNearbyVertices(vec3 position);

	WorldLight* worldLight;
};

#endif // __TERRAINV2_HPP__