/*
** Gobliiins 1
** Original game by CoktelVision
**
** Reverse engineered by Ivan Dubrov <WFrag@yandex.ru>
**
*/
#include "gob/gob.h"
#include "gob/global.h"
#include "gob/timer.h"
#include "gob/util.h"
#include "gob/debug.h"
#include "gob/draw.h"
#include "gob/game.h"

namespace Gob {

static int16 _mouseX, _mouseY, _keyPressed, _mouseButtons;

void util_initInput(void) {
	_mouseX = _mouseY = _keyPressed = _mouseButtons = 0;
}

void util_waitKey(void) {
	while (_keyPressed) {
		util_processInput();
		g_system->delayMillis(10);
	}
}

int16 util_translateKey(int16 key) {
	struct keyS {
		int16 from;
		int16 to;
	} keys[] = {
		{8, 0xe08   },	// Backspace
		{13, 0x1C0D },	// Enter
		{27, 0x11b  },	// ESC 
		{127, 0x5300},	// Del
		{273, 0x4800},	// Up arrow
		{274, 0x5000},	// Down arrow
		{275, 0x4D00},	// Right arrow
		{276, 0x4B00},	// Left arrow
		{282, 0x3b00},	// F1
		{283, 0x3c00},	// F2
		{284, 0x3d00},	// F3
		{285, 0x3E00},	// F4
		{286, 0x3F00},	// F5
		{287, 0x4000},	// F6
		{288, 0x4100},	// F7
		{289, 0x4200},	// F8
		{290, 0x4300},	// F9
		{291, 0x4400}	// F10
	};
	int i;

	for (i = 0; i < ARRAYSIZE(keys); i++)
		if (key == keys[i].from)
			return keys[i].to;

	return key;
}

int16 util_getKey(void) {
	while (!_keyPressed) {
		util_processInput();

		if (_keyPressed)
			break;

		g_system->delayMillis(10);
	}
	return util_translateKey(_keyPressed);
}

int16 util_checkKey(void) {
	int key = _keyPressed;

	if (_keyPressed)
		_keyPressed = 0;

	return util_translateKey(key);
}

int16 util_getRandom(int16 max) {
	return ((int32)rand() * max) / (RAND_MAX + 1);
}

void util_processInput() {
	OSystem::Event event;
	while (g_system->pollEvent(event)) {
		switch (event.type) {
		case OSystem::EVENT_MOUSEMOVE:
			_mouseX = event.mouse.x;
		    _mouseY = event.mouse.y;
			break;
		case OSystem::EVENT_LBUTTONDOWN:
			_mouseButtons |= 1;
			break;
		case OSystem::EVENT_RBUTTONDOWN:
			_mouseButtons |= 2;
			break;
		case OSystem::EVENT_LBUTTONUP:
			_mouseButtons &= ~1;
			break;
		case OSystem::EVENT_RBUTTONUP:
			_mouseButtons &= ~2;
			break;
		case OSystem::EVENT_KEYDOWN:
			_keyPressed = event.kbd.keycode;
			break;
		case OSystem::EVENT_KEYUP:
			_keyPressed = 0;
			break;
		case OSystem::EVENT_QUIT:
			g_system->quit();
			break;
		default:
			break;
		}
	}
}

void util_getMouseState(int16 *pX, int16 *pY, int16 *pButtons) {
//	int16 x = 0;
//	int16 y = 0;
//	int16 buttons = 0;

	*pX = _mouseX;
	*pY = _mouseY;

	if (pButtons != 0)
		*pButtons = _mouseButtons;
//	if (pX != 0)
//		*pX = x >> mouseXShift;
//	if (pY != 0)
//		*pY = y >> mouseYShift;
}

void util_setMousePos(int16 x, int16 y) {
	g_system->warpMouse(x, y);
}

void util_delay(uint16 msecs) {
	g_system->delayMillis(msecs);
}

void util_beep(int16 freq) {
	if (soundFlags == 0)
		return;

	//sound(freq);
	util_delay(50);
	//nosound();
}

uint32 util_getTimeKey(void) {
	return g_system->getMillis();
}

void util_waitMouseUp(void) {
	int16 x;
	int16 y;
	int16 buttons;

	do {
		util_getMouseState(&x, &y, &buttons);
	} while (buttons != 0);
}

void util_waitMouseDown(void) {
	int16 x;
	int16 y;
	int16 buttons;

	do {
		util_getMouseState(&x, &y, &buttons);
	} while (buttons == 0);
}

/* NOT IMPLEMENTED */
int16 util_calcDelayTime() {
	return 0;
}

/* NOT IMPLEMENTED */
void util_checkJoystick() {
	useJoystick = 0;
}

void util_setFrameRate(int16 rate) {
	if (rate == 0)
		rate = 1;

	frameWaitTime = 1000 / rate;
	startFrameTime = util_getTimeKey();
}

void util_waitEndFrame() {
	int32 time;

	if (pPrimarySurfDesc) {
		g_system->copyRectToScreen(pPrimarySurfDesc->vidPtr, 320, 0, 0, 320, 200);
		g_system->updateScreen();
	}

	time = util_getTimeKey() - startFrameTime;
	if (time > 1000 || time < 0) {
		startFrameTime = util_getTimeKey();
		return;
	}
	if (timer_enabled) {
		do {
			time = util_getTimeKey();

		} while (time - startFrameTime < frameWaitTime);
	} else {
		if (frameWaitTime - time > 0)
			util_delay(frameWaitTime - time);
	}
	startFrameTime = util_getTimeKey();
}

int16 joy_getState() {
	return 0;
}

int16 joy_calibrate() {
	return 0;
}

FontDesc *util_loadFont(const char *path) {
	FontDesc *fontDesc = (FontDesc *) malloc(sizeof(FontDesc));
	char *data;

	if (fontDesc == 0)
		return 0;

	data = data_getData(path);
	if (data == 0) {
		free((char *)fontDesc);
		return 0;
	}

	fontDesc->dataPtr = data + 4;
	fontDesc->itemWidth = data[0] & 0x7f;
	fontDesc->itemHeight = data[1];
	fontDesc->startItem = data[2];
	fontDesc->endItem = data[3];

	fontDesc->itemSize =
	    ((fontDesc->itemWidth - 1) / 8 + 1) * fontDesc->itemHeight;
	fontDesc->bitWidth = fontDesc->itemWidth;

	if (data[0] & 0x80)
		fontDesc->extraData =
		    data + 4 + fontDesc->itemSize * (fontDesc->endItem -
		    fontDesc->startItem + 1);
	else
		fontDesc->extraData = 0;
	return fontDesc;
}

void util_freeFont(FontDesc * fontDesc) {
	free(fontDesc->dataPtr - 4);
	free((char *)fontDesc);
}

void util_clearPalette(void) {
	int16 i;
	byte colors[768];

	if (videoMode != 0x13)
		error("util_clearPalette: Video mode 0x%x is not supported!",
		    videoMode);

	if (setAllPalette) {
		for (i = 0; i < 768; i++)
			colors[i] = 0;
		g_system->setPalette(colors, 0, 256);

		return;
	}

	for (i = 0; i < 16; i++)
		vid_setPalElem(i, 0, 0, 0, 0, videoMode);
}

void util_insertStr(const char *str1, char *str2, int16 pos) {
	int16 len1;
	int16 i;
	int16 from;

	i = strlen(str2);
	len1 = strlen(str1);
	if (pos < i)
		from = pos;
	else
		from = i;

	for (; i >= from; i--)
		str2[len1 + i] = str2[i];

	for (i = 0; i < len1; i++)
		str2[i + from] = str1[i];
}

void util_cutFromStr(char *str, int16 from, int16 cutlen) {
	int16 len;
	int16 i;

	log_write("util_cutFromStr: str = %s, ", str);
	len = strlen(str);
	if (from >= len)
		return;
	if (from + cutlen > len) {
		str[from] = 0;
		log_write("res = %s\n", str);
		return;
	}

	i = from;
	do {
		str[i] = str[i + cutlen];
		i++;
	} while (str[i] != 0);
	log_write("res = %s\n", str);
}

int16 util_strstr(const char *str1, char *str2) {
	char c;
	uint16 len1;
	uint16 i;

	log_write("util_strstr: str1 = %s, str2 = %s\n", str1, str2);

	for (i = 0, len1 = strlen(str1); strlen(str2 + i) >= len1; i++) {
		c = str2[i + len1];
		str2[i + len1] = 0;
		if (strcmp(str2 + i, str1) == 0) {
			str2[i + len1] = c;
			return i + 1;
		}
		str2[i + len1] = c;
	}
	return 0;
}

void util_listInsertFront(Util_List * list, void *data) {
	Util_ListNode *node;

	node = (Util_ListNode *) malloc(sizeof(Util_ListNode));
	if (list->pHead != 0) {
		node->pData = data;
		node->pNext = list->pHead;
		node->pPrev = 0;
		list->pHead->pPrev = node;
		list->pHead = node;
	} else {
		list->pHead = node;
		list->pTail = node;
		node->pData = data;
		node->pNext = 0;
		node->pPrev = 0;
	}
}

void util_listInsertBack(Util_List * list, void *data) {
	Util_ListNode *node;

	if (list->pHead != 0) {
		if (list->pTail == 0) {
			list->pTail = list->pHead;
			log_write("util_listInsertBack: Broken list!");
		}

		node =
		    (Util_ListNode *) malloc(sizeof(Util_ListNode));
		node->pData = data;
		node->pPrev = list->pTail;
		node->pNext = 0;
		list->pTail->pNext = node;
		list->pTail = node;
	} else {
		util_listInsertFront(list, data);
	}
}

void util_listDropFront(Util_List * list) {
	if (list->pHead->pNext == 0) {
		free((char *)(list->pHead));
		list->pHead = 0;
		list->pTail = 0;
	} else {
		list->pHead = list->pHead->pNext;
		free((char *)(list->pHead->pPrev));
		list->pHead->pPrev = 0;
	}
}

void util_deleteList(Util_List * list) {
	while (list->pHead != 0) {
		util_listDropFront(list);
	}

	free((char *)list);
}

char util_str1[] =
    "       '   + - :0123456789: <=>  abcdefghijklmnopqrstuvwxyz      abcdefghijklmnopqrstuvwxyz     ";
char util_str2[] =
    " ueaaaaceeeiii     ooouu        aioun                                                           ";
char util_str3[] = "                                ";

void util_prepareStr(char *str) {
	uint16 i;
	int16 j;
	char buf[300];

	strcpy(buf, util_str1);
	strcat(buf, util_str2);
	strcat(buf, util_str3);

	for (i = 0; i < strlen(str); i++)
		str[i] = buf[str[i] - 32];

	while (str[0] == ' ')
		util_cutFromStr(str, 0, 1);

	while (strlen(str) > 0 && str[strlen(str) - 1] == ' ')
		util_cutFromStr(str, strlen(str) - 1, 1);

	i = util_strstr(" ", str);

	while (1) {
		if (i == 0)
			return;

		if (str[i] == ' ') {
			util_cutFromStr(str, i - 1, 1);
			continue;
		}

		j = util_strstr(" ", str + i);
		if (j != 0)
			i += j;
		else
			i = 0;
	}
}

void util_waitMouseRelease(char drawMouse) {
	int16 buttons;
	int16 mouseX;
	int16 mouseY;

	do {
		game_checkKeys(&mouseX, &mouseY, &buttons, drawMouse);
		if (drawMouse != 0)
			draw_animateCursor(2);
	} while (buttons != 0);
}

void keyboard_release(void) {;}
} // End of namespace Gob
