///////////////////////////////////////////////////////////////////////////////
//
// util/pretty.h
//

//
// Human readable time suite
//

namespace ircd { inline namespace util
{
	using pretty_time_formats = std::array<string_view, 2>;
	using pretty_time_element = std::tuple<pretty_time_formats, long double>;

	template<size_t i,
	         class T>
	static string_view pretty(const mutable_buffer &, const T &, const uint &fmt);

	[[gnu::visibility("internal")]]
	extern const std::array<pretty_time_element, 9> pretty_time_unit;
}}

decltype(ircd::util::pretty_time_unit)
ircd::util::pretty_time_unit
{{
	//  fmt=0              fmt=1
	{ { "nanoseconds",     "ns" },     1000.0L  },
	{ { "microseconds",    "us" },     1000.0L  },
	{ { "milliseconds",    "ms" },     1000.0L  },
	{ { "seconds",         "s"  },       60.0L  },
	{ { "minutes",         "m"  },       60.0L  },
	{ { "hours",           "h"  },       24.0L  },
	{ { "days",            "d"  },        7.0L  },
	{ { "weeks",           "w"  },        4.0L  },
	{ { "months",          "M"  },       12.0L  },
}};

template<size_t i,
         class T>
inline ircd::string_view
ircd::util::pretty(const mutable_buffer &out,
                   const T &dur,
                   const uint &fmt)
{
	static const auto &unit
	{
		pretty_time_unit
	};

	const string_view &fmtstr
	{
		fmt == 1?
			"%.2lf%s"_sv:
			"%.2lf %s"_sv
	};

	size_t pos(i);
	long double val(dur.count());
	for(; val > std::get<1>(unit.at(pos)) && pos < unit.size() - 1; ++pos)
		val /= std::get<1>(unit.at(pos));

	return fmt::sprintf
	{
		out, fmtstr,
		val,
		std::get<0>(unit.at(pos)).at(fmt)
	};
}

ircd::string_view
ircd::util::pretty(const mutable_buffer &out,
                   const nanoseconds &val,
                   const uint &fmt)
{
	return pretty<0>(out, val, fmt);
}

ircd::string_view
ircd::util::pretty(const mutable_buffer &out,
                   const microseconds &val,
                   const uint &fmt)
{
	return pretty<1>(out, val, fmt);
}

ircd::string_view
ircd::util::pretty(const mutable_buffer &out,
                   const milliseconds &val,
                   const uint &fmt)
{
	return pretty<2>(out, val, fmt);
}

ircd::string_view
ircd::util::pretty(const mutable_buffer &out,
                   const seconds &val,
                   const uint &fmt)
{
	return pretty<3>(out, val, fmt);
}

//
// Human readable space suite
//

decltype(ircd::util::pretty_fmt)
ircd::util::pretty_fmt
{
	"%.2lf %s (%lu)"_sv,
	"%.2lf %s"_sv,
	"%.2lf%s"_sv,
	string_view{},
};

std::string
ircd::util::pretty(const human_readable_size &value,
                   const uint &fmt)
{
	return pretty(value, pretty_fmt[fmt]);
}

ircd::string_view
ircd::util::pretty(const mutable_buffer &out,
                   const human_readable_size &value,
                   const uint &fmt)
{
	return pretty(out, pretty_fmt[fmt], value);
}

std::string
ircd::util::pretty(const human_readable_size &value,
                   const string_view &fmt)
{
	return util::string(64, [&value, &fmt]
	(const mutable_buffer &out)
	{
		return pretty(out, fmt, value);
	});
}

ircd::string_view
ircd::util::pretty(const mutable_buffer &out,
                   const string_view &fmt,
                   const human_readable_size &value)
try
{
	return fmt::sprintf
	{
		out, fmt,
		std::get<long double>(value),
		std::get<const string_view &>(value),
		std::get<uint64_t>(value)
	};
}
catch(const std::out_of_range &e)
{
	return fmt::sprintf
	{
		out, "%lu",
		std::get<uint64_t>(value)
	};
}

ircd::human_readable_size
ircd::util::si(const uint64_t &value)
{
	static const std::array<string_view, 7> unit
	{
		" ", "K", "M", "G", "T", "P", "E"
	};

	auto pos(0);
	long double v(value);
	for(; v > 1000.0L; v /= 1000.0L, ++pos);
	return
	{
		value, v, unit.at(pos)
	};
}

ircd::human_readable_size
ircd::util::iec(const uint64_t &value)
{
	static const std::array<string_view, 7> unit
	{
		"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"
	};

	auto pos(0);
	long double v(value);
	for(; v > 1024.0L; v /= 1024.0L, ++pos);
	return
	{
		value, v, unit.at(pos)
	};
}