#pragma once

namespace AnomalyLabelsOrder
{
	enum class EOutcome : unsigned char
	{
		Sorted = 0,
		AlreadyInOrder = 1,
		NotWritten = 2,
		WriterStalled = 3,
		WriterFlushIncomplete = 4,
		NoFile = 5,
		ReadFailed = 6,
		RowWithoutSessionIndex = 7,
		WriteFailed = 8,
		ReplaceFailed = 9
	};

	inline bool IsInFrameOrder(EOutcome O)
	{
		return O == EOutcome::Sorted || O == EOutcome::AlreadyInOrder;
	}

	inline const char* Describe(EOutcome O)
	{
		switch (O)
		{
		case EOutcome::Sorted: return "sorted";
		case EOutcome::AlreadyInOrder: return "already_in_order";
		case EOutcome::NotWritten: return "not_written";
		case EOutcome::WriterStalled: return "writer_stalled";
		case EOutcome::WriterFlushIncomplete: return "writer_flush_incomplete";
		case EOutcome::NoFile: return "no_file";
		case EOutcome::ReadFailed: return "read_failed";
		case EOutcome::RowWithoutSessionIndex: return "row_without_session_index";
		case EOutcome::WriteFailed: return "write_failed";
		case EOutcome::ReplaceFailed: return "replace_failed";
		default: return "unknown";
		}
	}

	inline bool ShouldAttempt(bool bLabelsWritten, bool bWriterStalled, bool bWriterFlushComplete, EOutcome& OutSkip)
	{
		if (!bLabelsWritten)
		{
			OutSkip = EOutcome::NotWritten;
			return false;
		}
		if (bWriterStalled)
		{
			OutSkip = EOutcome::WriterStalled;
			return false;
		}
		if (!bWriterFlushComplete)
		{
			OutSkip = EOutcome::WriterFlushIncomplete;
			return false;
		}
		return true;
	}

	struct FRow
	{
		long long Begin = 0;
		long long End = 0;
		long long Key = 0;
		long long Original = 0;
		bool bHasKey = false;
	};

	inline bool IsSpace(unsigned char C)
	{
		return C == ' ' || C == '\t' || C == '\r';
	}

	inline bool ParseSessionIndex(const unsigned char* Line, long long Len, long long& OutKey)
	{
		static const char Token[] = "\"session_index\"";
		const long long TokenLen = static_cast<long long>(sizeof(Token) - 1);
		for (long long i = 0; i + TokenLen <= Len; ++i)
		{
			bool bMatch = true;
			for (long long k = 0; k < TokenLen; ++k)
			{
				if (Line[i + k] != static_cast<unsigned char>(Token[k]))
				{
					bMatch = false;
					break;
				}
			}
			if (!bMatch)
			{
				continue;
			}
			long long j = i + TokenLen;
			while (j < Len && IsSpace(Line[j]))
			{
				++j;
			}
			if (j >= Len || Line[j] != ':')
			{
				continue;
			}
			++j;
			while (j < Len && IsSpace(Line[j]))
			{
				++j;
			}
			bool bNegative = false;
			if (j < Len && Line[j] == '-')
			{
				bNegative = true;
				++j;
			}
			long long Value = 0;
			int Digits = 0;
			while (j < Len && Line[j] >= '0' && Line[j] <= '9')
			{
				if (Digits >= 18)
				{
					return false;
				}
				Value = Value * 10 + static_cast<long long>(Line[j] - '0');
				++Digits;
				++j;
			}
			if (Digits == 0)
			{
				return false;
			}
			if (j < Len && Line[j] != ',' && Line[j] != '}' && !IsSpace(Line[j]))
			{
				return false;
			}
			OutKey = bNegative ? -Value : Value;
			return true;
		}
		return false;
	}

	inline long long CountRows(const unsigned char* Data, long long Len)
	{
		long long Rows = 0;
		long long Start = 0;
		for (long long i = 0; i <= Len; ++i)
		{
			if (i == Len || Data[i] == '\n')
			{
				if (i > Start)
				{
					++Rows;
				}
				Start = i + 1;
			}
		}
		return Rows;
	}

	inline long long SplitRows(const unsigned char* Data, long long Len, FRow* OutRows, long long Capacity)
	{
		long long N = 0;
		long long Start = 0;
		for (long long i = 0; i <= Len; ++i)
		{
			if (i == Len || Data[i] == '\n')
			{
				if (i > Start && N < Capacity)
				{
					FRow& R = OutRows[N];
					R.Begin = Start;
					R.End = i;
					R.Original = N;
					R.Key = 0;
					R.bHasKey = ParseSessionIndex(Data + Start, i - Start, R.Key);
					++N;
				}
				Start = i + 1;
			}
		}
		return N;
	}

	inline long long CountWithoutKey(const FRow* Rows, long long N)
	{
		long long Missing = 0;
		for (long long i = 0; i < N; ++i)
		{
			if (!Rows[i].bHasKey)
			{
				++Missing;
			}
		}
		return Missing;
	}

	inline bool Before(const FRow& A, const FRow& B)
	{
		if (A.bHasKey != B.bHasKey)
		{
			return A.bHasKey;
		}
		return A.bHasKey && A.Key < B.Key;
	}

	inline bool InFrameOrder(const FRow* Rows, long long N)
	{
		for (long long i = 1; i < N; ++i)
		{
			if (Before(Rows[i], Rows[i - 1]))
			{
				return false;
			}
		}
		return true;
	}

	inline void StableSort(FRow* Rows, long long N, FRow* Scratch)
	{
		FRow* Src = Rows;
		FRow* Dst = Scratch;
		for (long long Width = 1; Width < N; Width *= 2)
		{
			for (long long Lo = 0; Lo < N; Lo += 2 * Width)
			{
				const long long Mid = Lo + Width < N ? Lo + Width : N;
				const long long Hi = Lo + 2 * Width < N ? Lo + 2 * Width : N;
				long long a = Lo;
				long long b = Mid;
				long long o = Lo;
				while (a < Mid && b < Hi)
				{
					if (Before(Src[b], Src[a]))
					{
						Dst[o++] = Src[b++];
					}
					else
					{
						Dst[o++] = Src[a++];
					}
				}
				while (a < Mid)
				{
					Dst[o++] = Src[a++];
				}
				while (b < Hi)
				{
					Dst[o++] = Src[b++];
				}
			}
			FRow* Swap = Src;
			Src = Dst;
			Dst = Swap;
		}
		if (Src != Rows)
		{
			for (long long i = 0; i < N; ++i)
			{
				Rows[i] = Src[i];
			}
		}
	}

	inline long long CountMoved(const FRow* Rows, long long N)
	{
		long long Moved = 0;
		for (long long i = 0; i < N; ++i)
		{
			if (Rows[i].Original != i)
			{
				++Moved;
			}
		}
		return Moved;
	}

	inline long long OutputSize(const FRow* Rows, long long N)
	{
		long long Size = 0;
		for (long long i = 0; i < N; ++i)
		{
			Size += (Rows[i].End - Rows[i].Begin) + 1;
		}
		return Size;
	}

	inline void Assemble(const unsigned char* Data, const FRow* Rows, long long N, unsigned char* Out)
	{
		long long o = 0;
		for (long long i = 0; i < N; ++i)
		{
			for (long long k = Rows[i].Begin; k < Rows[i].End; ++k)
			{
				Out[o++] = Data[k];
			}
			Out[o++] = '\n';
		}
	}
}
