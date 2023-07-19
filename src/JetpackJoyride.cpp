#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "textures.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <shader.h>
#include <bits/stdc++.h>
#include <ft2build.h>
#include FT_FREETYPE_H

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);

// settings
const unsigned int SCR_WIDTH = 2048;
const unsigned int SCR_HEIGHT = 1024;
const unsigned int nZappers = 2;
const unsigned int nBackpLevel = 8;

float bgSpeed = 0.009;

// physics
float y = 0;
float max_y = 1.52;
float v = 0;
float up_a = 0.0009;
float down_a = 0.0007;

int ZapRotSpeed = 1;

int flagFlying = 0;

int levelLength = 223 * nBackpLevel;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;
unsigned int textVAO, textVBO;

// orthographic projection
glm::mat4 chartrans = glm::mat4(1.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Jetpack Joyride Clone", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("../src/vertexShader.vs", "../src/fragmentShader.fs");

    glm::vec4 character[4] = {glm::vec4(-0.75f, -0.82f, 0.0f, 1.0f), glm::vec4(-0.75f, -0.62f, 0.0f, 1.0f), glm::vec4(-0.65f, -0.62f, 0.0f, 1.0f), glm::vec4(-0.65f, -0.82f, 0.0f, 1.0f)};

    glm::vec4 zapper1[4] = {glm::vec4(-0.55f, -0.35f, 0.0f, 1.0f), glm::vec4(-0.55f,  0.35f, 0.0f, 1.0f), glm::vec4(-0.47f,  0.35f, 0.0f, 1.0f), glm::vec4(-0.47f, -0.35f, 0.0f, 1.0f)};
    glm::vec4 zapper2[4] = {glm::vec4(0.47f, -0.35f, 0.0f, 1.0f), glm::vec4(0.47f,  0.35f, 0.0f, 1.0f), glm::vec4(0.55f,  0.35f, 0.0f, 1.0f), glm::vec4(0.55f, -0.35f, 0.0f, 1.0f)};
    glm::vec4 zapper3[4] = {glm::vec4(1.47f, -0.35f, 0.0f, 1.0f), glm::vec4(1.47f,  0.35f, 0.0f, 1.0f), glm::vec4(1.55f,  0.35f, 0.0f, 1.0f), glm::vec4(1.55f, -0.35f, 0.0f, 1.0f)};
    glm::vec4 zapper4[4] = {glm::vec4(2.47f, -0.35f, 0.0f, 1.0f), glm::vec4(2.47f,  0.35f, 0.0f, 1.0f), glm::vec4(2.55f,  0.35f, 0.0f, 1.0f), glm::vec4(2.55f, -0.35f, 0.0f, 1.0f)};

    glm::vec4 coin1[4] = {glm::vec4(-0.47f, -0.5f,  0.0f,1.0f), glm::vec4(-0.47f, -0.4f, 0.0f,1.0f), glm::vec4(-0.52f, -0.4f, 0.0f,1.0f), glm::vec4(-0.52f, -0.5f, 0.0f,1.0f)};
    glm::vec4 coin2[4] = {glm::vec4(-0.025f, -0.05f, 0.0f, 1.0f), glm::vec4(-0.025f, 0.05f, 0.0f, 1.0f), glm::vec4(0.025f, 0.05f, 0.0f, 1.0f), glm::vec4(0.025f, -0.05f, 0.0f, 1.0)};
    glm::vec4 coin3[4] = {glm::vec4(0.47f, 0.5f,  0.0f, 1.0f), glm::vec4(0.47f, 0.4f, 0.0f, 1.0), glm::vec4(0.52f, 0.4f, 0.0f, 1.0), glm::vec4(0.52f, 0.5f, 0.0f, 1.0)};
    glm::vec4 coin4[4] = {glm::vec4(1.47f, -0.5f,  0.0f,1.0f), glm::vec4(1.47f, -0.4f, 0.0f,1.0f), glm::vec4(1.52f, -0.4f, 0.0f,1.0f), glm::vec4(1.52f, -0.5f, 0.0f,1.0f)};
    glm::vec4 coin5[4] = {glm::vec4(1.975f, -0.05f, 0.0f, 1.0f), glm::vec4(1.975f, 0.05f, 0.0f, 1.0f), glm::vec4(2.025f, 0.05f, 0.0f, 1.0f), glm::vec4(2.025f, -0.05f, 0.0f, 1.0)};
    glm::vec4 coin6[4] = {glm::vec4(2.47f, 0.5f,  0.0f, 1.0f), glm::vec4(2.47f, 0.4f, 0.0f, 1.0), glm::vec4(2.52f, 0.4f, 0.0f, 1.0), glm::vec4(2.52f, 0.5f, 0.0f, 1.0)};


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[416] = {
        -1.0f, -1.0f, 0.0f, 0.0, 0.0, 0.0, 0.0, 0.0,  
         1.0f, -1.0f, 0.0f, 0.0, 0.0, 0.0, 1.0, 0.0, 
        -1.0f,  1.0f, 0.0f, 0.0, 0.0, 0.0, 0.0, 1.0,
         1.0f,  1.0f, 0.0f, 0.0, 0.0, 0.0, 1.0, 1.0,    // background 1
         1.0f,  1.0f, 0.0f, 0.0, 0.0, 0.0, 0.0, 1.0,
         1.0f, -1.0f, 0.0f, 0.0, 0.0, 0.0, 0.0, 0.0,
         3.0f, -1.0f, 0.0f, 0.0, 0.0, 0.0, 1.0, 0.0,
         3.0f,  1.0f, 0.0f, 0.0, 0.0, 0.0, 1.0, 1.0,    // background 2
        -0.75f, -0.82f, 0.0f, 1.0, 1.0, 1.0, 0.0, 0.0,
        -0.75f, -0.62f, 0.0f, 1.0, 1.0, 1.0, 0.0, 1.0,
        -0.65f, -0.62f, 0.0f, 1.0, 1.0, 1.0, 1.0, 1.0,
        -0.65f, -0.82f, 0.0f, 1.0, 1.0, 1.0, 1.0, 0.0,  // character
        -0.55f, -0.35f, 0.0f, 1.0, 1.0, 0.0, 0.0f, 0.0f,
        -0.55f,  0.35f, 0.0f, 1.0, 1.0, 0.0, 0.0f, 1.0f,
        -0.47f,  0.35f, 0.0f, 1.0, 1.0, 0.0, 1.0f, 1.0f,
        -0.47f, -0.35f, 0.0f, 1.0, 1.0, 0.0, 1.0f, 0.0f,  // zapper 1
         0.47f, -0.35f, 0.0f, 1.0, 1.0, 0.0, 0.0f, 0.0f,
         0.47f,  0.35f, 0.0f, 1.0, 1.0, 0.0, 0.0f, 1.0f,
         0.55f,  0.35f, 0.0f, 1.0, 1.0, 0.0, 1.0f, 1.0f,
         0.55f, -0.35f, 0.0f, 1.0, 1.0, 0.0, 1.0f, 0.0f,  // zapper 2
         1.47f, -0.35f, 0.0f, 1.0, 1.0, 0.0, 0.0f, 0.0f,
         1.47f,  0.35f, 0.0f, 1.0, 1.0, 0.0, 0.0f, 1.0f,
         1.55f,  0.35f, 0.0f, 1.0, 1.0, 0.0, 1.0f, 1.0f,
         1.55f, -0.35f, 0.0f, 1.0, 1.0, 0.0, 1.0f, 0.0f,  // zapper 3
         2.47f, -0.35f, 0.0f, 1.0, 1.0, 0.0, 0.0f, 0.0f,
         2.47f,  0.35f, 0.0f, 1.0, 1.0, 0.0, 0.0f, 1.0f,
         2.55f,  0.35f, 0.0f, 1.0, 1.0, 0.0, 1.0f, 1.0f,
         2.55f, -0.35f, 0.0f, 1.0, 1.0, 0.0, 1.0f, 0.0f,   // zapper 4
        -0.47f, -0.5f,  0.0f, 1.0f, 0.843f, 0.0f, 0.0f, 0.0f,
        -0.47f, -0.4f, 0.0f, 1.0, 0.843, 0.0, 0.0f, 1.0f,
        -0.52f, -0.4f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 1.0f,
        -0.52f, -0.5f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 0.0f,   //coin 1
        -0.025f, -0.05f, 0.0f, 1.0, 0.843, 0.0, 0.0f, 0.0f,
        -0.025f, 0.05f, 0.0f, 1.0, 0.843, 0.0, 0.0f, 1.0f,
         0.025f, 0.05f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 1.0f,
        0.025f, -0.05f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 0.0f,  //coin 2
        0.47f, 0.5f,  0.0f, 1.0f, 0.843f, 0.0f, 0.0f, 0.0f,
        0.47f, 0.4f, 0.0f, 1.0, 0.843, 0.0, 0.0f, 1.0f,
        0.52f, 0.4f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 1.0f,
        0.52f, 0.5f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 0.0f,     //coin 3
        1.47f, -0.5f,  0.0f, 1.0f, 0.843f, 0.0f, 0.0f, 0.0f,
        1.47f, -0.4f, 0.0f, 1.0, 0.843, 0.0, 0.0f, 1.0f,
        1.52f, -0.4f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 1.0f,
        1.52f, -0.5f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 0.0f,   //coin 4
        1.975f, -0.05f, 0.0f, 1.0, 0.843, 0.0, 0.0f, 0.0f,
        1.975f, 0.05f, 0.0f, 1.0, 0.843, 0.0, 0.0f, 1.0f,
        2.025f, 0.05f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 1.0f,
        2.025f, -0.05f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 0.0f,  //coin 5
        2.47f, 0.5f,  0.0f, 1.0f, 0.843f, 0.0f, 0.0f, 0.0f,
        2.47f, 0.4f, 0.0f, 1.0, 0.843, 0.0, 0.0f, 1.0f,
        2.52f, 0.4f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 1.0f,
        2.52f, 0.5f, 0.0f, 1.0, 0.843, 0.0, 1.0f, 0.0f,     //coin 6
    };
    unsigned int indices[78] = {
        0, 1, 2,
        1, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 11,
        9, 10, 11,
        12, 13, 15,
        13, 14, 15,
        16, 17, 19,
        17, 18 ,19,
        20, 21, 23,
        21, 22, 23,
        24, 25, 27,
        25, 26, 27,
        28, 29, 31,
        29, 30, 31,
        32, 33, 35,
        33, 34, 35,
        36, 37, 39,
        37, 38, 39,
        40, 41, 43,
        41, 42, 43,
        44, 45, 47,
        45, 46, 47,
        48, 49, 51,
        49, 50, 51
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 416 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 78 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // Font Stuff
    Shader shader("../src/text.vs", "../src/text.fs");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

	// find path to font
    std::string font_name = "../src/Antonio-Bold.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }
	
	// load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Texture stuff
    
    unsigned int bgtexture = create_texture((char*)"../src/bg.jpeg");
    unsigned int chartexture;
    unsigned int zappertexture = create_texture((char*)"../src/Zapper.png");
    unsigned int cointexture = create_texture((char*)"../src/Coin.png");
    unsigned int losetexture = create_texture((char*)"../src/lose.jpg");
    unsigned int wintexture = create_texture((char*)"../src/win.jpg");

    // glBindTexture(GL_TEXTURE_2D,texture2);-
    
    int c = 0;

    char sources[4][30] = {"../src/Left.png", "../src/Neutral.png", "../src/Right.png", "../src/Neutral.png"};
    int imgIndex = 0;
    int x = 0;

    glm::mat4 trans1, trans2, trans3, trans4;
    glm::mat4 transc1, transc2, transc3, transc4, transc5, transc6;

    float h1,h2,h3,h4;
    int angle1, angle2, angle3, angle4;
    float y1, y2, y3, y4;
    float yc1, yc2, yc3, yc4, yc5, yc6;
    srand(time(NULL));

    int nRot = 7;
    int countZ = 2;

    // For each zapper, translate to origin, perform scaling and rotation and then move back

    int flag1=0,flag2=0,flag3=0,flag4=0;

    h1 = (float)rand()/RAND_MAX;
    angle1 = rand()%18;
    // h1 = 3*h1/4;
    h1 = 0.5 + h1/2;
    y1 = (float)rand()/RAND_MAX;
    y1 = y1/2;
    yc1 = (float)rand()/RAND_MAX;
    yc1 = yc1*0.4;
    yc1 = yc1 - 0.2;
    transc1 = glm::translate(transc1, glm::vec3(0.0f, yc1, 0.0f));
    trans1 = genTransf(angle1, h1, y1, -0.51);
    
    h2 = (float)rand()/RAND_MAX;
    angle2 = rand()%18;
    // h2 = 3*h2/4;
    h2 = 0.5 + h2/2;
    yc2 = (float)rand()/RAND_MAX;
    yc2 = yc2*1.5;
    yc2 = yc2 - 0.75;
    transc2 = glm::translate(transc2, glm::vec3(0.0f, yc2, 0.0f));
    y2 = (float)rand()/RAND_MAX;
    y2 = -1*y2/2;
    trans2 = genTransf(angle2, h2, y2, 0.51);
    
    h3 = (float)rand()/RAND_MAX;
    angle3 = rand()%18;
    // h3 = 3*h3/4;
    h3 = 0.5 + h3/2;
    yc3 = (float)rand()/RAND_MAX;
    yc3 = yc3*0.4;
    yc3 = yc3 - 0.2;
    transc3 = glm::translate(transc3, glm::vec3(0.0f, yc3, 0.0f));
    y3 = (float)rand()/RAND_MAX;
    y3 = y3/2;
    trans3 = genTransf(angle3, h3, y3, 1.51);
    
    h4 = (float)rand()/RAND_MAX;
    angle4 = rand()%18;
    // h4 = 3*h4/4;
    h4 = 0.5 + h4/2;
    y4 = (float)rand()/RAND_MAX;
    y4 = -1*y4/2;
    trans4 = genTransf(angle4, h4, y4, 2.51);

    yc4 = (float)rand()/RAND_MAX;
    yc4 = yc4*0.4;
    yc4 = yc4 - 0.2;
    transc4 = glm::translate(transc4, glm::vec3(0.0f, yc4, 0.0f));

    yc5 = (float)rand()/RAND_MAX;
    yc5 = yc5*1.5;
    yc5 = yc5 - 0.75;
    transc5 = glm::translate(transc5, glm::vec3(0.0f, yc5, 0.0f));

    yc6 = (float)rand()/RAND_MAX;
    yc6 = yc6*0.4;
    yc6 = yc6 - 0.2;
    transc6 = glm::translate(transc6, glm::vec3(0.0f, yc6, 0.0f));


    // std::cout << h1 << " " << h2 << " " << h3 << " " << h4 << "\n";

    int flagZap1 = 0;
    int flagZap2 = 0;

    int hitByZapper = 0;

    int nBackRep = 0;

    int score = 0;
    int level = 1;

    int lose = 0;
    int won = 0;

    int progress = 0;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our background
        // ------------
        ourShader.use();

        if(won){
            glBindTexture(GL_TEXTURE_2D,wintexture);
            glBindVertexArray(VAO);
            ourShader.setMat4("model", glm::mat4(1.0f));
            ourShader.setFloat("isZapper", 0);
            ourShader.setInt("isFlying",0);
            ourShader.setInt("needsTexture",1);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            std::string val = "Score: " + std::to_string(score);
            RenderText(shader, val, 890.0f, 100.0f, 2.0, glm::vec3(0.5f, 1.0f, 0.0f));

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        else if(lose){
            glBindTexture(GL_TEXTURE_2D,losetexture);
            glBindVertexArray(VAO);
            ourShader.setMat4("model", glm::mat4(1.0f));
            ourShader.setFloat("isZapper", 0);
            ourShader.setInt("isFlying",0);
            ourShader.setInt("needsTexture",1);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            std::string val = "Score: " + std::to_string(score);
            RenderText(shader, val, 890.0f, 100.0f, 2.0, glm::vec3(0.5f, 1.0f, 0.0f));

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        ourShader.setInt("needsTexture",0);

        // glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bgtexture);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3((-1*bgSpeed*c), 0.0f, 0.0f));

        if(c==0){
            if(!flagZap1){
                flagZap1 = 1;
            }
            else{
                flagZap2 = 1;
            
                srand(time(NULL));
            
                h1 = h3;
                angle1 = angle3;
                y1 = y3;
                flag1 = flag3;
                trans1 = genTransf(angle1, h1, y1, -0.51);
                
                h2 = h4;
                angle2 = angle4;
                y2 = y4;
                flag2 = flag4;
                trans2 = genTransf(angle2, h2, y2, 0.51);
                
                h3 = (float)rand()/RAND_MAX;
                // h3 = 3*h3/4;
                h3 = 0.5 + h3/2;
                angle3 = rand()%18;
                y3 = (float)rand()/RAND_MAX;
                y3 = y3/2;
                trans3 = genTransf(angle3, h3, y3, 1.51);
                countZ=(countZ+1)%nRot;
                if(flag3==1){
                    flag3=0;
                }
                if(countZ==0){
                    flag3=1;
                    // std::cout << nZappers << std::endl;
                }
                
                h4 = (float)rand()/RAND_MAX;
                angle4 = rand()%18;
                // h4 = 3*h4/4;
                h4 = 0.5 + h4/2;
                y4 = (float)rand()/RAND_MAX;
                y4 = -1*y4/2;
                trans4 = genTransf(angle4, h4, y4, 2.51);
                countZ=(countZ+1)%nRot;
                if(flag4==1){
                    flag4=0;
                }
                if(countZ==0){
                    flag4=1;
                    // std::cout << nRot << std::endl;
                }

                transc1 = transc4;
                yc1 = yc4;

                transc2 = transc5;
                yc2 = yc5;

                transc3 = transc6;
                yc3 = yc6;

                yc4 = (float)rand()/RAND_MAX;
                yc4 = yc4*0.4;
                yc4 = yc4 - 0.2;
                transc4 = glm::mat4(1.0f);
                transc4 = glm::translate(transc4, glm::vec3(0.0f, yc4, 0.0f));

                yc5 = (float)rand()/RAND_MAX;
                yc5 = yc5*1.5;
                yc5 = yc5 - 0.75;
                transc5 = glm::mat4(1.0f);
                transc5 = glm::translate(transc5, glm::vec3(0.0f, yc5, 0.0f));

                yc6 = (float)rand()/RAND_MAX;
                yc6 = yc6*0.4;
                yc6 = yc6 - 0.2;
                transc6 = glm::mat4(1.0f);
                transc6 = glm::translate(transc6, glm::vec3(0.0f, yc6, 0.0f));

                nBackRep = (nBackRep + 1) % nBackpLevel;
                if(nBackRep == 0){
                    flagZap2 = 0;
                    bgSpeed *= 1.5;
                    nRot-=2;
                    level++;
                    progress = 0;
                    if(level==4){
                        // std::cout << "Congrats!!! You won\n";
                        // glfwSetWindowShouldClose(window,true);
                        won = 1;
                        continue;
                    }
                    // std::cout << levelLength;
                }
            }
            // std::cout << h1 << " " << h2 << " " << h3 << " " << h4 << "\n";
            // std::cout << "trans1: " << glm::to_string(trans1) << std::endl;
            // std::cout << "trans2: " << glm::to_string(trans2) << std::endl;
            // std::cout << "trans3: " << glm::to_string(trans3) << std::endl;
            // std::cout << "trans4: " << glm::to_string(trans4) << std::endl;
        }

        c = (c + 1)%((int)(2/bgSpeed) + 1);

        // glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"), 1, GL_FALSE, &trans[0][0]);
        ourShader.setMat4("model",trans);
        ourShader.setInt("isZapper",0);
        ourShader.setInt("isFlying",0);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time
        glBindTexture(GL_TEXTURE_2D,0);
        // -----------

        // draw our character
        // ----------
        // glActiveTexture(GL_TEXTURE0);
        if(y==0){
            if(x%11==0){
                chartexture = create_texture(sources[imgIndex]);
                imgIndex = (imgIndex + 1) % 4;
            }
            x = (x + 1)%11;
        }
        else{
            chartexture = create_texture((char*)"../src/Fly.png");
            x=0;
            imgIndex = 2;
        }
        glBindTexture(GL_TEXTURE_2D,chartexture);
        ourShader.setMat4("model",chartrans);
        ourShader.setInt("isZapper",0);
        if(flagFlying){
            ourShader.setInt("isFlying",1);
        }
        else{
            ourShader.setInt("isFlying",0);
        }
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(12 * sizeof(unsigned int)));
        // ---------

        // draw zappers
        // ------------
        // glm::mat4 trans1 = glm::mat4(1.0f);
        // // trans1 = glm::translate(trans1, glm::vec3(0.0f, 0.75f, 0.0f));
        // trans = trans1 * trans;
        // std::cout << y - 0.72 << std::endl;
        glm::vec4 new_ch[4] = {chartrans*character[0], chartrans*character[1], chartrans*character[2], chartrans*character[3]};
        glBindTexture(GL_TEXTURE_2D, zappertexture);
        // ourShader.use();
        // ourShader.setMat4("model", trans);
        if(flagZap2){
            ourShader.setMat4("model",trans * trans1);
            ourShader.setInt("isZapper",1);
            ourShader.setInt("isFlying",0);
            ourShader.setFloat("x",-0.51f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(18 * sizeof(unsigned int)));
            glm::vec4 new_zap1[4] = {trans*trans1*zapper1[0], trans*trans1*zapper1[1], trans*trans1*zapper1[2], trans*trans1*zapper1[3]};
            if(detectCollision(new_ch, new_zap1, angle1)){
                // glfwSetWindowShouldClose(window,true);
                lose = 1; 
            }
            if(flag1){
                angle1 = (angle1 + ZapRotSpeed)%360;
                trans1 = genTransf(angle1,h1,y1,-0.51);
            }


            ourShader.setMat4("model",trans * trans2);
            ourShader.setInt("isZapper",1);
            ourShader.setInt("isFlying",0);
            ourShader.setFloat("x", 0.51);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(24 * sizeof(unsigned int)));
            glm::vec4 new_zap2[4] = {trans*trans2*zapper2[0], trans*trans2*zapper2[1], trans*trans2*zapper2[2], trans*trans2*zapper2[3]};
            if(detectCollision(new_ch, new_zap2, angle2)){
                // glfwSetWindowShouldClose(window,true);
                lose = 1; 
            }
            if(flag2){
                angle2 = (angle2 + ZapRotSpeed)%360;
                trans2 = genTransf(angle2,h2,y2,0.51);
            }
        }
        ourShader.setMat4("model",trans * trans3);
        ourShader.setInt("isZapper",1);
        ourShader.setInt("isFlying",0);
        ourShader.setFloat("x", 1.51);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(30 * sizeof(unsigned int)));
        glm::vec4 new_zap3[4] = {trans*trans3*zapper3[0], trans*trans3*zapper3[1], trans*trans3*zapper3[2], trans*trans3*zapper3[3]};
        if(detectCollision(new_ch, new_zap3, angle3)){
            // glfwSetWindowShouldClose(window,true);
            lose = 1;
        }
        if(flag3){
            angle3 = (angle3 + ZapRotSpeed)%360;
            // std::cout << ZapRotSpeed << std::endl;
            trans3 = genTransf(angle3,h3,y3,1.51);
        }

        ourShader.setMat4("model",trans * trans4);
        ourShader.setInt("isZapper",1);
        ourShader.setInt("isFlying",0);
        ourShader.setFloat("x",2.51);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(36 * sizeof(unsigned int)));
        glm::vec4 new_zap4[4] = {trans*trans4*zapper4[0], trans*trans4*zapper4[1], trans*trans4*zapper4[2], trans*trans4*zapper4[3]};
        if(detectCollision(new_ch, new_zap4, angle4)){
            // glfwSetWindowShouldClose(window,true);
            lose = 1; 
        }
        if(flag4){
            angle4 = (angle4 + ZapRotSpeed)%360;
            trans4 = genTransf(angle4,h4,y4,2.51);
        }
        // --------------

        // draw coins
        // // ---------------------------------------
        // ourShader.use();
        glBindTexture(GL_TEXTURE_2D, cointexture);
        if(flagZap2){
            glm::vec4 new_coin1[4] =  {trans*transc1*coin1[0], trans*transc1*coin1[1], trans*transc1*coin1[2], trans*transc1*coin1[3]};
            if(detectCoinCollision(new_ch,new_coin1)){
                score++;
                transc1 = glm::mat4(1.0f);
                yc1 = -2.0f;
                transc1 = glm::translate(transc1, glm::vec3(0.0f, -2.0f, 0.0f));
            }
            ourShader.setMat4("model",trans * transc1);
            ourShader.setInt("isZapper",0);
            ourShader.setInt("isFlying",0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(42 * sizeof(unsigned int)));

            glm::vec4 new_coin2[4] =  {trans*transc2*coin2[0], trans*transc2*coin2[1], trans*transc2*coin2[2], trans*transc2*coin2[3]};
            if(detectCoinCollision(new_ch,new_coin2)){
                score++;
                transc2 = glm::mat4(1.0f);
                yc2 = -2.0f;
                transc2 = glm::translate(transc2, glm::vec3(0.0f, -2.0f, 0.0f));
            }
            ourShader.setMat4("model",trans * transc2);
            ourShader.setInt("isZapper",0);
            ourShader.setInt("isFlying",0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(48 * sizeof(unsigned int)));

            glm::vec4 new_coin3[4] =  {trans*transc3*coin3[0], trans*transc3*coin3[1], trans*transc3*coin3[2], trans*transc3*coin3[3]};
            if(detectCoinCollision(new_ch,new_coin3)){
                score++;
                yc3 = -2.0f;
                transc3 = glm::mat4(1.0f);
                transc3 = glm::translate(transc3, glm::vec3(0.0f, -2.0f, 0.0f));
            }
            ourShader.setMat4("model",trans * transc3);
            ourShader.setInt("isZapper",0);
            ourShader.setInt("isFlying",0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(54 * sizeof(unsigned int)));
        }

        glm::vec4 new_coin4[4] =  {trans*transc4*coin4[0], trans*transc4*coin4[1], trans*transc4*coin4[2], trans*transc4*coin4[3]};
        if(detectCoinCollision(new_ch,new_coin4)){
            score++;
            transc4 = glm::mat4(1.0f);
            yc4 = -2.0f;
            transc4 = glm::translate(transc4, glm::vec3(0.0f, -2.0f, 0.0f));
        }
        ourShader.setMat4("model",trans * transc4);
        ourShader.setInt("isZapper",0);
        ourShader.setInt("isFlying",0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(60 * sizeof(unsigned int)));

        glm::vec4 new_coin5[4] =  {trans*transc5*coin5[0], trans*transc5*coin5[1], trans*transc5*coin5[2], trans*transc5*coin5[3]};
        if(detectCoinCollision(new_ch,new_coin5)){
            score++;
            transc5 = glm::mat4(1.0f);
            yc5 = -2.0f;
            transc5 = glm::translate(transc5, glm::vec3(0.0f, -2.0f, 0.0f));
        }
        ourShader.setMat4("model",trans * transc5);
        ourShader.setInt("isZapper",0);
        ourShader.setInt("isFlying",0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(66 * sizeof(unsigned int)));

        glm::vec4 new_coin6[4] =  {trans*transc6*coin6[0], trans*transc6*coin6[1], trans*transc6*coin6[2], trans*transc6*coin6[3]};
        if(detectCoinCollision(new_ch,new_coin6)){
            score++;
            transc6 = glm::mat4(1.0f);
            yc6 = -2.0f;
            transc6 = glm::translate(transc6, glm::vec3(0.0f, -2.0f, 0.0f));
        }
        ourShader.setMat4("model",trans * transc6);
        ourShader.setInt("isZapper",0);
        ourShader.setInt("isFlying",0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(72 * sizeof(unsigned int)));
        // ---------------------------------------

        // Render Information
        // --------------------------------------

        std::string val = "Level: " + std::to_string(level);
        // std::cout << val;
        RenderText(shader, val, 25.0f, 950.0f, 0.75, glm::vec3(1.0f, 1.0f, 1.0f));

        val = "Score: " + std::to_string(score);
        RenderText(shader, val, 25.0f, 900.0f, 0.75, glm::vec3(1.0f, 1.0f, 1.0f));

        val = "Progress: " + std::to_string((int)progress*100/levelLength) + "%";
        RenderText(shader, val, 25.0f, 850.0f, 0.75, glm::vec3(1.0f, 1.0f, 1.0f));

        // --------------------------------------

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        progress++;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    // std::cout << "Score: " << score << std::endl;
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        y += v + (up_a/2);
        v += up_a;
        if(y>=max_y){
            y=max_y;
            v=0;
        }
        if(y<=0){
            y=0;
            v=0;
        }
        // std::cout << y << std::endl;
        chartrans = glm::mat4(1.0f);
        chartrans = glm::translate(chartrans,y*glm::vec3(0.0f, 1.0f, 0.0f));
        flagFlying = 1;
    }
    else{
        y += v - (down_a/2);
        v-=down_a;
        if(y<=0){
            y=0;
            v=0;
        }
        if(y>=max_y){
            y=max_y;
            v=0;
        }
        // std::cout << y << std::endl;
        // std::cout << v << std::endl;
        chartrans = glm::mat4(1.0f);
        chartrans = glm::translate(chartrans,y*glm::vec3(0.0f, 1.0f, 0.0f));
        flagFlying = 0;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state
    glEnable(GL_CULL_FACE); // enable the blendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}