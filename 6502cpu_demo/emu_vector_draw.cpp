#include "emu_vector_draw.h"
#include "sys_log.h"

EmuDraw2D::EmuDraw2D(void) //Add init here for blending color or BW from emulator code
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

EmuDraw2D::~EmuDraw2D()
{
	//Nothing here.
}


void EmuDraw2D::add_line(float sx, float sy, float ex, float ey, uint32_t col)
{
	linelist.emplace_back(sx, sy, col);
	linelist.emplace_back(ex, ey, col);
}

void EmuDraw2D::draw_all()
{
	if (!linelist.empty())
	{

		glDisable(GL_TEXTURE_2D);
		//Draw Lines and endpoints
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, sizeof(fpoint), &linelist[0].x);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(fpoint), &linelist[0].color);

		glDrawArrays(GL_POINTS, 0, (GLsizei)linelist.size());
		glDrawArrays(GL_LINES, 0, (GLsizei)linelist.size());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		//Reset index pointers.
		linelist.clear();
	}
}