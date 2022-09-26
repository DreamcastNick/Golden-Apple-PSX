/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week8.h"

#include "../mem.h"
#include "../archive.h"

//Week 8 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Background 1
	Gfx_Tex tex_back1; //Background 2
	Gfx_Tex tex_back2; //Background 3
} Back_Week8;

void Back_Week8_DrawFG(StageBack *back)
{
	Back_Week8 *this = (Back_Week8*)back;
	
	//Draw window
	fixed_t fx, fy;
	fx = stage.camera.x >> 4;
	fy = stage.camera.y >> 4;
	
	RECT windowl_src = {0, 128, 128, 256};
	RECT_FIXED windowl_dst = {
		FIXED_DEC(-275 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-150,1) - fy,
		FIXED_DEC(560 + SCREEN_WIDEOADD,1),
		FIXED_DEC(560,1)
	};
	if (stage.stage_id == StageId_1_4 && stage.timercount >= 4680 && stage.timercount <= 4710)
	Stage_DrawTex(&this->tex_back2, &windowl_src, &windowl_dst, stage.camera.bzoom);
	if (stage.stage_id == StageId_5_1 && stage.song_step >= 1022 && stage.song_step <= 1024)
	Stage_DrawTex(&this->tex_back2, &windowl_src, &windowl_dst, stage.camera.bzoom);
	
	RECT windowr_src = {0, 0, 128, 128};
	RECT_FIXED windowr_dst = {
		FIXED_DEC(-165 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(340 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};
	if (stage.stage_id == StageId_5_1 && stage.song_step >= 1919 && stage.song_step <= 1935)
	Stage_DrawTex(&this->tex_back2, &windowr_src, &windowr_dst, stage.camera.bzoom);
}

//Week 8 background functions
void Back_Week8_DrawBG(StageBack *back)
{
	Back_Week8 *this = (Back_Week8*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw background
	RECT back_src = {0, 0, 256, 256};
	RECT_FIXED back_dst = {
		FIXED_DEC(-275,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(560,1),
		FIXED_DEC(380,1)
	};
	if (stage.stage_id == StageId_1_4)
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom);
	if (stage.stage_id == StageId_5_1)
	Stage_DrawTex(&this->tex_back1, &back_src, &back_dst, stage.camera.bzoom);
}

void Back_Week8_Free(StageBack *back)
{
	Back_Week8 *this = (Back_Week8*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week8_New(void)
{
	//Allocate background structure
	Back_Week8 *this = (Back_Week8*)Mem_Alloc(sizeof(Back_Week8));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Week8_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week8_DrawBG;
	this->back.free = Back_Week8_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK8\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}