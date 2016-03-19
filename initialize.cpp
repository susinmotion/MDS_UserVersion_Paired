#include "trie.h"
#include <string>
#include <map>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <iostream>
using namespace std;
#define BUFMAX 100

string reverseComplement(string sequence){
    map <char, char> complements;
    complements['A']='T';
    complements ['T']='A';
    complements ['G']='C';
    complements ['C']='G';
    complements ['N']='N';

    string reverseComplement="";
 
    for (int i = sequence.length()-1; i>=0; --i){
        reverseComplement = reverseComplement+complements[sequence[i]];
    }
    return reverseComplement;
}

map <string, vector <string> > readConfig(string filename){ //read config file into map of user defined variables
    map <string, vector<string> > userDefinedVariables;
    userDefinedVariables.clear();
    ifstream infile(filename.c_str());
    if (infile.is_open()){
        string key;
        string allValues;
        string value;
        while (infile >> key >> allValues ){
            vector <string> values;
            cout <<key<<" "<<allValues<<endl;
            stringstream ss(allValues);
            while (getline (ss, value, ',')){
                values.push_back(value);
            }

            pair <string, vector<string> > varPair = make_pair (key, values);
            userDefinedVariables.insert(varPair);
        }
    }
    else {
        cout<<"error opening config file "<<filename<<endl;
    }
    return userDefinedVariables;
}
    
Trie* readFileIntoTrie(string filename){//set constants based on config file
    map <string, vector <string> > userDefinedVariables=readConfig(filename);
    cout<<" done reading config "<<endl;
    const int BARCODE_LENGTH =atoi(userDefinedVariables["BARCODE_LENGTH"][0].c_str() );
    const vector <string> GENES = userDefinedVariables["GENES"];
    vector <string> FORWARD_ALIGN_SEQ=userDefinedVariables["FORWARD_ALIGN_SEQ"];
    vector <string> REVERSE_ALIGN_SEQ=userDefinedVariables["REVERSE_ALIGN_SEQ"];
    vector <string> TARGET=userDefinedVariables["TARGET"];
    vector <string> PHASE_SHIFTS=userDefinedVariables["PHASE_SHIFTS_REV_TO_FORWARD"];
    const vector <string> FILENAMES =userDefinedVariables["FILENAMES"];
    vector <int> TARGET_LENGTHS;
    vector <int> THRESHOLDS_OF_IMPORTANCE;

    for (int i=0; i<userDefinedVariables["THRESHOLD_OF_IMPORTANCE"].size(); ++i){
        THRESHOLDS_OF_IMPORTANCE.push_back(atoi(userDefinedVariables["THRESHOLD_OF_IMPORTANCE"][i].c_str()));
    }

    for (int i=0; i<TARGET.size(); ++i){
        TARGET_LENGTHS.push_back(TARGET[i].length());
    }
    
    int numberOfROIs=FORWARD_ALIGN_SEQ.size();
    int numberOfPhases=atoi(userDefinedVariables["MAX_PHASE"][0].c_str() )+1;

    map<int, int> empty_map;
    vector <map <int, int> > PHASE_MAPS=vector <map <int, int> >(numberOfROIs, empty_map);
    string keyvalue;
    for (int i=0; i<PHASE_SHIFTS.size(); ++i){
        stringstream ss(PHASE_SHIFTS[i]);
        while (getline (ss, keyvalue, '|')){
            int key =atoi((keyvalue.substr(0, keyvalue.find(":"))).c_str());
            int value=atoi((keyvalue.substr(keyvalue.find(":")+1,keyvalue.length())).c_str());
            PHASE_MAPS[i].insert(make_pair(key, value));
        }

    }
    for (int i=0; i<numberOfROIs; ++i){
        FORWARD_ALIGN_SEQ.push_back(reverseComplement(FORWARD_ALIGN_SEQ[i]));
        REVERSE_ALIGN_SEQ.push_back(reverseComplement(REVERSE_ALIGN_SEQ[i]));
    }
    Trie* trie = new Trie;
    trie->setThresholdROIPhaseGenesBarcodelenTargetlen( THRESHOLDS_OF_IMPORTANCE, numberOfROIs, numberOfPhases, GENES, BARCODE_LENGTH, TARGET_LENGTHS);
    
   // for (int i=0; i<FILENAMES.size(); ++i){
      //  ifstream readfile (FILENAMES[i].c_str());
    int count=0;
    string sequence;
    string barcode;
    string throwoutstring;
    int phase;
    int ROINumber;
    int indexForwardAlign;
    int indexReverseAlign;
   // cin>>throwoutstring;
    char buffer[BUFMAX + 1];
    char *bp = buffer;
    int c;
    string target;
    string message;
    cout<<"Beginning to read files"<<endl;
    getline(cin,message);
    cout<<message<<endl;
    while (true){

        cin>>ROINumber;
        if (ROINumber ==-100){
            string message2;
            cin>>message2;
            if (message2=="Processing"){
                string next;
                string words;
                string in;
                cin>>next>>words>>in>>message;
                cout<<message2<<" "<<next<<" "<<words<<" "<<in<<" "<<message<<endl;
                cin>>ROINumber;

            }
            if (message2=="-1000"){
               break;
            }

        }

        cin>>phase>>barcode>>sequence;

        trie->addBarcode(ROINumber, phase,barcode,sequence, TARGET[ROINumber]);
    }
           
    
//  }
// else {
//    cout<<"Error opening file "<< userDefinedVariables["FILENAME"][i]<<endl;
//    }

    cout<<"made the trie"<<endl;
    return trie;
}

