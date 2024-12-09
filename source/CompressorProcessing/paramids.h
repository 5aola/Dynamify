#pragma once

const int FPS = 60; // Hz
const int BUFFERSIZE = 4096;
const float RELEASETIME = 0.6; //sec
const int NUMBANDS = 24;

enum DynamifyParams
{
	/** parameter ID */
	ratioValueid = 1,
	thresholdValueid = 2,
	attackValueid = 3,
	releaseValueid = 4,
	gainValueid = 5,
	autogainValueid = 6,
	drywetValueid = 7,

};

enum CompressorType
{
	tCompress = 1,
	tUpwardCompress = 2,
	tUpDownCompress = 3,
	tLimit =  4,
	tExpand = 5,
	tGate = 6,
};