#pragma once
class NMCommands
{
public:
	enum Commands
	{
		NoCommand,
		
		Clear,
		Refresh,
		
		SYSPage,
		NAVPage,
		COMPage,
		DBGPage,
		
		I2CScan,

		TestFont,
		ClearDebugText,
		CheckFreeHeap,
		
		Prev,
		Next,

		Last
	};
};

