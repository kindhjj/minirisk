#pragma once

#include "ITrade.h"
#include "Streamer.h"
#include <sstream>
#include <string>

namespace minirisk {

template <typename T>
struct Trade : ITrade
{
    virtual double quantity() const
    {
        return m_quantity;
    }

    virtual uint64_t hex_quantity() const
    {
        return m_q_hex.h;
    }

    void init(double quantity = 1.0)
    {
        m_quantity = quantity;
    }

    virtual const guid_t& id() const
    {
        return T::m_id;
    }

    virtual const std::string& idname() const
    {
        return T::m_name;
    }

protected:
    virtual void print(std::ostream& os) const
    {
        os << format_label("Id") << id() << std::endl;
        os << format_label("Name") << idname() << std::endl;
        os << format_label("Quantity") << quantity() << std::endl;
        static_cast<const T*>(this)->print_details(os);
        os << std::endl;
    }

    virtual void save(my_ofstream& os) const
    {
        std::stringstream ofs;
        ofs << std::hex << hex_quantity();
        string tmp_str;
        ofs >> tmp_str;
        os << id()
           << tmp_str;
        static_cast<const T *>(this)->save_details(os);
    }

    virtual void load(my_ifstream& is)
    {
        // read everything but id
        std::string str;
        is >> str;
        std::istringstream tmp_str(str);
        tmp_str >> std::hex >> m_q_hex.h;
        m_quantity = m_q_hex.d;
        static_cast<T *>(this)->load_details(is);
    }

private:
    double m_quantity;
    union{double d; uint64_t h;} m_q_hex;
};

} // namespace minirisk
