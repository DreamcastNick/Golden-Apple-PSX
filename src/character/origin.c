/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "origin.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Origin character structure
enum
{
	Origin_ArcMain_Idle0,
	Origin_ArcMain_Idle1,
	Origin_ArcMain_Idle2,
	Origin_ArcMain_Idle3,
	Origin_ArcMain_Left0,
	Origin_ArcMain_Left1,
	Origin_ArcMain_Down0,
	Origin_ArcMain_Down1,
	Origin_ArcMain_Up0,
	Origin_ArcMain_Up1,
	Origin_ArcMain_Right0,
	Origin_ArcMain_Right1,
	Origin_ArcMain_Ugh0,
	Origin_ArcMain_Ugh1,
	Origin_ArcMain_Ugh2,
	Origin_ArcMain_Ugh3,

	Origin_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Origin_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Origin;

//Origin character definitions
static const CharFrame char_origin_frame[] = {
	{Origin_ArcMain_Idle0, {  0,   0, 148, 229}, { 50, 90}}, //0 idle 1
	{Origin_ArcMain_Idle1, {  0,   0, 148, 229}, { 50, 90}}, //1 idle 2
	{Origin_ArcMain_Idle2, {  0,   0, 148, 229}, { 50, 90}}, //2 idle 3
	{Origin_ArcMain_Idle3, {  0,   0, 148, 229}, { 50, 90}}, //3 idle 4
	
	{Origin_ArcMain_Left0, {  0,   0, 224, 197}, { 90, 90}}, //4 left 1
	{Origin_ArcMain_Left1, {  0,   0, 224, 197}, { 90, 90}}, //5 left 2
	
	{Origin_ArcMain_Down0, {  0,   0, 180, 222}, {40, 20}}, //6 down 1
	{Origin_ArcMain_Down1, {  0,   0, 180, 222}, {40, 20}}, //7 down 2

	{Origin_ArcMain_Up0, {  0,   0,  164, 256}, {80, 120}}, //8 up 1
	{Origin_ArcMain_Up1, {  0,   0,  164, 256}, {80, 120}}, //9 up 2
	
	{Origin_ArcMain_Right0, {  0,   0, 248, 211}, {30, 80}}, //10 right 1
	{Origin_ArcMain_Right1, {  0,   0, 248, 211}, {30, 80}}, //11 right 2
	
	{Origin_ArcMain_Ugh0, {  0,   0, 116, 232}, {30, 90}}, //12 ugh 1
	{Origin_ArcMain_Ugh1, {  0,   0, 116, 232}, {30, 90}}, //13 ugh 2
	{Origin_ArcMain_Ugh2, {  0,   0, 116, 232}, {30, 90}}, //14 ugh 3
	{Origin_ArcMain_Ugh3, {  0,   0, 116, 232}, {30, 90}}, //15 ugh 4
};

static const Animation char_origin_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, ASCR_BACK, 1}},      	   	   	  				   	   //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){12, 13, 14, 15, ASCR_BACK, 1}},      		   				   		   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){12, 13, 14, 15, ASCR_BACK, 1}},      		   				   		   //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){12, 13, 14, 15, ASCR_BACK, 1}},      		   				   		   //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){12, 13, 14, 15, ASCR_BACK, 1}},      		   				   		   //CharAnim_RightAlt
};

//Origin character functions
void Char_Origin_SetFrame(void *user, u8 frame)
{
	Char_Origin *this = (Char_Origin*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_origin_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Origin_Tick(Character *character)
{
	Char_Origin *this = (Char_Origin*)character;

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Origin_SetFrame);

    Character_Draw(character, &this->tex, &char_origin_frame[this->frame]);
}
	
void Char_Origin_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Origin_Free(Character *character)
{
	Char_Origin *this = (Char_Origin*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Origin_New(fixed_t x, fixed_t y)
{
	//Allocate origin object
	Char_Origin *this = Mem_Alloc(sizeof(Char_Origin));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Origin_New] Failed to allocate origin object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Origin_Tick;
	this->character.set_anim = Char_Origin_SetAnim;
	this->character.free = Char_Origin_Free;
	
	Animatable_Init(&this->character.animatable, char_origin_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 21;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-36,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\ORIGIN.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Origin_ArcMain_Idle0
		"idle1.tim",  //Origin_ArcMain_Idle1
		"idle2.tim",  //Origin_ArcMain_Idle2
		"idle3.tim",  //Origin_ArcMain_Idle3
		"left0.tim",  //Origin_ArcMain_Left0
		"left1.tim",  //Origin_ArcMain_Left1
		"down0.tim",  //Origin_ArcMain_Down0
		"down1.tim",  //Origin_ArcMain_Down1
		"up0.tim",    //Origin_ArcMain_Up0
		"up1.tim",    //Origin_ArcMain_Up1
		"right0.tim", //Origin_ArcMain_Right0
		"right1.tim", //Origin_ArcMain_Right1
		"ugh0.tim",   //Origin_ArcMain_Ugh0
		"ugh1.tim",   //Origin_ArcMain_Ugh1
		"ugh2.tim",   //Origin_ArcMain_Ugh2
		"ugh3.tim",   //Origin_ArcMain_Ugh3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
