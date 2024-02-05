/*	NMControls.cpp
*	NMControlsClass - Base class for managing data from control panel of the MRS RC Navigation Module
*
*/

#include "NMControls.h"
#include "DEBUG Macros.h"

void NMControlsClass::Init()
{


}

void NMControlsClass::Init(byte rightRockerSwitchPin)
{
	RightRockerSwitchPin = rightRockerSwitchPin;

	RightRockerSwitch = new ezButton(RightRockerSwitchPin);
}

void NMControlsClass::Update()
{
	RightRockerSwitch->loop();
	int newState = RightRockerSwitch->getState();
	newState = newState ? 0 : 1;	// Need to reverse sense of state for rocker switch
	if (RightRockerSwitchState != newState)
	{
		RightRockerSwitchState = newState;
		_PL(RightRockerSwitchState);
	}

}

NMControlsClass NMControls;

