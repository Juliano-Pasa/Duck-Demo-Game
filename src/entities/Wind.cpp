#include "entities/Wind.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


Wind::Wind(vec3 position, vec3 scale) : Entity (position, vec3(0), scale)
{
	this->totalAnimationDuration = 4.5f;
	this->currentDuration = 0.0;

	this->totalSequences = 3;
	this->totalParticles = 100;
	this->maxVisibleParticles = 20;
	this->particleSpacement = 1;
	this->windColor = vec3(1);

	this->firstWindParticle = 0;
	this->lastWindParticle = firstWindParticle - maxVisibleParticles;

	this->render = false;
}

float Wind::GetLoopingRotation(double animationKey)
{
	float a = 270;

	if (animationKey >= 0.5)
	{
		animationKey = animationKey * 2 - 1;
		return animationKey * a;
	}

	animationKey *= 2;

	a = 1280 / 3.0f;
	float b = -640;
	float c = 640 / 3.0f;

	return a * animationKey * animationKey * animationKey + b * animationKey * animationKey + c * animationKey;
}

float Wind::GetBreezeRotation(double animationKey)
{
	return 0;
}

void Wind::SetNewSpawn(vec3 position, vec3 angles)
{
	transform.position(position);
	transform.angles(angles);

	animationUpdateCount = 0.0;
	animationStep = totalAnimationDuration / (float)(totalParticles + maxVisibleParticles);

	firstWindParticle = 0;
	lastWindParticle = firstWindParticle - maxVisibleParticles;

	UpdateParticleIndices();
	render = true;
}

void Wind::UpdateParticleIndices()
{
	int adjustedFirstParticle = firstWindParticle;
	if (firstWindParticle >= totalParticles)
	{
		adjustedFirstParticle = totalParticles - 1;
	}
	else if (lastWindParticle < 0)
	{
		lastWindParticle = 0;
	}

	int firstIndex = lastWindParticle * 24 * totalSequences;
	totalSize = (adjustedFirstParticle - lastWindParticle + 1) * 24 * totalSequences;

	glBindVertexArray(vaoID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalSize * sizeof(int), (GLvoid*)&indices[firstIndex], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

#pragma region GenerateFunctions

void Wind::GenerateVertices()
{
	vec3 displacement = vec3(0);
	float lastRotation = 0;
	for (int i = 0; i < totalParticles; i++)
	{
		float loopingRotation = -glm::radians(GetLoopingRotation(i/float(totalParticles)));
		vec3 newDisplacement = glm::rotateX(vec3(0, 0, 1), lastRotation);
		displacement += newDisplacement;

		GenerateAnimationSequence(i * totalSequences, loopingRotation, displacement);
		GenerateAnimationSequence(i * totalSequences + 1, loopingRotation, displacement + vec3(3, -3, -3));
		GenerateAnimationSequence(i * totalSequences + 2, loopingRotation, displacement + vec3(-3, -3, -3));

		lastRotation = loopingRotation;
	}
}

void Wind::GenerateAnimationSequence(int index, float loopingRotation, vec3 displacement)
{
	vertices.push_back(glm::rotateX(vec3(-0.5, 0.5, -0.5), loopingRotation) + displacement);
	vertices.push_back(glm::rotateX(vec3(0.5, 0.5, -0.5), loopingRotation) + displacement);
	vertices.push_back(glm::rotateX(vec3(0.5, 0.5, 0.0), loopingRotation) + displacement);
	vertices.push_back(glm::rotateX(vec3(-0.5, 0.5, 0.0), loopingRotation) + displacement);

	vertices.push_back(glm::rotateX(vec3(-0.5, -0.5, 0.0), loopingRotation) + displacement);
	vertices.push_back(glm::rotateX(vec3(-0.5, -0.5, -0.5), loopingRotation) + displacement);
	vertices.push_back(glm::rotateX(vec3(0.5, -0.5, -0.5), loopingRotation) + displacement);
	vertices.push_back(glm::rotateX(vec3(0.5, -0.5, 0.0), loopingRotation) + displacement);

	indices.push_back(0 + index * 8);
	indices.push_back(3 + index * 8);
	indices.push_back(1 + index * 8);
	indices.push_back(5 + index * 8);
	indices.push_back(3 + index * 8);

	indices.push_back(primitiveRestartIndex);

	indices.push_back(4 + index * 8);
	indices.push_back(5 + index * 8);
	indices.push_back(7 + index * 8);
	indices.push_back(3 + index * 8);
	indices.push_back(5 + index * 8);

	indices.push_back(primitiveRestartIndex);

	indices.push_back(2 + index * 8);
	indices.push_back(1 + index * 8);
	indices.push_back(3 + index * 8);
	indices.push_back(7 + index * 8);
	indices.push_back(1 + index * 8);

	indices.push_back(primitiveRestartIndex);

	indices.push_back(6 + index * 8);
	indices.push_back(7 + index * 8);
	indices.push_back(5 + index * 8);
	indices.push_back(1 + index * 8);
	indices.push_back(7 + index * 8);

	indices.push_back(primitiveRestartIndex);
}

void Wind::GenerateBuffers()
{
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glGenBuffers(2, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), (GLvoid*)&vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int), (GLvoid*)&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

#pragma endregion

#pragma region EntityFunctions

void Wind::Initialize()
{
	try {
		shader.compileShader("../shaders/entities/Wind.vert", GLSLShader::VERTEX);
		shader.compileShader("../shaders/entities/Wind.frag", GLSLShader::FRAGMENT);

		shader.link();
		shader.use();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	this->primitiveRestartIndex = -1;
	glPrimitiveRestartIndex(primitiveRestartIndex);

	GenerateVertices();
	GenerateBuffers();
	UpdateParticleIndices();
}

void Wind::Update(double deltaTime)
{
	if (!render)
	{
		return;
	}

	if (lastWindParticle == totalParticles - 1)
	{
		animationUpdateCount = 0.0;
		firstWindParticle = 0;
		lastWindParticle = firstWindParticle - maxVisibleParticles;
		currentDuration = 0.0;
		render = false;
		return;
	}

	if (animationUpdateCount > animationStep)
	{
		firstWindParticle += 1;
		lastWindParticle = firstWindParticle - maxVisibleParticles;
		UpdateParticleIndices();

		animationUpdateCount = 0.0;
	}

	shader.use();
	shader.setUniform("windColor", windColor);

	UpdateSelfAndChildren(false);

	animationUpdateCount += deltaTime;
	currentDuration += deltaTime;
}

void Wind::Render(mat4 projection, mat4 view)
{
	if (!render)
	{
		return;
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_PRIMITIVE_RESTART_INDEX);

	shader.use();
	mat4 MVPMatrix = projection * view * transform.modelMatrix();

	shader.setUniform("MVP", MVPMatrix);

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLE_FAN, totalSize, GL_UNSIGNED_INT, (GLubyte*)NULL);
	glBindVertexArray(0);
}

#pragma endregion
