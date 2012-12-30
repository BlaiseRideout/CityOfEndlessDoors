#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>
#include <string>
#include <fstream>
#include <streambuf>

extern const GLubyte *gluErrorString(GLenum error);

using namespace std;

struct Vec3 {
  public:
    float x, y, z;

  float length() {
    return sqrt(x * x + y * y + z * z);
  }

  void normalize() {
    float length = this->length();
    x /= length;
    y /= length;
    z /= length;
  }

  Vec3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
};

struct Vec2 {
  public:
    float x, y;

  void normalize() {
    if(x * x + y * y > 0) {
      float length = sqrt(x * x + y * y);
      x /= length;
      y /= length;
    }
  }

  void add(Vec2 *vec) {
    x += vec->x;
    y += vec->y;
  }

  void subtract(Vec2 *vec) {
    x -= vec->x;
    y -= vec->y;
  }

  void multiply(float c) {
    x *= c;
    y *= c;
  }

  void zero() {
    x = y = 0;
  }

  void nonzero() {
    if(x < 0.01f && x > -0.01f && y < 0.01f && y > -0.01f) {
      x = 0.1f;
      y = 0.1f; 
    }
  }

  Vec2(float x, float y) {
    this->x = x;
    this->y = y;
  }

  Vec2() {
    this->x = 0;
    this->y = 0;
  }
};

static void     draw_stuff();
static void     draw_quad(float shade, Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4);
static void     update();
static void     handle_input();
static void     clear_screen();
static void     game();
static void     cleanup();
static void     init_shaders();
static void     glPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
static void     init();
static string*  filetobuf(const char *file);

struct Building {
  Vec2 pos;
  int stories;
  int facing;
  float shade;
  float windowshade;
  bool open;

  void draw() {
    glPushMatrix();
    glTranslatef(pos.x, 0.0, pos.y);
    glRotatef(facing * 90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-4.0f, 0, -4.0f);
    const float story_scale = 2.0f;
    const float width = 8.0f;
    for(float i = 0; i < width; i+=width / 9) {
      draw_quad(shade, Vec3(i, story_scale * 1.5, 0), Vec3(i, stories * story_scale, 0), Vec3(i + width / 9, stories * story_scale, 0), Vec3(i + width / 9, story_scale * 1.5, 0));
      i += width / 9;
      if(i < width) {
        for(float j = story_scale * 1.5; j + story_scale * 3 / 4 < stories * story_scale; j+=story_scale) {
          draw_quad(shade, Vec3(i, j + story_scale * 3 / 4, 0), Vec3(i, j + story_scale * 5 / 4, 0), Vec3(i + width / 9, j + story_scale * 5 / 4, 0), Vec3(i + width / 9, j + story_scale * 3 / 4, 0));

          draw_quad(shade, Vec3(i, j + story_scale / 4, 0.1f), Vec3(i, j + story_scale / 4, 0), Vec3(i, j + story_scale * 3 / 4, 0), Vec3(i, j + story_scale * 3 / 4, 0.1f));
          draw_quad(shade, Vec3(i + width / 9, j + story_scale / 4, 0), Vec3(i + width / 9, j + story_scale / 4, 0.1f), Vec3(i + width / 9, j + story_scale * 3 / 4, 0.1f), Vec3(i + width / 9, j + story_scale * 3 / 4, 0));

          draw_quad(shade, Vec3(i, j + story_scale * 3 / 4, 0.1f), Vec3(i, j + story_scale * 3 / 4, 0), Vec3(i + width / 9, j + story_scale * 3 / 4, 0), Vec3(i + width / 9, j + story_scale * 3 / 4, 0.1f));

          draw_quad(windowshade, Vec3(i, j + story_scale / 4, 0.1f), Vec3(i, j + story_scale * 3 / 4, 0.1f), Vec3(i + width / 9, j + story_scale * 3 / 4, 0.1f), Vec3(i + width / 9, j + story_scale / 4, 0.1f));
        }
        draw_quad(shade, Vec3(i, stories * story_scale - story_scale / 4, 0), Vec3(i, stories * story_scale, 0), Vec3(i + width / 9, stories * story_scale, 0), Vec3(i + width / 9, stories * story_scale - story_scale / 4, 0));
      }
    }
    draw_quad(shade, Vec3(0, 0, 0), Vec3(0, story_scale * 1.75, 0), Vec3(width * 4 / 9, story_scale * 1.75, 0), Vec3(width * 4 / 9, 0, 0));
    draw_quad(shade, Vec3(width * 5 / 9, 0, 0), Vec3(width * 5 / 9, story_scale * 1.75, 0), Vec3(width, story_scale * 1.75, 0), Vec3(width, 0, 0));
    if(open) {

    }
    else {
      draw_quad(shade, Vec3(width * 4 / 9, 0, 0.1f), Vec3(width * 4 / 9, story_scale * 1.5, 0.1f), Vec3(width * 5 / 9, story_scale * 1.5, 0.1f), Vec3(width * 5 / 9, 0, 0.1f));
      draw_quad(shade, Vec3(width * 4 / 9, 0, 0.1f), Vec3(width * 4 / 9, 0, 0), Vec3(width * 4 / 9, story_scale * 1.5, 0), Vec3(width * 4 / 9, story_scale * 1.5, 0.1f));
      draw_quad(shade, Vec3(width * 5 / 9, 0, 0), Vec3(width * 5 / 9, 0, 0.1f), Vec3(width * 5 / 9, story_scale * 1.5, 0.1f), Vec3(width * 5 / 9, story_scale * 1.5, 0));
      draw_quad(shade, Vec3(width * 4 / 9, story_scale * 1.5, 0.1f), Vec3(width * 4 / 9, story_scale * 1.5, 0), Vec3(width * 5 / 9, story_scale * 1.5, 0), Vec3(width * 5 / 9, story_scale * 1.5, 0.1f));
    }
    draw_quad(shade, Vec3(0, 0, 0), Vec3(0, 0, width), Vec3(0, stories * story_scale, width), Vec3(0, stories * story_scale, 0));
    draw_quad(shade, Vec3(width, 0, 0), Vec3(width, stories * story_scale, 0), Vec3(width, stories * story_scale, width), Vec3(width, 0, width));
    draw_quad(shade, Vec3(width, stories * story_scale, width), Vec3(0, stories * story_scale, width), Vec3(0, 0, width), Vec3(width, 0, width));
    glPopMatrix();
  }

  Building(float shade, float windowshade, Vec2 pos, int stories, int facing) {
    this->shade = shade;
    this->windowshade = windowshade;
    this->pos = pos;
    this->stories = stories;
    this->facing = facing % 4;
  }

  Building() {
    this->shade = 0.0f;
    this->windowshade = 0.0f;
    this->pos.x = 0.0f;
    this->pos.y = 0.0f;
    this->stories = 0;
    this->facing = 0;
  }
};

static GLuint           shaderprogram;
static unsigned int     SCREEN_BPP = 32;
static unsigned int     SCREEN_WIDTH = 1366;
static unsigned int     SCREEN_HEIGHT = 768;
static SDL_Surface*     screen = NULL;
static SDL_Event        event;
static int              prevkeys[323] = { 0 };
static int              keys[323] = { 0 };
static Vec2             mrel;
static Vec2             playerpos(5.0f, -6.0f);
static Vec2             playervel(0.0f, 0.0f);
static Vec2             look(-1.5f, 0.0f);
static bool             running = true;
static bool             fullscreen = false;
static bool             sound = false;
static Building         buildings[10][10];

static void draw_stuff() {
  draw_quad(.3f, Vec3(playerpos.x - 200, 0, playerpos.y - 200), Vec3(playerpos.x - 200, 1, playerpos.y + 200), Vec3(playerpos.x + 200, 0, playerpos.y + 200), Vec3(playerpos.x + 200, 0, playerpos.y - 200));
  for(int i = 0; i < 10; ++i)
    for(int j = 0; j < 10; ++j)
      buildings[i][j].draw(); 
}
/*
static void draw_building(float shade, float windowshade, int facing, int stories, float x, float z) {
  glPushMatrix();
  glTranslatef(x, 0, z);
  glRotatef(facing * 90.0f, 0.0f, 1.0f, 0.0f);
  glTranslatef(-4.5f, 0, -4.5f);
  const float story_scale = 2.0f;
  const float width = 9.0f;
  for(float i = 0; i < width; i+=width / 9) {
    draw_quad(shade, Vec3(i, story_scale * 1.5, 0), Vec3(i, stories * story_scale, 0), Vec3(i + width / 9, stories * story_scale, 0), Vec3(i + width / 9, story_scale * 1.5, 0));
    i += width / 9;
    if(i < width) {
      for(float j = story_scale * 1.5; j + story_scale * 3 / 4 < stories * story_scale; j+=story_scale) {
        draw_quad(shade, Vec3(i, j + story_scale * 3 / 4, 0), Vec3(i, j + story_scale * 5 / 4, 0), Vec3(i + width / 9, j + story_scale * 5 / 4, 0), Vec3(i + width / 9, j + story_scale * 3 / 4, 0));

        draw_quad(shade, Vec3(i, j + story_scale / 4, 0.1f), Vec3(i, j + story_scale / 4, 0), Vec3(i, j + story_scale * 3 / 4, 0), Vec3(i, j + story_scale * 3 / 4, 0.1f));
        draw_quad(shade, Vec3(i + width / 9, j + story_scale / 4, 0), Vec3(i + width / 9, j + story_scale / 4, 0.1f), Vec3(i + width / 9, j + story_scale * 3 / 4, 0.1f), Vec3(i + width / 9, j + story_scale * 3 / 4, 0));

        draw_quad(shade, Vec3(i, j + story_scale * 3 / 4, 0.1f), Vec3(i, j + story_scale * 3 / 4, 0), Vec3(i + width / 9, j + story_scale * 3 / 4, 0), Vec3(i + width / 9, j + story_scale * 3 / 4, 0.1f));

        draw_quad(windowshade, Vec3(i, j + story_scale / 4, 0.1f), Vec3(i, j + story_scale * 3 / 4, 0.1f), Vec3(i + width / 9, j + story_scale * 3 / 4, 0.1f), Vec3(i + width / 9, j + story_scale / 4, 0.1f));
      }
      draw_quad(shade, Vec3(i, stories * story_scale - story_scale / 4, 0), Vec3(i, stories * story_scale, 0), Vec3(i + width / 9, stories * story_scale, 0), Vec3(i + width / 9, stories * story_scale - story_scale / 4, 0));
    }
  }
  draw_quad(shade, Vec3(0, 0, 0), Vec3(0, story_scale * 1.75, 0), Vec3(width * 4 / 9, story_scale * 1.75, 0), Vec3(width * 4 / 9, 0, 0));
  draw_quad(shade, Vec3(width * 5 / 9, 0, 0), Vec3(width * 5 / 9, story_scale * 1.75, 0), Vec3(width, story_scale * 1.75, 0), Vec3(width, 0, 0));
  draw_quad(shade, Vec3(width * 4 / 9, 0, 0.1f), Vec3(width * 4 / 9, story_scale * 1.5, 0.1f), Vec3(width * 5 / 9, story_scale * 1.5, 0.1f), Vec3(width * 5 / 9, 0, 0.1f));
  draw_quad(shade, Vec3(width * 4 / 9, 0, 0.1f), Vec3(width * 4 / 9, 0, 0), Vec3(width * 4 / 9, story_scale * 1.5, 0), Vec3(width * 4 / 9, story_scale * 1.5, 0.1f));
  draw_quad(shade, Vec3(width * 5 / 9, 0, 0), Vec3(width * 5 / 9, 0, 0.1f), Vec3(width * 5 / 9, story_scale * 1.5, 0.1f), Vec3(width * 5 / 9, story_scale * 1.5, 0));
  draw_quad(shade, Vec3(width * 4 / 9, story_scale * 1.5, 0.1f), Vec3(width * 4 / 9, story_scale * 1.5, 0), Vec3(width * 5 / 9, story_scale * 1.5, 0), Vec3(width * 5 / 9, story_scale * 1.5, 0.1f));

  draw_quad(shade, Vec3(0, 0, 0), Vec3(0, 0, width), Vec3(0, stories * story_scale, width), Vec3(0, stories * story_scale, 0));
  draw_quad(shade, Vec3(width, 0, 0), Vec3(width, stories * story_scale, 0), Vec3(width, stories * story_scale, width), Vec3(width, 0, width));
  draw_quad(shade, Vec3(width, stories * story_scale, width), Vec3(0, stories * story_scale, width), Vec3(0, 0, width), Vec3(width, 0, width));
  glPopMatrix();
}
*/
static void draw_quad(float shade, Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4) {
  glBegin(GL_QUADS);

  GLint location = glGetAttribLocation(shaderprogram, "in_Color");
  glVertexAttrib1f(location, shade);
  glBindAttribLocation(shaderprogram, location, "in_Color");
  location = glGetAttribLocation(shaderprogram, "v1");
  glVertexAttrib3f(location, p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
  glBindAttribLocation(shaderprogram, location, "v1");
  location = glGetAttribLocation(shaderprogram, "v2");
  glVertexAttrib3f(location, p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
  glBindAttribLocation(shaderprogram, location, "v2");

  glVertex3f(p1.x, p1.y, p1.z);
  glVertex3f(p2.x, p2.y, p2.z);
  glVertex3f(p3.x, p3.y, p3.z);
  glVertex3f(p4.x, p4.y, p4.z);

  glEnd();
}

static void update() {
  const float pi = 3.14159265358979323846264338327950288;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  look.x += mrel.x / 300.0f;
  look.y += mrel.y / 400.0f;
  if(look.y > pi / 2) {
    look.y = pi / 2;
  }
  if(look.y < -pi / 2) {
    look.y = -pi / 2;
  }
  if(keys[SDLK_PERIOD]) {
    for(int i = 0; i < 10; ++i)
      for(int j = 0; j < 10; ++j) {
        Vec2 doorpos;
        if(buildings[i][j].facing == 0) {
          doorpos.y = i * 15 - 75 - 4.5;
          doorpos.x = j * 15 - 75;
        }
        else if(buildings[i][j].facing == 2) {
          doorpos.y = i * 15 - 75 + 4.5;
          doorpos.x = j * 15 - 75;
        }
        else if(buildings[i][j].facing == 1) {
          doorpos.y = i * 15 - 75;
          doorpos.x = j * 15 - 75 - 4.5;
        }
        else if(buildings[i][j].facing == 3) {
          doorpos.y = i * 15 - 75;
          doorpos.x = j * 15 - 75 + 4.5;
        }
        if(abs(playerpos.x - doorpos.x) < 1.0f && abs(playerpos.y - doorpos.y) < 1.0f)
          buildings[i][j].open = true;
      }
  }
  Vec2 acc;
  if(keys[SDLK_COMMA]){
    acc.x += cos(look.x);
    acc.y -= sin(look.x);
  }
  else if(keys[SDLK_o]) {
    acc.x -= cos(look.x);
    acc.y += sin(look.x);
  }

  if(keys[SDLK_a]) {
    acc.x -= sin(look.x);
    acc.y -= cos(look.x);
  }
  else if(keys[SDLK_e]) {
    acc.x += sin(look.x);
    acc.y += cos(look.x);
  }
  acc.normalize();
  if(keys[SDLK_LSHIFT])
    acc.multiply(0.07f);
  else
    acc.multiply(0.03f);
  playervel.multiply(0.85f);
  playervel.add(&acc);
  playerpos.add(&playervel);
  for(int i = 0; i < 10; ++i)
    for(int j = 0; j < 10; ++j)
      if(playerpos.x + 4.5 < i * 15 - 75 + 9 && playerpos.x + 4.5 > i * 15 - 75 && playerpos.y + 4.5 > j * 15 - 75 && playerpos.y + 4.5 < j * 15 - 75 + 9) {
        playervel.nonzero();
        playervel.multiply(-0.1f);
        while(playerpos.x + 4.5 < i * 15 - 75 + 9 && playerpos.x + 4.5 > i * 15 - 75 && playerpos.y + 4.5 > j * 15 - 75 && playerpos.y + 4.5 < j * 15 - 75 + 9) {
          playerpos.add(&playervel);
        }
      }
/*  if((int)abs(playerpos.x + 4.5) % 15 < 9 && (int)abs(playerpos.y + 4.5) % 15 < 9) {
    playervel.multiply(-0.1f);
    while((int)abs(playerpos.x + 4.5) % 15 < 9 && (int)abs(playerpos.y + 4.5) % 15 < 9) {
      playerpos.add(&playervel);
    }
  } */

  glPerspective(45.0f, (GLfloat) SCREEN_WIDTH / (GLfloat) SCREEN_HEIGHT, 0.1f, 100.0f);
  gluLookAt(playerpos.x, 2.0f, playerpos.y,
            playerpos.x + cos(look.x), 2.0f + sin(look.y) * 2, playerpos.y - sin(look.x),
            0.0f, 1.0f, 0.0f);
  glMatrixMode(GL_MODELVIEW);
}

static void handle_input() {
  mrel.zero();
  for(unsigned int i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
    prevkeys[i] = keys[i];
  }
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_KEYDOWN:
        keys[event.key.keysym.sym] = 1;
        break;
      case SDL_KEYUP:
        keys[event.key.keysym.sym] = 0;
        break;
      case SDL_MOUSEMOTION:
        mrel.x = event.motion.xrel;
        mrel.y = event.motion.yrel;
        SDL_WarpMouse(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        break;
      case SDL_QUIT:
       running = false;
       break;
    }
  }
  if(keys[SDLK_q])
    running = false;
}

static void clear_screen() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void game() {
  while(running) {
    clear_screen();
    draw_stuff();
    SDL_GL_SwapBuffers();
    handle_input();
    update();
  }
}

static void cleanup() {
  SDL_Quit();
}

static void init_shaders() {
  shaderprogram = glCreateProgram();

  const GLchar **fragmentsource;
  const GLchar **vertexsource;
  GLint vertexlengths[1], fragmentlengths[1];

  GLuint vertexshader, fragmentshader;

  vertexshader = glCreateShader(GL_VERTEX_SHADER);
  fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);

  vertexsource = (const GLchar**)malloc(sizeof(GLchar**));
  fragmentsource = (const GLchar**)malloc(sizeof(GLchar**));

  string *source = filetobuf("src/screen.vert");
  vertexsource[0] = (char *)source->data();
  vertexlengths[0] = source->size();
  free(source);

  source = filetobuf("src/screen.frag");
  fragmentsource[0] = (char *)source->data();
  fragmentlengths[0] = source->size();
  free(source);

  glShaderSource(vertexshader, 1, vertexsource, vertexlengths); 
  glShaderSource(fragmentshader, 1, fragmentsource, fragmentlengths);

  glCompileShader(vertexshader);
  GLsizei length;
  GLchar infoLog[256];
  glGetShaderInfoLog(vertexshader, 255, &length, infoLog);
  if(length != 0) {
    printf("%s\n", infoLog);
  }

  glCompileShader(fragmentshader);
  glGetShaderInfoLog(fragmentshader, 255, &length, infoLog);
  if(length != 0) {
    printf("%s\n", infoLog);
  }

  glAttachShader(shaderprogram, vertexshader);
  glAttachShader(shaderprogram, fragmentshader);

  glLinkProgram(shaderprogram);
  glValidateProgram(shaderprogram);

  free(fragmentsource);
  free(vertexsource);
  glUseProgram(shaderprogram);
}

static void glPerspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar) {
  const GLdouble pi = 3.14159265358979323846264338327950288;
  GLfloat fW, fH;
  fH = tan( fovY / 360 * pi ) * zNear;
  fW = fH * aspect;
  glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

static void init() {
  srand(time(NULL));

  for(int i = 0; i < 10; ++i)
    for(int j = 0; j < 10; ++j) {
      int stories = 0;
      if(j != 0)
        stories = buildings[i][j - 1].stories;
      else if(i != 0)
        stories = buildings[i - 1][j].stories;
      else
        stories = 11;

      stories += rand() % 16 - 8;

      if(stories < 3)
        stories = 3;
      else if(stories > 25)
        stories = 25;

      buildings[i][j] = Building(rand() % 256 / 255.0f, rand() % 256 / 255.0f, Vec2(i * 15 - 75, j * 15 - 75), stories, rand() % 4);
    }

  SDL_Init(SDL_INIT_EVERYTHING);

  if(fullscreen)
    screen = SDL_SetVideoMode(0, 0, SCREEN_BPP, SDL_OPENGL | SDL_FULLSCREEN);
  else
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL);

  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_ShowCursor(SDL_DISABLE);

  glClearColor(0.01f, 0.01f, 0.01f, 0.0f);
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

//  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
//  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glPerspective(45.0f, (GLfloat) SCREEN_WIDTH / (GLfloat) SCREEN_HEIGHT, 0.1f, 200.0f);
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
//  glEnable(GL_MULTISAMPLE);
  glDisable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);  

  init_shaders();

  if(sound) {
    Mix_OpenAudio(44100, AUDIO_S16, 2, 256);
//    load_music();
//    Mix_PlayMusic(music, 0);
  }

  while(SDL_PollEvent(&event));
}

static string *filetobuf(const char *file) {
  ifstream t(file);
  string *ret = new string((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
  return ret;
}

int main(int argc, char **argv) {
  init();
  game();
  cleanup();
  return 0;
}
