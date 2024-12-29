/* Asteroid Emu 
* This code uses the current "asteroid" romset from M.A.M.E (TM)
* Roms not included.
* This code is based of the original example included with Neil Bradleys 6502 CPU Core. 
* Some code below taken from M.A.M.E. (TM) version .30 from 1998, originally taken from VECSIM. See https://mamedev.org for details.  
* VECSIM Copyright 1991-1993, 1996, 2003 Hedley Rainnie, Doug Neubauer, and Eric Smith
* Profiling of the code is enabled and logged by default. See source for further details. 
*/

//For messages
#include "sys_window.h"
//Main header
#include "asteroid.h"
//For input
#include "sys_rawinput.h"
//For OpenGL Commands 
#include "sys_gl.h"
//Our CPU Core
#include "cpu_6502.h"
//For logging
#include "sys_log.h"
//For simple OpenGL line drawing
#include "emu_vector_draw.h"
//For Performance profiling
#include <chrono>
using namespace std;
using namespace chrono;
//To remove annoying warning for fopen.
#pragma warning(disable:4996 4102)

//The Game Memory Image
unsigned char* GI = nullptr;
//CPU Class
cpu_6502* CPU;
//Vector Drawing Class
EmuDraw2D* emuscreen;

//Configuration variables
int closeit = 0;
int testsw = 0;

unsigned char* vec_mem;

UINT32 m6502NmiTicks = 0;
UINT32 dwElapsedTicks = 0;

int lastret = 0;





//DVG Code Below
#define MAKE_RGB(r,g,b) ((((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define vector_word(address) ((GI[pc]) | (GI[pc+1]<<8))
int twos_comp_val(int num, int bits) { return (num << (32 - bits)) >> (32 - bits); }

void LoadRom(const char* Filename, int Offset, int Size)
{
	FILE* fp = NULL;

	fp = fopen(Filename, "rb");
	if (fp)
	{
		fread(GI + Offset, Size, 1, fp);
		fclose(fp);
	}
	else
	{
		allegro_message("ROM MISSING","Please make sure the current M.A.M.E (TM) roms for \"asteroid\" are \rextracted to the roms\\asteroid folder!\r035127-02.np3\r035145-04e.ef2\r035144-04e.h2\r035143-02.j2\r034602-01.c8");
		exit(1);
	}
}

void CloseProgram(void)
{
	closeit = 1;
}

////////////  CALL ASTEROIDS SWAPRAM  ////////////////////////////////////////////
void SwapRam(UINT32 address, UINT8 data, struct MemoryWriteByte* psMemWrite)
{
	static int asteroid_bank = 0;
	int asteroid_newbank;
	UINT8 buffer[0x100];

	asteroid_newbank = (data >> 2) & 1;
	if (asteroid_bank != asteroid_newbank)
	{
		// Perform bankswitching on page 2 and page 3
		asteroid_bank = asteroid_newbank;
		memcpy(buffer, GI + 0x200, 0x100);
		memcpy(GI + 0x200, GI + 0x300, 0x100);
		memcpy(GI + 0x300, buffer, 0x100);
	}
}

void NoWrite(UINT32 address, UINT8 data, struct MemoryWriteByte* psMemWrite)
{}

void AudioOut(UINT32 address, UINT8 data, struct MemoryWriteByte* psMemWrite)
{
}

void dvg_generate_vector_list()
{
	int pc = 0x4000;
	int sp = 0;
	int stack[4];
	int scale = 0;
	int done = 0;
	UINT16 firstwd, secondwd = 0;
	UINT16 opcode;
	int  x, y;
	int temp;
	int z;
	int a;
	int deltax;
	int deltay;
	int currentx = 0;
	int currenty = 0;
	while (!done)
	{
		firstwd = vector_word(pc);
		opcode = firstwd >> 12;
		pc++;
		pc++;
		switch (opcode)
		{
		case 0xf:
			// compute raw X and Y values //

			z = (firstwd & 0xf0) >> 4;
			y = firstwd & 0x0300;
			x = (firstwd & 0x03) << 8;

			x = x * 0xfff;
			y = y * 0xfff;

			//Check Sign Values and adjust as necessary
			if (firstwd & 0x0400)
			{
				y = -y;
			}
			if (firstwd & 0x04)
			{
				x = -x;
			}

			temp = 2 + ((firstwd >> 2) & 0x02) + ((firstwd >> 11) & 0x01);
			temp = ((scale + temp) & 0x0f);
			if (temp > 9) temp = -1;

			deltax = x >> (9 - temp);
			deltay = y >> (9 - temp);

			deltax = deltax / 0xfff;
			deltay = deltay / 0xfff;

			goto DRAW;
			break;
        
		case 0:   
		case 0x1:
		case 0x2:
		case 0x3:
		case 0x4:
		case 0x5:
		case 0x6:
		case 0x7:
		case 0x8:
		case 0x9:

			// Get Second Word
			secondwd = vector_word(pc);
			pc++;
			pc++;
			// compute raw X and Y values and intensity //
			z = secondwd >> 12;
			y = firstwd & 0x03ff;
			x = secondwd & 0x03ff;
			//Scale Y 
			x = x * 0xfff;
			y = y * 0xfff;

			//Check Sign Values and adjust as necessary
			if (firstwd & 0x0400)	{y = -y;}
			if (secondwd & 0x400)	{x = -x;}

			// Do overall scaling
			temp = scale + opcode;
			temp = temp & 0x0f;

			if (temp > 9) { temp = -1; }

			deltax = x >> (9 - temp);
			deltay = y >> (9 - temp);
			deltax = deltax / 0xfff;
			deltay = deltay / 0xfff;
		DRAW:

			if (z)
			{
				z = (z << 4) + 15;
				emuscreen->add_line((float) currentx, (float) currenty,(float) (currentx + deltax), (float) (currenty - deltay), MAKE_RGBA(z, z, z, 0xff));
			}

			currentx += deltax;
			currenty -= deltay;
			deltax, deltay = 0;
			break;

		case 0xa:

			secondwd = vector_word(pc);
			pc++;
			pc++;
			x = twos_comp_val(secondwd, 12);
			y = twos_comp_val(firstwd, 12);

			//Do overall draw scaling
			scale = (secondwd >> 12) & 0x0f;

			currenty = (1060 - y);
			currentx = x;
			break;

		case 0xb:
			done = 1;
			break;

		case 0xc:

			a = 0x4000 + ((firstwd & 0x1fff) << 1);

			stack[sp] = pc;

			if (sp == 4)
			{
				done = 1;
				sp = 0;
			}
			else
				sp = sp + 1;
			pc = a;
			break;

		case 0xd:
			sp = sp - 1;
			pc = stack[sp];
			break;

		case 0xe:

			a = 0x4000 + ((firstwd & 0x1fff) << 1);
			pc = a;
			break;
		}
	}
}

void BWVectorGeneratorInternal(UINT32 address, UINT8 data, struct MemoryWriteByte* psMemWrite)
{
	dvg_generate_vector_list();
}

/////////////////////READ KEYS FROM PIA 1 //////////////////////////////////////
UINT8 AstPIA1Read(UINT32 address, struct MemoryReadByte* psMemRead)
{
	switch (address)
	{
	case 0x01: //Kinda sorta emulate the 3K clock
		lastret ^= 1;
		if (lastret) return 0x7f;
		return 0x80;
		break;
	case 0x03: /*Shield */
		if (key[KEY_SPACE])return 0x80; break;
	case 0x04: /* Fire */
		if (key[KEY_LCONTROL])return 0x80; break;
	case 0x07: /* Self Test */
		if (testsw)return 0x80; break;
	}

	return 0x7f;
}

UINT8 AstPIA2Read(UINT32 address, struct MemoryReadByte* psMemRead)
{
	switch (address)
	{
	case 0x0: /* Coin in */
		if (key[KEY_5])
			return 0x80;
		break;
	case 0x3: /* 1 Player start */
		if (key[KEY_1])
			return 0x80;
		break;
	case 0x4: /* 2 Player start */
		if (key[KEY_2])
			return 0x80;
		break;
	case 0x5: /* Thrust */
		if (key[KEY_ALT])
			return 0x80;
		break;
	case 0x6: /* Rotate right */
		if (key[KEY_RIGHT])
			return 0x80;
		break;
	case 0x7: /* Rotate left */
		if (key[KEY_LEFT])
			return 0x80;
		break;
	}
	return 0x7f;
}

///////////////////////  MAIN LOOP /////////////////////////////////////
void asteroid_run()
{
	//Speed 1512000/60/4ish interrupts per frame/ 4.1 is accurate, but Asteroids is very forgiving and it doesn't
	//really matter or affect the emulation speed.

	if (key[KEY_ESC]) { closeit = 1; }
	if (key[KEY_F2]) {
		Sleep(300); testsw ^= 1;
		CPU->reset6502();
	}

	auto start = chrono::steady_clock::now();

	CPU->exec6502(6150);
	CPU->nmi6502();
	CPU->exec6502(6150);
	CPU->nmi6502();
	CPU->exec6502(6150);
	CPU->nmi6502();
	CPU->exec6502(6150);
	CPU->nmi6502();
	auto end = chrono::steady_clock::now();
	auto diff = end - start;
	wrlog("CPU Time this frame is %f milliseconds.", chrono::duration <double, milli>(diff).count());
	emuscreen->draw_all();
}

void asteroid_end()
{
	free(CPU);
	free(GI);
	free(emuscreen);
	wrlog("asteroids shutting down");
}

struct MemoryWriteByte AsteroidWrite[] =
{
	{ 0x3000, 0x3000, BWVectorGeneratorInternal},
	{ 0x3200, 0x3200, SwapRam},
	{ 0x3600, 0x3600, AudioOut},
	{ 0x3a00, 0x3a00, AudioOut},
	{ 0x3c00, 0x3c05, AudioOut},
	{ 0x6800, 0x7fff, NoWrite },
	{ 0x5000, 0x57ff, NoWrite },
	{(UINT32)-1,	(UINT32)-1,		NULL}
};

struct MemoryReadByte AsteroidRead[] =
{
	{ 0x2000, 0x2007, AstPIA1Read},
	{ 0x2400, 0x2407, AstPIA2Read},
	{(UINT32)-1,	(UINT32)-1,		NULL}
};

int asteroid_init()
{
	// SETUP OPENGL
	ViewOrtho(1024, 900);
	SetVSync(true);

	//Setup some really basic openGl defaults.

	glEnable(GL_BLEND);										// Enable Blending
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		// Type Of Blending To Use
	glLineWidth(2.0f);
	glPointSize(1.8f);

	//Initialize our super simple drawing class.
	emuscreen = new EmuDraw2D();

	//Inialize memory for the Game Image
	GI = (unsigned char*)malloc(65536);
	if (GI == NULL)
	{
		wrlog("Error, Can't allocate system ram!");
		exit(1);
	}
	// Clear Memory
	for (int loop = 0; loop < 0x10000; loop++)
		GI[loop] = (char)0x00;
	// load romsets:

	LoadRom("roms\\asteroid\\035127-02.np3", 0x5000, 0x800);
	LoadRom("roms\\asteroid\\035145-04e.ef2", 0x6800, 0x800);
	LoadRom("roms\\asteroid\\035144-04e.h2", 0x7000, 0x800);
	LoadRom("roms\\asteroid\\035143-02.j2", 0x7800, 0x800);

	/* Now that everything's ready to go, let's go ahead and fire up the
	* CPU emulator
	*/
	/* First set the base address of the 64K image */
	/* Now set up the read/write handlers for the emulation */
	/* Set the context in the 6502 emulator core */
	CPU = new cpu_6502(GI, AsteroidRead, AsteroidWrite, 0x7fff, 1);
	/* Now reset the processor to fetch the start vector */
	CPU->reset6502();
	CPU->log_unhandled_rw(0);
	CPU->mame_memory_handling(0);

	/* Set up some defaults so that the Asteroids code actually runs the game
	* and not just diag mode.
	*/
	GI[0x2000] = 0x7f;
	GI[0x2001] = 0x7f;
	GI[0x2002] = 0x7f;
	GI[0x2003] = 0x7f;
	GI[0x2004] = 0x7f;
	GI[0x2005] = 0x7f;
	GI[0x2006] = 0x7f;
	GI[0x2007] = 0x7f;

	GI[0x2800] = 0x02; //ff 2 coins 1 play 00 free play /02 1 coin 1 play
	GI[0x2801] = 0xff; // Just to clear random values
	GI[0x2802] = 0x0f; // number of ships
	GI[0x2803] = 0x00; //01 german /04 spanish /00 english 03 french
	///END!!!
	return 0;
}
