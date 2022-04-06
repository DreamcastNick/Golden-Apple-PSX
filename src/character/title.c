/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "title.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Title character structure
enum
{
	Title_ArcMain_Idle0,
	Title_ArcMain_Idle1,
	Title_ArcMain_Idle2,
	Title_ArcMain_Idle3,
	Title_ArcMain_Idle4,
	Title_ArcMain_Idle5,
	Title_ArcMain_Idle6,
	Title_ArcMain_Idle7,

	Title_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Title_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Title;

//Title character definitions
static const CharFrame char_title_frame[] = {
	{Title_ArcMain_Idle0, {  0,   0, 184, 173}, { 37, 72}}, //0 idle 1
	{Title_ArcMain_Idle1, {  0,   0, 184, 173}, { 37, 72}}, //1 idle 2
	{Title_ArcMain_Idle2, {  0,   0, 184, 173}, { 37, 72}}, //2 idle 3
	{Title_ArcMain_Idle3, {  0,   0, 184, 173}, { 37, 72}}, //3 idle 4
	{Title_ArcMain_Idle4, {  0,   0, 184, 173}, { 37, 72}}, //4 idle 5
	{Title_ArcMain_Idle5, {  0,   0, 184, 173}, { 37, 72}}, //5 idle 6
	{Title_ArcMain_Idle6, {  0,   0, 184, 173}, { 37, 72}}, //6 idle 7
	{Title_ArcMain_Idle7, {  0,   0, 184, 173}, { 37, 72}}, //7 idle 8
	
};

static const Animation char_title_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},      	   	   	   	   //CharAnim_Idle
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},      	   	   	   	   //CharAnim_Left
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},      	   	   	   	   //CharAnim_LeftAlt
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},      	   	   	   	   //CharAnim_Down
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},      	   	   	   	   //CharAnim_DownAlt
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},      	   	   	   	   //CharAnim_Up
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},      	   	   	   	   //CharAnim_UpAlt
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},      	   	   	   	   //CharAnim_Right
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}},      	   	   	   	   //CharAnim_RightAlt
};

//Title character functions
void Char_Title_SetFrame(void *user, u8 frame)
{
	Char_Title *this = (Char_Title*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_title_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Title_Tick(Character *character)
{
	Char_Title *this = (Char_Title*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Title_SetFrame);
	 
    Character_Draw(character, &this->tex, &char_title_frame[this->frame]);
}
	
void Char_Title_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Title_Free(Character *character)
{
	Char_Title *this = (Char_Title*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Title_New(fixed_t x, fixed_t y)
{
	//Allocate title object
	Char_Title *this = Mem_Alloc(sizeof(Char_Title));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Title_New] Failed to allocate title object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Title_Tick;
	this->character.set_anim = Char_Title_SetAnim;
	this->character.free = Char_Title_Free;
	
	Animatable_Init(&this->character.animatable, char_title_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 1;
	
	this->character.focus_x = (stage.stage_id == StageId_1_1) ? FIXED_DEC(50,1) : FIXED_DEC(140,1);
	this->character.focus_y = (stage.stage_id == StageId_1_1) ? FIXED_DEC(25,1) : FIXED_DEC(25,1);
	this->character.focus_zoom = (stage.stage_id == StageId_1_1) ? FIXED_DEC(1,1) : FIXED_DEC(05,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\TITLE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Title_ArcMain_Idle0
		"idle1.tim",  //Title_ArcMain_Idle1
		"idle2.tim",  //Title_ArcMain_Idle2
		"idle3.tim",  //Title_ArcMain_Idle3
		"idle4.tim",  //Title_ArcMain_Idle4
		"idle5.tim",  //Title_ArcMain_Idle5
		"idle6.tim",  //Title_ArcMain_Idle6
		"idle7.tim",  //Title_ArcMain_Idle7
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
