#include <fc_light/log/logger_config.hpp>
#include <fc_light/log/appender.hpp>
#include <fc_light/io/json.hpp>
#include <fc_light/filesystem.hpp>
#include <unordered_map>
#include <string>
#include <fc_light/log/console_appender.hpp>
#include <fc_light/reflect/variant.hpp>
#include <fc_light/exception/exception.hpp>
#include <iostream>

namespace fc_light {
   extern std::unordered_map<std::string,logger>& get_logger_map();
   extern std::unordered_map<std::string,appender::ptr>& get_appender_map();
   logger_config& logger_config::add_appender( const string& s ) { appenders.push_back(s); return *this; }

   void configure_logging( const fc_light::path& lc )
   {
      configure_logging( fc_light::json::from_file<logging_config>(lc) );
   }
   bool configure_logging( const logging_config& cfg )
   {
      try {
      static bool reg_console_appender = appender::register_appender<console_appender>( "console" );
      get_logger_map().clear();
      get_appender_map().clear();

      //slog( "\n%s", fc_light::json::to_pretty_string(cfg).c_str() );
      for( size_t i = 0; i < cfg.appenders.size(); ++i ) {
         appender::create( cfg.appenders[i].name, cfg.appenders[i].type, cfg.appenders[i].args );
        // TODO... process enabled
      }
      for( size_t i = 0; i < cfg.loggers.size(); ++i ) {
         auto lgr = logger::get( cfg.loggers[i].name );

         // TODO: finish configure logger here...
         if( cfg.loggers[i].parent.valid() ) {
            lgr.set_parent( logger::get( *cfg.loggers[i].parent ) );
         }
         lgr.set_name(cfg.loggers[i].name);
         if( cfg.loggers[i].level.valid() ) lgr.set_log_level( *cfg.loggers[i].level );
         

         for( auto a = cfg.loggers[i].appenders.begin(); a != cfg.loggers[i].appenders.end(); ++a ){
            auto ap = appender::get( *a );
            if( ap ) { lgr.add_appender(ap); }
         }
      }
        return reg_console_appender ;
      } catch ( exception& e )
      {
         std::cerr<<e.to_detail_string()<<"\n";
      }
      return false;
   }

   logging_config logging_config::default_config() {
      //slog( "default cfg" );
      logging_config cfg;

     variants  c;
               c.push_back(  mutable_variant_object( "level","debug")("color", "green") );
               c.push_back(  mutable_variant_object( "level","warn")("color", "brown") );
               c.push_back(  mutable_variant_object( "level","error")("color", "red") );

      cfg.appenders.push_back( 
             appender_config( "stderr", "console", 
                 mutable_variant_object()
                     ( "stream","std_error")
                     ( "level_colors", c ) 
                 ) ); 
      cfg.appenders.push_back( 
             appender_config( "stdout", "console", 
                 mutable_variant_object()
                     ( "stream","std_out") 
                     ( "level_colors", c ) 
                 ) ); 
      
      logger_config dlc;
      dlc.name = "default";
      dlc.level = log_level::debug;
      dlc.appenders.push_back("stderr");
      cfg.loggers.push_back( dlc );
      return cfg;
   }
}
