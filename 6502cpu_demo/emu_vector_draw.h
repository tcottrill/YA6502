//-----------------------------------------------------------------------------
// Copyright (c) 2011-2012
//
// Rev 11-2012
// Simple class for caching and rendering line drawing geometry
// Currently using old school glDrawArrays because glDrawElements would buy nothing here.
// Points can easily be replaced by your favorite vec2 
// TODO::Add Blendmode default to initialization
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef EMU_VECTOR_DRAW_H
#define EMU_VECTOR_DRAW_H

#include <vector>
#include "sys_gl.h"

#define MAKE_RGBA(r,g,b,a)  (r | (g << 8) | (b << 16) | (a << 24))

class fpoint
{
public:
	float x;
	float y;
	uint32_t color;

	fpoint(float x, float y, uint32_t color) : x(x), y(y), color(color) {}
	fpoint() : x(0), y(0), color(0) {}
	~fpoint() {};
};


class EmuDraw2D
{
public:

	EmuDraw2D(void);
	~EmuDraw2D();
	
	void add_line(float sx, float sy, float ex, float ey, uint32_t col);
	void draw_all();
		

private:

	std::vector<fpoint> linelist;
};


#endif