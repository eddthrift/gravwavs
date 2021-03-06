//                             GRAVITATIONAL WAVE ASTRONOMY                            //
//                                    Data Generation                                  //

// This header file includes the functions necessary to initialise the parameters of a //
//  BBH and simulate the gravitational wave signature of the system in the frequency   //
//                                         domain.                                     //

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <complex>

#include "gwReadWrite.h"

#define C_CONST 2.99792458E8
#define G_CONST 6.6740831E-11
#define SM_CONST 1.989E30
#define MPc_CONST 3.086E22
#define MHz_CONST 1E6

#define SUCCESS 0
#define FAILURE 1

using namespace std;

//-------------------------------------------------------------------------------------//

// Total mass
double comboMass(double m1, double m2){
	return m1 + m2;
}

// Symmetric mass ratio
double nu(double m1, double m2){
	return (m1*m2)/pow(comboMass(m1, m2), 2.0);
}


//-------------------------------------------------------------------------------------//

// Vector for storage of mass and distance parameters 
// and intial parameters
typedef double theta_vec [3];

// Vector for storage of phase parameters
typedef double psi_vec [8];

// Vector for storage of transition parameters
typedef double mu_vec [4];

enum polarisation {
	Cross,
	Plus
};

// Struct for storage of all essential system parameters
struct parameters{
	
	// Inclinatinon angles of orbit (radians)
	theta_vec angles;
	
	// Vector of initial values:
	// init[0] = time of signal arrival
	// init[1] = phase of signal at arrival time
	// init[2] = minimum frequency of generated signal
	theta_vec init;
	
	// Vector of mass and distance parameters:
	// theta[0] = distance, D 
	// theta[1] = combined mass, M 
	// theta[2] = symmetrical mass ratio, n 
	theta_vec theta;
	
	// Vector of phase components
	psi_vec psi;
	
	// Vector of transition frequency components:
	// mu[0] = inspiral -> merger transition frequency, fMerge 
	// mu[1] = merger -> ringdown transition frequency, fRing
	// mu[2] = width of Lorentzian frequency distribution, sigma
	// mu[3] = maximum cutoff frequency, fCut
	mu_vec mu;
	
	// Total time of data set
	double totTime;
	
	// Frequency increment in geometric natural units
	double df;
	
	// Scaling amplitude of the wave
	double amp0;
	
};

// Calculate phase components
double computePhaseComps(double x, double y, double z, double n, double M, double k){
	return (x*pow(n, 2.0) + y*n + z)/(n*pow(M_PI*M, (5.0-k)/3.0));
}

// Calculate transition frequencies and sigma
double computeTransitionComps(double a, double b, double c, double n, double M){
	return (a*pow(n, 2.0) + b*n + c)/(M_PI*M);
}

// Preceding constant scaling amplitude
double effAmp(parameters *params){
	return pow(params->mu[0], -7.0/6.0)*pow(params->theta[1], 5.0/6.0)*pow(5.0*params->theta[2]/24.0, 1.0/2.0)
			/(params->theta[0]*pow(M_PI, 2.0/3.0));
}

// Set the fundamental parameters of the system
void setFundamentalParameters(
	double init_time, 
	double total_time,
	double init_phase, 
	double min_freq,
	double m_1, 
	double m_2, 
	double dist_MPc, 
	double Thet,
	double Psi,
	double Phi,
	parameters *params
	){
		
	// Conversion factor for masses
	double convFact = G_CONST/pow(C_CONST, 2.0);
	
	// Convert masses into units of distance
	double m1 = m_1*convFact*SM_CONST;
	double m2 = m_2*convFact*SM_CONST;
	
	double M = comboMass(m1, m2);
	double n = nu(m1, m2);
	
	// Convert distance into meters
	double dist = dist_MPc*MPc_CONST;
	
	// Set inclination angle parameter
	params->angles[0] = Thet;
	params->angles[1] = Phi;
	params->angles[2] = Psi;
	
	// Set total time of signal
	params->totTime = total_time;
	// Set the frequency increment (meters^-1)
	params->df = 1.0/(total_time*C_CONST);
	
	// Set initial time and phase parameters
	// (time in meters, frequency in meters^-1)
	params->init[0] = init_time*C_CONST;
	params->init[1] = init_phase;
	params->init[2] = min_freq/C_CONST;
	
	// Set mass and distance parameters
	params->theta[0] = dist;
	params->theta[1] = M;
	params->theta[2] = n;
	
	// Set phase parameters
	params->psi[0] = computePhaseComps(1.7516E-1, 7.9483E-2, -7.2390E-2, n, M, 0.);
	params->psi[1] = 0.0;
	params->psi[2] = computePhaseComps(-5.1571E1, -1.7595E1, 1.3253E1, n, M, 2.);
	params->psi[3] = computePhaseComps(6.5866E2, 1.7803E2, -1.5972E2, n, M, 3.);
	params->psi[4] = computePhaseComps(-3.9031E3, -7.7493E2, 8.8195E2, n, M, 4.);
	params->psi[5] = 0.0;
	params->psi[6] = computePhaseComps(-2.4874E4, -1.4892E3, 4.4588E3, n, M, 6.);
	params->psi[7] = computePhaseComps(2.5196E4, 3.3970E2, -3.9573E3, n, M, 7.);
	
	// Set transition frequencies and width of ringdown frequency distribution (Sigma)
	// fMerge
	params->mu[0] = computeTransitionComps(2.9740E-1, 4.4810E-2, 9.5560E-2, n, M);
	// fRing
	params->mu[1] = computeTransitionComps(5.9411E-1, 8.9794E-2, 1.9111E-1, n, M);
	// Sigma
	params->mu[2] = computeTransitionComps(5.0801E-1, 7.7515E-2, 2.2369E-2, n, M);
	// fCut
	params->mu[3] = computeTransitionComps(8.4845E-1, 1.2848E-1, 2.7299E-1, n, M);
	
	// Set constant scaling amplitude
	params->amp0 = effAmp(params);
	 
}
//-------------------------------------------------------------------------------------//

// Lorentz equation for ringdown phase
double Lorentzian(parameters *params, double f){
	return params->mu[2]
			/(2.0*M_PI*(pow((f - params->mu[1]), 2.0) + pow((params->mu[2]/2.0), 2.0)));
}
// Merger -> Ringdown scaling factor for consistency
double omega_r(parameters *params){
	return (M_PI/2.0)*params->mu[2]*pow(params->mu[1]/params->mu[0], -2.0/3.0);
}

//-------------------------------------------------------------------------------------//

// Amplitude corrections:
// Inspiral
double inspiralPhenomBAmp(parameters *params, double f){
	double fDash = f/params->mu[0];	
	return pow(fDash, -7.0/6.0);	
}
// Merger
double mergerPhenomBAmp(parameters *params, double f){
	double fDash = f/params->mu[0];
	return pow(fDash, -2.0/3.0);
}
// Ringdown
double ringdownPhenomBAmp(parameters *params, double f){
	return omega_r(params)*Lorentzian(params, f);
}

// Phase of wave
double phenomBPhase(parameters *params, double f){
	
	// 2*pi*t0*f + phase0
	double sumPhase = 2.0*M_PI*params->init[0]*f + params->init[1];
	
	for(int i = 0; i<8; i++){
		
		// + psi[i]*f^((i-5)/3)
		sumPhase += params->psi[i]*pow(f, (double(i)-5.0)/3.0);
	}
	
	return sumPhase;
}

// Effects of orbital inclination
complex<double> inclinationEffects(parameters *params, complex<double> gWave){
	
	double thet = params->angles[0];
	double phi = params->angles[1];
	double psi = params->angles[2];
	
	complex<double> new_gWave;
	
	double fplus = (0.5*
						(1+cos(thet)*cos(thet))*cos(2*phi)*cos(2*psi)) 
										- (cos(thet)*sin(2*phi)*sin(2*psi));
	double fcross = (0.5*
						(1+cos(thet)*cos(thet))*cos(2*phi)*cos(2*psi)) 
										+ (cos(thet)*sin(2*phi)*cos(2*psi));
		
	//h+ is imag, hx is real
	//wave->waveform[1][i] *= fcross;
	//wave->waveform[1][i+1] *= fplus;
	
	//h+ is real, hx is img
	new_gWave.real() = gWave.real()*fplus;
	new_gWave.imag() = gWave.imag()*fcross;
	
	return new_gWave;
}

// Calculate amplitude for a given input frequency
double updatedAmplitude(parameters *params, double f, complex<double> * gw){

	complex<double> J(0.0, 1.0);
	complex<double> gw_hz;
	
	double finalAmp;
	
	if (f >= params->init[2]){
		
		if (f < params->mu[0]){
			finalAmp = inspiralPhenomBAmp(params, f);
		}
		else if (f >= params->mu[0] && f < params->mu[1]){
			finalAmp = mergerPhenomBAmp(params, f);
		}
		else if (f >= params->mu[1] && f < params->mu[3]){
			finalAmp = ringdownPhenomBAmp(params, f);
		}				
		else{
			finalAmp = 0.0;
		}
		
	}
	else{
		finalAmp = 0.0;
	}
	
	double wavePhase = phenomBPhase(params, f);
	
	if (f == 0.0){			
		*gw = 0.0;
	}else{		
		*gw = params->amp0*finalAmp*exp(J*wavePhase);
	}
	
	double gwAmp = abs(*gw);
	
	// Return the absolute magnitude in SI units (Hz^-1)
	return gwAmp/C_CONST;
}


// Generate whole wave signal
int gravitationalWave(parameters *params, vector<Signal> *sigs){
	
	// Maximum frequency
	double maxFreq = 4096.0/C_CONST;
	
	// Number of positive frequency bins
	double nPosFreq = maxFreq/params->df + 1.0;
		
	// Set up storage of both real and imaginary components of the wave
	Signal totSig;
	
	// Total number of data points is 4 times the number of positive frequencies investigated
	double nPoints = 4.0*nPosFreq + 1.0;
	
	// Populate waveforms of arrays with all zeros to allow future direct index reference
	for (int k = 0; k < int(nPoints); k++){
		
		totSig.waveform[0].push_back(0.0);
		totSig.waveform[1].push_back(0.0);
		
	}
	
	// Initialise frequency
	double freq = 0.0;
	
	// Set up imaginary number J
	const complex<double> J(0.0, 1.0);
	// Set up complex number for the wave itself and its amplitude in Hz-1
	complex<double> gravWav, gw_Hz;
	
	// Set up pointer to wave itself
	complex<double> * gWave = &gravWav;
	
	// Storage for frequency in Hz and amplitude in Hz^-1
	double freq_Hz, amp_Hz;
	
	// Storage for signal of pure amplitude
	Signal ampSig;
			
	for (int i = 0; i < nPosFreq; i++){
		
		freq = double(i)*params->df;
		
        amp_Hz = updatedAmplitude(params, freq, gWave);
		
		// Apply inclination effects to this wave
		//gravWav = inclinationEffects(params, gravWav);
		
		// Convert frequency back to Hz
		freq_Hz = freq*C_CONST;		
		gw_Hz = gravWav/C_CONST;
		
		// Save pure amplitude for positive frequencies
		ampSig.waveform[0].push_back(freq_Hz);
		ampSig.waveform[1].push_back(amp_Hz);
			
		totSig.waveform[0][2*i] = freq_Hz;
		totSig.waveform[0][2*i+1] = freq_Hz;
		
		totSig.waveform[1][2*i] = gw_Hz.real();
		totSig.waveform[1][2*i+1] = gw_Hz.imag();
		
		int k = nPoints - 2*(i + 1);
		
		totSig.waveform[0][k] = -freq_Hz;
		totSig.waveform[0][k+1] = -freq_Hz;
		
		totSig.waveform[1][k] = gw_Hz.real();
		totSig.waveform[1][k+1] = -gw_Hz.imag();	
			
	}
	
	sigs->push_back(totSig);
	
	return SUCCESS;
}