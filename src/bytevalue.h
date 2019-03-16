#ifndef CBYTEVALUE_H
#define CBYTEVALUE_H

#include <cstdint>     // needed for uint64_t
#include <string>      // needed for std::string
#include <type_traits> // needed for std::enable_if
#include <map>         // needed for std::map

#include <QString>

// here follows a little trick to calculate the pow function at build time (in recursive manner)
// I use it to define, at build time, constants that will be used later

//!
//! \brief literal operator _p10
//!
//! Evaluated at build time: this can be used in constant declaration.
//! This computes the pow function recursively.
//!
//! \param v  exponent to use to calculate the power of 10
//!
//! \return value corresponding to 10^v
//!
constexpr unsigned long long operator"" _p10(unsigned long long v)
{
    return ((v == 0) ? 1 : (10 * operator""_p10(v-1)));
}

// Another method to compute pow of 2 (at build time) that simply uses bit shift operator:
// this time I choosr a literal operator


//!
//! \brief literal operator _p2
//!
//! Evaluated at build time: this can be used in constant declaration.
//! This computes the pow function using shift operator
//!
//! \param v  exponent to use to calculate the power of 10
//!
//! \return value corresponding to 10^v
//!
constexpr unsigned long long operator"" _p2(unsigned long long v)
{
    return ((v == 0) ? 1 : (static_cast<uint64_t>(2) << (v-1)));
}

// The previous code should be enough but, to improve readability of sources,
// I've chose to define also some constants using the previous structs/custom literal operator suffix

static constexpr uint64_t A_BYTE{ 1 }; //!< Value of a byte (obviously it's just 1)

// Here follows constants that uses pow of 10

static constexpr uint64_t A_KILOBYTE{  3_p10 }; //!< 1 kilobyte in bytes = 10^3 bytes
static constexpr uint64_t A_MEGABYTE{  6_p10 }; //!< 1 megabyte in bytes = 10^6 bytes
static constexpr uint64_t A_GIGABYTE{  9_p10 }; //!< 1 gigabyte in bytes = 10^9 bytes
static constexpr uint64_t A_TERABYTE{ 12_p10 }; //!< 1 terabyte in bytes = 10^12 bytes

// Here follows constants that uses pow of 2

static constexpr uint64_t A_KIBIBYTE{ 10_p2 }; //!< 1 kibibyte in bytes = 2^10 bytes = 0x400         bytes = 1024          bytes
static constexpr uint64_t A_MEBIBYTE{ 20_p2 }; //!< 1 mebibyte in bytes = 2^20 bytes = 0x100000      bytes = 1048576       bytes
static constexpr uint64_t A_GIBIBYTE{ 30_p2 }; //!< 1 gibibyte in bytes = 2^30 bytes = 0x40000000    bytes = 1073741824    bytes
static constexpr uint64_t A_TEBIBYTE{ 40_p2 }; //!< 1 tebibyte in bytes = 2^40 bytes = 0x10000000000 bytes = 1099511627776 bytes

// Here follow definitions of custom literal suffix operators used later in the code to define constants
// related to the unit beasure based on byte values

//!
//! \brief literal suffix operator _B
//! \param v  Value in byte
//! \return   Value in byte (just th specified value)
//!
constexpr unsigned long long operator""_B(unsigned long long v) { return v; }

// Please notice that the following literal operators (except for kilobyte case) have name beginning wuth _underscore followed by capital letter.
// To avoid confusion with reserved words (names with underscore followed by capital letters are used by keywords) and, according to C++ recommended standard,
// I omit the space between "" and the name of the literal.

//!
//! \brief literal suffix operator _KiB (kibibytes to bytes conversion)
//!
//! Converts the specified kibibyte value in bytes
//!
//! \param v  Value in kibibyte
//! \return   Value in bytes corresponding to specified KiB value
//!
constexpr unsigned long long operator""_KiB(unsigned long long v) { return (A_KIBIBYTE * v); }

//!
//! \brief literal suffix operator _MiB (mebibytes to bytes conversion)
//!
//! Converts the specified mebibyte value in bytes
//!
//! \param v  Value in mebibyte
//! \return   Value in bytes corresponding to specified MiB value
//!
constexpr unsigned long long operator""_MiB(unsigned long long v) { return (A_MEBIBYTE * v); }

//!
//! \brief literal suffix operator _GiB (gibibytes to bytes conversion)
//!
//! Converts the specified gibibyte value in bytes
//!
//! \param v  Value in gibibyte
//! \return   Value in bytes corresponding to specified GiB value
//!
constexpr unsigned long long operator""_GiB(unsigned long long v) { return (A_GIBIBYTE * v); }

//!
//! \brief literal suffix operator _TiB (tebibytes to bytes conversion)
//!
//! Converts the specified gibibyte value in bytes
//!
//! \param v  Value in tebibyte
//! \return   Value in bytes corresponding to specified TiB value
//!
constexpr unsigned long long operator""_TiB(unsigned long long v) { return (A_TEBIBYTE * v); }

//!
//! \brief literal suffix operator _kB (kilobytes to bytes conversion)
//!
//! Converts the specified kilobyte value in bytes
//!
//! \param v  Value in kilobyte
//! \return   Value in bytes corresponding to specified kB value
//!
constexpr unsigned long long operator""_kB(unsigned long long v) { return (A_KILOBYTE * v); }

//!
//! \brief literal suffix operator _MB (megabytes to bytes conversion)
//!
//! Converts the specified megabyte value in bytes
//!
//! \param v  Value in megabyte
//! \return   Value in bytes corresponding to specified MB value
//!
constexpr unsigned long long operator""_MB(unsigned long long v) { return (A_MEGABYTE * v); }

//!
//! \brief literal suffix operator _GB (gigabytes to bytes conversion)
//!
//! Converts the specified gigabyte value in bytes
//!
//! \param v  Value in gigabyte
//! \return   Value in bytes corresponding to specified GB value
//!
constexpr unsigned long long operator""_GB(unsigned long long v) { return (A_GIGABYTE * v); }

//!
//! \brief literal suffix operator _TB (terabytes to bytes conversion)
//!
//! Converts the specified terabyte value in bytes
//!
//! \param v  Value in terabyte
//! \return   Value in bytes corresponding to specified TB value
//!
constexpr unsigned long long operator""_TB(unsigned long long v) { return (A_TERABYTE * v); }

// The following are used just to verify previous constants/operators
static_assert(1_KiB == A_KIBIBYTE, "KiB failed");
static_assert(2_MiB == 2*A_MEBIBYTE, "MiB failed");
static_assert(3_GiB == 3*A_GIBIBYTE, "GiB failed");
static_assert(4_TiB == 4*A_TEBIBYTE, "TiB failed");

static_assert(1_kB == A_KILOBYTE, "kB failed");
static_assert(2_MB == 2*A_MEGABYTE, "MB failed");
static_assert(3_GB == 3*A_GIGABYTE, "GB failed");
static_assert(4_TB == 4*A_TERABYTE, "TB failed");



constexpr auto LABEL_B = "B";       //! label to use for byte


constexpr auto LABEL_kB = "kB";     //! label to use for kilobyte
constexpr auto LABEL_MB = "MB";     //! label to use for megabyte
constexpr auto LABEL_GB = "GB";     //! label to use for gigabyte
constexpr auto LABEL_TB = "TB";     //! label to use for terabyte

constexpr auto LABEL_KiB = "KiB";   //! label to use for kibybyte
constexpr auto LABEL_MiB = "MiB";   //! label to use for mebybyte
constexpr auto LABEL_GiB = "GiB";   //! label to use for gibybyte
constexpr auto LABEL_TiB = "TiB";   //! label to use for tebybyte


//!
//! \brief The CByteValue class
//!
//! Simple and stupic helper class used to represent and to manage byte values
//!
class CByteValue
{
public:
    using bytevalue_t = uint64_t;  //!< numeric type used to memorize value of bytes. It should be at least a unsigned 64 bit type

private:
    bytevalue_t m_value; //! Value of bytes

public:

    //!
    //! \brief The decimalUnit_t enum class: type used to identify the units of measure based on decimal system (pow of 10)
    //!
    enum class decimalUnit_t : bytevalue_t
    {
        B = 1,              //!< byte
        kB = A_KILOBYTE,    //!< kilobyte (= 10^3 bytes)
        MB = A_MEGABYTE,    //!< megabyte (= 10^6 bytes)
        GB = A_GIGABYTE,    //!< gigabyte (= 10^9 bytes)
        TB = A_TERABYTE,    //!< terabyte (= 10^12 bytes)
    };


    //!
    //! \brief The binarylUnit_t enum class: type used to identify the units of measure based on binary system (pow of 2)
    //!
    enum class binaryUnit_t : bytevalue_t
    {
        B   = 1,            //!< byte
        KiB = A_KIBIBYTE,   //!< kibibyte  (= 2^10 bytes)
        MiB = A_MEBIBYTE,   //!< membibyte (= 2^20 bytes)
        GiB = A_GIBIBYTE,   //!< gibibyte  (= 2^30 bytes)
        TiB = A_TEBIBYTE,   //!< tebibyte  (= 2^40 bytes)
    };

public:

    //!
    //! \brief CByteValue default constructor
    //!
    //! \param[in] v  Value in bytes used to initialize the class instance
    //!
    explicit CByteValue(const bytevalue_t& v = 0) : m_value(v)
    {
    }

    //!
    //! \brief ~CByteValue class destructor
    //!
    virtual ~CByteValue();

    //!
    //! \brief rank_decimalUnit Returns the "rank" (=highest unit in decimal system) of the specified value
    //!
    //! \param[in] value_in_bytes
    //!
    //! Retrieve the highest "rank"/unit of measure in decimal system (well, at least its identifier).
    //! For example, if you specify a value of 3000 this function classifies the input value as a value that
    //! can be expressed in kilobytes (kB is the highest rank/unit of measure) that can be used to represent
    //! this value with a integer > 0) and so in this case returns decimalUnit::kB
    //!
    //! \return identifier of decimal unit/rank
    //!
    static decimalUnit_t rank_decimalUnit(const bytevalue_t& value_in_bytes);

    //!
    //! \brief rank_decimalUnit
    //! \return Returns the "rank" (=highest unit in decimal system) of value stored in instance
    //!
    //! \sa static decimalUnit_t rank_decimalUnit(const value_t& value_in_bytes);
    //!
    decimalUnit_t rank_decimalUnit() const
    {
        return CByteValue::rank_decimalUnit(m_value);
    }

    //!
    //! \brief rank_binaryUnit Returns the "rank" (=highest unit in "binary" system) of the specified value
    //!
    //! \param[in] value_in_bytes
    //!
    //! Retrieve the highest "rank"/unit of measure in decimal system (well, at least its identifier).
    //! For example, if you specify a value of 4096 this function classifies the input value as a value that
    //! can be expressed in kibibytes (KiB is the highest rank/unit of measure) that can be used to represent
    //! this value with a integer > 0) and so in this case returns binaryUnit::KiB
    //!
    //! \return identifier of decimal unit/rank
    //!
    static binaryUnit_t rank_binaryUnit(const bytevalue_t& value_in_bytes);

    //!
    //! \brief rank_binaryUnit
    //! \return Returns the "rank" (=highest unit in binary system) of value stored in instance
    //!
    //! \sa static binaryUnit_t rank_binaryUnit(const value_t& value_in_bytes);
    //!
    binaryUnit_t rank_binaryUnit() const
    {
        return CByteValue::rank_binaryUnit(m_value);
    }

    //!
    //! \brief decimalUnitLabel Returns label of "highest" rank/unit of measure (decimal system) that can be used for specified value
    //!
    //! \tparam the_string_type string type to use (currently only std::string, std::wstring, QString have implementation)
    //!
    //! \note currently a string among B (byte), kB (kilobyte), GB (gigabyte), MB (megabyte), TB (terabyte) is returned
    //!
    //! \return string representing the unit of measure (highest rank) in decimal system
    //!
    template <class the_string_type>
    static const the_string_type& decimalUnitLabel(decimalUnit_t dut);

    //!
    //! \brief decimalUnitLabel Returns label of "highest" rank/unit of measure (decimal system) that can be used to express the instance value
    //!
    //! \note currently a string among B (byte), kB (kilobyte), GB (gigabyte), MB (megabyte), TB (terabyte) is returned
    //!
    //! \return string representing the unit of measure (highest rank) in decimal system
    //!
    //! \sa template <class the_string_type> static const the_string_type& decimalUnitLabel(decimalUnit_t dut);
    //!
    template <class the_string_type>
    const the_string_type& decimalUnitLabel() const
    {
        return CByteValue::decimalUnitLabel<the_string_type>(rank_decimalUnit());
    }

    //!
    //! \brief binaryUnitLabel Returns label of "highest" rank/unit of measure (binary system) that can be used for specified value
    //!
    //! \tparam the_string_type string type to use (currently only std::string, std::wstring, QString have implementation)
    //!
    //! \note currently a string among B (byte), KiB (kibibyte), GiB (gibibyte), MiB (mebibyte), TB (tebibyte) is returned
    //!
    //! \return string representing the unit of measure (highest rank) in binary system
    //!
    template <typename the_string_type>
    static const the_string_type& binaryUnitLabel(binaryUnit_t dut);

    //!
    //! \brief binaryUnitLabel Returns label of "highest" rank/unit of measure (binary system)that can be used to express the instance value
    //!
    //! \tparam the_string_type string type to use (currently only std::string, std::wstring, QString have implementation)
    //!
    //! \note currently a string among B (byte), KiB (kibibyte), GiB (gibibyte), MiB (mebibyte), TB (tebibyte) is returned
    //!
    //! \return string representing the unit of measure (highest rank) in binary system
    //!
    //! \sa template <typename the_string_type> static const the_string_type& binaryUnitLabel(binaryUnit_t dut);
    //!
    template <typename the_string_type>
    const the_string_type& binaryUnitLabel() const
    {
        return CByteValue::binaryUnitLabel<the_string_type>(rank_binaryUnit());
    }

    //!
    //! \brief to_decimalValue Convert specified byte value in a value corresponding to specified unit of measure (decimal system)
    //! \param[in] du    id of unit of measure to use for conversion (="destination" unit of measure)
    //! \param[in] value value in bytes to convert
    //!
    //! \return value converted in specified unit of measure
    //!
    static double to_decimalValue(decimalUnit_t du, const bytevalue_t& value)
    {
        if (du == decimalUnit_t::B)
        {
            return static_cast<double>(value);
        }
        return (static_cast<double>(value) / static_cast<double>(du));
    }

    //!
    //! \brief to_binaryValue Convert specified byte value in a value corresponding to specified unit of measure (binary system)
    //! \param[in] bu    id of unit of measure to use for conversion (="destination" unit of measure)
    //! \param[in] value value in bytes to convert
    //!
    //! \return value converted in specified unit of measure
    //!
    static double to_binaryValue(binaryUnit_t bu, const bytevalue_t& value)
    {
        if (bu == binaryUnit_t::B)
        {
            return static_cast<double>(value);
        }
        return (static_cast<double>(value) / static_cast<double>(bu));
    }

    //!
    //! \brief getValueAndRank_decimal Retrieves highest rank/unit of measure and converts specified byte value (decimal system)
    //!
    //! \tparam the_string_type string class to use for unit of measure label (currently only std::string, std::wstring, QString can be safely used)
    //!
    //! \param[in] value_in_bytes          value in bytes to convert
    //! \param[out] value_in_decimal_unit  value converted in unit of measure corresponding to determined rank
    //! \param[out] decimal_unit_label     label of unit of measure corresponding to determined rank
    //!
    template <typename the_string_type>
    static void getValueAndRank_decimal(bytevalue_t value_in_bytes, double& value_in_decimal_unit, the_string_type& decimal_unit_label)
    {
        decimalUnit_t ut = rank_decimalUnit(value_in_bytes);
        value_in_decimal_unit = to_decimalValue(ut, value_in_bytes);
        decimal_unit_label = decimalUnitLabel<the_string_type>(ut);
    }

    //!
    //! \brief getValueAndRank_decimal Retrieves highest rank/unit of measure and converts instance byte value (decimal system)
    //!
    //! \tparam the_string_type string class to use for unit of measure label (currently only std::string, std::wstring, QString can be safely used)
    //!
    //! \param[out] value_in_decimal_unit  value converted in unit of measure corresponding to determined rank
    //! \param[out] decimal_unit_label     label of unit of measure corresponding to determined rank
    //!
    //! \sa template <typename the_string_type> static void getValueAndRank_decimal(bytevalue_t value_in_bytes, double& value_in_decimal_unit, the_string_type& decimal_unit_label)
    //!
    template <typename the_string_type>
    void getValueAndRank_decimal(double& value_in_decimal_unit, the_string_type& decimal_unit_label) const
    {
        CByteValue::getValueAndRank_decimal<the_string_type>(m_value, value_in_decimal_unit, decimal_unit_label);
    }

    //!
    //! \brief getValueAndRank_binary Retrieves highest rank/unit of measure and converts specified byte value (binary system)
    //!
    //! \tparam the_string_type string class to use for unit of measure label (currently only std::string, std::wstring, QString can be safely used)
    //!
    //! \param[in] value_in_bytes         value in bytes to convert
    //! \param[out] value_in_binary_unit  value converted in unit of measure corresponding to determined rank
    //! \param[out] binary_unit_label     label of unit of measure corresponding to determined rank
    //!
    template <typename the_string_type>
    static void getValueAndRank_binary(bytevalue_t value_in_bytes, double& value_in_binary_unit, the_string_type& binary_unit_label)
    {
        binaryUnit_t ut = rank_binaryUnit(value_in_bytes);
        value_in_binary_unit = to_binaryValue(ut, value_in_bytes);
        binary_unit_label = binaryUnitLabel<the_string_type>(ut);
    }


    //!
    //! \brief convertToRankValueAndGetStrings_decimal Converts specified amount of bytes in a value using the highest rank/unit and returns it and the used unit of measure as strings (decimal system)
    //!
    //! This is the base template; currently only implementations using std::string, std::wstring and QString are provided.
    //!
    //! \tparam the_string_type  string type to use (must be one among std::string, std::wstring, QString)
    //!
    //! \param[in]  v                 amount of bytes to convert
    //! \param[out] got_rank_value    string returned containing value expressed in unit of measure corresponding to got_rank_value
    //! \param[out] got_rank_label    string returned containing used unit of measure
    //!
    template <typename the_string_type>
    static void convertToRankValueAndGetStrings_decimal(const CByteValue::bytevalue_t v, the_string_type& got_rank_value, the_string_type& got_rank_label);

    //!
    //! \brief convertToRankValueAndGetStrings_decimal Converts specified amount of bytes in a value using the highest rank/unit and returns it and the used unit of measure as strings (binary system)
    //!
    //! This is the base template; currently only implementations using std::string, std::wstring and QString are provided.
    //!
    //! \tparam the_string_type  string type to use (must be one among std::string, std::wstring, QString)
    //!
    //! \param[in]  v                 amount of bytes to convert
    //! \param[out] got_rank_value    string returned containing value expressed in unit of measure corresponding to got_rank_value
    //! \param[out] got_rank_label    string returned containing used unit of measure
    //!
    template <typename the_string_type>
    static void convertToRankValueAndGetStrings_binary(const CByteValue::bytevalue_t v, the_string_type& got_rank_value, the_string_type& got_rank_label);

    //!
    //! \brief convertToRankValueAndGetStrings_rate Converts specified amount of bytes used to express rates in a value using the highest rank/unit and returns it and the used unit of measure as strings
    //!
    //! This is the base template; currently only implementations using std::string, std::wstring and QString are provided.
    //!
    //! \note usually rates are expressed using unit of measure based on powers of 10 ("decimal system"), so this is just an alias for convertToRankValueAndGetStrings_decimal
    //! 
    //! \tparam the_string_type  string type to use (must be one among std::string, std::wstring, QString)
    //!
    //! \param[in]  v                 amount of bytes to convert
    //! \param[out] got_rank_value    string returned containing value expressed in unit of measure corresponding to got_rank_value
    //! \param[out] got_rank_label    string returned containing used unit of measure
    //!
    //! \sa convertToRankValueAndGetStrings_decimal
    //! 
    template <typename the_string_type>
    static void convertToRankValueAndGetStrings_rate(const CByteValue::bytevalue_t v, the_string_type& got_rank_value, the_string_type& got_rank_label)
    {
        return CByteValue::convertToRankValueAndGetStrings_decimal<the_string_type>(v, got_rank_value, got_rank_label);
    }

    //!
    //! \brief convertToRankValueAndGetStrings_size Converts specified amount of bytes used to express file sizes in a value using the highest rank/unit and returns it and the used unit of measure as strings
    //!
    //! This is the base template; currently only implementations using std::string, std::wstring and QString are provided.
    //!
    //! \note usually rates are expressed using unit of measure based on powers of 2 ("binary system"), so this is just an alias for convertToRankValueAndGetStrings_binary
    //! 
    //! \tparam the_string_type  string type to use (must be one among std::string, std::wstring, QString)
    //!
    //! \param[in]  v                 amount of bytes to convert
    //! \param[out] got_rank_value    string returned containing value expressed in unit of measure corresponding to got_rank_value
    //! \param[out] got_rank_label    string returned containing used unit of measure
    //!
    //! \sa convertToRankValueAndGetStrings_decimal
    //! 
    template <typename the_string_type>
    static void convertToRankValueAndGetStrings_size(const CByteValue::bytevalue_t v, the_string_type& got_rank_value, the_string_type& got_rank_label)
    {
        return CByteValue::convertToRankValueAndGetStrings_binary<the_string_type>(v, got_rank_value, got_rank_label);
    }

    //!
    //! \brief to_bytes
    //! \return Value in bytes
    //!
    const bytevalue_t& to_bytes() const
    {
        return m_value;
    }

    //!
    //! \brief to_decimalValue Convert to specified unit of measure (decimal system)
    //! \param[in] du id of unit of measure to use for conversion
    //! \return value converted to specified unit of measure
    //!
    double to_decimalValue(decimalUnit_t du) const
    {
        return CByteValue::to_decimalValue(du, m_value);
    }

    //!
    //! \brief to_binaryValue Convert to specified unit of measure (binary system)
    //! \param[in] bu id of unit of measure to use for conversion
    //! \return value converted to specified unit of measure
    //!
    double to_binaryValue(binaryUnit_t bu) const
    {
        return CByteValue::to_binaryValue(bu, m_value);
    }


    //!
    //! \brief to_kilobyte Convert to kilobyte
    //! \return value expressed in kilobytes
    //!
    double to_kilobyte() const
    {
        return to_decimalValue(decimalUnit_t::kB);
    }

    //!
    //! \brief to_megabyte Convert to megabyte
    //! \return value expressed in megabytes
    //!
    double to_megabyte() const
    {
        return to_decimalValue(decimalUnit_t::MB);
    }

    //!
    //! \brief to_gigabyte Convert to gigabyte
    //! \return value expressed in gigabytes
    //!
    double to_gigabyte() const
    {
        return to_decimalValue(decimalUnit_t::GB);
    }

    //!
    //! \brief to_terabyte Convert to terabyte
    //! \return value expressed in terabytes
    //!
    double to_terabyte() const
    {
        return to_decimalValue(decimalUnit_t::TB);
    }

    //!
    //! \brief to_kibibyte Convert to kibibyte
    //! \return value expressed in kibibytes
    //!
    double to_kibibyte() const
    {
        return to_binaryValue(binaryUnit_t::KiB);
    }

    //!
    //! \brief to_mebibyte Convert to mebibyte
    //! \return value expressed in mebibytes
    //!
    double to_mebibyte() const
    {
        return to_binaryValue(binaryUnit_t::MiB);
    }

    //!
    //! \brief to_gibibyte Convert to gibibyte
    //! \return value expressed in gibibytes
    //!
    double to_gibibyte() const
    {
        return to_binaryValue(binaryUnit_t::GiB);
    }

    //!
    //! \brief to_tebibyte Convert to tebibyte
    //! \return value expressed in tebibytes
    //!
    double to_tebibyte() const
    {
        return to_binaryValue(binaryUnit_t::TiB);
    }

    //!
    //! \brief operator const bytevalue_t &
    //!
    //! Just to return a const reference to stored value
    //!
    operator const bytevalue_t& () const
    {
        return to_bytes();
    }

};

// here follows declaration of extern templates used for template implementations/specializations using std::string, std::wstring, QString


template<>
const std::string& CByteValue::decimalUnitLabel<std::string>(CByteValue::decimalUnit_t dut);

template<>
const std::wstring& CByteValue::decimalUnitLabel<std::wstring>(CByteValue::decimalUnit_t dut);

template<>
const QString& CByteValue::decimalUnitLabel<QString>(CByteValue::decimalUnit_t dut);

template <>
void CByteValue::convertToRankValueAndGetStrings_decimal<std::string>(const CByteValue::bytevalue_t v, std::string& got_rank_value, std::string& got_rank_label);

template <>
void CByteValue::convertToRankValueAndGetStrings_decimal<std::wstring>(const CByteValue::bytevalue_t v, std::wstring& got_rank_value, std::wstring& got_rank_label);

template <>
void CByteValue::convertToRankValueAndGetStrings_decimal<QString>(const CByteValue::bytevalue_t v, QString& got_rank_value, QString& got_rank_label);

extern template
const std::string& CByteValue::decimalUnitLabel<std::string>(CByteValue::decimalUnit_t dut);

extern template
const std::wstring& CByteValue::decimalUnitLabel<std::wstring>(CByteValue::decimalUnit_t dut);

extern template
const QString& CByteValue::decimalUnitLabel<QString>(CByteValue::decimalUnit_t dut);

extern template
void CByteValue::convertToRankValueAndGetStrings_decimal<std::string>(const CByteValue::bytevalue_t v, std::string& got_rank_value, std::string& got_rank_label);

extern template
void CByteValue::convertToRankValueAndGetStrings_decimal<std::wstring>(const CByteValue::bytevalue_t v, std::wstring& got_rank_value, std::wstring& got_rank_label);

extern template
void CByteValue::convertToRankValueAndGetStrings_decimal<QString>(const CByteValue::bytevalue_t v, QString& got_rank_value, QString& got_rank_label);



template<>
const std::string& CByteValue::binaryUnitLabel<std::string>(CByteValue::binaryUnit_t dut);

template<>
const std::wstring& CByteValue::binaryUnitLabel<std::wstring>(CByteValue::binaryUnit_t dut);

template<>
const QString& CByteValue::binaryUnitLabel<QString>(CByteValue::binaryUnit_t dut);

template <>
void CByteValue::convertToRankValueAndGetStrings_binary<std::string>(const CByteValue::bytevalue_t v, std::string& got_rank_value, std::string& got_rank_label);

template <>
void CByteValue::convertToRankValueAndGetStrings_binary<std::wstring>(const CByteValue::bytevalue_t v, std::wstring& got_rank_value, std::wstring& got_rank_label);

template <>
void CByteValue::convertToRankValueAndGetStrings_binary<QString>(const CByteValue::bytevalue_t v, QString& got_rank_value, QString& got_rank_label);

extern template
const std::string& CByteValue::binaryUnitLabel<std::string>(CByteValue::binaryUnit_t dut);

extern template
const std::wstring& CByteValue::binaryUnitLabel<std::wstring>(CByteValue::binaryUnit_t dut);

extern template
const QString& CByteValue::binaryUnitLabel<QString>(CByteValue::binaryUnit_t dut);

extern template
void CByteValue::convertToRankValueAndGetStrings_binary<std::string>(const CByteValue::bytevalue_t v, std::string& got_rank_value, std::string& got_rank_label);

extern template
void CByteValue::convertToRankValueAndGetStrings_binary<std::wstring>(const CByteValue::bytevalue_t v, std::wstring& got_rank_value, std::wstring& got_rank_label);

extern template
void CByteValue::convertToRankValueAndGetStrings_binary<QString>(const CByteValue::bytevalue_t v, QString& got_rank_value, QString& got_rank_label);

#endif // CBYTEVALUE_H
