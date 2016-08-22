#include "indri/Parameters.hpp"
#include "indri/RelevanceModel.hpp"
#include "lemur/Exception.hpp"
#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"
#include "iRetrievalMethods.hpp"

#include <iostream>
#include <fstream>
#include <string>

using namespace lemur::api;
using namespace indri::api;


void usage() {

    cout << "Requires paramfile" << endl;
    
}

double getDoubleValue(indri::api::Parameters& parameters, std::string paramStr, double defaultVal){
    double val = defaultVal;
    if (parameters.exists(paramStr)){
          val = parameters[paramStr];
      }
      return val;
}

int getIntValue(indri::api::Parameters& parameters, std::string paramStr, int defaultVal){
    int val = defaultVal;
    if (parameters.exists(paramStr)){
          val = parameters[paramStr];
      }
      return val;
}



int main( int argc, char** argv ) {
    
    try {
      indri::api::Parameters& parameters = indri::api::Parameters::instance();
      parameters.loadCommandLine( argc, argv );
      // check for required parameters
      if (! parameters.exists("index") ) {
        usage();
        exit (1);
      }
      std::string indexPath = parameters["index"];
      std::string queryFile = parameters["queryFile"];
      std::string resultFile = parameters["resultFile"];
      
      int model = getIntValue(parameters, "model", 0);
      int resultCount = getIntValue(parameters, "resultCount", 1000);
      int weighting = getIntValue(parameters, "weighting", 0);
        
      std::cerr << "indexPath: " << indexPath << endl; 
      std::cerr << "queryFile: " << queryFile << endl;
      std::cerr << "resultCount: " << resultCount << endl;
      
      indri::collection::Repository r;
      r.openRead( indexPath );
      indri::collection::CompressedCollection* collection = r.collection();
      indri::collection::Repository::index_state state = r.indexes();
      indri::index::Index* index = (*state)[0];
      std::cout << "TOTAL" << " " << index->termCount() << " " << index->documentCount() << std::endl;
      
      cerr<< "Parameter File Read" << endl;
            
      int t = getIntValue(parameters, "type", 0);
      
      iRetrievalMethod *irm;
      if (t==0)      
          irm = new iRetrievalMethod(r, *index, 0);
      else
          irm = new iLanguageModelRetrievalMethod(r, *index, 0);
          
      irm->setScoreFunction(model);
      irm->setB(getDoubleValue(parameters, "b", 1.0));
      irm->setC(getDoubleValue(parameters, "c", 1.0));
      irm->setBeta(getDoubleValue(parameters, "beta", 1.0));
      irm->setLambda(getDoubleValue(parameters, "lambda", 0.5));
      irm->setAlpha(getDoubleValue(parameters, "alpha", 1.0));
      irm->setWeighting(weighting);

      string sLine = "";
      ifstream infile;
      fstream result_file ( resultFile.c_str(), ios::out | ios::trunc );
      
      
      infile.open(queryFile.c_str());
      bool kq = true;
      while (!infile.eof() && kq){
          getline(infile, sLine);
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
          cerr << "QID: -" <<  queryid << "-" << endl;
          if (queryid == "") kq =false;
          // take tokens and issue as query.
          std::vector<R> results = irm->scoreCollection(tokens);
          
          int n = results.size();
           if (n > resultCount) n = resultCount;
           for (int i=0; i<n; i++){
               result_file << queryid << " Q0 " << collection->retrieveMetadatum( ( results[i].docid ), "docno" ) << " " << i+1 << " " << results[i].score << " EXP" << endl;
           }
          
      }

      infile.close();
      cerr << "Read file completed!!" << endl;
      result_file.close();
    } catch( lemur::api::Exception& e ) {
        cerr << "Something bad happened" << endl;
        
    }
    
    
    return 0; 
 
 }

