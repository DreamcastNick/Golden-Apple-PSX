/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bandu.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Bandu character structure
enum
{
	Bandu_ArcMain_IdleA0,
	Bandu_ArcMain_IdleA1,
	Bandu_ArcMain_IdleA2,
	Bandu_ArcMain_IdleA3,
	Bandu_ArcMain_IdleA4,
	Bandu_ArcMain_IdleA5,
	Bandu_ArcMain_IdleB0,
	Bandu_ArcMain_LeftA0,
	Bandu_ArcMain_LeftA1,
	Bandu_ArcMain_LeftB0,
	Bandu_ArcMain_LeftB1,
	Bandu_ArcMain_DownA0,
	Bandu_ArcMain_DownA1,
	Bandu_ArcMain_DownB0,
	Bandu_ArcMain_DownB1,
	Bandu_ArcMain_UpA0,
	Bandu_ArcMain_UpA1,
	Bandu_ArcMain_UpB0,
	Bandu_ArcMain_UpB1,
	Bandu_ArcMain_RightA0,
	Bandu_ArcMain_RightA1,
	Bandu_ArcMain_RightB0,
	Bandu_ArcMain_RightB1,
	Bandu_ArcMain_Huh0,
	Bandu_ArcMain_Huh1,
	Bandu_ArcMain_Huh2,
	Bandu_ArcMain_Huh3,
	Bandu_ArcMain_Huh4,

	Bandu_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Bandu_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Bandu;

//Bandu character definitions
static const CharFrame char_bandu_frame[] = {
	{Bandu_ArcMain_IdleA0, {  0,   0, 128, 188}, { 51, 90}}, //0 idle a 1
	{Bandu_ArcMain_IdleA1, {  0,   0, 128, 188}, { 51, 90}}, //1 idle a 2
	{Bandu_ArcMain_IdleA2, {  0,   0, 128, 188}, { 51, 90}}, //2 idle a 3
	{Bandu_ArcMain_IdleA3, {  0,   0, 128, 188}, { 51, 90}}, //3 idle a 4
	{Bandu_ArcMain_IdleA4, {  0,   0, 128, 188}, { 51, 90}}, //4 idle a 5
	{Bandu_ArcMain_IdleA5, {  0,   0, 128, 188}, { 51, 90}}, //5 idle a 6
	
	{Bandu_ArcMain_IdleB0, {  0,   0, 128, 181}, { 51, 86}}, //6 idle b 1
	
	{Bandu_ArcMain_LeftA0, {  0,   0, 228, 189}, {159, 84}}, //7 left a 1
	{Bandu_ArcMain_LeftA1, {  0,   0, 228, 189}, {159, 84}}, //8 left a 2
	
	{Bandu_ArcMain_LeftB0, {  0,   0, 116, 152}, { 65, 57}}, //9 left b 1
	{Bandu_ArcMain_LeftB1, {  0,   0, 116, 152}, { 65, 57}}, //10 left b 2
	
	{Bandu_ArcMain_DownA0, {  0,   0, 164, 180}, {70, 88}}, //11 down a 1
	{Bandu_ArcMain_DownA1, {  0,   0, 164, 180}, {70, 88}}, //12 down a 2
	
	{Bandu_ArcMain_DownB0, {  0,   0,  96, 139}, {36, 44}},  //13 down b 1
	{Bandu_ArcMain_DownB1, {  0,   0,  96, 139}, {36, 44}},  //14 down b 2
	
	{Bandu_ArcMain_UpA0, {  0,   0,  164, 211}, {65, 118}}, //15 up a 1
	{Bandu_ArcMain_UpA1, {  0,   0,  164, 211}, {65, 118}}, //16 up a 2
	
	{Bandu_ArcMain_UpB0, {   0,   0,  128, 164}, { 51, 67}}, //17 up b 1
	{Bandu_ArcMain_UpB1, {   0,   0,  128, 164}, { 51, 67}}, //18 up b 2
	
	{Bandu_ArcMain_RightA0, {  0,   0, 224, 183}, {50, 84}}, //19 right a 1
	{Bandu_ArcMain_RightA1, {  0,   0, 224, 183}, {50, 84}}, //20 right a 2
	
	{Bandu_ArcMain_RightB0, {  0,   0, 116, 139}, { 30, 43}}, //21 right b 1
	{Bandu_ArcMain_RightB1, {  0,   0, 116, 139}, { 30, 43}}, //22 right b 2
	
	{Bandu_ArcMain_Huh0, {  0,   0, 116, 179}, { 33, 98}}, //23 huh 0
	{Bandu_ArcMain_Huh1, {  0,   0, 116, 179}, { 13, 118}}, //24 huh 1
	{Bandu_ArcMain_Huh2, {  0,   0, 116, 179}, { -7, 138}}, //25 huh 2
	{Bandu_ArcMain_Huh3, {  0,   0, 116, 179}, {-27, 158}}, //26 huh 3
	{Bandu_ArcMain_Huh4, {  0,   0, 116, 179}, {-47, 178}}, //27 huh 4
};

static const Animation char_bandu_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5, ASCR_BACK, 1}},      	   	   		   		   //CharAnim_Idle
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},       		   				    			   //CharAnim_Left
	{2, (const u8[]){ 9, 10, ASCR_CHGANI, CharAnim_IdleAlt}},      		   				   //CharAnim_LeftAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},  		   					   				   //CharAnim_Down
	{2, (const u8[]){13, 14, ASCR_CHGANI, CharAnim_IdleAlt}},  	          				   //CharAnim_DownAlt
	{2, (const u8[]){15, 16, ASCR_BACK, 1}},                		   	   				   //CharAnim_Up
	{2, (const u8[]){17, 18, ASCR_CHGANI, CharAnim_IdleAlt}},              			       //CharAnim_UpAlt
	{2, (const u8[]){19, 20, ASCR_BACK, 1}}, 							   				   //CharAnim_Right
	{2, (const u8[]){21, 22, ASCR_CHGANI, CharAnim_IdleAlt}},              				   //CharAnim_RightAlt
	{2, (const u8[]){ 6,  6, ASCR_CHGANI, CharAnim_IdleAlt}},						       //CharAnim_IdleAlt
	{2, (const u8[]){23, 23, 24, 24, 25, 25, 26, 26, 27, 27, ASCR_BACK, 1}},      	   	   //CharAnim_Huh
};

//Bandu character functions
void Char_Bandu_SetFrame(void *user, u8 frame)
{
	Char_Bandu *this = (Char_Bandu*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bandu_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Bandu_Tick(Character *character)
{
	Char_Bandu *this = (Char_Bandu*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);

	 Animatable_Animate(&character->animatable, (void*)this, Char_Bandu_SetFrame);
	
	this->character.number_i = 1;
	this->character.swap_i = stage.song_step % 0x1;
	this->character.swapdeath_i = stage.song_step % 0x1;
	
	if (stage.stage_id == StageId_1_2 && stage.timercount <= 11555)
    Character_Draw(character, &this->tex, &char_bandu_frame[this->frame]);

	//Stage specific animations
	if (stage.note_scroll >= 0)
	{
		switch (stage.stage_id)
		{
			case StageId_1_2: //Applecore
				if ((stage.timercount) == 4565)
					character->set_anim(character, CharAnim_IdleAlt);
				if ((stage.timercount) == 11520)
					character->set_anim(character, CharAnim_Huh);
				break;
			default:
				break;
		}
	}
}
	
void Char_Bandu_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Bandu_Free(Character *character)
{
	Char_Bandu *this = (Char_Bandu*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Bandu_New(fixed_t x, fixed_t y)
{
	//Allocate bandu object
	Char_Bandu *this = Mem_Alloc(sizeof(Char_Bandu));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Bandu_New] Failed to allocate bandu object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Bandu_Tick;
	this->character.set_anim = Char_Bandu_SetAnim;
	this->character.free = Char_Bandu_Free;
	
	Animatable_Init(&this->character.animatable, char_bandu_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = stage.tex_hud2;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-25,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BANDU.ARC;1");
	
	const char **pathp = (const char *[]){
		"idlea0.tim",  //Bandu_ArcMain_IdleA0
		"idlea1.tim",  //Bandu_ArcMain_IdleA1
		"idlea2.tim",  //Bandu_ArcMain_IdleA2
		"idlea3.tim",  //Bandu_ArcMain_IdleA3
		"idlea4.tim",  //Bandu_ArcMain_IdleA4
		"idlea5.tim",  //Bandu_ArcMain_IdleA5
		"idleb0.tim",  //Bandu_ArcMain_IdleB0
		"lefta0.tim",  //Bandu_ArcMain_LeftA0
		"lefta1.tim",  //Bandu_ArcMain_LeftA1
		"leftb0.tim",  //Bandu_ArcMain_LeftB0
		"leftb1.tim",  //Bandu_ArcMain_LeftB1
		"downa0.tim",  //Bandu_ArcMain_DownA0
		"downa1.tim",  //Bandu_ArcMain_DownA1
		"downb0.tim",  //Bandu_ArcMain_DownB0
		"downb1.tim",  //Bandu_ArcMain_DownB1
		"upa0.tim",    //Bandu_ArcMain_UpA0
		"upa1.tim",    //Bandu_ArcMain_UpA1
		"upb0.tim",    //Bandu_ArcMain_UpB0
		"upb1.tim",    //Bandu_ArcMain_UpB1
		"righta0.tim", //Bandu_ArcMain_RightA0
		"righta1.tim", //Bandu_ArcMain_RightA1
		"rightb0.tim", //Bandu_ArcMain_RightB0
		"rightb1.tim", //Bandu_ArcMain_RightB1
		"huh0.tim",    //Bandu_ArcMain_Huh0
		"huh1.tim",    //Bandu_ArcMain_Huh1
		"huh2.tim",    //Bandu_ArcMain_Huh2
		"huh3.tim",    //Bandu_ArcMain_Huh3
		"huh4.tim",    //Bandu_ArcMain_Huh4
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
