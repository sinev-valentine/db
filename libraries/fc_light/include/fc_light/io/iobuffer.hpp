#pragma once
#include <fc_light/io/iostream.hpp>
#include <fc_light/exception/exception.hpp>

namespace fc_light
{
  /**
   *  Records the size, but discards the data.
   */
  class size_stream : public virtual fc_light::ostream
  {
      public:
      size_stream( size_t s  = 0):_size(s){}

      size_t size()const { return _size; }
      size_t seek( size_t pos ) { return _size = pos; }

      virtual size_t     writesome( const char* /*ignored buf*/, size_t len )
      {
         _size += len;
         return len;
      }

      virtual void       close(){}
      virtual void       flush(){}

      private:
      size_t            _size;
  };

  
  class iobuffer : public virtual fc_light::iostream
  {
      public:
      iobuffer( size_t s )
      :_data(s){}

      size_t size()const { return _data.size(); }
      size_t pos()const  { return _pos;  }
      size_t seek( size_t pos ) 
      { 
         return _pos = std::min<size_t>(_data.size(),pos); 
      }

      virtual size_t     readsome( char* buf, size_t len )
      {
          auto avail = std::min<size_t>( _data.size()-_pos, len );
          if( avail == 0 ) throw fc_light::eof_exception();
          memcpy( buf, _data.data()+_pos, avail );
          _pos += avail;
          return avail;
      }
      /**
       *  This method may block until at least 1 character is
       *  available.
       */
      char               peek()const 
      { 
          if( _pos == _data.size() ) throw fc_light::eof_exception();
         return _data[_pos]; 
      }

      virtual size_t     writesome( const char* buf, size_t len )
      {
         auto avail = std::max<size_t>( _data.size(), _pos + len );
         _data.resize(avail);
         memcpy( _data.data()+_pos, buf, len );
         _pos += avail;
         return avail;
      }
      char* data() { return _data.data(); }

      virtual void       close(){}
      virtual void       flush(){}

      private:
        std::vector<char> _data;
        size_t            _pos;
  };

}
