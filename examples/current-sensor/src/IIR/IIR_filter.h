#ifndef _IIR_FILTER_
#define _IIR_FILTER_

class IIR_filter
{
public:
	IIR_filter(int timeBase);

	virtual ~IIR_filter();
	int doSample(int data);
	bool setCoef(int);
	int getCoef();

	int lastValue = 0;

protected:
	int timeBase;
	unsigned long _prev;
};

#endif //_IIR_FILTER_