#include "indri/Parameters.hpp"
#include "indri/RelevanceModel.hpp"
#include "lemur/Exception.hpp"
#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/LocalQueryServer.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>

using namespace lemur::api;
using namespace indri::api;



void usage() {
    cout << "Requires paramfile" << endl;
}

struct sort_pred {
    bool operator()(const std::pair<string,int> &left, const std::pair<string,int> &right) {
        return left.second > right.second;
    }
};

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
        std::string queryFile = parameters["queryFile"];
        std::string termsS = parameters["terms"];
        int terms = atoi(termsS.c_str());
        
        indri::collection::Repository r;
        r.openRead( indexPath );
        indri::collection::CompressedCollection* collection = r.collection();
        indri::collection::Repository::index_state state = r.indexes();
        indri::index::Index* index = (*state)[0];
        fstream out_file ( outFile.c_str(), ios::out | ios::trunc );
        
        
        int N = index->documentCount();
        cerr << "N: " << N << endl;
        
        indri::index::VocabularyIterator* iter = index->vocabularyIterator();
        
        map<lemur::api::TERMID_T, double> idf;
        iter->startIteration();
        while( !iter->finished() ) {
            indri::index::DiskTermData* entry = iter->currentEntry();
            indri::index::TermData* termData = entry->termData;
            int tid = index->term(r.processTerm(termData->term));
            idf[tid] = log((double(N)/(termData->corpus.documentCount+1.0)));
            iter->nextEntry();
        }
        delete iter;
        
        
        // for each topic in the topic file
        
        string sLine = "";
        ifstream infile;
        ofstream outfile;
        
        outfile.open(outFile.c_str());
        infile.open(queryFile.c_str());
        bool kq = true;
        while (!infile.eof() && kq){
            getline(infile, sLine);
            int ind;
            for (ind = 0; ind < sLine.length(); ++ind) {
                switch (sLine[ind]) {
                    case '?':
                    case '-':
                    case '"':
                    case '(':
                    case ')':
                    case ',':
                        sLine[ind] = ' ';
                }
            }
            stringstream ss(sLine);
            std::vector< std::string > tokens;
            std::string queryid;
            std::string tok;
            int i=0;
            while ( ss >> tok ) {
                if (i==0){
                    queryid = tok;
                }
                else { tokens.push_back( tok ); }
                i++;
            }
            if (queryid == "") kq =false;
            // take tokens and issue as query.
            std::map<std::string,int> tokenMap;
            std::string key;
            // take each token from the list, and put it into a dictionary/map,
            for (int i = 0; i<tokens.size(); i++){
                key = r.processTerm(tokens.at(i));
                // if not in map, set to 1, else +1
                if (tokenMap.count(key)){
                    tokenMap[key]++;
                }
                else {
                    tokenMap[key] = 1;
                }
            }
            
            std::vector<std::pair<string, double> > ntfidfVec;
            std::pair <string,double> termScore;
            for (map<string,int>::iterator pos=tokenMap.begin(); pos!=tokenMap.end(); pos++){
                string term = r.processTerm(pos->first);
                if(!term.empty()){
                    double idfScore = idf[index->term(term)];
                    double ntfidfScore = (idfScore*pos->second);
                    termScore = std::make_pair (term,ntfidfScore);
                    ntfidfVec.push_back(termScore);
                }
            }
            
            std::sort (ntfidfVec.begin(), ntfidfVec.end(),sort_pred());
            
            outfile << queryid << " ";
            for(int i = 0; i < terms; i++)
            {
                if(i >= ntfidfVec.size()){
                    continue;
                }
                outfile << ntfidfVec[i].first << " ";
            }
            outfile << endl;
        }
        infile.close();
        cerr << "Read file completed!!" << endl;
        outfile.close();
    } catch( lemur::api::Exception& e ) {
        cerr << "Something bad happened" << endl;
    }
    return 0;
}

