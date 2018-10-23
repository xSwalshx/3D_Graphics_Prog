#include "VertexBuffer.h"
#include "VertexArray.h"
#include "ShaderProgram.h"

#include <stb_image/stb_image.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/ext.hpp>

#include <exception>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char *argv[])
{
  //-----------------------------------------------------------------------------------------------//

  ///SDL SET UP
  if (SDL_Init(SDL_INIT_VIDEO) < 0) { throw std::exception(); }

  SDL_Window *window = SDL_CreateWindow("Triangle",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    WINDOW_WIDTH, WINDOW_HEIGHT,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

  if (!SDL_GL_CreateContext(window)) { throw std::exception(); }

  if (glewInit() != GLEW_OK) { throw std::exception(); }

  //-----------------------------------------------------------------------------------------------//
  
  //SET UP THE IMAGE LOADER
  int w = 0;
  int h = 0;
  int channels = 0;

  //IMAGE LOADED AS AN UNSIGNED CHAR* (NAME OF IMAGE, WIDTH, HEIGHT, CHANNELS, NUMBER OF CHANNLES (R/G/B/A)
  unsigned char *data = stbi_load("../images/Abstergo_Logo.png", &w, &h, &channels, 4);
  
  if (!data) { throw std::exception(); }

  //-----------------------------------------------------------------------------------------------//

  //CREATE AND BIND TEXTURE
  GLuint textureId = 0;
  glGenTextures(1, &textureId);

  if (!textureId) { throw std::exception(); }

  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  free(data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  //-----------------------------------------------------------------------------------------------//

  //POSITIONS
  VertexBuffer *positions = new VertexBuffer();
  positions->add(glm::vec3(0.0f, 0.5f, 0.0f));
  positions->add(glm::vec3(-0.5f, -0.5f, 0.0f));
  positions->add(glm::vec3(0.5f, -0.5f, 0.0f));

  //-----------------------------------------------------------------------------------------------//

  //COLOURS
  //VertexBuffer *colors = new VertexBuffer();
  //colors->add(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
  //colors->add(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
  //colors->add(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

  //-----------------------------------------------------------------------------------------------//
  
  //TEXTURE
  VertexBuffer *texCoords = new VertexBuffer();
  texCoords->add(glm::vec2(0.5f, 0.0f));
  texCoords->add(glm::vec2(0.0f, 1.0f));
  texCoords->add(glm::vec2(1.0f, 1.0f));

  //-----------------------------------------------------------------------------------------------//

  //CREATEING A NEW VERTEX ARRAY CALLED SHAPE
  VertexArray *shape = new VertexArray();
  shape->setBuffer("in_Position", positions);
  //shape->setBuffer("in_Color", positions);
  shape->setBuffer("in_TexCoord", texCoords);

  ShaderProgram *shader = new ShaderProgram("../shaders/simple.vert", "../shaders/simple.frag");

  //-----------------------------------------------------------------------------------------------//

  //DEFINED FLOAT TO BE USED FOR THE ANGLE OF THE ROTATION
  float angle = 0;

  //-----------------------------------------------------------------------------------------------//
  
  ///WHILE LOOP FOR SDL
  bool quit = false;

  while (!quit)
  {
    SDL_Event event = { 0 };

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        quit = true;
      }
    }

    //-----------------------------------------------------------------------------------------------//
    
    //SET THE BACKGROUND TO WHITE
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //-----------------------------------------------------------------------------------------------//

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, textureId);

    //-----------------------------------------------------------------------------------------------//



    //-----------------------------------------------------------------------------------------------//

    ///DRAW THE PERSPECTIVE PROJECTION MATRIX
    shader->setUniform("in_Projection", glm::perspective(glm::radians(45.0f),
      (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.f));
    /*There is an issue with the uniform id coming back as -1*/
    
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(0, 0, -2.5f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
    shader->setUniform("in_Model", model);

    shader->setUniform("in_Texture", 1);

    shader->draw(shape);

    //INCREASE ANGLE BY VALUE
    angle += 0.1f;

    //-----------------------------------------------------------------------------------------------//

    ///DRAW WITH ORTHOGRAPHIC PROJECTION MATRIX
    shader->setUniform("in_Projection", glm::ortho(0.0f,
      (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, 0.0f, 1.0f));

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(100, WINDOW_HEIGHT - 100, 0));
    model = glm::scale(model, glm::vec3(100, 100, 1));

    shader->setUniform("in_Model", model);

    shader->draw(shape);

    //-----------------------------------------------------------------------------------------------//
    
    SDL_GL_SwapWindow(window);
  }

  //-----------------------------------------------------------------------------------------------//
  
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
