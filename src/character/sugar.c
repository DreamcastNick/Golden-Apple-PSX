/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "sugar.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Sugar character structure
enum
{
	Sugar_ArcMain_Idle0,
	Sugar_ArcMain_Idle1,
	Sugar_ArcMain_Idle2,
	Sugar_ArcMain_Idle3,
	Sugar_ArcMain_LeftA0,
	Sugar_ArcMain_LeftA1,
	Sugar_ArcMain_LeftB0,
	Sugar_ArcMain_LeftB1,
	Sugar_ArcMain_DownA0,
	Sugar_ArcMain_DownA1,
	Sugar_ArcMain_DownB0,
	Sugar_ArcMain_DownB1,
	Sugar_ArcMain_UpA0,
	Sugar_ArcMain_UpA1,
	Sugar_ArcMain_UpB0,
	Sugar_ArcMain_UpB1,
	Sugar_ArcMain_RightA0,
	Sugar_ArcMain_RightA1,
	Sugar_ArcMain_RightB0,
	Sugar_ArcMain_RightB1,

	Sugar_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Sugar_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Sugar;

//Sugar character definitions
static const CharFrame char_sugar_frame[] = {
	{Sugar_ArcMain_Idle0, {  0,   0, 148, 181}, { 50, 90}}, //0 idle 1
	{Sugar_ArcMain_Idle1, {  0,   0, 148, 181}, { 50, 90}}, //1 idle 2
	{Sugar_ArcMain_Idle2, {  0,   0, 148, 181}, { 50, 90}}, //2 idle 3
	{Sugar_ArcMain_Idle3, {  0,   0, 148, 181}, { 50, 90}}, //3 idle 4
	
	{Sugar_ArcMain_LeftA0, {  0,   0, 140, 187}, { 50, 96}}, //4 left a 1
	{Sugar_ArcMain_LeftA1, {  0,   0, 140, 187}, { 50, 96}}, //5 left a 2
	
	{Sugar_ArcMain_LeftB0, {  0,   0, 164, 184}, { 79, 93}}, //6 left b 1
	{Sugar_ArcMain_LeftB1, {  0,   0, 164, 184}, { 79, 93}}, //7 left b 2
	
	{Sugar_ArcMain_DownA0, {  0,   0, 124, 178}, { 40, 88}}, //8 down a 1
	{Sugar_ArcMain_DownA1, {  0,   0, 124, 178}, { 40, 88}}, //9 down a 2
	
	{Sugar_ArcMain_DownB0, {  0,   0, 148, 172}, { 50, 71}}, //10 down b 1
	{Sugar_ArcMain_DownB1, {  0,   0, 148, 172}, { 50, 71}}, //11 down b 2
	
	{Sugar_ArcMain_UpA0, {   0,   0,  180, 187}, { 64, 92}}, //12 up a 1
	{Sugar_ArcMain_UpA1, {   0,   0,  180, 187}, { 64, 92}}, //13 up a 2
	
	{Sugar_ArcMain_UpB0, {   0,   0,  148, 203}, { 46, 111}}, //14 up b 1
	{Sugar_ArcMain_UpB1, {   0,   0,  148, 203}, { 46, 111}}, //15 up b 2
	
	{Sugar_ArcMain_RightA0, {  0,   0, 140, 180}, { 55, 89}}, //16 right a 1
	{Sugar_ArcMain_RightA1, {  0,   0, 140, 180}, { 55, 89}}, //17 right a 2
	
	{Sugar_ArcMain_RightB0, {  0,   0, 164, 183}, { 43, 92}}, //18 right b 1
	{Sugar_ArcMain_RightB1, {  0,   0, 164, 183}, { 43, 92}}, //19 right b 2
};

static const Animation char_sugar_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, ASCR_BACK, 1}},      	   	   		   //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},       		   				   //CharAnim_Left
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},      		   				   //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},  		   					   //CharAnim_Down
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},  	          			   	   //CharAnim_DownAlt
	{2, (const u8[]){12, 13, ASCR_BACK, 1}},                		   	   //CharAnim_Up
	{2, (const u8[]){14, 15, ASCR_BACK, 1}},              				   //CharAnim_UpAlt
	{2, (const u8[]){16, 17, ASCR_BACK, 1}}, 							   //CharAnim_Right
	{2, (const u8[]){18, 19, ASCR_BACK, 1}},              				   //CharAnim_RightAlt
};

//Sugar character functions
void Char_Sugar_SetFrame(void *user, u8 frame)
{
	Char_Sugar *this = (Char_Sugar*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sugar_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Sugar_Tick(Character *character)
{
	Char_Sugar *this = (Char_Sugar*)character;

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Sugar_SetFrame);
	Character_Draw(character, &this->tex, &char_sugar_frame[this->frame]);
}

void Char_Sugar_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Sugar_Free(Character *character)
{
	Char_Sugar *this = (Char_Sugar*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Sugar_New(fixed_t x, fixed_t y)
{
	//Allocate sugar object
	Char_Sugar *this = Mem_Alloc(sizeof(Char_Sugar));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Sugar_New] Failed to allocate sugar object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Sugar_Tick;
	this->character.set_anim = Char_Sugar_SetAnim;
	this->character.free = Char_Sugar_Free;
	
	Animatable_Init(&this->character.animatable, char_sugar_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 9;
	
	this->character.focus_x = FIXED_DEC(40,1);
	this->character.focus_y = FIXED_DEC(-16,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SUGAR.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Sugar_ArcMain_Idle0
		"idle1.tim",  //Sugar_ArcMain_Idle1
		"idle2.tim",  //Sugar_ArcMain_Idle2
		"idle3.tim",  //Sugar_ArcMain_Idle3
		"lefta0.tim",  //Sugar_ArcMain_LeftA0
		"lefta1.tim",  //Sugar_ArcMain_LeftA1
		"leftb0.tim",  //Sugar_ArcMain_LeftB0
		"leftb1.tim",  //Sugar_ArcMain_LeftB1
		"downa0.tim",  //Sugar_ArcMain_DownA0
		"downa1.tim",  //Sugar_ArcMain_DownA1
		"downb0.tim",  //Sugar_ArcMain_DownB0
		"downb1.tim",  //Sugar_ArcMain_DownB1
		"upa0.tim",    //Sugar_ArcMain_UpA0
		"upa1.tim",    //Sugar_ArcMain_UpA1
		"upb0.tim",    //Sugar_ArcMain_UpB0
		"upb1.tim",    //Sugar_ArcMain_UpB1
		"righta0.tim", //Sugar_ArcMain_RightA0
		"righta1.tim", //Sugar_ArcMain_RightA1
		"rightb0.tim", //Sugar_ArcMain_RightB0
		"rightb1.tim", //Sugar_ArcMain_RightB1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
