#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifndef TEXTURES
#define TEXTURES

unsigned int create_texture(char* src);
glm::mat4 genTransf(int angle, float length, float y, float x);
int detectCollision(glm::vec4 character[4], glm::vec4 zapper[4], int angle);
int detectCoinCollision(glm::vec4 character[4], glm::vec4 coin[4]);

#endif