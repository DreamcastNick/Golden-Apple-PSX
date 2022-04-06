/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cripple.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Cripple character structure
enum
{
	Cripple_ArcMain_Idle0,
	Cripple_ArcMain_Idle1,
	Cripple_ArcMain_Idle2,
	Cripple_ArcMain_Idle3,
	Cripple_ArcMain_Left0,
	Cripple_ArcMain_Left1,
	Cripple_ArcMain_Down0,
	Cripple_ArcMain_Down1,
	Cripple_ArcMain_Up0,
	Cripple_ArcMain_Up1,
	Cripple_ArcMain_Right0,
	Cripple_ArcMain_Right1,

	Cripple_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Cripple_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Cripple;

//Cripple character definitions
static const CharFrame char_cripple_frame[] = {
	{Cripple_ArcMain_Idle0, {  0,   0, 116, 162}, { 50, 90}}, //0 idle 1
	{Cripple_ArcMain_Idle1, {  0,   0, 116, 162}, { 50, 90}}, //1 idle 2
	{Cripple_ArcMain_Idle2, {  0,   0, 116, 162}, { 50, 90}}, //2 idle 3
	{Cripple_ArcMain_Idle3, {  0,   0, 116, 162}, { 50, 90}}, //3 idle 4
	
	{Cripple_ArcMain_Left0, {  0,   0, 136, 180}, { 65, 95}}, //4 left 1
	{Cripple_ArcMain_Left1, {  0,   0, 136, 180}, { 65, 95}}, //5 left 2
	
	{Cripple_ArcMain_Down0, {  0,   0, 136, 180}, {65, 95}}, //6 down 1
	{Cripple_ArcMain_Down1, {  0,   0, 136, 180}, {65, 95}}, //7 down 2

	{Cripple_ArcMain_Up0, {  0,   0,  136, 180}, {65, 95}}, //8 up 1
	{Cripple_ArcMain_Up1, {  0,   0,  136, 180}, {65, 95}}, //9 up 2
	
	{Cripple_ArcMain_Right0, {  0,   0, 136, 180}, {65, 95}}, //10 right 1
	{Cripple_ArcMain_Right1, {  0,   0, 136, 180}, {65, 95}}, //11 right 2
};

static const Animation char_cripple_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, ASCR_BACK, 1}},      	   	   	  				   	   //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},      		   				   				   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},  	          				   				   //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},              			        			   //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},              				     			   //CharAnim_RightAlt
};

//Cripple character functions
void Char_Cripple_SetFrame(void *user, u8 frame)
{
	Char_Cripple *this = (Char_Cripple*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_cripple_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Cripple_Tick(Character *character)
{
	Char_Cripple *this = (Char_Cripple*)character;

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Cripple_SetFrame);

    Character_Draw(character, &this->tex, &char_cripple_frame[this->frame]);
}
	
void Char_Cripple_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Cripple_Free(Character *character)
{
	Char_Cripple *this = (Char_Cripple*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Cripple_New(fixed_t x, fixed_t y)
{
	//Allocate cripple object
	Char_Cripple *this = Mem_Alloc(sizeof(Char_Cripple));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Cripple_New] Failed to allocate cripple object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Cripple_Tick;
	this->character.set_anim = Char_Cripple_SetAnim;
	this->character.free = Char_Cripple_Free;
	
	Animatable_Init(&this->character.animatable, char_cripple_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 6;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-10,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\CRIPPLE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Cripple_ArcMain_Idle0
		"idle1.tim",  //Cripple_ArcMain_Idle1
		"idle2.tim",  //Cripple_ArcMain_Idle2
		"idle3.tim",  //Cripple_ArcMain_Idle3
		"left0.tim",  //Cripple_ArcMain_Left0
		"left1.tim",  //Cripple_ArcMain_Left1
		"down0.tim",  //Cripple_ArcMain_Down0
		"down1.tim",  //Cripple_ArcMain_Down1
		"up0.tim",    //Cripple_ArcMain_Up0
		"up1.tim",    //Cripple_ArcMain_Up1
		"right0.tim", //Cripple_ArcMain_Right0
		"right1.tim", //Cripple_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
