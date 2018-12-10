    }
    ret = (len == 0);
  }
  return ret;
}

#if defined(_WIN32)

#if defined(GLEW_MX)
GLboolean GLEWAPIENTRY wglewContextIsSupported (const WGLEWContext* ctx, const char* name)
#else
GLboolean GLEWAPIENTRY wglewIsSupported (const char* name)
#endif
{
  const GLubyte* pos = (const GLubyte*)name;
  GLuint len = _glewStrLen(pos);
  GLboolean ret = GL_TRUE;
  while (ret && len > 0)
  {
    if (_glewStrSame1(&pos, &len, (const GLubyte*)"WGL_", 4))
    {
// CG_REVISION 2c745227b702779fc1999dee8c34f43b977f0871
