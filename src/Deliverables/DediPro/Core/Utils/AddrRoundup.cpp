#include "AddrRoundup.h"
#include <boost/foreach.hpp>

namespace address_util
{
	namespace helper
	{
		bool is_all_0xFF(std::vector<unsigned char> v)
		{
			std::vector<unsigned char>::iterator itr, itr_end;
			itr_end = v.end();
			for(itr = v.begin(); itr != itr_end; ++itr)
			{
				if(*itr ^ 0xFF) return false;
			}
			return true;
		}

	}


	CAddrRoundup::CAddrRoundup(size_t step)
		: m_step(step), m_out()
	{
	}

	CAddrRoundup::~CAddrRoundup(void)
	{
	}

	std::pair<size_t, size_t> CAddrRoundup::SectionRound(size_t addr)
	{
		return std::make_pair(addr & (~(m_step - 1)), addr | (m_step - 1));
	}

	std::pair<size_t, size_t> CAddrRoundup::SectionRound(const std::pair<size_t, size_t>& addr_range)
	{
		return std::make_pair(addr_range.first &(~(m_step - 1)) , 
			((addr_range.second & (m_step - 1)) ? (addr_range.second | (m_step - 1)) + 1 : addr_range.second));
	}

	std::vector<size_t>& CAddrRoundup::GenerateDiff(const std::vector<unsigned char>& in1, const std::vector<unsigned char>& in2,  size_t baseAddr)
	{
		m_out.clear();

		size_t upper = std::min(in1.size(), in2.size());
		size_t realAddr;

		for(size_t i = 0; i < upper; ++i)
		{
			if(in1[i] != in2[i])
			{
				realAddr = i + baseAddr;
				m_out.push_back(realAddr & (~(m_step - 1)));

				i += ((m_step - 1)|realAddr) - realAddr;
				if(i >= upper) break;
			}
		}

		return m_out;
	}

	size_t CAddrRoundup::GetStep() const
	{
		return m_step;
	}

	std::vector<size_t> CAddrRoundup::Condense(const std::vector<unsigned char>& vc, std::vector<size_t>& addrs, size_t baseAddr)
	{
		std::vector<size_t> v;
		std::vector<size_t>::iterator itr, itr_end(addrs.end());

		for(itr = addrs.begin(); itr != itr_end; ++itr)
		{
			if(!helper::is_all_0xFF(std::vector<unsigned char>(boost::next(vc.begin(), *itr - baseAddr), boost::next(vc.begin(), *itr - baseAddr + m_step))))
				v.push_back(*itr);
		}

		return v;

		//BOOST_FOREACH(size_t& addr, addrs)
		//{
		//	BOOST_FOREACH(unsigned char& c, vc)
		//	{
		//		//if(c !=			
		//	}
		//}
	}
}

