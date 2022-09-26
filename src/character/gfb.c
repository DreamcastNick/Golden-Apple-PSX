/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfb.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//GFB character structure
enum
{
	GFB_ArcMain_GFB0,
	GFB_ArcMain_GFB1,
	GFB_ArcMain_GFB2,
	
	GFB_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_scene;
	IO_Data arc_ptr[GFB_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_GFB;

//GFB character definitions
static const CharFrame char_gfb_frame[] = {
	{GFB_ArcMain_GFB0, {  0,   0,  74, 103}, { 37,  72}}, //0 bop left 1
	{GFB_ArcMain_GFB0, { 75,   0,  74, 103}, { 38,  72}}, //1 bop left 2
	{GFB_ArcMain_GFB0, {150,   0,  73, 102}, { 37,  72}}, //2 bop left 3
	{GFB_ArcMain_GFB0, {  0, 104,  73, 103}, { 36,  73}}, //3 bop left 4
	{GFB_ArcMain_GFB0, { 74, 104,  78, 105}, { 38,  75}}, //4 bop left 5
	{GFB_ArcMain_GFB0, {153, 103,  81, 106}, { 41,  76}}, //5 bop left 6
	
	{GFB_ArcMain_GFB1, {  0,   0,  81, 104}, { 40,  73}}, //6 bop right 1
	{GFB_ArcMain_GFB1, { 82,   0,  81, 104}, { 40,  73}}, //7 bop right 2
	{GFB_ArcMain_GFB1, {164,   0,  80, 103}, { 39,  73}}, //8 bop right 3
	{GFB_ArcMain_GFB1, {  0, 104,  79, 103}, { 38,  74}}, //9 bop right 4
	{GFB_ArcMain_GFB1, { 80, 105,  74, 104}, { 32,  74}}, //10 bop right 5
	{GFB_ArcMain_GFB1, {155, 104,  74, 104}, { 32,  74}}, //11 bop right 6
	
	{GFB_ArcMain_GFB2, {  0,   0,  73, 100}, { 34,  71}}, //12 cry 1
	{GFB_ArcMain_GFB2, { 74,   0,  73, 102}, { 35,  72}}, //13 cry 2
	{GFB_ArcMain_GFB2, {148,   0,  73, 102}, { 34,  72}}, //14 cry 3
	{GFB_ArcMain_GFB2, {  0, 101,  74, 102}, { 35,  72}}, //15 cry 4
	{GFB_ArcMain_GFB2, { 75, 102,  73, 102}, { 34,  72}}, //16 cry 5
	
};

static const Animation char_gfb_anim[CharAnim_Max] = {
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Idle
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Left
	{1, (const u8[]){ 0,  0,  1,  1,  2,  2,  3,  4,  4,  5, ASCR_BACK, 1}}, //CharAnim_LeftAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Down
	{1, (const u8[]){12, 13, 14, 15, 16, ASCR_REPEAT}},                      //CharAnim_DownAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_UpAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_LeftAlt}},                        //CharAnim_Right
	{1, (const u8[]){ 6,  6,  7,  7,  8,  8,  9, 10, 10, 11, ASCR_BACK, 1}}, //CharAnim_RightAlt
};

//GFB character functions
void Char_GFB_SetFrame(void *user, u8 frame)
{
	Char_GFB *this = (Char_GFB*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfb_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFB_Tick(Character *character)
{
	Char_GFB *this = (Char_GFB*)character;
			
		if (stage.flag & STAGE_FLAG_JUST_STEP)
		{
			//Perform dance
			if (stage.note_scroll >= character->sing_end && (stage.song_step % stage.gf_speed) == 0)
			{
				//Switch animation
				if (character->animatable.anim == CharAnim_LeftAlt || character->animatable.anim == CharAnim_Right)
					character->set_anim(character, CharAnim_RightAlt);
				else
					character->set_anim(character, CharAnim_LeftAlt);
			}
		}

	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFB_SetFrame);
	Character_Draw(character, &this->tex, &char_gfb_frame[this->frame]);
}

void Char_GFB_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFB_Free(Character *character)
{
	Char_GFB *this = (Char_GFB*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_scene);
}

Character *Char_GFB_New(fixed_t x, fixed_t y)
{
	//Allocate gfb object
	Char_GFB *this = Mem_Alloc(sizeof(Char_GFB));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFB_New] Failed to allocate gfb object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_GFB_Tick;
	this->character.set_anim = Char_GFB_SetAnim;
	this->character.free = Char_GFB_Free;
	
	Animatable_Init(&this->character.animatable, char_gfb_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;
	
	this->character.focus_x = FIXED_DEC(2,1);
	this->character.focus_y = FIXED_DEC(-40,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GF.ARC;1");
	
	const char **pathp = (const char *[]){
		"gf0.tim", //GFB_ArcMain_GFB0
		"gf1.tim", //GFB_ArcMain_GFB1
		"gf2.tim", //GFB_ArcMain_GFB2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
