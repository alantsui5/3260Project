#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec2 vertexUV;
in layout(location=2) vec3 normal;

out vec2 UV;
out vec3 normalWorld;
out vec3 vertexPositionWorld;

uniform mat4 modelTransformMatrix;
uniform mat4 modelScalingMatrix;
uniform mat4 modelRotationMatrix;
uniform mat4 projectionMatrix;


void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 new_position = modelTransformMatrix * modelScalingMatrix * modelRotationMatrix * v;
	vec4 projectedPosition = projectionMatrix * new_position;
	gl_Position = projectedPosition;

	// vec4 normal_temp = modelTransformMatrix * modelScalingMatrix * modelRotationMatrix* vec4(normal, 0);
	normalWorld = mat3(modelTransformMatrix * modelScalingMatrix * modelRotationMatrix) * normal;
	vertexPositionWorld = new_position.xyz;

	UV = vertexUV;
}
