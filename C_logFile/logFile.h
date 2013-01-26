#ifndef C_LOG_FILE_H_
#define C_LOG_FILE_H_

#include <time.h>

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class C_logFile {
	private:
		const char* filename;	//the name of the log file.
		ofstream LOG_FILE_P;	//stream attached to previous file.

		void writeHeader(void);

		time_t osBinaryTime;		// C run-time time (defined in <time.h>)

	public:

		C_logFile(const char* fileName);
		~C_logFile(void);

		void writeToFile(string);
		void endLogging(void);
};

#endif
