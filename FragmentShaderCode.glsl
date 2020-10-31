#version 430

in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform sampler2D myTextureSampler0;

// Special Light
uniform vec4 ambientLight;

struct SpecialLight{
	vec4 ambient;
	vec3 lightPosition;
	vec3 eyePosition;
};
uniform SpecialLight specialLight;

//Directional Light
struct DirectionalLight{
	vec4 ambient;
	vec3 lightPosition;
	vec3 eyePosition;
};
uniform DirectionalLight directionalLight;

out vec4 color;

void main()
{
	vec3 LightColor = vec3(1,1,1);

	vec4 MaterialAmbientColor = texture( myTextureSampler0, UV ).rgba;
	vec4 MaterialDiffuseColor = texture( myTextureSampler0, UV ).rgba;

	//Diffuse
	vec3 lightVectorWorld = normalize(specialLight.lightPosition - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1);

	//Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
	vec3 eyeVectorWorld = normalize(specialLight.eyePosition - vertexPositionWorld);
	float s =clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	s = pow(s, 50);
	vec4 specularLight = vec4(s , s, s, 1);

	//Directional Light
	//Diffuse
	vec3 lightVectorWorld0 = normalize(directionalLight.lightPosition - vertexPositionWorld);
	float brightness0 = dot(lightVectorWorld0, normalize(normalWorld));
	vec4 diffuseLight0 = vec4(brightness0, brightness0, brightness0, 1.0);

	//Specular
	vec3 reflectedLightVectorWorld0 = reflect(-lightVectorWorld0, normalWorld);
	vec3 eyeVectorWorld0 = normalize(directionalLight.eyePosition - vertexPositionWorld);
	float s1 =clamp(dot(reflectedLightVectorWorld0, eyeVectorWorld0), 0, 1);
	s1 = pow(s1, 50);
	vec4 specularLight0 = vec4(s1 , s1, s1, 1);

	 color = 
		MaterialAmbientColor * specialLight.ambient +
		MaterialDiffuseColor * clamp(diffuseLight, 0, 1) * 1.5f+
		specularLight +
		MaterialAmbientColor * 0.2f +
		MaterialAmbientColor * directionalLight.ambient + 
		MaterialDiffuseColor * clamp(diffuseLight0, 0, 1) * 0.5f+
		specularLight0 * MaterialAmbientColor *0.5f;
}
