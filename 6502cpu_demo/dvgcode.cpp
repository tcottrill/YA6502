//DVG Code Below
#include "sys_gl.h"
#include "sys_log.h"

extern unsigned char* GI;

int twos_comp_val(int num, int bits)
{
	return (num << (32 - bits)) >> (32 - bits);
}

#define VEC_SHIFT 16
#define MAKE_RGB(r,g,b) ((((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define vector_word(address) ((GI[pc]) | (GI[pc+1]<<8))


void dvg_generate_vector_list()
{
	int pc = 0x4000;
	int sp = 0;
	int stack[4];
	int scale = 0;
	int color = 0;
	int done = 0;
	UINT16 firstwd, secondwd = 0;
	UINT16 opcode;
	int  x, y;
	int temp;
	int z;
	int a;
	int deltax, deltay;
	int currentx, currenty;

	while (!done)
	{
		firstwd = vector_word(pc);
		opcode = firstwd >> 12;
		pc++;
		if ((opcode >= 0 /* DVCTR */) && (opcode <= 0x0a /*DLABS*/))
		{
			secondwd = vector_word(pc);
			pc++;

		}

		switch (opcode)
		{
		case 0xf:
			// compute raw X and Y values //

			z = (firstwd & 0xf0) >> 4;
			y = firstwd & 0x0300;
			x = (firstwd & 0x03) << 8;

			//Check Sign Values and adjust as necessary
			if (firstwd & 0x0400) { y = -y; }
			if (firstwd & 0x04) { x = -x; }

			y = -y; //inversion

			temp = 2 + ((firstwd >> 2) & 0x02) + ((firstwd >> 11) & 0x01);
			temp = ((scale + temp) & 0x0f);
			if (temp > 9) { temp = -1; }

			deltax = (x << VEC_SHIFT) >> (9 - temp);
			deltay = (y << VEC_SHIFT) >> (9 - temp);
			goto DRAWCODE;

			currentx += deltax;
			currenty -= deltay;
			deltax, deltay = 0;
			break;

		case 0:	done = 1; break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:

			// compute raw X and Y values and intensity //
			z = secondwd >> 12;
			y = firstwd & 0x03ff;
			x = secondwd & 0x03ff;
			//Check Sign Values and adjust as necessary
			if (firstwd & 0x0400)
			{
				y = -y;
			}
			if (secondwd & 0x400)
			{
				x = -x;
			}

			y = -y;

			//Invert Drawing if in Cocktal mode and Player 2 selected

			// Do overall scaling
			temp = scale + (opcode >> 12);
			temp = temp & 0x0f;

			if (temp > 0x09) { temp = -1; }

			deltax = (x << VEC_SHIFT) >> (9 - temp);
			deltay = (y << VEC_SHIFT) >> (9 - temp);

		DRAWCODE:
			if (z)
			{
				if (z) z = (z << 4) | 0x0f;
				color = MAKE_RGB(z,z,z);


				if ((currentx == (currentx)+deltax) && (currenty == (currenty)-deltay))
				{
					//Add_Point(currentx >> VEC_SHIFT, currenty >> VEC_SHIFT, color);
				}
				else {
					//Add_Line((currentx >> VEC_SHIFT), (currenty >> VEC_SHIFT), ((currentx + deltax) >> VEC_SHIFT), ((currenty - deltay) >> VEC_SHIFT), color);
					//Add_Point(currentx >> VEC_SHIFT, currenty >> VEC_SHIFT, color);
					//Add_Point((currentx + deltax) >> VEC_SHIFT, (currenty - deltay) >> VEC_SHIFT, color);
				}
			}
		
		currentx += deltax;
		currenty -= deltay;
		deltax, deltay = 0;
		break;

		case 0xa:
			
			x = twos_comp_val(secondwd, 12);
			y = twos_comp_val(firstwd, 12);

			//Invert the screen drawing if cocktail and player 2 selected

			//Do overall draw scaling
			y = 1023 - y; //Invert Y
			scale = (secondwd >> 12) & 0x0f;
			currenty = (870 - y) << VEC_SHIFT; //With Ortho Scaling
			currentx = x << VEC_SHIFT;
			break;

		case 0xb: done = 1; break;

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