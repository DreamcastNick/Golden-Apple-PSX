/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week0.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../mutil.h"
#include "../timer.h"

//Week 0 background functions
void Back_Week0_DrawBG(StageBack *back)
{
	Back_Week0 *this = (Back_Week0*)back;
	
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

	//Draw background 2
	RECT backb_src = {0, 0, 256, 256};
	RECT_FIXED backb_dst = {
		FIXED_DEC(-275,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(553,1),
		FIXED_DEC(367,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &backb_src, &backb_dst, stage.camera.bzoom);

	//Draw background 3
	RECT backc_src = {0, 0, 256, 256};
	RECT_FIXED backc_dst = {
		FIXED_DEC(-275,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(553,1),
		FIXED_DEC(367,1)
	};
	
	Stage_DrawTex(&this->tex_back2, &backc_src, &backc_dst, stage.camera.bzoom);
}

static fixed_t week0_back_paraly[] = {
	FIXED_DEC(240,100),
	FIXED_DEC(240,100),
	FIXED_DEC(240,100),
	FIXED_DEC(240,100),
	FIXED_DEC(112,10),
	FIXED_DEC(208,10),
};

static fixed_t week0_back_warpx[] = {
	FIXED_DEC(40,1),
	FIXED_DEC(40,1),
	FIXED_DEC(40,1),
	FIXED_DEC(32,1),
	FIXED_DEC(32,1),
	FIXED_DEC(24,1),
};

static fixed_t week0_back_warpy[] = {
	FIXED_DEC(40,1),
	FIXED_DEC(40,1),
	FIXED_DEC(40,1),
	FIXED_DEC(32,1),
	FIXED_DEC(32,1),
	FIXED_DEC(24,1),
};

static s32 Back_Week0_GetX(int x, int y)
{
	return ((fixed_t)x << (FIXED_SHIFT + 7)) + FIXED_DEC(-480,1) - FIXED_MUL(0, week0_back_paraly[y]) + ((MUtil_Cos((animf_count << 2) + ((x + y) << 5)) * week0_back_warpx[y]) >> 7);
}

static s32 Back_Week0_GetY(int x, int y)
{
	return ((fixed_t)y << (FIXED_SHIFT + 7)) + FIXED_DEC(-330,1) - FIXED_MUL(0, week0_back_paraly[y]) + ((MUtil_Sin((animf_count << 2) + ((x + y) << 5)) * week0_back_warpy[y]) >> 7);
}

void Back_Week0_DrawBG3(StageBack *back)
{
	Back_Week0 *this = (Back_Week0*)back;
	
	//Get quad points
	POINT_FIXED back_dst[6][9];
	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 9; x++)
		{
			back_dst[y][x].x = Back_Week0_GetX(x, y);
			back_dst[y][x].y = Back_Week0_GetY(x, y);
		}
	}
	
	//Draw 32x32 quads of the background
	for (int y = 0; y < 5; y++)
	{
		RECT back_src = {0, y * 32, 64, 32};
		for (int x = 0; x < 8; x++)
		{
			//Draw quad and increment source rect
			if (stage.stage_id == StageId_1_2 && stage.timercount >= 0 && stage.timercount <= 4565)
				Stage_DrawTexArb(&this->tex_back0, &back_src, &back_dst[y][x], &back_dst[y][x + 1], &back_dst[y + 1][x], &back_dst[y + 1][x + 1], stage.camera.bzoom);
			if (stage.stage_id == StageId_1_2 && stage.timercount >= 4565 && stage.timercount <= 10670)
				Stage_DrawTexArb(&this->tex_back1, &back_src, &back_dst[y][x], &back_dst[y][x + 1], &back_dst[y + 1][x], &back_dst[y + 1][x + 1], stage.camera.bzoom);
			if (stage.stage_id == StageId_1_2 && stage.timercount >= 10670)
				Stage_DrawTexArb(&this->tex_back2, &back_src, &back_dst[y][x], &back_dst[y][x + 1], &back_dst[y + 1][x], &back_dst[y + 1][x + 1], stage.camera.bzoom);
			
			if ((back_src.x += 32) >= 0xE0)
			if ((back_src.y += 64) >= 0xE0)
				back_src.w--;
		}
	}
}

void Back_Week0_Free(StageBack *back)
{
	Back_Week0 *this = (Back_Week0*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week0_New(void)
{
	//Allocate background structure
	Back_Week0 *this = (Back_Week0*)Mem_Alloc(sizeof(Back_Week0));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week0_DrawBG3;
	this->back.free = Back_Week0_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK0\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
