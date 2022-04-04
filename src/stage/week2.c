/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week2.h"

#include "../mem.h"
#include "../archive.h"

//Week 2 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_hench, arc_hench_ptr[2];
	
	Gfx_Tex tex_back0; //Background
	Gfx_Tex tex_back1; //BG Junkers
	Gfx_Tex tex_back2; //Diamond Man
	Gfx_Tex tex_back3; //Camera Flash
	
	//Henchmen state
	Gfx_Tex tex_hench;
	u8 hench_frame, hench_tex_id;

	Animatable davemad_animatable;
	Animatable davevirus_animatable;
	Animatable virusidle_animatable;
	
} Back_Week2;

//tv0 animation and rects
static const CharFrame henchmen_frame[8] = {
	//mad
	{0, { 0,  19,  47, 129}, { 128,  63}}, //dave mad 0
	//taze
	{0, {48,   0,  66, 148}, { 166,  82}}, //dave mad 1
	{0, {114, 18,  63, 130}, { 183,  63}}, //dave mad 2
	{0, {181, 18,  57, 130}, { 207,  63}}, //dave mad 3
	{1, { 8,  19,  49, 129}, { 229,  63}}, //dave mad 4
	{1, {65,  19,  49, 129}, { 229,  63}}, //dave mad 5
};



static const Animation davemad_anim[1] = {

	{2, (const u8[]){0, 0, ASCR_BACK, 1}}, 
};


static const Animation davevirus_anim[1] = {

	{2, (const u8[]){1, 2, 3, 4, 5, ASCR_BACK, 1}},
};   


static const Animation virusidle_anim[1] = {

	{2, (const u8[]){5, ASCR_BACK, 1}},
};

//Henchmen functions
void Week2_Henchmen_SetFrame(void *user, u8 frame)
{
	Back_Week2 *this = (Back_Week2*)user;
	
	//Check if this is a new frame
	if (frame != this->hench_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &henchmen_frame[this->hench_frame = frame];
		if (cframe->tex != this->hench_tex_id)
			Gfx_LoadTex(&this->tex_hench, this->arc_hench_ptr[this->hench_tex_id = cframe->tex], 0);
	}
}

void Week2_Henchmen_Draw(Back_Week2 *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &henchmen_frame[this->hench_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, (src.w + 15) << FIXED_SHIFT, (src.h + 30) << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_hench, &src, &dst, stage.camera.bzoom);
}

void Back_Week2_DrawFG(StageBack *back)
{
	Back_Week2 *this = (Back_Week2*)back;
	
	//Draw window
	fixed_t fx, fy;
	fx = stage.camera.x >> 4;
	fy = stage.camera.y >> 4;
	
	RECT window_src = {0, 0, 256, 256};
	RECT_FIXED window_dst = {
		FIXED_DEC(-275 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-200,1) - fy,
		FIXED_DEC(560 + SCREEN_WIDEOADD,1),
		FIXED_DEC(370,1)
	};
	if (stage.timercount >= 4490 && stage.timercount <= 4510|| (stage.timercount >= 9700) && stage.timercount <= 9720 || (stage.timercount >= 11985) && stage.timercount <= 11995 || (stage.timercount >= 12105) && stage.timercount <= 12125 || (stage.timercount >= 14055) && (stage.timercount <= 14075) || (stage.timercount >= 23040) && (stage.timercount <= 23060) || ((stage.timercount >= 28865) && stage.timercount <= 28885) || ((stage.timercount >= 34799) && stage.timercount <=  34819))
	Stage_DrawTex(&this->tex_back3, &window_src, &window_dst, stage.camera.bzoom);
}

//Week 2 background functions
void Back_Week2_DrawBG(StageBack *back)
{
	Back_Week2 *this = (Back_Week2*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;

	//Animate and draw dave angry
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step & 7)
		{
			case 0:
				Animatable_SetAnim(&this->davemad_animatable, 0);
				Animatable_SetAnim(&this->davevirus_animatable, 0);
				Animatable_SetAnim(&this->virusidle_animatable, 0);
				break;
		}
	}

	if (stage.timercount >= 11985 && stage.timercount <= 12105|| ((stage.timercount >= 14055) && stage.timercount <= 34799))	
	Week2_Henchmen_Draw(this, FIXED_DEC(100,1) - fx, FIXED_DEC(-4,1) - fy);
	
	switch(stage.stage_id)
	{
	
	
	case StageId_2_1:
    
	//eye
    if (stage.timercount >= 0 && stage.timercount <= 32030)
	{
	Animatable_Animate(&this->davemad_animatable, (void*)this, Week2_Henchmen_SetFrame);

	Animatable_Animate(&this->davemad_animatable, (void*)this, Week2_Henchmen_SetFrame);	
	}
	
    //drop
	else if (stage.timercount >= 32030 && stage.timercount <= 32040)
	{
	Animatable_Animate(&this->davevirus_animatable, (void*)this, Week2_Henchmen_SetFrame);

	Animatable_Animate(&this->davevirus_animatable, (void*)this, Week2_Henchmen_SetFrame);	
	}

    //warning
    else if (stage.timercount >= 32040 && stage.timercount <= 39710)
	{
	Animatable_Animate(&this->virusidle_animatable, (void*)this, Week2_Henchmen_SetFrame);

	Animatable_Animate(&this->virusidle_animatable, (void*)this, Week2_Henchmen_SetFrame);	
	}

	else
	{
	Animatable_Animate(&this->davemad_animatable, (void*)this, Week2_Henchmen_SetFrame);

	Animatable_Animate(&this->davemad_animatable, (void*)this, Week2_Henchmen_SetFrame);	
	}

	break;
  default:
break;


}

	//Draw boppers
	static const struct Back_Week2_LowerBop
	{
		RECT src;
		RECT_FIXED dst;
	} lbop_piece[] = {
		{{2, 2, 52, 128}, {FIXED_DEC(-32,1), FIXED_DEC(-72,1), FIXED_DEC(68,1), FIXED_DEC(164,1)}},
	};
	
	const struct Back_Week2_LowerBop *lbop_p = lbop_piece;
	for (size_t i = 0; i < COUNT_OF(lbop_piece); i++, lbop_p++)
	{
		RECT_FIXED lbop_dst = {
			lbop_p->dst.x - fx,
			lbop_p->dst.y - fy,
			lbop_p->dst.w,
			lbop_p->dst.h,
		};
		if (stage.timercount >= 4490 && stage.timercount <= 9700)
		Stage_DrawTex(&this->tex_back1, &lbop_p->src, &lbop_dst, stage.camera.bzoom);
	}
	
	//Draw boppers 2
	static const struct Back_Week2_LowerBop2
	{
		RECT src;
		RECT_FIXED dst;
	} lbop2_piece[] = {
		{{56, 2, 124, 139}, {FIXED_DEC(-100,1), FIXED_DEC(-108,1), FIXED_DEC(148,1), FIXED_DEC(196,1)}},
	};
	
	const struct Back_Week2_LowerBop2 *lbop2_p = lbop2_piece;
	for (size_t i = 0; i < COUNT_OF(lbop2_piece); i++, lbop2_p++)
	{
		RECT_FIXED lbop2_dst = {
			lbop2_p->dst.x - fx,
			lbop2_p->dst.y - fy,
			lbop2_p->dst.w,
			lbop2_p->dst.h,
		};
		
		if (stage.timercount >= 9700 && stage.timercount <= 11985|| ((stage.timercount >= 12105) && stage.timercount <= 39800))	
		Stage_DrawTex(&this->tex_back1, &lbop2_p->src, &lbop2_dst, stage.camera.bzoom);
	}
	
	//Draw boppers 3
	static const struct Back_Week2_LowerBop3
	{
		RECT src;
		RECT_FIXED dst;
	} lbop3_piece[] = {
		{{181, 0, 75, 153}, {FIXED_DEC(136,1), FIXED_DEC(-80,1), FIXED_DEC(96,1), FIXED_DEC(172,1)}},
	};
	
	const struct Back_Week2_LowerBop3 *lbop3_p = lbop3_piece;
	for (size_t i = 0; i < COUNT_OF(lbop3_piece); i++, lbop3_p++)
	{
		RECT_FIXED lbop3_dst = {
			lbop3_p->dst.x - fx,
			lbop3_p->dst.y - fy,
			lbop3_p->dst.w,
			lbop3_p->dst.h,
		};
		
		if (stage.timercount >= 23040 && stage.timercount <= 39800)
		Stage_DrawTex(&this->tex_back1, &lbop3_p->src, &lbop3_dst, stage.camera.bzoom);
	}
	
	//Draw boppers 4
	static const struct Back_Week2_LowerBop4
	{
		RECT src;
		RECT_FIXED dst;
	} lbop4_piece[] = {
		{{101, 154, 72, 100}, {FIXED_DEC(-220,1), FIXED_DEC(-28,1), FIXED_DEC(87,1), FIXED_DEC(116,1)}},
	};
	
	const struct Back_Week2_LowerBop4 *lbop4_p = lbop4_piece;
	for (size_t i = 0; i < COUNT_OF(lbop4_piece); i++, lbop4_p++)
	{
		RECT_FIXED lbop4_dst = {
			lbop4_p->dst.x - fx,
			lbop4_p->dst.y - fy,
			lbop4_p->dst.w,
			lbop4_p->dst.h,
		};
		
		if (stage.timercount >= 34800 && stage.timercount <= 39800)	
		Stage_DrawTex(&this->tex_back1, &lbop4_p->src, &lbop4_dst, stage.camera.bzoom);
	}
	
	//Draw boppers 5
	static const struct Back_Week2_LowerBop5
	{
		RECT src;
		RECT_FIXED dst;
	} lbop5_piece[] = {
		{{0, 0, 52, 158}, {FIXED_DEC(35,1), FIXED_DEC(-120,1), FIXED_DEC(52,1), FIXED_DEC(210,1)}},
	};
	
	const struct Back_Week2_LowerBop5 *lbop5_p = lbop5_piece;
	for (size_t i = 0; i < COUNT_OF(lbop5_piece); i++, lbop5_p++)
	{
		RECT_FIXED lbop5_dst = {
			lbop5_p->dst.x - fx,
			lbop5_p->dst.y - fy,
			lbop5_p->dst.w,
			lbop5_p->dst.h,
		};
		
		if (stage.timercount >= 14055 && stage.timercount <= 23040)	
		Stage_DrawTex(&this->tex_back2, &lbop5_p->src, &lbop5_dst, stage.camera.bzoom);
	}
	
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

void Back_Week2_Free(StageBack *back)
{
	Back_Week2 *this = (Back_Week2*)back;

	//Free henchmen archive
	Mem_Free(this->arc_hench);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week2_New(void)
{
	//Allocate background structure
	Back_Week2 *this = (Back_Week2*)Mem_Alloc(sizeof(Back_Week2));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Week2_DrawFG;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week2_DrawBG;
	this->back.free = Back_Week2_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK2\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Mem_Free(arc_back);
	
	//Load henchmen textures
	this->arc_hench = IO_Read("\\WEEK2\\HENCH.ARC;1");
	this->arc_hench_ptr[0] = Archive_Find(this->arc_hench, "hench0.tim");
	this->arc_hench_ptr[1] = Archive_Find(this->arc_hench, "hench1.tim");
	
	//Initialize henchmen state
	Animatable_Init(&this->davemad_animatable, davemad_anim);
	Animatable_Init(&this->davevirus_animatable, davevirus_anim);
	Animatable_Init(&this->virusidle_animatable, virusidle_anim);
	
	Animatable_SetAnim(&this->davemad_animatable, 0);
	Animatable_SetAnim(&this->davevirus_animatable, 0);
	Animatable_SetAnim(&this->virusidle_animatable, 0);
	this->hench_frame = this->hench_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
