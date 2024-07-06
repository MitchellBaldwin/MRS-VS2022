/*	MFCDPage.h
*	MFCDPageClass - Base class implementing the concept of a single Page of a Multifunction Color Display (MFCD)
*	
*	Designed for use with the MFCDClass, which builds and maintains a list of references to instances of this class
*	Must be capable of drawing itself using the reference to a tft instance supplied by the parent MFCDClass instance
*	Builds and maintains a list of SoftOSBClass instances associated with physical Option Select Buttons; the MFCDClass
*	is responsible for setting up interface to the physical OSB arrays as well as to rotary encoders, switches or
*	other physical controls
*	
*
*
*	Mitchell Baldwin copyright 2024
*
*	v 0.0:	Initial commit
*	v 0.1:
*
*/
#ifndef _MFCDPAGE_h
#define _MFCDPAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <TFT_eSPI.h>
#include "SoftOSB.h"

class MFCDPageClass
{
protected:
	TFT_eSPI* tft = nullptr;
	SoftOSBClass* OSBs[SoftOSBClass::OSBPositions::ROSB4];
	String Title;

public:
	MFCDPageClass();
	MFCDPageClass(TFT_eSPI* parent_tft);
	virtual bool Init(TFT_eSPI* parent_tft);

	virtual void Draw();		// Draw / redraw entire page
	virtual void Update();		// Update dynamic page content
};

//extern MFCDPageClass MFCDPage;

class NavigationPageClass : public MFCDPageClass
{
public:
	virtual bool Init(TFT_eSPI* parent_tft);
	virtual void Draw();
};

class CommunicationsPageClass : public MFCDPageClass
{
public:
	virtual bool Init(TFT_eSPI* parent_tft);
	virtual void Draw();
};

class SystemPageClass : public MFCDPageClass
{
public:
	virtual bool Init(TFT_eSPI* parent_tft);
	virtual void Draw();
};

class DebugPageClass : public MFCDPageClass
{
public:
	virtual bool Init(TFT_eSPI* parent_tft);
	virtual void Draw();
};

extern NavigationPageClass NavigationPage;
extern CommunicationsPageClass CommunicationsPage;
extern SystemPageClass SystemPage;
extern DebugPageClass DebugPage;
#endif

