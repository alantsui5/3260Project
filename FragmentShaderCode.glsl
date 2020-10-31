#version 430

in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform sampler2D myTextureSampler0;

// Special Light
uniform vec4 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;

//Directional Light
uniform vec4 ambientLight0;
uniform vec3 lightPositionWorld0;
uniform vec3 eyePositionWorld0;

out vec4 color;

struct SpecialLight{

	vec3 lightPosition;
	vec3 eyePosition;
};
uniform SpecialLight specialLight;

struct MultiLight
{
	
    vec3 position;
    vec3 color;
};
#define LIGHT_NUM 12
uniform MultiLight littleLight[LIGHT_NUM];

void main()
{
	vec3 LightColor = vec3(1,1,1);

	vec4 MaterialAmbientColor = texture( myTextureSampler0, UV ).rgba;
	vec4 MaterialDiffuseColor = texture( myTextureSampler0, UV ).rgba;

	//Diffuse
	vec3 lightVectorWorld = normalize(specialLight.lightPosition - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	//Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
	vec3 eyeVectorWorld = normalize(specialLight.eyePosition - vertexPositionWorld);
	float s =clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	s = pow(s, 50);
	vec4 specularLight = vec4(s , s, s, 1);

	//dir light
	//Diffuse
	vec3 lightVectorWorld0 = normalize(lightPositionWorld0 - vertexPositionWorld);
	float brightness0 = dot(lightVectorWorld0, normalize(normalWorld));
	vec4 diffuseLight0 = vec4(brightness0, brightness0, brightness0, 1.0);

	//Specular
	vec3 reflectedLightVectorWorld0 = reflect(-lightVectorWorld0, normalWorld);
	vec3 eyeVectorWorld0 = normalize(eyePositionWorld0 - vertexPositionWorld);
	float s1 =clamp(dot(reflectedLightVectorWorld0, eyeVectorWorld0), 0, 1);
	s1 = pow(s1, 50);
	vec4 specularLight0 = vec4(s1 , s1, s1, 1);

	vec4 multi_result;
	for(int i = 0; i < LIGHT_NUM; ++i)
    {
        vec3 lightVectorWorld2 = normalize(littleLight[i].position - vertexPositionWorld);
		float brightness2 = dot(lightVectorWorld2, normalize(normalWorld));
		vec4 diffuseLight2 = vec4(brightness2, brightness2, brightness2, 1.0) * vec4(littleLight[i].color, 1.0f);

		vec3 reflectedLightVectorWorld2 = reflect(-lightVectorWorld2, normalWorld);
		vec3 eyeVectorWorld2 = normalize(eyePositionWorld0 - vertexPositionWorld);
		float s2 =clamp(dot(reflectedLightVectorWorld2, eyeVectorWorld2), 0, 1);
		s2 = pow(s2, 50);
		vec4 specularLight2 = vec4(s2, s2, s2, 1)  * vec4(littleLight[i].color, 1.0f);
		multi_result += diffuseLight2 * 0.6f + specularLight2 * 0.3f  ;
    }

	 color = 
		MaterialDiffuseColor * clamp(diffuseLight, 0, 1) * 1.5f+
		specularLight +
		MaterialAmbientColor * 0.2f +
		MaterialAmbientColor * ambientLight0 + 
		MaterialDiffuseColor * clamp(diffuseLight0, 0, 1) * 0.5f+
		specularLight0 * MaterialAmbientColor *0.5f;
}
