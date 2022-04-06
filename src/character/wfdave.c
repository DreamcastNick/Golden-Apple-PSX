/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "wfdave.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//WFDave character structure
enum
{
	WFDave_ArcMain_Idle0,
	WFDave_ArcMain_Idle1,
	WFDave_ArcMain_Idle2,
	WFDave_ArcMain_Idle3,
	WFDave_ArcMain_Idle4,
	WFDave_ArcMain_Left0,
	WFDave_ArcMain_Left1,
	WFDave_ArcMain_Down0,
	WFDave_ArcMain_Down1,
	WFDave_ArcMain_Up0,
	WFDave_ArcMain_Up1,
	WFDave_ArcMain_Right0,
	WFDave_ArcMain_Right1,

	WFDave_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[WFDave_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_WFDave;

//WFDave character definitions
static const CharFrame char_wfdave_frame[] = {
	{WFDave_ArcMain_Idle0, {  0,   0, 174, 191}, { 60, 90}}, //0 idle 1
	{WFDave_ArcMain_Idle1, {  0,   0, 174, 191}, { 60, 90}}, //1 idle 2
	{WFDave_ArcMain_Idle2, {  0,   0, 174, 191}, { 60, 90}}, //2 idle 3
	{WFDave_ArcMain_Idle3, {  0,   0, 174, 191}, { 60, 90}}, //3 idle 4
	{WFDave_ArcMain_Idle4, {  0,   0, 174, 191}, { 60, 90}}, //4 idle 5
	
	{WFDave_ArcMain_Left0, {  0,   0, 194, 156}, { 62, 47}}, //5 left 1
	{WFDave_ArcMain_Left1, {  0,   0, 194, 156}, { 62, 47}}, //6 left 2
	
	{WFDave_ArcMain_Down0, {  0,   0, 196, 183}, {57, 86}}, //7 down 1
	{WFDave_ArcMain_Down1, {  0,   0, 196, 183}, {57, 86}}, //8 down 2

	{WFDave_ArcMain_Up0, {  0,   0,  164, 188}, {34, 84}}, //9 up 1
	{WFDave_ArcMain_Up1, {  0,   0,  164, 188}, {34, 84}}, //10 up 2
	
	{WFDave_ArcMain_Right0, {  0,   0, 222, 165}, {53, 66}}, //11 right 1
	{WFDave_ArcMain_Right1, {  0,   0, 222, 165}, {53, 66}}, //12 right 2
};

static const Animation char_wfdave_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}},      	   	   	  				   //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},      		   				   				   //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},  	          				   				   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},              			        			   //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},              				     			   //CharAnim_RightAlt
};

//WFDave character functions
void Char_WFDave_SetFrame(void *user, u8 frame)
{
	Char_WFDave *this = (Char_WFDave*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_wfdave_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_WFDave_Tick(Character *character)
{
	Char_WFDave *this = (Char_WFDave*)character;
	
	if (stage.stage_id == StageId_1_4 && stage.timercount >= 4700)
	this->character.health_i = 12;

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_WFDave_SetFrame);

	if (stage.stage_id == StageId_1_4 && stage.timercount <= 4700)
    Character_Draw(character, &this->tex, &char_wfdave_frame[this->frame]);
}
	
void Char_WFDave_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_WFDave_Free(Character *character)
{
	Char_WFDave *this = (Char_WFDave*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_WFDave_New(fixed_t x, fixed_t y)
{
	//Allocate wfdave object
	Char_WFDave *this = Mem_Alloc(sizeof(Char_WFDave));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_WFDave_New] Failed to allocate wfdave object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_WFDave_Tick;
	this->character.set_anim = Char_WFDave_SetAnim;
	this->character.free = Char_WFDave_Free;
	
	Animatable_Init(&this->character.animatable, char_wfdave_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 2;
	
	this->character.focus_x = FIXED_DEC(50,1);
	this->character.focus_y = FIXED_DEC(25,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\WFDAVE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //WFDave_ArcMain_Idle0
		"idle1.tim",  //WFDave_ArcMain_Idle1
		"idle2.tim",  //WFDave_ArcMain_Idle2
		"idle3.tim",  //WFDave_ArcMain_Idle3
		"idle4.tim",  //WFDave_ArcMain_Idle4
		"left0.tim",  //WFDave_ArcMain_Left0
		"left1.tim",  //WFDave_ArcMain_Left1
		"down0.tim",  //WFDave_ArcMain_Down0
		"down1.tim",  //WFDave_ArcMain_Down1
		"up0.tim",    //WFDave_ArcMain_Up0
		"up1.tim",    //WFDave_ArcMain_Up1
		"right0.tim", //WFDave_ArcMain_Right0
		"right1.tim", //WFDave_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
