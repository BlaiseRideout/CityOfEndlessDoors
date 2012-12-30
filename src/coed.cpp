#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
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

  void normalize() {
    float length = sqrt(x * x + y * y + z * z);
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
    float length = sqrt(x * x + y * y);
    x /= length;
    y /= length;
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
static void     draw_quad(float shade, Vec3 *p1, Vec3 *p2, Vec3 *p3, Vec3 *p4);
static void     update();
static void     handle_input();
static void     clear_screen();
static void     game();
static void     cleanup();
static void     init_shaders();
static void     glPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
static void     init();
static string*  filetobuf(const char *file);

static GLuint           shaderprogram;
static unsigned int     SCREEN_BPP = 32;
static unsigned int     SCREEN_WIDTH = 1366;
static unsigned int     SCREEN_HEIGHT = 768;
static SDL_Surface*     screen = NULL;
static SDL_Event        event;
static int              prevkeys[323] = { 0 };
static int              keys[323] = { 0 };
static Vec2             mrel;
static Vec2             playerpos(-1.0f, -1.0f);
static Vec2             playervel;
static Vec2             look;
static bool             running = true;
static bool             fullscreen = false;
static bool             sound = false;

static void draw_stuff() {
  draw_quad(.5f, new Vec3(0, 0, 0), new Vec3(1, 0, 0), new Vec3(1, 1, 0), new Vec3(0, 1, 0));
}

static void draw_quad(float shade, Vec3 *p1, Vec3 *p2, Vec3 *p3, Vec3 *p4) {
  glUseProgram(shaderprogram);
  glBegin(GL_QUADS);

  glColor3f(shade, shade, shade);

  glVertex3f(p1->x, p1->y, p1->z);
  glVertex3f(p2->x, p2->y, p2->z);
  glVertex3f(p3->x, p3->y, p3->z);
  glVertex3f(p4->x, p4->y, p4->z);

  glEnd();
}

static void update() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if(keys[SDLK_COMMA])
    playervel.add(&look);
  else if(keys[SDLK_o])
    playervel.subtract(&look);

  if(keys[SDLK_a]) {
    Vec2 tmp;
    tmp.x = -look.y;
    tmp.y = look.x;
    playervel.add(&tmp);
  }
  else if(keys[SDLK_e]) {
    Vec2 tmp;
    tmp.x = look.y;
    tmp.y = -look.x;
    playervel.add(&tmp);
  }
  playervel.normalize();
  playerpos.add(&playervel);
  Vec2 ablook;
  ablook.x = playerpos.x - look.x;
  ablook.y = playerpos.y - look.y;
  look.x += mrel.x / 300.0f;
  look.y += mrel.y / 300.0f;
  glPerspective(45.0f, (GLfloat) SCREEN_WIDTH / (GLfloat) SCREEN_HEIGHT, 0.1f, 20.0f);
  gluLookAt(playerpos.x, 0.0f, playerpos.y,
            look.x, look.y, 0.0f,
            0.0f, 1.0f, 0.0f);
  glMatrixMode(GL_MODELVIEW);
}

static void handle_input() {
  look.zero();
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
        mrel.x = event.motion.yrel;
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
//  GLfloat quad[4][3] = { { p1->x,  p1->y,  p1->z }, { p2->x,  p2->y,  p2->z }, { p3->x,  p3->y,  p3->z }, { p4->x,  p4->y, p4->z  } };

//  GLfloat colors[4][3] = { {  color->r,  color->g,  color->b  }, {  color->r,  color->g,  color->b  }, {  color->r,  color->g,  color->b  }, {  color->r,  color->g,  color->b  } };

  shaderprogram = glCreateProgram();

  /* These pointers will receive the contents of our shader source code files */
//  const GLchar *vertexsource[] = { "varying vec3 vertColor; void main(){ gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex; vertColor = vec4(0.6, 0.3, 0.4, 1.0); } " };
//  const GLchar *fragmentsource[] =  { "varying vec4 vertColor; void main(){ gl_FragColor = vertColor; }" };
  const GLchar **fragmentsource;
  const GLchar **vertexsource;
  GLint vertexlengths[1], fragmentlengths[1];

  /* These are handles used to reference the shaders */
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

  glShaderSource(vertexshader, 1, vertexsource, vertexlengths); 
  glShaderSource(fragmentshader, 1, fragmentsource, fragmentlengths);

  glCompileShader(vertexshader);
  glCompileShader(fragmentshader);

  glAttachShader(shaderprogram, vertexshader);
  glAttachShader(shaderprogram, fragmentshader);

  glLinkProgram(shaderprogram);
  glValidateProgram(shaderprogram);

  free(fragmentsource);
  free(vertexsource);
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

  glClearColor((100.0f / 255.0f), (149.0f / 255.0f), (237.0f / 255.0f), 0.0f);
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
//  glFrustum(-1.0, 1.0, 1.0, -1.0, -1.0, 1.0); // left, right, bottom, top, near, far
  glPerspective(45.0f, (GLfloat) SCREEN_WIDTH / (GLfloat) SCREEN_HEIGHT, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_TEXTURE_2D);
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
