/// These are the digital organisms we are using for the diagnostic experiments

#ifndef DIA_PRO_H
#define DIA_PRO_H

///< standard headers
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <string>
#include <utility>

///< empirical headers
#include "base/vector.h"

class Diagnostic
{
  public:
    //typename for target vector
    using target_t = emp::vector<double>;
    using score_t = emp::vector<double>;
    using genome_t = emp::vector<double>;

  public:

    Diagnostic(target_t & t_)
    {
      target.resize(t_.size());
      std::copy(t_.begin(), t_.end(), target.begin());
    }

    // getters
    target_t GetTarget() const {return target;}
    // const target_t & GetTarget() const {return target;}

    //setters
    void SetTarget(target_t & t) {target.clear(); target.resize(t.size()); std::copy(t.begin(), t.end(), target.begin());}

    // exploration function
    score_t Exploration(const genome_t & g);
    // exploitation function
    score_t Exploitation(const genome_t & g);
    // ecological niche function
    score_t Exploration(const genome_t & g);
    // structured exploration function
    score_t Exploration(const genome_t & g);

  private:
    // variable that holds target vector
    target_t target;
};
#endif