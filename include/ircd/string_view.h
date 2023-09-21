#pragma once
#define HAVE_IRCD_STRING_VIEW_H

namespace ircd
{
	struct string_view;

	constexpr size_t _constexpr_strlen(const char *) noexcept;
	template<size_t N> consteval size_t _constexpr_strlen(const char (&)[N]) noexcept;

	constexpr bool _constexpr_equal(const char *a, const char *b) noexcept;
	constexpr bool _constexpr_equal(const char *a, const char *b, size_t) noexcept;
	constexpr bool _constexpr_equal(const char *a, const size_t, const char *b, const size_t) noexcept;
	constexpr bool _constexpr_equal(const string_view &, const string_view &) noexcept;
	template<size_t N0, size_t N1> consteval bool _constexpr_equal(const char (&)[N0], const char (&)[N1]) noexcept;

	constexpr const char *data(const string_view &) noexcept;
	constexpr size_t size(const string_view &) noexcept;

	bool empty(const string_view &);
	bool operator!(const string_view &);
	bool defined(const string_view &);
	bool null(const string_view &);

	int cmp(const string_view &, const string_view &) noexcept;

	constexpr string_view operator ""_sv(const char *const literal, const size_t size);
}

namespace std
{
	template<> struct std::hash<ircd::string_view>;
	template<> struct std::less<ircd::string_view>;
	template<> struct std::equal_to<ircd::string_view>;
}

/// Customized std::string_view (experimental TS / C++17)
///
/// This class adds iterator-based (char*, char*) construction to std::string_view which otherwise
/// takes traditional (char*, size_t) arguments. This allows boost::spirit grammars to create
/// string_view's using the raw[] directive achieving zero-copy/zero-allocation parsing.
///
struct ircd::string_view
:std::string_view
{
	// (non-standard)
	explicit operator bool() const
	{
		return !empty();
	}

	/// CAREFUL. THIS IS ON PURPOSE. By relaxing this conversion we're reducing
	/// the amount of explicit std::string() pollution when calling out to code
	/// which doesn't support string_view *yet* (keyword: yet). When it does
	/// support string_view then this conversion won't happen, and we don't
	/// have to change anything in our code. The price here is that an
	/// occasional regression analysis on where these conversions are occurring
	/// should be periodically performed to make sure there are no unwanted
	/// silent accidents.
	operator std::string() const
	{
		return std::string(cbegin(), cend());
	}

	/// (non-standard) When data() != nullptr we consider the string defined
	/// downstream in this project wrt JS/JSON. This is the bit of information
	/// we're deciding on for defined|undefined. If this string_view is
	/// constructed from a literal "" we must assert that inputs a valid pointer
	/// in the std::string_view with length 0; stdlib can't optimize that with
	/// a nullptr replacement.
	bool undefined() const
	{
		return data() == nullptr;
	}

	bool defined() const
	{
		return !undefined();
	}

	/// (non-standard) string_view's have no guarantee to be null terminated
	/// and most likely aren't. The std::string_view does not offer the
	/// c_str() function because using it is overwhelmingly likely to be wrong.
	/// Nevertheless if our developer is certain their view is of a null
	/// terminated string where the terminator is one past the end they can
	/// invoke this function rather than data() to assert their intent. Note
	/// that this assertion is still not foolproof because reading beyond
	/// size() might still be incorrect whether or not a null is found there
	/// and there is nothing else we can do. The developer must be sure.
	auto c_str() const
	{
		assert(!data() || data()[size()] == '\0');
		return data();
	}

	/// (non-standard) After using data() == nullptr for undefined, we're fresh
	/// out of legitimate bits here to represent the null type string. In this
	/// case we expect a hack pointer of 0x1 which will mean JS null
	bool null() const
	{
		return data() == reinterpret_cast<const char *>(0x1);
	}

	// (non-standard) Non-throwing substr() which returns empty rather than std::out_of_range.
	// This avoids generating numerous eh handlers/terminations/unwind blocks etc.
	using std::string_view::substr;
	constexpr auto substr(std::nothrow_t, size_t pos = 0, size_t count = npos) const noexcept
	{
		pos = std::min(pos, size());
		count = std::min(count, size() - pos);
		return std::string_view::substr(pos, count);
	}

	// (non-standard) our faux insert stub
	// Tricks boost::spirit into thinking this is mutable string (hint: it's not).
	// Instead, the raw[] directive in Qi grammar will use the iterator constructor only.
	// __attribute__((error("string_view is not insertable (hint: use raw[] directive)")))
	#if !defined(NDEBUG) && !defined(RB_ASSERT)
	[[noreturn]]
	#endif
	void insert(const iterator &, const char &)
	{
		assert(0);
	}

	// (non-standard) our iterator-based assign
	string_view &assign(const char *const &__restrict__ begin, const char *const &__restrict__ end)
	{
		this->~string_view();
		new (this) string_view{begin, end};
		return *this;
	}

	// (non-standard) intuitive wrapper for remove_suffix.
	// Unlike std::string, we can cheaply involve a reference to the removed character
	// which still exists.
	const char &pop_back()
	{
		const char &ret(back());
		remove_suffix(1);
		return ret;
	}

	// (non-standard) intuitive wrapper for remove_prefix.
	// Unlike std::string, we can cheaply involve a reference to the removed character
	// which still exists.
	const char &pop_front()
	{
		const char &ret(front());
		remove_prefix(1);
		return ret;
	}

	/// (non-standard) resize viewer
	void resize(const size_t &count)
	{
		*this = string_view{data(), data() + count};
	}

	// (non-standard) our iterator-based constructor
	constexpr string_view(const char *const &__restrict__ begin, const char *const &__restrict__ end) noexcept
	:string_view{begin, size_t(end - begin)}
	{
		assert(begin <= end);
	}

	// (non-standard) our iterator-based constructor
	string_view(const std::string::const_iterator &begin, const std::string::const_iterator &end)
	:string_view{&*begin, &*end}
	{}

	// (non-standard) our array based constructor
	template<size_t SIZE> constexpr
	__attribute__((always_inline))
	string_view(const std::array<char, SIZE> &array) noexcept
	:string_view
	{
		array.data(), std::find(array.begin(), array.end(), '\0')
	}{}

	// (non-standard) our buffer based constructor
	template<size_t SIZE> constexpr
	__attribute__((always_inline))
	string_view(const char (&__restrict__ buf)[SIZE]) noexcept
	:string_view
	{
		buf, std::find(buf, buf + SIZE, '\0')
	}{}

	constexpr string_view(const char *const &__restrict__ start, const size_t &size) noexcept
	:std::string_view{start, size}
	{}

	constexpr string_view(const char *const &__restrict__ start) noexcept
	:std::string_view{start, _constexpr_strlen(start)}
	{}

	string_view(std::string &&) noexcept = delete;
	string_view(const std::string &string) noexcept
	:string_view{string.data(), string.size()}
	{}

	constexpr string_view(const std::string_view &sv)
	:std::string_view{sv}
	{}

	/// Our default constructor sets the elements to 0 for best behavior by
	/// defined() and null() et al.
	constexpr string_view()
	:std::string_view{}
	{}
};

/// Specialization for std::hash<> participation
template<>
struct std::hash<ircd::string_view>
:std::hash<std::string_view>
{
	using std::hash<std::string_view>::operator();
	using std::hash<std::string_view>::hash;
};

/// Specialization for std::less<> participation
template<>
struct std::less<ircd::string_view>
:std::less<std::string_view>
{
	using std::less<std::string_view>::operator();
	using std::less<std::string_view>::less;
};

/// Specialization for std::equal_to<> participation
template<>
struct std::equal_to<ircd::string_view>
:std::equal_to<std::string_view>
{
	using std::equal_to<std::string_view>::operator();
	using std::equal_to<std::string_view>::equal_to;
};

/// Compile-time conversion from a string literal into a string_view.
constexpr ircd::string_view
ircd::operator ""_sv(const char *const literal, const size_t size)
{
	return string_view{literal, size};
}

inline int
ircd::cmp(const string_view &a,
          const string_view &b)
noexcept
{
	const auto res
	{
		#if __has_builtin(__builtin_memcmp_inline) && !defined(RB_GENERIC)
			__builtin_memcmp_inline(a.data(), b.data(), std::min(a.size(), b.size()))
		#else
			__builtin_memcmp(a.data(), b.data(), std::min(a.size(), b.size()))
		#endif
	};

	const auto zf
	{
		boolmask<uint>(res == 0)
	};

	const auto lt
	{
		boolmask<uint>(a.size() < b.size())
	};

	const auto gt
	{
		boolmask<uint>(a.size() > b.size())
	};

	return (~zf & res) | (zf & lt & -1U) | (zf & gt & 1U);
}

inline bool
ircd::operator!(const string_view &str)
{
	return empty(str);
}

inline bool
ircd::empty(const string_view &str)
{
	return str.empty();
}

inline bool
ircd::null(const string_view &str)
{
	return str.null();
}

inline bool
ircd::defined(const string_view &str)
{
	return str.defined();
}

constexpr size_t
ircd::size(const string_view &str)
noexcept
{
	return str.size();
}

constexpr const char *
ircd::data(const string_view &str)
noexcept
{
	return str.data();
}

template<size_t N0,
         size_t N1>
consteval bool
ircd::_constexpr_equal(const char (&a)[N0],
                       const char (&b)[N1])
noexcept
{
	if constexpr(_constexpr_strlen(a) != _constexpr_strlen(b))
		return false;

	for(size_t i(0); i < _constexpr_strlen(a); ++i)
		if constexpr(a[i] != b[i])
			return false;

	return true;
}

constexpr bool
ircd::_constexpr_equal(const string_view &a,
                       const string_view &b)
noexcept
{
	if constexpr(__cplusplus >= 202002L)
		return a == b;
	else
		return _constexpr_equal(data(a), size(a), data(b), size(b));
}

constexpr bool
ircd::_constexpr_equal(const char *const a,
                       const size_t a_len,
                       const char *const b,
                       const size_t b_len)
noexcept
{
	return a_len == b_len && _constexpr_equal(a, b, a_len);
}

constexpr bool
ircd::_constexpr_equal(const char *const a,
                       const char *const b,
                       size_t len)
noexcept
{
	return !len || (*a == *b && (*a == '\0' || _constexpr_equal(a + 1, b + 1, len - 1)));
}

constexpr bool
ircd::_constexpr_equal(const char *const a,
                       const char *const b)
noexcept
{
	return *a == *b && (*a == '\0' || _constexpr_equal(a + 1, b + 1));
}

template<size_t N>
consteval size_t
ircd::_constexpr_strlen(const char (&a)[N])
noexcept
{
	size_t i(0);
	while(i < N)
		if constexpr(a[i] == '\0')
			return i;

	return N;
}

constexpr size_t
ircd::_constexpr_strlen(const char *const s)
noexcept
{
	const char *e(s);
	if(e) for(; *e; ++e);
	return e - s;
}