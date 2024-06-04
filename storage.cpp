#include <stdio.h>
#include <string.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <objidl.h>
#include "ncbind/ncbind.hpp"
#include <map>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include "mz_compat.h"
#include "mz_strm.h"
#include "CharacterSet.h"

extern mz_stream_vtbl tvp_mz_stream_vtbl;

struct tvp_mz_stream_t
{
	mz_stream stream;
	IStream *file;
	bool initalized;

	tvp_mz_stream_t()
	{
		file = NULL;
		initalized = false;
		memset(&stream, 0, sizeof(stream));
		stream.vtbl = &tvp_mz_stream_vtbl;
	}
};

int32_t tvp_mz_stream_open (void *stream, const char *filename, int mode)
{
	tvp_mz_stream_t *tvpstream = (tvp_mz_stream_t*)stream;
	int tjsmode = 0;
	if ((mode & MZ_OPEN_MODE_READWRITE) == MZ_OPEN_MODE_READ)
	{
		tjsmode = TJS_BS_READ;
	}
	else if (mode & MZ_OPEN_MODE_EXISTING)
	{
		tjsmode = TJS_BS_APPEND;
	}
	else if (mode & MZ_OPEN_MODE_CREATE)
	{
		tjsmode = TJS_BS_WRITE;
	}

	if (filename != NULL)
	{
		tvpstream->file = TVPCreateIStream(ttstr((const tjs_char*)filename), tjsmode);
		tvpstream->initalized = true;
	}
	return MZ_OK;
}

int32_t tvp_mz_stream_is_open(void *stream)
{
	tvp_mz_stream_t *tvpstream = (tvp_mz_stream_t*)stream;
	if (!tvpstream->initalized)
	{
		return MZ_OPEN_ERROR;
	}
	return MZ_OK;
}

int32_t tvp_mz_stream_read(void *stream, void* buf, int32_t size)
{
	tvp_mz_stream_t *tvpstream = (tvp_mz_stream_t*)stream;
	IStream *istrm = tvpstream->file;
	if (istrm)
	{
		ULONG len;
		if (istrm->Read(buf, size, &len) == S_OK)
		{
			return len;
		}
	}
	return MZ_DATA_ERROR;
}

int32_t tvp_mz_stream_write(void *stream, const void* buf, int32_t size)
{
	tvp_mz_stream_t *tvpstream = (tvp_mz_stream_t*)stream;
	IStream *istrm = tvpstream->file;
	if (istrm)
	{
		DWORD len;
		if (istrm->Write(buf, size, &len) == S_OK)
		{
			return len;
		}
	}
	return MZ_DATA_ERROR;
}

int64_t tvp_mz_stream_tell(void *stream)
{
	tvp_mz_stream_t *tvpstream = (tvp_mz_stream_t*)stream;
	IStream *istrm = tvpstream->file;
	if (istrm)
	{
		LARGE_INTEGER move = { .QuadPart = (LONGLONG)0 };
		ULARGE_INTEGER newposition;
		if (istrm->Seek(move, STREAM_SEEK_CUR, &newposition) == S_OK)
		{
			return newposition.QuadPart;
		}
	}
	return MZ_TELL_ERROR;
}

int32_t tvp_mz_stream_seek(void *stream, int64_t offset, int32_t origin)
{
	tvp_mz_stream_t *tvpstream = (tvp_mz_stream_t*)stream;
	IStream *istrm = tvpstream->file;

	DWORD dwOrigin;
	switch (origin)
	{
		case MZ_SEEK_CUR: dwOrigin = STREAM_SEEK_CUR; break;
		case MZ_SEEK_END: dwOrigin = STREAM_SEEK_END; break;
		case MZ_SEEK_SET: dwOrigin = STREAM_SEEK_SET; break;
		default: return MZ_SEEK_ERROR;
	}
	if (istrm)
	{
		LARGE_INTEGER move;
		move.QuadPart = offset;
		ULARGE_INTEGER newposition;
		if (istrm->Seek(move, dwOrigin, &newposition) == S_OK)
		{
			return MZ_OK;
		}
	}
	return MZ_SEEK_ERROR;
}

int32_t tvp_mz_stream_close(void *stream)
{
	tvp_mz_stream_t *tvpstream = (tvp_mz_stream_t*)stream;
	IStream *istrm = tvpstream->file;
	if (istrm)
	{
		istrm->Release();
		istrm = NULL;
		tvpstream->file = NULL;
		tvpstream->initalized = false;
		return MZ_OK;
	}
	return MZ_CLOSE_ERROR;
}

int32_t tvp_mz_stream_error(void *stream)
{
	tvp_mz_stream_t *tvpstream = (tvp_mz_stream_t*)stream;
	IStream *istrm = tvpstream->file;
	if (istrm)
	{
		return MZ_OK;
	}
	return MZ_STREAM_ERROR;
}

void* tvp_mz_stream_create(void **stream)
{
	tvp_mz_stream_t *tvpstream = new tvp_mz_stream_t();
	if (stream != NULL)
	{
		*stream = tvpstream;
	}
	return tvpstream;
}

void tvp_mz_stream_destroy(void **stream)
{
	if (stream == NULL)
	{
		return;
	}
	tvp_mz_stream_t *tvpstream = (tvp_mz_stream_t*)*stream;
	delete tvpstream;
	*stream = NULL;
}

mz_stream_vtbl tvp_mz_stream_vtbl =
{
	tvp_mz_stream_open,
	tvp_mz_stream_is_open,
	tvp_mz_stream_read,
	tvp_mz_stream_write,
	tvp_mz_stream_tell,
	tvp_mz_stream_seek,
	tvp_mz_stream_close,
	tvp_mz_stream_error,
	tvp_mz_stream_create,
	tvp_mz_stream_destroy,
	NULL,
	NULL,
};

zlib_filefunc64_def tvp_zlib_filefunc_def =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	(void *)&tvp_mz_stream_vtbl,
};

class ZIPStream : public IStream {

public:
	ZIPStream()
	{
		ref_count = 1;
		fs = NULL;
		size = 0;
	}

	bool initialize(const ttstr &filename, unz64_file_pos *file_pos)
	{
		if ((fs = unzOpen2_64((const void*)filename.c_str(), &tvp_zlib_filefunc_def)) != nullptr)
		{
			if (unzGoToFilePos64(fs, file_pos) != UNZ_OK)
			{
				unzClose(fs);
				fs = nullptr;
				return false;
			}
			unz_file_info file_info;
			if (unzGetCurrentFileInfo(fs, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
			{
				unzClose(fs);
				fs = nullptr;
				return false;
			}
			this->size = file_info.uncompressed_size;
			if (unzOpenCurrentFile(fs) != UNZ_OK)
			{
				unzClose(fs);
				fs = nullptr;
				return false;
			}
			return true;
		}
		return false;
	}

	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		if (riid == IID_IUnknown || riid == IID_ISequentialStream || riid == IID_IStream)
		{
			if (ppvObject == NULL)
				return E_POINTER;
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		else
		{
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
	}

	ULONG STDMETHODCALLTYPE AddRef(void)
	{
		ref_count++;
		return ref_count;
	}
	
	ULONG STDMETHODCALLTYPE Release(void)
	{
		int ret = --ref_count;
		if (ret <= 0) {
			delete this;
			ret = 0;
		}
		return ret;
	}

	// ISequentialStream
	HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead)
	{
		DWORD size = unzReadCurrentFile(fs, pv, cb);
		if (pcbRead)
		{
			*pcbRead = size;
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten)
	{
		return E_NOTIMPL;
	}

	// IStream
	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,	DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
	{
		ZPOS64_T pos;
		switch (dwOrigin) {
		case STREAM_SEEK_CUR:
			pos = unzTell64(fs);
			pos += dlibMove.QuadPart;
			break;
		case STREAM_SEEK_SET:
			pos = dlibMove.QuadPart;
			break;
		case STREAM_SEEK_END:
			pos = this->size;
			pos += dlibMove.QuadPart;
			break;
		}
		HRESULT ret = unzSetOffset64(fs, pos) == UNZ_OK ? S_OK : S_FALSE;
		if (plibNewPosition)
		{
			plibNewPosition->QuadPart = pos;
		}
		return ret;
	}
	
	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize)
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Revert(void)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		if (pstatstg)
		{
			ZeroMemory(pstatstg, sizeof(*pstatstg));

#if 0
			// pwcsName
			// this object's storage pointer does not have a name ...
			if(!(grfStatFlag &  STATFLAG_NONAME))
			{
				// anyway returns an empty string
				LPWSTR str = (LPWSTR)CoTaskMemAlloc(sizeof(*str));
				if(str == NULL) return E_OUTOFMEMORY;
				*str = L'\0';
				pstatstg->pwcsName = str;
			}
#endif

			// type
			pstatstg->type = STGTY_STREAM;

			// cbSize
			pstatstg->cbSize.QuadPart = size;
			
			// mtime, ctime, atime unknown

			// grfMode unknown
			pstatstg->grfMode = STGM_DIRECT | STGM_READ | STGM_SHARE_DENY_WRITE ;
			
			// grfLockSuppoted
			pstatstg->grfLocksSupported = 0;
			
			// grfStatBits unknown
		}
		else
		{
			return E_INVALIDARG;
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm)
	{
		return E_NOTIMPL;
	}

protected:

	virtual ~ZIPStream()
	{
		unzClose(fs);
	}

private:
	int ref_count;
	unzFile fs;
	uint64_t size;
};

static std::vector<iTVPStorageMedia*> storage_media_vector;

class ZIPStorage : public iTVPStorageMedia
{

public:
	ZIPStorage()
	{
		ref_count = 1;
		char buf[(sizeof(void *) * 2) + 1];
		snprintf(buf, (sizeof(void *) * 2) + 1, "%p", this);
		// The hash function does not work properly with numbers, so change to letters.
		char *p = buf;
		while (*p)
		{
			if(*p >= '0' && *p <= '9')
				*p = 'g' + (*p - '0');
			if(*p >= 'A' && *p <= 'Z')
				*p |= 32;
			p++;
		}
		name = ttstr(TJS_W("zip")) + buf;
	}

	virtual ~ZIPStorage()
	{
	}

	bool initialize(const ttstr &in_filename)
	{
		unzFile fs = unzOpen2_64((const void*)in_filename.c_str(), &tvp_zlib_filefunc_def);
		if (fs == NULL)
		{
			return false;
		}
		filename = in_filename;
		unzGoToFirstFile(fs);
		unz_file_info file_info;
		unz64_file_pos file_pos;
		do
		{
			char nfname[1024];
			if (unzGetCurrentFileInfo(fs, &file_info, nfname, sizeof(nfname), nullptr, 0, nullptr, 0) == UNZ_OK && file_info.compression_method == 0 && unzGetFilePos64(fs, &file_pos) == UNZ_OK)
			{
				tjs_char wfname[1024];
				tjs_int count = TVPUtf8ToWideCharString( nfname, wfname );
				wfname[count] = TJS_W('\0');
				ttstr file = wfname;
				tjs_char *p = file.Independ();
				while (*p)
				{
					// make all characters small
					if (*p >= TJS_W('A') && *p <= TJS_W('Z'))
					{
						*p += TJS_W('a') - TJS_W('A');
					}
					p++;
				}
				file_name_to_offset[file] = file_pos;
				ttstr dname = TJS_W("./");
				ttstr fname;
				const tjs_char *p2 = file.Independ();
				const tjs_char *q;
				if ((q = wcsrchr(p2, '/')))
				{
					dname += ttstr(p2, q - p2 + 1);
					fname = ttstr(q + 1);
				}
				else
				{
					fname = name;
				}
				dir_entry_table[dname].push_back(fname);
			}
		}
		while (unzGoToNextFile(fs) == UNZ_OK);
		unzClose(fs);
		return true;
	}

public:
	// -----------------------------------
	// iTVPStorageMedia Intefaces
	// -----------------------------------

	virtual void TJS_INTF_METHOD AddRef()
	{
		ref_count++;
	};

	virtual void TJS_INTF_METHOD Release()
	{
		if (ref_count == 1)
		{
			delete this;
		}
		else
		{
			ref_count--;
		}
	};

	// returns media name like "file", "http" etc.
	virtual void TJS_INTF_METHOD GetName(ttstr &out_name)
	{
		out_name = name;
	}

	//	virtual ttstr TJS_INTF_METHOD IsCaseSensitive() = 0;
	// returns whether this media is case sensitive or not

	// normalize domain name according with the media's rule
	virtual void TJS_INTF_METHOD NormalizeDomainName(ttstr &name)
	{
		// normalize domain name
		// make all characters small
		tjs_char *p = name.Independ();
		while(*p)
		{
			if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
				*p += TJS_W('a') - TJS_W('A');
			p++;
		}
	}

	// normalize path name according with the media's rule
	virtual void TJS_INTF_METHOD NormalizePathName(ttstr &name)
	{
		// normalize path name
		// make all characters small
		tjs_char *p = name.Independ();
		while(*p)
		{
			if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
				*p += TJS_W('a') - TJS_W('A');
			p++;
		}
	}

	// check file existence
	virtual bool TJS_INTF_METHOD CheckExistentStorage(const ttstr &name)
	{
		const tjs_char *ptr = name.c_str();

		// The domain name needs to be "."
		if (!TJS_strncmp(ptr, TJS_W("./"), 2))
		{
			ttstr fpname = ptr + 2;
			file_name_to_offset_t::const_iterator fit = file_name_to_offset.find(fpname);
			if (fit != file_name_to_offset.end())
			{
				return true;
			}
		}
		return false;
	}

	// open a storage and return a tTJSBinaryStream instance.
	// name does not contain in-archive storage name but
	// is normalized.
	virtual tTJSBinaryStream * TJS_INTF_METHOD Open(const ttstr & name, tjs_uint32 flags)
	{
		if (flags == TJS_BS_READ)
		{
			const tjs_char *ptr = name.c_str();

			// The domain name needs to be "."
			if (!TJS_strncmp(ptr, TJS_W("./"), 2))
			{
				ptr += 2;
				ttstr fpname = ptr + 2;
				file_name_to_offset_t::const_iterator fit = file_name_to_offset.find(fpname);
				if (fit != file_name_to_offset.end())
				{
					unz64_file_pos offset = (*fit).second;
					ZIPStream *stream = new ZIPStream();
					if (stream->initialize(filename, &offset))
					{
						tTJSBinaryStream *ret = TVPCreateBinaryStreamAdapter(stream);
						stream->Release();
						return ret;
					}
					else
					{
						stream->Release();
					}
				}
			}
		}
		return NULL;
	}

	// list files at given place
	virtual void TJS_INTF_METHOD GetListAt(const ttstr &name, iTVPStorageLister * lister)
	{
		dir_entry_table_t::const_iterator dit = dir_entry_table.find(name);
		if (dit != dir_entry_table.end())
		{
			file_name_list_t::const_iterator fit = dit->second.begin();
			while (fit != dit->second.end())
			{
				lister->Add(*fit);
				fit += 1;
			}
		}
	}

	// basically the same as above,
	// check wether given name is easily accessible from local OS filesystem.
	// if true, returns local OS native name. otherwise returns an empty string.
	virtual void TJS_INTF_METHOD GetLocallyAccessibleName(ttstr &name)
	{
		name = "";
	}

private:
	tjs_uint ref_count;
	ttstr name;
	ttstr filename;
	typedef std::vector<ttstr> file_name_list_t;
	typedef std::map<ttstr, unz64_file_pos> file_name_to_offset_t;
	typedef std::map<ttstr, file_name_list_t> dir_entry_table_t;
	dir_entry_table_t dir_entry_table;
	file_name_to_offset_t file_name_to_offset;
};

class StoragesZIP {

public:
	static ttstr mountZIP(ttstr filename)
	{	
		ZIPStorage * zipstorage = new ZIPStorage();
		if (zipstorage->initialize(filename))
		{
			TVPRegisterStorageMedia(zipstorage);
			storage_media_vector.push_back(zipstorage);
			ttstr zipstorage_name;
			zipstorage->GetName(zipstorage_name);
			return zipstorage_name;
		}
		else
		{
			zipstorage->Release();
		}
		return TJS_W("");
	}

	static bool unmountZIP(ttstr medianame)
	{
		for (auto i = storage_media_vector.begin();
			i != storage_media_vector.end(); i += 1)
		{
			ttstr this_medianame;
			(*i)->GetName(this_medianame);
			if (medianame == this_medianame)
			{
				TVPUnregisterStorageMedia(*i);
				(*i)->Release();
				storage_media_vector.erase(i);
				return true;
			}
		}

		return false;
	}
};

NCB_ATTACH_CLASS(StoragesZIP, Storages) {
	NCB_METHOD(mountZIP);
	NCB_METHOD(unmountZIP);
};

static void PreRegistCallback()
{
}

static void PostRegistCallback()
{
}

static void PostUnregistCallback()
{
	for (auto i = storage_media_vector.begin();
		i != storage_media_vector.end(); i += 1)
	{
		TVPUnregisterStorageMedia(*i);
	}
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_REGIST_CALLBACK(PostRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
