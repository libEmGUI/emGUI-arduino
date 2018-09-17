#ifndef _WINDOW_PAGE_WINDOW_PLOT_
#define _WINDOW_PAGE_WINDOW_PLOT_

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
		plotLead.ulWritePos = 0;

		plot = pxPlotCreate(0, 0, EMGUI_LCD_WIDTH, EMGUI_LCD_HEIGHT - EMGUI_STATUS_BAR_HEIGHT -20, xThis, &plotLead);
		vPlotSetScale(plot, 250);   // Size of grid cell (yval)
		currentMonitor = pxLabelCreate(10, EMGUI_LCD_HEIGHT - EMGUI_STATUS_BAR_HEIGHT - 20, EMGUI_LCD_WIDTH, 20, "I: _ (0.1 mA)", xGetDefaultFont(), 100, xThis);
		vLabelSetTextAlign(currentMonitor, LABEL_ALIGN_CENTER);
		vLabelSetVerticalAlign(currentMonitor, LABEL_ALIGN_MIDDLE);	
	}
	
 	xPlot * plot;
	virtual bool onCloseRequest() {
		return false;
	}
	xLabel * currentMonitor;
	uint16_t data;
	void update(short data) {
		this->data = data;
		vPlotAddValue(&plotLead, data);
		//long data = plotLead.psData[plotLead.ulWritePos];
	} 

	bool onDrawUpdate(){
		static char textBuffer[80];

		//sprintf (textBuffer, "I_Avg: %d; I: %d.%d (mA)\0", data / 10, data / 10, abs(data % 10));
		sprintf (textBuffer, "%d\0", data / 10);

		pcLabelSetText(currentMonitor, textBuffer);
		return false;
	}

};

#endif // _WINDOW_PAGE_WINDOW_PLOT_
