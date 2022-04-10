/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bambom.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BamBom character structure
enum
{
	BamBom_ArcMain_Idle0,
	BamBom_ArcMain_Idle1,
	BamBom_ArcMain_Idle2,
	BamBom_ArcMain_Idle3,
	BamBom_ArcMain_Idle4,
	BamBom_ArcMain_Idle5,
	BamBom_ArcMain_Left0,
	BamBom_ArcMain_Left1,
	BamBom_ArcMain_Down0,
	BamBom_ArcMain_Down1,
	BamBom_ArcMain_Up0,
	BamBom_ArcMain_Up1,
	BamBom_ArcMain_Right0,
	BamBom_ArcMain_Right1,

	BamBom_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BamBom_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BamBom;

//BamBom character definitions
static const CharFrame char_bambom_frame[] = {
	{BamBom_ArcMain_Idle0, {  0,   0, 116, 126}, { 50, 90}}, //0 idle 1
	{BamBom_ArcMain_Idle1, {  0,   0, 116, 126}, { 50, 90}}, //1 idle 2
	{BamBom_ArcMain_Idle2, {  0,   0, 116, 126}, { 50, 90}}, //2 idle 3
	{BamBom_ArcMain_Idle3, {  0,   0, 116, 126}, { 50, 90}}, //3 idle 4
	{BamBom_ArcMain_Idle4, {  0,   0, 116, 126}, { 50, 90}}, //4 idle 5
	{BamBom_ArcMain_Idle5, {  0,   0, 116, 126}, { 50, 90}}, //5 idle 6
	
	{BamBom_ArcMain_Left0, {  0,   0, 124, 131}, { 70, 95}}, //6 left 1
	{BamBom_ArcMain_Left1, {  0,   0, 124, 131}, { 70, 95}}, //7 left 2
	
	{BamBom_ArcMain_Down0, {  0,   0, 124, 109}, {40, 75}}, //8 down 1
	{BamBom_ArcMain_Down1, {  0,   0, 124, 109}, {40, 75}}, //9 down 2

	{BamBom_ArcMain_Up0, {  0,   0,  96, 135}, {48, 99}}, //10 up 1
	{BamBom_ArcMain_Up1, {  0,   0,  96, 135}, {48, 99}}, //11 up 2
	
	{BamBom_ArcMain_Right0, {  0,   0, 124, 113}, {51, 74}}, //12 right 1
	{BamBom_ArcMain_Right1, {  0,   0, 124, 113}, {51, 74}}, //13 right 2
};

static const Animation char_bambom_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5, ASCR_BACK, 1}},      	   	   	  			   //CharAnim_Idle
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},       		   				    			   //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},  		   					   				   //CharAnim_DownAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},  		   					   				   //CharAnim_UpAlt
	{2, (const u8[]){12, 13, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){12, 13, ASCR_BACK, 1}},  		   					   				   //CharAnim_RightAlt
};

//BamBom character functions
void Char_BamBom_SetFrame(void *user, u8 frame)
{
	Char_BamBom *this = (Char_BamBom*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bambom_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BamBom_Tick(Character *character)
{
	Char_BamBom *this = (Char_BamBom*)character;

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_BamBom_SetFrame);

    Character_Draw(character, &this->tex, &char_bambom_frame[this->frame]);
}
	
void Char_BamBom_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BamBom_Free(Character *character)
{
	Char_BamBom *this = (Char_BamBom*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BamBom_New(fixed_t x, fixed_t y)
{
	//Allocate bambom object
	Char_BamBom *this = Mem_Alloc(sizeof(Char_BamBom));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BamBom_New] Failed to allocate bambom object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BamBom_Tick;
	this->character.set_anim = Char_BamBom_SetAnim;
	this->character.free = Char_BamBom_Free;
	
	Animatable_Init(&this->character.animatable, char_bambom_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 14;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-36,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BAMBOM.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //BamBom_ArcMain_Idle0
		"idle1.tim",  //BamBom_ArcMain_Idle1
		"idle2.tim",  //BamBom_ArcMain_Idle2
		"idle3.tim",  //BamBom_ArcMain_Idle3
		"idle4.tim",  //BamBom_ArcMain_Idle4
		"idle5.tim",  //BamBom_ArcMain_Idle5
		"left0.tim",  //BamBom_ArcMain_Left0
		"left1.tim",  //BamBom_ArcMain_Left1
		"down0.tim",  //BamBom_ArcMain_Down0
		"down1.tim",  //BamBom_ArcMain_Down1
		"up0.tim",    //BamBom_ArcMain_Up0
		"up1.tim",    //BamBom_ArcMain_Up1
		"right0.tim", //BamBom_ArcMain_Right0
		"right1.tim", //BamBom_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
