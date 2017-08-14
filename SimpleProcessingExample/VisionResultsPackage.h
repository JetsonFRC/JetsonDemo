#ifndef VISION_RESULTS_PACKAGE_H
#define VISION_RESULTS_PACKAGE_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "networktables/NetworkTable.h"
#include <memory>

namespace JetsonCV {
	typedef unsigned long long ui64;
	class VisionResultsPackage{
	private:
		friend std::ostream& operator<< (std::ostream& os, VisionResultsPackage& vrp);
		std::map<std::string, int> mInts;
		std::map<std::string, std::string> mStrings;
		std::map<std::string, bool> mBools;
		std::map<std::string, double> mDoubles;
		std::shared_ptr<NetworkTable> mNetworkTable;
		std::string mLastSuccess;
		std::ofstream logStream;
	public:
		void put(std::string, int);//
		void put(std::string, std::string);///
		void put(std::string, bool);//
		void put(std::string, double);//
		void setWriteTime(ui64 pSuccess);
		void setNetworkTable(std::shared_ptr<NetworkTable> pNetworkTable){mNetworkTable = pNetworkTable;}
		void setLogFile(const std::string& pFilename);
		std::string getLastSuccessfulWrite() const {return mLastSuccess;}
		void writeToNetworkTables();//
		void writeToLogFile();//
	};
}
#endif