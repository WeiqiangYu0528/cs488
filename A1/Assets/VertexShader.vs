#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
in vec3 position;
in vec3 normal;
out vec3 fragPos;
out vec3 normal1;

void main() {
	fragPos = vec3(M * vec4(position, 1.0));
	normal1 = mat3(transpose(inverse(M))) * normal;

    gl_Position = P * V * vec4(fragPos, 1.0);
}
