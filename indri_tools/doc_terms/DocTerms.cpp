#include "indri/Parameters.hpp"
#include "indri/RelevanceModel.hpp"
#include "lemur/Exception.hpp"
#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/QueryEnvironment.hpp"
#include <iostream>

#include <iostream>
#include <fstream>
#include <string>

#include <cmath>

using namespace lemur::api;
using namespace indri::api;

struct Terms {
    int tid;
    std::string term;
    float score;
    bool operator<( const Terms& val ) const {
    	return score < val.score;
    }
    Terms(int id, std::string t, float s) : tid(id), term(t), score(s) {};
};


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




void usage() {
    cout << "Requires paramfile" << endl;
}

vector<Terms> getTextFromField(indri::index::Index *thisIndex, int documentID, std::string documentName, std::string field, int numterms, fstream &outFile) {
    
    // get the field ID
    int fieldID=thisIndex->field(field);
    
    // is this a valid field? If not, exit
    if (fieldID < 1) {return vector<Terms>(); }
    
    // retrieve the document vector for this document
    const indri::index::TermList *termList=thisIndex->termList(documentID);
    
    // ensure we have a valid term list!
    if (!termList) {return vector<Terms>(); }
    
    // get the vector of fields
    indri::utility::greedy_vector< indri::index::FieldExtent > fieldVec=termList->fields();
    
    // go through the fields (create an iterator)
    indri::utility::greedy_vector< indri::index::FieldExtent >::iterator fIter=fieldVec.begin();
    vector <Terms> terms = vector<Terms>();
    while (fIter!=fieldVec.end()) {
        // does the ID of this field extent match our field ID?
        if ((*fIter).id==fieldID) {
            // yes! We can print out the text here
            int beginTerm=(*fIter).begin;
            int endTerm=(*fIter).end;
            
            // note that the text is inclusive of the beginning
            // but exclusive of the ending
            vector <Terms> terms;
            terms.clear();
            
            for (int t=beginTerm; t < endTerm; t++) {
                // get this term ID
                int thisTermID=termList->terms()[t];
                std::string termSTR = thisIndex->term(thisTermID);
                if (isValidTerm(termSTR)) {
                    terms.push_back(Terms(thisTermID, termSTR, thisIndex->documentCount(termSTR))) ;
                    
                }
            }
            if(terms.size() == 0) return vector<Terms>();
            std::sort( terms.begin(), terms.end() );
            return terms;
        }
        fIter++; // increment the iterator
    }
    return vector<Terms>();
}


void getFieldText(indri::index::Index *thisIndex, int documentID, std::string documentName, std::string field, std::string field2, int numterms, fstream &outFile) {
    
    // get the field ID
    int fieldID=thisIndex->field(field);
    
    // is this a valid field? If not, exit
    if (fieldID < 1) {return; }
    
    // retrieve the document vector for this document
    const indri::index::TermList *termList=thisIndex->termList(documentID);
    
    // ensure we have a valid term list!
    if (!termList) {return; }
    
    // get the vector of fields
    indri::utility::greedy_vector< indri::index::FieldExtent > fieldVec=termList->fields();
    
    // go through the fields (create an iterator)
    indri::utility::greedy_vector< indri::index::FieldExtent >::iterator fIter=fieldVec.begin();
    vector <Terms> terms = vector<Terms>();
    outFile << documentID << " " << documentName  << " ";
    std::cout  << documentID << " " << documentName  << " ";
    while (fIter!=fieldVec.end()) {
        // does the ID of this field extent match our field ID?
        if ((*fIter).id==fieldID) {
            // yes! We can print out the text here
            int beginTerm=(*fIter).begin;
            int endTerm=(*fIter).end;
            
            // note that the text is inclusive of the beginning
            // but exclusive of the ending
            terms.clear();
            
            for (int t=beginTerm; t < endTerm; t++) {
                // get this term ID
                int thisTermID=termList->terms()[t];
                std::string termSTR = thisIndex->term(thisTermID);
                if (isValidTerm(termSTR)) {
                    terms.push_back(Terms(thisTermID, termSTR, thisIndex->documentCount(termSTR))) ;
                    
                }
            }
            break;
        }
        fIter++; // increment the iterator
    }
    
    std::sort( terms.begin(), terms.end() );
    int s = numterms;
    vector<Terms> padTerms = vector<Terms>();
    set<Terms> allTermsSet (terms.begin() , terms.end());
    
    if (allTermsSet.size() < s){
        padTerms = getTextFromField(thisIndex, documentID, documentName, field2, numterms, outFile);
        std::copy(padTerms.begin(),padTerms.end(), std::inserter(allTermsSet, allTermsSet.end()));
        if (allTermsSet.size() < s){
            s = allTermsSet.size();
        }
    }
    
    vector<Terms> allTerms (allTermsSet.begin(), allTermsSet.end());
    std::sort(allTerms.begin(), allTerms.end());
    
       for(int i=0; i<s; i++){
            outFile << allTerms[i].term << " ";
            std::cout << allTerms[i].term << " ";
        
        }
    //
    //    int sP = numterms - s;
    //    if (padTerms.size() < sP) {
    //        std::cout <<endl;
    //        outFile << endl;
    //        return;
    //    }
    //
    //    for(int i=0; i<sP; i++){
    //        outFile << padTerms[i].term << " ";
    //        std::cout << padTerms[i].term << " ";
    //    }
    
    std::cout <<endl;
    outFile << endl;
    
    // destroy the term list object
    delete termList;
    termList=NULL;
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
        std::string fieldString = parameters["field"];
        std::cerr << "field: " << fieldString << endl;
        std::string field2String = parameters["field2"];
        std::cerr << "field2: " << field2String << endl;
        int num_terms = parameters["numTerms"];
        std::cerr << "numTerms: " << num_terms << endl;
        indri::collection::Repository r;
        r.openRead( indexPath );
        indri::collection::CompressedCollection* collection = r.collection();
        indri::collection::Repository::index_state state = r.indexes();
        indri::index::Index* index = (*state)[0];
        fstream out_file ( outFile.c_str(), ios::out | ios::trunc );
        
        
        int N = index->documentCount();
        cerr << "N: " << N << endl;
        
        for (int documentID = 1; documentID<=N; documentID++){
            std::string documentName = collection->retrieveMetadatum( documentID, "docno" );
            getFieldText(index, documentID, documentName, fieldString, field2String, num_terms, out_file);
        }
        out_file.close();
        
    } catch( lemur::api::Exception& e ) {
        cerr << "Something bad happened" << endl;
    }
    return 0;
}

