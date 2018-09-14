#pragma once

#include "WindowPack.h"

using namespace emGUI;
class BatteryWidget : public PictureStack {
public:
	typedef std::unique_ptr<BatteryWidget> uniquePtr;

	BatteryWidget(uint16_t usX, uint16_t usY, xWidget *pxWidParent)
		: PictureStack(usX, usY, "/batch/0.bmp", pxWidParent) {
		addPicture("/batch/25.bmp");
		addPicture("/batch/50.bmp");
		addPicture("/batch/75.bmp");
		addPicture("/batch/100.bmp");
		addPicture("/batd/0.bmp");
		addPicture("/batd/25.bmp");
		addPicture("/batd/50.bmp");
		addPicture("/batd/75.bmp");
		addPicture("/batd/100.bmp");
		addPicture("/batd/done.bmp");
	}

	//TODO: refactor this 
	void selectItem(int voltage, int charging) {
		int number = 0;
		switch (charging) {
		case 3: number = 10; break;
		case 2:
			if (voltage >= 4100) number = 9;
			else if (voltage > 3850) number = 8;
			else if (voltage > 3700) number = 7;
			else if (voltage > 3500) number = 6;
			else number = 5;
			break;
		case 1:
			if (voltage >= 4100) number = 4;
			else if (voltage > 3850) number = 3;
			else if (voltage > 3700) number = 2;
			else if (voltage > 3500) number = 1;
			else number = 0;
		}


		bPictureStackSelect(xThis, (char)number);
	}
	void addPicture(xPicture xPic) {
		bPictureStackAddItem(xThis, xPic);
	}

	bool onDrawUpdate() {
		selectItem(Peripherial::getInstance()->bat.voltage, Peripherial::getInstance()->bat.status.pick());
		return false;
	}
};
