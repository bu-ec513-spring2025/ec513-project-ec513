#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_LRU_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_LRU_RP_HH__

#include "mem/cache/replacement_policies/base.hh"

namespace gem5 {
struct LRURPParams;

namespace replacement_policy {

/**
 * Min-visit + score-based LRU replacement policy.
 *
 * Each line tracks:
 *   - lastTouchTick: timestamp of last access
 *   - visitCount:   number of hits
 *   - score:        replacement score (here = lastTouchTick)
 *
 * Victim selection:
 *   1) find the minimum visitCount among candidates
 *   2) among those, evict the one with smallest score (oldest timestamp)
 */
class LRU : public Base {
  protected:
    struct LRUReplData : ReplacementData {
        Tick     lastTouchTick;  ///< last access timestamp
        unsigned visitCount;     ///< hit count
        Tick     score;          ///< replacement score
	bool WriteInst;

        LRUReplData()
          : lastTouchTick(0),
            visitCount(0),
            score(0),
	    WriteInst(NULL)
        {}
    };

  public:
    using Params = LRURPParams;
    LRU(const Params &p);
    ~LRU() override = default;

    void invalidate(
        const std::shared_ptr<ReplacementData>& replacement_data
    ) override;

//    void touch(
//        const std::shared_ptr<ReplacementData>& replacement_data,
//	const PacketPtr pkt) override;
    void touch(
        const std::shared_ptr<ReplacementData>& replacement_data
    ) const override;

    void reset(
        const std::shared_ptr<ReplacementData>& replacement_data
    ) const override;

    ReplaceableEntry*
    getVictim(const ReplacementCandidates& candidates) const override;

    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

} // namespace replacement_policy
} // namespace gem5

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_LRU_RP_HH__

