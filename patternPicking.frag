#version 330 core

// Output data
out vec4 color;

uniform vec4 PickingColor;

void main() {
	color = PickingColor;
}