#pragma once

#include <utility>
#include <vector>

namespace address_util
{
	namespace helper
	{
		bool is_all_0xFF(std::vector<unsigned char> v);
	
	}

    class CAddrRoundup
    {
    public:
        CAddrRoundup(size_t step);
        ~CAddrRoundup(void);

    public:
        size_t CAddrRoundup::GetStep() const;
		std::vector<size_t> Condense(const std::vector<unsigned char>& vc, std::vector<size_t>& addrs, size_t baseAddr);

        std::pair<size_t, size_t> SectionRound(size_t addr);
        std::pair<size_t, size_t> SectionRound(const std::pair<size_t, size_t>& addr_range);
        std::vector<size_t>& GenerateDiff(const std::vector<unsigned char>& in1, const std::vector<unsigned char>& in2,  size_t baseAddr);
    private:
        size_t m_step;
        std::vector<size_t> m_out;

    };

}

