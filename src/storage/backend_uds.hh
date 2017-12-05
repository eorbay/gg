/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_UDS_HH
#define STORAGE_BACKEND_UDS_HH

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "boost/asio.hpp"
#include "backend.hh"

using boost::asio::local::stream_protocol;

class UDSStorageBackend : public StorageBackend
{
private:
  const std::string & pathname_;
  boost::asio::io_service io_service_;
  stream_protocol::socket sock_;
  stream_protocol::endpoint end_;
  
public:
  UDSStorageBackend( const std::string & path ); 
  
  void put( const std::vector<storage::PutRequest> & requests,
            const PutCallback & success_callback = []( const storage::PutRequest & ){} ) override;

  void get( const std::vector<storage::GetRequest> & requests,
            const GetCallback & success_callback = []( const storage::GetRequest & ){} ) override;

};

#endif /* STORAGE_BACKEND_UDS_HH */
