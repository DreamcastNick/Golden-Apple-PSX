/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "robot.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Robot character structure
enum
{
	Robot_ArcMain_IdleA0,
	Robot_ArcMain_IdleA1,
	Robot_ArcMain_IdleA2,
	Robot_ArcMain_IdleA3,
	Robot_ArcMain_IdleB0,
	Robot_ArcMain_IdleB1,
	Robot_ArcMain_IdleB2,
	Robot_ArcMain_IdleB3,
	Robot_ArcMain_LeftA0,
	Robot_ArcMain_LeftA1,
	Robot_ArcMain_LeftB0,
	Robot_ArcMain_LeftB1,
	Robot_ArcMain_DownA0,
	Robot_ArcMain_DownA1,
	Robot_ArcMain_DownB0,
	Robot_ArcMain_DownB1,
	Robot_ArcMain_UpA0,
	Robot_ArcMain_UpA1,
	Robot_ArcMain_UpB0,
	Robot_ArcMain_UpB1,
	Robot_ArcMain_RightA0,
	Robot_ArcMain_RightA1,
	Robot_ArcMain_RightB0,
	Robot_ArcMain_RightB1,

	Robot_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Robot_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Robot;

//Robot character definitions
static const CharFrame char_robot_frame[] = {
	{Robot_ArcMain_IdleA0, {  0,   0,  88, 125}, { -51, -90}}, //0 idle a 1
	{Robot_ArcMain_IdleA1, {  0,   0,  88, 125}, { -51, -90}}, //1 idle a 2
	{Robot_ArcMain_IdleA2, {  0,   0,  88, 125}, { -51, -90}}, //2 idle a 3
	{Robot_ArcMain_IdleA3, {  0,   0,  88, 125}, { -51, -90}}, //3 idle a 4
	
	{Robot_ArcMain_IdleB0, {  0,   0,  88, 103}, { -55, -111}}, //4 idle b 1
	{Robot_ArcMain_IdleB1, {  0,   0,  88, 103}, { -55, -111}}, //5 idle b 2
	{Robot_ArcMain_IdleB2, {  0,   0,  88, 103}, { -55, -111}}, //6 idle b 3
	{Robot_ArcMain_IdleB3, {  0,   0,  88, 103}, { -55, -111}}, //7 idle b 4
	
	{Robot_ArcMain_LeftA0, {  0,   0, 148, 115}, {   2, -104}}, //8 left a 1
	{Robot_ArcMain_LeftA1, {  0,   0, 148, 115}, {   2, -104}}, //9 left a 2
	
	{Robot_ArcMain_LeftB0, {  0,   0,  88,  94}, {  12, -119}}, //10 left b 1
	{Robot_ArcMain_LeftB1, {  0,   0,  88,  94}, {  -4, -120}}, //11 left b 2
	
	{Robot_ArcMain_DownA0, {  0,   0, 128, 78}, { -30, -140}}, //12 down a 1
	{Robot_ArcMain_DownA1, {  0,   0, 128, 78}, { -30, -140}}, //13 down a 2
	
	{Robot_ArcMain_DownB0, {  0,   0,  96,  81}, { -10, -149}},  //14 down b 1
	{Robot_ArcMain_DownB1, {  0,   0,  96,  81}, { -15, -137}},  //15 down b 2
	
	{Robot_ArcMain_UpA0, {   0,   0,  128, 131}, { -59, -87}}, //16 up a 1
	{Robot_ArcMain_UpA1, {   0,   0,  128, 131}, { -59, -87}}, //17 up a 2
	
	{Robot_ArcMain_UpB0, {   0,   0,   96,  86}, { -70, -128}}, //18 up b 1
	{Robot_ArcMain_UpB1, {   0,   0,   96,  86}, { -70, -138}}, //19 up b 2
	
	{Robot_ArcMain_RightA0, {  0,   0, 128, 114}, { -34, -107}}, //20 right a 1
	{Robot_ArcMain_RightA1, {  0,   0, 128, 114}, { -34, -107}}, //21 right a 2
	
	{Robot_ArcMain_RightB0, {  0,   0,  96, 119}, { -65, -100}}, //22 right b 1
	{Robot_ArcMain_RightB1, {  0,   0,  96, 119}, { -55, -100}}, //23 right b 2
};

static const Animation char_robot_anim[CharAnim_Max] = {
	{3, (const u8[]){ 0,  1,  2,  3,  2,  1,  0, ASCR_BACK, 1}},      	   	   		   	   //CharAnim_Idle
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){10, 11, ASCR_CHGANI, CharAnim_IdleAlt}},      		   				   //CharAnim_LeftAlt
	{2, (const u8[]){12, 13, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){14, 15, ASCR_CHGANI, CharAnim_IdleAlt}},  	          				   //CharAnim_DownAlt
	{2, (const u8[]){16, 17, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){18, 19, ASCR_CHGANI, CharAnim_IdleAlt}},              			       //CharAnim_UpAlt
	{2, (const u8[]){20, 21, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){22, 23, ASCR_CHGANI, CharAnim_IdleAlt}},              				   //CharAnim_RightAlt
	{3, (const u8[]){ 4,  5,  6,  7,  6,  5,  4, ASCR_CHGANI, CharAnim_IdleAlt}},      	   //CharAnim_IdleAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_IdleAlt}},									   //CharAnim_Huh
};

//Robot character functions
void Char_Robot_SetFrame(void *user, u8 frame)
{
	Char_Robot *this = (Char_Robot*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_robot_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Robot_Tick(Character *character)
{
	Char_Robot *this = (Char_Robot*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Robot_SetFrame);
	
	if (stage.stage_id == StageId_2_1 && stage.timercount >= 28865 && stage.timercount <= 34799)
    Character_Draw(character, &this->tex, &char_robot_frame[this->frame]);

	//Stage specific animations
	if (stage.note_scroll >= 0)
	{
		switch (stage.stage_id)
		{
			case StageId_2_1: //Algebra
				if ((stage.timercount) == 32050)
					character->set_anim(character, CharAnim_IdleAlt);
				break;
			default:
				break;
		}
	}
}

void Char_Robot_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Robot_Free(Character *character)
{
	Char_Robot *this = (Char_Robot*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Robot_New(fixed_t x, fixed_t y)
{
	//Allocate robot object
	Char_Robot *this = Mem_Alloc(sizeof(Char_Robot));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Robot_New] Failed to allocate robot object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Robot_Tick;
	this->character.set_anim = Char_Robot_SetAnim;
	this->character.free = Char_Robot_Free;
	
	Animatable_Init(&this->character.animatable, char_robot_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.healthb_i = 19;
	
	this->character.focus_x = FIXED_DEC(68,1);
	this->character.focus_y = FIXED_DEC(116,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\ROBOT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idlea0.tim",  //Robot_ArcMain_IdleA0
		"idlea1.tim",  //Robot_ArcMain_IdleA1
		"idlea2.tim",  //Robot_ArcMain_IdleA2
		"idlea3.tim",  //Robot_ArcMain_IdleA3
		"idleb0.tim",  //Robot_ArcMain_IdleB0
		"idleb1.tim",  //Robot_ArcMain_IdleB1
		"idleb2.tim",  //Robot_ArcMain_IdleB2
		"idleb3.tim",  //Robot_ArcMain_IdleB3
		"lefta0.tim",  //Robot_ArcMain_LeftA0
		"lefta1.tim",  //Robot_ArcMain_LeftA1
		"leftb0.tim",  //Robot_ArcMain_LeftB0
		"leftb1.tim",  //Robot_ArcMain_LeftB1
		"downa0.tim",  //Robot_ArcMain_DownA0
		"downa1.tim",  //Robot_ArcMain_DownA1
		"downb0.tim",  //Robot_ArcMain_DownB0
		"downb1.tim",  //Robot_ArcMain_DownB1
		"upa0.tim",    //Robot_ArcMain_UpA0
		"upa1.tim",    //Robot_ArcMain_UpA1
		"upb0.tim",    //Robot_ArcMain_UpB0
		"upb1.tim",    //Robot_ArcMain_UpB1
		"righta0.tim", //Robot_ArcMain_RightA0
		"righta1.tim", //Robot_ArcMain_RightA1
		"rightb0.tim", //Robot_ArcMain_RightB0
		"rightb1.tim", //Robot_ArcMain_RightB1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
