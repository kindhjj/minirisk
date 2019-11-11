#include "CurveFXSpot.h"
#include "Market.h"
#include "Streamer.h"


namespace minirisk 
{
    CurveFXSpot::CurveFXSpot(Market *mkt, const Date& today, const string& curve_name)
        :m_today(mkt->today())
        ,m_name(fx_spot_prefix + curve_name)
        ,m_fx_spot(mkt->get_fx_spot(fx_spot_prefix + curve_name))
        {}

    double CurveFXSpot::spot() const
    {
        return m_fx_spot;
    }
}