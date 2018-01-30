// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glew.h"
#else
#include <GL/glew.h>
#endif

#include "main.h"

#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/glcanvas.h"
#include "wx/clrpicker.h"
#include "wx/filedlg.h"

#include <array>
#include <vector>
#include <random>
#include <string>
#include <iostream>

#include <png.h>

class MyApp: public wxApp
{
    virtual bool OnInit();
 
    wxFrame *frame;
    BasicGLPane * glPane;

public:
    void onbutton(wxCommandEvent & event);
};
 
IMPLEMENT_APP(MyApp)
 
const char* vertexSource = R"glsl(
    #version 330

    in vec2 position;
    in vec2 texcoord;

    out vec2 Texcoord;

    void main()
    {
        Texcoord = texcoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";

const char* fragmentSource = R"glsl(
    #version 330

    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D tex;

    void main()
    {
        outColor = texture(tex, Texcoord) * vec4(1.0, 1.0, 1.0, 1.0);
    }
)glsl";

bool MyApp::OnInit()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Pikassho"), wxPoint(50,50), wxSize(640,480));

    int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, WX_GL_CORE_PROFILE, 0};
 
    glPane = new BasicGLPane( (wxFrame*) frame, args);

    wxPanel* btns = new wxPanel((wxFrame*) frame);

    wxBoxSizer* btnssizer = new wxBoxSizer(wxVERTICAL);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.0, 255.0);
    std::vector<int> initcolors;

    int colors = (int) dist(mt) % 4 + 4;

    for(int i = 0; i < colors; i++)
    {
        int red = round(dist(mt));
        int green = round(dist(mt));
        int blue = round(dist(mt));

        initcolors.push_back(red);
        initcolors.push_back(green);
        initcolors.push_back(blue);

        wxColourPickerCtrl* randclr = new wxColourPickerCtrl(
            btns, 1337 + i, wxColour(red, green, blue));
        
        btnssizer->Add(randclr, 1, 0);
    }

    wxButton* savebtn = new wxButton(btns, -1, "Save");
    btnssizer->Add(savebtn, 1, 0);

    savebtn->Bind(wxEVT_BUTTON, &MyApp::onbutton, this);

    glPane->setinitcolors(initcolors);

    btns->SetSizer(btnssizer);

    sizer->Add(glPane, 1, wxEXPAND);
    sizer->Add(btns, 1, wxEXPAND);
 
    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);
 
    frame->Show();

    return true;
}

static GLubyte *pixels = NULL;
static png_byte *png_bytes = NULL;
static png_byte **png_rows = NULL;
static void screenshot_png(const char *filename, unsigned int width, unsigned int height,
        GLubyte **pixels, png_byte **png_bytes, png_byte ***png_rows) {
    size_t i, nvals;
    const size_t format_nchannels = 4;
    FILE *f = fopen(filename, "wb");
    nvals = format_nchannels * width * height;

    *pixels = (GLubyte*) realloc(*pixels, nvals * sizeof(GLubyte));
    *png_bytes = (png_byte*) realloc(*png_bytes, nvals * sizeof(png_byte));
    *png_rows = (png_byte**) realloc(*png_rows, height * sizeof(png_byte*));

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *pixels);
    for (i = 0; i < nvals; i++)
        (*png_bytes)[i] = (*pixels)[i];
    for (i = 0; i < height; i++)
        (*png_rows)[height - i - 1] = &(*png_bytes)[i * width * format_nchannels];
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();
    png_infop info = png_create_info_struct(png);
    if (!info) abort();
    if (setjmp(png_jmpbuf(png))) abort();
    png_init_io(png, f);
    png_set_IHDR(
        png,
        info,
        width,
        height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);
    png_write_image(png, *png_rows);
    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(f);
}

void MyApp::onbutton(wxCommandEvent & event)
{
    wxFileDialog 
        saveFileDialog((wxFrame*) NULL, _("Save image"), "", "",
                       "PNG files (*.png)|*.png", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...

    screenshot_png(saveFileDialog.GetPath(), 640, 480, &pixels, &png_bytes, &png_rows);
}

BEGIN_EVENT_TABLE(BasicGLPane, wxGLCanvas)
EVT_MOTION(BasicGLPane::mouseMoved)
EVT_LEFT_DOWN(BasicGLPane::mouseDown)
EVT_LEFT_UP(BasicGLPane::mouseReleased)
EVT_RIGHT_DOWN(BasicGLPane::rightClick)
EVT_LEAVE_WINDOW(BasicGLPane::mouseLeftWindow)
EVT_SIZE(BasicGLPane::resized)
EVT_KEY_DOWN(BasicGLPane::keyPressed)
EVT_KEY_UP(BasicGLPane::keyReleased)
EVT_MOUSEWHEEL(BasicGLPane::mouseWheelMoved)
EVT_PAINT(BasicGLPane::render)
END_EVENT_TABLE()
 
// some useful events to use
void BasicGLPane::mouseMoved(wxMouseEvent& event) {}
void BasicGLPane::mouseDown(wxMouseEvent& event) {}
void BasicGLPane::mouseWheelMoved(wxMouseEvent& event) {}
void BasicGLPane::mouseReleased(wxMouseEvent& event) {}
void BasicGLPane::rightClick(wxMouseEvent& event) {}
void BasicGLPane::mouseLeftWindow(wxMouseEvent& event) {}
void BasicGLPane::keyPressed(wxKeyEvent& event) {}
void BasicGLPane::keyReleased(wxKeyEvent& event) {}

BasicGLPane::BasicGLPane(wxFrame* parent, int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	m_context = new wxGLContext(this);

    havesetup = false;
 
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}
 
BasicGLPane::~BasicGLPane()
{
    glDeleteTextures(1, &tex);

    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);
	delete m_context;
}

void BasicGLPane::setinitcolors(std::vector<int> thecolors)
{
    basecolors = thecolors;
}
 
void BasicGLPane::resized(wxSizeEvent& evt)
{
//	wxGLCanvas::OnSize(evt);
 
    Refresh();
}
 
int BasicGLPane::getWidth()
{
    return GetSize().x;
}
 
int BasicGLPane::getHeight()
{
    return GetSize().y;
}

void MessageCallback( GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar* message,
                      const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

void BasicGLPane::setupGL()
{
    havesetup = true;

    GLenum err = glewInit();

    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( (GLDEBUGPROC) MessageCallback, 0 );

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    GLfloat vertices[] = {
    //  Position      Color             Texcoords
        -1.0f,  1.0f, 0.0f, 0.0f, // Top-left
         1.0f,  1.0f, 1.0f, 0.0f, // Top-right
         1.0f, -1.0f, 1.0f, 1.0f, // Bottom-right
        -1.0f, -1.0f, 0.0f, 1.0f  // Bottom-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an element array
    GLuint ebo;
    glGenBuffers(1, &ebo);

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        
        // We don't need the shader anymore.
        glDeleteShader(vertexShader);

        // Use the infoLog as you see fit.
        
        // In this simple program, we'll just leave
        return;
    }
    
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(posAttrib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int allcolors = basecolors.size() / 3;
    const int texturewidth = 128;
    const int textureheight = 128;
    constexpr int texturesize = texturewidth * textureheight;

    float pixels[texturesize * 3];
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> posdist(0, texturewidth);

    struct ColorPoint {
        int x;
        int y;
        int clridx;

        ColorPoint(int paramx, int paramy, int parmclridx) : x(paramx), y(paramy), clridx(parmclridx) {}
    };

    std::vector<ColorPoint> points;

    for (int i = 0; i < allcolors; i++)
    {
        ColorPoint cp(posdist(mt), posdist(mt), i * 3);

        points.push_back(cp);
    }

    int colorpointcount = allcolors;

    //Select the nearest point
    for (int i = 0; i < texturesize * 3; i = i + 3)
    {
        int nearest;
        int x = i % (texturewidth * 3);
        int y = (int) (i - 3) / (texturewidth * 3);
        int cmpdist = -1;

        for (int cpj = 0; cpj < colorpointcount; cpj++)
        {
            int pointdistx = points[cpj].x - x;
            pointdistx = pointdistx * pointdistx;
            int pointdisty = points[cpj].y - y;
            pointdisty = pointdisty * pointdisty;

            if(cmpdist < 0 || pointdistx + pointdisty < cmpdist)
            {
                cmpdist = pointdistx + pointdisty;
                nearest = cpj;
            }
        }

        pixels[i] = basecolors[points[nearest].clridx] / 255.0;
        pixels[i + 1] = basecolors[points[nearest].clridx + 1] / 255.0;
        pixels[i + 2] = basecolors[points[nearest].clridx + 2] / 255.0;
    }

    glTexImage2D(GL_TEXTURE_2D, 
        0, 
        GL_RGB, 
        texturewidth, 
        textureheight, 
        0, 
        GL_RGB,
        GL_FLOAT, 
        pixels);
}

void BasicGLPane::render( wxPaintEvent& evt )
{
    if(!IsShown()) return;
 
    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event

    if(!havesetup)
    {
        setupGL();
    }
 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
 
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    std::cout << glGetString(GL_VERSION);

    SwapBuffers();
}
