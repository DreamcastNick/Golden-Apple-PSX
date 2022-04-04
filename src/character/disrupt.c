/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "disrupt.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Disrupt character structure
enum
{
	Disrupt_ArcMain_Idle0,
	Disrupt_ArcMain_Idle1,
	Disrupt_ArcMain_Idle2,
	Disrupt_ArcMain_Idle3,
	Disrupt_ArcMain_Idle4,
	Disrupt_ArcMain_Idle5,
	Disrupt_ArcMain_Idle6,
	Disrupt_ArcMain_Idle7,
	Disrupt_ArcMain_Left0,
	Disrupt_ArcMain_Left1,
	Disrupt_ArcMain_Down0,
	Disrupt_ArcMain_Down1,
	Disrupt_ArcMain_Up0,
	Disrupt_ArcMain_Up1,
	Disrupt_ArcMain_Right0,
	Disrupt_ArcMain_Right1,
	Disrupt_ArcMain_Huh0,
	Disrupt_ArcMain_Huh1,
	Disrupt_ArcMain_Huh2,
	Disrupt_ArcMain_Huh3,
	Disrupt_ArcMain_Huh4,

	Disrupt_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Disrupt_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Disrupt;

//Disrupt character definitions
static const CharFrame char_disrupt_frame[] = {
	{Disrupt_ArcMain_Idle0, {  0,   0, 140, 180}, { 51, 90}}, //0 idle 1
	{Disrupt_ArcMain_Idle1, {  0,   0, 140, 180}, { 51, 90}}, //1 idle 2
	{Disrupt_ArcMain_Idle2, {  0,   0, 140, 180}, { 51, 90}}, //2 idle 3
	{Disrupt_ArcMain_Idle3, {  0,   0, 140, 180}, { 51, 90}}, //3 idle 4
	{Disrupt_ArcMain_Idle4, {  0,   0, 140, 180}, { 51, 90}}, //4 idle 5
	{Disrupt_ArcMain_Idle5, {  0,   0, 140, 180}, { 51, 90}}, //5 idle 6
	{Disrupt_ArcMain_Idle6, {  0,   0, 140, 180}, { 51, 90}}, //6 idle 7
	{Disrupt_ArcMain_Idle7, {  0,   0, 140, 180}, { 51, 90}}, //7 idle 8
	
	{Disrupt_ArcMain_Left0, {  0,   0, 164, 205}, { 77, 97}}, //8 left 1
	{Disrupt_ArcMain_Left1, {  0,   0, 164, 205}, { 77, 97}}, //9 left 2
	
	{Disrupt_ArcMain_Down0, {  0,   0, 164, 194}, {80, 91}}, //10 down 1
	{Disrupt_ArcMain_Down1, {  0,   0, 164, 194}, {80, 91}}, //11 down 2

	{Disrupt_ArcMain_Up0, {  0,   0,  164, 206}, {58, 111}}, //12 up 1
	{Disrupt_ArcMain_Up1, {  0,   0,  164, 206}, {58, 111}}, //13 up 2
	
	{Disrupt_ArcMain_Right0, {  0,   0, 196, 190}, {118, 100}}, //14 right 1
	{Disrupt_ArcMain_Right1, {  0,   0, 196, 190}, {118, 100}}, //15 right 2
	
	{Disrupt_ArcMain_Huh0, {  0,   0, 148, 191}, { 38,  91}},  //16 huh 0
	{Disrupt_ArcMain_Huh1, {  0,   0, 148, 191}, {  8, 121}}, //17 huh 1
	{Disrupt_ArcMain_Huh2, {  0,   0, 148, 191}, {-12, 141}}, //18 huh 2
	{Disrupt_ArcMain_Huh3, {  0,   0, 148, 191}, {-32, 161}}, //19 huh 3
	{Disrupt_ArcMain_Huh4, {  0,   0, 148, 191}, {-62, 191}}, //20 huh 4
};

static const Animation char_disrupt_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7,  7, ASCR_BACK, 1}},      	   	   	   //CharAnim_Idle
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 8,  9, ASCR_CHGANI, CharAnim_IdleAlt}},      		   				   //CharAnim_LeftAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){10, 11, ASCR_CHGANI, CharAnim_IdleAlt}},  	          				   //CharAnim_DownAlt
	{2, (const u8[]){12, 13, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){12, 13, ASCR_CHGANI, CharAnim_IdleAlt}},              			       //CharAnim_UpAlt
	{2, (const u8[]){14, 15, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){14, 15, ASCR_CHGANI, CharAnim_IdleAlt}},              				   //CharAnim_RightAlt
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7,  7, ASCR_BACK, 1}}, 				   //CharAnim_IdleAlt
	{2, (const u8[]){16, 16, 17, 17, 18, 18, 19, 19, 20, 20, ASCR_BACK, 1}},      	   	   //CharAnim_Huh
};

//Disrupt character functions
void Char_Disrupt_SetFrame(void *user, u8 frame)
{
	Char_Disrupt *this = (Char_Disrupt*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_disrupt_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Disrupt_Tick(Character *character)
{
	Char_Disrupt *this = (Char_Disrupt*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Disrupt_SetFrame);
	 
	if (stage.stage_id == StageId_1_2 && stage.timercount >= 4565 && stage.timercount <= 11555)
    Character_Draw(character, &this->tex, &char_disrupt_frame[this->frame]);
	
	//Stage specific animations
	if (stage.note_scroll >= 0)
	{
		switch (stage.stage_id)
		{
			case StageId_1_2: //Applecore
				if ((stage.timercount) == 11520)
					character->set_anim(character, CharAnim_Huh);
				break;
			default:
				break;
		}
	}
}
	
void Char_Disrupt_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Disrupt_Free(Character *character)
{
	Char_Disrupt *this = (Char_Disrupt*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Disrupt_New(fixed_t x, fixed_t y)
{
	//Allocate disrupt object
	Char_Disrupt *this = Mem_Alloc(sizeof(Char_Disrupt));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Disrupt_New] Failed to allocate disrupt object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Disrupt_Tick;
	this->character.set_anim = Char_Disrupt_SetAnim;
	this->character.free = Char_Disrupt_Free;
	
	Animatable_Init(&this->character.animatable, char_disrupt_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 8;
	
	this->character.focus_x = FIXED_DEC(140,1);
	this->character.focus_y = FIXED_DEC(25,1);
	this->character.focus_zoom = FIXED_DEC(05,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MESS.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Disrupt_ArcMain_Idle0
		"idle1.tim",  //Disrupt_ArcMain_Idle1
		"idle2.tim",  //Disrupt_ArcMain_Idle2
		"idle3.tim",  //Disrupt_ArcMain_Idle3
		"idle4.tim",  //Disrupt_ArcMain_Idle4
		"idle5.tim",  //Disrupt_ArcMain_Idle5
		"idle6.tim",  //Disrupt_ArcMain_Idle6
		"idle7.tim",  //Disrupt_ArcMain_Idle7
		"left0.tim",  //Disrupt_ArcMain_Left0
		"left1.tim",  //Disrupt_ArcMain_Left1
		"down0.tim",  //Disrupt_ArcMain_Down0
		"down1.tim",  //Disrupt_ArcMain_Down1
		"up0.tim",    //Disrupt_ArcMain_Up0
		"up1.tim",    //Disrupt_ArcMain_Up1
		"right0.tim", //Disrupt_ArcMain_Right0
		"right1.tim", //Disrupt_ArcMain_Right1
		"huh0.tim",    //Disrupt_ArcMain_Huh0
		"huh1.tim",    //Disrupt_ArcMain_Huh1
		"huh2.tim",    //Disrupt_ArcMain_Huh2
		"huh3.tim",    //Disrupt_ArcMain_Huh3
		"huh4.tim",    //Disrupt_ArcMain_Huh4
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
