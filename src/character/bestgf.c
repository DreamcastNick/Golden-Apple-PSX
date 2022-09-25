/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bestgf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BestGF character structure
enum
{
	BestGF_ArcMain_BestGF0,
	
	BestGF_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_scene;
	IO_Data arc_ptr[BestGF_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BestGF;

//BestGF character definitions
static const CharFrame char_bestgf_frame[] = {
	{BestGF_ArcMain_BestGF0, {  0,   0, 196, 173}, { 37,  72}}, //0 best frame
};

static const Animation char_bestgf_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, ASCR_BACK, 1}},	                                 //CharAnim_Idle
	{2, (const u8[]){ 0, ASCR_BACK, 1}},	                                 //CharAnim_Left
	{2, (const u8[]){ 0, ASCR_BACK, 1}},	                                 //CharAnim_LeftAlt
	{2, (const u8[]){ 0, ASCR_BACK, 1}},	                                 //CharAnim_Down
	{2, (const u8[]){ 0, ASCR_BACK, 1}},	                                 //CharAnim_DownAlt
	{2, (const u8[]){ 0, ASCR_BACK, 1}},	                                 //CharAnim_Up
	{2, (const u8[]){ 0, ASCR_BACK, 1}},	                                 //CharAnim_UpAlt
	{2, (const u8[]){ 0, ASCR_BACK, 1}},	                                 //CharAnim_Right
	{2, (const u8[]){ 0, ASCR_BACK, 1}},	                                 //CharAnim_RightAlt
};

//BestGF character functions
void Char_BestGF_SetFrame(void *user, u8 frame)
{
	Char_BestGF *this = (Char_BestGF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bestgf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BestGF_Tick(Character *character)
{
	Char_BestGF *this = (Char_BestGF*)character;
			
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
	Animatable_Animate(&character->animatable, (void*)this, Char_BestGF_SetFrame);
	Character_Draw(character, &this->tex, &char_bestgf_frame[this->frame]);
}

void Char_BestGF_SetAnim(Character *character, u8 anim)
{
	//Set animation
	if (anim == CharAnim_Left || anim == CharAnim_Down || anim == CharAnim_Up || anim == CharAnim_Right || anim == CharAnim_UpAlt)
		character->sing_end = stage.note_scroll + FIXED_DEC(22,1); //Nearly 2 steps
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_BestGF_Free(Character *character)
{
	Char_BestGF *this = (Char_BestGF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_scene);
}

Character *Char_BestGF_New(fixed_t x, fixed_t y)
{
	//Allocate bestgf object
	Char_BestGF *this = Mem_Alloc(sizeof(Char_BestGF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BestGF_New] Failed to allocate bestgf object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BestGF_Tick;
	this->character.set_anim = Char_BestGF_SetAnim;
	this->character.free = Char_BestGF_Free;
	
	Animatable_Init(&this->character.animatable, char_bestgf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 0;
	
	this->character.focus_x = FIXED_DEC(2,1);
	this->character.focus_y = FIXED_DEC(-40,1);
	this->character.focus_zoom = FIXED_DEC(2,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BEST.ARC;1");
	
	const char **pathp = (const char *[]){
		"bestgf0.tim", //BestGF_ArcMain_BestGF0
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
