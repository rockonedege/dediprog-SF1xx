#include <boost\filesystem.hpp>
#include <string>

using std::wstring;
using boost::filesystem::path;
using boost::filesystem::directory_iterator;

struct BoardInfo
{
    wstring         board_type;
    wstring         version;
};

class CFirmwareFile
{
public:
    CFirmwareFile(BoardInfo& info);

public:
    bool isFileCorrect(const wstring& sFolder);

    wstring getLatestVersionPathPrefix(std::pair<wstring,wstring>& v);

private:
    BoardInfo   m_info;
    boost::filesystem::path       m_path;
};
