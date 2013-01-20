#include "logFile.h"

C_logFile::C_logFile ( const char* fileName )
{
	this->filename = fileName;
	this->LOG_FILE_P.open ( this->filename );
	this->writeHeader ();
}

C_logFile::~C_logFile ( void )
{
	LOG_FILE_P << "\n\n*End of log file*\n\n";
	this->LOG_FILE_P.close ();
}

void C_logFile::writeToFile ( string msg )
{

//	if ( msg[msg.size() - 1 ] == '\n' )
//		this->LOG_FILE_P << msg;
//	else
//		this->LOG_FILE_P << msg << '\n';

	this->LOG_FILE_P << msg;
}

void C_logFile::writeHeader ( void )
{
	string header;

	time( &osBinaryTime ) ;		// Get the current time from the operating system.
	struct tm when;

	time( &osBinaryTime );
	when = *localtime( &osBinaryTime );

	//cout << asctime( &when ) << '\n';

	header = "*************************\n" + string ( asctime( &when ) ) + "*************************\n";


	LOG_FILE_P << "*Start of log file*\n\n";
	LOG_FILE_P << header << '\n';
}

void C_logFile::endLogging ( void )
{
	this->~C_logFile ();
}
