/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "tdbf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

//Boyfriend skull fragments
static SkullFragment char_tdbf_skull[15] = {
	{ 1 * 8, -87 * 8, -13, -13},
	{ 9 * 8, -88 * 8,   5, -22},
	{18 * 8, -87 * 8,   9, -22},
	{26 * 8, -85 * 8,  13, -13},
	
	{-3 * 8, -82 * 8, -13, -11},
	{ 8 * 8, -85 * 8,  -9, -15},
	{20 * 8, -82 * 8,   9, -15},
	{30 * 8, -79 * 8,  13, -11},
	
	{-1 * 8, -74 * 8, -13, -5},
	{ 8 * 8, -77 * 8,  -9, -9},
	{19 * 8, -75 * 8,   9, -9},
	{26 * 8, -74 * 8,  13, -5},
	
	{ 5 * 8, -73 * 8, -5, -3},
	{14 * 8, -76 * 8,  9, -6},
	{26 * 8, -67 * 8, 15, -3},
};

//Boyfriend player types
enum
{
	TDBF_ArcMain_3DBF0,
	TDBF_ArcMain_3DBF1,
	TDBF_ArcMain_3DBF2,
	TDBF_ArcMain_3DBF3,
	TDBF_ArcMain_3DBF4,
	TDBF_ArcMain_Dead0, //BREAK
	
	TDBF_ArcMain_Max,
};

enum
{
	TDBF_ArcDead_Dead1, //Mic Drop
	TDBF_ArcDead_Dead2, //Twitch
	TDBF_ArcDead_Retry, //Retry prompt
	
	TDBF_ArcDead_Max,
};

#define TDBF_Arc_Max TDBF_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_dead;
	CdlFILE file_dead_arc; //dead.arc file position
	IO_Data arc_ptr[TDBF_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
	
	u8 retry_bump;
	
	SkullFragment skull[COUNT_OF(char_tdbf_skull)];
	u8 skull_scale;
} Char_TDBF;

//Boyfriend player definitions
static const CharFrame char_tdbf_frame[] = {
	{TDBF_ArcMain_3DBF0, { 14,   4,  78,  93}, { 53,  92}}, //0 idle 1
	{TDBF_ArcMain_3DBF0, {117,   3,  78,  94}, { 53,  93}}, //1 idle 2
	{TDBF_ArcMain_3DBF0, { 14, 103,  81,  97}, { 56,  95}}, //2 idle 3
	{TDBF_ArcMain_3DBF0, {102, 104,  85,  97}, { 60,  95}}, //3 idle 4
	
	{TDBF_ArcMain_3DBF1, {  3, 100,  83, 100}, { 64,  98}}, //4 left 1
	{TDBF_ArcMain_3DBF1, {101, 102,  84,  98}, { 62,  96}}, //5 left 2
	
	{TDBF_ArcMain_3DBF1, { 11,  12,  83,  85}, { 58,  84}}, //6 down 1
	{TDBF_ArcMain_3DBF1, {118,   7,  79,  90}, { 54,  90}}, //7 down 2
	
	{TDBF_ArcMain_3DBF2, { 29, 132,  80, 110}, { 55, 108}}, //8 up 1
	{TDBF_ArcMain_3DBF2, {131, 140,  81, 103}, { 56, 102}}, //9 up 2
	
	{TDBF_ArcMain_3DBF2, { 36,  18,  88,  98}, { 62,  96}}, //10 right 1
	{TDBF_ArcMain_3DBF2, {138,  18,  85,  98}, { 60,  96}}, //11 right 2
	
	{TDBF_ArcMain_3DBF4, {  3, 100,  83, 100}, { 64,  98}}, //12 left miss 1
	{TDBF_ArcMain_3DBF4, {101, 102,  84,  98}, { 62,  96}}, //13 left miss 2
	
	{TDBF_ArcMain_3DBF4, { 11,  12,  83,  85}, { 58,  84}}, //14 down miss 1
	{TDBF_ArcMain_3DBF4, {118,   7,  79,  90}, { 54,  90}}, //15 down miss 2
	
	{TDBF_ArcMain_3DBF3, { 29, 132,  80, 110}, { 55, 108}}, //16 up miss 1
	{TDBF_ArcMain_3DBF3, {131, 140,  81, 103}, { 56, 103}}, //17 up miss 2
	
	{TDBF_ArcMain_3DBF3, { 36,  18,  88,  98}, { 62,  96}}, //18 right miss 1
	{TDBF_ArcMain_3DBF3, {138,  18,  85,  98}, { 60,  96}}, //19 right miss 2

	{TDBF_ArcMain_Dead0, {  0,   0, 128, 128}, { 53,  98}}, //20 dead0 0
	{TDBF_ArcMain_Dead0, {128,   0, 128, 128}, { 53,  98}}, //21 dead0 1
	{TDBF_ArcMain_Dead0, {  0, 128, 128, 128}, { 53,  98}}, //22 dead0 2
	{TDBF_ArcMain_Dead0, {128, 128, 128, 128}, { 53,  98}}, //23 dead0 3
	
	{TDBF_ArcDead_Dead1, {  0,   0, 128, 128}, { 53,  98}}, //24 dead1 0
	{TDBF_ArcDead_Dead1, {128,   0, 128, 128}, { 53,  98}}, //25 dead1 1
	{TDBF_ArcDead_Dead1, {  0, 128, 128, 128}, { 53,  98}}, //26 dead1 2
	{TDBF_ArcDead_Dead1, {128, 128, 128, 128}, { 53,  98}}, //27 dead1 3
	
	{TDBF_ArcDead_Dead2, {  0,   0, 128, 128}, { 53,  98}}, //28 dead2 body twitch 0
	{TDBF_ArcDead_Dead2, {128,   0, 128, 128}, { 53,  98}}, //29 dead2 body twitch 1
	{TDBF_ArcDead_Dead2, {  0, 128, 128, 128}, { 53,  98}}, //30 dead2 balls twitch 0
	{TDBF_ArcDead_Dead2, {128, 128, 128, 128}, { 53,  98}}, //31 dead2 balls twitch 1
};

static const Animation char_tdbf_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},             //CharAnim_Left
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},      		 //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},             //CharAnim_Down
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},	         //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},             //CharAnim_Up
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},       		 //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},             //CharAnim_Right
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},             //CharAnim_RightAlt
	
	{1, (const u8[]){ 4, 12, 12, 13, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 6, 14, 14, 15, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 8, 16, 16, 17, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){10, 18, 18, 19, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){ 8,  8,  9, ASCR_BACK, 1}},         //PlayerAnim_Peace
	{2, (const u8[]){ 8,  8,  9,  9, ASCR_REPEAT}},      //PlayerAnim_Sweat
	
	{5, (const u8[]){21, 22, 23, 23, 23, 23, 23, 23, 23, 23, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{5, (const u8[]){23, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{3, (const u8[]){25, 26, 27, 27, 27, 27, 27, 27, 27, 27, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{3, (const u8[]){27, ASCR_REPEAT}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){28, 29, 27, 27, 27, 27, 27, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){30, 31, 27, 27, 27, 27, 27, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){28, 29, 28, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){30, 31, 30, ASCR_REPEAT}},  //PlayerAnim_Dead5
};

//Boyfriend player functions
void Char_TDBF_SetFrame(void *user, u8 frame)
{
	Char_TDBF *this = (Char_TDBF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_tdbf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_TDBF_Tick(Character *character)
{
	Char_TDBF *this = (Char_TDBF*)character;
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
	}
	
	//Retry screen
	if (character->animatable.anim >= PlayerAnim_Dead3)
	{
		//Tick skull fragments
		if (this->skull_scale)
		{
			SkullFragment *frag = this->skull;
			for (size_t i = 0; i < COUNT_OF_MEMBER(Char_TDBF, skull); i++, frag++)
			{
				//Draw fragment
				RECT frag_src = {
					(i & 1) ? 112 : 96,
					(i >> 1) << 4,
					16,
					16
				};
				fixed_t skull_dim = (FIXED_DEC(16,1) * this->skull_scale) >> 6;
				fixed_t skull_rad = skull_dim >> 1;
				RECT_FIXED frag_dst = {
					character->x + (((fixed_t)frag->x << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.x,
					character->y + (((fixed_t)frag->y << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.y,
					skull_dim,
					skull_dim,
				};
				Stage_DrawTex(&this->tex_retry, &frag_src, &frag_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
				
				//Move fragment
				frag->x += frag->xsp;
				frag->y += ++frag->ysp;
			}
			
			//Decrease scale
			this->skull_scale--;
		}
		
		//Draw input options
		u8 input_scale = 16 - this->skull_scale;
		if (input_scale > 16)
			input_scale = 0;
		
		RECT button_src = {
			 0, 96,
			16, 16
		};
		RECT_FIXED button_dst = {
			character->x - FIXED_DEC(32,1) - stage.camera.x,
			character->y - FIXED_DEC(88,1) - stage.camera.y,
			(FIXED_DEC(16,1) * input_scale) >> 4,
			FIXED_DEC(16,1),
		};
		
		//Cross - Retry
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Circle - Blueball
		button_src.x = 16;
		button_dst.y += FIXED_DEC(56,1);
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Draw 'RETRY'
		u8 retry_frame;
		
		if (character->animatable.anim == PlayerAnim_Dead6)
		{
			//Selected retry
			retry_frame = 2 - (this->retry_bump >> 3);
			if (retry_frame >= 3)
				retry_frame = 0;
			if (this->retry_bump & 2)
				retry_frame += 3;
			
			if (++this->retry_bump == 0xFF)
				this->retry_bump = 0xFD;
		}
		else
		{
			//Idle
			retry_frame = 1 +  (this->retry_bump >> 2);
			if (retry_frame >= 3)
				retry_frame = 0;
			
			if (++this->retry_bump >= 55)
				this->retry_bump = 0;
		}
		
		RECT retry_src = {
			(retry_frame & 1) ? 48 : 0,
			(retry_frame >> 1) << 5,
			48,
			32
		};
		RECT_FIXED retry_dst = {
			character->x -  FIXED_DEC(7,1) - stage.camera.x,
			character->y - FIXED_DEC(92,1) - stage.camera.y,
			FIXED_DEC(48,1),
			FIXED_DEC(32,1),
		};
		Stage_DrawTex(&this->tex_retry, &retry_src, &retry_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_TDBF_SetFrame);
	Character_Draw(character, &this->tex, &char_tdbf_frame[this->frame]);
}

void Char_TDBF_SetAnim(Character *character, u8 anim)
{
	Char_TDBF *this = (Char_TDBF*)character;
	
	//Perform animation checks
	switch (anim)
	{
		case PlayerAnim_Dead0:
			//Begin reading dead.arc and adjust focus
			this->arc_dead = IO_AsyncReadFile(&this->file_dead_arc);
			character->focus_x = FIXED_DEC(0,1);
			character->focus_y = FIXED_DEC(-40,1);
			character->focus_zoom = FIXED_DEC(125,100);
			break;
		case PlayerAnim_Dead2:
			//Unload main.arc
			Mem_Free(this->arc_main);
			this->arc_main = this->arc_dead;
			this->arc_dead = NULL;
			
			//Find dead.arc files
			const char **pathp = (const char *[]){
				"dead1.tim", //TDBF_ArcDead_Dead1
				"dead2.tim", //TDBF_ArcDead_Dead2
				"retry.tim", //TDBF_ArcDead_Retry
				NULL
			};
			IO_Data *arc_ptr = this->arc_ptr;
			for (; *pathp != NULL; pathp++)
				*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
			
			//Load retry art
			Gfx_LoadTex(&this->tex_retry, this->arc_ptr[TDBF_ArcDead_Retry], 0);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_TDBF_Free(Character *character)
{
	Char_TDBF *this = (Char_TDBF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_dead);
}

Character *Char_TDBF_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_TDBF *this = Mem_Alloc(sizeof(Char_TDBF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_TDBF_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_TDBF_Tick;
	this->character.set_anim = Char_TDBF_SetAnim;
	this->character.free = Char_TDBF_Free;
	
	Animatable_Init(&this->character.animatable, char_tdbf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;
	
	this->character.healthb_i = 0;
	
	this->character.focus_x = FIXED_DEC(-60,1);
	this->character.focus_y = FIXED_DEC(-72,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\3DBF.ARC;1");
	this->arc_dead = NULL;
	IO_FindFile(&this->file_dead_arc, "\\CHAR\\BFDEAD.ARC;1");
	
	const char **pathp = (const char *[]){
		"3dbf0.tim",   //TDBF_ArcMain_3DBF0
		"3dbf1.tim",   //TDBF_ArcMain_3DBF1
		"3dbf2.tim",   //TDBF_ArcMain_3DBF2
		"3dbf3.tim",   //TDBF_ArcMain_3DBF3
		"3dbf4.tim",   //TDBF_ArcMain_3DBF4
		"3ddead0.tim", //TDBF_ArcMain_Dead0
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize player state
	this->retry_bump = 0;
	
	//Copy skull fragments
	memcpy(this->skull, char_tdbf_skull, sizeof(char_tdbf_skull));
	this->skull_scale = 64;
	
	SkullFragment *frag = this->skull;
	for (size_t i = 0; i < COUNT_OF_MEMBER(Char_TDBF, skull); i++, frag++)
	{
		//Randomize trajectory
		frag->xsp += RandomRange(-4, 4);
		frag->ysp += RandomRange(-2, 2);
	}
	
	return (Character*)this;
}
