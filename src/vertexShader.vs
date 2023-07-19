#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTex;

out vec4 outCol;
out vec2 outTex;
uniform mat4 model;
uniform int isZapper;
uniform float x;

void main()
{
   outCol = vec4(aCol,1.0f);
   gl_Position = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   outTex = aTex;
}