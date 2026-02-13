#pragma once

#include <boost/json.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/message.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace logging = boost::log;
namespace json = boost::json;

inline std::string ToString(logging::trivial::severity_level severity) {
	switch (severity) {
	case logging::trivial::trace:
		return "TRACE";
	case logging::trivial::debug:
		return "DEBUG";
	case logging::trivial::info:
		return "INFO";
	case logging::trivial::warning:
		return "WARN";
	case logging::trivial::error:
		return "ERROR";
	case logging::trivial::fatal:
		return "FATAL";
	}

	return "";
}

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(request_type, "request_type", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(request_arg, "request_arg", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(scenario_id, "scenario_id", size_t)
BOOST_LOG_ATTRIBUTE_KEYWORD(exception, "exception", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(where, "where", std::string)

inline void JsonFormatter(logging::record_view const& rec, logging::formatting_ostream& strm) {
	auto ts = rec[timestamp];
	auto message = rec[logging::expressions::smessage];
	auto severity = rec[logging::trivial::severity];

	json::object log;
	json::object data;

	log["timestamp"] = boost::posix_time::to_iso_extended_string(*ts);

	if (severity) {
		data["type"] = ToString(*severity);
	}

	auto req_type_attr = rec[request_type];
	if (req_type_attr) {
		data["request_type"] = req_type_attr->c_str();
	}

	auto req_arg_attr = rec[request_arg];
	if (req_arg_attr) {
		data["request_arg"] = req_arg_attr->c_str();
	}

	auto scenario_id_attr = rec[scenario_id];
	if (scenario_id_attr) {
		data["scenario_id"] = *scenario_id_attr;
	}

	auto exception_attr = rec[exception];
	if (exception_attr) {
		data["exception"] = exception_attr->c_str();
	}

	auto where_attr = rec[where];
	if (where_attr) {
		data["where"] = where_attr->c_str();
	}

	log["data"] = std::move(data);
	log["message"] = *message;

	strm << log;
}
