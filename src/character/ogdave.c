/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ogdave.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//OGDave character structure
enum
{
	OGDave_ArcMain_IdleA0,
	OGDave_ArcMain_IdleB0,
	OGDave_ArcMain_LeftA0,
	OGDave_ArcMain_LeftA1,
	OGDave_ArcMain_LeftB0,
	OGDave_ArcMain_LeftB1,
	OGDave_ArcMain_DownA0,
	OGDave_ArcMain_DownA1,
	OGDave_ArcMain_DownB0,
	OGDave_ArcMain_DownB1,
	OGDave_ArcMain_UpA0,
	OGDave_ArcMain_UpA1,
	OGDave_ArcMain_UpB0,
	OGDave_ArcMain_UpB1,
	OGDave_ArcMain_RightA0,
	OGDave_ArcMain_RightA1,
	OGDave_ArcMain_RightB0,
	OGDave_ArcMain_RightB1,

	OGDave_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[OGDave_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_OGDave;

//OGDave character definitions
static const CharFrame char_ogdave_frame[] = {
	{OGDave_ArcMain_IdleA0, {  0,   0, 104, 186}, { -51, -92}}, //0 idle a 1
	
	{OGDave_ArcMain_IdleB0, {  0,   0, 104, 186}, { -51, -92}}, //1 idle b 1
	
	{OGDave_ArcMain_LeftA0, {  0,   0, 136, 186}, { -20, -92}}, //2 left a 1
	{OGDave_ArcMain_LeftA1, {  0,   0, 128, 186}, { -28, -92}}, //3 left a 2
	
	{OGDave_ArcMain_LeftB0, {  0,   0, 164, 194}, { -4, -80}}, //4 left b 1
	{OGDave_ArcMain_LeftB1, {  0,   0, 148, 188}, { -7, -92}}, //5 left b 2
	
	{OGDave_ArcMain_DownA0, {  0,   0, 104, 178}, { -51, -96}}, //6 down a 1
	{OGDave_ArcMain_DownA1, {  0,   0, 108, 180}, { -49, -94}}, //7 down a 2
	
	{OGDave_ArcMain_DownB0, {  0,   0, 104, 164}, { -50, -110}}, //8 down b 1
	{OGDave_ArcMain_DownB1, {  0,   0, 132, 172}, { -43, -102}}, //9 down b 2
	
	{OGDave_ArcMain_UpA0, {   0,   0,  120, 190}, { -46, -89}}, //10 up a 1
	{OGDave_ArcMain_UpA1, {   0,   0,  120, 190}, { -36, -89}}, //11 up a 2
	
	{OGDave_ArcMain_UpB0, {   0,   0,  120, 214}, { -51, -60}}, //12 up b 1
	{OGDave_ArcMain_UpB1, {   0,   0,  108, 210}, { -51, -64}}, //13 up b 2
	
	{OGDave_ArcMain_RightA0, {  0,   0, 132, 182}, { -51, -92}}, //14 right a 1
	{OGDave_ArcMain_RightA1, {  0,   0, 116, 184}, { -51, -92}}, //15 right a 2
	
	{OGDave_ArcMain_RightB0, {  0,   0, 148, 190}, { -15, -90}}, //16 right b 1
	{OGDave_ArcMain_RightB1, {  0,   0, 132, 188}, { -21, -86}}, //17 right b 2
};

static const Animation char_ogdave_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, ASCR_BACK, 1}},      	   	   				   	   //CharAnim_Idle
	{2, (const u8[]){ 2,  3, ASCR_BACK, 1}},       		   				   //CharAnim_Left
	{2, (const u8[]){ 4,  5, ASCR_CHGANI, CharAnim_IdleAlt}},      		   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},  		   					   //CharAnim_Down
	{2, (const u8[]){ 8,  9, ASCR_CHGANI, CharAnim_IdleAlt}},  	           //CharAnim_DownAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},                		   	   //CharAnim_Up
	{2, (const u8[]){12, 13, ASCR_CHGANI, CharAnim_IdleAlt}},              //CharAnim_UpAlt
	{2, (const u8[]){14, 15, ASCR_BACK, 1}}, 							   //CharAnim_Right
	{2, (const u8[]){16, 17, ASCR_CHGANI, CharAnim_IdleAlt}},              //CharAnim_RightAlt
	{2, (const u8[]){ 1, ASCR_CHGANI, CharAnim_IdleAlt}},      	   	  	   //CharAnim_IdleAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_IdleAlt}},					   //CharAnim_Huh
};

//OGDave character functions
void Char_OGDave_SetFrame(void *user, u8 frame)
{
	Char_OGDave *this = (Char_OGDave*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_ogdave_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_OGDave_Tick(Character *character)
{
	Char_OGDave *this = (Char_OGDave*)character;
	
	if (stage.stage_id == StageId_2_1 && stage.timercount >= 4490)
	this->character.health_i = 16;

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 9700)
	this->character.health_i = 3;

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 11985)
	this->character.health_i = 16;

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 12105)
	this->character.health_i = 3;

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 14055)
	this->character.health_i = 17;

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 23040)
	this->character.health_i = 18;

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 28865)
	this->character.health_i = 19;

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 34800)
	this->character.health_i = 3;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_OGDave_SetFrame);
	
	if (stage.stage_id == StageId_2_1 && stage.timercount <= 4490)
	Character_Draw(character, &this->tex, &char_ogdave_frame[this->frame]);

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 9700 && stage.timercount <= 11985)
	Character_Draw(character, &this->tex, &char_ogdave_frame[this->frame]);

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 12105 && stage.timercount <= 14055)
	Character_Draw(character, &this->tex, &char_ogdave_frame[this->frame]);

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 34800 && stage.timercount <= 40000)
	Character_Draw(character, &this->tex, &char_ogdave_frame[this->frame]);
}

void Char_OGDave_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_OGDave_Free(Character *character)
{
	Char_OGDave *this = (Char_OGDave*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_OGDave_New(fixed_t x, fixed_t y)
{
	//Allocate ogdave object
	Char_OGDave *this = Mem_Alloc(sizeof(Char_OGDave));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_OGDave_New] Failed to allocate ogdave object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_OGDave_Tick;
	this->character.set_anim = Char_OGDave_SetAnim;
	this->character.free = Char_OGDave_Free;
	
	Animatable_Init(&this->character.animatable, char_ogdave_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 3;
	
	this->character.focus_x = FIXED_DEC(75,1);
	this->character.focus_y = FIXED_DEC(180,1);
	this->character.focus_zoom = FIXED_DEC(07,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\OGDAVE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idlea0.tim",  //OGDave_ArcMain_IdleA0
		"idleb0.tim",  //OGDave_ArcMain_IdleB0
		"lefta0.tim",  //OGDave_ArcMain_LeftA0
		"lefta1.tim",  //OGDave_ArcMain_LeftA1
		"leftb0.tim",  //OGDave_ArcMain_LeftB0
		"leftb1.tim",  //OGDave_ArcMain_LeftB1
		"downa0.tim",  //OGDave_ArcMain_DownA0
		"downa1.tim",  //OGDave_ArcMain_DownA1
		"downb0.tim",  //OGDave_ArcMain_DownB0
		"downb1.tim",  //OGDave_ArcMain_DownB1
		"upa0.tim",    //OGDave_ArcMain_UpA0
		"upa1.tim",    //OGDave_ArcMain_UpA1
		"upb0.tim",    //OGDave_ArcMain_UpB0
		"upb1.tim",    //OGDave_ArcMain_UpB1
		"righta0.tim", //OGDave_ArcMain_RightA0
		"righta1.tim", //OGDave_ArcMain_RightA1
		"rightb0.tim", //OGDave_ArcMain_RightB0
		"rightb1.tim", //OGDave_ArcMain_RightB1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
