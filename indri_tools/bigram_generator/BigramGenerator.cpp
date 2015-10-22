#include "indri/Parameters.hpp"
#include "indri/RelevanceModel.hpp"
#include "lemur/Exception.hpp"
#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/LocalQueryServer.hpp"


#include <iostream>
#include <fstream>
#include <string>

#include <cmath>

using namespace lemur::api;
using namespace indri::api;

void usage() {
	cout << "Requires paramfile" << endl;
}

bool isValidTerm(string term) {

	int minWordLength = 3;
	
	// must be valid word length
	if (term.length() < minWordLength) {
		return false;
	}
	
	int removeNumbers = 2;
	
	switch (removeNumbers) {
		case 0: //nothing removed
			return true;
		case 1: 	// must not contain only numeric characters - a mix is allowed
			for (int i = 0; i < term.length(); i++) {
				if (!isdigit(term[i])) {
				return true;
				}
			}
			return false;
		case 2: // remove any term with a number
				//2 is the value you are meant to use
		default:
			for (int i = 0; i < term.length(); i++) {
				if (isdigit(term[i])) {
					return false;
				}
			}
			return true; 
	}
	
}

int main( int argc, char** argv ) {
	
	try {
		indri::api::Parameters& parameters = indri::api::Parameters::instance();	
		parameters.loadCommandLine( argc, argv );
		// check for required parameters
		if ((! parameters.exists("index") ) ||  (!parameters.exists("outFile") )) {
		usage();
		exit (1);
		}
		std::string indexPath = parameters["index"];
		std::cerr << "index: " << indexPath << endl; 
		std::string outFile = parameters["outFile"];
		std::cerr << "outFile: " << outFile << endl; 
	  
		indri::collection::Repository r;
		r.openRead( indexPath );
		indri::collection::CompressedCollection* collection = r.collection();
		indri::collection::Repository::index_state state = r.indexes();
		indri::index::Index* index = (*state)[0];
	
		int N = index->documentCount()+1;
		int D = 1000;
		if (parameters.exists("maxdocs")){ 
			D = parameters["maxdocs"];
			if (D == 0) D = N;
		}
		cerr << "N: " << N << " D: " << D << endl;
		int cutoff = 0;
		if (parameters.exists("cutoff")){ 
			cutoff = parameters["cutoff"];
		}
		cerr << "cutoff: " << cutoff << endl;

		bool conseq = true;
		if (parameters.exists("conseq")){ 
			conseq = parameters["conseq"];
		}
		cerr << "conseq: " << conseq << endl;
		int cutoff = 0;
		if (parameters.exists("cutoff")){
			cutoff = parameters["cutoff"];
		}

		bool topic=false;
		if ((parameters.exists("docs")) && (parameters.exists("idmap"))){
			std::string docsFile = parameters["docs"];
			std::string idmapFile = parameters["idmap"];
			topic=true;
			
			std::map<std::string int> name2id;
			std::vector<std::string> docs;
			std::string line;
			while (std::getline(infile, line)){
				std::istringstream iss(line);
				std::string docname
				if (!(iss >> docname)) { break; }
				docs.pushback(docname);
			}
		}

		map< UINT64, int > bigrams; 
		map< UINT64, int >::iterator iter; 
		indri::server::LocalQueryServer local(r);
		lemur::api::DOCID_T documentID;
		std::vector<lemur::api::DOCID_T> documentIDs;
	  
		UINT64 V = index->uniqueTermCount() + 1;
		UINT64 curr_tid = 0;
		UINT64 prev_tid = 0;
		UINT64 bid;
		UINT64 total_num_bigrams = 0;

		for (int did = 1; did < D;  did++){
			documentIDs.clear();
			documentIDs.push_back(did);
			indri::server::QueryServerVectorsResponse* response = local.documentVectors( documentIDs );
			curr_tid = 0;
			prev_tid = 0;
			if ( response->getResults().size() ) {
				indri::api::DocumentVector* docVector = response->getResults()[0];
				//cout << endl;
				for( size_t i=0; i<docVector->positions().size(); i++ ) {
					int position = docVector->positions()[i];
				//if (position) cout << index->term(position) << " "; else cout << " * ";
				prev_tid = curr_tid;
				
				if (conseq){
					// if you want consequative terms only
					curr_tid = position;
				} else {
					// if you non-conseq bigrams.
					if (position > 0)
						curr_tid = position;
				}
				
				if ((prev_tid > 0) && (curr_tid>0) && (position>0)){
					bid = (prev_tid - 1) * V + curr_tid;
					if ( bigrams[bid] > 0 ) { 
						bigrams[bid]++;

					} else {
						bigrams[bid] = 1;
					}
					total_num_bigrams++;
					if (total_num_bigrams % 100000 == 0){
						cerr << "Number of Bigrams (so far): " << total_num_bigrams << " " << bigrams.size() << endl;
					}
				}
				
				// const std::string& stem = docVector->stems()[position];
				// std::cout << i << " " << position << " " << stem << std::endl;
				
			  }
			 delete docVector;
			}
			delete response;
		}
		cerr << "Number of Bigrams: " << total_num_bigrams << endl;
		cerr << "Number of Unique Bigrams: " << bigrams.size() << endl;
		
		double TV = double(local.termCount());
		//cerr << "TV: " << TV << endl;
		
		fstream out_file ( outFile.c_str(), ios::out | ios::trunc );
		
		int qid = 1;

			for (iter = bigrams.begin(); iter != bigrams.end(); iter++ ) {
				bid = iter->first; // key from the map
					curr_tid = (bid % V );
					prev_tid = (bid / V ) + 1; 
					if (iter->second > cutoff){
						std::string t1 = index->term( prev_tid );
						std::string t2 = index->term( curr_tid );
						if ( isValidTerm(t1) && isValidTerm(t2)) {    
						
							long double px = double(index->termCount( t1)) / double(index->termCount());
							long double py = double(index->termCount( t2)) / double(index->termCount());
							long double pxy = double(iter->second) / double(total_num_bigrams);
							long double apwmi =  double (iter->second) * log( pxy /( px * py ));
							out_file  << qid << " " << t1 << " " << t2 << " " <<  iter->second <<  " " <<  apwmi << endl;
							qid++;
						}
					}
			}
			   
			out_file.close();
			  
	} catch( lemur::api::Exception& e ) {
		cerr << "Something bad happened" << endl;        
	}
	return 0; 
}

