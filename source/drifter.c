// C Source File
// Created 10/16/01; 1:18:05 PM

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#define SAVE_SCREEN           // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files
#include "extgraph.h"
#include <intr.h>


const int crazy_drift = 120;
const int super_drift = 210;
const int mega_drift  = 300;
const unsigned char score_update_freq = 3;

INT_HANDLER Old_Int_1 = NULL;	

short _ti89;                  // Produce .89Z File

int i, j, lwall, rwall, lslope, rslope, width, crashed, rsign, lsign;
int min_width, max_width, rchange, lchange;
int drift, hidrift, crazy, super, mega;
int ultra, hyper, supreme;
int ultimate, perfect, eternal;
int crazy_total, super_total, mega_total, dt4, dt5, dt6, dt7, dt8, dt9;
int bonus_time;		// time to keep the bonus text in the status bar
int combo, last_combo;			// number of consecutive drifts with only one thrust in between
int thrusted;		// used to keep track of combos
int fuel_used, last_fuel_used, quit_game, action, got_hiscore;
int game_mode = 0;
int game_speed = 4;

long score, time, num, miles;
long hiscore = 50000;

long hiscores[2][10] = {{10000000, 1000000, 750000, 500000, 250000, 150000, 100000, 50000, 25000, 1000}, 
						{100000, 75000, 50000, 35000, 20000, 10000, 7500, 5000, 2500, 1000}};
unsigned char himiles[2][10] = {{28, 23, 26, 17, 10, 8, 8, 5, 4, 5},
								{28, 23, 26, 17, 10, 8, 8, 5, 4, 5}};
static unsigned char initials[2][10][4] = {{"BUU\0", "BAB\0", "DVU\0", "CEL\0", "FRZ\0", 
											"#17\0", "DRG\0", "GJR\0", "BRT\0", "HRC\0"},
											{"GKU\0", "GHN\0", "TKS\0", "GTN\0", "PCL\0", 
											"#18\0", "KLN\0", "TEN\0", "YAM\0", "YAJ\0"}};
static unsigned char letters[57] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_~/\?<>.,";
static unsigned char last_initials[3] = {0, 0, 0};

short font;

float xpos, old_xpos, velocity; 

//static unsigned char ship[] = {0x81,0x81,0xC3,0xC3,0xFF,0x7E,0x3C,0x3C,0x3C,0x7E,0xFF,0xC3};
//static unsigned char ship[]     = {0xC3,0xFF,0xFF,0xE7,0xC3,0x42,0x42,0x7E,0xC3,0xC3,0xFF,0xC3};
static unsigned char ship[]     = {0x81,0xC3,0xC3,0xFF,0xFF,0x7E,0x7E,0x7E,0xFF,0xFF,0xC3,0xC3};
static unsigned short csprite[] = {0xC003,0xC003,0xC3F3,0xC7F3,0xCE03,0xCC03,0xCC03,0xCE03,0xC7F3,0xC3F3,0xC003,0xC003};
static unsigned short ssprite[] = {0xC003,0xC003,0xC3F3,0xC7F3,0xCC03,0xCC03,0xC7C3,0xC073,0xCFF3,0xCFE3,0xC003,0xC003};
static unsigned short msprite[] = {0xC003,0xC003,0xCC33,0xCE73,0xCFF3,0xCDB3,0xCC33,0xCC33,0xCC33,0xCC33,0xC003,0xC003};
static unsigned short dsprite[] = {0xC003,0xC003,0xC083,0xC1C3,0xC3E3,0xC7F3,0xC3E3,0xC1C3,0xC083,0xC003,0xC003,0xC003};
static unsigned char pointer[]  = {0xC0,0xF0,0xFC,0xFF,0xFF,0xFC,0xF0,0xC0};
static unsigned char vpointer[] = {0x18,0x18,0x3C,0x3C,0x7E,0x7E,0xFF,0xFF};
volatile int ticks = 0;		// have to declare this 'volatile' since it will be modified from an interrupt handler routine

static char bonus_text[] = "                      \0";
static char score_text[] = "               \0";
static char combo_text[] = "00x Combo!\0";



//*******************************************************************************************************
//********************   SUPPORT FUNCTIONS  ************************************************************* 
//*******************************************************************************************************


DEFINE_INT_HANDLER (My_Int_1)
// This interrupt handler grabs auto_int_1, which fires ~395 times per second.
// Obviously it's important to keep a procedure small which is firing that often;
// all we do here is increment the "ticks" variable, which is used for timing.
// Then we execute the old handler as well, so we don't break anything.
{
	ticks++;
	ExecuteHandler(Old_Int_1);
}

char *IntToStr (unsigned long an_integer)
{
  static char result [] = "           \0";    
  char *ptr = result + 10;
  int digits = 0;
 
  strcpy(result, "           \0");
  if (an_integer == 0) {
  	strcpy(result, "          0\0");
  	ptr--;
  }
  while (an_integer)
    {
      if (((++digits % 3) == 1) && (digits > 1)) {
      	*ptr-- = ',';
      }
      *ptr-- = an_integer % 10 + '0';
      an_integer/=10;
    }
  return ptr;
}

int get_menu_choice(int x, int top_y, int choices, int spacing, int chosen) {
	// parameter "x" is the horizontal coordinate where the choices are drawn; the pointer will be left of it.
	// first choice is considered to be 0, not 1.
	
	int last_choice;
	int current = chosen;
	short key;
	
	// draw the initial pointer position:
	Sprite8_XOR(x - 10, top_y + current * spacing, 8, pointer, LCD_MEM);
	
	while (1) {
		last_choice = current;
		key = ngetchx();
		if (key == KEY_ENTER) { return current; }
		if (key == KEY_RIGHT) { return current; }
		if (key == KEY_DOWN) {
			current = ((++current) % choices);
		}
		if (key == KEY_UP) {
			current = ((--current + choices) % choices);
		}
		Sprite8_XOR(x - 10, top_y + last_choice * spacing, 8, pointer, LCD_MEM);
		Sprite8_XOR(x - 10, top_y + current * spacing, 8, pointer, LCD_MEM);
	}
}

void new_game() {
// initialize global variables for a new game
	velocity = 0;
	old_xpos = xpos = 76.0;
	
	min_width = ((game_mode == 0) ? 35 : 40);
	max_width = ((game_mode == 0) ? 55 : 65);
	rwall = 130;
	lwall = 30;
	rslope = 1;
	lslope = -1;
	score = ((game_mode == 0) ? 0 : 0);
	miles = 0;
	time = 0;
	fuel_used = 0;
	drift = 0;
	crazy = super = mega = ultra = hyper = supreme = ultimate = perfect = eternal = 0;
	crazy_total = super_total = mega_total = dt4 = dt5 = dt6 = dt7 = dt8 = dt9 = 0;
	got_hiscore = bonus_time = 0;
	thrusted = combo = last_combo = quit_game = 0;
}


//*******************************************************************************************************
//********************   MENU / SCREEN ROUTINES  ******************************************************** 
//*******************************************************************************************************
void combo_scores(){
	// display high scores for combo mode
	
	static char place[] = "xth\0";
	
	ClrScr();
	FontSetSys(F_6x8);
	FastDrawHLine(LCD_MEM, 0, 159, 3, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 4, A_NORMAL);
	DrawStr(50,  7, "Combo Mode", A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 16, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
	FontSetSys(F_8x10);
	DrawStr(10, 20, "1st", A_NORMAL);
	DrawStr(34, 20, IntToStr(hiscores[1][0]), A_NORMAL);
	DrawStr(130, 20, initials[1][0], A_NORMAL);
	FontSetSys(F_6x8);
	DrawStr(15, 32, "2nd", A_NORMAL);
	DrawStr(35, 32, IntToStr(hiscores[1][1]), A_NORMAL);
	DrawStr(130, 32, initials[1][1], A_NORMAL);
	FontSetSys(F_4x6);
	DrawStr(20, 42, "3rd", A_NORMAL);
	DrawStr(40, 42, IntToStr(hiscores[1][2]), A_NORMAL);
	DrawStr(130, 42, initials[1][2], A_NORMAL);
	for (i = 4; i < 10; i++) {
		place[0] = '0' + i;
		DrawStr(20, 24 + i * 6, place, A_NORMAL);
		DrawStr(40, 24 + i * 6, IntToStr(hiscores[1][i-1]), A_NORMAL);
		DrawStr(130, 24 + i * 6, initials[1][i-1], A_NORMAL);
	}
	DrawStr(16, 84, "10th", A_NORMAL);
	DrawStr(40, 84, IntToStr(hiscores[1][9]), A_NORMAL);
	DrawStr(130, 84, initials[1][9], A_NORMAL);
	ngetchx();
}

void classic_scores(){
	// display high scores for classic mode
	
	static char place[] = "xth\0";
	
	ClrScr();
	FontSetSys(F_6x8);
	FastDrawHLine(LCD_MEM, 0, 159, 3, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 4, A_NORMAL);
	DrawStr(45,  7, "Classic Mode", A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 16, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
	FontSetSys(F_8x10);
	DrawStr(10, 20, "1st", A_NORMAL);
	DrawStr(34, 20, IntToStr(hiscores[0][0]), A_NORMAL);
	DrawStr(130, 20, initials[0][0], A_NORMAL);
	FontSetSys(F_6x8);
	DrawStr(15, 32, "2nd", A_NORMAL);
	DrawStr(35, 32, IntToStr(hiscores[0][1]), A_NORMAL);
	DrawStr(130, 32, initials[0][1], A_NORMAL);
	FontSetSys(F_4x6);
	DrawStr(20, 42, "3rd", A_NORMAL);
	DrawStr(40, 42, IntToStr(hiscores[0][2]), A_NORMAL);
	DrawStr(130, 42, initials[0][2], A_NORMAL);
	for (i = 4; i < 10; i++) {
		place[0] = '0' + i;
		DrawStr(20, 24 + i * 6, place, A_NORMAL);
		DrawStr(40, 24 + i * 6, IntToStr(hiscores[0][i-1]), A_NORMAL);
		DrawStr(130, 24 + i * 6, initials[0][i-1], A_NORMAL);
	}
	DrawStr(16, 84, "10th", A_NORMAL);
	DrawStr(40, 84, IntToStr(hiscores[0][9]), A_NORMAL);
	DrawStr(130, 84, initials[0][9], A_NORMAL);
	ngetchx();
}

void update_hiscores() {
	static char message[] = "You got      Place!\0";
	short key;
	int pos, old_pos, place;
	
	// find out what place they earned
	i = 9;
	while ((score > hiscores[game_mode][i]) && (i > 0)) { i--; }
	if (score < hiscores[game_mode][i]) { i++; }
	place = i;
	
	// move everyone with a lower score down by one spot
	for (i = 9; i > place; i--) {
		hiscores[game_mode][i] = hiscores[game_mode][i-1];
		for (j = 0; j < 3; j++) {
			initials[game_mode][i][j] = initials[game_mode][i-1][j];
		}
	}
	
	if (place == 0) {
		message[8] = ' ';
		message[9] = '1';
		message[10] = 's';
		message[11] = 't';
	}
	if (place == 1) {
		message[8] = ' ';
		message[9] = '2';
		message[10] = 'n';
		message[11] = 'd';
	}
	if (place == 2) {
		message[8] = ' ';
		message[9] = '3';
		message[10] = 'r';
		message[11] = 'd';
	}
	if ((place > 2) && (place < 9)) {
		message[8] = ' ';
		message[9] = ('1' + i);
		message[10] = 't';
		message[11] = 'h';
	}
	if (place == 9) {
		message[8] = '1';
		message[9] = '0';
		message[10] = 't';
		message[11] = 'h';
	}
	FontSetSys(F_8x10);
	ClrScr();
	DrawStr(15, 0, "Congratulations!", A_NORMAL);
	DrawStr(5, 12, message, A_NORMAL);
	DrawStr(0, 24, "Enter your initials:", A_NORMAL);
	FontSetSys(F_6x8);
	DrawStr(20, 60, "Up/Down - Change letter", A_NORMAL);
	DrawStr(20, 70, "Left/Right - Move cursor", A_NORMAL);
	DrawStr(20, 80, "CLEAR - Erase letter", A_NORMAL);
	DrawStr(20, 90, "ENTER - Confirm", A_NORMAL);

	FontSetSys(F_8x10);
	pos = 0;

	// draw the initial pointer:
	Sprite8_XOR(64 + pos * 10, 51, 8, vpointer, LCD_MEM);
	
	key = KEY_ESC;
	while (key != KEY_ENTER) {
		old_pos = pos;
		if (key == KEY_LEFT)  { pos = ((--pos + 3) % 3); }
		if (key == KEY_RIGHT) { pos = ((++pos) % 3); }
		if (key == KEY_DOWN) {
			last_initials[pos] = ((last_initials[pos] + 56) % 57);
		}
		if (key == KEY_UP) {
			last_initials[pos] = ((++last_initials[pos]) % 57);
		}
		if (key == KEY_CLEAR) { last_initials[pos] = 0; }
		DrawChar(64, 40, letters[last_initials[0]], A_REPLACE);
		DrawChar(74, 40, letters[last_initials[1]], A_REPLACE);
		DrawChar(84, 40, letters[last_initials[2]], A_REPLACE);
		Sprite8_XOR(64 + old_pos * 10, 51, 8, vpointer, LCD_MEM);
		Sprite8_XOR(64 + pos * 10, 51, 8, vpointer, LCD_MEM);
		key = ngetchx();
	}
	initials[game_mode][place][0] = letters[last_initials[0]];
	initials[game_mode][place][1] = letters[last_initials[1]];
	initials[game_mode][place][2] = letters[last_initials[2]];
	hiscores[game_mode][place] = score;
}

void crash_screen(){

	FontSetSys(F_6x8);
	
	for (i = 15; i<76; i++) {
		FastDrawHLine(LCD_MEM, 0, 159, i, A_REVERSE);		
	}

	FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 72, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 73, A_NORMAL);
	
	DrawStr(40, 25, " = CRASH!! = ", A_XOR);
	DrawStr(30, 35, "Score:", A_XOR);
	DrawStr(68, 35, IntToStr(score), A_XOR);
	DrawStr(50, 45, "Miles:", A_XOR);
	if (miles == 0) {
		DrawStr(88, 45, "<1 (Doh!)", A_XOR);
	} else {
		DrawStr(88, 45, IntToStr(miles), A_XOR);
	}
	if (score > hiscores[game_mode][9]) {
		DrawStr(30, 55, "NEW HIGH SCORE!", A_XOR);
		got_hiscore = 1;
	}
	
	ticks = 0;
	while (ticks < 200) {};		// slight (~1/2 sec) pause so player doesn't accidentally skip the score screen
	GKeyFlush();
	

	ngetchx();

	for (i = 15; i<76; i++) {
		FastDrawHLine(LCD_MEM, 0, 159, i, A_REVERSE);		
	}
	FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 72, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 73, A_NORMAL);
	DrawStr(30, 25, "Drifts:", A_XOR);
	DrawStr(30, 35, "Crazy", A_XOR);
	DrawStr(30, 45, "Super", A_XOR);
	DrawStr(30, 55, "MEGA ", A_XOR);
	DrawStr(80, 35, "-", A_XOR);
	DrawStr(80, 45, "-", A_XOR);
	DrawStr(80, 55, "- ", A_XOR);
	DrawStr(100, 35, IntToStr(crazy_total), A_XOR);
	DrawStr(100, 45, IntToStr(super_total), A_XOR);
	DrawStr(100, 55, IntToStr(mega_total), A_XOR);
	
	ngetchx();
	
	if (dt4) {
		for (i = 15; i<76; i++) {
			FastDrawHLine(LCD_MEM, 0, 159, i, A_REVERSE);		
		}
		FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 72, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 73, A_NORMAL);
		DrawStr(30, 25, "Drifts (Continued):", A_XOR);
		DrawStr(30, 35, "Ultra", A_XOR);
		DrawStr(80, 35, "-", A_XOR);
		DrawStr(100, 35, IntToStr(dt4), A_XOR);
		if (dt5) {
			DrawStr(30, 45, "Hyper", A_XOR);
			DrawStr(80, 45, "-", A_XOR);
			DrawStr(100, 45, IntToStr(dt5), A_XOR);
		}
		if (dt6) {
			DrawStr(30, 55, "Supreme", A_XOR);
			DrawStr(80, 55, "- ", A_XOR);
			DrawStr(100, 55, IntToStr(dt6), A_XOR);
		}
		ngetchx();
	}

	if (dt7) {
		for (i = 15; i<76; i++) {
			FastDrawHLine(LCD_MEM, 0, 159, i, A_REVERSE);		
		}
		FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 72, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 73, A_NORMAL);
		DrawStr(30, 25, "Drifts (Continued):", A_XOR);
		DrawStr(30, 35, "Ultimate", A_XOR);
		DrawStr(80, 35, "-", A_XOR);
		DrawStr(100, 35, IntToStr(dt7), A_XOR);
		if (dt8) {
			DrawStr(30, 45, "Perfect", A_XOR);
			DrawStr(80, 45, "-", A_XOR);
			DrawStr(100, 45, IntToStr(dt8), A_XOR);
		}
		if (dt9) {
			DrawStr(30, 55, "Eternal", A_XOR);
			DrawStr(80, 55, "- ", A_XOR);
			DrawStr(100, 55, IntToStr(dt9), A_XOR);
		}
		ngetchx();
	}	
	if (got_hiscore == 1) { update_hiscores(); }
	if (game_mode == 0) { classic_scores(); } else { combo_scores(); }
}

void view_high_scores() {
	classic_scores();
	combo_scores();
}

void instructions() {
}

int main_menu() {
	ClrScr();
	
	// print the intro text
	FontSetSys(F_8x10);
	FastDrawHLine(LCD_MEM, 0, 159, 3, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 4, A_NORMAL);
	DrawStr(30,  7, "Drifter 3.0a", A_NORMAL);
	DrawStr(14, 18, "by Eric Burgess", A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 29, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 30, A_NORMAL);
	
	// Draw the menu choices on the screen
	FontSetSys(F_6x8);
	DrawStr(30, 39, "Start Game!", A_NORMAL);
	DrawStr(30, 51, "Instructions", A_NORMAL);
	DrawStr(30, 63, "Game Options", A_NORMAL);
	DrawStr(30, 75, "High Scores", A_NORMAL);
	DrawStr(30, 87, "Quit", A_NORMAL);
	
	return get_menu_choice(30, 39, 5, 12, 0);
}

void wait_for_key() {
	int savefont;
	savefont = FontGetSys();
	FontSetSys(F_4x6);
	DrawStr(70, 94, "Press any key...", A_NORMAL);
	FontSetSys(savefont);
	ngetchx();
}

void instructions_basic() {
	ClrScr();
	FontSetSys(F_8x10);
	FastDrawHLine(LCD_MEM, 0, 159, 3, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 4, A_NORMAL);
	DrawStr(56,  7, "Basics", A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
	FontSetSys(F_6x8);
	DrawStr(0, 21, "Guide your ship through a ", A_NORMAL);
	DrawStr(0, 31, "twisting tunnel using the ", A_NORMAL);
	DrawStr(0, 41, "RIGHT and LEFT ARROW keys.", A_NORMAL);
	DrawStr(0, 51, "If you touch a wall, you  ", A_NORMAL);
	DrawStr(0, 61, "will crash and the game ", A_NORMAL);
	DrawStr(0, 71, "will end.", A_NORMAL);
	wait_for_key();
	ClrScr();
	FontSetSys(F_6x8);
	DrawStr(0, 10, "Your ship has inertia, and", A_NORMAL);
	DrawStr(0, 20, "will continue to drift if ", A_NORMAL);
	DrawStr(0, 30, "you release the arrow keys.", A_NORMAL);
	DrawStr(0, 40, "With careful piloting, you", A_NORMAL);
	DrawStr(0, 50, "can drift for long ", A_NORMAL);
	DrawStr(0, 60, "distances without hitting ", A_NORMAL);
	DrawStr(0, 70, "a wall.  This is the key  ", A_NORMAL);
	DrawStr(0, 80, "to getting huge scores!", A_NORMAL);
	wait_for_key();
}

void instructions_drifts() {
	ClrScr();
	FontSetSys(F_8x10);
	FastDrawHLine(LCD_MEM, 0, 159, 3, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 4, A_NORMAL);
	DrawStr(56,  7, "Drifts", A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
	FontSetSys(F_6x8);
	DrawStr(0, 21, "A \"drift\" begins as soon", A_NORMAL);
	DrawStr(0, 31, "as you resease the arrow  ", A_NORMAL);
	DrawStr(0, 41, "keys, and lasts until the ", A_NORMAL);
	DrawStr(0, 51, "next time you press one.  ", A_NORMAL);
	DrawStr(0, 61, "Obviously, you won't have ", A_NORMAL);
	DrawStr(0, 71, "control of the ship during", A_NORMAL);
	DrawStr(0, 81, "this time, so be careful. ", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "If you get a long enough  ", A_NORMAL);
	DrawStr(0, 20, "drift, you will see a \"C\"", A_NORMAL);
	DrawStr(0, 30, "icon appear on the left   ", A_NORMAL);
	DrawStr(0, 40, "edge of the screen.  You  ", A_NORMAL);
	DrawStr(0, 50, "have just performed a     ", A_NORMAL);
	DrawStr(0, 60, "Crazy drift!  The bonus   ", A_NORMAL);
	DrawStr(0, 70, "for a Crazy drift is 10%  ", A_NORMAL);
	DrawStr(0, 80, "of your current score.    ", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "If you can continue the   ", A_NORMAL);
	DrawStr(0, 20, "drift until the \"C\" icon", A_NORMAL);
	DrawStr(0, 30, "reaches the bottom of the ", A_NORMAL);
	DrawStr(0, 40, "screen, then an \"S\" icon", A_NORMAL);
	DrawStr(0, 50, "will appear. (Super drift)", A_NORMAL);
	DrawStr(0, 60, "Super drifts are worth 25%", A_NORMAL);
	DrawStr(0, 70, "of your current score.    ", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "After Super drift comes   ", A_NORMAL);
	DrawStr(0, 20, "the MEGA drift, which is  ", A_NORMAL);
	DrawStr(0, 30, "worth a whopping 100% of  ", A_NORMAL);
	DrawStr(0, 40, "the current score.  Even  ", A_NORMAL);
	DrawStr(0, 50, "longer drifts are possible,", A_NORMAL);
	DrawStr(0, 60, "although they are rare.   ", A_NORMAL);
	wait_for_key();
}

void instructions_combo() {
	ClrScr();
	FontSetSys(F_8x10);
	FastDrawHLine(LCD_MEM, 0, 159, 3, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 4, A_NORMAL);
	DrawStr(40,  7, "Combo Mode", A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
	FontSetSys(F_6x8);
	DrawStr(0, 21, "In combo mode, you don't  ", A_NORMAL);
	DrawStr(0, 31, "gain any points for moving", A_NORMAL);
	DrawStr(0, 41, "forward.  Instead, the    ", A_NORMAL);
	DrawStr(0, 51, "only way to increase your ", A_NORMAL);
	DrawStr(0, 61, "score is with drifts.     ", A_NORMAL);
	DrawStr(0, 71, "Instead of multiplying the", A_NORMAL);
	DrawStr(0, 81, "score, drifts are worth...", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "a fixed number of points: ", A_NORMAL);
	DrawStr(0, 20, "Crazy:  100    Ultra:  500", A_NORMAL);
	DrawStr(0, 30, "Super:  200    Hyper:  800", A_NORMAL);
	DrawStr(0, 40, "MEGA :  300  Supreme: 1300", A_NORMAL);
	DrawStr(0, 50, "and so forth.  Obviously, ", A_NORMAL);
	DrawStr(0, 60, "points are harder to score", A_NORMAL);
	DrawStr(0, 70, "in Combo Mode compared to ", A_NORMAL);
	DrawStr(0, 80, "Classic Mode.", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "However, you do have one  ", A_NORMAL);
	DrawStr(0, 20, "new trick at your disposal", A_NORMAL);
	DrawStr(0, 30, "in this mode: the combo.  ", A_NORMAL);
	DrawStr(0, 40, "When you link drifts into ", A_NORMAL);
	DrawStr(0, 50, "combos, their point values", A_NORMAL);
	DrawStr(0, 60, "are increased by their    ", A_NORMAL);
	DrawStr(0, 70, "position within the combo.", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "For instance, if you link ", A_NORMAL);
	DrawStr(0, 20, "two Crazy drifts, then the", A_NORMAL);
	DrawStr(0, 30, "second is worth double, or", A_NORMAL);
	DrawStr(0, 40, "200.  The third drift is  ", A_NORMAL);
	DrawStr(0, 50, "quadrupled, and the fourth", A_NORMAL);
	DrawStr(0, 60, "would be worth eight times", A_NORMAL);
	DrawStr(0, 70, "normal.  If you could get ", A_NORMAL);
	DrawStr(0, 80, "a 10th, it would be 512x! ", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "Linking drifts together in", A_NORMAL);
	DrawStr(0, 20, "combos requires skill.  A ", A_NORMAL);
	DrawStr(0, 30, "combo is a set of drifts  ", A_NORMAL);
	DrawStr(0, 40, "with only ONE adjustment  ", A_NORMAL);
	DrawStr(0, 50, "between each drift.  That ", A_NORMAL);
	DrawStr(0, 60, "is, you only get one press", A_NORMAL);
	DrawStr(0, 70, "of the arrow key to set up", A_NORMAL);
	DrawStr(0, 80, "the next drift.           ", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "An indicator at the bottom", A_NORMAL);
	DrawStr(0, 20, "of the screen will show   ", A_NORMAL);
	DrawStr(0, 30, "when a combo is performed,", A_NORMAL);
	DrawStr(0, 40, "along with the length of  ", A_NORMAL);
	DrawStr(0, 50, "the combo so far.  Long   ", A_NORMAL);
	DrawStr(0, 60, "drifts at the end of long ", A_NORMAL);
	DrawStr(0, 70, "combos are the key to     ", A_NORMAL);
	DrawStr(0, 80, "enormous scores!", A_NORMAL);
	wait_for_key();
}

void instructions_classic() {
	ClrScr();
	FontSetSys(F_8x10);
	FastDrawHLine(LCD_MEM, 0, 159, 3, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 4, A_NORMAL);
	DrawStr(36,  7, "Classic Mode", A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
	FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
	FontSetSys(F_6x8);
	DrawStr(0, 21, "Classic Mode is the same  ", A_NORMAL);
	DrawStr(0, 31, "as previous versions of   ", A_NORMAL);
	DrawStr(0, 41, "Drifter.  For each pixel  ", A_NORMAL);
	DrawStr(0, 51, "of forward progress, you  ", A_NORMAL);
	DrawStr(0, 61, "earn points equal to the  ", A_NORMAL);
	DrawStr(0, 71, "mile you are on. (EX: 8   ", A_NORMAL);
	DrawStr(0, 81, "pts per pixel on mile 8)  ", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "Drift bonuses are a % of  ", A_NORMAL);
	DrawStr(0, 20, "your score at the time:   ", A_NORMAL);
	DrawStr(0, 30, "   Crazy : + 10%          ", A_NORMAL);
	DrawStr(0, 40, "   Super : + 25%          ", A_NORMAL);
	DrawStr(0, 50, "    MEGA : +100% (Double!)", A_NORMAL);
	DrawStr(0, 60, "Drifts beyond MEGA are all", A_NORMAL);
	DrawStr(0, 70, "worth +100%.              ", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "The bonuses are cumulative.", A_NORMAL);
	DrawStr(0, 20, "This means that when you  ", A_NORMAL);
	DrawStr(0, 30, "do a MEGA drift, you also ", A_NORMAL);
	DrawStr(0, 40, "get the Crazy and Super.  ", A_NORMAL);
	DrawStr(0, 50, "The total bonus is:       ", A_NORMAL);
	DrawStr(0, 60, "1.1 x 1.25 x 2.0 = 2.75   ", A_NORMAL);
	DrawStr(0, 70, "So, a MEGA drift nearly   ", A_NORMAL);
	DrawStr(0, 80, "triples your score!       ", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "Getting drifts late in the", A_NORMAL);
	DrawStr(0, 20, "game, when your score is  ", A_NORMAL);
	DrawStr(0, 30, "already high, will earn   ", A_NORMAL);
	DrawStr(0, 40, "giant bonuses.  Likewise, ", A_NORMAL);
	DrawStr(0, 50, "even a MEGA drift right at", A_NORMAL);
	DrawStr(0, 60, "the beginning won't do    ", A_NORMAL);
	DrawStr(0, 70, "very much for your score. ", A_NORMAL);
	DrawStr(0, 80, "(2.75 x 300 = not much).  ", A_NORMAL);
	wait_for_key();
	ClrScr();
	DrawStr(0, 10, "Unfortunately, pulling off", A_NORMAL);
	DrawStr(0, 20, "drifts becomes harder the ", A_NORMAL);
	DrawStr(0, 30, "farther you go.  Every two", A_NORMAL);
	DrawStr(0, 40, "miles, the tunnel gets    ", A_NORMAL);
	DrawStr(0, 50, "narrower by one pixel. But", A_NORMAL);
	DrawStr(0, 60, "if you can get that MEGA  ", A_NORMAL);
	DrawStr(0, 70, "on mile 23, just watch    ", A_NORMAL);
	DrawStr(0, 80, "your score explode!       ", A_NORMAL);
	wait_for_key();
}

void instruction_menu() {
	int choice;
	
	choice = 0;
	while (choice != 4) {
		ClrScr();
		FontSetSys(F_8x10);
		FastDrawHLine(LCD_MEM, 0, 159, 3, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 4, A_NORMAL);
		DrawStr(30,  7, "Instructions", A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
		FontSetSys(F_6x8);
		DrawStr(0, 21, "Choose a topic:", A_NORMAL);
		DrawStr(30, 33, "Basics", A_NORMAL);
		DrawStr(30, 45, "Drifts", A_NORMAL);
		DrawStr(30, 57, "Classic Mode", A_NORMAL);
		DrawStr(30, 69, "Combo Mode", A_NORMAL);
		DrawStr(30, 81, "Return to Main", A_NORMAL);
		choice = get_menu_choice(30, 33, 5, 12, choice);
		if (choice == 0) { instructions_basic(); }
		if (choice == 1) { instructions_drifts(); }
		if (choice == 2) { instructions_classic(); }
		if (choice == 3) { instructions_combo(); }
	}
}

void options_menu() {

	int choice;

	choice = 0;
	while (choice != 2) {
		ClrScr();
		FontSetSys(F_8x10);
		FastDrawHLine(LCD_MEM, 0, 159, 3, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 4, A_NORMAL);
		DrawStr(30,  7, "Game Options", A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 17, A_NORMAL);
		FastDrawHLine(LCD_MEM, 0, 159, 18, A_NORMAL);
		FontSetSys(F_6x8);
		DrawStr(30, 33, ((game_mode == 0) ? "Game Mode: Classic" : "Game Mode: Combo"), A_NORMAL);
		switch(game_speed) {
			case 3:
				DrawStr(30, 45, "Speed: Frantic!", A_NORMAL);
				break;
			case 4:
				DrawStr(30, 45, "Speed: Normal", A_NORMAL);
				break;
			case 5:
				DrawStr(30, 45, "Speed: Sedate", A_NORMAL);
				break;
			case 6:
				DrawStr(30, 45, "Speed: Molasses", A_NORMAL);
				break;
		}
		DrawStr(30, 57, "Return to Main", A_NORMAL);
		choice = get_menu_choice(30, 33, 3, 12, choice);
		if (choice == 0) { game_mode = ((game_mode == 0) ? 1 : 0); }
		if (choice == 1) { 
			--game_speed;
			if (game_speed == 2) { game_speed = 6; }
		}
	}
}

//*******************************************************************************************************
//********************   GAME ROUTINES  ***************************************************************** 
//*******************************************************************************************************

void move(int t) {
   
    width = rwall - lwall;
    if (fuel_used) {
		drift = 0;
		crazy = super = mega = ultra = hyper = supreme = ultimate = perfect = eternal = 0;
		if (last_fuel_used == 0) { 
			thrusted ++; 
			if (thrusted > 1) {
				combo = 0;
			}
		}
	} else {
	    drift += 1;
	}
    
    if (game_mode == 0) { 
	    score += (miles + 1); 
	    combo = 1;
	}	

    if ((drift > crazy_drift) && (crazy == 0)) {
    	crazy = 1;
    	thrusted = 0;
    	combo++;
    	crazy_total++;
    	if (game_mode == 0){
	    	score += (0.1 * score);
	    } else {
	    	score += (50 * (2 << (combo-1)));
	    }
    	Sprite16_AND(0, 0, 12, csprite, LCD_MEM);
    	strcpy(bonus_text, "Crazy Drift!");
    	bonus_time = 100;
    }
    
    if ((drift > super_drift) && (super == 0)) {
    	super = 1;
    	super_total++;
    	if (game_mode == 0){
	    	score += (0.25 * score);
	    } else {
	    	score += (100 * (2 << (combo-1)));
	    }
    	Sprite16_AND(0, 0, 12, ssprite, LCD_MEM);
    	strcpy(bonus_text, "Super Drift!");
    	bonus_time = 100;
    }

    if ((drift > mega_drift) && (mega == 0)) {
    	mega = 1;
    	mega_total++;
    	if (game_mode == 0){
	    	score += (score);
	    } else {
	    	score += (150 * (2 << (combo-1)));
	    }
    	Sprite16_AND(0, 0, 12, msprite, LCD_MEM);
    	strcpy(bonus_text, "MEGA Drift!");
    	bonus_time = 100;
    }
    
    if ((drift > mega_drift + 90) && (ultra == 0)) {
    	ultra = 1;
    	dt4++;
    	if (game_mode == 0){
	    	score += (score);
	    } else {
	    	score += (250 * (2 << (combo-1)));
	    }
    	Sprite16_AND(0, 0, 12, dsprite, LCD_MEM);
    	strcpy(bonus_text, "-Ultra Drift!-");
    	bonus_time = 100;
    }

    if ((drift > mega_drift + 180) && (hyper == 0)) {
    	hyper = 1;
    	dt5++;
    	if (game_mode == 0){
	    	score += (score);
	    } else {
	    	score += (400 * (2 << (combo-1)));
	    }
    	Sprite16_AND(0, 0, 12, dsprite, LCD_MEM);
    	strcpy(bonus_text, "-Hyper Drift!-");
    	bonus_time = 100;
    }

    if ((drift > mega_drift + 270) && (supreme == 0)) {
    	supreme = 1;
    	dt6++;
    	if (game_mode == 0){
	    	score += (score);
	    } else {
	    	score += (650 * (2 << (combo-1)));
	    }
    	Sprite16_AND(0, 0, 12, dsprite, LCD_MEM);
    	strcpy(bonus_text, "-Supreme Drift!-");
    	bonus_time = 100;
    }

    if ((drift > mega_drift + 360) && (ultimate == 0)) {
    	ultimate = 1;
    	dt7++;
    	if (game_mode == 0){
	    	score += (score);
	    } else {
	    	score += (1050 * (2 << (combo-1)));
	    }
    	Sprite16_AND(0, 0, 12, dsprite, LCD_MEM);
    	strcpy(bonus_text, "**Ultimate Drift!**");
    	bonus_time = 100;
    }

    if ((drift > mega_drift + 450) && (perfect == 0)) {
    	perfect = 1;
    	dt8++;
    	if (game_mode == 0){
	    	score += (score);
	    } else {
	    	score += (1700 * (2 << (combo-1)));
	    }
    	Sprite16_AND(0, 0, 12, dsprite, LCD_MEM);
    	strcpy(bonus_text, "**Perfect Drift!**");
    	bonus_time = 100;
    }

    if ((drift > mega_drift + 540) && (eternal == 0)) {
    	eternal = 1;
    	dt9++;
    	if (game_mode == 0){
	    	score += (score);
	    } else {
	    	score += (2750 * (2 << (combo-1)));
	    }
    	Sprite16_AND(0, 0, 12, dsprite, LCD_MEM);
    	strcpy(bonus_text, "**Eternal Drift!**");
    	bonus_time = 100;
    }

    rchange = lchange = 0;
    
	if (width < min_width) {
		if (random(2) == 0) {
			rsign = 1;
			rwall++;
		} else {
			lsign = -1;
			lwall--;
		}
	}	
	
	if (width > max_width) {
		if (random(2) == 0) {
			rsign = -1;
			rwall--;
		} else {
			lsign = 1;
			lwall++;
		}
	}	

	if (random(((game_mode == 0) ? 30 : 75)) == 0){
		if (random(2) == 0) {
			rchange = 1;
		} else {
			lchange = 1;
		}
	}
	
	if (rslope * lslope == 1){
		if (rslope == 1) {
			lslope *= 2;
		} else {
			rslope *= 2;
		}
	}
	
	if (rchange == 1) {
		rslope = 11 - (random(5) + 1 + 5);
		rsign = 1;
		if (random(2) == 0) {rsign = -1;}
	}
	
	if (lchange == 1) {
		lslope = 11 - (random(5) + 1 + 5);
		lsign = 1;
		if (random(2) == 0) {lsign = -1;}
	}

	if (rwall > 138) {rsign = -1;}
	if (lwall < 22)  {lsign = 1;}

	if (time < 200) {
		if (rslope < 2) {rslope = 2;}
		if (lslope < 2) {lslope = 2;}
	}

	if ((t % lslope) == 0){
		lwall += lsign;
	}
	if ((t % rslope) == 0){
		rwall += rsign;
	}
	
	// move the screen down one pixel
	Sprite8_XOR ((int)old_xpos, 80, 12, ship, LCD_MEM);
    ScrollDown160(LCD_MEM, 94);
	Sprite8_XOR ((int)xpos, 80, 12, ship, LCD_MEM);

	// draw a new topmost row
	FastDrawHLine(LCD_MEM, 0, 159, 0, A_REVERSE);
	if (((t % 500) > 2) || (t < 100)) {	// leave a blank line each mile
		FastDrawHLine(LCD_MEM, 0, lwall, 0, A_NORMAL);
		FastDrawHLine(LCD_MEM, rwall, 159, 0, A_NORMAL);
	} else {
		if ((t % 500) == 0) {
			if ((miles % 2) == 0) {
				min_width--;
				max_width--;
			}
		}
	}
	if (((t % 500) == 82) && (t > 100)) { miles++; }
	
	// clear out the one line of the status area that is overwritten when we scroll the top part of the screen down:
	FastDrawHLine(LCD_MEM, 0, 159, 94, A_REVERSE);
	// display the score
	if ((t % score_update_freq) == 0) {
		score_text[0] = '0' + miles / 10;
		score_text[1] = '0' + miles % 10;
		score_text[2] = ':';
		strcpy(score_text + 3, IntToStr(score));	
		DrawStr(0, 95, score_text, A_REPLACE);
	}
	// display bonus text
/*	DrawStr(100, 95, bonus_text, A_REPLACE);
	if (bonus_time-- < 0) {
		strcpy(bonus_text, "\0");
	}
*/	
	if (combo != last_combo) {
		if (combo > 1) {
			combo_text[0] = '0' + (combo / 10);
			combo_text[1] = '0' + (combo % 10);
			DrawStr(60, 95, combo_text, A_REPLACE);
		} else {
			DrawStr(60, 95, "                     ", A_REPLACE);
		}
	}
}


void start_game() { 
// begin the game
	ClrScr();	
	new_game();
	
	Sprite8_XOR ((int)xpos, 80, 12, ship, LCD_MEM);
	
	ticks = 0;
	FontSetSys(F_8x10);
	DrawStr(60, 28, "Ready...", A_REPLACE);
	while (ticks < 100) {}
	ticks = 0;
	DrawStr(60, 28, "        ", A_REPLACE);
	while (ticks < 100) {}
	ticks = 0;
	DrawStr(60, 28, " Set... ", A_REPLACE);
	while (ticks < 100) {}
	ticks = 0;
	DrawStr(60, 28, "        ", A_REPLACE);
	while (ticks < 100) {}
	ticks = 0;
	DrawStr(60, 28, "  GO!   ", A_REPLACE);
	while (ticks < 100) {}
	ticks = 0;
	DrawStr(60, 28, "        ", A_REPLACE);
	
	font = FontGetSys();
	FontSetSys(F_4x6);

	crashed = 0;
	while (crashed == 0) {
		last_fuel_used = fuel_used;
		fuel_used = 0;	
		if (OSGetStatKeys() == ARROW_RIGHT) {
			fuel_used = 1;
			if (velocity <= 0.95) {
				velocity += 0.05;
			}
		}
		if (OSGetStatKeys() == ARROW_LEFT) {
			fuel_used = 1;
			if (velocity >= -0.95) {
				velocity -= 0.05;
			}
		}
		
		xpos += velocity;

		if (xpos < 2.0) {xpos = 2.0;}
		if (xpos > 150.0) {xpos = 150.0;}

		time += 1;
		
		// even out the time taken for each frame
		while (ticks < game_speed) {}
		ticks = 0;
		
		move(time);

		old_xpos = xpos;
		last_combo = combo;
		
		// see if we have crashed
		if (GetPix(xpos, 79) || GetPix(xpos + 7, 79) || GetPix(xpos, 92) || GetPix(xpos + 7, 92)) { crashed = 1;}
	}
	
	crash_screen();
}

//*******************************************************************************************************
//********************  MAIN FUNCTION  ****************************************************************** 
//*******************************************************************************************************

void _main(void)
{
	// trap the auto-int 1 (395/sec)
	Old_Int_1 = GetIntVec(AUTO_INT_1);
	SetIntVec(AUTO_INT_1, My_Int_1);
	
	// seed the random number generator
	randomize();
	
	action = 0;
	while (action != 4) {		// player hasn't chosen "quit"
		action = main_menu();
		if (action == 0) { start_game(); 		}
		if (action == 1) { instruction_menu(); 	}
		if (action == 2) { options_menu(); 		}
		if (action == 3) { view_high_scores(); 	}
	}
	
	// restore auto-int 1
	SetIntVec (AUTO_INT_1, Old_Int_1);
}






















