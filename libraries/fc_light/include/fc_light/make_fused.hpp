#pragma once
#include <fc_light/tuple.hpp>
#include <fc_light/function.hpp>

namespace fc_light {
   template<typename R>
   std::function<R, fc_light::tuple<> > make_fused( const std::function<R>& f ) {
      return [=]( fc_light::tuple<> ){ return f(); };
   }
   template<typename R,typename A>
   std::function<R(fc_light::tuple<A>) > make_fused( const std::function<R(A)>& f ) {
      return [f]( fc_light::tuple<A> t){ return f(t.a); };
   }
   template<typename R,typename A,typename B>
   std::function<R(fc_light::tuple<A,B>) > make_fused( const std::function<R(A,B)>& f ) {
      return [f]( fc_light::tuple<A,B> t){ return f(t.a,t.b); };
   }
   template<typename R,typename A,typename B,typename C>
   std::function<R(fc_light::tuple<A,B,C>) > make_fused( const std::function<R(A,B,C)>& f ) {
      return [f]( fc_light::tuple<A,B,C> t){ return f(t.a,t.b,t.c); };
   }
   template<typename R,typename A,typename B,typename C,typename D>
   std::function<R(fc_light::tuple<A,B,C,D>) > make_fused( const std::function<R(A,B,C,D)>& f ) {
      return [f]( fc_light::tuple<A,B,C> t){ return f(t.a,t.b,t.c,t.d); };
   }
}
