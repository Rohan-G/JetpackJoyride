#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "textures.h"

unsigned int create_texture(char* src){
    int w,h,nr;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* img = stbi_load(src,&w,&h,&nr,0);
    // unsigned char* bg2 = stbi_load("./bg2.jpeg",&w2,&h2,&nr2,0);
    
    unsigned int texture;
    
    glGenTextures(1,&texture);
    // glGenTextures(1,&texture2);

    glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // std::cout << nr << "\n";

    if (img)
    {
        if(nr==3){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        }
        else if(nr==4){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(img);

    return texture;
}

glm::mat4 genTransf(int angle, float length, float y, float x){
    glm::mat4 trans1, rot1, trans2,scale,trans3;
    trans1 = glm::translate(trans1, glm::vec3(-1 * x, 0.0f, 0.0f));
    scale = glm::scale(scale,glm::vec3(1.0f, length, 1.0f));
    rot1 = glm::rotate(rot1, glm::radians((float)angle*10), glm::vec3(0.0f, 0.0f, 1.0f));
    trans2 = glm::translate(trans2, glm::vec3(x, y, 0.0f));
    return trans2*rot1*scale*trans1;

}

int detectCollision(glm::vec4 character[4], glm::vec4 zapper[4], int angle){
    // std::cout << character[1].x << " " << character[1].y << " " << character[0].y << std::endl;
    if(zapper[0].x >= character[0].x && zapper[0].x<=character[3].x){
        if(zapper[0].y >= character[0].y && zapper[0].y <= character[1].y){
            // std::cout << "1" <<std::endl;
            return 1;
        }
    }
    if(zapper[1].x >= character[0].x && zapper[1].x<=character[3].x){
        if(zapper[1].y >= character[0].y && zapper[1].y <= character[1].y){
            // std::cout << "2" <<std::endl;
            return 1;
        }
    }
    if(zapper[2].x >= character[0].x && zapper[2].x<=character[3].x){
        if(zapper[2].y >= character[0].y && zapper[2].y <= character[1].y){
            // std::cout << "3" <<std::endl;
            return 1;
        }
    }
    if(zapper[3].x >= character[0].x && zapper[3].x<=character[3].x){
        if(zapper[3].y >= character[0].y && zapper[3].y <= character[1].y){
            // std::cout << "4" <<std::endl;
            return 1;
        }
    }

    glm::mat4 rot, trans1, trans2, trans_char1, trans_char2;
    rot = glm::rotate(rot, glm::radians((float)angle*-10), glm::vec3(0.0f, 0.0f, 1.0f));

    float zapper_x = (zapper[0].x + zapper[3].x + zapper[2].x + zapper[1].x)/ 4;
    float zapper_y = (zapper[0].y + zapper[1].y  + zapper[2].y + zapper[3].y)/ 4;
    trans1 = glm::translate(trans1, glm::vec3(-1 * zapper_x, -1 * zapper_y, 0.0f));
    trans2 = glm::translate(trans2, glm::vec3(zapper_x, zapper_y, 0.0f));
    float char_x = (character[0].x + character[3].x )/ 2;
    float char_y = (character[0].y + character[1].y )/ 2;
    // std::cout << char_x << " " << char_y << std::endl;
    trans_char1 = glm::translate(trans_char1, glm::vec3(-1 * char_x, -1 * char_y, 0.0f));
    trans_char2 = glm::translate(trans_char2, glm::vec3(char_x, char_y, 0.0f));

    glm::vec4 new_ch[4] = {trans_char1 * rot * trans_char2 * character[0], trans_char1 * rot * trans_char2 * character[1], trans_char1 * rot * trans_char2 * character[2], trans_char1 * rot * trans_char2 * character[3]};
    glm::vec4 new_zp[4] = {trans1 * rot * trans2 * zapper[0], trans1 * rot * trans2 * zapper[1], trans1 * rot * trans2 * zapper[2], trans1 * rot * trans2 * zapper[3]};

    if(new_ch[0].x >= new_zp[0].x && new_ch[0].x<=new_zp[3].x){
        if(new_ch[0].y >= new_zp[0].y && new_ch[0].y <= new_zp[1].y){
            // std::cout << "5" <<std::endl;
            return 1;
        }
    }
    if(new_ch[1].x >= new_zp[0].x && new_ch[1].x<=new_zp[3].x){
        if(new_ch[1].y >= new_zp[0].y && new_ch[1].y <= new_zp[1].y){
            // std::cout << "6" <<std::endl;
            return 1;
        }
    }
    if(new_ch[2].x >= new_zp[0].x && new_ch[2].x<=new_zp[3].x){
        if(new_ch[2].y >= new_zp[0].y && new_ch[2].y <= new_zp[1].y){
            // std::cout << "7" <<std::endl;
            return 1;
        }
    }
    if(new_ch[3].x >= new_zp[0].x && new_ch[3].x<=new_zp[3].x){
        if(new_ch[3].y >= new_zp[0].y && new_ch[3].y <= new_zp[1].y){
            // std::cout << "8" <<std::endl;
            return 1;
        }
    }

    return 0;
}

int detectCoinCollision(glm::vec4 character[4], glm::vec4 coin[4]){
    if(coin[0].x >= character[0].x && coin[0].x<=character[3].x){
        if(coin[0].y >= character[0].y && coin[0].y <= character[1].y){
            // std::cout << "1" <<std::endl;
            return 1;
        }
    }
    if(coin[1].x >= character[0].x && coin[1].x<=character[3].x){
        if(coin[1].y >= character[0].y && coin[1].y <= character[1].y){
            // std::cout << "2" <<std::endl;
            return 1;
        }
    }
    if(coin[2].x >= character[0].x && coin[2].x<=character[3].x){
        if(coin[2].y >= character[0].y && coin[2].y <= character[1].y){
            // std::cout << "3" <<std::endl;
            return 1;
        }
    }
    if(coin[3].x >= character[0].x && coin[3].x<=character[3].x){
        if(coin[3].y >= character[0].y && coin[3].y <= character[1].y){
            // std::cout << "4" <<std::endl;
            return 1;
        }
    }
    return 0;
}