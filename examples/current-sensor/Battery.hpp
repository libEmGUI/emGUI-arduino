#ifndef _BATTERY_
#define _BATTERY_

#include <Arduino.h>
#include "src/IIR/IIR_filter.h"
#include "Stopwatch.h"
#include "Config.hpp"
#include "UpdatableValue.hpp"

typedef enum {
	BAT_UNKNOWN = 0,
	BAT_DISCHARGING = 1,
	BAT_CHARGING = 2,
	BAT_CHARGED = 3,
	BAT_ERROR = 4
} chargeStatus_t;


class Battery{
	public:
		Battery(int PE_CHRG_CH, int PE_CHRG_STB) :CH(PE_CHRG_CH), STB(PE_CHRG_STB){
			status = BAT_UNKNOWN;
			_configFN = F("/bat.json");
			_configRatioParam = F("dr");
			adcValue = 0;
			voltage = 0;
		}
		void loop(){
			#if defined(_MSC_VER)
			adcValue = analogRead(A0);
			#else
			adcValue = _batADC.doSample(analogRead(A0) * 1000) / 1000;
			#endif
			
			voltage = (adcValue * ratioM) / ratio;

			//Serial.printf("V: %d; dr: %d, adc: %d\n", voltage, ratio, adcValue);
		}

		int charge(){
			int val = ((voltage - min) * 100 / ( max - min)); 
			if (val < 0)   return 0;
			if (val > 100) return 100;
			return val;
		}

		bool init(){
			JSONConfig jc(_configFN);
			auto &j = jc.load();
			if (!j.containsKey(_configRatioParam) || j[_configRatioParam] <= 1000) {
				resetCalib();
				return false;
			}

			ratio = j[_configRatioParam];
			
			return true;
		}

		bool hasCalib() {
			JSONConfig jc(_configFN);
			return jc.exists();
		}

		bool resetCalib() {
			JSONConfig jc(_configFN);
			return jc.remove();
		}

		bool saveCalib() {
			JSONConfig jc(_configFN);
			if (jc.exists()) return true;

			auto &j = jc.create();
			j[_configRatioParam] = adcValue * ratioM / chargeThreshold; // ratio is calculated at full charge (4300mv)
			return jc.save(j) && init();
		}

		bool checkStatus(int charge_state){
			int newStatus;

			if (charge_state == CH) newStatus = BAT_CHARGING;
			if (charge_state == 0) newStatus = BAT_DISCHARGING;
			if (charge_state == STB) newStatus = BAT_CHARGED;
			if (charge_state == (STB | CH)) newStatus = BAT_ERROR;

			if (status == newStatus) return false;

			if (newStatus == BAT_CHARGED)
				saveCalib();
			status = newStatus;
			return true;
		}

		const int chargeThreshold = 4300; // TP4056 charger threshold = 4.3v
		const int max = 4200; // 100% charge boundary
		const int min = 3350; // 0% charge boundary
		UpdatableValue adcValue;
		UpdatableValue voltage;

		int ratio = 1867; // ( resitor divider 43k/10k ) * adcRatio * ratioM
		const int ratioM = 10000;
		UpdatableValue status;

	protected:
		IIR_filter _batADC = IIR_filter(10 * 1000);  // 10 seconds in ms
		int STB;
		int CH;
		const __FlashStringHelper * _configFN;
		const __FlashStringHelper * _configRatioParam;

};

#endif