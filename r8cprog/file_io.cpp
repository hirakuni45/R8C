//=====================================================================//
/*!	@file
	@brief	ファイル入出力関連、ユーティリティー@n
			文字列のコード変換など
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "file_io.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#ifdef __PPU__
#include <sys/paths.h>
#include <cell/sysmodule.h>
#include <cell/l10n.h>
#endif

#include <iostream>
#include <boost/format.hpp>

namespace utils {

	using namespace std;

	//-----------------------------------------------------------------//
	/*!
		@brief	UTF-32 対応のファイルオープン
		@param[in]	fn	ファイル名
		@param[in]	md	オープンモード
		@return オープンできれば、ファイル構造体のポインターを返す
	*/
	//-----------------------------------------------------------------//
	std::FILE* wfopen(const lstring& fn, const std::string& md)
	{
		std::FILE* fp = 0;
#ifdef WIN32
		wchar_t* wsm = new wchar_t[md.size() + 1];
		for(uint32_t i = 0; i < md.size(); ++i) {
			wsm[i] = md[i];
		}
		wsm[md.size()] = 0;
		wchar_t* wfn = new wchar_t[fn.size() + 1];
		for(uint32_t i = 0; i < fn.size(); ++i) {
			wfn[i] = fn[i];
		}
		wfn[fn.size()] = 0;
		fp = _wfopen(wfn, wsm);
		delete[] wfn;
		delete[] wsm;
#else
		std::string s;
		utils::utf32_to_utf8(fn, s);
		fp = fopen(s.c_str(), md.c_str());
#endif
#ifndef NDEBUG
		if(fp == 0) {
			std::string s;
			utils::utf32_to_utf8(fn, s);
			std::string tt;
			if(strchr(md.c_str(), 'w')) tt = "output";
			else tt = "input";
			std::cerr << boost::format("Can't open %1% file (file_io::wfopen): '%2%'") % tt % s << std::endl;
		}
#endif
		return fp;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ディレクトリーか調べる
		@param[in]	fn	ファイル名
		@return ディレクトリーなら「true」
	*/
	//-----------------------------------------------------------------//
	bool is_directory(const std::string& fn)
	{
#ifdef WIN32
		struct _stat st;
		wchar_t* wfn = new wchar_t[fn.size() + 1];
		for(uint32_t i = 0; i < fn.size(); ++i) {
			wfn[i] = fn[i];
		}
		wfn[fn.size()] = 0;
		int ret = _wstat(wfn, &st);
		delete[] wfn;
		if(ret == 0) {
			return S_ISDIR(st.st_mode) != 0;
		}
#else
		struct stat st;
		if(stat(fn.c_str(), &st) == 0) {
			return S_ISDIR(st.st_mode) != 0;
		}
#endif
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルの検査（UTF32)
		@param[in]	fn	ファイル名
		@param[in]	dir	「true」ならディレクトリーとして検査
		@return ファイルが有効なら「true」
	*/
	//-----------------------------------------------------------------//
	bool probe_file(const lstring& fn, bool dir)
	{
#ifdef WIN32
		struct _stat st;
		wchar_t* wfn = new wchar_t[fn.size() + 1];
		for(uint32_t i = 0; i < fn.size(); ++i) {
			wfn[i] = fn[i];
		}
		wfn[fn.size()] = 0;
		int ret = _wstat(wfn, &st);
		delete[] wfn;
		if(ret == 0) {
#else
		std::string s;
		utf32_to_utf8(fn, s);
		struct stat st;
		if(stat(s.c_str(), &st) == 0) {
#endif
			if(dir) {
				bool d = S_ISDIR(st.st_mode);
				if(d) return true;
			} else {
				return true;
			}
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルのサイズを返す
		@param[in]	fn	ファイル名
		@return ファイルサイズ
	*/
	//-----------------------------------------------------------------//
	size_t get_file_size(const std::string& fn)
	{
		size_t fsz = 0;

		struct stat st;
		if(stat(fn.c_str(), &st) == 0) {
			fsz = st.st_size;
		}
		return fsz;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	１バイト読み出し
		@param[out]	ch	読み込み先
		@return	ファイルの終端なら「false」
	*/
	//-----------------------------------------------------------------//
	bool file_io::get_char(char& ch)
	{
		if(fp_) {
			int cha = ::fgetc(fp_);
			if(cha != EOF) {
				ch = cha;
				return true;
			}
		} else if(open_) {
			if(rbuff_ != 0 && size_ > 0) {
				if(fpos_ < size_) {
					ch = rbuff_[fpos_];
					fpos_++;
					return true;
				} else {
					return false;
				}
			}
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	１バイト書き出し
		@param[out]	c	書き出しデータ
		@return	エラーなら「false」
	*/
	//-----------------------------------------------------------------//
	bool file_io::put_char(char c)
	{
		if(fp_) {
			fputc(c, fp_);
			return true;
		} else {
			if(!open_) return false;
			wbuff_.push_back(c);
			return true;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルサイズを得る
		@return	ファイルのサイズ
	*/
	//-----------------------------------------------------------------//
	size_t file_io::get_file_size()
	{
		size_t pos = tell();
		seek(0, seek::end);
		size_t size = tell();
		seek(pos, seek::set);
		return size;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	1 行読み込み
		@param[out]	buff	読み込み先
		@return	ファイルの終端なら「false」
	*/
	//-----------------------------------------------------------------//
	bool file_io::get_line(std::string& buff)
	{
		if(open_ == false) return false;

		char ch;
		while(get_char(ch) == true) {
			if(ch == 0x0d) ;
			else if(ch == 0x0a) {
				return true;
			} else {
				buff.append(1, ch);
			}
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	エンディアン並べ替え
		@param[in]	ptr	元データ
		@param[in]	size	構造体のサイズ
		@param[in]	list	構造体、個々のイニシャル
	*/
	//-----------------------------------------------------------------//
	void file_io::reorder_memory(void* ptr, size_t size, const char* list)
	{
		if(list == 0) return;

		unsigned char* p = static_cast<unsigned char*>(ptr);
		const char* l = list;
		size_t s = 0;
		while(s < size) {
			char c = *l++;
			switch(c) {
			case 0:
				l = list;
				break;
			case 2:
			case 's':
			case 'S':
				{
					unsigned char l = p[0];
					unsigned char h = p[1];
					*p++ = h;
					*p++ = l;
					s += 2;
				}
				break;
			case 4:
			case 'i':
			case 'I':
			case 'l':
			case 'L':
			case 'f':
			case 'F':
				{
					unsigned char t[4];
					memcpy(p, t, 4);
					for(int i = 0; i < 4; ++i) {
						*p++ = t[4 - 1 - i];
					}
					s += 4;
				}
				break;
			case 8:
			case 'd':
			case 'D':
				{
					unsigned char t[8];
					memcpy(p, t, 8);
					for(int i = 0; i < 8; ++i) {
						*p++ = t[8 - 1 - i];
					}
					s += 4;
				}
				break;
			default:
				++s;
				++p;
				break;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルをメモリー上に全て読み込む
		@param[in]	fin	ファイル入力コンテキスト
		@param[out]	array	アレイ構造
		@param[in]	len	読み込むバイト数（省略する「０」と全て）
		@return 成功すれば「true」
	*/
	//-----------------------------------------------------------------//
	bool read_array(file_io& fin, array_uc& array, size_t len)
	{
		size_t l;
		if(len == 0) l = fin.get_file_size() - fin.tell();
		else l = len;
		array.resize(l);
		if(fin.read(&array[0], l) != l) return false;
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	メモリー上のデータを全てファイルに書き込む
		@param[in]	fin	ファイル出力コンテキスト
		@param[in]	array	アレイ構造
		@param[in]	len	書き込むバイト数（省略する「０」と全て）
		@return 成功すれば「true」
	*/
	//-----------------------------------------------------------------//
	bool write_array(file_io& fout, const array_uc& array, size_t len)
	{
		size_t l;
		if(len > 0 && array.size() > len) l = len;
		else l = array.size();
		if(fout.write(&array[0], l) != l) return false;
		return true;
	}

}
