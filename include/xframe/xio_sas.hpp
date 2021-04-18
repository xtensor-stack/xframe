/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay, Wolf Vollprecht and         *
* Martin Renou                                                             *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XFRAME_IO_SAS_HPP
#define XFRAME_IO_SAS_HPP

#include <string>
#include <fstream>

#include <xtl/xany.hpp>

#include "xvariable.hpp"

namespace xf
{
    enum class sas_format
    {
        sas7bdat,
        xport
    };
//    inline xf::xvariable<xtl::any, xf::xcoordinate<xf::fstring>> read_sas(const std::ifstream& ifs, const sas_format& format = sas_format::sas7bdata);
    void read_sas(const std::ifstream& ifs, const sas_format& format = sas_format::sas7bdat);

    namespace detail
    {
        class xsas7bdat_parser
        {
        public:
            explicit xsas7bdat_parser(std::ifstream& ifs);

            xsas7bdat_parser(xsas7bdat_parser&) = delete;
            xsas7bdat_parser(xsas7bdat_parser&&) = delete;
            xsas7bdat_parser& operator=(const xsas7bdat_parser&) = delete;
            xsas7bdat_parser& operator=(xsas7bdat_parser&&) = delete;
            void parse_meta();
            void parse_data();

        private:
            using memory_data_type = std::vector<char>;
            using memory_data_iterator = std::vector<char>::iterator;
            using difference_type = typename memory_data_type::difference_type;
            using size_type = memory_data_type::size_type;
            using value_type = memory_data_type::value_type;

            enum class endian : uint8_t
            {
                endian_big = 0x00,
                endian_little = 0x01
            };

            enum class column_type : uint8_t
            {
                column_type_number = 0x01,
                column_type_char = 0x02
            };


            enum class aligment_offset : uint8_t
            {
                aligment_offset_0 = 0x22,
                aligment_offset_4 = 0x33
            };

            enum class subheader_signature_type : uint64_t
            {
                subheader_signature_row_size = 0xF7F7F7F7,
                subheader_signature_column_size = 0xF6F6F6F6,
                subheader_signature_counts = 0xFFFFFC00,
                subheader_signature_column_label = 0xFFFFFBFE,
                subheader_signature_column_attrs = 0xFFFFFFFC,
                subheader_signature_column_text = 0xFFFFFFFD,
                subheader_signature_column_list = 0xFFFFFFFE,
                subheader_signature_column_name = 0xFFFFFFFF,
            };

            enum class page_type : uint16_t
            {
                meta = 0x0000,
                data = 0x0100,
                mix = 0x0200,
                amd = 0x0400,
                mask = 0x0F00,
                meta2 = 0x4000,
                comp = 0x9000,
            };

            enum class subheader_pointer_offset : uint8_t
            {
                subheader_pointer_offset_32bit = 12,
                subheader_pointer_offset_64bit = 24,
            };

            enum class compression_type : uint8_t
            {
                none = 0x00,
                trunc = 0x01,
                rle = 0x04
            };

            enum class rle_command : uint8_t
            {
                copy64 = 0,
                unknown1 = 1,
                unknown2 = 2,
                unknown3 = 3,
                insert_byte18 = 4,
                insert_at17 = 5,
                insert_blank17 = 6,
                insert_zero17 = 7,
                copy1 = 8,
                copy17 = 9,
                copy33 = 10,
                copy49 = 11,
                insert_byte3 = 12,
                insert_at2 = 13,
                insert_blank2 = 14,
                insert_zero2 = 15
            };

#pragma pack(push, 1)
            struct subheader
            {
                memory_data_type data;
                uint64_t signature;
                uint8_t compression;
            };
#pragma pack(pop)

            bool little_endian();

            template <typename T>
            auto swap_endian(const T&) -> T;
            
            memory_data_type load_data(std::ifstream& ifs, uint64_t length);

            template <typename T>
            auto read_memory_data(memory_data_type memory_data, uint64_t pos, bool swap) -> T;

            template <typename Container>
            auto read_memory_data(memory_data_type memory_data, uint64_t pos, uint64_t length) -> Container;

            template <typename T>
            auto iterator_memory_data(memory_data_iterator& memory_data_it, bool swap) -> T;

            template <typename Container>
            auto iterator_memory_data(memory_data_iterator& memory_data_it, uint64_t length) -> Container;

            void parse_head();
            std::vector<subheader> parse_page_subheader(memory_data_type& page_memory);
            void parse_colname_subheader(std::vector<subheader>& colname_subheader_vec, std::vector<subheader>& coltext_subheader_vec);
            void parse_collabel_subheader(std::vector<subheader>& collabel_subheader_vec, std::vector<subheader>& coltext_subheader_vec);
            void parse_colattr_subheader(std::vector<subheader>& colattr_subheader_vec);

            std::ifstream& m_sas_ifs;
            bool m_64bit {false};
            bool m_swap_endian {false};
            uint64_t m_page_count {0};
            uint64_t m_header_size {0};
            uint64_t m_page_size {0};
            std::vector<std::string> m_colname_vec;
            std::vector<std::string> m_colfmt_vec;
            std::vector<std::string> m_collabel_vec;
            std::vector<column_type> m_coltype_vec;
        };

        xsas7bdat_parser::xsas7bdat_parser(std::ifstream& ifs) : m_sas_ifs(ifs)
        {
        }

        inline bool xsas7bdat_parser::little_endian()
        {
            const int value { 0x01 };
            const void * address = static_cast<const void *>(&value);
            const unsigned char * least_significant_address = static_cast<const unsigned char *>(address);
            return (*least_significant_address == 0x01);
        }

        template <typename T>
        inline auto xsas7bdat_parser::swap_endian(const T &val) -> T
        {
            union
            {
                T val;
                std::array<std::uint8_t, sizeof (T)> raw;
            } src, dst;
            src.val = val;
            std::reverse_copy(src.raw.begin(), src.raw.end(), dst.raw.begin());
            return dst.val;
        }

        inline xsas7bdat_parser::memory_data_type xsas7bdat_parser::load_data(std::ifstream& ifs, uint64_t length)
        {
            memory_data_type memory_data;
            memory_data.resize(static_cast<size_t>(length));
            ifs.read(&memory_data[0], static_cast<std::streamsize>(length));
            return memory_data;
        }

        template <typename T>
        auto xsas7bdat_parser::read_memory_data(memory_data_type memory_data, uint64_t pos, bool swap) -> T
        {
            T ret;
            std::memcpy(&ret, &memory_data[static_cast<size_type>(pos)], sizeof (T));
            if (swap)
                ret = swap_endian(ret);
            return ret;
        }

        template <typename Container>
        auto xsas7bdat_parser::read_memory_data(memory_data_type memory_data, uint64_t pos, uint64_t length) -> Container
        {
            Container ret(memory_data.begin() + static_cast<difference_type>(pos), memory_data.begin() + static_cast<difference_type>(pos + length));
            return ret;
        }

        template <typename T>
        auto xsas7bdat_parser::iterator_memory_data(memory_data_iterator& memory_data_it, bool swap) -> T
        {
            T ret;
            std::memcpy(&ret, &*memory_data_it, sizeof (T));
            if (swap)
                ret = swap_endian(ret);
            memory_data_it += sizeof (T);
            return ret;
        }

        template <typename Container>
        auto xsas7bdat_parser::iterator_memory_data(memory_data_iterator& memory_data_it, uint64_t length) -> Container
        {
            Container ret(memory_data_it, memory_data_it + static_cast<difference_type>(length));
            memory_data_it += static_cast<difference_type>(length);
            return ret;
        }

        void xsas7bdat_parser::parse_meta()
        {
            parse_head();
            std::vector<subheader> rowsize_subheader_vec;
            std::vector<subheader> colsize_subheader_vec;
            std::vector<subheader> coltext_subheader_vec;
            std::vector<subheader> colname_subheader_vec;
            std::vector<subheader> colattr_subheader_vec;
            std::vector<subheader> collabs_subheader_vec;

            for (uint64_t idx = 0; idx < m_page_count; idx++)
            {
                auto page_offset = m_header_size + idx * m_page_size;
                if (!m_sas_ifs.seekg(static_cast<std::ifstream::off_type>(page_offset), m_sas_ifs.beg))
                    throw std::runtime_error("parse sas error");

                auto page_memory = load_data(m_sas_ifs, m_page_size);
                auto p_type = m_64bit ? read_memory_data<uint16_t>(page_memory, 16, m_swap_endian) : read_memory_data<uint16_t>(page_memory, 32, m_swap_endian);
                if (static_cast<page_type>(p_type) == page_type::comp)
                    continue;
                if (static_cast<page_type>(p_type) == page_type::data)
                    break;

                auto sub_header_vec = parse_page_subheader(page_memory);
                for (auto& sub_header : sub_header_vec)
                {
                    switch (static_cast<subheader_signature_type>(sub_header.signature))
                    {
                    case subheader_signature_type::subheader_signature_row_size:
                        rowsize_subheader_vec.emplace_back(std::move(sub_header));
                        break;
                    case subheader_signature_type::subheader_signature_column_size:
                        colsize_subheader_vec.emplace_back(std::move(sub_header));
                        break;
                    case subheader_signature_type::subheader_signature_column_text:
                        coltext_subheader_vec.emplace_back(std::move(sub_header));
                        break;
                    case subheader_signature_type::subheader_signature_column_name:
                        colname_subheader_vec.emplace_back(std::move(sub_header));
                        break;
                    case subheader_signature_type::subheader_signature_column_attrs:
                        colattr_subheader_vec.emplace_back(std::move(sub_header));
                        break;
                    case subheader_signature_type::subheader_signature_column_label:
                        collabs_subheader_vec.emplace_back(std::move(sub_header));
                        break;
                    default:
                        break;
                    }
                }
            }
            parse_colname_subheader(colname_subheader_vec, coltext_subheader_vec);
            parse_collabel_subheader(collabs_subheader_vec, coltext_subheader_vec);
            parse_colattr_subheader(colattr_subheader_vec);
        }

        void xsas7bdat_parser::parse_data()
        {

        }

        inline void xsas7bdat_parser::parse_head()
        {
            constexpr unsigned char sas7bdat_magic_number[32] = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0xc2, 0xea, 0x81, 0x60,
                0xb3, 0x14, 0x11, 0xcf, 0xbd, 0x92, 0x08, 0x00,
                0x09, 0xc7, 0x31, 0x8c, 0x18, 0x1f, 0x10, 0x11
            };
            auto basic_info_memory_data = load_data(m_sas_ifs, 38);
            auto magic_number = read_memory_data<std::string>(basic_info_memory_data, 0, static_cast<uint64_t>(sizeof (sas7bdat_magic_number)));
            if (std::memcmp(magic_number.c_str(), sas7bdat_magic_number, sizeof (sas7bdat_magic_number)) != 0)
                throw std::runtime_error("magic number not match");

            auto a2 = read_memory_data<uint8_t>(basic_info_memory_data, 32, false);
            auto a1 = read_memory_data<uint8_t>(basic_info_memory_data, 35, false);
            auto file_endian = read_memory_data<uint8_t>(basic_info_memory_data, 37, false);
            if (static_cast<aligment_offset>(a1) == aligment_offset::aligment_offset_4)
                a1 = 4;
            else
                a1 = 0;
            if (static_cast<aligment_offset>(a2) == aligment_offset::aligment_offset_4)
                m_64bit = true;
            m_swap_endian = false;
            if (static_cast<endian>(file_endian) == endian::endian_big)
                m_swap_endian = little_endian();
            else if (static_cast<endian>(file_endian) == endian::endian_little)
                m_swap_endian = !little_endian();
            else
                throw std::runtime_error("unsupport endian");

            if (!m_sas_ifs.seekg(a1 + 196 - 38, m_sas_ifs.cur))
                throw std::runtime_error("seek sas stream error");

            auto header_size_memory_data = load_data(m_sas_ifs, 4);
            m_header_size = read_memory_data<uint32_t>(header_size_memory_data, 0, m_swap_endian);

            auto page_info_memory_data = m_64bit ? load_data(m_sas_ifs, 12) : load_data(m_sas_ifs, 8);
            auto it = page_info_memory_data.begin();
            m_page_size = iterator_memory_data<uint32_t>(it, m_swap_endian);
            m_page_count = m_64bit ? iterator_memory_data<uint64_t>(it, m_swap_endian) : iterator_memory_data<uint32_t>(it, m_swap_endian);
        }

        std::vector<xsas7bdat_parser::subheader> xsas7bdat_parser::parse_page_subheader(memory_data_type& page_memory)
        {
            std::vector<subheader> ret_vec;
            auto it = page_memory.begin();
            it = m_64bit ? it + 36 : it + 20;
            auto subheader_pointers_count = iterator_memory_data<uint16_t>(it, m_swap_endian);
            it = it + 2;
            for (auto idx = 0; idx < subheader_pointers_count; idx++)
            {
                auto offset_to_subhead = m_64bit ? iterator_memory_data<uint64_t>(it, m_swap_endian) : iterator_memory_data<uint32_t>(it, m_swap_endian);
                auto length = m_64bit ? iterator_memory_data<uint64_t>(it, m_swap_endian) : iterator_memory_data<uint32_t>(it, m_swap_endian);
                if (length == 0)
                    continue;
                auto compression = iterator_memory_data<uint8_t>(it, m_swap_endian);
                it = it + (m_64bit ? 7 : 3);
                if (static_cast<compression_type>(compression) == compression_type::none)
                {
                    auto subheader_it = page_memory.begin() + static_cast<difference_type>(offset_to_subhead);
                    subheader sub_header;
                    sub_header.data = iterator_memory_data<memory_data_type>(subheader_it, length);
                    sub_header.signature = m_64bit ? read_memory_data<uint64_t>(sub_header.data, 0, m_swap_endian)
                                                   : read_memory_data<uint32_t>(sub_header.data, 0, m_swap_endian);
                    sub_header.compression = compression;
                    ret_vec.emplace_back(sub_header);
                }
                else if (static_cast<compression_type>(compression) == compression_type::rle)
                {
                    auto subheader_it = page_memory.begin() + static_cast<difference_type>(offset_to_subhead);
                    subheader sub_header;
                    sub_header.data = iterator_memory_data<memory_data_type>(subheader_it, length);
                    auto it = sub_header.data.begin();
                    while (it != sub_header.data.end())
                    {
                        auto control = iterator_memory_data<uint8_t>(it, false);
                        auto command = (control & 0xF0) >> 4;
                        auto length = (control & 0x0F);
                        memory_data_type memory_data;
                        switch (static_cast<rle_command>(command))
                        {
                        case rle_command::copy64:
                        {
                            auto copy_len = iterator_memory_data<uint8_t>(it, false) + 64 + length * 256;
                            memory_data = iterator_memory_data<memory_data_type>(it, static_cast<uint64_t>(copy_len));
                            break;
                        }
                        case rle_command::unknown1:
                        case rle_command::unknown2:
                        case rle_command::unknown3:
                            break;
                        case rle_command::insert_byte18:
                        {
                            auto insert_len = iterator_memory_data<uint8_t>(it, false) + 18 + length * 256;
                            auto insert_byte = iterator_memory_data<uint8_t>(it, false);
                            memory_data.insert(memory_data.begin(), static_cast<size_type>(insert_len), static_cast<value_type>(insert_byte));
                        }
                            break;
                        case rle_command::insert_at17:
                        {
                            auto insert_len = iterator_memory_data<uint8_t>(it, false) + 17 + length * 256;
                            auto insert_byte = '@';
                            memory_data.insert(memory_data.begin(), static_cast<size_type>(insert_len), static_cast<value_type>(insert_byte));
                            break;
                        }
                        case rle_command::insert_blank17:
                        {
                            auto insert_len = iterator_memory_data<uint8_t>(it, false) + 17 + length * 256;
                            auto insert_byte = ' ';
                            memory_data.insert(memory_data.begin(), static_cast<size_type>(insert_len), static_cast<value_type>(insert_byte));
                            break;
                        }
                        case rle_command::insert_zero17:
                        {
                            auto insert_len = iterator_memory_data<uint8_t>(it, false) + 17 + length * 256;
                            auto insert_byte = '\0';
                            memory_data.insert(memory_data.begin(), static_cast<size_type>(insert_len), static_cast<value_type>(insert_byte));
                            break;
                        }
                        case rle_command::copy1:
                        {
                            auto copy_len = length + 1;
                            memory_data = iterator_memory_data<memory_data_type>(it, static_cast<uint64_t>(copy_len));
                            break;
                        }
                        case rle_command::copy17:
                        {
                            auto copy_len = length + 17;
                            memory_data = iterator_memory_data<memory_data_type>(it, static_cast<uint64_t>(copy_len));
                            break;
                        }
                        case rle_command::copy33:
                        {
                            auto copy_len = length + 33;
                            memory_data = iterator_memory_data<memory_data_type>(it, static_cast<uint64_t>(copy_len));
                            break;
                        }
                        case rle_command::copy49:
                        {
                            auto copy_len = length + 49;
                            memory_data = iterator_memory_data<memory_data_type>(it, static_cast<uint64_t>(copy_len));
                            break;
                        }
                        case rle_command::insert_byte3:
                        {
                            auto insert_len = length + 3;
                            auto insert_byte = iterator_memory_data<uint8_t>(it, false);
                            memory_data.insert(memory_data.begin(), static_cast<size_type>(insert_len), static_cast<value_type>(insert_byte));
                            break;
                        }
                        case rle_command::insert_at2:
                        {
                            auto insert_len = length + 2;
                            auto insert_byte = '@';
                            memory_data.insert(memory_data.begin(), static_cast<size_type>(insert_len), static_cast<value_type>(insert_byte));
                            break;
                        }
                        case rle_command::insert_blank2:
                        {
                            auto insert_len = length + 2;
                            auto insert_byte = ' ';
                            memory_data.insert(memory_data.begin(), static_cast<size_type>(insert_len), static_cast<value_type>(insert_byte));
                            break;
                        }
                        case rle_command::insert_zero2:
                        {
                            auto insert_len = length + 2;
                            auto insert_byte = '\0';
                            memory_data.insert(memory_data.begin(), static_cast<size_type>(insert_len), static_cast<value_type>(insert_byte));
                            break;
                        }
                        }
                    }
                }
                else
                {
                    // do nothing.
                }

            }
            return ret_vec;
        }

        void xsas7bdat_parser::parse_colname_subheader(std::vector<subheader>& colname_subheader_vec, std::vector<subheader>& coltext_subheader_vec)
        {
            if (colname_subheader_vec.size() == 0 || coltext_subheader_vec.size() == 0)
                throw std::runtime_error("colsize subheader vec expect greater than 0");
            for (auto& colname_subheader : colname_subheader_vec)
            {
                auto subheader_pointor_length = m_64bit ? colname_subheader.data.size() - 28 : colname_subheader.data.size() - 20;
                auto subheader_pointor_count = subheader_pointor_length / 8;
                auto subheader_pointor_offset = m_64bit ? 16 : 12;
                auto pointor_it = colname_subheader.data.begin() + subheader_pointor_offset;
                for (size_t idx = 0; idx < subheader_pointor_count; idx++)
                {
                    auto it = pointor_it + static_cast<difference_type>(idx * 8);
                    auto select_index = iterator_memory_data<uint16_t>(it, m_swap_endian);
                    auto offset = iterator_memory_data<uint16_t>(it, m_swap_endian) + (m_64bit ? 8 : 4);
                    auto length = iterator_memory_data<uint16_t>(it, m_swap_endian);
                    auto colname = read_memory_data<std::string>(coltext_subheader_vec[select_index].data, static_cast<uint64_t>(offset), static_cast<uint64_t>(length));
                    m_colname_vec.emplace_back(std::move(colname));
                }
            }
        }

        void xsas7bdat_parser::parse_collabel_subheader(std::vector<subheader>& collabel_subheader_vec, std::vector<subheader>& coltext_subheader_vec)
        {
            if (collabel_subheader_vec.size() == 0 || coltext_subheader_vec.size() == 0)
                throw std::runtime_error("colsize subheader vec expect greater than 0");
            for (auto& collab : collabel_subheader_vec)
            {
                auto it = m_64bit ? collab.data.begin() + 46 : collab.data.begin() + 34;
                auto select_index = iterator_memory_data<uint16_t>(it, m_swap_endian);
                auto offset = iterator_memory_data<uint16_t>(it, m_swap_endian) + (m_64bit ? 8 : 4);
                auto length = iterator_memory_data<uint16_t>(it, m_swap_endian);
                if (length > 0)
                {
                    auto fmt = read_memory_data<std::string>(coltext_subheader_vec[select_index].data, static_cast<uint64_t>(offset), static_cast<uint64_t>(length));
                    m_colfmt_vec.emplace_back(std::move(fmt));
                }
                
                it = m_64bit ? collab.data.begin() + 52 : collab.data.begin() + 40;
                select_index = iterator_memory_data<uint16_t>(it, m_swap_endian);
                offset = iterator_memory_data<uint16_t>(it, m_swap_endian) + (m_64bit ? 8 : 4);
                length = iterator_memory_data<uint16_t>(it, m_swap_endian);
                if (length > 0)
                {
                    auto label = read_memory_data<std::string>(coltext_subheader_vec[select_index].data, static_cast<uint64_t>(offset), static_cast<uint64_t>(length));
                    m_collabel_vec.emplace_back(std::move(label));
                }
            }
        }

        void xsas7bdat_parser::parse_colattr_subheader(std::vector<subheader>& colattr_subheader_vec)
        {
            if (colattr_subheader_vec.size() == 0 )
                throw std::runtime_error("colattr subheader vec expect greater than 0");
            for (auto& colattr_subheader : colattr_subheader_vec)
            {
                auto subheader_pointor_length = m_64bit ? colattr_subheader.data.size() - 28 : colattr_subheader.data.size() - 20;
                auto subheader_pointor_count = m_64bit ? subheader_pointor_length / 16 : subheader_pointor_length / 12;
                auto subheader_pointor_offset = m_64bit ? 16 : 12;
                auto pointor_it = colattr_subheader.data.begin() + subheader_pointor_offset;
                for (size_t idx = 0; idx < subheader_pointor_count; idx++)
                {
                    auto it = pointor_it + static_cast<difference_type>(idx * (m_64bit ? 16 : 12));
                    it = it + static_cast<difference_type>(m_64bit ? 14 : 10);
                    auto col_type = iterator_memory_data<uint8_t>(it, m_swap_endian);
                    m_coltype_vec.emplace_back(static_cast<column_type>(col_type));
                }
            }
        }
    }
//    inline xf::xvariable<xtl::any, xf::xcoordinate<xf::fstring>> xf::read_sas(const std::ifstream& ifs, const sas_format& format = sas_format::sas7bdata)
//    {
//        if (format == sas_format::sas7bdata)
//        {
//           detail::xsas7bdat_parser reader(ifs);
//           return reader.parse();
//        }
//        else
//        {

//        }
//        return xf::xvariable<xtl::any, xf::xcoordinate<xf::fstring>();
//    }
}

#endif
