/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_writer.hh"

#include <iostream>
#include <fstream>

#include "serialization.hh"
#include "ggpaths.hh"
#include "path.hh"
#include "digest.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

std::string ThunkWriter::write_thunk( const Thunk & thunk )
{
  const string serialized_thunk = serialize_thunk( thunk );
  const string thunk_hash = digest::sha256( serialized_thunk );

  if ( not roost::exists( paths::blob_path( thunk_hash ) ) ) {
    atomic_create( serialized_thunk, paths::blob_path( thunk_hash ) );
  }

  return thunk_hash;
}

string ThunkWriter::serialize_thunk( const gg::thunk::Thunk & thunk )
{
  string ret { MAGIC_NUMBER };
  if ( not thunk.to_protobuf().AppendToString( &ret ) ) {
    throw runtime_error( "could not serialize thunk for " + thunk.outfile() );
  }
  return ret;
}
