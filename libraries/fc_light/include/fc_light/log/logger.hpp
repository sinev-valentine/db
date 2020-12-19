#pragma once
#include <fc_light/string.hpp>
#include <fc_light/time.hpp>
#include <fc_light/shared_ptr.hpp>
#include <fc_light/log/log_message.hpp>

namespace fc_light
{

   class appender;

   /**
    *
    *
    @code
      void my_class::func() 
      {
         fc_dlog( my_class_logger, "Format four: ${arg}  five: ${five}", ("arg",4)("five",5) );
      }
    @endcode
    */
   class logger 
   {
      public:
         static logger get( const fc_light::string& name = "default");

         logger();
         logger( const string& name, const logger& parent = nullptr );
         logger( std::nullptr_t );
         logger( const logger& c );
         logger( logger&& c );
         ~logger();
         logger& operator=(const logger&);
         logger& operator=(logger&&);
         friend bool operator==( const logger&, nullptr_t );
         friend bool operator!=( const logger&, nullptr_t );

         logger&    set_log_level( log_level e );
         log_level  get_log_level()const;
         logger&    set_parent( const logger& l );
         logger     get_parent()const;

         void  set_name( const fc_light::string& n );
         const fc_light::string& name()const;

         void add_appender( const fc_light::shared_ptr<appender>& a );
         std::vector<fc_light::shared_ptr<appender> > get_appenders()const;
         void remove_appender( const fc_light::shared_ptr<appender>& a );

         bool is_enabled( log_level e )const;
         void log( log_message m );

      private:
         class impl;
         fc_light::shared_ptr<impl> my;
   };

//#define WITH_DIRECT_LOGGER
#ifdef WITH_DIRECT_LOGGER
   void ddlog(const char *fname, int line, const char *func, const char *fmt, ...);
#define DDLOG(fmt, ...) ddlog(__FILE__, __LINE__, __func__, (fmt), ##__VA_ARGS__);
#else
#define DDLOG(fmt, ...) FC_LIGHT_MULTILINE_MACRO_BEGIN FC_LIGHT_MULTILINE_MACRO_END
#endif // WITH_DIRECT_LOGGER


} // namespace fc_light

#ifndef DEFAULT_LOGGER
#define DEFAULT_LOGGER
#endif

// suppress warning "conditional expression is constant" in the while(0) for visual c++
// http://cnicholson.net/2009/03/stupid-c-tricks-dowhile0-and-c4127/
#define FC_LIGHT_MULTILINE_MACRO_BEGIN do {
#ifdef _MSC_VER
# define FC_LIGHT_MULTILINE_MACRO_END \
    __pragma(warning(push)) \
    __pragma(warning(disable:4127)) \
    } while (0) \
    __pragma(warning(pop))
#else
# define FC_LIGHT_MULTILINE_MACRO_END  } while (0)
#endif

#define fc_dlog( LOGGER, FORMAT, ... ) \
  FC_LIGHT_MULTILINE_MACRO_BEGIN \
   if( (LOGGER).is_enabled( fc_light::log_level::debug ) ) \
      (LOGGER).log( FC_LIGHT_LOG_MESSAGE( debug, FORMAT, __VA_ARGS__ ) ); \
  FC_LIGHT_MULTILINE_MACRO_END

#define fc_ilog( LOGGER, FORMAT, ... ) \
  FC_LIGHT_MULTILINE_MACRO_BEGIN \
   if( (LOGGER).is_enabled( fc_light::log_level::info ) ) \
      (LOGGER).log( FC_LIGHT_LOG_MESSAGE( info, FORMAT, __VA_ARGS__ ) ); \
  FC_LIGHT_MULTILINE_MACRO_END

#define fc_wlog( LOGGER, FORMAT, ... ) \
  FC_LIGHT_MULTILINE_MACRO_BEGIN \
   if( (LOGGER).is_enabled( fc_light::log_level::warn ) ) \
      (LOGGER).log( FC_LIGHT_LOG_MESSAGE( warn, FORMAT, __VA_ARGS__ ) ); \
  FC_LIGHT_MULTILINE_MACRO_END

#define fc_elog( LOGGER, FORMAT, ... ) \
  FC_LIGHT_MULTILINE_MACRO_BEGIN \
   if( (LOGGER).is_enabled( fc_light::log_level::error ) ) \
      (LOGGER).log( FC_LIGHT_LOG_MESSAGE( error, FORMAT, __VA_ARGS__ ) ); \
  FC_LIGHT_MULTILINE_MACRO_END

#define dlog( FORMAT, ... ) \
  FC_LIGHT_MULTILINE_MACRO_BEGIN \
   if( (fc_light::logger::get(DEFAULT_LOGGER)).is_enabled( fc_light::log_level::debug ) ) \
      (fc_light::logger::get(DEFAULT_LOGGER)).log( FC_LIGHT_LOG_MESSAGE( debug, FORMAT, __VA_ARGS__ ) ); \
  FC_LIGHT_MULTILINE_MACRO_END

/**
 * Sends the log message to a special 'user' log stream designed for messages that
 * the end user may like to see.
 */
#define ulog( FORMAT, ... ) \
  FC_LIGHT_MULTILINE_MACRO_BEGIN \
   if( (fc_light::logger::get("user")).is_enabled( fc_light::log_level::debug ) ) \
      (fc_light::logger::get("user")).log( FC_LIGHT_LOG_MESSAGE( debug, FORMAT, __VA_ARGS__ ) ); \
  FC_LIGHT_MULTILINE_MACRO_END


#define ilog( FORMAT, ... ) \
  FC_LIGHT_MULTILINE_MACRO_BEGIN \
   if( (fc_light::logger::get(DEFAULT_LOGGER)).is_enabled( fc_light::log_level::info ) ) \
      (fc_light::logger::get(DEFAULT_LOGGER)).log( FC_LIGHT_LOG_MESSAGE( info, FORMAT, __VA_ARGS__ ) ); \
  FC_LIGHT_MULTILINE_MACRO_END

#define wlog( FORMAT, ... ) \
  FC_LIGHT_MULTILINE_MACRO_BEGIN \
   if( (fc_light::logger::get(DEFAULT_LOGGER)).is_enabled( fc_light::log_level::warn ) ) \
      (fc_light::logger::get(DEFAULT_LOGGER)).log( FC_LIGHT_LOG_MESSAGE( warn, FORMAT, __VA_ARGS__ ) ); \
  FC_LIGHT_MULTILINE_MACRO_END

#define elog( FORMAT, ... ) \
  FC_LIGHT_MULTILINE_MACRO_BEGIN \
   if( (fc_light::logger::get(DEFAULT_LOGGER)).is_enabled( fc_light::log_level::error ) ) \
      (fc_light::logger::get(DEFAULT_LOGGER)).log( FC_LIGHT_LOG_MESSAGE( error, FORMAT, __VA_ARGS__ ) ); \
  FC_LIGHT_MULTILINE_MACRO_END

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/punctuation/paren.hpp>


#define FC_LIGHT_FORMAT_ARG(r, unused, base) \
  BOOST_PP_STRINGIZE(base) ": ${" BOOST_PP_STRINGIZE( base ) "} "

#define FC_LIGHT_FORMAT_ARGS(r, unused, base) \
  BOOST_PP_LPAREN() BOOST_PP_STRINGIZE(base),fc_light::variant(base) BOOST_PP_RPAREN()

#define FC_LIGHT_FORMAT( SEQ )\
    BOOST_PP_SEQ_FOR_EACH( FC_LIGHT_FORMAT_ARG, v, SEQ )

// takes a ... instead of a SEQ arg because it can be called with an empty SEQ 
// from FC_LIGHT_CAPTURE_AND_THROW()
#define FC_LIGHT_FORMAT_ARG_PARAMS( ... )\
    BOOST_PP_SEQ_FOR_EACH( FC_LIGHT_FORMAT_ARGS, v, __VA_ARGS__ )

#define idump( SEQ ) \
    ilog( FC_LIGHT_FORMAT(SEQ), FC_LIGHT_FORMAT_ARG_PARAMS(SEQ) )
#define wdump( SEQ ) \
    wlog( FC_LIGHT_FORMAT(SEQ), FC_LIGHT_FORMAT_ARG_PARAMS(SEQ) )
#define edump( SEQ ) \
    elog( FC_LIGHT_FORMAT(SEQ), FC_LIGHT_FORMAT_ARG_PARAMS(SEQ) )

// this disables all normal logging statements -- not something you'd normally want to do,
// but it's useful if you're benchmarking something and suspect logging is causing
// a slowdown.
#ifdef FC_LIGHT_DISABLE_LOGGING
# undef ulog
# define ulog(...) FC_LIGHT_MULTILINE_MACRO_BEGIN FC_LIGHT_MULTILINE_MACRO_END
# undef elog
# define elog(...) FC_LIGHT_MULTILINE_MACRO_BEGIN FC_LIGHT_MULTILINE_MACRO_END
# undef wlog
# define wlog(...) FC_LIGHT_MULTILINE_MACRO_BEGIN FC_LIGHT_MULTILINE_MACRO_END
# undef ilog
# define ilog(...) FC_LIGHT_MULTILINE_MACRO_BEGIN FC_LIGHT_MULTILINE_MACRO_END
# undef dlog
# define dlog(...) FC_LIGHT_MULTILINE_MACRO_BEGIN FC_LIGHT_MULTILINE_MACRO_END
#endif
