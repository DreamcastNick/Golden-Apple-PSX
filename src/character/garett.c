/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "garett.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Garett character structure
enum
{
	Garett_ArcMain_IdleA0,
	Garett_ArcMain_IdleA1,
	Garett_ArcMain_IdleA2,
	Garett_ArcMain_IdleA3,
	Garett_ArcMain_IdleB0,
	Garett_ArcMain_IdleB1,
	Garett_ArcMain_IdleB2,
	Garett_ArcMain_IdleB3,
	Garett_ArcMain_LeftA0,
	Garett_ArcMain_LeftA1,
	Garett_ArcMain_LeftB0,
	Garett_ArcMain_LeftB1,
	Garett_ArcMain_DownA0,
	Garett_ArcMain_DownA1,
	Garett_ArcMain_DownB0,
	Garett_ArcMain_DownB1,
	Garett_ArcMain_UpA0,
	Garett_ArcMain_UpA1,
	Garett_ArcMain_UpB0,
	Garett_ArcMain_UpB1,
	Garett_ArcMain_RightA0,
	Garett_ArcMain_RightA1,
	Garett_ArcMain_RightB0,
	Garett_ArcMain_RightB1,

	Garett_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Garett_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Garett;

//Garett character definitions
static const CharFrame char_garett_frame[] = {
	{Garett_ArcMain_IdleA0, {  0,   0, 180, 210}, { -54, -90}}, //0 idle a 1
	{Garett_ArcMain_IdleA1, {  0,   0, 180, 210}, { -53, -90}}, //1 idle a 2
	{Garett_ArcMain_IdleA2, {  0,   0, 180, 212}, { -53, -88}}, //2 idle a 3
	{Garett_ArcMain_IdleA3, {  0,   0, 184, 212}, { -51, -90}}, //3 idle a 4
	
	{Garett_ArcMain_IdleB0, {  0,   0, 120, 194}, { -46, -95}}, //4 idle b 1
	{Garett_ArcMain_IdleB1, {  0,   0, 116, 196}, { -46, -93}}, //5 idle b 2
	{Garett_ArcMain_IdleB2, {  0,   0, 100, 202}, { -51, -90}}, //6 idle b 3
	{Garett_ArcMain_IdleB3, {  0,   0, 100, 202}, { -51, -90}}, //7 idle b 4
	
	{Garett_ArcMain_LeftA0, {  0,   0, 172, 225}, { -36, -76}}, //8 left a 1
	{Garett_ArcMain_LeftA1, {  0,   0, 168, 222}, { -35, -80}}, //9 left a 2
	
	{Garett_ArcMain_LeftB0, {  0,   0, 160, 194}, { -37, -98}}, //10 left b 1
	{Garett_ArcMain_LeftB1, {  0,   0, 128, 200}, { -55, -92}}, //11 left b 2
	
	{Garett_ArcMain_DownA0, {  0,   0, 164, 207}, { -50, -95}}, //12 down a 1
	{Garett_ArcMain_DownA1, {  0,   0, 168, 206}, { -50, -96}}, //13 down a 2
	
	{Garett_ArcMain_DownB0, {  0,   0, 120, 155}, { -51, -137}}, //14 down b 1
	{Garett_ArcMain_DownB1, {  0,   0, 128, 166}, { -55, -126}},  //15 down b 2
	
	{Garett_ArcMain_UpA0, {   0,   0,  164, 228}, { -52, -74}}, //16 up a 1
	{Garett_ArcMain_UpA1, {   0,   0,  172, 221}, { -50, -81}}, //17 up a 2
	
	{Garett_ArcMain_UpB0, {   0,   0,  100, 219}, { -58, -73}}, //18 up b 1
	{Garett_ArcMain_UpB1, {   0,   0,  112, 204}, { -53, -87}}, //19 up b 2
	
	{Garett_ArcMain_RightA0, {  0,   0, 192, 224}, { -71, -78}}, //20 right a 1
	{Garett_ArcMain_RightA1, {  0,   0, 192, 221}, { -68, -81}}, //21 right a 2
	
	{Garett_ArcMain_RightB0, {  0,   0, 128, 202}, { -43, -90}}, //22 right b 1
	{Garett_ArcMain_RightB1, {  0,   0, 100, 202}, { -50, -92}}, //23 right b 2
};

static const Animation char_garett_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, ASCR_BACK, 1}},      	   	   		   				   //CharAnim_Idle
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){10, 11, ASCR_CHGANI, CharAnim_IdleAlt}},      		   				   //CharAnim_LeftAlt
	{2, (const u8[]){12, 13, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){14, 15, ASCR_CHGANI, CharAnim_IdleAlt}},  	          				   //CharAnim_DownAlt
	{2, (const u8[]){16, 17, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){18, 19, ASCR_CHGANI, CharAnim_IdleAlt}},              			       //CharAnim_UpAlt
	{2, (const u8[]){20, 21, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){22, 23, ASCR_CHGANI, CharAnim_IdleAlt}},              				   //CharAnim_RightAlt
	{2, (const u8[]){ 4,  5,  6,  7, ASCR_CHGANI, CharAnim_IdleAlt}},     				   //CharAnim_IdleAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_IdleAlt}},									   //CharAnim_Huh
};

//Garett character functions
void Char_Garett_SetFrame(void *user, u8 frame)
{
	Char_Garett *this = (Char_Garett*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_garett_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Garett_Tick(Character *character)
{
	Char_Garett *this = (Char_Garett*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Garett_SetFrame);
	
	if (stage.stage_id == StageId_2_1 && stage.timercount >= 4490 && stage.timercount <= 9700)
    Character_Draw(character, &this->tex, &char_garett_frame[this->frame]);

    if (stage.stage_id == StageId_2_1 && stage.timercount >= 11985 && stage.timercount <= 12105)
    Character_Draw(character, &this->tex, &char_garett_frame[this->frame]);

    if (stage.stage_id == StageId_2_1 && stage.timercount >= 14055 && stage.timercount <= 23040)
    Character_Draw(character, &this->tex, &char_garett_frame[this->frame]);
}

void Char_Garett_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Garett_Free(Character *character)
{
	Char_Garett *this = (Char_Garett*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Garett_New(fixed_t x, fixed_t y)
{
	//Allocate garett object
	Char_Garett *this = Mem_Alloc(sizeof(Char_Garett));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Garett_New] Failed to allocate garett object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Garett_Tick;
	this->character.set_anim = Char_Garett_SetAnim;
	this->character.free = Char_Garett_Free;
	
	Animatable_Init(&this->character.animatable, char_garett_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 16;
	
	this->character.focus_x = FIXED_DEC(75,1);
	this->character.focus_y = FIXED_DEC(180,1);
	this->character.focus_zoom = FIXED_DEC(07,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GARETT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idlea0.tim",  //Garett_ArcMain_IdleA0
		"idlea1.tim",  //Garett_ArcMain_IdleA1
		"idlea2.tim",  //Garett_ArcMain_IdleA2
		"idlea3.tim",  //Garett_ArcMain_IdleA3
		"idleb0.tim",  //Garett_ArcMain_IdleB0
		"idleb1.tim",  //Garett_ArcMain_IdleB1
		"idleb2.tim",  //Garett_ArcMain_IdleB2
		"idleb3.tim",  //Garett_ArcMain_IdleB3
		"lefta0.tim",  //Garett_ArcMain_LeftA0
		"lefta1.tim",  //Garett_ArcMain_LeftA1
		"leftb0.tim",  //Garett_ArcMain_LeftB0
		"leftb1.tim",  //Garett_ArcMain_LeftB1
		"downa0.tim",  //Garett_ArcMain_DownA0
		"downa1.tim",  //Garett_ArcMain_DownA1
		"downb0.tim",  //Garett_ArcMain_DownB0
		"downb1.tim",  //Garett_ArcMain_DownB1
		"upa0.tim",    //Garett_ArcMain_UpA0
		"upa1.tim",    //Garett_ArcMain_UpA1
		"upb0.tim",    //Garett_ArcMain_UpB0
		"upb1.tim",    //Garett_ArcMain_UpB1
		"righta0.tim", //Garett_ArcMain_RightA0
		"righta1.tim", //Garett_ArcMain_RightA1
		"rightb0.tim", //Garett_ArcMain_RightB0
		"rightb1.tim", //Garett_ArcMain_RightB1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
