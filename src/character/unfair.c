/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "unfair.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../fixed.h"
#include "../main.h"

//Unfair character structure
enum
{
	Unfair_ArcMain_Idle0,
	Unfair_ArcMain_Idle1,
	Unfair_ArcMain_Idle2,
	Unfair_ArcMain_Idle3,
	Unfair_ArcMain_Left0,
	Unfair_ArcMain_Left1,
	Unfair_ArcMain_Down0,
	Unfair_ArcMain_Down1,
	Unfair_ArcMain_Up0,
	Unfair_ArcMain_Up1,
	Unfair_ArcMain_Right0,
	Unfair_ArcMain_Right1,
	Unfair_ArcMain_Eat0,
	Unfair_ArcMain_Eat1,
	Unfair_ArcMain_Eat2,

	Unfair_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Unfair_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Unfair;

//Unfair character definitions
static const CharFrame char_unfair_frame[] = {
	{Unfair_ArcMain_Idle0, {  0,   0, 200, 256}, { 51, 90}}, //0 idle 1
	{Unfair_ArcMain_Idle1, {  0,   0, 200, 256}, { 51, 90}}, //1 idle 2
	{Unfair_ArcMain_Idle2, {  0,   0, 200, 256}, { 51, 90}}, //2 idle 3
	{Unfair_ArcMain_Idle3, {  0,   0, 200, 256}, { 51, 90}}, //3 idle 4
	
	{Unfair_ArcMain_Left0, {  0,   0, 168, 177}, { 51, -13}}, //4 left 1
	{Unfair_ArcMain_Left1, {  0,   0, 168, 177}, { 51, -13}}, //5 left 2
	
	{Unfair_ArcMain_Down0, {  0,   0, 116, 176}, {-10, -20}}, //6 down 1
	{Unfair_ArcMain_Down1, {  0,   0, 116, 176}, {-10, -20}}, //7 down 2

	{Unfair_ArcMain_Up0, {  0,   0,  128, 179}, {0, 60}}, //8 up 1
	{Unfair_ArcMain_Up1, {  0,   0,  128, 179}, {0, 60}}, //9 up 2
	
	{Unfair_ArcMain_Right0, {  0,   0, 120, 176}, {-11, 10}}, //10 right 1
	{Unfair_ArcMain_Right1, {  0,   0, 120, 176}, {-11, 10}}, //11 right 2
	
	{Unfair_ArcMain_Eat0, {  0,   0, 236, 241}, {120, 120}},  //12 eat 0
	{Unfair_ArcMain_Eat1, {  0,   0, 236, 241}, {120, 120}},  //13 eat 1
	{Unfair_ArcMain_Eat2, {  0,   0, 236, 241}, {120, 120}},  //14 eat 2
};

static const Animation char_unfair_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  3,  3,  3, ASCR_BACK, 1}},      	   	   	   		   //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 4,  5, ASCR_CHGANI, CharAnim_IdleAlt}},      		   				   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){ 6,  7, ASCR_CHGANI, CharAnim_IdleAlt}},  	          				   //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){ 8,  9, ASCR_CHGANI, CharAnim_IdleAlt}},              			       //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){10, 11, ASCR_CHGANI, CharAnim_IdleAlt}},              				   //CharAnim_RightAlt
	{2, (const u8[]){ 0,  1,  2,  3,  3,  3,  3, ASCR_CHGANI, CharAnim_IdleAlt}},      	   //CharAnim_IdleAlt
	{2, (const u8[]){12, 13, 14, 12, 13, 14, 12, 13, 14, 12, 13, 14, ASCR_BACK, 1}},       //CharAnim_Huh
};

void Unfair_Draw(Character *this, Gfx_Tex *tex, const CharFrame *cframe)
{
	//Draw character
	fixed_t x = this->x - stage.camera.x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t y = this->y - stage.camera.y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {x, y, src.w*2 << FIXED_SHIFT, src.h*2 << FIXED_SHIFT};
	Stage_DrawTex(tex, &src, &dst, stage.camera.bzoom);
}

//Unfair character functions
void Char_Unfair_SetFrame(void *user, u8 frame)
{
	Char_Unfair *this = (Char_Unfair*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_unfair_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Unfair_Tick(Character *character)
{
	Char_Unfair *this = (Char_Unfair*)character;
	
	if (stage.stage_id == StageId_1_2 && stage.timercount >= 4565)
	this->character.healthb_i = 10;

	if (stage.stage_id == StageId_1_2 && stage.timercount >= 11210)
	this->character.healthb_i = 11;

	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Unfair_SetFrame);
	 
	if (stage.stage_id == StageId_1_2 && stage.timercount >= 11210)
    Unfair_Draw(character, &this->tex, &char_unfair_frame[this->frame]);
	
	//Stage specific animations
	if (stage.note_scroll >= 0)
	{
		switch (stage.stage_id)
		{
			case StageId_1_2: //Applecore
				if ((stage.timercount) == 11520)
					character->set_anim(character, CharAnim_Huh);
				break;
			default:
				break;
		}
	}
}
	
void Char_Unfair_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Unfair_Free(Character *character)
{
	Char_Unfair *this = (Char_Unfair*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Unfair_New(fixed_t x, fixed_t y)
{
	//Allocate unfair object
	Char_Unfair *this = Mem_Alloc(sizeof(Char_Unfair));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Unfair_New] Failed to allocate unfair object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Unfair_Tick;
	this->character.set_anim = Char_Unfair_SetAnim;
	this->character.free = Char_Unfair_Free;
	
	Animatable_Init(&this->character.animatable, char_unfair_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.healthb_i = 9;
	
	this->character.focus_x = FIXED_DEC(50,1);
	this->character.focus_y = FIXED_DEC(-25,1);
	this->character.focus_zoom = FIXED_DEC(07,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\UNFAIR.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Unfair_ArcMain_Idle0
		"idle1.tim",  //Unfair_ArcMain_Idle1
		"idle2.tim",  //Unfair_ArcMain_Idle2
		"idle3.tim",  //Unfair_ArcMain_Idle3
		"left0.tim",  //Unfair_ArcMain_Left0
		"left1.tim",  //Unfair_ArcMain_Left1
		"down0.tim",  //Unfair_ArcMain_Down0
		"down1.tim",  //Unfair_ArcMain_Down1
		"up0.tim",    //Unfair_ArcMain_Up0
		"up1.tim",    //Unfair_ArcMain_Up1
		"right0.tim", //Unfair_ArcMain_Right0
		"right1.tim", //Unfair_ArcMain_Right1
		"eat0.tim",    //Unfair_ArcMain_Eat0
		"eat1.tim",    //Unfair_ArcMain_Eat1
		"eat2.tim",    //Unfair_ArcMain_Eat2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
