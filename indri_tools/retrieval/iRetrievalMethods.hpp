#include "indri/Repository.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/LocalQueryServer.hpp"
#include "iScoreFunctions.hpp"

struct R {
    int docid;
    float score;
    bool operator<( const R& val ) const { 
    	return score > val.score; 
    }
    R(int did, float s) : docid(did), score(s) {};
};

class iRetrievalMethod {
public:
	iRetrievalMethod(indri::collection::Repository &mRepo, indri::index::Index &mIndex, int mSetting){
		alpha = 1.0; beta = 1.0; c = 1.0; lambda = 1.0;
		modelSetting = mSetting;
        index = &mIndex;
        repo = &mRepo;
        prior = false;
        indri::server::LocalQueryServer local(mRepo);
        ttc = local.termCount();
        N = index->documentCount()+1;
        UV = index->uniqueTermCount();
        avg_doc_len = double(ttc) / double(N);
        cerr << ttc << " " << avg_doc_len << endl;
        // create lookups for doclen, ctf, cdf
        cerr << "Initializing Document Length vector:" << endl;        
        doclens.resize(N);
        for (int j=1; j<N; j++){
            doclens[j] = double(index->documentLength(j));
        }
        sFunction = NULL;
    }
	
	~iRetrievalMethod(){
			if ( sFunction != NULL ) delete sFunction;
	}
	
	virtual void setWeighting(int aWeight){
        weighting = aWeight;
        sFunction->setWeighting(aWeight);
	}
	
	virtual void setAlpha( double aAlpha ){ alpha = aAlpha; 
        sFunction->setAlpha(alpha);
	    }
	virtual void setBeta( double aBeta ){ beta = aBeta;
	    sFunction->setBeta(beta);
	     }
    virtual void setB( double aB ){ b = aB;
        sFunction->setB(b);
    }
	virtual void setC( double aC ){ c = aC;
        sFunction->setC(c);
	     }
	virtual void setLambda( double aLambda ){ lambda = aLambda;
        sFunction->setLambda(lambda);
         }
	virtual void setK1( double aK1 ){ k1 = aK1; }
	virtual void setScoreFunction( int mSetting ){modelSetting = mSetting;
	    // tbi
	    if ( sFunction != NULL ) delete sFunction;
	    switch (modelSetting){
	        case 1:
                sFunction = new iBM25sf(N, avg_doc_len);
                cerr << "Score Function : BM25" << endl;
                break;
            case 2:
                sFunction = new iPL2sf(N, avg_doc_len);
                cerr << "Score Function : PL2" << endl;
                break;
            case 3:
                sFunction = new iTFIDFsf(N, avg_doc_len);
                cerr << "Score Function : TFIDF" << endl;
                break;
            case 4:
                sFunction = new iDPHsf(N, avg_doc_len);
                cerr << "Score Function : DPH" << endl;
                break;
            case 5:
                sFunction = new iDPIsf(N, avg_doc_len);
                cerr << "Score Function : DPI - default irra1" << endl;
                break;

            case 6:
                sFunction = new iTFsf(N, avg_doc_len);
                cerr << "Score Function : TF" << endl;
                break;

            case 7:
                sFunction = new iNTFIDFsf(N, avg_doc_len);
                cerr << "Score Function : NTFIDF" << endl;
                break;

            case 8:
                sFunction = new iNTFsf(N, avg_doc_len);
                cerr << "Score Function : NTF" << endl;
                break;

            case 9:
                sFunction = new iCLMsf(N, avg_doc_len);
                cerr << "Score Function : CLM" << endl;
                break;

            case 10:
                sFunction = new iLGDsf(N, avg_doc_len);
                cerr << "Score Function : LGD" << endl;
                break;

            case 11:
                sFunction = new iPivotedTFIDFsf(N, avg_doc_len);
                cerr << "Score Function : Pivoted TFIDF" << endl;
                break;

            default:
                sFunction = new iBM25sf(N, avg_doc_len);
                cerr << "Score Function : BM25" << endl;
                break;
	    }
	    
	    };
	    
	virtual std::vector<R> scoreCollection(vector <std::string> query){	    
	    // clear results / list and clear documentation score accumulator/ hash
        scores.clear();
	    results.clear();
	    // for each query term:
	    for(std::vector<std::string>::iterator it = query.begin(); it != query.end(); ++it) {
            // get posting list
            std::string t = *it;
            std::string st = repo->processTerm( t );
            int tid = index->term(st);
            // get ctf, cdf from lookup
            //cerr << t << " " <<st << " " << tid << endl;
            // score the term if it appears in the index.
            if (tid > 0){
                double ctf = double(index->termCount(st));
                double cdf = double(index->documentCount(st));
                
                // not sure what the scopedlock does...
                indri::thread::ScopedLock( index->iteratorLock() );
                indri::index::DocListIterator* iter = index->docListIterator( st );
                indri::index::DocListIterator::DocumentData* entry;
                
                // for each posting (docid)
                for( iter->startIteration(); iter->finished() == false; iter->nextEntry() ) {
                    // get score (for term give docid)
                    entry = iter->currentEntry();
                    // accumulate score for docid
                    // call score term function term_count, doc_len, ctf, cdf,  
                    double doclen = doclens[entry->document];
                    scores[entry->document] += sFunction->termWeight( double(entry->positions.size()), doclen, ctf, cdf);
                }
                delete iter;
            }
        }
	    
	    // transfer scores from accumulator to list
	    map<int, double>::const_iterator siter;
            for (siter=scores.begin(); siter != scores.end(); ++siter) {
                // might want to add in document dependent score here...
                int docid = siter->first;
                double score = siter->second;
                if (prior){
                    score = documentPriorScore(docid, score);
                }
                results.push_back( R(docid,score));
            }
      
         // sort list - return results.
	     std::sort( results.begin(), results.end() );   
	      
         return results;
  
	};
	
	virtual double documentPriorScore(int docid, double score){
        // probably need to create a documentprior class - and create a doc prior object in the constructor, and then
        // use the doc prior onbject (as a lookup) to get the prior and do the combination with the existing score.
        return score;
        
	};

	iScoreFunction *sFunction;
	double alpha; 
	double beta; 
	double b;
	double c;
	double lambda;
	double k1;
    int weighting;
	int modelSetting;
    indri::index::Index* index;
    indri::collection::Repository* repo;
    std::map<int,double> scores;
    std::vector<R> results;
    std::vector<double> doclens;
    std::vector<double> ctfs;
    std::vector<double> cdfs;
    UINT64 ttc;
    UINT64 N;
    UINT64 UV;
    double avg_doc_len;
    bool prior;
};



class iLanguageModelRetrievalMethod: public iRetrievalMethod {
public:
    
    iLanguageModelRetrievalMethod(indri::collection::Repository &mRepo, 
        indri::index::Index &mIndex, int mSetting): iRetrievalMethod(mRepo, mIndex, mSetting){
        };
    
	virtual std::vector<R> scoreCollection(vector <std::string> query){	    
	    // clear results / list and clear documentation score accumulator/ hash
        scores.clear();
	    results.clear();
	    // for each query term:
        double query_adj = 0.0;
        int query_length = 0;
	    for(std::vector<std::string>::iterator it = query.begin(); it != query.end(); ++it) {
            // get posting list
            std::string t = *it;
            std::string st = repo->processTerm( t );
            int tid = index->term(st);
            // get ctf, cdf from lookup
            //cerr << t << " " <<st << " " << tid << endl;
            // score the term if it appears in the index.
            if (tid > 0){
                double ctf = double(index->termCount(st));
                double tprob = double(ctf)/double(ttc);
                query_adj += sFunction->termAdj(tprob);
                query_length += 1;
                
                //cout << query_length << " " << ctf << " " << ttc << " " << tprob << endl;
                // not sure what the scopedlock does...
                indri::thread::ScopedLock( index->iteratorLock() );
                indri::index::DocListIterator* iter = index->docListIterator( st );
                indri::index::DocListIterator::DocumentData* entry;
                // for each posting (docid)
                for( iter->startIteration(); iter->finished() == false; iter->nextEntry() ) {
                    // get score (for term give docid)
                    entry = iter->currentEntry();
                    // accumulate score for docid
                    double doclen = doclens[entry->document];
                    scores[entry->document] += (sFunction->termWeight( double(entry->positions.size()), doclen, tprob, 0.0) - sFunction->termWeight( 0.0, doclen, tprob, 0.0));
                }
                delete iter;
            }
        }
	    
	    // transfer scores from accumulator to list
	    map<int, double>::const_iterator siter;
            for (siter=scores.begin(); siter != scores.end(); ++siter) {
                // might want to add in document dependent score here...
                int docid = siter->first;
                double score = siter->second;
                score = score + (query_adj - (double(query_length) * sFunction->docAdj(doclens[docid])));
                
                if (prior){
                    score = documentPriorScore(docid, score);
                }
                results.push_back( R(docid,score));
            }
      
         // sort list - return results.
	     std::sort( results.begin(), results.end() );   
	      
         return results;
  
	};
	
	virtual void setScoreFunction( int mSetting ){
	    modelSetting = mSetting;
        if ( sFunction != NULL ) delete sFunction;
	    switch (modelSetting){
	        case 1:
               sFunction = new iBayesSF(N, avg_doc_len);
                cerr << "Score Function : LM Bayes Smoothing" << endl;
                break;
            case 2:
                sFunction = new iJelinekMercerSF(N, avg_doc_len);
                cerr << "Score Function : LM JelinekMercer Smoothing" << endl;
                break;
                
            case 3:
                sFunction = new iLaplaceSF(N, avg_doc_len, UV);
                cerr << "Score Function : LM Laplace Smoothing" << endl;
                break;
            
            default:
                sFunction = new iBayesSF(N, avg_doc_len);
                cerr << "Score Function : LM Bayes Smoothing" << endl;
                break;
	    }
	    
	};
	


};