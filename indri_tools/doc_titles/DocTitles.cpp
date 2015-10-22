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

void usage() {
    cout << "Requires paramfile" << endl;
}

void getFieldText(indri::index::Index *thisIndex, int documentID, std::string documentName, std::string field, fstream &outFile) {

    // get the field ID
    int fieldID=thisIndex->field(field);

    // is this a valid field? If not, exit
    if (fieldID < 1) { return; }

    // retrieve the document vector for this document
    const indri::index::TermList *termList=thisIndex->termList(documentID);

    // ensure we have a valid term list!
    if (!termList) { return; }

    // get the vector of fields
    indri::utility::greedy_vector< indri::index::FieldExtent > fieldVec=termList->fields();

    // go through the fields (create an iterator)
    indri::utility::greedy_vector< indri::index::FieldExtent >::iterator fIter=fieldVec.begin();
    while (fIter!=fieldVec.end()) {
      // does the ID of this field extent match our field ID?
      if ((*fIter).id==fieldID) {
        // yes! We can print out the text here
        int beginTerm=(*fIter).begin;
        int endTerm=(*fIter).end;

        // note that the text is inclusive of the beginning
        // but exclusive of the ending
        outFile << documentID << " " << documentName  << " ";
        
        for (int t=beginTerm; t < endTerm; t++) {
          // get this term ID
          int thisTermID=termList->terms()[t];
          // convert the term ID to its string representation
          outFile << thisIndex->term(thisTermID) << " ";
        }
        outFile << endl;
      }

      // increment the iterator
      fIter++;
    }

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
      std:string fieldString = parameters["field"];
      std::cerr << "field: " << fieldString << endl; 
      
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
            getFieldText(index, documentID, documentName, fieldString, out_file);
      }
      out_file.close();
              
    } catch( lemur::api::Exception& e ) {
        cerr << "Something bad happened" << endl;        
    }
    return 0; 
}

