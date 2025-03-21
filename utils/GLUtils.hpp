#ifndef __GLUTILS_HPP__
#define __GLUTILS_HPP__

#include "glad.h"
#include <GLFW/glfw3.h>

namespace GLUtils
{
    int checkForOpenGLError(const char *, int);
    
    void dumpGLInfo(bool dumpExtensions = false);
    
	void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar * msg, const void * param );
}

#endif // __GLUTILS_HPP__
