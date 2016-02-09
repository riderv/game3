#pragma once

inline std::wstring GetExePatch()
{
	static const int FileName_BufSize = 4 * 1024;
	wchar_t FileName[FileName_BufSize] = { 0 };
	wchar_t FilePath[FileName_BufSize] = { 0 };
	int PathLen = GetModuleFileNameW(NULL, FilePath, FileName_BufSize);
	if (int er = GetLastError())
	{
		std::wstring w(L"GetModuleFileNameW faliled. ");
		w += LastErrStr(er);
		Log(w);
		throw TException(w);
	}
	// Ёх, где же ты удобна€ VCL
	*wcsrchr(FilePath, L'\\') = L'\0';
	return std::wstring(FilePath);
}

struct ICallable
{
	virtual ICallable::~ICallable() {}
	virtual void ICallable::operator()() = 0;
};

class TFinally
{
	std::vector<ICallable*> vec;
	typedef std::vector<ICallable*>::reverse_iterator rev_itr;
public:
	~TFinally()
	{
		for (rev_itr v = vec.rbegin(); v != vec.rend(); ++v) {
			(*v)->operator()();
			delete (*v);
		}
	}
	template <class T>
	void operator+=(const T& fun)
	{
		struct Callable: ICallable {
			T t;
			void operator()()override {
				t();
			}
			Callable(const T& f) : t(f) {}
		};
		vec.push_back(new Callable(fun));
	}
	int Count() const { return static_cast<int> (vec.size()); }
	void Erase(int i)
	{
		if (i < 0 && size_t(i) >= vec.size())
			throw TException(L"TFinally::Erase(i) out of range.");

		ICallable* C = vec[i];
		delete C;
		vec.erase(vec.begin() + i);
	}
};

inline 
std::pair<void*, size_t> MapFile(const std::wstring& FileName)
{
	TFinally Finally;

	HANDLE hFile;
	HANDLE hMapFile;
	void* pBuf;

	hFile = CreateFileW(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		std::wstring erm = L"MapFile,CreateFile failed. " + LastErrStr();
		Log(erm);
		throw TException(erm);
	}
	Finally += [&]() { CloseHandle(hFile); };

	hMapFile = CreateFileMappingW(
		hFile,    // use paging file
		NULL,                    // default security
		PAGE_READONLY,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		0,				 // maximum object size (low-order DWORD)
		0);                 // name of mapping object

	if (hMapFile == NULL)
	{
		std::wstring erm = L"Could not create file mapping object.\n" + LastErrStr();
		Log(erm);
		throw TException(erm);
	}
	Finally += [&]() { CloseHandle(hMapFile); };

	pBuf = MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_READ, // read/write permission
		0,
		0,
		0);

	if (pBuf == NULL)
	{
		std::wstring erm = L"Could not map view of file.\n" + LastErrStr();
		Log(erm);
		throw TException(erm);
	}
	Finally += [&]() { UnmapViewOfFile(pBuf); };

	DWORD FileSize;
	FileSize = GetFileSize(hFile, 0);
	if (FileSize == INVALID_FILE_SIZE)
	{
		std::wstring erm = L"GetFileSize failed. " + LastErrStr();
		Log(erm);
		throw TException(erm);
	}

	Finally.Erase(Finally.Count() - 1);
	return std::make_pair(pBuf, FileSize);
}