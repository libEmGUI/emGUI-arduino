#include "WindowPack.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"
#include "Fonts/FreeSans24pt7b.h"
#include "../UTF8-fonts/MyriadPro_Bold9pt8b.h"
#include "../UTF8-fonts/MyriadPro_Regular9pt8b.h"
#include "../UTF8-fonts/MyriadPro_Regular12pt8b.h"
#include "../UTF8-fonts/MyriadPro_Regular24pt8b.h"
#include "../UTF8-fonts/customfonts.h"

const GFXfont * bigFont[] = { &Roboto_Regular24pt7b , &Roboto_Regular24pt8bRUS, NULL };
const GFXfont * smallFont[] = { &Roboto_Regular9pt7b , &Roboto_Regular9pt8bRUS, NULL };
const GFXfont * boldFont[] = { &Roboto_Bold9pt7b , &Roboto_Bold9pt8bRUS, NULL };


xFont getBigFont(){
	return bigFont;
}
xFont getSmallFont(){
	return smallFont;
}
xFont getBoldFont(){
	return boldFont;
}

void WindowTask::create() {
	auto w = 50;
	auto x = EMGUI_LCD_WIDTH - w - bo;
	auto y = so;
	auto h = 0;


	clock = std::make_unique<TimeClock>(x, y, w, h, getBoldFont(), 6, xThis);

	x = bo;
	y = so;
	w = usWidgetGetX0(clock->get(), false) - x - so;
	h = 0;

	vWindowSetHeader(xThis, F_WRP("New message"));

	title = pxLabelCreate(x, y, w, h, F_WRP("Встреча в  19:00..."), getSmallFont(), 40, xThis);

	y += usWidgetGetH(title) + so;
	w = EMGUI_LCD_WIDTH - 2 * x;
	h = 130;


	frame = std::make_unique<TextFrame>(x, y, w, h, F_WRP(
		"Your message doesn’t need to be long, but it does need to "
		"capture the reader’s attention and sell them on why you’re a strong applicant for the job."), getSmallFont(), 1000, this);

	x = bo;
	y = usWidgetGetY1(frame->get(), false) + so;
	w = (usWidgetGetW(xThis) - 3 * bo) / 2;
	h = usWidgetGetH(xThis) - y - bo;

	btn_ok = std::make_unique<ButtonWithBG>(x, y, F_WRP("OK"), this);
	btn_ok->onClick = [this](Button* b) {
		deviceState::getInstance()->responseTask(0);
		this->close();
		return true; 
	};

	x = usWidgetGetX1(btn_ok->get(), false) + 8;

	btn_cancel = std::make_unique<ButtonWithBG>(x, y, F_WRP("Cancel"), this);
	btn_cancel->onClick = [this](Button* b) {
		deviceState::getInstance()->responseTask(1);
		this->close();
		return true; 
	};

}

bool WindowTask::onOpen() {
	auto ds = deviceState::getInstance();
	frame->setText(ds->_taskMessage.c_str());
	pcLabelSetText(title, ds->_taskTitle.c_str());
	vWindowSetHeader(xThis, ds->_taskType.c_str());

	vButtonSetText(btn_ok->get(), ds->_taskOption1.c_str());
	vButtonSetText(btn_cancel->get(), ds->_taskOption2.c_str());

	return true;
}

void WindowMessage::create() {
	auto x = 0;
	auto y = 60;
	auto w = usWidgetGetW(xThis);
	auto h = 18;


	title = pxLabelCreate(x, y, w, h, F_WRP("Welcome"), getBoldFont(), 50, xThis);
	vLabelSetTextAlign(title, LABEL_ALIGN_CENTER);
	y += usWidgetGetH(title) + so;
	header = pxLabelCreate(x, y, w, h * 4, F_WRP("Boris Smirnov"), getSmallFont(), 100, xThis);
	vLabelSetTextAlign(header, LABEL_ALIGN_CENTER);
}

void WindowPopUp::create() {
	auto x = 0;
	auto y = 40;
	auto w = EMGUI_LCD_WIDTH;
	auto h = 0;
	clock = std::make_unique<TimeClock>(x, y, w, h, getBigFont(), 6, xThis);


	//message
	x = 20;
	y = 120;
	w = 200;
	h = 70;
	frame = std::make_unique<Button>(x, y, w, h, F_WRP(""), this);

	x = 10;
	y = 10;
	w = 150;
	h = 0;

	auto header = pxLabelCreate(x, y, w, h, F_WRP("New message"), getBoldFont(), 20, frame->get());

	x = x;
	y += usWidgetGetH(header) + 5;
	w = w;
	h = h;
	auto msg = pxLabelCreate(x, y, w, h, F_WRP("Meeting at 19:00..."), getSmallFont(), 50, frame->get());

	x = usWidgetGetW(frame->get()) - 35;
	y = usWidgetGetH(frame->get()) / 2 - 15;
	w = 30;
	h = 30;
	auto mesIco = pxButtonCreateFromImage(x, y, "/msg.bmp", frame->get());

	frame->onClick = [this](Button* b) {
		this->close();
		WindowTask::getInstance()->open();
		return true;
	};

}

void WindowAuth::create() {

	auto x = 0;
	auto y = 40;
	auto w = usWidgetGetW(xThis);
	auto h = 0;


	auto title = pxLabelCreate(x, y, w, h, F_WRP("Аутентификация"), getBoldFont(), 50, xThis);
	vLabelSetTextAlign(title, LABEL_ALIGN_CENTER);

	w = 40;
	h = 40;

	x = (usWidgetGetW(xThis) - w) / 2;
	y = usWidgetGetH(xThis) - bo - h;

	auto ico = pxButtonCreateFromImage(x, y, "/nfc.bmp", xThis);

	x = 0;
	w = usWidgetGetW(xThis);
	y = usWidgetGetY1(title, false) + so;
	h = usWidgetGetY0(ico, false) - so - y;
	auto msg = pxLabelCreate(x, y, w, h, F_WRP("Приложите браслет к NFC считывателю"), getSmallFont(), 200, xThis);
	vLabelSetTextAlign(msg, LABEL_ALIGN_CENTER);
}