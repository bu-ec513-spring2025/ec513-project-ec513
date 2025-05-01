#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_LSLRU_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_LSLRU_RP_HH__

#include "mem/cache/replacement_policies/base.hh"

namespace gem5
{
struct LSLRURPParams;

namespace replacement_policy
{

/**
 * Min-visit + LRU replacement policy.
 *
 * Each cache line carries:
 *   - lastTouchTick: timestamp of last access
 *   - visitCount: number of times this line has been accessed
 *
 * Victim selection is a two-pass scan:
 *   1) Find the minimum visitCount among candidates.
 *   2) Within those with visitCount == min, choose the one with
 *      the oldest lastTouchTick (i.e. true LRU).
 */
class LSLRU : public Base
{
  protected:
    /** Per-line replacement data. */
    struct LSLRUReplData : ReplacementData
    {
        Tick     lastTouchTick;  ///< Tick of last access
        unsigned visitCount;     ///< Access count

        /** Default: mark as never touched. */
        LSLRUReplData()
          : lastTouchTick(0),
            visitCount(0)
        {}
    };

  public:
    using Params = LSLRURPParams;

    LSLRU(const Params &p);
    ~LSLRU() override = default;

    /**
     * Called when a line is invalidated/evicted in upper level.
     * Reset timestamp and visit count.
     */
    void invalidate(
        const std::shared_ptr<ReplacementData>& replacement_data
    ) override;

    /**
     * Called on every cache hit (touch).
     * Update timestamp and increment visit count.
     */
    void touch(
        const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt
    ) const override;

    /**
     * Called when a new line is filled.
     * Treat as first access: timestamp=now, visitCount=1.
     */
    void reset(
        const std::shared_ptr<ReplacementData>& replacement_data
    ) const override;

    /**
     * Victim selection: two-pass over all candidates.
     * Returns the ReplaceableEntry to evict.
     */
    ReplaceableEntry*
    getVictim(const ReplacementCandidates& candidates) const override;

    /**
     * Instantiate a fresh LRUReplData for each new line.
     */
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

} // namespace replacement_policy
} // namespace gem5

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_LSLRU_RP_HH__


