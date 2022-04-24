#pragma once
#include "../object/object.h"

// Singleton pattern, I would avoid but the buffer size needs to chnage with window resize callback
class ScreenManager {
private:

  // Contains all frameBuffers and their textures
  static ScreenManager* m_instance;
  Object m_screenQuad;
  unsigned int m_sceneFBO;
  unsigned int m_sceneTextures[2];
  float m_prevExposure;
  ScreenManager();

public:
  static ScreenManager* getInstance();
  void generateFrameBuffers();
  void bindDefaultBuffer();
  void bindSceneBuffer();
  void clearScreenBuffer();
  void renderToScreen();
};
