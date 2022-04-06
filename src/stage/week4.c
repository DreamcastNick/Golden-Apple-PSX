/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week4.h"

#include "../stage.h"
#include "../archive.h"
#include "../mem.h"
#include "../mutil.h"
#include "../timer.h"

//Week 4 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Background
} Back_Week4;

//Week 4 background functions
void Back_Week4_DrawBG(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw background
	RECT back_src = {0, 0, 256, 256};
	RECT_FIXED back_dst = {
		FIXED_DEC(-275,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(553,1),
		FIXED_DEC(367,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom);
}

static fixed_t week4_back_paraly[] = {
	FIXED_DEC(120,100),
	FIXED_DEC(120,100),
	FIXED_DEC(120,100),
	FIXED_DEC(120,100),
	FIXED_DEC(56,10),
	FIXED_DEC(104,10),
};

static fixed_t week4_back_warpx[] = {
	FIXED_DEC(40,1),
	FIXED_DEC(40,1),
	FIXED_DEC(40,1),
	FIXED_DEC(32,1),
	FIXED_DEC(32,1),
	FIXED_DEC(24,1),
};

static fixed_t week4_back_warpy[] = {
	FIXED_DEC(100,10),
	FIXED_DEC(80,10),
	FIXED_DEC(60,10),
	FIXED_DEC(40,10),
	FIXED_DEC(40,10),
	FIXED_DEC(100,10),
};

static s32 Back_Week4_GetX(int x, int y)
{
	return ((fixed_t)x << (FIXED_SHIFT + 7)) + FIXED_DEC(-320,1) - FIXED_MUL(0, week4_back_paraly[y]) + ((MUtil_Cos((animf_count << 2) + ((x + y) << 5)) * week4_back_warpx[y]) >> 7);
}

static s32 Back_Week4_GetY(int x, int y)
{
	return ((fixed_t)y << (FIXED_SHIFT + 6)) + FIXED_DEC(-172,1) - FIXED_MUL(0, week4_back_paraly[y]) + ((MUtil_Sin((animf_count << 2) + ((x + y) << 5)) * week4_back_warpy[y]) >> 7);
}

void Back_Week4_DrawBG3(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	//Get quad points
	POINT_FIXED back_dst[6][9];
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 9; x++)
		{
			back_dst[y][x].x = Back_Week4_GetX(x, y);
			back_dst[y][x].y = Back_Week4_GetY(x, y);
		}
	}
	
	//Draw 32x32 quads of the background
	for (int y = 0; y < 5; y++)
	{
		RECT back_src = {0, y * 32, 32, 32};
		for (int x = 0; x < 8; x++)
		{
			//Draw quad and increment source rect
			Stage_DrawTexArb(&this->tex_back0, &back_src, &back_dst[y][x], &back_dst[y][x + 1], &back_dst[y + 1][x], &back_dst[y + 1][x + 1], stage.camera.bzoom);
			if ((back_src.x += 32) >= 0xE0)
			if ((back_src.y += 32) >= 0xE0)
				back_src.w--;
		}
	}
}

void Back_Week4_Free(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week4_New(void)
{
	//Allocate background structure
	Back_Week4 *this = (Back_Week4*)Mem_Alloc(sizeof(Back_Week4));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week4_DrawBG3;
	this->back.free = Back_Week4_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK4\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}