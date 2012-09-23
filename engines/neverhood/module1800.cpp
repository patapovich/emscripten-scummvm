/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "neverhood/module1800.h"
#include "neverhood/navigationscene.h"

namespace Neverhood {

Module1800::Module1800(NeverhoodEngine *vm, Module *parentModule, int which)
	: Module(vm, parentModule) {
	
	// TODO SoundMan_addSoundList(0x04A14718, dword_4AFE70);
	// TODO SoundMan_setSoundListParams(dword_4AFE70, true, 50, 600, 10, 150);
	_vm->_soundMan->playTwoSounds(0x04A14718, 0x8A382B55, 0x0C242F1D, 0);

	if (which < 0) {
		createScene(_vm->gameState().sceneNum, -1);
	} else if (which == 2) {
		createScene(5, 0);
	} else if (which == 3) {
		createScene(0, 0);
	} else {
		createScene(3, 1);
	} 

}

Module1800::~Module1800() {
	_vm->_soundMan->deleteGroup(0x04A14718);
}

void Module1800::createScene(int sceneNum, int which) {
	static const byte kNavigationTypes00[] = {1, 0, 2, 0};
	static const byte kNavigationTypes01[] = {5};
	debug("Module1800::createScene(%d, %d)", sceneNum, which);
	_vm->gameState().sceneNum = sceneNum;
	switch (_vm->gameState().sceneNum) {
	case 0:
		createNavigationScene(0x004AFD38, which, kNavigationTypes00);
		break;
	case 1:
		createNavigationScene(0x004AFD98, which, kNavigationTypes01);
		break;
	case 2:
		createSmackerScene(0x006C0085, true, true, false);
		break;
	case 3:
		createNavigationScene(0x004AFDB0, which);
		break;
	case 4:
		createNavigationScene(0x004AFDE0, which);
		break;
	case 5:
		createNavigationScene(0x004AFE40, which);
		break;
	case 6:
		_vm->_soundMan->deleteGroup(0x04A14718);
		createSmackerScene(0x08D84010, true, true, false);
		break;
	case 7:
		// TODO SoundMan_setSoundListParams(dword_4AFE70, false, 0, 0, 0, 0);
		createSmackerScene(0x0168B121, true, true, false);
		break;
	case 8:
		// TODO _childObject = new CreditsScene(_vm, this, 0);
		break;
	case 9:
		// NOTE: Newly introduced sceneNum 9 (was duplicate 3 with own update handler)
		createSmackerScene(0x0A840C01, true, true, false);
		break;
	}
	SetUpdateHandler(&Module1800::updateScene);
	_childObject->handleUpdate();
}

void Module1800::updateScene() {
	if (!updateChild()) {
		switch (_vm->gameState().sceneNum) {
		case 0:
			if (_moduleResult == 1) {
				createScene(4, 0);
			} else if (_moduleResult == 2) {
				createScene(1, -1);
			} else if (_moduleResult == 3) {
				createScene(3, 0);
			}
			break;
		case 1:
			if (_navigationAreaType == 3) {
				createScene(7, -1);
			} else {
				createScene(2, -1);
			}
			break;
		case 2:
			createScene(0, 2);
			break;
		case 3:
			if (_moduleResult == 0) {
				createScene(9, -1);
			} else if (_moduleResult == 1) {
				createScene(0, 1);
			}
			break;
		case 4:
			if (_moduleResult == 0) {
				createScene(6, -1);
			} else if (_moduleResult == 1) {
				createScene(5, 0);
			} else if (_moduleResult == 2) {
				createScene(0, 3);
			} else if (_moduleResult == 3) {
				createScene(4, 3);
			}
			break;
		case 5:
			if (_moduleResult == 0) {
				leaveModule(2);
			} else if (_moduleResult == 1) {
				createScene(4, 3);
			}
			break;
		case 6:
			createScene(8, -1);
			break;
		case 7:
			leaveModule(3);
			break;
		case 8:
			leaveModule(1);
			// TODO GameState stuff
			break;
		case 9:
			leaveModule(0);
			break;
		}
	} else {
		switch (_vm->gameState().sceneNum) {
		case 0:
			if (navigationScene()->getSoundFlag1() && navigationScene()->getNavigationIndex() == 2) {
				_vm->_soundMan->setTwoSoundsPlayFlag(false);
			}
			break;
		}
	}
}
		
} // End of namespace Neverhood
