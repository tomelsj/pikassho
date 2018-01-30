#ifndef _glpane_
#define _glpane_
 
#include "wx/wx.h"
#include "wx/glcanvas.h"

#include <vector>
 
class BasicGLPane : public wxGLCanvas
{
private:
    wxGLContext*	m_context;

    GLuint vao;
    GLuint vbo;
    GLuint tex;
    GLuint ebo;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;

    bool havesetup;

    std::vector<int> basecolors;

    void setupGL();
 
public:
	BasicGLPane(wxFrame* parent, int* args);
	virtual ~BasicGLPane();

    void setinitcolors(std::vector<int> thecolors);

	void resized(wxSizeEvent& evt);

	int getWidth();
	int getHeight();
 
	void render(wxPaintEvent& evt);
 
	// events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);
 
	DECLARE_EVENT_TABLE()
};
#endif 
