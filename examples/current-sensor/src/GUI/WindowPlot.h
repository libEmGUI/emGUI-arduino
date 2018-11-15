#pragma once


#include "WindowPack.h"
using namespace emGUI;

class WindowPlot : public DisposableWindow<WINDOW_PLOT, WindowPlot> {
public:
	xPlotData_t plotLead;
	void create() {				
		vWindowSetHeader(xThis, "Plot, mA");
		plotLead.bDataFilled = false;
		plotLead.bWriteEnabled = false;
		plotLead.sDataDCOffset = -500;
		plotLead.sName = "Test";
		plotLead.ulElemCount = AFE_DATA_RATE * 10;
		plotLead.psData = (short *)malloc(sizeof(*plotLead.psData)*plotLead.ulElemCount);
		memset(plotLead.psData, 0, sizeof(*plotLead.psData)*plotLead.ulElemCount);
		plotLead.ulWritePos = 0;

		Serial.println("Create window");
		plot = pxPlotCreate(0, 0, EMGUI_LCD_WIDTH, EMGUI_LCD_HEIGHT - EMGUI_STATUS_BAR_HEIGHT -20, xThis, &plotLead);
		vPlotSetScale(plot, 250);   // Size of grid cell (yval)

		currentMonitorHeader = pxLabelCreate(0, EMGUI_LCD_HEIGHT - EMGUI_STATUS_BAR_HEIGHT - 20, 100, 20, "I (mA):", xGetDefaultFont(), 100, xThis);
		vLabelSetTextAlign(currentMonitorHeader, LABEL_ALIGN_RIGHT);
		vLabelSetVerticalAlign(currentMonitorHeader, LABEL_ALIGN_MIDDLE);	

		currentMonitor = pxLabelCreate(100, EMGUI_LCD_HEIGHT - EMGUI_STATUS_BAR_HEIGHT - 20, 100, 20, "0", xGetDefaultFont(), 100, xThis);
		vLabelSetTextAlign(currentMonitor, LABEL_ALIGN_LEFT);
		vLabelSetVerticalAlign(currentMonitor, LABEL_ALIGN_MIDDLE);	
	}
	
 	xPlot * plot;

	xLabel * currentMonitor;
	xLabel * currentMonitorHeader;
	short data;
	short passDelay = 60;
	bool onClose(){
		free(plotLead.psData);
		return false;
	};
	void update(float data) {
		this->data = (short)(data * 10);
		vPlotAddValue(&plotLead, data);
		//long data = plotLead.psData[plotLead.ulWritePos];
	} 

	bool onDrawUpdate(){
		static char textBuffer[80];
		static auto lastMillis = millis();

		if (millis() - lastMillis > passDelay) {
			sprintf (textBuffer, "%d\0", data / 10);
			pcLabelSetText(currentMonitor, textBuffer);
			lastMillis = millis();
		} 
		return false;
	}

};