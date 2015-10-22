#include <cmath>


class iScoreFunction {
public:
	iScoreFunction(double aN, double aDL){
		alpha = 1.0; 
		beta = 1.0; 
		b = 0.75;
		lambda = 0.5; 
		c = 1.0; 
		avgDocLen = aDL;
		N = aN;
        k1 = 1.2;
        k3 = 7.0;
        qtc = 1.0;
        weighting  = 0;
        updateQTW();
	};

	~iScoreFunction(){};
	
	virtual double termWeight(double tCount, double dLen, double tProb){ return 1.0; }
	virtual double termWeight(double tCount, double dLen, double cft, double dft){ return 1.0; }
	virtual void setNormalisation() {};
    virtual void setWeighting(int aWeight){
        weighting = aWeight;
    };
	
	virtual void setAlpha( double aAlpha ){ 
		alpha = aAlpha;
		cerr << "alpha: " << alpha << endl;
        
	};
	virtual void setBeta( double aBeta ){
		beta = aBeta;
		cerr << "beta: " << beta << endl;
        
	};

	virtual void setB( double aB ){
                b = aB;
                cerr << "b: " << b << endl;
                
    };

	virtual void setLambda( double aLambda ){ 
	    if ((aLambda >= 0.0) && (aLambda <= 1.0)){ 
		    lambda = aLambda;
	    }
		cerr << "lambda: " << lambda << endl;
        
	};
	virtual void setC( double aC ){ 
		c = aC;
		cerr << "c: " << c << endl;
        
	};
	
	virtual void setK1( double aK1 ){
                k1 = aK1;
                cerr << "k1: " << k1 << endl;
	            
	};

    virtual void setK3( double aK3 ){
        k3 = aK3;
        cerr << "k1: " << k3 << endl;
        updateQTW();
	};
    
    
    
    virtual void setQTC( double aQTC ){
        qtc = aQTC;
        cerr << "qtc: " << qtc << endl;
        updateQTW();
	};

    virtual void updateQTW(){
        qtw = ((k3 + 1.0)+qtc)/(k3+qtc);
        cerr << "qtw: " << qtw << endl;

	};

    
    
	virtual void setNumberOfDocuments(int aN){ N = aN; };


	double stirlingFormula(double n, double m){
		return ((( m + 0.5 ) * (log(n/m)/log(2.0)) ) + ((n-m) * (log(n)/log(2.0)) ));
	};
	
	double tfNormalized(double tCount, double dLen){
		double tmp;
		tmp = tCount * (log( 1.0 + c * (avgDocLen / dLen))/ log(2.0) );
		return tmp;		
	};
	
	
	double tfNormalizede(double tCount, double dLen){
		double tmp;
		tmp = tCount * (log( 1.0 + c * (avgDocLen / dLen))/log(M_E));
		return tmp;		
	};
	
	// aDft is the document freq of t
	// aCft is the collection freq of t
	double eNormalized(double aDft, double aCft){
		return (N * ( 1.0 - pow( (1.0-( aDft / N )), aCft) ) );
	return 1.0;
	}
	
	virtual double termAdj(double tprob){
        return 1.0;
	}
	
	virtual double docAdj(double docLen){
        return 1.0;
	}

protected:	
	double alpha;
	double beta;
	double b;
	double lambda;
	double c;
	double k1;
    double qtw;
    double qtc;
    double k3;
	double avgDocLen; // average length of a documnet
	double N; // number of documents
    int weighting;
};



class iBM25sf: public iScoreFunction{
public:
	iBM25sf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){ };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
        
		double wtd;
		double K;
		K = (1.0-b) + b * (dLen / avgDocLen);
		wtd = log((N+1.0)/(dft+1.0));
		wtd = qtw * wtd * ( (k1 + 1.0) * tCount)  /(k1 * K + tCount );
        return wtd;
	};

};

class iPL2sf: public iScoreFunction{
public:
	iPL2sf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){ };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
		double wtd;
		double tfn;
		double lam;
		tfn =  tfNormalized( tCount, dLen );
		lam = cft / N;
		wtd = ( lam + ( ( 1.0 / 12.0 ) * tfn ) - tfn  ) * ( log( M_E ) / log( 2.0 ) ); 
		wtd = wtd + ( tfn * ( log( tfn / lam ) / log( 2.0 ) ) );
		wtd = wtd + ( 0.5 * ( log( 2.0 * M_PI * tfn ) / log( 2.0 ) ) );
		wtd = wtd * (1.0 /( tfn + 1.0)); // same as in BL2
		return wtd;
					
	};
};


class iLGDsf: public iScoreFunction{
public:
	iLGDsf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){ };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
		double wtd;
		double tf;
		double freq;
		tf = log(tCount * (1.0+ ((c * avgDocLen)/dLen)))/log(2.0);
		freq = (1.0 * dft) /(1.0 * N);
		wtd = log((freq + tf)/freq)/log(2.0);
		return wtd;
	};
};







class iDPHsf: public iScoreFunction{
public:
	iDPHsf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){ };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
		double wtd;
        double ex1;
        double ex2;
        double pi = 3.141;
        double ex3;

        ex1 = (tCount * pow((1.0- tCount/dLen),2))/(tCount+1.0);
        ex2 = log(tCount * avgDocLen * N / ( dLen * cft)) / log(2.0);
        ex3 = 0.5 * log(2.0 * pi*tCount*(1.0 - tCount/dLen))/log(2.0);
        wtd = ex1 * ex2 + ex3;
		return wtd;
					
	};
};




class iDPIsf: public iScoreFunction{
public:
	iDPIsf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){
    };
    
    virtual void setWeighting(int aWeight){
        weighting = aWeight;
        switch(weighting){
            case 0:
                    cerr << "Score Function : DPI - irra12a" << endl;
                    break;
            case 1:
                    cerr << "Score Function : DPI - irra12b" << endl;
                    break;
            case 2:
                    cerr << "Score Function : DPI - bayes" << endl;
                    break;
            case 3:
                    cerr << "Score Function : DPI - info change" << endl;
                    break;
            default:
                    cerr << "Score Function : DPI - irra12a" << endl;
                    weighting = 0;
                    break;
                };
        
        
    };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
        double ex1;
        double ex2;
        double eij;
        double eij_p;
        double aij;
        double bij;
        double ex3;
        double wij;
        

        eij = (cft * dLen) / (N * avgDocLen);
        //double idf = log((N + 1.0)/dft);
        
        if (tCount > eij) {
            switch(weighting){
                case 0:
                        wij =log((((tCount - eij)*(tCount - eij))/eij)+1.0)/log(2.0);
                        break;
                case 1:
                        wij = log(((tCount-eij)/sqrt(eij ))+1.0)/log(2.0);
                        break;
                case 2:
                        wij = log(((tCount - eij)/eij)+1.0)/log(2.0);
                        break;
                case 3:
                        aij = (dLen - tCount)/dLen;
                        bij = (2.0/3.0)*(tCount+1.0)/tCount;
                        ex3 = pow(aij,0.75) * pow(bij,0.25);
                        eij_p = ((cft+1.0) * (dLen + 1.0 )) / ( (N* avgDocLen) + 1.0);
                        ex1 = (tCount + 1.0) * (log((tCount+1.0)/sqrt(eij_p) )/log(2.0) );
                        ex2 = tCount * log(tCount/sqrt(eij))/log(2.0);
                        wij = (ex1-ex2)*ex3;
                        break;
                
            }
            
        }
        else {
            wij = 0;
        }
        
		return wij;
					
	};
};




class iTFIDFsf: public iScoreFunction{
public:
	iTFIDFsf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){ 
	    normalisation = false;
	    };
	
	virtual double termWeight(double tCount, double dLen, double cft, double dft){
		double wtd;
	
		double idf = log((N + 1.0)/dft);
		
		if (normalisation) {
			wtd = tCount * idf / ((1.0-b) + b * dLen/avgDocLen);
		}
		else {
			wtd = tCount * idf;
		}
		
		return wtd;
	};
	
	void setNormalisation() {
		normalisation = true;
	}

protected:
	bool normalisation;
};


class iPivotedTFIDFsf: public iScoreFunction{
public:
	iPivotedTFIDFsf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){

	    };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
		double wtd;
		double idf = log((N + 1.0)/dft);
		wtd = tCount * idf / ((1.0-b) + b * dLen/avgDocLen);
		return wtd;
	};

};




class iNTFIDFsf: public iScoreFunction{
public:
	iNTFIDFsf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){

	    };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
		double wtd;
		double idf = log((N + 1.0)/dft);
		wtd = (tCount/dLen) * idf;
		return wtd;
	};

};


class iTFsf: public iScoreFunction{
public:
	iTFsf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){
	    };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
		double wtd;
		wtd = tCount;
        return wtd;
	};

};


class iNTFsf: public iScoreFunction{
public:
	iNTFsf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){
	    };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
		double wtd;
		wtd = tCount/dLen;
        return wtd;
	};

};

class iCLMsf: public iScoreFunction{
public:
	iCLMsf(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){
	    };

	virtual double termWeight(double tCount, double dLen, double cft, double dft){
		double wtd;
		wtd = 1.0;
        return wtd;
	};

};




class iBayesSF: public iScoreFunction{
public:
	iBayesSF(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){

	};
	
	virtual double termWeight(double tCount, double dLen, double cft, double dft){
	    // cft is the tprob here.
			return log((tCount + beta*cft)/(dLen + beta));
	}
	
	virtual double termAdj(double tprob){
        return log(beta * tprob);
	}
	
	virtual double docAdj(double docLen){
        return log(docLen+beta);
	}
	
};


class iJelinekMercerSF: public iScoreFunction{
public:
	iJelinekMercerSF(double aN, double aAvgDocLen):iScoreFunction(aN, aAvgDocLen){

	};
	
	virtual double termWeight(double tCount, double dLen, double cft, double dft){
	        // cft is the tprob here.
			return log( (1.0-lambda)*(tCount/dLen) +  (lambda)*(cft));
	}
	
	virtual double termAdj(double tprob){
        return log((lambda)*(tprob));
	}
	
	virtual double docAdj(double docLen){
        return 0;
	}
	
};


class iLaplaceSF: public iScoreFunction{
public:
	iLaplaceSF(double aN, double aAvgDocLen, double aUV):iScoreFunction(aN, aAvgDocLen){
        UV = aUV;
	};
	
	virtual double termWeight(double tCount, double dLen, double cft, double dft){
	    // cft is the tprob here.
			return log( (tCount+(alpha/UV))/(dLen+alpha));
	}
	
	virtual double termAdj(double tprob){
        return log((alpha/UV));
	}
	
	virtual double docAdj(double docLen){
        return log(docLen+alpha);
	}
protected:	
    double UV; 
};
