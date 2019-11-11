#pragma once

#include <memory>

#include "IObject.h"
#include "Market.h"
#include "FixingDataServer.h"

namespace minirisk {

struct IPricer : IObject
{
    virtual std::pair<double, string> price(Market& m, ptr_fds_t& fds) const = 0;
    virtual string get_ir_curve() const = 0;
};

typedef std::shared_ptr<const IPricer> ppricer_t;

} // namespace minirisk
