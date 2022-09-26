/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "rpc.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../fixed.h"
#include "../main.h"

//RPC character structure
enum
{
	RPC_ArcMain_Idle0,
	RPC_ArcMain_Left0,
	RPC_ArcMain_Left1,
	RPC_ArcMain_Left2,
	RPC_ArcMain_Down0,
	RPC_ArcMain_Down1,
	RPC_ArcMain_Down2,
	RPC_ArcMain_Up0,
	RPC_ArcMain_Up1,
	RPC_ArcMain_Up2,
	RPC_ArcMain_Right0,
	RPC_ArcMain_Right1,
	RPC_ArcMain_Right2,

	RPC_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[RPC_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_RPC;

//RPC character definitions
static const CharFrame char_rpc_frame[] = {
	{RPC_ArcMain_Idle0, {  0,   0,  56, 168}, { 41, 83}}, //0 idle 1
	
	{RPC_ArcMain_Left0, {  0,   0, 128, 127}, {88, 80}}, //1 left 1
	{RPC_ArcMain_Left1, {  0,   0, 128, 127}, {88, 80}}, //2 left 2
	{RPC_ArcMain_Left2, {  0,   0, 128, 127}, {88, 80}}, //3 left 3
	
	{RPC_ArcMain_Down0, {  0,   0, 128,  95}, {64,  9}}, //4 down 1
	{RPC_ArcMain_Down1, {  0,   0, 128,  95}, {64,  9}}, //5 down 2
	{RPC_ArcMain_Down2, {  0,   0, 128,  95}, {64,  9}}, //6 down 3

	{RPC_ArcMain_Up0, {  0,   0,  164, 168}, {94, 110}}, //7 up 1
	{RPC_ArcMain_Up1, {  0,   0,  164, 168}, {94, 110}}, //8 up 2
	{RPC_ArcMain_Up2, {  0,   0,  164, 168}, {94, 110}}, //9 up 3
	
	{RPC_ArcMain_Right0, {  0,   0, 128, 144}, {57, 59}}, //10 right 1
	{RPC_ArcMain_Right1, {  0,   0, 128, 144}, {57, 59}}, //11 right 2
	{RPC_ArcMain_Right2, {  0,   0, 128, 144}, {57, 59}}, //12 right 3
};

static const Animation char_rpc_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, ASCR_BACK, 1}},      	   	   	   		   		   				   	   //CharAnim_Idle
	{2, (const u8[]){ 1,  2,  3, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 1,  2,  3, ASCR_BACK, 1}},       		   				    			   //CharAnim_LeftAlt
	{2, (const u8[]){ 4,  5,  6, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){ 4,  5,  6, ASCR_BACK, 1}},  		   					   				   //CharAnim_DownAlt
	{2, (const u8[]){ 7,  8,  9, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){ 7,  8,  9, ASCR_BACK, 1}},     	              			       		   //CharAnim_UpAlt
	{2, (const u8[]){10, 11, 12, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){10, 11, 12, ASCR_BACK, 1}}, 							   				   //CharAnim_RightAlt
};

//RPC character functions
void Char_RPC_SetFrame(void *user, u8 frame)
{
	Char_RPC *this = (Char_RPC*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_rpc_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_RPC_Tick(Character *character)
{
	Char_RPC *this = (Char_RPC*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_RPC_SetFrame);
	 
	if (stage.stage_id == StageId_5_1 && stage.song_step >= 1921 && stage.song_step <= 3500)
    Character_Draw(character, &this->tex, &char_rpc_frame[this->frame]);
}
	
void Char_RPC_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_RPC_Free(Character *character)
{
	Char_RPC *this = (Char_RPC*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_RPC_New(fixed_t x, fixed_t y)
{
	//Allocate rpc object
	Char_RPC *this = Mem_Alloc(sizeof(Char_RPC));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_RPC_New] Failed to allocate rpc object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_RPC_Tick;
	this->character.set_anim = Char_RPC_SetAnim;
	this->character.free = Char_RPC_Free;
	
	Animatable_Init(&this->character.animatable, char_rpc_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(40,1);
	this->character.focus_y = FIXED_DEC(-16,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\RPC.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //RPC_ArcMain_Idle0
		"left0.tim",  //RPC_ArcMain_Left0
		"left1.tim",  //RPC_ArcMain_Left1
		"left2.tim",  //RPC_ArcMain_Left2
		"down0.tim",  //RPC_ArcMain_Down0
		"down1.tim",  //RPC_ArcMain_Down1
		"down2.tim",  //RPC_ArcMain_Down2
		"up0.tim",    //RPC_ArcMain_Up0
		"up1.tim",    //RPC_ArcMain_Up1
		"up2.tim",    //RPC_ArcMain_Up2
		"right0.tim", //RPC_ArcMain_Right0
		"right1.tim", //RPC_ArcMain_Right1
		"right2.tim", //RPC_ArcMain_Right2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
