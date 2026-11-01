#include "../../Source/AnomalyCapture/Private/Compat/AnomalyWriteQueue425.h"
#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

struct Future
{
    std::future<int> Value;
    int Get() { return Value.get(); }
};
struct Queue
{
    std::vector<Future> Values;
    int Num() const { return int(Values.size()); }
    Future& operator[](int Index) { return Values[Index]; }
    void RemoveAt(int Index) { Values.erase(Values.begin()+Index); }
};
int Run(bool Bounded)
{
    Queue Writes;
    std::vector<int> Completed;
    auto Consume = [&](int Index) { Completed.push_back(Index); };
    int Peak = 0;
    for (int I=0; I<32; ++I)
    {
        if (Bounded) { AnomalyWriteQueue425::MakeRoom(Writes,Consume); }
        Writes.Values.push_back(Future{std::async(std::launch::async,[I]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            return I;
        })});
        Peak = std::max(Peak,Writes.Num());
    }
    while (Writes.Num()) { Consume(Writes[0].Get()); Writes.RemoveAt(0); }
    if (Completed.size()!=32) { return -1; }
    for (int I=0; I<32; ++I) { if (Completed[I]!=I) { return -1; } }
    return Peak;
}
int main()
{
    const int Bounded=Run(true),Unbounded=Run(false);
    const bool Good=Bounded==4 && Unbounded>4;
    std::cout << "{\"result\":\"" << (Good ? "PASS" : "FAIL") << "\",\"bounded_peak\":" << Bounded
        << ",\"unbounded_negative_peak\":" << Unbounded << ",\"ordered_completed_each\":32}" << std::endl;
    return Good ? 0 : 1;
}
