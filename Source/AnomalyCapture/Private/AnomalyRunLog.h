#pragma once

#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "Misc/OutputDevice.h"

class FArchive;

class FAnomalyRunLog : public FOutputDevice
{
public:
	FAnomalyRunLog();
	virtual ~FAnomalyRunLog();

	bool Open(const FString& InFilePath, const FString& InHeader);
	void Close(const FString& InFinalLine);

	bool IsOpen() const;
	FString GetFilePath() const;
	int32 GetLinesWritten() const;

	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category, const double Time) override;
	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override;
	virtual void Flush() override;

	virtual bool CanBeUsedOnAnyThread() const override { return true; }
	virtual bool CanBeUsedOnMultipleThreads() const override { return true; }

	static bool AcceptsCategory(const FName& Category);

private:
	void WriteLineLocked(const FString& Line);

	mutable FCriticalSection CS;
	TUniquePtr<FArchive> Writer;
	FString FilePath;
	int32 LinesWritten = 0;
};
