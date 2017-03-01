class Binary {

	public:
		
		Binary(double m1, double m2, double a);

		double getMass(int n);
		double getRadius(int n);
		double getSeparation(int n);
		double getRatio();
		
		void setMass(int n, double m);
		void setSeparation(int n, double a);

		void updateRadii();

		double rocheLobe();
		double keplerFrequency();
		double mixingFrequency(int n);
		double mergeTime();

		void evolveMainSequence();
		void evolveWolfRayet();
		void evolveSupernova();

		bool checkRocheLobe();
		bool checkHomogeneousMixing();
		bool checkPairInstability();
		bool checkMergeTime();
			

	private:
		
		double fm1;
		double fm2;
		double fr1;
		double fr2;
		double fa;		

}
