#pragma once

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <vector>

#include "numeric_conversion.h"

using boost::filesystem::path;
using boost::filesystem::recursive_directory_iterator;

namespace tfit
{
	using namespace std;

	struct item
	{
	public:
		item(const wstring& _fullname)
			:fullname(_fullname)
			,type()
			,manufacturer()
			,size()
		{
			init();
		}
	public:
		wstring manufacturer;
		wstring type;
		size_t size;
		wstring fullname;
	private:
		void init()
		{
			std::vector<wstring> v;
			boost::algorithm::split(v, fullname, boost::algorithm::is_any_of(L"_."));

			if(5 == v.size())
			{
				manufacturer = v.at(1);
				type = v.at(2);
				size = numeric_conversion::hexstring_to_size_t(v.at(3));
			}    

		}
	};

	class CFindTFIT
	{
	public:
		CFindTFIT(const path& dir_path) 
			: m_dir_path(dir_path)
			, m_items()
		{
			iterate_files();
		}
	private:
		bool iterate_files( )
		{
			if ( !exists( m_dir_path ) ) return false;
			recursive_directory_iterator end_itr; // default construction yields past-the-end
			for ( recursive_directory_iterator itr( m_dir_path );
				itr != end_itr;
				++itr )
			{
				wstring  file_name(itr->path().filename().wstring());
				if ( !is_directory(*itr) &&  boost::algorithm::istarts_with(file_name, L"TFIT_"))
				{
					//m_type_list.push_back(get_type(filename));
					//m_targets.push_back(*itr);
					m_items.push_back(item(file_name));
				}
			}
			return true;
		}

	public:

		const vector<item>& get_typelist() const
		{
			return m_items;
		}

		item get_selected(const wstring& s)
		{
			BOOST_FOREACH(const item& i, m_items)
			{
				if(boost::algorithm::equals(i.type, s))	return i;
			}
			return item(L"");
		}

		static bool is_vpp_version(const wstring& s)
		{
			return boost::algorithm::icontains(s, L"vpp");

		}

	private:
		path m_dir_path;
		vector<item> m_items;
	};




}

