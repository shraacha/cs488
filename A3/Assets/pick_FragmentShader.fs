#version 330

uniform vec3 objCol;

out vec4 fragColour;

void main() {
	fragColour = vec4(objCol, 1.0);
}
