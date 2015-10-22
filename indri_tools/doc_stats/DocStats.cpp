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
      fstream out_file ( outFile.c_str(), ios::out | ios::trunc );


      int N = index->documentCount();
      cerr << "N: " << N << endl;

      indri::index::VocabularyIterator* iter = index->vocabularyIterator();

      map<lemur::api::TERMID_T, double> idf;
      iter->startIteration();
      while( !iter->finished() ) {
          indri::index::DiskTermData* entry = iter->currentEntry();
          indri::index::TermData* termData = entry->termData;
          idf[index->term(termData->term)] = log((double(N)/(termData->corpus.documentCount+1.0)));
          iter->nextEntry();
      }
      delete iter;

      lemur::api::DOCID_T documentID;
      std::vector<lemur::api::DOCID_T> documentIDs;
      
      for (documentID = 1; documentID<=N; documentID++){
          int documentLength = index->documentLength( documentID );
          const indri::index::TermList  *tl = index->termList(documentID);
          indri::utility::greedy_vector<lemur::api::TERMID_T> gv = tl->terms();
          map<lemur::api::TERMID_T, int> terms;
          for (indri::utility::greedy_vector<lemur::api::TERMID_T>::iterator gvIter = gv.begin(); gvIter < gv.end(); gvIter++)
          {
            map<lemur::api::TERMID_T, int>::iterator iter = terms.find(*gvIter);
            if (iter != terms.end() )
            {
                terms[*gvIter] = terms[*gvIter] + 1;

            }
            else {
            //cout << index->term(*gvIter) << endl;
                terms[*gvIter] = 1;
            }
          }

          double info = 0.0;
          for (map<lemur::api::TERMID_T, int>::iterator iter = terms.begin(); iter!=terms.end(); iter++){
                info = info + (double( (*iter).second) * idf[(*iter).first]);
          }

          delete tl;

          std::string documentName = collection->retrieveMetadatum( documentID, "docno" );
          out_file  << documentID << " " << documentName << " " << documentLength  << " " << terms.size() << " " << info << endl;

        } 
      out_file.close();
              
    } catch( lemur::api::Exception& e ) {
        cerr << "Something bad happened" << endl;        
    }
    return 0; 
}

