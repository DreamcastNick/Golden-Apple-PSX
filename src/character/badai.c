/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "badai.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Badai character structure
enum
{
	Badai_ArcMain_Idle0,
	Badai_ArcMain_Idle1,
	Badai_ArcMain_Idle2,
	Badai_ArcMain_Idle3,
	Badai_ArcMain_Idle4,
	Badai_ArcMain_Idle5,
	Badai_ArcMain_Left0,
	Badai_ArcMain_Left1,
	Badai_ArcMain_Down0,
	Badai_ArcMain_Down1,
	Badai_ArcMain_Up0,
	Badai_ArcMain_Up1,
	Badai_ArcMain_Right0,
	Badai_ArcMain_Right1,

	Badai_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Badai_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Badai;

//Badai character definitions
static const CharFrame char_badai_frame[] = {
	{Badai_ArcMain_Idle0, {  0,   0, 164, 164}, { 60, 90}}, //0 idle 1
	{Badai_ArcMain_Idle1, {  0,   0, 164, 164}, { 60, 90}}, //1 idle 2
	{Badai_ArcMain_Idle2, {  0,   0, 164, 164}, { 60, 90}}, //2 idle 3
	{Badai_ArcMain_Idle3, {  0,   0, 164, 164}, { 60, 90}}, //3 idle 4
	{Badai_ArcMain_Idle4, {  0,   0, 164, 164}, { 60, 90}}, //4 idle 5
	{Badai_ArcMain_Idle5, {  0,   0, 164, 164}, { 60, 90}}, //5 idle 6
	
	{Badai_ArcMain_Left0, {  0,   0, 184, 142}, {129, 71}}, //6 left 1
	{Badai_ArcMain_Left1, {  0,   0, 184, 142}, {129, 71}}, //7 left 2
	
	{Badai_ArcMain_Down0, {  0,   0, 196, 158}, {80, 96}}, //8 down 1
	{Badai_ArcMain_Down1, {  0,   0, 196, 158}, {80, 96}}, //9 down 2

	{Badai_ArcMain_Up0, {  0,   0,  132, 168}, {50, 106}}, //10 up 1
	{Badai_ArcMain_Up1, {  0,   0,  132, 168}, {50, 106}}, //11 up 2
	
	{Badai_ArcMain_Right0, {  0,   0, 180, 155}, {10, 90}}, //12 right 1
	{Badai_ArcMain_Right1, {  0,   0, 180, 155}, {10, 90}}, //13 right 2
};

static const Animation char_badai_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5, ASCR_BACK, 1}},      	   	   	  				   //CharAnim_Idle
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},      		   				   				   //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},  	          				   				   //CharAnim_DownAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},              			        			   //CharAnim_UpAlt
	{2, (const u8[]){12, 13, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){12, 13, ASCR_BACK, 1}},              				     			   //CharAnim_RightAlt
};

//Badai character functions
void Char_Badai_SetFrame(void *user, u8 frame)
{
	Char_Badai *this = (Char_Badai*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_badai_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Badai_Tick(Character *character)
{
	Char_Badai *this = (Char_Badai*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Badai_SetFrame);

	if (stage.stage_id == StageId_1_4 && stage.timercount >= 4700)
    Character_Draw(character, &this->tex, &char_badai_frame[this->frame]);
}
	
void Char_Badai_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Badai_Free(Character *character)
{
	Char_Badai *this = (Char_Badai*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Badai_New(fixed_t x, fixed_t y)
{
	//Allocate badai object
	Char_Badai *this = Mem_Alloc(sizeof(Char_Badai));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Badai_New] Failed to allocate badai object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Badai_Tick;
	this->character.set_anim = Char_Badai_SetAnim;
	this->character.free = Char_Badai_Free;
	
	Animatable_Init(&this->character.animatable, char_badai_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 12;
	
	this->character.focus_x = FIXED_DEC(50,1);
	this->character.focus_y = FIXED_DEC(16,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BADAI.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Badai_ArcMain_Idle0
		"idle1.tim",  //Badai_ArcMain_Idle1
		"idle2.tim",  //Badai_ArcMain_Idle2
		"idle3.tim",  //Badai_ArcMain_Idle3
		"idle4.tim",  //Badai_ArcMain_Idle4
		"idle5.tim",  //Badai_ArcMain_Idle5
		"left0.tim",  //Badai_ArcMain_Left0
		"left1.tim",  //Badai_ArcMain_Left1
		"down0.tim",  //Badai_ArcMain_Down0
		"down1.tim",  //Badai_ArcMain_Down1
		"up0.tim",    //Badai_ArcMain_Up0
		"up1.tim",    //Badai_ArcMain_Up1
		"right0.tim", //Badai_ArcMain_Right0
		"right1.tim", //Badai_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
