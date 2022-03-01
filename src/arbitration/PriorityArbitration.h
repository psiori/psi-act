#ifndef _PSIACT_PRIORITYARBITRATION_H_
#define _PSIACT_PRIORITYARBITRATION_H_

//
//  PriorityArbitration.h
//  psi-act
//
//  Sascha Lange, 2003,2004,2019
//


#include <string>
#include <vector>

#include "Arbitrator.h"
#include "Command.h"
#include "CycleManager.h"

namespace psi_act {

  /**
   * \attention: the implemenation assumes that within the same
   *             cycle of the control loop, checkCommitmentCondition,
   *             checkInvocationCondition, and getCommand all are called
   *             with only the same single timestamp; if you need handling
   *             of queries with different timestamps within the same
   *             cycle, you might have to overwrite these three methods.
   */
  class PriorityArbitration : public Arbitrator {
  public:
    PriorityArbitration(const std::string& name="PriorityArbitration");
    virtual ~PriorityArbitration() noexcept;

    virtual Command* calculateCommand(const TimePoint&) noexcept(false);

    /** true, if at least one of the available options is invocable */
    virtual bool checkInvocationCondition(const TimePoint& time) noexcept;
    /** true, if the presently executed option can continue, or at least
      one different option is invocable */
    virtual bool checkCommitmentCondition(const TimePoint& time) noexcept;

    virtual void gainControl(const TimePoint&) noexcept;
    virtual void loseControl(const TimePoint&) noexcept;

    virtual std::string currentOptionName() const;

    /** 
     * @brief Adds the given argument to the end of the list of available 
     * options, with lowest priority.
     *
     * \param interrupatable if true, this option may be interrupted by another
     *                       option of higher priority.
     *
     * \param takeoverOwnership if true, the plan owns the given option and
     *                          has to take care of freeing its memory
     */
    psi_act::PriorityArbitration*
    addOption(Option* option,
              bool interruptable     = true,
              bool takeoverOwnership = true) noexcept;

    virtual std::string listOptions(bool listNested=true);

    std::vector<Option*> getOptions(void) {
      std::vector<Option*> opts;
      for (OptionWithFlags& stage : this->options) {
        opts.emplace_back(stage.option);
      }
      return opts;
    }

  protected:
    virtual void determineUpcomingOption(const TimePoint& time) noexcept;

    struct OptionWithFlags {
      Option* option;
      bool interrupatable;
      bool owned;
    };
    std::vector<OptionWithFlags> options; ///< in order of decreasing priority

    int upcomingOption; ///<
    int presentOption;
  };


  class AlternativeOptions {
  public:
    // selects and follows the most applicable
  protected:
    std::vector<Option*> options;
  };

}

#endif
