/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cycles.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../fixed.h"
#include "../main.h"

//Cycles character structure
enum
{
	Cycles_ArcMain_Idle0,
	Cycles_ArcMain_Idle1,
	Cycles_ArcMain_Idle2,
	Cycles_ArcMain_Idle3,
	Cycles_ArcMain_Idle4,
	Cycles_ArcMain_Left0,
	Cycles_ArcMain_Left1,
	Cycles_ArcMain_Down0,
	Cycles_ArcMain_Down1,
	Cycles_ArcMain_Up0,
	Cycles_ArcMain_Up1,
	Cycles_ArcMain_Right0,
	Cycles_ArcMain_Right1,

	Cycles_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Cycles_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Cycles;

//Cycles character definitions
static const CharFrame char_cycles_frame[] = {
	{Cycles_ArcMain_Idle0, {  0,   0, 154, 145}, { 45, 83}}, //0 idle 1
	{Cycles_ArcMain_Idle1, {  0,   0, 154, 145}, { 45, 83}}, //1 idle 2
	{Cycles_ArcMain_Idle2, {  0,   0, 154, 145}, { 45, 83}}, //2 idle 3
	{Cycles_ArcMain_Idle3, {  0,   0, 154, 145}, { 45, 83}}, //3 idle 4
	{Cycles_ArcMain_Idle3, {  0,   0, 154, 145}, { 45, 83}}, //4 idle 5
	
	{Cycles_ArcMain_Left0, {  0,   0, 256, 158}, {176, 100}}, //5 left 1
	{Cycles_ArcMain_Left1, {  0,   0, 256, 158}, {176, 100}}, //6 left 2
	
	{Cycles_ArcMain_Down0, {  0,   0, 164, 148}, {51, 87}}, //7 down 1
	{Cycles_ArcMain_Down1, {  0,   0, 164, 148}, {51, 87}}, //8 down 2

	{Cycles_ArcMain_Up0, {  0,   0,  148, 236}, {35, 180}}, //9 up 1
	{Cycles_ArcMain_Up1, {  0,   0,  148, 236}, {35, 180}}, //10 up 2
	
	{Cycles_ArcMain_Right0, {  0,   0, 148, 154}, {25, 93}}, //11 right 1
	{Cycles_ArcMain_Right1, {  0,   0, 148, 154}, {25, 93}}, //12 right 2
};

static const Animation char_cycles_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}},      	   	   	   		   		   //CharAnim_Idle
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},      		   				   		   //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},  	          				   		   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},              			       		   //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},              				           //CharAnim_RightAlt
};

void Cycles_Draw(Character *this, Gfx_Tex *tex, const CharFrame *cframe)
{
	//Draw character
	fixed_t x = this->x - stage.camera.x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t y = this->y - stage.camera.y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {x, y, (src.w + 50) << FIXED_SHIFT, (src.h + 50) << FIXED_SHIFT};
	Stage_DrawTex(tex, &src, &dst, stage.camera.bzoom);
}

//Cycles character functions
void Char_Cycles_SetFrame(void *user, u8 frame)
{
	Char_Cycles *this = (Char_Cycles*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_cycles_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Cycles_Tick(Character *character)
{
	Char_Cycles *this = (Char_Cycles*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Cycles_SetFrame);
    Cycles_Draw(character, &this->tex, &char_cycles_frame[this->frame]);
}
	
void Char_Cycles_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Cycles_Free(Character *character)
{
	Char_Cycles *this = (Char_Cycles*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Cycles_New(fixed_t x, fixed_t y)
{
	//Allocate cycles object
	Char_Cycles *this = Mem_Alloc(sizeof(Char_Cycles));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Cycles_New] Failed to allocate cycles object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Cycles_Tick;
	this->character.set_anim = Char_Cycles_SetAnim;
	this->character.free = Char_Cycles_Free;
	
	Animatable_Init(&this->character.animatable, char_cycles_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 20;
	
	this->character.focus_x = FIXED_DEC(-10,1);
	this->character.focus_y = FIXED_DEC(-19,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\CYCLES.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Cycles_ArcMain_Idle0
		"idle1.tim",  //Cycles_ArcMain_Idle1
		"idle2.tim",  //Cycles_ArcMain_Idle2
		"idle3.tim",  //Cycles_ArcMain_Idle3
		"idle4.tim",  //Cycles_ArcMain_Idle4
		"left0.tim",  //Cycles_ArcMain_Left0
		"left1.tim",  //Cycles_ArcMain_Left1
		"down0.tim",  //Cycles_ArcMain_Down0
		"down1.tim",  //Cycles_ArcMain_Down1
		"up0.tim",    //Cycles_ArcMain_Up0
		"up1.tim",    //Cycles_ArcMain_Up1
		"right0.tim", //Cycles_ArcMain_Right0
		"right1.tim", //Cycles_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
