#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Object {
  private:
    std::string m_meshFilePath;
    std::string m_vertexShaderPath;
    std::string m_fragShaderPath;
    std::string m_imgTexFilePath;
    std::string m_name;

    glm::vec3 m_position;
    glm::quat m_rotation;
    float m_scale;

  public:
    Object();
    void setName(std::string name);
    std::string getName();
    glm::vec3 getPosition();
    void setPosition(float x, float y, float z);
    void setPosition(glm::vec3 position);
    void setRotation(float angle, glm::vec3 axis);
    glm::mat4 getRotationMat();
    float getScale();
    void setScale(float scale);
    void setMesh(std::string meshFilePath);
    void setShaders(std::string vertexShaderPath, std::string fragShaderPath);
    void setImageTexture(std::string textureFilePath);
    void bind();

};