#include <cstdio>
#include <cassert>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <IL/il.h>
#include <IL/ilu.h>

static void reshape(GLFWwindow* window, int width, int height);
static void keypressed(GLFWwindow* window, int key, int scancode, int action, int mods);
void shader_set_gl_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void my_image_draw(GLuint tex);
GLuint my_image_load(const char* path, const char* name);
static void render_image();
static void init_default_texture();

static int win_w = 1024;
static int win_h = 768;
static int running = 1;
static double now = 0;

static bool tick_once = true;
static int ticks = 0;

void glfw_init(GLFWwindow* window);
void glew_init();
static void tick(GLFWwindow* window, GLuint tex);


int main()
{
	if (!glfwInit()) { fprintf(stderr, "could not initialize glfw!\n"); }
	GLFWwindow* window = glfwCreateWindow(win_w, win_h, "O2 Beamer", NULL, NULL);
	assert(window);

	// our custom initializations that contain all everytime stuff
	glfw_init(window);
	glew_init();
	ilInit();
	iluInit();
	init_default_texture();

	now = glfwGetTime();
	GLuint tex = my_image_load("samples/image-1.jpg", "something");
		
	while (running) {        
			tick(window, tex);
			++ticks;
	}

	fprintf(stdout, "Copyright (c) 2019 O2 Labs <info@o2labs.com>\n");
	return 0;	
}

static void tick(GLFWwindow* window, GLuint tex) {
    now = glfwGetTime();
    
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(
        GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE_MINUS_DST_ALPHA, GL_ONE
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, win_w, win_h);
    glOrtho(0, win_w,
            win_h, 0,
            -1000, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
        
		glClearColor(0.05, 0.05, 0.05, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    // load image
    my_image_draw(tex);
    
    glfwSwapBuffers(window);
    glfwPollEvents();

    if (glfwWindowShouldClose(window))
        running = 0;
}


// CALLBACKS
// =======================================================================================
static void reshape(GLFWwindow* window, int width, int height) {
    win_w = width;
    win_h = height;
    fprintf(stderr, "resized to %dx%d\n", width, height);
}

static void keypressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS)
        return;

    switch (key) {
        case GLFW_KEY_ESCAPE:
            running = 0;
            break;
    }
}

// GLEW
// =======================================================================================
void glew_init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
			fprintf(stderr, "cannot initialize glew\n");
	if (!glewIsSupported("GL_VERSION_3_0"))
			fprintf(stderr, "need opengl 3.0 support\n");
	return;
}

// GLFW
// =======================================================================================
void glfw_init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, reshape);
	glfwSetKeyCallback(window, keypressed);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwGetFramebufferSize(window, &win_w, &win_h);
}

// GL
// =======================================================================================
static void init_default_texture() {
		GLuint default_tex; // white default texture
    glGenTextures(1, &default_tex);
    glBindTexture(GL_TEXTURE_2D, default_tex);
    unsigned char white_pixel[] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 1, 1, 0, 
        GL_RGBA, GL_UNSIGNED_BYTE, white_pixel);
}

void shader_set_gl_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    glColor4f(r, g, b, a);

    GLint prog, color;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    color = glGetUniformLocation(prog, "Color");
    if (color != -1)
        glUniform4f(color, r, g, b, a);
}

void my_image_draw(GLuint tex)
{
    GLfloat x1 = 0.f; 
    GLfloat y1 = 99.27f; 
    GLfloat x2 = 1024.f; 
    GLfloat y2 = 668.72f; 
    GLfloat alpha = 1.f;
    GLfloat sx1 = 0.f;
    GLfloat sy1 = 0.f;
    GLfloat sx2 = 1.f;
    GLfloat sy2 = 1.f;

    glBindTexture(GL_TEXTURE_2D, tex);
    shader_set_gl_color(1.0, 1.0, 1.0, alpha);

    glBegin(GL_QUADS); 

    glTexCoord2f(sx1, sy1); glVertex3f(x1, y1, 0);
    glTexCoord2f(sx2, sy1); glVertex3f(x2, y1, 0);
    glTexCoord2f(sx2, sy2); glVertex3f(x2, y2, 0);
    glTexCoord2f(sx1, sy2); glVertex3f(x1, y2, 0);
    glEnd();
		return;
}

GLuint my_image_load(const char* path, const char* name) 
{
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (!ilLoadImage(path)) {
        ilDeleteImages(1, &imageID);
        fprintf(stderr, "[ - ] Unable to load image at image.c:my_image_load()\n");
    }
 
    ILinfo ImageInfo;
    iluGetImageInfo(&ImageInfo);

    if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
        ilDeleteImages(1, &imageID);
        fprintf(stderr, "[ - ] Unable to convert image at image.c:my_image_load()\n");
    }

    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), width, height, 0,
                 ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
    glGenerateMipmap(GL_TEXTURE_2D);
    ilDeleteImages(1, &imageID);
    return tex;
}
