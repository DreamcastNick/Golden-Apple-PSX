/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pooper.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../fixed.h"
#include "../main.h"

//Pooper character structure
enum
{
	Pooper_ArcMain_Idle0,
	Pooper_ArcMain_Idle1,
	Pooper_ArcMain_Idle2,
	Pooper_ArcMain_Idle3,
	Pooper_ArcMain_Idle4,
	Pooper_ArcMain_Left0,
	Pooper_ArcMain_Left1,
	Pooper_ArcMain_Down0,
	Pooper_ArcMain_Down1,
	Pooper_ArcMain_Up0,
	Pooper_ArcMain_Up1,
	Pooper_ArcMain_Right0,
	Pooper_ArcMain_Right1,

	Pooper_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Pooper_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Pooper;

//Pooper character definitions
static const CharFrame char_pooper_frame[] = {
	{Pooper_ArcMain_Idle0, {  0,   0, 116, 141}, { 45, 83}}, //0 idle 1
	{Pooper_ArcMain_Idle1, {  0,   0, 116, 141}, { 45, 83}}, //1 idle 2
	{Pooper_ArcMain_Idle2, {  0,   0, 116, 141}, { 45, 83}}, //2 idle 3
	{Pooper_ArcMain_Idle3, {  0,   0, 116, 141}, { 45, 83}}, //3 idle 4
	{Pooper_ArcMain_Idle4, {  0,   0, 116, 141}, { 45, 83}}, //4 idle 5
	
	{Pooper_ArcMain_Left0, {  0,   0, 156, 137}, {53, 79}}, //5 left 1
	{Pooper_ArcMain_Left1, {  0,   0, 156, 137}, {53, 79}}, //6 left 2
	
	{Pooper_ArcMain_Down0, {  0,   0, 100, 149}, {33, 79}}, //7 down 1
	{Pooper_ArcMain_Down1, {  0,   0, 100, 149}, {33, 79}}, //8 down 2

	{Pooper_ArcMain_Up0, {  0,   0,  116, 207}, {42, 149}}, //9 up 1
	{Pooper_ArcMain_Up1, {  0,   0,  116, 207}, {42, 149}}, //10 up 2
	
	{Pooper_ArcMain_Right0, {  0,   0, 156, 137}, {45, 79}}, //11 right 1
	{Pooper_ArcMain_Right1, {  0,   0, 156, 137}, {45, 79}}, //12 right 2
};

static const Animation char_pooper_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}},      	   	   	   		   		   //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},       		   				    			   //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},  		   					   				   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},     	              			       		   //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){11, 12, ASCR_BACK, 1}}, 							   				   //CharAnim_RightAlt
};

//Pooper character functions
void Char_Pooper_SetFrame(void *user, u8 frame)
{
	Char_Pooper *this = (Char_Pooper*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_pooper_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Pooper_Tick(Character *character)
{
	Char_Pooper *this = (Char_Pooper*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Pooper_SetFrame);
    Character_Draw(character, &this->tex, &char_pooper_frame[this->frame]);
}
	
void Char_Pooper_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Pooper_Free(Character *character)
{
	Char_Pooper *this = (Char_Pooper*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Pooper_New(fixed_t x, fixed_t y)
{
	//Allocate pooper object
	Char_Pooper *this = Mem_Alloc(sizeof(Char_Pooper));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Pooper_New] Failed to allocate pooper object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Pooper_Tick;
	this->character.set_anim = Char_Pooper_SetAnim;
	this->character.free = Char_Pooper_Free;
	
	Animatable_Init(&this->character.animatable, char_pooper_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 4;
	
	this->character.focus_x = FIXED_DEC(40,1);
	this->character.focus_y = FIXED_DEC(-16,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\POOPER.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Pooper_ArcMain_Idle0
		"idle1.tim",  //Pooper_ArcMain_Idle1
		"idle2.tim",  //Pooper_ArcMain_Idle2
		"idle3.tim",  //Pooper_ArcMain_Idle3
		"idle4.tim",  //Pooper_ArcMain_Idle4
		"left0.tim",  //Pooper_ArcMain_Left0
		"left1.tim",  //Pooper_ArcMain_Left1
		"down0.tim",  //Pooper_ArcMain_Down0
		"down1.tim",  //Pooper_ArcMain_Down1
		"up0.tim",    //Pooper_ArcMain_Up0
		"up1.tim",    //Pooper_ArcMain_Up1
		"right0.tim", //Pooper_ArcMain_Right0
		"right1.tim", //Pooper_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
