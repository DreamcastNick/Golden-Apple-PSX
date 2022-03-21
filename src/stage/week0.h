/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef PSXF_GUARD_WEEK0_H
#define PSXF_GUARD_WEEK0_H

#include "../stage.h"

//Week 0 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Background
	Gfx_Tex tex_back1; //Background 2
	Gfx_Tex tex_back2; //Background 3
	
	//Pico chart
	u16 *pico_chart;
} Back_Week0;

//Week 0 functions
StageBack *Back_Week0_New();

#endif
