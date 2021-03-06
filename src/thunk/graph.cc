/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "graph.hh"

#include <stdexcept>

#include "ggpaths.hh"
#include "thunk_reader.hh"
#include "thunk_writer.hh"

using namespace std;
using namespace gg::thunk;

DependencyGraph::DependencyGraph()
{}

void DependencyGraph::add_thunk( const string & hash )
{
  if ( thunks_.count( hash ) > 0 ) {
    return; // already added
  }

  ThunkReader thunk_reader { gg::paths::blob_path( hash ).string() };
  Thunk thunk = thunk_reader.read_thunk();

  for ( const InFile & infile : thunk.infiles() ) {
    if ( infile.order() > 0 ) {
      add_thunk( infile.content_hash() );
      referenced_thunks_[ infile.content_hash() ].insert( hash );
    }
    else {
      switch ( infile.type() ) {
      case InFile::Type::FILE:
        order_zero_dependencies_.insert( infile.content_hash() );
        break;

      case InFile::Type::EXECUTABLE:
        executable_dependencies_.insert( infile.content_hash() );
        break;

      default:
        break;
      }
    }
  }

  thunks_.emplace( make_pair( hash, move( thunk ) ) );
}

void DependencyGraph::update_thunk_hash( const string & old_hash,
                                         const string & new_hash )
{
  assert( thunks_.at( old_hash ).order() == 1 );

  thunks_.emplace( make_pair( new_hash, move( thunks_.at( old_hash ) ) ) );
  thunks_.erase( old_hash );

  if ( original_hashes_.count( old_hash ) == 0 ) {
    updated_hashes_.insert( { old_hash, new_hash } );
    original_hashes_.insert( { new_hash, old_hash } );
  }
  else {
    updated_hashes_[ original_hashes_[ old_hash ] ] = new_hash;
    original_hashes_.erase( old_hash );
  }

  if ( referenced_thunks_.count( old_hash ) ) {
    referenced_thunks_.emplace( make_pair( new_hash, move( referenced_thunks_.at( old_hash ) ) ) );
    referenced_thunks_.erase( old_hash );

    uint32_t new_size = gg::hash::extract_size( new_hash );

    for ( const string & thash : referenced_thunks_.at( new_hash ) ) {
      Thunk & ref_thunk = thunks_.at( thash );
      ref_thunk.update_infile( old_hash, new_hash, ref_thunk.order(), new_size );
    }
  }
}

unordered_set<string> DependencyGraph::force_thunk( const string & old_hash,
                                                    const string & new_hash )
{
  unordered_set<string> order_one_thunks;

  const Thunk & old_thunk = thunks_.at( old_hash );

  if ( old_thunk.order() != 1 ) {
    throw runtime_error( "can't force thunks with order != 1" );
  }

  if ( referenced_thunks_.count( old_hash ) == 0 ) {
    // no other thunk actually referenced this thunk
    // XXX is this the right thing to do?
    return order_one_thunks;
  }

  uint32_t new_size = gg::hash::extract_size( new_hash );

  for ( const string & thash : referenced_thunks_.at( old_hash ) ) {
    Thunk & ref_thunk = thunks_.at( thash );
    ref_thunk.update_infile( old_hash, new_hash, 0, new_size );

    if ( ref_thunk.order() == 1 ) {
      const string ref_thunk_hash = ThunkWriter::write_thunk( ref_thunk );
      update_thunk_hash( thash, ref_thunk_hash );
      order_one_thunks.insert( ref_thunk_hash );
    }
  }

  return order_one_thunks;
}

unordered_set<string>
DependencyGraph::order_one_dependencies( const string & thunk_hash ) const
{
  unordered_set<string> result;
  const Thunk & thunk = get_thunk( thunk_hash );

  if ( thunk.order() == 1 ) {
    result.insert( thunk_hash );
  }

  for ( const InFile & infile : thunk.infiles() ) {
    if ( infile.order() == 1 ) {
      result.insert( infile.content_hash() );
    }
    else if ( infile.order() > 1 ) {
      unordered_set<string> subresult = order_one_dependencies( infile.content_hash() );
      result.insert( subresult.begin(), subresult.end() );
    }
  }

  return result;
}

std::string DependencyGraph::updated_hash( const std::string & original_hash ) const
{
  if ( updated_hashes_.count( original_hash ) ) {
    return updated_hashes_.at( original_hash );
  }
  else {
    return original_hash;
  }
}
