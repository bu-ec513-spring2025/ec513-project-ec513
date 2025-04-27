#include "mem/cache/replacement_policies/lru_rp.hh"
#include "params/LRURP.hh"
#include <cassert>
#include <limits>
#include "sim/cur_tick.hh"

namespace gem5 {
namespace replacement_policy {

LRU::LRU(const Params &p)
  : Base(static_cast<const Base::Params&>(p))
{}

// Invalidate: clear timestamp, count, and reset score = 0
void
LRU::invalidate(const std::shared_ptr<ReplacementData>& rd)
{
    auto d = std::static_pointer_cast<LRUReplData>(rd);
    d->lastTouchTick = Tick(0);
    d->visitCount    = 0;
    d->score         = d->lastTouchTick;
}

// Touch (hit): update timestamp, increment count, score = timestamp
//void
//LRU::touch(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt) 
//const
//{
//    auto d = std::static_pointer_cast<LRUReplData>(replacement_data);
//    d->lastTouchTick = curTick();
//    ++d->visitCount;
//    d->score= d->lastTouchTick;
//    if(pkt->isRead()||pkt->isWrite()){
//    if(pkt->isWrite()){
//        d->WriteInst=true;
//        ++d->visitCount;
//    }
//    }
//}

void
LRU::touch(const std::shared_ptr<ReplacementData>& replacement_data)
    const
{
    auto d = std::static_pointer_cast<LRUReplData>(replacement_data);
    d->lastTouchTick = curTick();
    ++d->visitCount;
    d->score= d->lastTouchTick;
}


// Reset (new fill): treat as first access: timestamp now, count=1, score=timestamp
void
LRU::reset(const std::shared_ptr<ReplacementData>& rd) const
{
    auto d = std::static_pointer_cast<LRUReplData>(rd);
    d->lastTouchTick = curTick();
    d->visitCount    = 1;
    d->score         = d->lastTouchTick;
}

// Two-pass victim selection: min visitCount, then min score
ReplaceableEntry*
LRU::getVictim(const ReplacementCandidates &candidates) const
{
    assert(!candidates.empty());

    // Pass 1: find smallest visitCount
    unsigned minVC = std::numeric_limits<unsigned>::max();
    std::vector<std::pair<Tick, ReplaceableEntry*>> minVCBlocks;
    for (auto *blk : candidates) {
        auto d = std::static_pointer_cast<LRUReplData>(blk->replacementData);
        if (d->visitCount < minVC){
            minVC = d->visitCount;
            minVCBlocks.clear();
            minVCBlocks.emplace_back(d->score, blk);
        }else if(d->visitCount == minVC){
            minVCBlocks.emplace_back(d->score, blk);
        }
    }

    // Pass 2: among those, pick the one with smallest score
    ReplaceableEntry *victim = nullptr;
    // Tick bestScore = Tick(-1);
    // for (auto *blk : candidates) {
    //     auto d = std::static_pointer_cast<LRUReplData>(blk->replacementData);
    //     if (d->visitCount == minVC) {
    //         if (!victim || d->score < bestScore) {
    //             victim    = blk;
    //             bestScore = d->score;
    //         }
    //     }
    // }


//this one is super slow, so I will try out another method of approximate
//    size_t medianIndex = minVCBlocks.size() / 2;

    // Partition so that the median is in the correct place
//    std::nth_element(
//        minVCBlocks.begin(),
//        minVCBlocks.begin() + medianIndex,
//        minVCBlocks.end(),
//        [](const auto &a, const auto &b) {
//            return a.first < b.first;
//        });

//    victim = minVCBlocks[medianIndex].second;

    if(!minVCBlocks.empty()){
	size_t medianIndex = minVCBlocks.size() / 2;
	victim = minVCBlocks[medianIndex].second;
    }
    assert(victim);
    return victim;
}

// Create fresh replacement data
std::shared_ptr<ReplacementData>
LRU::instantiateEntry()
{
    return std::make_shared<LRUReplData>();
}

} // namespace replacement_policy
} // namespace gem5
