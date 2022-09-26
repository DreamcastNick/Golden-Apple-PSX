/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "rpb.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../fixed.h"
#include "../main.h"

//RPB character structure
enum
{
	RPB_ArcMain_Idle0,
	RPB_ArcMain_Idle1,
	RPB_ArcMain_Idle2,
	RPB_ArcMain_Idle3,
	RPB_ArcMain_Idle4,
	RPB_ArcMain_Idle5,
	RPB_ArcMain_Idle6,
	RPB_ArcMain_Idle7,
	RPB_ArcMain_Idle8,
	RPB_ArcMain_Idle9,
	RPB_ArcMain_Idle10,
	RPB_ArcMain_Idle11,
	RPB_ArcMain_Left0,
	RPB_ArcMain_Left1,
	RPB_ArcMain_Down0,
	RPB_ArcMain_Down1,
	RPB_ArcMain_Up0,
	RPB_ArcMain_Up1,
	RPB_ArcMain_Right0,
	RPB_ArcMain_Right1,

	RPB_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[RPB_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_RPB;

//RPB character definitions
static const CharFrame char_rpb_frame[] = {
	{RPB_ArcMain_Idle0,  {  0,   0,  72, 222}, { 41, 83}}, //0 idle 1
	{RPB_ArcMain_Idle1,  {  0,   0,  72, 222}, { 41, 83}}, //1 idle 2
	{RPB_ArcMain_Idle2,  {  0,   0,  72, 222}, { 41, 83}}, //2 idle 3
	{RPB_ArcMain_Idle3,  {  0,   0,  72, 222}, { 41, 83}}, //3 idle 4
	{RPB_ArcMain_Idle4,  {  0,   0,  72, 222}, { 41, 83}}, //4 idle 5
	{RPB_ArcMain_Idle5,  {  0,   0,  72, 222}, { 41, 83}}, //5 idle 6
	{RPB_ArcMain_Idle6,  {  0,   0,  72, 222}, { 41, 83}}, //6 idle 7
	{RPB_ArcMain_Idle7,  {  0,   0,  72, 222}, { 41, 83}}, //7 idle 8
	{RPB_ArcMain_Idle8,  {  0,   0,  72, 222}, { 41, 83}}, //8 idle 9
	{RPB_ArcMain_Idle9,  {  0,   0,  72, 222}, { 41, 83}}, //9 idle 10
	{RPB_ArcMain_Idle10, {  0,   0,  72, 222}, { 41, 83}}, //10 idle 11
	{RPB_ArcMain_Idle11, {  0,   0,  72, 222}, { 41, 83}}, //11 idle 12
	
	{RPB_ArcMain_Left0, {  0,   0, 128, 214}, {123, 76}}, //12 left 1
	{RPB_ArcMain_Left1, {  0,   0, 128, 214}, {123, 76}}, //13 left 2
	
	{RPB_ArcMain_Down0, {  0,   0, 148, 125}, {76, -15}}, //14 down 1
	{RPB_ArcMain_Down1, {  0,   0, 148, 125}, {76, -15}}, //15 down 2

	{RPB_ArcMain_Up0, {  0,   0,  72, 195}, {41, 93}}, //16 up 1
	{RPB_ArcMain_Up1, {  0,   0,  72, 195}, {41, 93}}, //17 up 2
	
	{RPB_ArcMain_Right0, {  0,   0, 116, 205}, {15, 64}}, //18 right 1
	{RPB_ArcMain_Right1, {  0,   0, 116, 205}, {15, 64}}, //19 right 2
};

static const Animation char_rpb_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, ASCR_BACK, 0}},       //CharAnim_Idle
	{2, (const u8[]){12, 13, ASCR_BACK, 0}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){12, 13, ASCR_BACK, 0}},       		   				    			   //CharAnim_LeftAlt
	{2, (const u8[]){14, 15, ASCR_BACK, 0}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){14, 15, ASCR_BACK, 0}},  		   					   				   //CharAnim_DownAlt
	{2, (const u8[]){16, 17, ASCR_BACK, 0}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){16, 17, ASCR_BACK, 0}},     	              			       		   //CharAnim_UpAlt
	{2, (const u8[]){18, 19, ASCR_BACK, 0}}, 							   				   //CharAnim_Right
	{2, (const u8[]){18, 19, ASCR_BACK, 0}}, 							   				   //CharAnim_RightAlt
};

//RPB character functions
void Char_RPB_SetFrame(void *user, u8 frame)
{
	Char_RPB *this = (Char_RPB*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_rpb_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_RPB_Tick(Character *character)
{
	Char_RPB *this = (Char_RPB*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_RPB_SetFrame);
	
	if (stage.stage_id == StageId_5_1 && stage.song_step >= 1023 && stage.song_step <= 1920)
    Character_Draw(character, &this->tex, &char_rpb_frame[this->frame]);
}
	
void Char_RPB_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_RPB_Free(Character *character)
{
	Char_RPB *this = (Char_RPB*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_RPB_New(fixed_t x, fixed_t y)
{
	//Allocate rpb object
	Char_RPB *this = Mem_Alloc(sizeof(Char_RPB));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_RPB_New] Failed to allocate rpb object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_RPB_Tick;
	this->character.set_anim = Char_RPB_SetAnim;
	this->character.free = Char_RPB_Free;
	
	Animatable_Init(&this->character.animatable, char_rpb_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(40,1);
	this->character.focus_y = FIXED_DEC(-16,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\RPB.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //RPB_ArcMain_Idle0
		"idle1.tim",  //RPB_ArcMain_Idle1
		"idle2.tim",  //RPB_ArcMain_Idle2
		"idle3.tim",  //RPB_ArcMain_Idle3
		"idle4.tim",  //RPB_ArcMain_Idle4
		"idle5.tim",  //RPB_ArcMain_Idle5
		"idle6.tim",  //RPB_ArcMain_Idle6
		"idle7.tim",  //RPB_ArcMain_Idle7
		"idle8.tim",  //RPB_ArcMain_Idle8
		"idle9.tim",  //RPB_ArcMain_Idle9
		"idle10.tim", //RPB_ArcMain_Idle10
		"idle11.tim", //RPB_ArcMain_Idle11
		"left0.tim",  //RPB_ArcMain_Left0
		"left1.tim",  //RPB_ArcMain_Left1
		"down0.tim",  //RPB_ArcMain_Down0
		"down1.tim",  //RPB_ArcMain_Down1
		"up0.tim",    //RPB_ArcMain_Up0
		"up1.tim",    //RPB_ArcMain_Up1
		"right0.tim", //RPB_ArcMain_Right0
		"right1.tim", //RPB_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
