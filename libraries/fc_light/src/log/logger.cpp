#include <fc_light/log/logger.hpp>
#include <fc_light/log/log_message.hpp>
#include <fc_light/thread/spin_lock.hpp>
#include <fc_light/thread/scoped_lock.hpp>
#include <fc_light/log/appender.hpp>
#include <fc_light/filesystem.hpp>
#include <unordered_map>
#include <string>
#include <fc_light/log/logger_config.hpp>

#ifdef WITH_DIRECT_LOGGER
#include <stdarg.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif // WITH_DIRECT_LOGGER

namespace fc_light {

    class logger::impl : public fc_light::retainable {
      public:
         impl()
         :_parent(nullptr),_enabled(true),_additivity(false),_level(log_level::warn){}
         fc_light::string       _name;
         logger           _parent;
         bool             _enabled;
         bool             _additivity;
         log_level        _level;

         std::vector<appender::ptr> _appenders;
    };


    logger::logger()
    :my( new impl() ){}

    logger::logger(nullptr_t){}

    logger::logger( const string& name, const logger& parent )
    :my( new impl() )
    {
       my->_name = name;
       my->_parent = parent;
    }


    logger::logger( const logger& l )
    :my(l.my){}

    logger::logger( logger&& l )
    :my(fc_light::move(l.my)){}

    logger::~logger(){}

    logger& logger::operator=( const logger& l ){
       my = l.my;
       return *this;
    }
    logger& logger::operator=( logger&& l ){
       fc_light_swap(my,l.my);
       return *this;
    }
    bool operator==( const logger& l, std::nullptr_t ) { return !l.my; }
    bool operator!=( const logger& l, std::nullptr_t ) { return l.my;  }

    bool logger::is_enabled( log_level e )const {
       return e >= my->_level;
    }

    void logger::log( log_message m ) {
       m.get_context().append_context( my->_name );

       for( auto itr = my->_appenders.begin(); itr != my->_appenders.end(); ++itr )
          (*itr)->log( m );

       if( my->_additivity && my->_parent != nullptr) {
          my->_parent.log(m);
       }
    }
    void logger::set_name( const fc_light::string& n ) { my->_name = n; }
    const fc_light::string& logger::name()const { return my->_name; }

    extern bool do_default_config;

    std::unordered_map<std::string,logger>& get_logger_map() {
      static bool force_link_default_config = fc_light::do_default_config;
      //TODO: Atomic compare/swap set
      static std::unordered_map<std::string,logger>* lm = new std::unordered_map<std::string, logger>();
      (void)force_link_default_config; // hide warning;
      return *lm;
    }

    logger logger::get( const fc_light::string& s ) {
       static fc_light::spin_lock logger_spinlock;
       scoped_lock<spin_lock> lock(logger_spinlock);
       return get_logger_map()[s];
    }

    logger  logger::get_parent()const { return my->_parent; }
    logger& logger::set_parent(const logger& p) { my->_parent = p; return *this; }

    log_level logger::get_log_level()const { return my->_level; }
    logger& logger::set_log_level(log_level ll) { my->_level = ll; return *this; }

    void logger::add_appender( const fc_light::shared_ptr<appender>& a )
    { my->_appenders.push_back(a); }
    
//    void logger::remove_appender( const fc_light::shared_ptr<appender>& a )
 //   { my->_appenders.erase(a); }

    std::vector<fc_light::shared_ptr<appender> > logger::get_appenders()const
    {
        return my->_appenders;
    }

   bool configure_logging( const logging_config& cfg );
   bool do_default_config      = configure_logging( logging_config::default_config() );

#ifdef WITH_DIRECT_LOGGER
   void ddlog(const char *fname, int line, const char *func, const char *fmt, ...)
   {
       const char *fnm = strrchr(fname, '/');
       if(!fnm)
           fnm = fname;
       else
           fnm++;

       char buf[2048];
       char *c = buf;
       int csz = sizeof(buf);
       int r;

       {
           unsigned long tmm = (unsigned long)time(nullptr);
           r = snprintf(c, csz, "%lu", tmm);
           if((r > 0) && (r < csz))
               { csz -= r; c += r; }
       }
       {
           unsigned long tid = syscall(__NR_gettid);;
           r = snprintf(c, csz, " [%lu]", tid);
           if((r > 0) && (r < csz))
               { csz -= r; c += r; }
       }
       {
           r = snprintf(c, csz, " %s:%.3d (%s) ", fnm, line, func);
           if((r > 0) && (r < csz))
               { csz -= r; c += r; }
       }


       va_list ap;
       va_start(ap, fmt);

       r = vsnprintf(c, csz, fmt, ap);
       if((r > 0) && (r < csz))
           { csz -= r; c += r; }
       if(csz)
       {
           *c = '\n';
           csz--; c++;
       }

       va_end(ap);

       write(2, buf, c - buf);
   }
#endif // WITH_DIRECT_LOGGER


} // namespace fc_light
