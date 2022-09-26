/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "rpa.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../fixed.h"
#include "../main.h"

//RPA character structure
enum
{
	RPA_ArcMain_Idle0,
	RPA_ArcMain_Left0,
	RPA_ArcMain_Left1,
	RPA_ArcMain_Down0,
	RPA_ArcMain_Down1,
	RPA_ArcMain_Up0,
	RPA_ArcMain_Up1,
	RPA_ArcMain_Right0,
	RPA_ArcMain_Right1,

	RPA_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[RPA_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_RPA;

//RPA character definitions
static const CharFrame char_rpa_frame[] = {
	{RPA_ArcMain_Idle0, {  0,   0,  56, 165}, { 41, 83}}, //0 idle 1
	
	{RPA_ArcMain_Left0, {  0,   0,  88, 165}, {72, 84}}, //1 left 1
	{RPA_ArcMain_Left1, {  0,   0,  88, 165}, {72, 84}}, //2 left 2
	
	{RPA_ArcMain_Down0, {  0,   0,  56, 166}, {41, 85}}, //3 down 1
	{RPA_ArcMain_Down1, {  0,   0,  56, 166}, {41, 85}}, //4 down 2

	{RPA_ArcMain_Up0, {  0,   0,  88, 166}, {47, 85}}, //5 up 1
	{RPA_ArcMain_Up1, {  0,   0,  88, 166}, {47, 85}}, //6 up 2
	
	{RPA_ArcMain_Right0, {  0,   0,  96, 166}, {39, 84}}, //7 right 1
	{RPA_ArcMain_Right1, {  0,   0,  96, 166}, {39, 84}}, //8 right 2
};

static const Animation char_rpa_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,	 ASCR_BACK, 0}},      	   	   	   		   		  			   //CharAnim_Idle
	{2, (const u8[]){ 1,  2, ASCR_BACK, 0}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 1,  2, ASCR_BACK, 0}},       		   				    			   //CharAnim_LeftAlt
	{2, (const u8[]){ 3,  4, ASCR_BACK, 0}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){ 3,  4, ASCR_BACK, 0}},  		   					   				   //CharAnim_DownAlt
	{2, (const u8[]){ 5,  6, ASCR_BACK, 0}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){ 5,  6, ASCR_BACK, 0}},     	              			       		   //CharAnim_UpAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 0}}, 							   				   //CharAnim_Right
	{2, (const u8[]){ 7,  8, ASCR_BACK, 0}}, 							   				   //CharAnim_RightAlt
};

//RPA character functions
void Char_RPA_SetFrame(void *user, u8 frame)
{
	Char_RPA *this = (Char_RPA*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_rpa_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_RPA_Tick(Character *character)
{
	Char_RPA *this = (Char_RPA*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_RPA_SetFrame);
	
	if (stage.stage_id == StageId_5_1 && stage.song_step <= 1022)
    Character_Draw(character, &this->tex, &char_rpa_frame[this->frame]);
}
	
void Char_RPA_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_RPA_Free(Character *character)
{
	Char_RPA *this = (Char_RPA*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_RPA_New(fixed_t x, fixed_t y)
{
	//Allocate rpa object
	Char_RPA *this = Mem_Alloc(sizeof(Char_RPA));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_RPA_New] Failed to allocate rpa object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_RPA_Tick;
	this->character.set_anim = Char_RPA_SetAnim;
	this->character.free = Char_RPA_Free;
	
	Animatable_Init(&this->character.animatable, char_rpa_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(40,1);
	this->character.focus_y = FIXED_DEC(-16,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\RPA.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //RPA_ArcMain_Idle0
		"left0.tim",  //RPA_ArcMain_Left0
		"left1.tim",  //RPA_ArcMain_Left1
		"down0.tim",  //RPA_ArcMain_Down0
		"down1.tim",  //RPA_ArcMain_Down1
		"up0.tim",    //RPA_ArcMain_Up0
		"up1.tim",    //RPA_ArcMain_Up1
		"right0.tim", //RPA_ArcMain_Right0
		"right1.tim", //RPA_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}