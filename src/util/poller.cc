/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <algorithm>
#include <numeric>
#include <cassert>
#include "poller.hh"
#include "exception.hh"

using namespace std;
using namespace PollerShortNames;

void Poller::add_action( Poller::Action action )
{
  actions_.push_back( action );
  pollfds_.push_back( { action.fd.fd_num(), 0, 0 } );
}

unsigned int Poller::Action::service_count( void ) const
{
  return direction == Direction::In ? fd.read_count() : fd.write_count();
}

Poller::Result Poller::poll( const int & timeout_ms )
{
  /* make the pollfds structure */
  vector<pollfd> pollfds;
  for ( const Action & action : actions_ ) {
    /* tell poll whether we care about each fd */
    pollfd poll_entry { action.fd.fd_num(), 0, 0 };
    poll_entry.fd = action.fd.fd_num();
    if ( action.when_interested()
         and not ( action.direction == Direction::In
                   and action.fd.eof() ) ) {
      poll_entry.events = action.direction;
    }
  }

  /* Quit if no member in pollfds_ has a non-zero direction */
  if ( not accumulate( pollfds.begin(), pollfds.end(), false,
             [] ( bool acc, pollfd x ) { return acc or x.events; } ) ) {
    return Result::Type::Exit;
  }

  assert( pollfds.size() == actions.size() );

  if ( 0 == CheckSystemCall( "poll", ::poll( &pollfds[ 0 ], pollfds.size(), timeout_ms ) ) ) {
    return Result::Type::Timeout;
  }

  for ( unsigned int i = 0; i < pollfds.size(); i++ ) {
    if ( pollfds[ i ].revents & (POLLERR | POLLHUP | POLLNVAL) ) {
      //            throw Exception( "poll fd error" );
      return Result::Type::Exit;
    }

    if ( pollfds[ i ].revents & pollfds[ i ].events ) {
      /* we only want to call callback if revents includes
        the event we asked for */
      const auto count_before = actions_.at( i ).service_count();
      auto result = actions_.at( i ).callback();

      switch ( result.result ) {
      case ResultType::Exit:
        return Result( Result::Type::Exit, result.exit_status );
      case ResultType::Cancel:
        actions.erase( actions_.begin() + i );
        break;
      case ResultType::Continue:
        break;
      }

      if ( count_before == actions_.at( i ).service_count() ) {
        throw runtime_error( "Poller: busy wait detected: callback did not read/write fd" );
      }
    }
  }

  return Result::Type::Success;
}
