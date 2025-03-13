#include "terrain/TerrainV2.hpp"
#include <fstream>
#include <time.h>
#include <random>
#include <iostream>
#include <sstream>

TerrainV2::TerrainV2(int width, int height, int totalPatches, Camera* camera, string heightMapPath, string normalMapPath) : Entity(vec3(0), vec3(0), vec3(1))
{
	this->width = width;
	this->height = height;
	this->vaoID = 0;
	this->primitiveRestartIndex = -1;
	this->camera = camera;

	this->heightMapPath = heightMapPath;
	this->normalMapPath = normalMapPath;

	shells = 64;
	shellLength = 20.0f;
	shellDistanceAttenuation = 2.0f;
	minNoise = 0.0f;
	maxNoise = 1.0f;

	AOAttenuation = 2.0f;
	AOBias = 0.2f;
	density = 100.0f;
}

#pragma region EntityFunctions

void TerrainV2::Initialize()
{
	InitializeGrassColors();
	GenerateVertices();
	GenerateBuffers();

	glPrimitiveRestartIndex(primitiveRestartIndex);
	glEnable(GL_PRIMITIVE_RESTART);

	try {
		shader.compileShader("shader/TerrainShaders/TerrainV2.vert", GLSLShader::VERTEX);
		shader.compileShader("shader/TerrainShaders/TerrainV2.frag", GLSLShader::FRAGMENT);

		shader.link();
		shader.use();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	shader.printActiveAttribs();

	shader.setUniform("minNoise", minNoise);
	shader.setUniform("maxNoise", maxNoise);
	shader.setUniform("AOAttenuation", AOAttenuation);
	shader.setUniform("AOBias", AOBias);
	shader.setUniform("density", density);
}

void TerrainV2::Update(double deltaTime)
{
	shader.use();

	/*shader.setUniform("dirtColor", dirtColor);
	shader.setUniform("camPos", camera->CameraPosition());
	shader.setUniform("lightPos", worldLight->GetPosition());*/

	//SetRenderedPatches();
}

void TerrainV2::Render(mat4 projection, mat4 view)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	shader.use();
	mat4 MV = view * transform.modelMatrix();
	mat4 MVP = projection * MV;

	mat3 NM = mat3(inverse(transpose(MV)));

	shader.setUniform("MVP", MVP);
	shader.setUniform("NM", NM);

	glBindVertexArray(vaoID);

	for (int i = 0; i < shells; i++)
	{
		float shellIndex = (float)i / (float)shells;
		float shellHeight = pow(shellIndex, shellDistanceAttenuation) * shellLength;

		shader.setUniform("shellIndex", shellIndex);
		shader.setUniform("shellHeight", shellHeight);

		glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, (GLubyte*)NULL);
	}

	glBindVertexArray(0);
}

#pragma endregion

#pragma region GenerateFunctions

void TerrainV2::GenerateVertices()
{
	maxHeight = 256.0f;
	size = 256.0f;

	vector<vector<float>> heightMap = ReadHeightMap(heightMapPath);
	vertices.reserve(width * height + 1);
	verticesColors.reserve(width * height + 1);

	vector<vector<vec3>> normalMap = GenerateNormalMap(heightMap);
	normals.reserve(width * height + 1);

	uvs.reserve(width * height + 1);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			float height = heightMap[i][j];
			vertices.push_back(vec3(j * size, height * maxHeight, i * size));
			verticesColors.push_back(grassColors[int(height * 10.0f)]);

			vec3 normal = normalMap[i][j];
			normals.push_back(normal);

			uvs.push_back(vec2(j, i));
		}
	}
	for (int i = 0; i < height - 1; i++)
	{
		for (int j = 0; j < width; j++)
		{
			indices.push_back(i * width + j);
			indices.push_back((i + 1) * width + j);
		}
		indices.push_back(primitiveRestartIndex);
	}

	SetGoalPosition();
}

void TerrainV2::GenerateBuffers()
{
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glGenBuffers(5, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, verticesColors.size() * sizeof(vec3), (GLvoid*)&verticesColors[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), (GLvoid*)&normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), (GLvoid*)&uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)3, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), (GLvoid*)&indices[0], GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

#pragma endregion

#pragma region ReadMaps

vector<vector<float>> TerrainV2::ReadHeightMap(string mapPath)
{
	ifstream file;
	file.open(mapPath);

	vector<vector<float>> map;
	vector<float> row;
	string line, word;

	while (getline(file, line))
	{
		row.clear();

		stringstream str(line);

		while (getline(str, word, ','))
		{
			row.push_back(stof(word));
		}
		map.push_back(row);
	}
	file.close();

	return map;
}

vector<vector<vec3>> TerrainV2::ReadNormalMap(string mapPath)
{
	ifstream file;
	file.open(mapPath);

	vector<vector<vec3>> normalMap;
	vector<vec3> row;
	string line, word, token;
	size_t position;

	while (getline(file, line))
	{
		row.clear();
		stringstream str(line);

		while (getline(str, word, ','))
		{
			position = 0;
			int count = 0;
			float values[3];

			while ((position = word.find(';')) != string::npos)
			{
				token = word.substr(0, position);
				values[count] = stof(token);
				word.erase(0, position + 1);
				count++;
			}

			//row.push_back(vec3(values[0], values[1], values[2]));
			row.push_back(vec3(0.0, 1.0, 0.0));
		}
		normalMap.push_back(row);
	}
	file.close();

	return normalMap;
}

vector<vector<vec3>> TerrainV2::GenerateNormalMap(vector<vector<float>>& heightMap)
{
	vector<vector<vec3>> normalMap;
	for (int i = 0; i < heightMap.size(); i++)
	{
		vector<vec3> row;
		for (int j = 0; j < heightMap[i].size(); j++)
		{
			float up = heightMap[i][j];
			float down = heightMap[i][j];
			float right = heightMap[i][j];
			float left = heightMap[i][j];

			if (i - 1 >= 0)
			{
				up = heightMap[i - 1][j];
			}
			if (i + 1 < size)
			{
				down = heightMap[i + 1][j];
			}
			if (j - 1 >= 0)
			{
				left = heightMap[i][j - 1];
			}
			if (j + 1 < size)
			{
				right = heightMap[i][j + 1];
			}

			float zVec = up - down;
			float xVec = right - left;

			row.push_back(glm::normalize(vec3(- 2.0f * size * xVec, 4 * size * size, - 2.0f * size * zVec)));
		}
		normalMap.push_back(row);
	}
	return normalMap;
}

#pragma endregion

void TerrainV2::InitializeGrassColors()
{
	dirtColor = vec3(0.29411f, 0.20784f, 0.0f);
	goalColor = vec3(0.59215f, 0.09019f, 0.90196f);

	grassColors.push_back(vec3(0.23137, 0.31372, 0.02352));
	grassColors.push_back(vec3(0.65882, 0.74117, 0.02352));
	grassColors.push_back(vec3(0.90588, 0.90980, 0.05490));
	grassColors.push_back(vec3(0.43921, 0.55686, 0.02745));
	grassColors.push_back(vec3(0.58039, 0.67843, 0.02745));
	grassColors.push_back(vec3(0.80784, 0.83921, 0.18823));
	grassColors.push_back(vec3(0.90588, 0.90980, 0.05490));
	grassColors.push_back(vec3(0.65882, 0.74117, 0.02352));
	grassColors.push_back(vec3(0.58431, 0.54509, 0.13725));
	grassColors.push_back(vec3(0.31372, 0.46666, 0.01960));
	grassColors.push_back(vec3(0.31372, 0.46666, 0.01960));
}

vec3 TerrainV2::CenterPosition()
{
	int centerI = height / 2;
	int centerJ = width / 2;

	return vertices[centerI * width + centerJ];
}

vector<vec3> TerrainV2::GetNearbyVertices(vec3 position)
{
	int posI = position.z / size;
	int posJ = position.x / size;
	int startingPoint = posI * width + posJ;

	vector<vec3> nearbyVertices;
	nearbyVertices.reserve(4);

	nearbyVertices.push_back(vertices[startingPoint]);
	nearbyVertices.push_back(vertices[startingPoint + 1]);
	nearbyVertices.push_back(vertices[startingPoint + width]);
	nearbyVertices.push_back(vertices[startingPoint + width + 1]);

	return nearbyVertices;
}

//void TerrainV2::SetRenderedPatches()
//{
//	vec3 cameraPosition = camera->CameraPosition();
//	int neighbourPatches = 11;
//
//	int patchI = cameraPosition.z / patchHeight;
//	int patchJ = cameraPosition.x / patchWidth;
//
//	indices.clear();
//
//	for (int i = -neighbourPatches; i < neighbourPatches + 1; i++)
//	{
//		for (int j = -neighbourPatches; j < neighbourPatches + 1; j++)
//		{
//			if (i + patchI < 0 || i + patchI >= totalPatches)
//			{
//				continue;
//			}
//			if (j + patchJ < 0 || j + patchJ >= totalPatches)
//			{
//				continue;
//			}
//
//			int startingPoint = (i + patchI) * (totalPatches + 1) + (j + patchJ);
//
//			indices.push_back(startingPoint);
//			indices.push_back(startingPoint + 1);
//			indices.push_back(startingPoint + totalPatches + 2);
//			indices.push_back(startingPoint + totalPatches + 1);
//		}
//	}
//
//	glBindVertexArray(vaoID);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), (GLvoid*)&indices[0], GL_DYNAMIC_DRAW);
//
//	glBindVertexArray(0);
//}

void TerrainV2::SetGoalPosition()
{
	srand(time(NULL));
	int goalI = rand() % (height - 1);
	int goalJ = rand() % (width - 1);

	int startingPoint = goalI * width + goalJ;

	verticesColors[startingPoint] = goalColor;
	verticesColors[startingPoint + 1] = goalColor;
	verticesColors[startingPoint + width] = goalColor;
	verticesColors[startingPoint + width + 1] = goalColor;

	goalPosition = vertices[startingPoint] + vertices[startingPoint + width + 1];
	goalPosition /= 2;
}

vec3 TerrainV2::GoalPosition()
{
	return goalPosition;
}