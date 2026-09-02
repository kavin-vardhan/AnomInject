#include "AnomalyRunLog.h"

#include "Containers/StringConv.h"
#include "CoreGlobals.h"
#include "HAL/FileManager.h"
#include "Misc/OutputDeviceHelper.h"
#include "Serialization/Archive.h"

namespace
{
	const FName GAnomalyRunLogCategoryInjector(TEXT("LogAnomaly"));
	const FName GAnomalyRunLogCategoryCapture(TEXT("LogAnomalyCapture"));
	const ANSICHAR GAnomalyRunLogTerminator[2] = { '\r', '\n' };
}

FAnomalyRunLog::FAnomalyRunLog() = default;

FAnomalyRunLog::~FAnomalyRunLog()
{
	Close(FString());
}

bool FAnomalyRunLog::AcceptsCategory(const FName& Category)
{
	return Category == GAnomalyRunLogCategoryInjector || Category == GAnomalyRunLogCategoryCapture;
}

bool FAnomalyRunLog::Open(const FString& InFilePath, const FString& InHeader)
{
	FScopeLock Lock(&CS);
	if (Writer.IsValid())
	{
		return true;
	}

	Writer.Reset(IFileManager::Get().CreateFileWriter(*InFilePath, FILEWRITE_AllowRead));
	if (!Writer.IsValid())
	{
		return false;
	}

	FilePath = InFilePath;
	LinesWritten = 0;
	if (!InHeader.IsEmpty())
	{
		WriteLineLocked(InHeader);
	}
	return true;
}

void FAnomalyRunLog::Close(const FString& InFinalLine)
{
	FScopeLock Lock(&CS);
	if (!Writer.IsValid())
	{
		return;
	}
	if (!InFinalLine.IsEmpty())
	{
		WriteLineLocked(InFinalLine);
	}
	Writer->Flush();
	Writer->Close();
	Writer.Reset();
}

bool FAnomalyRunLog::IsOpen() const
{
	FScopeLock Lock(&CS);
	return Writer.IsValid();
}

FString FAnomalyRunLog::GetFilePath() const
{
	FScopeLock Lock(&CS);
	return FilePath;
}

int32 FAnomalyRunLog::GetLinesWritten() const
{
	FScopeLock Lock(&CS);
	return LinesWritten;
}

void FAnomalyRunLog::WriteLineLocked(const FString& Line)
{
	if (!Writer.IsValid())
	{
		return;
	}

	FTCHARToUTF8 Converted(*Line);
	if (Converted.Length() > 0)
	{
		Writer->Serialize(const_cast<ANSICHAR*>(Converted.Get()), Converted.Length());
	}
	Writer->Serialize(const_cast<ANSICHAR*>(GAnomalyRunLogTerminator), 2);
	Writer->Flush();
	++LinesWritten;
}

void FAnomalyRunLog::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category, const double Time)
{
	if (!V || !AcceptsCategory(Category))
	{
		return;
	}

	const FString Line = FOutputDeviceHelper::FormatLogLine(Verbosity, Category, V, ELogTimes::UTC, Time);

	FScopeLock Lock(&CS);
	WriteLineLocked(Line);
}

void FAnomalyRunLog::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category)
{
	Serialize(V, Verbosity, Category, -1.0);
}

void FAnomalyRunLog::Flush()
{
	FScopeLock Lock(&CS);
	if (Writer.IsValid())
	{
		Writer->Flush();
	}
}
