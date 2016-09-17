
// Copyright (c) 2010-2016 niXman (i dot nixman dog gmail dot com). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef _yas__detail__io__text_streams_hpp
#define _yas__detail__io__text_streams_hpp

#include <yas/detail/config/config.hpp>

#include <yas/detail/io/io_exceptions.hpp>
#include <yas/detail/io/serialization_exception.hpp>
#include <yas/detail/type_traits/type_traits.hpp>

#include <limits>

namespace yas {
namespace detail {

/***************************************************************************/

#if 0
#define YAS_READ_BY_CHAR(is, buf, cnt) \
	for ( char *cur = &buf[0], *end = &buf[sizeof(buf)-1]; cur != end; ++cur, ++cnt ) { \
		*cur = is.getch(); \
		if ( *cur == ' ' ) { \
			is.ungetch(*cur); \
			*cur = 0; \
			break; \
		} \
		if ( *cur == -1 ) { \
			break; \
		} \
	}
#endif

/***************************************************************************/

template<typename IS, typename Trait>
struct text_istream {
	text_istream(IS &is)
		:is(is)
	{}

	// for arrays
	void read(void *ptr, std::size_t size) {
		YAS_THROW_ON_READ_ERROR(size, !=, is.read(ptr, size));
	}

	// for chars only
	template<typename T>
	void read(T &v, YAS_ENABLE_IF_IS_ANY_OF(T, char, signed char, unsigned char)) {
		char buf[2];

		YAS_THROW_ON_READ_ERROR(sizeof(buf), !=, is.read(buf, sizeof(buf)));
		if ( buf[0] != ' ' ) YAS_THROW_SPACE_IS_EXPECTED();

		v = buf[1];
	}

	// for bools only
	template<typename T>
	void read(T &v, YAS_ENABLE_IF_IS_ANY_OF(T, bool)) {
		char buf[2];

		YAS_THROW_ON_READ_ERROR(sizeof(buf), !=, is.read(buf, sizeof(buf)));
		if ( buf[0] != ' ' ) YAS_THROW_SPACE_IS_EXPECTED();
		v = buf[1]-'0';
	}

	// for signed 16/32/64 bits
	template<typename T>
	void read(T &v, YAS_ENABLE_IF_IS_ANY_OF(T, std::int16_t, std::int32_t, std::int64_t, long long)) {
		char buf[sizeof(T)*4];

		YAS_THROW_ON_READ_ERROR(3, !=, is.read(buf, 3));
		if ( buf[0] != ' ' ) YAS_THROW_SPACE_IS_EXPECTED();

		std::size_t cnt = (buf[1]-'0')*10+(buf[2]-'0');
		buf[cnt] = 0;
		YAS_THROW_ON_READ_ERROR(cnt, !=, is.read(buf, cnt));

		Trait::atoi(v, buf, cnt);
	}

	// for unsigned 16/32/64 bits
	template<typename T>
	void read(T &v, YAS_ENABLE_IF_IS_ANY_OF(T, std::uint16_t, std::uint32_t, std::uint64_t, unsigned long long)) {
		char buf[sizeof(T)*4];

		YAS_THROW_ON_READ_ERROR(3, !=, is.read(buf, 3));
		if ( buf[0] != ' ' ) YAS_THROW_SPACE_IS_EXPECTED();

		std::size_t cnt = (buf[1]-'0')*10+(buf[2]-'0');
		buf[cnt] = 0;
		YAS_THROW_ON_READ_ERROR(cnt, !=, is.read(buf, cnt));

		Trait::atou(v, buf, cnt);
	}

	// for floats
	template<typename T>
	void read(T &v, YAS_ENABLE_IF_IS_ANY_OF(T, float)) {
		char buf[std::numeric_limits<T>::max_exponent10+20];

		YAS_THROW_ON_READ_ERROR(3, !=, is.read(buf, 3));
		if ( buf[0] != ' ' ) YAS_THROW_SPACE_IS_EXPECTED();

		std::size_t cnt = (buf[1]-'0')*10+(buf[2]-'0');
		buf[cnt] = 0;
		YAS_THROW_ON_READ_ERROR(cnt, !=, is.read(buf, cnt));

		Trait::atof(v, buf, cnt);
	}

	// for doubles
	template<typename T>
	void read(T &v, YAS_ENABLE_IF_IS_ANY_OF(T, double)) {
		char buf[std::numeric_limits<T>::max_exponent10+20];

		YAS_THROW_ON_READ_ERROR(3, !=, is.read(buf, 3));
		if ( buf[0] != ' ' ) YAS_THROW_SPACE_IS_EXPECTED();

		std::size_t cnt = (buf[1]-'0')*10+(buf[2]-'0');
		buf[cnt] = 0;
		YAS_THROW_ON_READ_ERROR(cnt, !=, is.read(buf, cnt));

		Trait::atod(v, buf, cnt);
	}

private:
	IS &is;
};

#undef YAS_READ_BY_CHAR

template<typename OS, typename Trait>
struct text_ostream {
	text_ostream(OS &os)
		:os(os)
	{}

	// for arrays
	void write(const void *ptr, std::size_t size) {
		YAS_THROW_ON_WRITE_ERROR(size, !=, os.write(ptr, size));
	}

	// for chars only
	template<typename T>
	void write(const T &v, YAS_ENABLE_IF_IS_ANY_OF(T, char, signed char, unsigned char)) {
		const char buf[2] = {
			 ' '
			,(char)v
		};

		YAS_THROW_ON_WRITE_ERROR(sizeof(buf), !=, os.write(&buf, sizeof(buf)));
	}

	// for bools only
	template<typename T>
	void write(const T &v, YAS_ENABLE_IF_IS_ANY_OF(T, bool)) {
		const char buf[2] = {
			 ' '
			,(char)('0'+v)
		};

		YAS_THROW_ON_WRITE_ERROR(sizeof(buf), !=, os.write(&buf, sizeof(buf)));
	}

	// for signed 16/32/64 bits
	template<typename T>
	void write(const T &v, YAS_ENABLE_IF_IS_ANY_OF(T, std::int16_t, std::int32_t, std::int64_t, long long)) {
		char buf[sizeof(v)*4];
		std::size_t len = 0;
		Trait::itoa(buf+3, sizeof(buf)-3, len, v);

		buf[0] = ' ';
		buf[1] = '0'+len/10;
		buf[2] = '0'+len%10;

		YAS_THROW_ON_WRITE_ERROR(len+3, !=, os.write(buf, len+3));
	}

	// for unsigned 16/32/64 bits
	template<typename T>
	void write(const T &v, YAS_ENABLE_IF_IS_ANY_OF(T, std::uint16_t, std::uint32_t, std::uint64_t, unsigned long long)) {
		char buf[sizeof(v)*4];
		std::size_t len = 0;
		Trait::utoa(buf+3, sizeof(buf), len, v);

		buf[0] = ' ';
		buf[1] = '0'+len/10;
		buf[2] = '0'+len%10;

		YAS_THROW_ON_WRITE_ERROR(len+3, !=, os.write(buf, len+3));
	}

	// for floats
	template<typename T>
	void write(const T &v, YAS_ENABLE_IF_IS_ANY_OF(T, float)) {
		char buf[std::numeric_limits<T>::max_exponent10 + 20];
		std::size_t len = 0;
		Trait::ftoa(buf+3, sizeof(buf), len, v);

		buf[0] = ' ';
		buf[1] = '0'+len/10;
		buf[2] = '0'+len%10;

		YAS_THROW_ON_WRITE_ERROR(len+3, !=, os.write(buf, len+3));
	}

	// for doubles
	template<typename T>
	void write(const T &v, YAS_ENABLE_IF_IS_ANY_OF(T, double)) {
		char buf[std::numeric_limits<T>::max_exponent10 + 20];
		std::size_t len = 0;
		Trait::dtoa(buf+3, sizeof(buf), len, v);

		buf[0] = ' ';
		buf[1] = '0'+len/10;
		buf[2] = '0'+len%10;

		YAS_THROW_ON_WRITE_ERROR(len+3, !=, os.write(buf, len+3));
	}

private:
	OS &os;
};

/***************************************************************************/

} // ns detail
} // ns yas

#endif // _yas__detail__io__text_streams_hpp
