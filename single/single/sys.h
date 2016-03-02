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


inline 
std::pair<void*, size_t> MapFile(const std::wstring& FileName)
{

	struct TFileHandle	{
		HANDLE Handle;
		~TFileHandle() { CloseHandle( Handle ); }
	} File = { CreateFileW( FileName.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 ) };

	if( INVALID_HANDLE_VALUE == File.Handle )
	{
		std::wstring erm = L"MapFile,CreateFile failed. " + LastErrStr();
		Log(erm);
		throw TException(erm);
	}

	struct TFileMapping {
		HANDLE Handle;
		~TFileMapping() { CloseHandle( Handle ); }
	} FileMapping = { CreateFileMappingW(
		File.Handle,		// set NULL if you want using paging file
		NULL,               // default security
		PAGE_READONLY,      // read/write access
		0,                  // maximum object size (high-order DWORD)
		0,					// maximum object size (low-order DWORD)
		0)					// name of mapping object
	};
	if( NULL == FileMapping.Handle )
	{
		std::wstring erm = L"Could not create file mapping object.\n" + LastErrStr();
		Log(erm);
		throw TException(erm);
	}

	struct TMapView {
		void* Buf;
		~TMapView() { if( Buf ) UnmapViewOfFile(Buf); }
	} MapView = { MapViewOfFile( FileMapping.Handle, FILE_MAP_READ, 0, 0, 0 ) };

	if( NULL == MapView.Buf )
	{
		std::wstring erm = L"Could not map view of file.\n" + LastErrStr();
		Log(erm);
		throw TException(erm);
	}	

	DWORD FileSize;
	FileSize = GetFileSize(File.Handle, 0);
	if (FileSize == INVALID_FILE_SIZE)
	{
		std::wstring erm = L"GetFileSize failed. " + LastErrStr();
		Log(erm);
		throw TException(erm);
	}

	auto result = std::make_pair( MapView.Buf, FileSize );
	MapView.Buf = 0;
	return result;
}

inline std::wstring IntToWStr( int i )
{
	wchar_t buf[128];
	_itow_s( i, buf, 10 );
	return std::wstring( buf );
}

