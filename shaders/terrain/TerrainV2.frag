// Fragment Shader //

#version 400

in vec3 vColor;
in vec3 vNormal;
in vec2 vUV;

uniform float density;
uniform float minNoise;
uniform float maxNoise;
uniform float shellIndex;
uniform float AOAttenuation;
uniform float AOBias;

float RandHash(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	uvec2 seedUV = uvec2(vUV * density);

	float rand = RandHash(seedUV);
	float noise = (1.0 - rand) * minNoise + rand * maxNoise;

	if (noise < shellIndex)
	{
		discard;
	}
	
	float ambientOcclusion = pow(shellIndex, AOAttenuation) + AOBias;
	ambientOcclusion = clamp(ambientOcclusion, 0.0, 1.0);

	gl_FragColor = vec4(vColor, 1.0) * ambientOcclusion;
}