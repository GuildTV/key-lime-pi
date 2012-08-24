/*
 *      Copyright (C) 2012 GuildTV
 *      http://www.guildtv.co.uk
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "OverlayRenderer.h"

OverlayRenderer::OverlayRenderer(NetIO *net){
#ifdef RENDERTEST
    x_display = NULL;
#endif
    //save network handler
    netIO = net;

    //create freetype renderer instance
    ft = new Freetype(this);

    //create window
    esCreateWindow ("Overlay Renderer");

    //setup opengl
    Init();

    //define default timecode position
#ifdef LIMESLAVE
    timecodePosition = NONE;
#else
    timecodePosition = CENTER;
#endif
    //load the overlay text characters
    LoadOverlayText();

    //create empty vector
    renderElms = vector<TextureRender*>();
}

void OverlayRenderer::Create(std::string file){
    filename = file;

    renderElms.clear();

    TextTexture *t = new TextTexture(this);
    t->setText("Hi you ;P",overlayText, 100,300,1,1);
    renderElms.push_back(t);

    TextTexture *t2 = new TextTexture(this);
    t2->setText("It Works XD",overlayText, 100,100,1,1);

    ColourTexture *c = new ColourTexture(this, t2);
    c->SetColour1(0.1f, 0.6f, 1.0f, 0.0f, 1.0f, 0.8f);//purple
    renderElms.push_back(c);
}

bool OverlayRenderer::LoadOverlayText() {
    //load as 100px tall
    int height = 100;
    if(!ft->LoadFreetypeRange("resources/Overlay.ttf", height, 32, 127, overlayText)){
        FLog::Log(FLOG_ERROR, "OverlayRenderer::LoadOverlayText - failed to load overlay text characters");
        return false;
    }

    FLog::Log(FLOG_INFO, "OverlayRenderer::LoadOverlayText - loaded overlay text characters");
    return true;
}

void OverlayRenderer::RenderTexture(GLuint texture) {
    GLfloat vVertices[] = { -1.0f,  1.0f, 0.0f,  // Position 0
                                0.0f,  1.0f,        // TexCoord 0
                               -1.0f, -1.0f, 0.0f,  // Position 1
                                0.0f,  0.0f,        // TexCoord 1
                                1.0f, -1.0f, 0.0f,  // Position 2
                                1.0f,  0.0f,        // TexCoord 2
                                1.0f,  1.0f, 0.0f,  // Position 3
                                1.0f,  1.0f         // TexCoord 3
                            };
        GLushort indices[] = { 0, 1, 2, 0, 2, 3};

        glVertexAttribPointer (positionLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vVertices );
        glVertexAttribPointer (texCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3] );

        glEnableVertexAttribArray ( positionLoc );
        glEnableVertexAttribArray ( texCoordLoc );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i ( samplerLoc, 0 );
        glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
}

void OverlayRenderer::Draw () {

    for (vector<TextureRender*>::iterator  it=renderElms.begin() ; it < renderElms.end(); ++it){
        (*it)->RenderToTexture();
    }

    //reset to output program and buffer
    glUseProgram(programObject);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

#ifdef RENDERTEST
    //draw the background png, if on RENDERTEST and it has loaded
    if(bgTexture != TEXTURE_LOAD_ERROR) {
        RenderTexture(bgTexture);
    }
#endif

    for (vector<TextureRender*>::iterator  it=renderElms.begin() ; it < renderElms.end(); ++it){
        RenderTexture((*it)->getTexture());
    }

    //draw the timestamp on the screen
    DrawTimeStamp();

    //swap display buffers
    eglSwapBuffers(eglDisplay, eglSurface);
}

void OverlayRenderer::DrawTimeStamp() {

    //generate string to print
    char str[10];
    int mins, secs, frames;
    frames = (currentRefresh/2)%25;
    secs = (currentRefresh/50)%60;
    mins = (currentRefresh/50)/60;
    sprintf(str, "%2.2d:%2.2d.%2.2d", mins, secs, frames);

    //determine x position, based upon timecodePosition
    int x;
    switch(timecodePosition){
    case RIGHT:
        x = width-150;
        break;
    case CENTER:
        x = width/2-75;
        break;
    case LEFT:
        x = 10;
        break;
    default:
        return;
    }

    //write timecode to string
    ft->WriteString(str,overlayText, x,540,0.25,0.25);
}

GLuint OverlayRenderer::CreateSimpleTexture2D( )
{
   // Texture object handle
   GLuint textureId;

   // 2x2 Image, 3 bytes per pixel (R, G, B)
   GLubyte pixels[4 * 3] =
   {
      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0  // Yellow
   };

   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, &textureId );

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_2D, textureId );

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   return textureId;

}

void OverlayRenderer::Run() {
    running = true;
#ifdef RENDERTEST
    //load background png if in RENDERTEST
    LoadBG("resources/background.png");
    int frameCount = 0;
#endif

    currentRefresh = 0;

    while(running){
        //draw whilst running
        Draw();
        currentRefresh++;

//force max length of 3 seconds for test bed (otherwise currently goes on forever
#ifdef RENDERTEST
        //sleep for the rest of the frame, as in RENDERTEST vsync has not been enabled.
        usleep(20000);//20ms (rest of frame)

        frameCount++;
        if(frameCount >= 500)//limit to 150 refresh (75 frame, 3 seconds) in RENDERTEST, as currently no other stop mechanism
            break;
#endif
    }

    //wipe screen
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    eglSwapBuffers(eglDisplay, eglSurface);

#ifndef TESTPLAY
    netIO->GetClient()->SendMessage("{\"type\":\"playVideo\",\"status\":\"playback finished\"}");
#endif
}

void OverlayRenderer::Stop() {
    running = false;
#ifndef TESTPLAY
    netIO->GetClient()->SendMessage("{\"type\":\"playVideo\",\"status\":\"aborting video\"}");
#endif
}

void OverlayRenderer::PreDraw() {
    glUseProgram(programObject);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //wipe screen
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
#ifdef LIMEMASTER
    //write to screen if on LIMEMASTER
    ft->WriteString("Video Ready", overlayText, 175,238,1,1);
#endif
    //swap buffers
    eglSwapBuffers(eglDisplay, eglSurface);
}

#ifdef RENDERTEST
void OverlayRenderer::LoadBG(string filename) {
    int w, h;
    bgTexture = loadTexture(filename, w, h);
    if(bgTexture == TEXTURE_LOAD_ERROR)
        FLog::Log(FLOG_ERROR, "OverlayRenderer::LoadBG - Failed to load BG image");
}

GLuint OverlayRenderer::loadTexture(const string filename, int &width, int &height) {
   //header for testing if it is a png
   png_byte header[8];

   //open file as binary
   FILE *fp = fopen(filename.c_str(), "rb");
   if (!fp) {
     return TEXTURE_LOAD_ERROR;
   }

   //read the header
   fread(header, 1, 8, fp);

   //test if png
   int is_png = !png_sig_cmp(header, 0, 8);
   if (!is_png) {
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }

   //create png struct
   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
       NULL, NULL);
   if (!png_ptr) {
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }

   //create png info struct
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
     png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }

   //create png info struct
   png_infop end_info = png_create_info_struct(png_ptr);
   if (!end_info) {
     png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }

   //png error stuff, not sure libpng man suggests this.
   if (setjmp(png_jmpbuf(png_ptr))) {
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }

   //init png reading
   png_init_io(png_ptr, fp);

   //let libpng know you already read the first 8 bytes
   png_set_sig_bytes(png_ptr, 8);

   // read all the info up to the image data
   png_read_info(png_ptr, info_ptr);

   //variables to pass to get info
   int bit_depth, color_type;
   png_uint_32 twidth, theight;

   // get info about png
   png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
       NULL, NULL, NULL);

   //update width and height based on png info
   width = twidth;
   height = theight;

   // Update the png info struct.
   png_read_update_info(png_ptr, info_ptr);

   // Row size in bytes.
   int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

   // Allocate the image_data as a big block, to be given to opengl
   png_byte *image_data = new png_byte[rowbytes * height];
   if (!image_data) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }

   //row_pointers is for pointing to image_data for reading the png with libpng
   png_bytep *row_pointers = new png_bytep[height];
   if (!row_pointers) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     delete[] image_data;
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }
   // set the individual row_pointers to point at the correct offsets of image_data
   for (int i = 0; i < height; ++i)
     row_pointers[height - 1 - i] = image_data + i * rowbytes;

   //read the png into image_data through row_pointers
   png_read_image(png_ptr, row_pointers);

   //Now generate the OpenGL texture object
   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, width, height, 0,
       GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   //clean up memory and close stuff
   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
   delete[] image_data;
   delete[] row_pointers;
   fclose(fp);

   return texture;
 }
 #endif

GLboolean OverlayRenderer::esCreateWindow (const char* title) {
    if (!WinCreate (title)) {
        return GL_FALSE;
    }

    if (!CreateEGLContext ()) {
        return GL_FALSE;
    }

    return GL_TRUE;
}

int OverlayRenderer::Init () {
    const char *vShaderStr =
      "attribute vec4 a_position;   \n"
      "attribute vec2 a_texCoord;   \n"
      "varying vec2 v_texCoord;     \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = a_position; \n"
      "   v_texCoord = a_texCoord;  \n"
      "}                            \n";

   const char *fShaderStr =
      "precision mediump float;                            \n"
      "varying vec2 v_texCoord;                            \n"
      "uniform sampler2D s_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
#ifdef TESTPLAY
      "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
#elif defined LIMEMASTER
      "  gl_FragColor = vec4(texture2D( s_texture, v_texCoord ).r, texture2D( s_texture, v_texCoord ).g, texture2D( s_texture, v_texCoord ).b, 1);\n"
#else
      "  gl_FragColor = vec4(1, 1, 1, texture2D( s_texture, v_texCoord ).a);\n"
#endif
      "}                                            \n";

    if(CreateProgram(vShaderStr, fShaderStr, &programObject) == GL_FALSE)
        return GL_FALSE;

    positionLoc = glGetAttribLocation (programObject, "a_position" );
    texCoordLoc = glGetAttribLocation (programObject, "a_texCoord" );
    samplerLoc =  glGetUniformLocation(programObject, "s_texture" );

    // Use the program object
    glUseProgram(programObject);

    //ensure display starts off clean
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    eglSwapBuffers(eglDisplay, eglSurface);

    return GL_TRUE;
}

GLuint OverlayRenderer::CreateProgram(const char *vShaderStr, const char *fShaderStr, GLuint *programObject){
    // Load the vertex/fragment shaders
    GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    // Create the program object
    *programObject = glCreateProgram();

    if (programObject == 0)
        return GL_FALSE;

    glAttachShader(*programObject, vertexShader);
    glAttachShader(*programObject, fragmentShader);

    // Link the program
    glLinkProgram(*programObject);

    GLint linked;

    // Check the link status
    glGetProgramiv(*programObject, GL_LINK_STATUS, &linked);

    GLint infoLen = 0;
    if (!linked) {

        glGetProgramiv(*programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            printf ( "Error linking program:\n");
        }

        glDeleteProgram(*programObject);
        return GL_FALSE;
    }

    //attempt (probably fail) to setup an ortho view
    float w = width;
    float h = height;
    float f = 100;
    float n = 0.0001;

    float ortho[] = {  1.0f/w,  0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f/h, 0.0f, 0.0f,
                        0.0f, 0.0f, -2.0f/(f-n), -(f+n)/(f-n),
                        0.0f, 0.0f, 0.0f, 1.0f };

    GLint i32Location;
    i32Location = glGetUniformLocation(*programObject, "WorldViewProjection");
    glUniformMatrix4fv(i32Location, 1, GL_FALSE, ortho);

    // Set the viewport
    glViewport(0, 0, width, height);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    return GL_TRUE;
}

GLuint OverlayRenderer::LoadShader (GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
        return 0;

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            printf ("Error compiling shader\n");
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

#ifndef RENDERTEST
EGLBoolean OverlayRenderer::WinCreate(const char *title) {
   int32_t success = 0;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   uint32_t display_width;
   uint32_t display_height;

   // create an EGL window surface, passing context width/height
   success = graphics_get_display_size(0, &display_width, &display_height);
   if (success < 0) {
      return EGL_FALSE;
   }

   // You can hardcode the resolution here:
   display_width = 720;
   display_height = 576;

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = display_width;
   dst_rect.height = display_height;

   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = display_width << 16;
   src_rect.height = display_height << 16;

   dispman_display = vc_dispmanx_display_open(0);
   dispman_update = vc_dispmanx_update_start(0);

   dispman_element = vc_dispmanx_element_add (dispman_update, dispman_display, 1, &dst_rect, 0, &src_rect,
                                              DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T) 0);

   nativewindow.element = dispman_element;
   nativewindow.width = display_width;
   nativewindow.height = display_height;
   vc_dispmanx_update_submit_sync( dispman_update );

   hWnd = &nativewindow;
   width = display_width;
   height = display_height;

	return EGL_TRUE;
}
#else
EGLBoolean OverlayRenderer::WinCreate(const char *title)
{
    Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes  xattr;
    Atom wm_state;
    XWMHints hints;
    XEvent xev;
    Window win;

    /*
     * X11 native display initialization
     */

    width = 1024;
    height = 576;

    x_display = XOpenDisplay(NULL);
    if (x_display == NULL) {
        return EGL_FALSE;
    }

    root = DefaultRootWindow(x_display);

    swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
    win = XCreateWindow(x_display, root, 0, 0, width, height, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

    xattr.override_redirect = 0;
    XChangeWindowAttributes (x_display, win, CWOverrideRedirect, &xattr);

    hints.input = 1;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    // make the window visible on the screen
    XMapWindow (x_display, win);
    XStoreName (x_display, win, title);

    // get identifiers for the provided atom name strings
    wm_state = XInternAtom (x_display, "_NET_WM_STATE", 0);

    memset ( &xev, 0, sizeof(xev) );
    xev.type                 = ClientMessage;
    xev.xclient.window       = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 1;
    xev.xclient.data.l[1]    = 0;
    XSendEvent (x_display, DefaultRootWindow (x_display), 0, SubstructureNotifyMask, &xev);

    hWnd = (EGLNativeWindowType) win;
    return EGL_TRUE;
}
#endif

EGLBoolean OverlayRenderer::CreateEGLContext(){
#ifndef RENDERTEST
    EGLint attribList[] =
    {
        EGL_RED_SIZE,       8,
        EGL_GREEN_SIZE,     8,
        EGL_BLUE_SIZE,      8,
        EGL_ALPHA_SIZE,     8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };
#else
    EGLint attribList[] =
    {
        EGL_RED_SIZE,       5,
        EGL_GREEN_SIZE,     6,
        EGL_BLUE_SIZE,      5,
        EGL_ALPHA_SIZE,     (0 & 1) ? 8 : EGL_DONT_CARE,
        EGL_DEPTH_SIZE,     (0 & 2) ? 8 : EGL_DONT_CARE,
        EGL_STENCIL_SIZE,   (0 & 4) ? 8 : EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS, (0 & 8) ? 1 : 0,
        EGL_NONE
    };
#endif

    EGLint numConfigs;
    EGLConfig config;
#ifdef RENDERTEST
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
#else
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
#endif

    // Get Display
#ifdef RENDERTEST
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)x_display);
#else
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif
    if (eglDisplay == EGL_NO_DISPLAY) {
        return EGL_FALSE;
    }

    // Initialize EGL
    if (!eglInitialize(eglDisplay, NULL, NULL)) {
        return EGL_FALSE;
    }

    // Get configs
    if (!eglGetConfigs(eglDisplay, NULL, 0, &numConfigs)) {
        return EGL_FALSE;
    }

    // Choose config
    if (!eglChooseConfig(eglDisplay, attribList, &config, 1, &numConfigs)) {
        return EGL_FALSE;
    }

    eglBindAPI(EGL_OPENGL_ES_API);

    // Create a surface
    eglSurface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)hWnd, NULL);
    if (eglSurface == EGL_NO_SURFACE) {
        return EGL_FALSE;
    }

    // Create a GL context
    eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttribs );
    if (eglContext == EGL_NO_CONTEXT) {
        return EGL_FALSE;
    }

    // Make the context current
    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        return EGL_FALSE;
    }

    return EGL_TRUE;
}
