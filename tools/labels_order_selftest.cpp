#include "../Source/AnomalyCapture/Private/AnomalyLabelsOrder.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace AnomalyLabelsOrder;

static int GFailures = 0;
static int GChecks = 0;

static void Check(bool bOk, const std::string& What)
{
	++GChecks;
	if (!bOk)
	{
		++GFailures;
		std::printf("FAIL %s\n", What.c_str());
	}
}

static const unsigned char* Bytes(const std::string& S)
{
	return reinterpret_cast<const unsigned char*>(S.data());
}

static std::string RunEnd(const std::string& In, bool bLabelsWritten, bool bWriterStalled, bool bFlushComplete, EOutcome& Out, long long& Moved)
{
	Moved = 0;
	if (!ShouldAttempt(bLabelsWritten, bWriterStalled, bFlushComplete, Out))
	{
		return In;
	}
	const long long Len = static_cast<long long>(In.size());
	const long long N = CountRows(Bytes(In), Len);
	std::vector<FRow> Rows(static_cast<size_t>(N));
	const long long Got = SplitRows(Bytes(In), Len, Rows.data(), N);
	if (Got != N)
	{
		Out = EOutcome::ReadFailed;
		return In;
	}
	if (CountWithoutKey(Rows.data(), N) > 0)
	{
		Out = EOutcome::RowWithoutSessionIndex;
		return In;
	}
	if (InFrameOrder(Rows.data(), N))
	{
		Out = EOutcome::AlreadyInOrder;
		return In;
	}
	std::vector<FRow> Scratch(static_cast<size_t>(N));
	StableSort(Rows.data(), N, Scratch.data());
	Moved = CountMoved(Rows.data(), N);
	std::string Result(static_cast<size_t>(OutputSize(Rows.data(), N)), '\0');
	Assemble(Bytes(In), Rows.data(), N, reinterpret_cast<unsigned char*>(&Result[0]));
	Out = EOutcome::Sorted;
	return Result;
}

static std::string Row(long long Si, const std::string& Extra = std::string())
{
	return "{\"type\":\"frame\",\"session_index\":" + std::to_string(Si) + ",\"frame_index\":" + std::to_string(25600 + Si * 3) +
		",\"tick_index\":" + std::to_string(Si) + ",\"anomalies\":[{\"anomaly_type\":\"blink\",\"labelled\":true}]" + Extra + "}";
}

static std::vector<long long> KeysOf(const std::string& Text)
{
	std::vector<long long> Keys;
	const long long Len = static_cast<long long>(Text.size());
	const long long N = CountRows(Bytes(Text), Len);
	std::vector<FRow> Rows(static_cast<size_t>(N));
	SplitRows(Bytes(Text), Len, Rows.data(), N);
	for (const FRow& R : Rows)
	{
		Keys.push_back(R.bHasKey ? R.Key : -999999);
	}
	return Keys;
}

static void TestPrecheck()
{
	EOutcome O = EOutcome::Sorted;
	Check(!ShouldAttempt(false, false, true, O) && O == EOutcome::NotWritten, "no labels file written this run: not attempted, reason not_written");
	Check(!ShouldAttempt(true, true, true, O) && O == EOutcome::WriterStalled, "a stalled writer: not attempted, reason writer_stalled");
	Check(!ShouldAttempt(true, true, false, O) && O == EOutcome::WriterStalled, "stalled outranks an incomplete flush");
	Check(!ShouldAttempt(true, false, false, O) && O == EOutcome::WriterFlushIncomplete, "an incomplete flush: not attempted, reason writer_flush_incomplete");
	Check(ShouldAttempt(true, false, true, O), "a written file with a finished writer is attempted");
	Check(IsInFrameOrder(EOutcome::Sorted) && IsInFrameOrder(EOutcome::AlreadyInOrder), "sorted and already_in_order both mean frame order");
	Check(!IsInFrameOrder(EOutcome::WriterStalled) && !IsInFrameOrder(EOutcome::ReplaceFailed) && !IsInFrameOrder(EOutcome::RowWithoutSessionIndex),
		"every skip or failure means not in frame order");
	Check(std::string(Describe(EOutcome::WriterStalled)) == "writer_stalled" && std::string(Describe(EOutcome::NotWritten)) == "not_written" &&
		std::string(Describe(EOutcome::ReplaceFailed)) == "replace_failed", "reason words");
}

static void TestParse()
{
	long long K = -1;
	const std::string A = "{\"session_index\":42,\"x\":1}";
	Check(ParseSessionIndex(Bytes(A), (long long)A.size(), K) && K == 42, "plain key at the start");
	const std::string B = "{\"type\":\"frame\", \"session_index\" : 7 }";
	Check(ParseSessionIndex(Bytes(B), (long long)B.size(), K) && K == 7, "spaces around the colon and before the brace");
	const std::string C = "{\"note\":\"x \\\"session_index\\\":99\",\"session_index\":3}";
	Check(ParseSessionIndex(Bytes(C), (long long)C.size(), K) && K == 3, "an escaped copy inside a string value is not the key");
	const std::string D = "{\"ref_session_index\":5,\"session_index\":6}";
	Check(ParseSessionIndex(Bytes(D), (long long)D.size(), K) && K == 6, "a longer key ending in session_index is not the key");
	const std::string E = "{\"frame_index\":5}";
	Check(!ParseSessionIndex(Bytes(E), (long long)E.size(), K), "a row without the key");
	const std::string F = "{\"session_index\":\"5\"}";
	Check(!ParseSessionIndex(Bytes(F), (long long)F.size(), K), "a string value is not a frame number");
	const std::string G = "{\"session_index\":5.5}";
	Check(!ParseSessionIndex(Bytes(G), (long long)G.size(), K), "a fractional value is not a frame number");
	const std::string H = "{\"session_index\":}";
	Check(!ParseSessionIndex(Bytes(H), (long long)H.size(), K), "a missing value");
	const std::string J = "{\"k\":\"session_index\",\"session_index\":4}";
	Check(ParseSessionIndex(Bytes(J), (long long)J.size(), K) && K == 4, "the word as a string value is not the key");
	const std::string I = "{\"session_index\":2999}";
	Check(ParseSessionIndex(Bytes(I), (long long)I.size(), K) && K == 2999, "the key as the last field");
}

static void TestOutOfOrder()
{
	const std::string In = Row(2) + "\n" + Row(0) + "\n" + Row(3) + "\n" + Row(1) + "\n";
	EOutcome O = EOutcome::AlreadyInOrder;
	long long Moved = 0;
	const std::string Out = RunEnd(In, true, false, true, O, Moved);
	Check(O == EOutcome::Sorted, "out-of-order rows are sorted");
	const std::string Want = Row(0) + "\n" + Row(1) + "\n" + Row(2) + "\n" + Row(3) + "\n";
	Check(Out == Want, "sorted rows are byte-identical to the originals, in session_index order, one per line");
	Check(Out.size() == In.size(), "the sorted file has exactly the original byte count");
	Check(Moved == 4, "all four rows moved");

	EOutcome O2 = EOutcome::Sorted;
	long long Moved2 = 0;
	const std::string Again = RunEnd(Out, true, false, true, O2, Moved2);
	Check(O2 == EOutcome::AlreadyInOrder && Again == Out && Moved2 == 0, "a sorted file is left as it is (already_in_order)");
}

static void TestStalledLeftUnsorted()
{
	const std::string In = Row(5) + "\n" + Row(4) + "\n" + Row(6) + "\n";
	EOutcome O = EOutcome::Sorted;
	long long Moved = 0;
	const std::string Out = RunEnd(In, true, true, false, O, Moved);
	Check(O == EOutcome::WriterStalled, "a stalled run is not sorted: reason writer_stalled");
	Check(Out == In, "a stalled run's file is left byte-identical");
	Check(!IsInFrameOrder(O), "a stalled run reports labels_sorted false");
	const std::string Out2 = RunEnd(In, true, false, false, O, Moved);
	Check(O == EOutcome::WriterFlushIncomplete && Out2 == In, "an incomplete flush leaves the file untouched too");
	const std::string Out3 = RunEnd(In, false, false, true, O, Moved);
	Check(O == EOutcome::NotWritten && Out3 == In, "a run that wrote no labels file reports not_written");
}

static void TestEmpty()
{
	EOutcome O = EOutcome::Sorted;
	long long Moved = 0;
	const std::string Out = RunEnd(std::string(), true, false, true, O, Moved);
	Check(O == EOutcome::AlreadyInOrder && Out.empty() && Moved == 0, "an empty file is already in order and stays empty");
	Check(IsInFrameOrder(O), "an empty file reports labels_sorted true");
	const std::string Blank = "\n\n";
	const std::string Out2 = RunEnd(Blank, true, false, true, O, Moved);
	Check(O == EOutcome::AlreadyInOrder && CountRows(Bytes(Blank), 2) == 0, "a file of blank lines has no rows");
}

static void TestRowWithoutKey()
{
	const std::string In = Row(1) + "\n{\"type\":\"frame\"}\n" + Row(0) + "\n";
	EOutcome O = EOutcome::Sorted;
	long long Moved = 0;
	const std::string Out = RunEnd(In, true, false, true, O, Moved);
	Check(O == EOutcome::RowWithoutSessionIndex, "a row without session_index stops the sort");
	Check(Out == In, "and the file is left byte-identical");
}

static void TestStableAndEdges()
{
	const std::string In = Row(1, ",\"tag\":\"first\"") + "\n" + Row(0) + "\n" + Row(1, ",\"tag\":\"second\"") + "\n";
	EOutcome O = EOutcome::AlreadyInOrder;
	long long Moved = 0;
	const std::string Out = RunEnd(In, true, false, true, O, Moved);
	const std::string Want = Row(0) + "\n" + Row(1, ",\"tag\":\"first\"") + "\n" + Row(1, ",\"tag\":\"second\"") + "\n";
	Check(O == EOutcome::Sorted && Out == Want, "equal frame numbers keep their written order (stable)");

	const std::string Crlf = Row(1) + "\r\n" + Row(0) + "\r\n";
	const std::string OutCrlf = RunEnd(Crlf, true, false, true, O, Moved);
	Check(O == EOutcome::Sorted && OutCrlf == Row(0) + "\r\n" + Row(1) + "\r\n", "a carriage return stays with its row");

	const std::string NoTail = Row(1) + "\n" + Row(0);
	const std::string OutNoTail = RunEnd(NoTail, true, false, true, O, Moved);
	Check(O == EOutcome::Sorted && OutNoTail == Row(0) + "\n" + Row(1) + "\n", "a last row without a newline is kept and ends with one");

	const std::string Gap = Row(9) + "\n\n" + Row(8) + "\n";
	const std::string OutGap = RunEnd(Gap, true, false, true, O, Moved);
	Check(O == EOutcome::Sorted && OutGap == Row(8) + "\n" + Row(9) + "\n", "a blank line carries no row and is not kept");
}

static void TestLargeShuffle()
{
	const int N = 3000;
	std::vector<int> Order(N);
	for (int i = 0; i < N; ++i)
	{
		Order[i] = i;
	}
	unsigned int Seed = 12345u;
	for (int i = N - 1; i > 0; --i)
	{
		Seed = Seed * 1103515245u + 12345u;
		const int j = static_cast<int>((Seed >> 8) % static_cast<unsigned int>(i + 1));
		std::swap(Order[i], Order[j]);
	}
	std::string In;
	std::string Want;
	for (int i = 0; i < N; ++i)
	{
		In += Row(Order[i]) + "\n";
		Want += Row(i) + "\n";
	}
	EOutcome O = EOutcome::AlreadyInOrder;
	long long Moved = 0;
	const std::string Out = RunEnd(In, true, false, true, O, Moved);
	Check(O == EOutcome::Sorted, "3000 shuffled rows are sorted");
	Check(Out == Want, "3000 shuffled rows come out byte-identical in frame order");
	const std::vector<long long> Keys = KeysOf(Out);
	bool bAscending = Keys.size() == static_cast<size_t>(N);
	for (size_t i = 1; i < Keys.size(); ++i)
	{
		bAscending = bAscending && Keys[i] == Keys[i - 1] + 1;
	}
	Check(bAscending, "every row's session_index is the previous row's plus one");

	std::vector<int> Reference(Order);
	std::stable_sort(Reference.begin(), Reference.end());
	bool bMatchesReference = true;
	for (int i = 0; i < N; ++i)
	{
		bMatchesReference = bMatchesReference && Keys[static_cast<size_t>(i)] == Reference[static_cast<size_t>(i)];
	}
	Check(bMatchesReference, "the merge sort agrees with std::stable_sort");
}

int main()
{
	TestPrecheck();
	TestParse();
	TestOutOfOrder();
	TestStalledLeftUnsorted();
	TestEmpty();
	TestRowWithoutKey();
	TestStableAndEdges();
	TestLargeShuffle();
	std::printf("labels_order_selftest: %d checks, %d failures\n", GChecks, GFailures);
	return GFailures == 0 ? 0 : 1;
}
