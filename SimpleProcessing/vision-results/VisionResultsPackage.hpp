#ifndef VISION_RESULTS_PACKAGE_HPP
#define VISION_RESULTS_PACKAGE_HPP

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

		//for quick debugging
		friend std::ostream& operator<< (std::ostream& os, VisionResultsPackage& vrp);

		//data storage
		std::map<std::string, int> mInts;
		std::map<std::string, std::string> mStrings;
		std::map<std::string, bool> mBools;
		std::map<std::string, double> mDoubles;
		std::string mLastSuccess;

		//NetworkTable and logfile
		std::shared_ptr<NetworkTable> mNetworkTable;
		std::ofstream logStream;
	public:
		//allow user to put without worrying about datatype
		void put(std::string, int);
		void put(std::string, std::string);
		void put(std::string, bool);
		void put(std::string, double);
		//set time of last successful write
		void setWriteTime(ui64 pSuccess);
		//add networktable, log file
		void setNetworkTable(std::shared_ptr<NetworkTable> pNetworkTable){mNetworkTable = pNetworkTable;}
		void setLogFile(const std::string& pFilename);

		std::string getLastSuccessfulWrite() const {return mLastSuccess;}
		//outputs
		void writeToNetworkTables();
		void writeToLogFile();
	};
}
#endif