#pragma once

//#include "myplugincids.h"
#include "base/source/fstreamer.h"
//#include "pluginterfaces/vst/ivstparameterchanges.h"



namespace saola {

	class VUmeter
	{
	public:
		VUmeter();
		~VUmeter();
		float** calculateRMSIndB(float** in, int numSamples);
		void set(double sampleRate, int numSamples, double tau = 0.05);

	protected:
		int numOfSamples;
		double sampleOfRates;
		float a;
		float vuRMS[2], vuOut[2];
		float** buffer;

	};




}