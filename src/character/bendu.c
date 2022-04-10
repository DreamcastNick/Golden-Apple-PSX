/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bendu.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Bendu character structure
enum
{
	Bendu_ArcMain_Idle0,
	Bendu_ArcMain_Idle1,
	Bendu_ArcMain_Idle2,
	Bendu_ArcMain_Idle3,
	Bendu_ArcMain_Idle4,
	Bendu_ArcMain_Left0,
	Bendu_ArcMain_Left1,
	Bendu_ArcMain_Down0,
	Bendu_ArcMain_Down1,
	Bendu_ArcMain_Up0,
	Bendu_ArcMain_Up1,
	Bendu_ArcMain_Right0,
	Bendu_ArcMain_Right1,

	Bendu_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Bendu_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Bendu;

//Bendu character definitions
static const CharFrame char_bendu_frame[] = {
	{Bendu_ArcMain_Idle0, {  0,   0, 156, 126}, { 50, 90}}, //0 idle 1
	{Bendu_ArcMain_Idle1, {  0,   0, 156, 126}, { 50, 90}}, //1 idle 2
	{Bendu_ArcMain_Idle2, {  0,   0, 156, 126}, { 50, 90}}, //2 idle 3
	{Bendu_ArcMain_Idle3, {  0,   0, 156, 126}, { 50, 90}}, //3 idle 4
	{Bendu_ArcMain_Idle4, {  0,   0, 156, 126}, { 50, 90}}, //4 idle 5
	
	{Bendu_ArcMain_Left0, {  0,   0, 116, 130}, { 64, 95}}, //5 left 1
	{Bendu_ArcMain_Left1, {  0,   0, 116, 130}, { 64, 95}}, //6 left 2
	
	{Bendu_ArcMain_Down0, {  0,   0, 116, 130}, {45, 95}}, //7 down 1
	{Bendu_ArcMain_Down1, {  0,   0, 116, 130}, {45, 95}}, //8 down 2

	{Bendu_ArcMain_Up0, {  0,   0, 116, 140}, {33, 105}}, //9 up 1
	{Bendu_ArcMain_Up1, {  0,   0, 116, 140}, {33, 105}}, //10 up 2
	
	{Bendu_ArcMain_Right0, {  0,   0, 136, 133}, {30, 96}}, //11 right 1
	{Bendu_ArcMain_Right1, {  0,   0, 136, 133}, {30, 96}}, //12 right 2
};

static const Animation char_bendu_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}},      	   	   	  			  	   //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},       		   				    			   //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},  		   					   				   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},  		   					   				   //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},  		   					   				   //CharAnim_RightAlt
};

//Bendu character functions
void Char_Bendu_SetFrame(void *user, u8 frame)
{
	Char_Bendu *this = (Char_Bendu*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bendu_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Bendu_Tick(Character *character)
{
	Char_Bendu *this = (Char_Bendu*)character;

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Bendu_SetFrame);

    Character_Draw(character, &this->tex, &char_bendu_frame[this->frame]);
}
	
void Char_Bendu_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Bendu_Free(Character *character)
{
	Char_Bendu *this = (Char_Bendu*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Bendu_New(fixed_t x, fixed_t y)
{
	//Allocate bendu object
	Char_Bendu *this = Mem_Alloc(sizeof(Char_Bendu));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Bendu_New] Failed to allocate bendu object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Bendu_Tick;
	this->character.set_anim = Char_Bendu_SetAnim;
	this->character.free = Char_Bendu_Free;
	
	Animatable_Init(&this->character.animatable, char_bendu_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 15;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-36,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BENDU.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Bendu_ArcMain_Idle0
		"idle1.tim",  //Bendu_ArcMain_Idle1
		"idle2.tim",  //Bendu_ArcMain_Idle2
		"idle3.tim",  //Bendu_ArcMain_Idle3
		"idle4.tim",  //Bendu_ArcMain_Idle4
		"left0.tim",  //Bendu_ArcMain_Left0
		"left1.tim",  //Bendu_ArcMain_Left1
		"down0.tim",  //Bendu_ArcMain_Down0
		"down1.tim",  //Bendu_ArcMain_Down1
		"up0.tim",    //Bendu_ArcMain_Up0
		"up1.tim",    //Bendu_ArcMain_Up1
		"right0.tim", //Bendu_ArcMain_Right0
		"right1.tim", //Bendu_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
