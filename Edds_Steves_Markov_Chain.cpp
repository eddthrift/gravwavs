#include <iostream>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <string>
#include <iomanip>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "pe_gwLikelihood.h"


//Prototype for the likelihood calculation function
long double likelihood(double, double, std::string, long double (*function)(double,double,std::string));
double gaussian(double, double, double, double, double);
double prior(double, double, double, double);


int main() {

	//Defines the mass of the sun
	const double mSolar = 1.989e30;


	/*Sets up a random number generator to seed the other two generators, and
	/ seeds this based on the system time.*/
	gsl_rng * seedGen = gsl_rng_alloc(gsl_rng_taus);
	gsl_rng_set(seedGen,(long unsigned int) time(NULL));
			

	/*Initialises random number generators to be used in the Markov-Chain
	/ routine, and seeds them using values from the previous RNG.*/
	gsl_rng * normGen = gsl_rng_alloc(gsl_rng_taus);
	gsl_rng_set(normGen, gsl_rng_get(seedGen));
	gsl_rng * rGen = gsl_rng_alloc(gsl_rng_taus);
	gsl_rng_set(rGen, gsl_rng_get(seedGen));
	gsl_rng * startGen = gsl_rng_alloc(gsl_rng_taus);
	gsl_rng_set(startGen, gsl_rng_get(seedGen));

	
	/*Takes an input for the standard deviation, sigma of distribution
	/ which is sampled to find the next ma and mb value [N(0,nSigma)]. The
	/ distribution from which the random number is drawn.*/
	std::cout << "Enter the standard deviation, sigma, of the distribtuion "
			  << "used to advance the Markov Chain routine. This should be "
			  << "given in solar masses."
			  << std::endl;
	double sigma;
	std::cin >> sigma;
	
	double nSigma = sigma*mSolar;

	//Takes mass limits for the prior function
	double mLower,mUpper;
	
	std::cout<< "Enter the mass upper limit in solar masses:" << std::endl;
	std::cin >> mUpper;
	std::cout<< "Enter the mass lower limit in solar masses:" << std::endl;
	std::cin >> mLower;	
	
	mLower*=mSolar;
	mUpper*=mSolar;
	
	//Takes an input for the number of samples used in the Monte Carlo routine
	std::cout<< "Enter the number of Monte-Carlo samples:" << std::endl;
	int N;
	std::cin >> N;


	//Declares the variables used throughout the routine
	std::string fileName = "signal.txt";
	double ma, maProposal, mb, mbProposal;
	double ma1, ma2, mb1, mb2;
	long double p, pProposal, alpha;
	double nZeroMA, nZeroMB, rZero;
	int acceptCount = 1;


	//Opens the output text file
	FILE * outFile;
	outFile = fopen("2DMonte.txt","w");
	

	//Sets the starting ma and mb values for the routine as random integers.
	ma = gsl_rng_uniform(startGen)*(mUpper-mLower)+mLower;
	mb = gsl_rng_uniform(startGen)*(mUpper-mLower)+mLower;
	

	/*Loops over the number of iterations specified by the input. In each run,
	/ the likelihood function is evaluated at (ma,mb). Then, one of the RNGs is
	/ used to draw values from a normal distribution of width nSigma. The
	/ values obtained in this way are then used to find a new trial (ma,mb) pair
	/ [(ma',mb') in Veitch's notation]. The likelihood at this new point is
	/ then found. This is used to evaluate the acceptance value, alpha, which
	/ is compared to a random number drawn from a uniform distribution, to
	/ decide if (ma',mb') is accepted as the new (ma,mb). Either way, the (ma,mb)
	/ is output to the file.*/
	for(int i = 1; i <= N; i++) {
		
		double gamma;

		if((acceptCount > 2) && (acceptCount%2==0)){
			gamma = 1;
		}
		else{
			gamma = gsl_ran_gaussian(normGen, 2.38/sqrt(2*2));		
		}

		p = likelihood (ma,mb,fileName,PdhFunction)*prior(ma,mb,mUpper,mLower);

		nZeroMA = gsl_ran_gaussian(normGen, nSigma);
		nZeroMB = gsl_ran_gaussian(normGen, nSigma);
		
		if(acceptCount > 2){
			maProposal = ma + gamma*(ma2 - ma1);
			mbProposal = mb + gamma*(mb2 - mb1);
		}
		else{
			maProposal = ma + nZeroMA;
			mbProposal = mb + nZeroMB;
		}
		pProposal = likelihood(maProposal,mbProposal,fileName,PdhFunction)*prior(maProposal,mbProposal,mUpper,mLower);

		alpha = pProposal/p;
		
		rZero = gsl_rng_uniform_pos(rGen);			
		
		if(acceptCount = 1){
			ma2 = ma;
			mb2 = mb;
		}
		
		if(acceptCount = 2){
			ma1 = ma;
			mb1 = mb;
		}

		if(alpha > rZero) {
			
			if(acceptCount > 2){							
				ma2 = ma1;
				mb2 = mb1;
				ma1 = ma;
				mb1 = mb;
			}

			ma = maProposal;
			mb = mbProposal;			

		}

		if (i%(N/20)==0) {
			std::cout << (i*100)/N << "% complete." << std::endl;
		}
		
		if (i%100==0){
			fprintf(outFile,"%.15g,%.15g\n",ma,mb);
		}
		
	}


	/*Closes the output file, then frees the memory associated with the random
	/ number generators.*/
	fclose(outFile);

	gsl_rng_free(seedGen);
	gsl_rng_free(normGen);
	gsl_rng_free(rGen);
	

}

/*Defines a general likelihood function, which can take any function containing the appropriate arguments as 
/ input.*/
long double likelihood(double ma, double mb, std::string file, long double (*function)(double,double,std::string)) {

	double g;
    g = (*function)(ma,mb,file);
    return (g);

}


/*Defines a prior function which is a step of height 1, centred on the origin,
/ with a width of w.*/
double prior(double ma, double mb, double mUpper, double mLower) {

	if((ma < mUpper && ma > mLower) 
	&& (mb < mUpper && mb > mLower)) { return 1/((mUpper-mLower)*(mUpper-mLower)); }

	else { return 0; }

}
