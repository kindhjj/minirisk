#include "CurveFXSpot.h"
#include "Market.h"
#include "Streamer.h"


namespace minirisk 
{
    CurveFXSpot::CurveFXSpot(Market *mkt, const Date& today, const string& fx_name)
        :m_today(mkt->today())
        ,m_name(fx_name)
        {
            if (fx_name.substr(fx_name.length() - 3) == base_ccy)
                m_fx_spot = 1.0;
            else
                m_fx_spot = mkt->get_fx_spot(fx_name);
        }

        double CurveFXSpot::spot() const
        {
            return m_fx_spot;
    }
}