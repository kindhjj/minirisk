#include "TradeFXForward.h"
#include "PricerFXForward.h"

namespace minirisk
{

ppricer_t TradeFXForward::pricer() const
{
    return ppricer_t(new PricerFXForward(*this));
}

} // namespace minirisk