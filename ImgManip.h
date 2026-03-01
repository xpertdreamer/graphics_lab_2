#ifndef IMGMANIP_H
#define IMGMANIP_H

#include "framework.h"

#define MAX_PHASES 8
#define TIMER_ID 1
#define SPRITE_WIDTH 100
#define SPRITE_HEIHT 100
#define MOVE_STEP 5

/*
* Bitmap for background image
* It need to be loaded as 1-bit .bmp
* That means an image would have only black-white color
*/
HBITMAP hBackgroundBM = nullptr;

/*
* Bitmap array for sprite images
* Similar to background sprites need to be loaded as 1-bit .bmp file
* Now we have only 8 phases as defined
*/
HBITMAP hSpritePhases[MAX_PHASES];

/*
* Here another globals
* I think explanation need only for few:
*	direction: 1 = right, 0 = left
*	animationSpeed: milliseconds between frames
*	isAnimating flag: used inside WndProc to run and stop animation
*/
int currentPhase = 0;
int spriteX = 50;
int spriteY = 300;
int direction = 1;
BOOL isAnimating = FALSE;
int animationSpeed = 75;

/*
* This functions created to be a simple image loader
* It accept no parameters: that means we operate with global variable hBackgroundBM
* Also, if image not found it throws an error and destroy created window
*/
BOOL load_background_bitmap() {
	hBackgroundBM = static_cast<HBITMAP>(LoadImage(nullptr, L"Resource/background.bmp", 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_MONOCHROME));
	if (hBackgroundBM == nullptr) {
		MessageBox(NULL, L"Failed to load background.bmp", L"Error", MB_OK);
		return FALSE;
	}
	return TRUE;
}

/*
* Similar to previous for background
* Sprites is white on black here
*/
BOOL load_sprite_phases() {
	WCHAR filename[100];
	BOOL success = TRUE;

	for (int i = 0; i < MAX_PHASES; i++) {
		swprintf(filename, 100, L"Resource/Phases/phase_%d.bmp", i + 1);
		hSpritePhases[i] = static_cast<HBITMAP>(LoadImage(nullptr, filename,
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_MONOCHROME));

		if (hSpritePhases[i] == nullptr) {
			WCHAR errorMsg[100];
			swprintf(errorMsg, 100, L"Failed to load: %s", filename);
			MessageBox(nullptr, errorMsg, L"Error", MB_OK);
			success = FALSE;
		}
	}
	return success;
}

/*
* Util func
* Cleans up bitmap with images
*/
void cleanup_bitmaps() {
	if (hBackgroundBM != nullptr) {
		DeleteObject(hBackgroundBM);
		hBackgroundBM = nullptr;
	}

	for (int i = 0; i < MAX_PHASES; i++) {
		if (hSpritePhases[i] != nullptr) {
			DeleteObject(hSpritePhases[i]);
			hSpritePhases[i] = nullptr;
		}
	}
}

/*
* This func draws sprite
* using XOR mode, or SRCINVERT
* using BitBlt procedure
*/
void draw_sprite(HDC hdc, int x, int y, int phase, int dir) {
	if (phase >= 0 && phase < MAX_PHASES && hSpritePhases[phase] != nullptr) {
		HDC tempDC = CreateCompatibleDC(hdc);
		SelectObject(tempDC, hSpritePhases[phase]);
		if (dir == 1)
			BitBlt(hdc, x, y, SPRITE_WIDTH, SPRITE_HEIHT, tempDC, 0, 0, SRCINVERT);
		else
			StretchBlt(hdc, x + SPRITE_WIDTH, y, -SPRITE_WIDTH, SPRITE_HEIHT,
				tempDC, 0, 0, SPRITE_WIDTH, SPRITE_HEIHT, SRCINVERT);
		DeleteDC(tempDC);
	}
}

/*
* Here we can just use second XOR to drawing to restore background
*/
void erase_sprite(HDC hdc, int x, int y, int phase, int dir) {
	draw_sprite(hdc, x, y, phase, dir);
}

/*
* This core animation implementation 
* called by a timer
* It updating graphical object and calculates direction
*/
void update_animation(HWND hWnd) {
	HDC hdc = GetDC(hWnd);

	erase_sprite(hdc, spriteX, spriteY, currentPhase, direction);
	spriteX += MOVE_STEP * direction;

	/*
	* Here we need to check window boundaries
	* and bounce sprite if needed
	*/
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	if (spriteX <= 0 || spriteX + SPRITE_WIDTH >= clientRect.right) {
		direction *= -1;
		if (spriteX <= 0) spriteX = 0;
		if (spriteX + SPRITE_WIDTH >= clientRect.right)
			spriteX = clientRect.right - SPRITE_WIDTH;
	}

	currentPhase = (currentPhase + 1) % MAX_PHASES;

	//if (currentPhase + 1 == 8)
	//	Beep(1000, 100);

	draw_sprite(hdc, spriteX, spriteY, currentPhase, direction);
	ReleaseDC(hWnd, hdc);
}

#endif // !IMGMANIP_H