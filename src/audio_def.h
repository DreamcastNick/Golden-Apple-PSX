#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(8000)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(3840)},  //XA_GameOver
	//WEEK1A.XA
	{XA_Week1A, XA_LENGTH(14600)}, //XA_Bopeebo
	{XA_Week1A, XA_LENGTH(30900)}, //XA_Fresh
	//WEEK1B.XA
	{XA_Week1B, XA_LENGTH(20200)}, //XA_Dadbattle
	{XA_Week1B, XA_LENGTH(20200)}, //XA_Tutorial
	//WEEK2A.XA
	{XA_Week2A, XA_LENGTH(66100)}, //XA_Spookeez
	{XA_Week2A, XA_LENGTH(13500)}, //XA_South
	//WEEK2B.XA
	{XA_Week2B, XA_LENGTH(11400)}, //XA_Monster
	{XA_Week2B, XA_LENGTH(11500)}, //XA_Clucked
	//WEEK3A.XA
	{XA_Week3A, XA_LENGTH(10800)},  //XA_Pico
	{XA_Week3A, XA_LENGTH(12600)}, //XA_Philly
	//WEEK3B.XA
	{XA_Week3B, XA_LENGTH(8600)}, //XA_Blammed
	//WEEK4A.XA
	{XA_Week4A, XA_LENGTH(15200)},  //XA_SatinPanties
	{XA_Week4A, XA_LENGTH(15800)}, //XA_High
	//WEEK4B.XA
	{XA_Week4B, XA_LENGTH(15100)}, //XA_MILF
	{XA_Week4B, XA_LENGTH(8900)}, //XA_Test
	//WEEK5A.XA
	{XA_Week5A, XA_LENGTH(23900)}, //XA_Cocoa
	{XA_Week5A, XA_LENGTH(14100)},  //XA_Eggnog
	//WEEK5B.XA
	{XA_Week5B, XA_LENGTH(12000)}, //XA_WinterHorrorland
	//CUT.XA
	{XA_Cut, XA_LENGTH(1400)},    //XA_Dialogue
	{XA_Cut, XA_LENGTH(8200)},    //XA_Ambiance
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\WEEK1A.XA;1", //XA_Week1A
	"\\MUSIC\\WEEK1B.XA;1", //XA_Week1B
	"\\MUSIC\\WEEK2A.XA;1", //XA_Week2A
	"\\MUSIC\\WEEK2B.XA;1", //XA_Week2B
	"\\MUSIC\\WEEK3A.XA;1", //XA_Week3A
	"\\MUSIC\\WEEK3B.XA;1", //XA_Week3B
	"\\MUSIC\\WEEK4A.XA;1", //XA_Week4A
	"\\MUSIC\\WEEK4B.XA;1", //XA_Week4B
	"\\MUSIC\\WEEK5A.XA;1", //XA_Week5A
	"\\MUSIC\\WEEK5B.XA;1", //XA_Week5B
	"\\MUSIC\\CUT.XA;1",    //XA_Cut
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	//WEEK1A.XA
	{"bopeebo", true}, //XA_Bopeebo
	{"fresh", true},   //XA_Fresh
	//WEEK1B.XA
	{"dadbattle", true}, //XA_Dadbattle
	{"tutorial", true}, //XA_Tutorial
	//WEEK2A.XA
	{"spookeez", true}, //XA_Spookeez
	{"south", true},    //XA_South
	//WEEK2B.XA
	{"monster", true}, //XA_Monster
	{"clucked", true}, //XA_Clucked
	//WEEK3A.XA
	{"pico", true},   //XA_Pico
	{"philly", true}, //XA_Philly
	//WEEK3B.XA
	{"blammed", true}, //XA_Blammed
	//WEEK4A.XA
	{"satinpanties", true}, //XA_SatinPanties
	{"high", true},         //XA_High
	//WEEK4B.XA
	{"milf", true}, //XA_MILF
	{"test", true}, //XA_Test
	//WEEK5A.XA
	{"cocoa", true},  //XA_Cocoa
	{"eggnog", true}, //XA_Eggnog
	//WEEK5B.XA
	{"winterhorrorland", true}, //XA_WinterHorrorland
	//CUT.XA
	{"dialogue", false}, //XA_Dialogue
	{"ambiance", false}, //XA_Ambiance
	
	{NULL, false}
};
