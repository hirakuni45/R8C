#pragma once
//=====================================================================//
/*!	@file
	@brief	RS232C 操作クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <string>
#include <iostream>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	RS232C クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class rs232c {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	パリティの設定
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class parity {
			none,
			even,
			odd
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	キャラクター長
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class char_len {
			bits7,
			bits8
		};


	private:
		int    fd_;

		termios    attr_back_;

		void close_() {
			tcsetattr(fd_, TCSANOW, &attr_back_);
			::close(fd_);
			fd_ = -1;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		rs232c() : fd_(-1) { }


		//-----------------------------------------------------------------//
		/*!
			@brief オープン
			@param[in]	path	シリアルポートパス
			@param[in]	brate	接続ボーレート
			@param[in]	clen	キャラクター長
			@param[in]	par		パリティ・ビット
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& path, speed_t brate, char_len clen = char_len::bits8, parity par = parity::none) {
			fd_ = ::open(path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
			if(fd_ < 0) {
				return false;
			}

			if(tcgetattr(fd_, &attr_back_) == -1) {
				::close(fd_);
				fd_ = -1;
			}

			termios attr;
			memset(&attr, 0, sizeof(attr));

			int cpar = 0;
			int ipar = IGNPAR;
			switch(par) {
			default:
			case parity::none:
				cpar = 0;
				ipar = IGNPAR;
				break;
			case parity::even:
				cpar = PARENB;
				ipar = INPCK;
				break;
			case parity::odd:
				cpar = (PARENB | PARODD);
				ipar = INPCK;
				break;
			}

			int bstop = 0;
#if 0
			switch(mode[2]) {
			case '1': bstop = 0;
				break;
			case '2': bstop = CSTOPB;
				break;
			}
#endif

			int cbits;
			switch(clen) {
			default:
			case char_len::bits8:
				cbits = CS8;
				break;
			case char_len::bits7:
				cbits = CS7;
				break;
			}

			attr.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
			attr.c_iflag = ipar;
			attr.c_oflag = 0;
			attr.c_lflag = 0;
			attr.c_cc[VMIN]  = 1;     // block untill n bytes are received
			attr.c_cc[VTIME] = 0;     // block untill a timer expires (n * 100 mSec.)

			if(cfsetispeed(&attr, brate) == -1) {
				close_();
				return false;
			}
			if(cfsetospeed(&attr, brate) == -1) {
				close_();
				return false;
			}

			if(tcsetattr(fd_, TCSANOW, &attr) == -1) {
				close_();
				return false;
			}

			int status;
			if(ioctl(fd_, TIOCMGET, &status) == -1) {
				close_();
				return false;
			}

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	クローズ
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool close() {
			if(fd_ < 0) return false;

			int status;
			if(ioctl(fd_, TIOCMGET, &status) == -1) {
				close_();
				return false;
			}

			status &= ~TIOCM_DTR;    /* turn off DTR */
			status &= ~TIOCM_RTS;    /* turn off RTS */

			if(ioctl(fd_, TIOCMSET, &status) == -1) {
				close_();
				return false;
			}

			close_();

			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	送信同期
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool sync_send() const {
			if(fd_ < 0) return false;

			tcdrain(fd_);
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	受信
			@param[out]	dst	受信データ転送先
			@param[in]	len	受信最大長さ
			@return 受信した長さ
		*/
		//-----------------------------------------------------------------//
		size_t recv(char* dst, size_t len) {
			if(fd_ < 0) return 0;

			return ::read(fd_, dst, len);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	受信
			@param[out]	dst	受信データ転送先
			@param[in]	len	受信最大長さ
			@param[in]	tv	タイムアウト指定
			@return 受信した長さ
		*/
		//-----------------------------------------------------------------//
		size_t recv(char* dst, size_t len, const timeval& tv) {
			if(fd_ < 0) return 0;

			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(fd_, &fds);

			timeval t;
			t.tv_sec  = tv.tv_sec;
			t.tv_usec = tv.tv_usec;
			int ret = select(fd_ + 1, &fds, NULL, NULL, &t);
			if(ret == -1) {
				// error..
				return 0;
			} else if(ret > 0) {
				return ::read(fd_, dst, len);
			}

			return 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	送信
			@param[in]	src	送信データ転送元
			@param[in]	len	送信長さ
			@return 送信した長さ
		*/
		//-----------------------------------------------------------------//
		size_t send(const char* src, size_t len) {
			if(fd_ < 0) return 0;

			return ::write(fd_, src, len);
		}


		bool get_DCD() const {
			if(fd_ < 0) return false;

			int status;
			if(ioctl(fd_, TIOCMGET, &status) == -1) {
				return false;
			}

			return status & TIOCM_CAR;
		}


		bool get_CTS() const {
			if(fd_ < 0) return false;

			int status;
			if(ioctl(fd_, TIOCMGET, &status) == -1) {
				return false;
			}

			return status & TIOCM_CTS;
		}


		bool get_DSR() const {
			if(fd_ < 0) return false;

			int status;

			if(ioctl(fd_, TIOCMGET, &status) == -1) {
				return false;
			}

			return status & TIOCM_DSR;
		}


		bool enable_DTR(bool ena = true) {
			if(fd_ < 0) return false;

			int status;
			if(ioctl(fd_, TIOCMGET, &status) == -1) {
				return false;
			}

			if(ena) status |= TIOCM_DTR;
			else status &= ~TIOCM_DTR;

			if(ioctl(fd_, TIOCMSET, &status) == -1) {
				return false;
			}

			return true;
		}


		bool enable_RTS(bool ena = true) {
			if(fd_ < 0) return false;

			int status;
			if(ioctl(fd_, TIOCMGET, &status) == -1) {
				return false;
			}

			if(ena) status |= TIOCM_RTS;
			else status &= ~TIOCM_RTS;

			if(ioctl(fd_, TIOCMSET, &status) == -1) {
				return false;
			}

			return true;
		}

	};
}
