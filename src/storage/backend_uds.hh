/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_UDS_HH
#define STORAGE_BACKEND_UDS_HH

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "backend.hh"
#include "unix_socket.hh"

class UDSStorageBackend : public StorageBackend
{
public:
  UDSStorageBackend( void ); 
 
  void put( const std::vector<storage::PutRequest> & requests,
            const PutCallback & success_callback = []( const storage::PutRequest & ){} ) override;

  void get( const std::vector<storage::GetRequest> & requests,
            const GetCallback & success_callback = []( const storage::GetRequest & ){} ) override;

};

#endif /* STORAGE_BACKEND_UDS_HH */
