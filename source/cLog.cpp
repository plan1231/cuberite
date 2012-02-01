
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "cLog.h"

#include <fstream>
#include <ctime>
#include <stdarg.h>
#include "cMakeDir.h"





cLog* cLog::s_Log = NULL;

cLog::cLog(const AString & a_FileName )
	: m_File(NULL)
{
	s_Log = this;

	// create logs directory
	cMakeDir::MakeDir("logs");

	OpenLog( (std::string("logs/") + a_FileName).c_str() );
}





cLog::~cLog()
{
	CloseLog();
	s_Log = NULL;
}





cLog* cLog::GetInstance()
{
	if(s_Log)
		return s_Log;

	new cLog("log.txt");
	return s_Log;
}





void cLog::CloseLog()
{
	if( m_File )
		fclose (m_File);
	m_File = 0;
}





void cLog::OpenLog( const char* a_FileName )
{
	if(m_File) fclose (m_File);
	#ifdef _WIN32
	fopen_s( &m_File, a_FileName, "a+" );
	#else
	m_File = fopen(a_FileName, "a+" );
	#endif
}





void cLog::ClearLog()
{
	#ifdef _WIN32
	if( fopen_s( &m_File, "log.txt", "w" ) == 0)
		fclose (m_File);
	#else
	m_File = fopen("log.txt", "w" );
	if( m_File )
		fclose (m_File);
	#endif
	m_File = 0;
}





void cLog::Log(const char * a_Format, va_list argList)
{
	AString Message;
	if (argList != NULL)
	{
		AppendVPrintf(Message, a_Format, argList);
	}
	else
	{
		// This branch needs to be here because of *nix crashing in vsnprintf() when argList is NULL
		Message.assign(a_Format);
	}

	time_t rawtime;
	time ( &rawtime );
	
	struct tm* timeinfo;
#ifdef _WIN32
	struct tm timeinforeal;
	timeinfo = &timeinforeal;
	localtime_s(timeinfo, &rawtime );
#else
	timeinfo = localtime( &rawtime );
#endif

	AString Line;
	Printf(Line, "[%02d:%02d:%02d] %s\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, Message.c_str());
	if (m_File)
	{
		fputs(Line.c_str(), m_File);
		fflush(m_File);
	}


	printf("%s", Line.c_str());
}





void cLog::Log(const char* a_Format, ...)
{
	va_list argList;
	va_start(argList, a_Format);
	Log( a_Format, argList );
	va_end(argList);
}





void cLog::SimpleLog(const char* a_String)
{
	Log("%s", a_String );
}




