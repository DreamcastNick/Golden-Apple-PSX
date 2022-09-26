/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week9.h"

#include "../mem.h"
#include "../archive.h"

//Week 9 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Background 1
	Gfx_Tex tex_back1; //Background 2
} Back_Week9;

//Week 9 background functions
void Back_Week9_DrawBG(StageBack *back)
{
	Back_Week9 *this = (Back_Week9*)back;
	
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
	if (stage.stage_id == StageId_4_1)
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom);

	//Draw background 2
	RECT backb_src = {0, 0, 256, 256};
	RECT_FIXED backb_dst = {
		FIXED_DEC(-330,1) - fx,
		FIXED_DEC(-240,1) - fy,
		FIXED_DEC(720,1),
		FIXED_DEC(480,1)
	};
	if (stage.stage_id == StageId_5_2)
	Stage_DrawTex(&this->tex_back1, &backb_src, &backb_dst, stage.camera.bzoom);
}

void Back_Week9_Free(StageBack *back)
{
	Back_Week9 *this = (Back_Week9*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week9_New(void)
{
	//Allocate background structure
	Back_Week9 *this = (Back_Week9*)Mem_Alloc(sizeof(Back_Week9));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week9_DrawBG;
	this->back.free = Back_Week9_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK9\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}