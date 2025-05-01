#include "mem/cache/replacement_policies/lslru_rp.hh"

#include <cassert>
#include <limits>
#include "sim/cur_tick.hh"

namespace gem5
{
namespace replacement_policy
{

LSLRU::LSLRU(const Params &p)
  : Base(p)
{}

// Invalidate a line: clear timestamp and access count.
void
LSLRU::invalidate(const std::shared_ptr<ReplacementData>& rd)
{
    auto d = std::static_pointer_cast<LRUReplData>(rd);
    d->lastTouchTick = Tick(0);
    d->visitCount    = 0;
}

// On every hit: record current tick and bump visit count.
void
LSLRU::touch(const std::shared_ptr<ReplacementData>& rd, const PacketPtr pkt) const
{
    auto d = std::static_pointer_cast<LRUReplData>(rd);
    d->lastTouchTick = curTick();
    ++d->visitCount;
    if(pkt.cmd.isRead()||pkt.cmd.isWrite()){
	if(pkt.cmd.isWrite()){
		++d->visitCount;
	}
   }
    	
}

// On new fill: treat as first access (count=1).
void
LSLRU::reset(const std::shared_ptr<ReplacementData>& rd) const
{
    auto d = std::static_pointer_cast<LRUReplData>(rd);
    d->lastTouchTick = curTick();
    d->visitCount    = 1;
}

// Two-pass victim selection.
ReplaceableEntry*
LSLRU::getVictim(const ReplacementCandidates &candidates) const
{
    assert(!candidates.empty());

    // Pass 1: find the minimum visitCount.
    unsigned minVC = std::numeric_limits<unsigned>::max();
    for (auto *blk : candidates) {
        auto d = std::static_pointer_cast<LRUReplData>(blk->replacementData);
        if (d->visitCount < minVC)
            minVC = d->visitCount;
    }

    // Pass 2: among those with visitCount == minVC, pick the oldest.
    ReplaceableEntry *victim = nullptr;
    Tick oldest = Tick(-1);  // max possible
    for (auto *blk : candidates) {
        auto d = std::static_pointer_cast<LRUReplData>(blk->replacementData);
        if (d->visitCount == minVC) {
            if (!victim || d->lastTouchTick < oldest) {
                victim = blk;
                oldest = d->lastTouchTick;
            }
        }
    }

    assert(victim);
    return victim;
}

// Allocate fresh replacement data object.
std::shared_ptr<ReplacementData>
LSLRU::instantiateEntry()
{
    return std::make_shared<LRUReplData>();
}

} // namespace replacement_policy
} // namespace gem5
GEM5_REGISTER_CLASS(LSLRU,"LSLRURP")
