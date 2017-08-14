#include "VisionResultsPackage.h"
#include <sstream>
#include <iostream>
using namespace std;

namespace JetsonCV{
	void VisionResultsPackage::put(string key, int value){
		mInts.insert(pair<string, int> (key, value));
	}

	void VisionResultsPackage::put(string key, string value){
		mStrings.insert(pair<string, string> (key, value));
	}

	void VisionResultsPackage::put(string key, bool value){
		mBools.insert(pair<string, bool> (key, value));
	}

	void VisionResultsPackage::put(string key, double value){
		mDoubles.insert(pair<string, double> (key, value));
	}

	void VisionResultsPackage::writeToNetworkTables(){
		for (pair<string, double> p : mDoubles){
			mNetworkTable->PutNumber(p.first, p.second);
		}
		for (pair<string, int> p : mInts){
			mNetworkTable->PutNumber(p.first, p.second);
		}
		for (pair<string, string> p : mStrings){
			mNetworkTable->PutString(p.first, p.second);
		}
		for (pair<string, bool> p : mBools){
			mNetworkTable->PutBoolean(p.first, p.second);
		}
		mNetworkTable->PutString("LastUpdated", mLastSuccess);
		mNetworkTable->Flush();
	}

	void VisionResultsPackage::writeToLogFile(){
		if(!logStream.good()){cout << "Unable to open logfile" << endl; return;}
		logStream << *this;
	}

	void VisionResultsPackage::setWriteTime(ui64 pSuccess){
		ostringstream oss;
		oss << pSuccess;
		mLastSuccess = oss.str();
	}

	void VisionResultsPackage::setLogFile(const string& pFilename){
		logStream.open(pFilename);
		if(!logStream.good()){cout << "Unable to open " + pFilename << endl; return;}
	}

	ostream& operator<< (ostream& os, VisionResultsPackage& vrp) {
		os << endl << "************************************************" << endl;
		os << "LastUpdated ======== " << vrp.mLastSuccess << endl;
		for (pair<string, int> p : vrp.mInts){
			os << p.first << " ======== " << p.second << endl;
		}
		for (pair<string, double> p : vrp.mDoubles){
			os << p.first << " ======== " << p.second << endl;
		}
		for (pair<string, string> p : vrp.mStrings){
			os << p.first << " ======== " << p.second << endl;
		}
		for (pair<string, bool> p : vrp.mBools){
			os << p.first << " ======== " << p.second << endl;
		}
		os << endl << "************************************************" << endl;
		return os;

	}
}