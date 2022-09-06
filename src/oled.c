/*
 * oled.c
 *
 *  Created on: 4/09/2022
 *      Author: Rohan
 */

#include "oled.h"

#include "r_typedefs.h"
#include "oled_low_level.h"
#include "font8x8_basic.h"
#include "rspi.h"
#include "oled_low_level_all_cpus.h"
#include "fonts.h"
#include "rspi_iodefine.h"
#include "mtu.h"

int workingAnimationCount;
char const* workingAnimationText; // NULL means animation not active

uint8_t oledMainImage[OLED_MAIN_HEIGHT_PIXELS >> 3][OLED_MAIN_WIDTH_PIXELS];
uint8_t oledMainConsoleImage[CONSOLE_IMAGE_NUM_ROWS][OLED_MAIN_WIDTH_PIXELS];
uint8_t oledMainPopupImage[OLED_MAIN_HEIGHT_PIXELS >> 3][OLED_MAIN_WIDTH_PIXELS];


void delayMS(uint32_t ms) {
	uint16_t startTime = *TCNT[TIMER_SYSTEM_SLOW];
	uint16_t stopTime = startTime + msToSlowTimerCount(ms);
	while ((uint16_t)(*TCNT[TIMER_SYSTEM_SLOW] - stopTime) >= 8);
}





void monitorInputFromPICForever() {
	while (1) {
		monitorInputFromPIC();
	}
}





// Clears area *inclusive* of maxX, but not maxY? Stupid.
void clearAreaExact(int minX, int minY, int maxX, int maxY, uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
	int firstRow = minY >> 3;
	int lastRow = maxY >> 3;

	int firstCompleteRow = firstRow;
	int lastCompleteRow = lastRow;

	int lastRowPixelWithin = maxY & 7;
	bool_t willDoLastRow = (lastRowPixelWithin != 7);
	uint8_t lastRowMask = 255 << (lastRowPixelWithin + 1);

	// First row
	int firstRowPixelWithin = minY & 7;
	if (firstRowPixelWithin) {
		firstCompleteRow++;
		uint8_t firstRowMask = ~(255 << firstRowPixelWithin);
		if (willDoLastRow && firstRow == lastRow) {
			firstRowMask &= lastRowMask;
		}
		int x;
		for (x = minX; x <= maxX; x++) {
			image[firstRow][x] &= firstRowMask;
		}

		if (firstRow == lastRow) return;
	}

	// Last row
	if (willDoLastRow) {
		lastCompleteRow--;
		int x;
		for (x = minX; x <= maxX; x++) {
			image[lastRow][x] &= lastRowMask;
		}
	}

	int row;
	for (row = firstCompleteRow; row <= lastCompleteRow; row++) {
		memset(&image[row][minX], 0, maxX - minX + 1);
	}
}

void moveAreaUpCrude(int minX, int minY, int maxX, int maxY, int delta, uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {

	int firstRow = minY >> 3;
	int lastRow = maxY >> 3;

	// First move any entire rows
	int deltaRows = delta >> 3;
	if (deltaRows) {
		delta &= 7;
		minY += delta; // There's a bit we can ignore here, potentially
		int firstRowHere = minY >> 3;
		lastRow -= deltaRows;
		int row;
		for (row = firstRow; row <= lastRow; row++) {
			memcpy(&image[row][minX], &image[row + deltaRows][minX], maxX - minX + 1);
		}
	}

	// Move final sub-row amount
	if (delta) {
		int x;
		for (x = minX; x <= maxX; x++) {
			uint8_t carry;

			int row;
			for (row = lastRow; row >= firstRow; row--) {
				uint8_t prevBitsHere = image[row][x];
				image[row][x] = (prevBitsHere >> delta) | (carry << (8 - delta));
				carry = prevBitsHere;
			}
		}
	}
}



void drawGraphic(uint8_t const* graphic, int startX, int startY, int width, uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
	int firstRowY = startY >> 3;
	int yOffset = startY & 7;
	uint8_t currentRowMask = (255 >> yOffset); // This is the mask for the bit we wish to *keep* of the existing canvas.

	// First row
	uint8_t* __restrict__ currentPos = &image[firstRowY][startX];
	uint8_t const* endPos = currentPos + width;
	uint8_t const* __restrict__ graphicPos = graphic;

	while (currentPos < endPos) {
		//*currentPos = (*currentPos & currentRowMask) | (*graphicPos << yOffset);
		*currentPos |= (*graphicPos << yOffset);
		currentPos++;
		graphicPos++;
	}

	if (!yOffset) return;

	// Second row
	currentPos = &image[firstRowY + 1][startX];
	endPos = currentPos + width;
	graphicPos = graphic;
	currentRowMask = ~currentRowMask;
	int yOffsetNegative = 8 - yOffset;

	while (currentPos < endPos) {
		//*currentPos = (*currentPos & currentRowMask) | (*graphicPos >> yOffsetNegative);
		*currentPos |= (*graphicPos >> yOffsetNegative);
		currentPos++;
		graphicPos++;
	}
}

void drawRectangle(int minX, int minY, int maxX, int maxY, uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
	drawVerticalLine(minX, minY, maxY, image);
	drawVerticalLine(maxX, minY, maxY, image);

	drawHorizontalLine(minY, minX + 1, maxX - 1, image);
	drawHorizontalLine(maxY, minX + 1, maxX - 1, image);
}


void drawVerticalLine(int pixelX, int startY, int endY, uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
	int firstRowY = startY >> 3;
	int lastRowY = endY >> 3;

	uint8_t firstRowMask = (255 << (startY & 7));
	uint8_t lastRowMask = (255 >> (7 - (endY & 7)));

	uint8_t* __restrict__ currentPos = &image[firstRowY][pixelX];

	if (firstRowY == lastRowY) {
		uint8_t mask = firstRowMask & lastRowMask;
		*currentPos |= mask;
	}

	else {
		uint8_t* const finalPos = &image[lastRowY][pixelX];

		// First row
		*currentPos |= firstRowMask;

		// Intermediate rows
		goto drawSolid;
		do {
			*currentPos = 255;
drawSolid:	currentPos += OLED_MAIN_WIDTH_PIXELS;
		} while (currentPos != finalPos);

		// Last row
		*currentPos |= lastRowMask;
	}
}


void drawHorizontalLine(int pixelY, int startX, int endX, uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
	uint8_t mask = 1 << (pixelY & 7);

	uint8_t* __restrict__ currentPos = &image[pixelY >> 3][startX];
	uint8_t* const lastPos = currentPos + (endX - startX);
	do {
		*currentPos |= mask;
		currentPos++;
	} while (currentPos <= lastPos);
}


void drawString(char const* string, int pixelX, int pixelY, uint8_t* image, int imageWidth, int textWidth, int textHeight, int scrollPos, int endX) {
	if (scrollPos) {
		int numCharsToChopOff = (uint16_t)scrollPos / (uint8_t)textWidth;
		if (numCharsToChopOff) {
			if (numCharsToChopOff >= strlen(string)) return;

			string += numCharsToChopOff;
			scrollPos -= numCharsToChopOff * textWidth;
		}
	}
	int originalX = pixelX;
	while (*string) {
		if (*string == '\n') {
			pixelX = originalX;
			pixelY += 9;
		}
		else {
			drawChar(*string, pixelX, pixelY, image, imageWidth, textWidth, textHeight, scrollPos, endX);
			pixelX += textWidth - scrollPos;
			if (pixelX >= endX) break;
			scrollPos = 0;
		}
		string++;
	}
}


void drawStringFixedLength(char const* string, int length, int pixelX, int pixelY, uint8_t* image, int imageWidth, int textWidth, int textHeight) {
	char const* const stopAt = string + length;
	while (string < stopAt) {
		drawChar(*string, pixelX, pixelY, image, imageWidth, textWidth, textHeight);
		pixelX += textWidth;
		if (pixelX + textWidth > OLED_MAIN_WIDTH_PIXELS) break;
		string++;
	}
}

void drawStringCentred(char const* string, int pixelY, uint8_t* image, int imageWidth, int textWidth, int textHeight, int centrePos) {
	int length = strlen(string);
	int pixelX = centrePos - ((textWidth * length) >> 1);
	drawStringFixedLength(string, length, pixelX, pixelY, image, imageWidth, textWidth, textHeight);
}

void drawStringAlignRight(char const* string, int pixelY, uint8_t* image, int imageWidth, int textWidth, int textHeight, int rightPos) {
	int length = strlen(string);
	int pixelX = rightPos - (textWidth * length);
	drawStringFixedLength(string, length, pixelX, pixelY, image, imageWidth, textWidth, textHeight);
}

void drawStringCentredShrinkIfNecessary(char const* string, int pixelY, uint8_t* image, int imageWidth, int textWidth, int textHeight) {
	int length = strlen(string);
	int maxTextWidth = (uint8_t)OLED_MAIN_WIDTH_PIXELS / (unsigned int)length;
	if (textWidth > maxTextWidth) {
		int newHeight = (unsigned int)(textHeight * maxTextWidth) / (unsigned int)textWidth;
		textWidth = maxTextWidth;

		int heightDiff = textHeight - newHeight;
		pixelY += heightDiff >> 1;
		textHeight = newHeight;
	}
	int pixelX = (imageWidth - textWidth * length) >> 1;
	drawStringFixedLength(string, length, pixelX, pixelY, image, imageWidth, textWidth, textHeight);
}


void drawChar(uint8_t theChar, int pixelX, int pixelY, uint8_t* image, int imageWidth, int textWidth, int textHeight, int scrollPos, int endX) {

#if !OLED_ALLOW_LOWER_CASE
	if (theChar >= 'a' && theChar <= 'z') theChar -= 32;
#endif

	lv_font_glyph_dsc_t const* descriptor;
	uint8_t const* font;
	int fontNativeHeight;

#if 0
	pixelY++;
	textHeight = 7;
	descriptor = font_apple_desc;
	font = font_apple;
	fontNativeHeight = 7;



	if (theChar > 'Z') return;
	int charIndex = theChar - 0x20;
	if (charIndex < 0) return;
	descriptor += charIndex;
	int numBytesPerRow = ((descriptor->w_px - 1) >> 3) + 1;
	int scaledFontWidth = (uint16_t)(descriptor->w_px * textHeight + (fontNativeHeight >> 1) - 1) / (uint8_t)fontNativeHeight;
	pixelX += (textWidth - scaledFontWidth) >> 1;
	if (pixelX < 0) {
		scrollPos += -pixelX;
		pixelX = 0;
	}

	int stopAtPixelX = pixelX + scaledFontWidth;
	if (stopAtPixelX > endX) stopAtPixelX = endX;

	int y;
	for (y = 0; y < textHeight; y++) {
		int oledLineY = pixelY >> 3;
		int oledPixelWithinLineY = pixelY & 7;

		int charY = y;//(uint16_t)(y * fontNativeHeight + (textHeight >> 1) - 1) / (uint8_t)textHeight;

		uint8_t const* charLine = &font[descriptor->glyph_index + charY * numBytesPerRow];

		uint8_t* outputLine					= &image[pixelX + (oledLineY * imageWidth)];
		uint8_t const* outputLineStopPoint	= &image[stopAtPixelX + (oledLineY * imageWidth)];

		int x;
		for (x = scrollPos; x < scaledFontWidth; x++) {
			if (outputLine >= outputLineStopPoint) break;

			int charX = x;//(uint16_t)(x * fontNativeHeight) / (uint8_t)textHeight;

			int isDotHere = (*(charLine + (charX >> 3)) >> (7 - (charX & 7))) & 1;

			*outputLine |= (isDotHere << oledPixelWithinLineY);

			outputLine++;
		}
		pixelY++;
	}


#else

	if (true || textHeight == 20) {
		/*
use20pxFont:
		fontNativeHeight = 20;
		descriptor = font_20px_desc;
		font = font_20px;
		*/
		pixelY++;
		textHeight = 7;
		descriptor = font_apple_desc;
		font = font_apple;
		fontNativeHeight = 7;
		//goto renderFont;

renderFont:
		if (theChar > 'Z') return;
		int charIndex = theChar - 0x20;
		if (charIndex < 0) return;
		descriptor += charIndex;
		int numBytesPerRow = ((descriptor->w_px - 1) >> 3) + 1;
		int scaledFontWidth = descriptor->w_px;//(uint16_t)(descriptor->w_px * textHeight + (fontNativeHeight >> 1) - 1) / (uint8_t)fontNativeHeight;
		pixelX += (textWidth - scaledFontWidth) >> 1;
		if (pixelX < 0) {
			scrollPos += -pixelX;
			pixelX = 0;
		}

		int stopAtPixelX = pixelX + scaledFontWidth;
		if (stopAtPixelX > endX) stopAtPixelX = endX;

		int y;
		for (y = 0; y < textHeight; y++) {
			int oledLineY = pixelY >> 3;
			int oledPixelWithinLineY = pixelY & 7;

			int charY = y;//(uint16_t)(y * fontNativeHeight + (textHeight >> 1) - 1) / (uint8_t)textHeight;
			if (charY < 0) charY = 0;
			else if (charY >= textHeight) charY = textHeight - 1;

			uint8_t const* charLine = &font[descriptor->glyph_index + charY * numBytesPerRow];

			uint8_t* outputLine					= &image[pixelX + (oledLineY * imageWidth)];
			uint8_t const* outputLineStopPoint	= &image[stopAtPixelX + (oledLineY * imageWidth)];

			int x;
			for (x = scrollPos; x < scaledFontWidth; x++) {
				if (outputLine >= outputLineStopPoint) break;

				int charX = x;//(uint16_t)(x * fontNativeHeight) / (uint8_t)textHeight;
				if (charX < 0) charX = 0;
				//else if (charX >= scaledFontWidth) charX = scaledFontWidth - 1;
				//else if (charX >= 100) charX = 99;

				int isDotHere = (*(charLine + (charX >> 3)) >> (7 - (charX & 7))) & 1;

				*outputLine |= (isDotHere << oledPixelWithinLineY);

				outputLine++;
			}
			pixelY++;
		}
	}

	/*
	else if (textHeight == 13) {
		descriptor = font_metric_bold_13px_desc;
		font = font_metric_bold_13px;
		fontNativeHeight = 13;
		goto renderFont;
	}

	else if (textHeight == 10) {
		textHeight = 9;
		descriptor = font_9px_desc;
		font = font_9px;
		fontNativeHeight = 9;
		goto renderFont;
	}
	else if (textHeight == 9) {
		pixelY++;

		goto use7pxFont;
	}
*/
	else if (textHeight == 7 || textHeight == 8) {

use7pxFont:
		//pixelY++;
		textHeight = 7;
		descriptor = font_apple_desc;
		font = font_apple;
		fontNativeHeight = 7;
		goto renderFont;
	}

	/*
	else if (false && textHeight < 9) {
		int outputY;
		for (outputY = 0; outputY < textHeight; outputY++) {
			int oledLineY = pixelY >> 3;
			int oledPixelWithinLineY = pixelY & 7;

			int charY = (uint16_t)(outputY << 3) / (uint8_t)textHeight;

			uint8_t charLine = font8x8_basic[theChar][charY];

			uint8_t* outputLine = &image[pixelX + (oledLineY * imageWidth)];
			uint8_t const* outputLineStopPoint = &image[((oledLineY + 1) * imageWidth)];

			int outputX;
			for (outputX = scrollPos; outputX < textWidth; outputX++) {
				if (outputLine >= outputLineStopPoint) break;

				int charX = (uint16_t)(outputX << 3) / (uint8_t)textWidth;

				int isDotHere = (charLine >> charX) & 1;

				*outputLine |= (isDotHere << oledPixelWithinLineY);

				outputLine++;
			}
			pixelY++;
		}
	}
	*/
	else {
		//goto use20pxFont;
	}
#endif
}


void drawScreenTitle(char const* title) {
	int extraY = (OLED_MAIN_HEIGHT_PIXELS == 64) ? 0 : 1;

	int startY = extraY + OLED_MAIN_TOPMOST_PIXEL;

	drawString(title, 0, startY, oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_TITLE_SPACING_X, TEXT_TITLE_SIZE_Y, 0, OLED_MAIN_WIDTH_PIXELS);
	drawHorizontalLine(extraY + 11 + OLED_MAIN_TOPMOST_PIXEL, 0, OLED_MAIN_WIDTH_PIXELS - 1, &oledMainImage[0]);
}


int oledPopupWidth = 0; // If 0, means popup isn't present / active.
int popupHeight;

int popupMinX;
int popupMaxX;
int popupMinY;
int popupMaxY;

void setupPopup(int width, int height) {
	if (height > OLED_MAIN_HEIGHT_PIXELS) height = OLED_MAIN_HEIGHT_PIXELS;

	oledPopupWidth = width;
	popupHeight = height;

	popupMinX = (OLED_MAIN_WIDTH_PIXELS - oledPopupWidth) >> 1;
	popupMaxX = OLED_MAIN_WIDTH_PIXELS - popupMinX;

	popupMinY = (OLED_MAIN_HEIGHT_PIXELS - popupHeight) >> 1;
	popupMaxY = OLED_MAIN_HEIGHT_PIXELS - popupMinY;

	if (popupMinY < 0) popupMinY = 0;
	if (popupMaxY > OLED_MAIN_HEIGHT_PIXELS - 1) popupMaxY = OLED_MAIN_HEIGHT_PIXELS - 1;

	// Clear the popup's area, not including the rectangle we're about to draw
	int popupFirstRow = (popupMinY + 1) >> 3;
	int popupLastRow = (popupMaxY - 1) >> 3;

	int row;
	for (row = popupFirstRow; row <= popupLastRow; row++) {
		memset(&oledMainPopupImage[row][popupMinX + 1], 0, popupMaxX - popupMinX - 1);
	}

	drawRectangle(popupMinX, popupMinY, popupMaxX, popupMaxY, oledMainPopupImage);
}



void displayPrompt(char const* message) {
	clearMainImage();
	//drawString(message, 0, OLED_MAIN_TOPMOST_PIXEL + TEXT_SIZE_Y_UPDATED, oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED, 0, OLED_MAIN_WIDTH_PIXELS);
	drawString(message, 0, OLED_MAIN_TOPMOST_PIXEL, oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED, 0, OLED_MAIN_WIDTH_PIXELS);
	sendMainImage();
}


#if 0
int consoleMaxX;
int consoleMinX = -1;


struct ConsoleItem {
	uint32_t timeoutTime;
	int16_t minY;
	int16_t maxY;
	bool_t cleared;
};

#define MAX_NUM_CONSOLE_ITEMS 10

struct ConsoleItem consoleItems[MAX_NUM_CONSOLE_ITEMS];
int numConsoleItems = 0;

void drawConsoleTopLine() {
	drawHorizontalLine(consoleItems[numConsoleItems - 1].minY - 1, consoleMinX + 1, consoleMaxX - 1, oledMainConsoleImage);
}

// Returns y position (minY)
int setupConsole(int height) {
	consoleMinX = 4;
	consoleMaxX = OLED_MAIN_WIDTH_PIXELS - consoleMinX;

	bool_t shouldRedrawTopLine = false;

	// If already some console items...
	if (numConsoleItems) {

		// If hit max num console items...
		if (numConsoleItems == MAX_NUM_CONSOLE_ITEMS) {
			numConsoleItems--;
			shouldRedrawTopLine = true;
		}

		// Shuffle existing console items along
		for (int i = numConsoleItems; i > 0; i--) {
			consoleItems[i] = consoleItems[i - 1];
		}

		// Place new item below existing ones
		consoleItems[0].minY = consoleItems[1].maxY + 1;
		consoleItems[0].maxY = consoleItems[0].minY + height;

		// If that's too low, we'll have to bump the other ones up immediately
		int howMuchTooLow = consoleItems[0].maxY - CONSOLE_IMAGE_HEIGHT + 1;
		if (howMuchTooLow > 0) {

			// Move their min and max values up
			for (int i = numConsoleItems; i >= 0; i--) { // numConsoleItems hasn't been updated yet - there's actually one more
				consoleItems[i].minY -= howMuchTooLow;
				// If at all offscreen, scrap that one
				if (consoleItems[i].minY < 1) {
					numConsoleItems = i - 1; // It's still going to get 1 added to it, below
					shouldRedrawTopLine = true;
				}

				consoleItems[i].maxY -= howMuchTooLow;
			}

			// Do the actual copying
			moveAreaUpCrude(consoleMinX, consoleItems[numConsoleItems].minY - 1, consoleMaxX, consoleItems[1].maxY + howMuchTooLow, howMuchTooLow, oledMainConsoleImage); // numConsoleItems hasn't been updated yet - there's actually one more
		}
	}

	// Or if no other items, easy
	else {
		shouldRedrawTopLine = true;
		consoleItems[0].minY = OLED_MAIN_HEIGHT_PIXELS;
		consoleItems[0].maxY = consoleItems[0].minY + height;
	}

	consoleItems[0].timeoutTime = AudioEngine::audioSampleTimer + 52000; // 1 and a bit seconds
	consoleItems[0].cleared = false;

	numConsoleItems++;

	// Clear the new console item's area
	clearAreaExact(consoleMinX, consoleItems[0].minY, consoleMaxX, consoleItems[0].maxY, oledMainConsoleImage);

	drawVerticalLine(consoleMinX, consoleItems[0].minY - 1, consoleItems[0].maxY, oledMainConsoleImage);
	drawVerticalLine(consoleMaxX, consoleItems[0].minY - 1, consoleItems[0].maxY, oledMainConsoleImage);

	if (shouldRedrawTopLine) drawConsoleTopLine();

	return consoleItems[0].minY;
}


void removePopup() {
	//if (!oledPopupWidth) return;

	oledPopupWidth = 0;
	workingAnimationText = NULL;
	if (!numConsoleItems && !sideScrollerDirection) uiTimerManager.unsetTimer(TIMER_DISPLAY);
	sendMainImage();
}

void copyRowWithMask(uint8_t destMask, uint8_t sourceRow[], uint8_t destRow[], int minX, int maxX) {
	uint8_t sourceMask = ~destMask;
	uint8_t* __restrict__ source = &sourceRow[minX];
	uint8_t* __restrict__ dest = &destRow[minX];

	uint8_t* const sourceStop = source + maxX - minX;
	do {
		*dest = (*dest & destMask) | (*source & sourceMask);
		dest++;
		source++;
	} while (source <= sourceStop);
}

void copyBackgroundAroundForeground(uint8_t backgroundImage[][OLED_MAIN_WIDTH_PIXELS], uint8_t foregroundImage[][OLED_MAIN_WIDTH_PIXELS], int minX, int minY, int maxX, int maxY) {

	// Copy everything above
	int firstRow = minY >> 3;
	int lastRow = maxY >> 3;
	if (firstRow) {
		memcpy(foregroundImage, backgroundImage, OLED_MAIN_WIDTH_PIXELS * firstRow);
	}

	// Partial row above
	int partialRowPixelsAbove = minY & 7;
	if (partialRowPixelsAbove) {
		uint8_t destMask = 255 << partialRowPixelsAbove;
		copyRowWithMask(destMask, backgroundImage[firstRow], foregroundImage[firstRow], minX, maxX);
	}


	// Copy stuff to left and right
	for (int row = firstRow; row <= lastRow; row++) {
		memcpy(foregroundImage[row], backgroundImage[row], minX);
		memcpy(&foregroundImage[row][maxX + 1], &backgroundImage[row][maxX + 1], OLED_MAIN_WIDTH_PIXELS - maxX - 1);
	}


	// Partial row below
	int partialRowPixelsBelow = 7 - (maxY & 7);
	if (partialRowPixelsBelow) {
		uint8_t destMask = 255 >> partialRowPixelsBelow;
		copyRowWithMask(destMask, backgroundImage[lastRow], foregroundImage[lastRow], minX, maxX);
	}

	// Copy everything below
	int numMoreRows = ((OLED_MAIN_HEIGHT_PIXELS - 1) >> 3) - lastRow;
	if (numMoreRows > 0) {
		memcpy(foregroundImage[lastRow + 1], backgroundImage[lastRow + 1], OLED_MAIN_WIDTH_PIXELS * numMoreRows);
	}
}




#define TEXT_MAX_NUM_LINES 8
struct TextLineBreakdown {
	char const* lines[TEXT_MAX_NUM_LINES];
	uint8_t lineLengths[TEXT_MAX_NUM_LINES];
	int numLines;
	int longestLineLength;
	int maxCharsPerLine;
};


void breakStringIntoLines(char const* text, struct TextLineBreakdown* textLineBreakdown) {
	textLineBreakdown->numLines = 0;
	textLineBreakdown->longestLineLength = 0;

	char const* lineStart = text;
	char const* wordStart = lineStart;

findNextSpace:
	char const* space = wordStart;
	while (true) {
		if (*space == '\n' || *space == ' ' || *space == 0) break;
		space++;
	}
	char const* lineEnd;

lookAtNextSpace:
	int lineLength = (int)space - (int)lineStart;

	// If line not too long yet
	if (lineLength <= textLineBreakdown->maxCharsPerLine) {
		lineEnd = space;
		if (lineLength > textLineBreakdown->longestLineLength) textLineBreakdown->longestLineLength = lineLength;

		// If end of whole line, or whole string...
		if (*space == '\n' || *space == 0) {
			textLineBreakdown->lines[textLineBreakdown->numLines] = lineStart;
			textLineBreakdown->lineLengths[textLineBreakdown->numLines] = lineLength;
			textLineBreakdown->numLines++;
			if (!*space || textLineBreakdown->numLines == TEXT_MAX_NUM_LINES) return;
			lineStart = lineEnd + 1;
		}

		wordStart = space + 1;
		goto findNextSpace;
	}
	else {
		textLineBreakdown->lines[textLineBreakdown->numLines] = lineStart;
		textLineBreakdown->lineLengths[textLineBreakdown->numLines] = (int)lineEnd - (int)lineStart;
		textLineBreakdown->numLines++;
		if (textLineBreakdown->numLines == TEXT_MAX_NUM_LINES) return;
		lineStart = lineEnd + 1;
		goto lookAtNextSpace;
	}
}




void popupText(char const* text, bool persistent) {

	struct TextLineBreakdown textLineBreakdown;
	textLineBreakdown.maxCharsPerLine = 12;

	breakStringIntoLines(text, &textLineBreakdown);


	int textWidth = textLineBreakdown.longestLineLength * TEXT_SPACING_X;
	int textHeight = textLineBreakdown.numLines * TEXT_SPACING_Y;

	int doubleMargin = 12;

	setupPopup(textWidth + doubleMargin, textHeight + doubleMargin);

	int textPixelY = (OLED_MAIN_HEIGHT_PIXELS - textHeight) >> 1;
	if (textPixelY < 0) textPixelY = 0;

	for (int l = 0; l < textLineBreakdown.numLines; l++) {
		int textPixelX = (OLED_MAIN_WIDTH_PIXELS - (TEXT_SPACING_X * textLineBreakdown.lineLengths[l])) >> 1;
		drawStringFixedLength(textLineBreakdown.lines[l], textLineBreakdown.lineLengths[l], textPixelX, textPixelY, oledMainPopupImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SPACING_Y);
		textPixelY += TEXT_SPACING_Y;
	}

	sendMainImage();
	if (!persistent) {
		if (!uiTimerManager.isTimerSet(TIMER_DISPLAY)) {
			uiTimerManager.setTimer(TIMER_DISPLAY, 800);
		}
	}
}


void updateWorkingAnimation() {
	String textNow;
	int error = textNow.set(workingAnimationText);		if (error) return;

	char buffer[4];
	buffer[3] = 0;

	for (int i = 0; i < 3; i++) {
		buffer[i] = (i <= workingAnimationCount) ? '.' : ' ';
	}

	error = textNow.concatenate(buffer);
	popupText(textNow.get(), true);
}

void displayWorkingAnimation(char const* word) {
	workingAnimationText = word;
	workingAnimationCount = 0;
	updateWorkingAnimation();
}

void removeWorkingAnimation() {
	if (workingAnimationText) removePopup();
}

void consoleText(char const* text) {
	struct TextLineBreakdown textLineBreakdown;
	textLineBreakdown.maxCharsPerLine = 18;

	breakStringIntoLines(text, &textLineBreakdown);

	int charWidth = 6;
	int charHeight = 7;

	int margin = 4;

	int textPixelY = setupConsole(textLineBreakdown.numLines * charHeight + 1) + 1;

	for (int l = 0; l < textLineBreakdown.numLines; l++) {
		int textPixelX = 8;
		drawStringFixedLength(textLineBreakdown.lines[l], textLineBreakdown.lineLengths[l], textPixelX, textPixelY, oledMainConsoleImage[0], OLED_MAIN_WIDTH_PIXELS, charWidth, charHeight);
		textPixelY += charHeight;
	}

	sendMainImage();

	uiTimerManager.setTimerSamples(TIMER_DISPLAY, CONSOLE_ANIMATION_FRAME_TIME_SAMPLES);
}


union {
	uint32_t u32;
	struct {
		uint8_t minX;
		uint8_t width;
		uint8_t minY;
		uint8_t maxY;
	};
} blinkArea;

void performBlink() {
	invertArea(blinkArea.minX, blinkArea.width, blinkArea.minY, blinkArea.maxY, oledMainImage);
	sendMainImage();
	uiTimerManager.setTimer(TIMER_DISPLAY, flashTime);
}

void setupBlink(int minX, int width, int minY, int maxY, bool shouldBlinkImmediately) {
	blinkArea.minX = minX;
	blinkArea.width = width;
	blinkArea.minY = minY;
	blinkArea.maxY = maxY;
	if (shouldBlinkImmediately) {
		invertArea(blinkArea.minX, blinkArea.width, blinkArea.minY, blinkArea.maxY, oledMainImage);
	}
	uiTimerManager.setTimer(TIMER_DISPLAY, flashTime);
	// Caller must do a sendMainImage() at some point after calling this.
}

void stopBlink() {
	if (blinkArea.u32) {
		blinkArea.u32 = 0;
		uiTimerManager.unsetTimer(TIMER_DISPLAY);
	}
}

struct SideScroller {
	char const* text; // NULL means not active.
	int textLength;
	int pos;
	int startX;
	int endX;
	int startY;
	int endY;
	int textSpacingX;
	int textSizeY;
	int stringLengthPixels;
	int boxLengthPixels;
	bool finished;
	bool doHilight;
};

#define NUM_SIDE_SCROLLERS 2

struct SideScroller sideScrollers[NUM_SIDE_SCROLLERS];

void setupSideScroller(int index, char const* text, int startX, int endX, int startY, int endY, int textSpacingX, int textSizeY, bool doHilight) {

	struct SideScroller* scroller = &sideScrollers[index];
	scroller->textLength = strlen(text);
	scroller->stringLengthPixels = scroller->textLength * textSpacingX;
	scroller->boxLengthPixels = endX - startX;
	if (scroller->stringLengthPixels <= scroller->boxLengthPixels) return;

	scroller->text = text;
	scroller->pos = 0;
	scroller->startX = startX;
	scroller->endX = endX;
	scroller->startY = startY;
	scroller->endY = endY;
	scroller->textSpacingX = textSpacingX;
	scroller->textSizeY = textSizeY;
	scroller->finished = false;
	scroller->doHilight = doHilight;

	sideScrollerDirection = 1;
	uiTimerManager.setTimer(TIMER_DISPLAY, 400);
}

void stopScrollingAnimation() {
	sideScrollerDirection = 0;
	for (int s = 0; s < NUM_SIDE_SCROLLERS; s++) {
		struct SideScroller* scroller = &sideScrollers[s];
		scroller->text = NULL;
	}
}

void timerRoutine() {

	if (workingAnimationText) {
		workingAnimationCount = (workingAnimationCount + 1) & 3;
		updateWorkingAnimation();
	}

	// If console active
	else if (numConsoleItems) {

		int timeTilNext = 0;

		// If console moving up
		if (consoleItems[0].maxY >= OLED_MAIN_HEIGHT_PIXELS) {

			bool anyRemoved = false;

			// Get rid of any items which have hit the top of the screen
			while (consoleItems[numConsoleItems - 1].minY < 2) {
				numConsoleItems--;
				anyRemoved = true;
			}

			if (anyRemoved) drawConsoleTopLine(); // Yeah the line will get copied - it's fine

			int firstRow = (consoleItems[numConsoleItems - 1].minY - 2) >> 3;
			int lastRow = consoleItems[0].maxY >> 3;

			for (int x = consoleMinX; x <= consoleMaxX; x++) {
				uint8_t carry;

				for (int row = lastRow; row >= firstRow; row--) {
					uint8_t prevBitsHere = oledMainConsoleImage[row][x];
					oledMainConsoleImage[row][x] = (prevBitsHere >> 1) | (carry << 7);
					carry = prevBitsHere;
				}
			}

			for (int i = 0; i < numConsoleItems; i++) {
				consoleItems[i].minY--;
				consoleItems[i].maxY--;
			}

			// If got further to go...
			if (consoleItems[0].maxY >= OLED_MAIN_HEIGHT_PIXELS) {
				timeTilNext = CONSOLE_ANIMATION_FRAME_TIME_SAMPLES;
			}
		}

		/*
		// Or if console moving down
		else if (false) {

			int firstRow = consoleItems[numConsoleItems - 1].minY >> 3;
			int lastRow = consoleItems[0].maxY >> 3;

			for (int x = consoleMinX; x <= consoleMaxX; x++) {
				uint8_t carry;

				for (int row = firstRow; row <= lastRow; row++) {
					uint8_t prevBitsHere = oledMainConsoleImage[row][x];
					oledMainConsoleImage[row][x] = (prevBitsHere << 1) | (carry >> 7);
					carry = prevBitsHere;
				}
			}

			for (int i = 0; i < numConsoleItems; i++) {
				consoleItems[i].minY++;
				consoleItems[i].maxY++;
			}

			// If got further to go...
			if (consoleItems[numConsoleItems - 1].minY < OLED_MAIN_HEIGHT_PIXELS) {
				uiTimerManager.setTimer(TIMER_DISPLAY, CONSOLE_ANIMATION_FRAME_TIME_SAMPLES);
			}

			// Or if now fully offscreen...
			else {
				numConsoleItems = 0;
			}
		}
*/
		// If top console item timed out
checkTimeTilTimeout:
		int32_t timeLeft = consoleItems[numConsoleItems - 1].timeoutTime - AudioEngine::audioSampleTimer;
		if (timeLeft <= 0) {
			if (!consoleItems[numConsoleItems - 1].cleared) {
				consoleItems[numConsoleItems - 1].cleared = true;
				clearAreaExact(consoleMinX + 1, consoleItems[numConsoleItems - 1].minY, consoleMaxX - 1, consoleItems[numConsoleItems - 1].maxY, oledMainConsoleImage);
			}
			consoleItems[numConsoleItems - 1].minY++;
			bool shouldCheckAgain = false;
			if (consoleItems[numConsoleItems - 1].minY > consoleItems[numConsoleItems - 1].maxY) {
				numConsoleItems--;
				shouldCheckAgain = numConsoleItems;
			}
			else {
				timeTilNext = CONSOLE_ANIMATION_FRAME_TIME_SAMPLES;
			}
			if (numConsoleItems) drawConsoleTopLine();
			if (shouldCheckAgain) goto checkTimeTilTimeout;
		}

		// Or if it hasn't timed out, then provided we didn't already want a real-soon callback, come back when it does time out
		else if (!timeTilNext) {
			timeTilNext = timeLeft;
		}

		if (timeTilNext) {
			uiTimerManager.setTimerSamples(TIMER_DISPLAY, timeTilNext);
		}

		sendMainImage();
	}

	else if (sideScrollerDirection) {

		bool finished = true;

		for (int s = 0; s < NUM_SIDE_SCROLLERS; s++) {
			SideScroller* scroller = &sideScrollers[s];
			if (scroller->text) {
				if (scroller->finished) continue;

				scroller->pos += sideScrollerDirection;

				if (scroller->pos <= 0) {
					scroller->finished = true;
				}
				else if (scroller->pos >= scroller->stringLengthPixels - scroller->boxLengthPixels) {
					scroller->finished = true;
				}
				else finished = false;

				// Ok, have to render.
				clearAreaExact(scroller->startX, scroller->startY, scroller->endX - 1, scroller->endY, oledMainImage);
				drawString(scroller->text, scroller->startX, scroller->startY, oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, scroller->textSpacingX, scroller->textSizeY, scroller->pos, scroller->endX);
				if (scroller->doHilight) {
					invertArea(scroller->startX, scroller->endX - scroller->startX, scroller->startY, scroller->endY, &OLED::oledMainImage[0]);
				}
			}
		}

		sendMainImage();

		int timeInterval;
		if (!finished) {
			timeInterval = (sideScrollerDirection >= 0) ? 15 : 5;
		}
		else {
			timeInterval = 400;
			sideScrollerDirection = -sideScrollerDirection;
			for (int s = 0; s < NUM_SIDE_SCROLLERS; s++) {
				sideScrollers[s].finished = false;
			}
		}
		uiTimerManager.setTimer(TIMER_DISPLAY, timeInterval);
	}

	else {
		removePopup();

		if (blinkArea.u32) {
			performBlink();
		}
	}
}

#endif
