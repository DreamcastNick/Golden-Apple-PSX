/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bambi.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../fixed.h"
#include "../main.h"

//Bambi character structure
enum
{
	Bambi_ArcMain_Idle0,
	Bambi_ArcMain_Idle1,
	Bambi_ArcMain_Left,
	Bambi_ArcMain_Down,
	Bambi_ArcMain_Up,
	Bambi_ArcMain_Right,

	Bambi_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Bambi_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Bambi;

//Bambi character definitions
static const CharFrame char_bambi_frame[] = {
	{Bambi_ArcMain_Idle0, {  4,   2,  88,  73}, { 53, 92}}, //0 idle 1
	{Bambi_ArcMain_Idle0, { 96,   2,  88,  73}, { 53, 92}}, //1 idle 2
	{Bambi_ArcMain_Idle0, {  4,  78,  88,  73}, { 53, 92}}, //2 idle 3
	{Bambi_ArcMain_Idle0, { 96,  78,  88,  73}, { 53, 92}}, //3 idle 4
	{Bambi_ArcMain_Idle1, {  0,   0,  88,  73}, { 53, 92}}, //4 idle 5
	
	{Bambi_ArcMain_Left, {  5,   2,  71,  72}, {51, 90}}, //5 left 1
	{Bambi_ArcMain_Left, { 80,   2,  71,  72}, {51, 90}}, //6 left 2
	
	{Bambi_ArcMain_Down, {  3,   2,  63,  62}, {48, 81}}, //7 down 1
	{Bambi_ArcMain_Down, { 70,   2,  63,  62}, {48, 81}}, //8 down 2

	{Bambi_ArcMain_Up, {  5,   2,  83,  75}, {64, 92}}, //9 up 1
	{Bambi_ArcMain_Up, { 92,   2,  83,  75}, {64, 92}}, //10 up 2
	
	{Bambi_ArcMain_Right, {  4,   2,  80,  72}, {43, 91}}, //11 right 1
	{Bambi_ArcMain_Right, { 88,   2,  80,  72}, {43, 91}}, //12 right 2
};

static const Animation char_bambi_anim[CharAnim_Max] = {
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

//Bambi character functions
void Char_Bambi_SetFrame(void *user, u8 frame)
{
	Char_Bambi *this = (Char_Bambi*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bambi_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Bambi_Tick(Character *character)
{
	Char_Bambi *this = (Char_Bambi*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Bambi_SetFrame);
    Character_Draw(character, &this->tex, &char_bambi_frame[this->frame]);
}
	
void Char_Bambi_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Bambi_Free(Character *character)
{
	Char_Bambi *this = (Char_Bambi*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Bambi_New(fixed_t x, fixed_t y)
{
	//Allocate bambi object
	Char_Bambi *this = Mem_Alloc(sizeof(Char_Bambi));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Bambi_New] Failed to allocate bambi object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Bambi_Tick;
	this->character.set_anim = Char_Bambi_SetAnim;
	this->character.free = Char_Bambi_Free;
	
	Animatable_Init(&this->character.animatable, char_bambi_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 7;
	
	this->character.focus_x = FIXED_DEC(50,1);
	this->character.focus_y = FIXED_DEC(-80,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BAMBI.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Bambi_ArcMain_Idle0
		"idle1.tim",  //Bambi_ArcMain_Idle1
		"left.tim",  //Bambi_ArcMain_Left
		"down.tim",  //Bambi_ArcMain_Down
		"up.tim",    //Bambi_ArcMain_Up
		"right.tim", //Bambi_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
