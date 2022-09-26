/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "sart.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../fixed.h"
#include "../main.h"

//Sart character structure
enum
{
	Sart_ArcMain_Idle0,
	Sart_ArcMain_Idle1,
	Sart_ArcMain_Idle2,
	Sart_ArcMain_Idle3,
	Sart_ArcMain_Idle4,
	Sart_ArcMain_Left0,
	Sart_ArcMain_Left1,
	Sart_ArcMain_Left2,
	Sart_ArcMain_Left3,
	Sart_ArcMain_Down0,
	Sart_ArcMain_Down1,
	Sart_ArcMain_Down2,
	Sart_ArcMain_Down3,
	Sart_ArcMain_Up0,
	Sart_ArcMain_Up1,
	Sart_ArcMain_Up2,
	Sart_ArcMain_Up3,
	Sart_ArcMain_Right0,
	Sart_ArcMain_Right1,
	Sart_ArcMain_Right2,
	Sart_ArcMain_Right3,

	Sart_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Sart_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Sart;

//Sart character definitions
static const CharFrame char_sart_frame[] = {
	{Sart_ArcMain_Idle0, {  0,   0, 120, 120}, { 45, 83}}, //0 idle 1
	{Sart_ArcMain_Idle1, {  0,   0, 120, 120}, { 45, 83}}, //1 idle 2
	{Sart_ArcMain_Idle2, {  0,   0, 120, 120}, { 45, 83}}, //2 idle 3
	{Sart_ArcMain_Idle3, {  0,   0, 120, 120}, { 45, 83}}, //3 idle 4
	{Sart_ArcMain_Idle4, {  0,   0, 120, 120}, { 45, 83}}, //4 idle 5
	
	{Sart_ArcMain_Left0, {  0,   0, 108, 116}, {41, 72}}, //5 left 1
	{Sart_ArcMain_Left1, {  0,   0, 108, 116}, {41, 72}}, //6 left 2
	{Sart_ArcMain_Left2, {  0,   0, 108, 116}, {41, 72}}, //7 left 3
	{Sart_ArcMain_Left3, {  0,   0, 108, 116}, {41, 72}}, //8 left 4
	
	{Sart_ArcMain_Down0, {  0,   0, 120, 116}, {44, 76}}, //9  down 1
	{Sart_ArcMain_Down1, {  0,   0, 120, 116}, {44, 76}}, //10 down 2
	{Sart_ArcMain_Down2, {  0,   0, 120, 116}, {44, 76}}, //11 down 3
	{Sart_ArcMain_Down3, {  0,   0, 120, 116}, {44, 76}}, //12 down 4

	{Sart_ArcMain_Up0, {  0,   0,  124, 168}, {51, 180}}, //13 up 1
	{Sart_ArcMain_Up1, {  0,   0,  124, 168}, {51, 180}}, //14 up 2
	{Sart_ArcMain_Up2, {  0,   0,  124, 168}, {51, 180}}, //15 up 3
	{Sart_ArcMain_Up3, {  0,   0,  124, 168}, {51, 180}}, //16 up 4
	
	{Sart_ArcMain_Right0, {  0,   0, 228, 133}, {56, 106}}, //17 right 1
	{Sart_ArcMain_Right1, {  0,   0, 228, 133}, {56, 106}}, //18 right 2
	{Sart_ArcMain_Right2, {  0,   0, 228, 133}, {56, 106}}, //19 right 3
	{Sart_ArcMain_Right3, {  0,   0, 228, 133}, {56, 106}}, //20 right 4
};

static const Animation char_sart_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, ASCR_BACK, 1}},      	   	   	   		   		   	   //CharAnim_Idle
	{2, (const u8[]){ 5,  6,  7,  8, ASCR_BACK, 1}},       		   				    		   //CharAnim_Left
	{2, (const u8[]){ 5,  6,  7,  8, ASCR_BACK, 1}},       		   				    		   //CharAnim_LeftAlt
	{2, (const u8[]){ 9, 10, 11, 12, ASCR_BACK, 1}},       		   				    		   //CharAnim_Down
	{2, (const u8[]){ 9, 10, 11, 12, ASCR_BACK, 1}},       		   				    		   //CharAnim_DownAlt
	{2, (const u8[]){13, 14, 15, 16, ASCR_BACK, 1}},       		   				    		   //CharAnim_Up
	{2, (const u8[]){13, 14, 15, 16, ASCR_BACK, 1}},       		   				    		   //CharAnim_UpAlt
	{2, (const u8[]){17, 18, 19, 20, ASCR_BACK, 1}},       		   				    		   //CharAnim_Right
	{2, (const u8[]){17, 18, 19, 20, ASCR_BACK, 1}},       		   				    		   //CharAnim_RightAlt
};

void Sart_Draw(Character *this, Gfx_Tex *tex, const CharFrame *cframe)
{
	//Draw character
	fixed_t x = this->x - stage.camera.x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t y = this->y - stage.camera.y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {x, y, src.w*2 << FIXED_SHIFT, src.h*2 << FIXED_SHIFT};
	Stage_DrawTex(tex, &src, &dst, stage.camera.bzoom);
}

//Sart character functions
void Char_Sart_SetFrame(void *user, u8 frame)
{
	Char_Sart *this = (Char_Sart*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_sart_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Sart_Tick(Character *character)
{
	Char_Sart *this = (Char_Sart*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Sart_SetFrame);
    Sart_Draw(character, &this->tex, &char_sart_frame[this->frame]);
}
	
void Char_Sart_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Sart_Free(Character *character)
{
	Char_Sart *this = (Char_Sart*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Sart_New(fixed_t x, fixed_t y)
{
	//Allocate sart object
	Char_Sart *this = Mem_Alloc(sizeof(Char_Sart));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Sart_New] Failed to allocate sart object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Sart_Tick;
	this->character.set_anim = Char_Sart_SetAnim;
	this->character.free = Char_Sart_Free;
	
	Animatable_Init(&this->character.animatable, char_sart_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 20;
	
	this->character.focus_x = FIXED_DEC(140,1);
	this->character.focus_y = FIXED_DEC(0,1);
	this->character.focus_zoom = FIXED_DEC(07,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SART.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",  //Sart_ArcMain_Idle0
		"idle1.tim",  //Sart_ArcMain_Idle1
		"idle2.tim",  //Sart_ArcMain_Idle2
		"idle3.tim",  //Sart_ArcMain_Idle3
		"idle4.tim",  //Sart_ArcMain_Idle4
		"left0.tim",  //Sart_ArcMain_Left0
		"left1.tim",  //Sart_ArcMain_Left1
		"left2.tim",  //Sart_ArcMain_Left2
		"left3.tim",  //Sart_ArcMain_Left3
		"down0.tim",  //Sart_ArcMain_Down0
		"down1.tim",  //Sart_ArcMain_Down1
		"down2.tim",  //Sart_ArcMain_Down2
		"down3.tim",  //Sart_ArcMain_Down3
		"up0.tim",    //Sart_ArcMain_Up0
		"up1.tim",    //Sart_ArcMain_Up1
		"up2.tim",    //Sart_ArcMain_Up2
		"up3.tim",    //Sart_ArcMain_Up3
		"right0.tim", //Sart_ArcMain_Right0
		"right1.tim", //Sart_ArcMain_Right1
		"right2.tim", //Sart_ArcMain_Right2
		"right3.tim", //Sart_ArcMain_Right3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
