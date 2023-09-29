#pragma once
#define HAVE_IRCD_FMT_H

/// Typesafe format strings from formal grammars & standard RTTI
namespace ircd::fmt
{
	IRCD_EXCEPTION(ircd::error, error);
	IRCD_PANICKING(error, invalid_format);
	IRCD_PANICKING(error, invalid_type);
	IRCD_PANICKING(error, illegal);

	struct sprintf;
	struct vsprintf;
	struct snprintf;
	struct vsnprintf;
	struct snstringf;
	struct vsnstringf;
	struct literal_snstringf;
	struct literal_snstringf__max;
	template<size_t MAX> struct bsprintf;
	using arg = std::tuple<const void *, const std::type_index &>;

	literal_snstringf operator ""_snstringf(const char *text, const size_t len);
	literal_snstringf__max operator,(const literal_snstringf, const ulong max);
	template<class... args> std::string operator,(const literal_snstringf__max, args&&...);
}

namespace ircd
{
	using fmt::operator ""_snstringf;
}

/// Typesafe snprintf() from formal grammar and RTTI.
///
/// This function accepts a format string and a variable number of arguments
/// composing formatted null-terminated output in the provided output buffer.
/// The format string is compliant with standard snprintf() (TODO: not yet).
/// The type information of the arguments is grabbed from the variadic template
/// and references are passed to the formal output grammars. This means you can
/// pass an std::string directly without calling c_str(), as well as pass a
/// non-null-terminated string_view safely.
///
class ircd::fmt::snprintf
{
	window_buffer out;                           // Window on the output buffer.
	const_buffer fmt;                            // Current running position in the fmtstr.
	short idx;                                   // Keeps count of the args for better err msgs

	bool finished() const noexcept;
	size_t remaining() const noexcept;
	size_t consumed() const                      { return out.consumed();                          }
	string_view completed() const                { return out.completed();                         }

	void append(const string_view &);
	void argument(const arg &);

  protected:
	IRCD_OVERLOAD(internal)
	snprintf(internal_t, const mutable_buffer &, const string_view &, const va_rtti &);

  public:
	operator ssize_t() const                     { return consumed();                              }
	operator string_view() const                 { return completed();                             }

	template<class... Args>
	snprintf(char *const &buf,
	         const size_t &max,
	         const string_view &fmt,
	         Args&&... args)
	:snprintf
	{
		internal, mutable_buffer{buf, max}, fmt, va_rtti{std::forward<Args>(args)...}
	}{}
};

/// Same as snprintf() where n= is deduced by the buffer size argument
///
struct ircd::fmt::sprintf
:snprintf
{
	template<class... Args>
	sprintf(const mutable_buffer &buf,
	        const string_view &fmt,
	        Args&&... args)
	:snprintf
	{
		internal, buf, fmt, va_rtti{std::forward<Args>(args)...}
	}{}
};

/// A complement to fmt::snprintf() accepting an already-made va_rtti.
///
/// This function has no variadic template; instead it accepts the type
/// which would be composed by such a variadic template called
/// ircd::va_rtti directly.
///
/// ircd::va_rtti is a lightweight pairing of argument pointers to runtime
/// type indexes. ircd::va_rtti is not a template itself because its purpose
/// is to bring this type information out of the header files to where the
/// grammar is instantiated.
///
struct ircd::fmt::vsnprintf
:snprintf
{
	vsnprintf(char *const &buf,
	          const size_t &max,
	          const string_view &fmt,
	          const va_rtti &ap)
	:snprintf
	{
		internal, mutable_buffer{buf, max}, fmt, ap
	}{}
};

/// Same as vsnprintf() where n is deduced from the buffer size.
///
struct ircd::fmt::vsprintf
:snprintf
{
	vsprintf(const mutable_buffer &buf,
	         const string_view &fmt,
	         const va_rtti &ap)
	:snprintf
	{
		internal, buf, fmt, ap
	}{}
};

/// Allocates a buffer (std::string) of size for the result.
///
/// see snstringf() for vararg format overload.
struct ircd::fmt::vsnstringf
:std::string
{
	vsnstringf(const size_t &max,
	           const string_view &fmt,
	           const va_rtti &ap)
	:std::string
	{
		ircd::string(max, [&fmt, &ap]
		(const mutable_buffer &buf) -> string_view
		{
			return vsprintf
			{
				buf, fmt, ap
			};
		})
	}{}
};

/// Allocates a buffer (std::string) of size for the result.
///
/// Note: that OR'ing the size with ircd::SHRINK_TO_FIT constant will trigger
/// a standard string re-evaluation to determine if the final result string
/// should be transferred to a smaller buffer.
struct ircd::fmt::snstringf
:vsnstringf
{
	template<class... args>
	snstringf(const size_t &max,
	          const string_view &fmt,
	          args&&... a)
	:vsnstringf
	{
		max, fmt, va_rtti{std::forward<args>(a)...}
	}{}
};

/// Efficient in-place buffer convenience template.
///
/// This is an object which contains the result buffer internally. The size
/// of that buffer is given in the template parameter. This is clean and
/// efficient for basic format string operations on the stack while avoiding
/// the clutter of buffer statements and mgmt.
template<size_t MAX>
struct ircd::fmt::bsprintf
:snprintf
,string_view
{
	char buf[MAX];

	template<class... args>
	bsprintf(const string_view &fmt,
	         args&&... a)
	:snprintf
	{
		internal, buf, fmt, va_rtti{std::forward<args>(a)...}
	}
	,string_view
	{
		buf, size_t(static_cast<snprintf &>(*this))
	}{}
};

/// Experimental typedef for format strings.
struct ircd::fmt::literal_snstringf
{
	string_view format;
};

/// Experimental typedef for format strings.
struct ircd::fmt::literal_snstringf__max
{
	literal_snstringf format;
	size_t max;
};

template<class... args>
inline std::string
ircd::fmt::operator,(const literal_snstringf__max format,
                     args&&... a)
{
	return fmt::snstringf
	{
		format.max, format.format.format, std::forward<args>(a)...
	};
}

inline ircd::fmt::literal_snstringf__max
ircd::fmt::operator,(const literal_snstringf format,
                     const ulong max)
{
	return literal_snstringf__max
	{
		format, max
	};
}

inline ircd::fmt::literal_snstringf
ircd::fmt::operator ""_snstringf(const char *const format,
                                 const size_t len)
{
	return literal_snstringf
	{
		string_view
		{
			format, len
		}
	};
}
