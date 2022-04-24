#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include FT_FREETYPE_H

#define GLFW_INCLUDE_NONE

#include "iostream"

#include "gui.h"
#include "../../config.h"
#include "../shader/shaderManager.h"


Gui::Gui()
{
    //GUI

    ShaderManager* shaderManager = ShaderManager::getInstance();
    shaderManager->bindShader(gui_vertShaderPath, gui_fragShaderPath);

    float vertices[] = {
        -0.68f,  1.00f, 0.0f,
        -0.68f,  0.70f, 0.0f,
        -1.00f,  0.70f, 0.0f,
        -1.00f,  1.00f, 0.0f 
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3   
    };


    glGenVertexArrays(1, &guiVAO);
    glGenBuffers(1, &guiVBO);
    glGenBuffers(1, &guiEBO);
    glBindVertexArray(guiVAO);

    glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, guiEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float leftBound = -0.99;
    float rightBound = -0.69;
    float topBound = 0.99;
    float bottomBound = 0.71;
    glUniform1fv(glGetUniformLocation(shaderManager->getBoundShader(), "leftBound"), 1, &leftBound);
    glUniform1fv(glGetUniformLocation(shaderManager->getBoundShader(), "rightBound"), 1, &rightBound);
    glUniform1fv(glGetUniformLocation(shaderManager->getBoundShader(), "topBound"), 1, &topBound);
    glUniform1fv(glGetUniformLocation(shaderManager->getBoundShader(), "bottomBound"), 1, &bottomBound);



    //TEXT

    shaderManager->bindShader(text_vertShaderPath, text_fragShaderPath);
    textShaderProgram = shaderManager->getBoundShader();

    FT_Library ft;
    FT_Init_FreeType(&ft);

    // find path to font
    std::string font_name = "C:\\Users\\erics\\source\\repos\\SolarSystemSimulator\\src\\fonts\\arial.ttf";
    // load font as face
    FT_Face face;
    FT_New_Face(ft, font_name.c_str(), 0, &face);
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

    Config* config = Config::getInstance();
    unsigned int SCR_WIDTH = config->getScreenWidth();
    unsigned int SCR_HEIGHT = config->getScreenHeight();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    //might be a problem
    glUniformMatrix4fv(glGetUniformLocation(shaderManager->getBoundShader(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));




}

//void Gui::render(std::unordered_set<unsigned int>* pressedKeys)
//{
//    if (pressedKeys->count(GLFW_KEY_G))
//    {
//        if (toggleGUI)
//            toggleGUI = false;
//        else
//            toggleGUI = true;
//    }
//    if (toggleGUI)
//    {
//        ShaderManager* shaderManager = ShaderManager::getInstance();
//        shaderManager->bindShader(gui_vertShaderPath, gui_fragShaderPath);
//        glBindVertexArray(guiVAO);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//        renderText();
//    }
//}

void Gui::render(double frameRate)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::string frameRateString = std::to_string(frameRate);
    if (frameRate < 100)
    {
        frameRateString = frameRateString.substr(0, 2);
    }
    else
    {
        frameRateString = frameRateString.substr(0, 3);
    }
    renderText(textShaderProgram, "System: Sol", 10.0f, 580.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));
    renderText(textShaderProgram, "Stars: 1", 10.0f, 560.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));
    renderText(textShaderProgram, "Planets: 9", 10.0f, 540.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));
    renderText(textShaderProgram, "Moons: 171", 10.0f, 520.0f, 0.4f, glm::vec3(0.5, 0.8f, 0.2f));
    renderText(textShaderProgram, "FPS " + frameRateString, 690.0f, 570.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
    ShaderManager* shaderManager = ShaderManager::getInstance();
    shaderManager->bindShader(gui_vertShaderPath, gui_fragShaderPath);
    glBindVertexArray(guiVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
}

unsigned int Gui::getShader()
{
    return textShaderProgram;
}


void Gui::renderText(unsigned int shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
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
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
