/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "diaman.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//DiaMan character structure
enum
{
	DiaMan_ArcMain_Idle0,
	DiaMan_ArcMain_Idle1,
	DiaMan_ArcMain_Left0,
	DiaMan_ArcMain_Left1,
	DiaMan_ArcMain_Down0,
	DiaMan_ArcMain_Down1,
	DiaMan_ArcMain_Up0,
	DiaMan_ArcMain_Up1,
	DiaMan_ArcMain_Right0,
	DiaMan_ArcMain_Right1,

	DiaMan_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[DiaMan_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_DiaMan;

//DiaMan character definitions
static const CharFrame char_diaman_frame[] = {
	{DiaMan_ArcMain_Idle0, {  0,   0,  64, 190}, { -50, -90}}, //0 idle 1
	{DiaMan_ArcMain_Idle1, {  0,   0,  64, 190}, { -53, -90}}, //1 idle 2

	{DiaMan_ArcMain_Left0, {  0,   0, 164, 190}, { 70, -70}}, //2 left 1
	{DiaMan_ArcMain_Left1, {  0,   0, 100, 190}, {  1, -90}}, //3 left 2

	{DiaMan_ArcMain_Down0, {  0,   0,  80, 120}, { -37, -160}}, //4 down 1
	{DiaMan_ArcMain_Down1, {  0,   0, 100,  80}, { -25, -200}}, //5 down 2

	{DiaMan_ArcMain_Up0, {  0,   0,   64, 208}, { -46, -73}}, //6 up 1
	{DiaMan_ArcMain_Up1, {  0,   0,   40, 255}, { -60, -25}}, //7 up 2

	{DiaMan_ArcMain_Right0, {  0,   0, 164, 190}, { -62, -90}}, //8 right 1
	{DiaMan_ArcMain_Right1, {  0,   0, 100, 190}, { -60, -90}}, //9 right 2
};

static const Animation char_diaman_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0,  1, ASCR_REPEAT}},								   //CharAnim_Idle
	{1, (const u8[]){ 2,  3, ASCR_BACK, 1}},       		   				   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},		         		   //CharAnim_LeftAlt
	{1, (const u8[]){ 4,  5, ASCR_BACK, 1}},  		   					   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},  	           		   //CharAnim_DownAlt
	{1, (const u8[]){ 6,  7, ASCR_BACK, 1}},                		   	   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},              		   //CharAnim_UpAlt
	{1, (const u8[]){ 8,  9, ASCR_BACK, 1}}, 							   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},              		   //CharAnim_RightAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},					   	   //CharAnim_IdleAlt
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},					   	   //CharAnim_Huh
};

//DiaMan character functions
void Char_DiaMan_SetFrame(void *user, u8 frame)
{
	Char_DiaMan *this = (Char_DiaMan*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_diaman_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_DiaMan_Tick(Character *character)
{
	Char_DiaMan *this = (Char_DiaMan*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_DiaMan_SetFrame);

	if (stage.stage_id == StageId_2_1 && stage.timercount >= 23040 && stage.timercount <= 28865)
    Character_Draw(character, &this->tex, &char_diaman_frame[this->frame]);
}

void Char_DiaMan_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_DiaMan_Free(Character *character)
{
	Char_DiaMan *this = (Char_DiaMan*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_DiaMan_New(fixed_t x, fixed_t y)
{
	//Allocate diaman object
	Char_DiaMan *this = Mem_Alloc(sizeof(Char_DiaMan));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_DiaMan_New] Failed to allocate diaman object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_DiaMan_Tick;
	this->character.set_anim = Char_DiaMan_SetAnim;
	this->character.free = Char_DiaMan_Free;
	
	Animatable_Init(&this->character.animatable, char_diaman_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 18;
	
	this->character.focus_x = FIXED_DEC(75,1);
	this->character.focus_y = FIXED_DEC(180,1);
	this->character.focus_zoom = FIXED_DEC(07,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\DIAMAN.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //DiaMan_ArcMain_Idle0
		"idle1.tim",  //DiaMan_ArcMain_Idle1
		"left0.tim",  //DiaMan_ArcMain_Left0
		"left1.tim",  //DiaMan_ArcMain_Left1
		"down0.tim",  //DiaMan_ArcMain_Down0
		"down1.tim",  //DiaMan_ArcMain_Down1
		"up0.tim",    //DiaMan_ArcMain_Up0
		"up1.tim",    //DiaMan_ArcMain_Up1
		"right0.tim", //DiaMan_ArcMain_Right0
		"right1.tim", //DiaMan_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
