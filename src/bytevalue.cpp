#include "bytevalue.h"

#include <map>      // needed for std::map
#include <cmath>    // needed for std::modf
#include <sstream>  // needed for std::ostringstream
#include <iomanip>  // needed for stream manip

CByteValue::~CByteValue()
{
}

// static
CByteValue::decimalUnit_t CByteValue::rank_decimalUnit(const bytevalue_t& value_in_bytes)
{
    if (value_in_bytes >= 1_TB)
    {
        return CByteValue::decimalUnit_t::TB;
    }
    if (value_in_bytes >= 1_GB)
    {
        return CByteValue::decimalUnit_t::GB;
    }
    if (value_in_bytes >= 1_MB)
    {
        return CByteValue::decimalUnit_t::MB;
    }
    if (value_in_bytes >= 1_kB)
    {
        return CByteValue::decimalUnit_t::kB;
    }
    return CByteValue::decimalUnit_t::B;
}

// static
CByteValue::binaryUnit_t CByteValue::rank_binaryUnit(const bytevalue_t& value_in_bytes)
{
    if (value_in_bytes >= 1_TiB)
    {
        return CByteValue::binaryUnit_t::TiB;
    }
    if (value_in_bytes >= 1_GiB)
    {
        return CByteValue::binaryUnit_t::GiB;
    }
    if (value_in_bytes >= 1_MiB)
    {
        return CByteValue::binaryUnit_t::MiB;
    }
    if (value_in_bytes >= 1_KiB)
    {
        return CByteValue::binaryUnit_t::KiB;
    }
    return CByteValue::binaryUnit_t::B;
}



template<>
const std::string& CByteValue::decimalUnitLabel<std::string>(CByteValue::decimalUnit_t dut)
{
    static const std::map<CByteValue::decimalUnit_t, std::string> labels{
        {CByteValue::decimalUnit_t::B, LABEL_B},
        {CByteValue::decimalUnit_t::kB, LABEL_kB},
        {CByteValue::decimalUnit_t::MB, LABEL_MB},
        {CByteValue::decimalUnit_t::GB, LABEL_GB},
        {CByteValue::decimalUnit_t::TB, LABEL_TB},
    };

    return labels.at(dut);
}

template<>
const std::string& CByteValue::binaryUnitLabel<std::string>(CByteValue::binaryUnit_t dut)
{
    static const std::map<CByteValue::binaryUnit_t, std::string> labels{
        {CByteValue::binaryUnit_t::B, LABEL_B},
        {CByteValue::binaryUnit_t::KiB, LABEL_KiB},
        {CByteValue::binaryUnit_t::MiB, LABEL_MiB},
        {CByteValue::binaryUnit_t::GiB, LABEL_GiB},
        {CByteValue::binaryUnit_t::TiB, LABEL_TiB},
    };

    return labels.at(dut);
}



// I really hate define but here probably I cannot do any different
#define CONCAT(A,B) A ## B
#define TO_WIDE(x) CONCAT(L, #x)

template<>
const std::wstring& CByteValue::decimalUnitLabel<std::wstring>(CByteValue::decimalUnit_t dut)
{
    static const std::map<CByteValue::decimalUnit_t, std::wstring> labels{
        {CByteValue::decimalUnit_t::B, TO_WIDE(LABEL_B)},
        {CByteValue::decimalUnit_t::kB, TO_WIDE(LABEL_kB)},
        {CByteValue::decimalUnit_t::MB, TO_WIDE(LABEL_MB)},
        {CByteValue::decimalUnit_t::GB, TO_WIDE(LABEL_GB)},
        {CByteValue::decimalUnit_t::TB, TO_WIDE(LABEL_TB)},
    };

    return labels.at(dut);

}


template<>
const std::wstring& CByteValue::binaryUnitLabel<std::wstring>(CByteValue::binaryUnit_t dut)
{
    static const std::map<CByteValue::binaryUnit_t, std::wstring> labels{
        {CByteValue::binaryUnit_t::B, TO_WIDE(LABEL_B)},
        {CByteValue::binaryUnit_t::KiB, TO_WIDE(LABEL_KiB)},
        {CByteValue::binaryUnit_t::MiB, TO_WIDE(LABEL_MiB)},
        {CByteValue::binaryUnit_t::GiB, TO_WIDE(LABEL_GiB)},
        {CByteValue::binaryUnit_t::TiB, TO_WIDE(LABEL_TiB)},
    };

    return labels.at(dut);
}


template<>
const QString& CByteValue::decimalUnitLabel<QString>(CByteValue::decimalUnit_t dut)
{
    static const std::map<CByteValue::decimalUnit_t, QString> labels{
        {CByteValue::decimalUnit_t::B, LABEL_B},
        {CByteValue::decimalUnit_t::kB, LABEL_kB},
        {CByteValue::decimalUnit_t::MB, LABEL_MB},
        {CByteValue::decimalUnit_t::GB, LABEL_GB},
        {CByteValue::decimalUnit_t::TB, LABEL_TB},
    };

    return labels.at(dut);
}


template<>
const QString& CByteValue::binaryUnitLabel<QString>(CByteValue::binaryUnit_t dut)
{
    static const std::map<CByteValue::binaryUnit_t, QString> labels{
        {CByteValue::binaryUnit_t::B, LABEL_B},
        {CByteValue::binaryUnit_t::KiB, LABEL_KiB},
        {CByteValue::binaryUnit_t::MiB, LABEL_MiB},
        {CByteValue::binaryUnit_t::GiB, LABEL_GiB},
        {CByteValue::binaryUnit_t::TiB, LABEL_TiB},
    };

    return labels.at(dut);
}


template<>
void CByteValue::convertToRankValueAndGetStrings_decimal<std::string>(const CByteValue::bytevalue_t v, std::string& got_rank_value, std::string& got_rank_label)
{
    double numeric_rank_value {};
    CByteValue::getValueAndRank_decimal<std::string>(v, numeric_rank_value, got_rank_label);

    std::ostringstream o;
    o << std::setw(4) << std::setprecision(3) << numeric_rank_value;
    got_rank_value = o.str();
}


template<>
void CByteValue::convertToRankValueAndGetStrings_decimal<std::wstring>(const CByteValue::bytevalue_t v, std::wstring& got_rank_value, std::wstring& got_rank_label)
{
    double numeric_rank_value {};
    CByteValue::getValueAndRank_decimal<std::wstring>(v, numeric_rank_value, got_rank_label);

    std::wostringstream o;
    o << std::setw(4) << std::setprecision(3) << numeric_rank_value;
    got_rank_value = o.str();
}


template<>
void CByteValue::convertToRankValueAndGetStrings_decimal<QString>(const CByteValue::bytevalue_t v, QString& got_rank_value, QString& got_rank_label)
{
    double numeric_rank_value {};
    CByteValue::getValueAndRank_decimal<QString>(v, numeric_rank_value, got_rank_label);

    double fract_part = std::modf(numeric_rank_value, &fract_part);
    if (fract_part != 0.0)
    {
        got_rank_value = QString().setNum(numeric_rank_value,'f', 1);
    }
    else
    {
        got_rank_value = QString("%0").arg(static_cast<int>(numeric_rank_value));
    }
}


template<>
void CByteValue::convertToRankValueAndGetStrings_binary<QString>(const CByteValue::bytevalue_t v, QString& got_rank_value, QString& got_rank_label)
{
    double numeric_rank_value {};
    CByteValue::getValueAndRank_binary<QString>(v, numeric_rank_value, got_rank_label);

    double fract_part = std::modf(numeric_rank_value, &fract_part);
    if (fract_part != 0.0)
    {
        got_rank_value = QString().setNum(numeric_rank_value,'f', 1);
    }
    else
    {
        got_rank_value = QString("%0").arg(static_cast<int>(numeric_rank_value));
    }
}
