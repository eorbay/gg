/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_HH
#define STORAGE_BACKEND_HH

#include <vector>
#include <string>
#include <functional>

#include "path.hh"
#include "optional.hh"
#include "storage_requests.hh"

typedef std::function<void( const storage::PutRequest & )> PutCallback;
typedef std::function<void( const storage::GetRequest & )> GetCallback;

class StorageBackend
{
public:
  virtual void put( const std::vector<storage::PutRequest> & requests,
                    const PutCallback & success_callback = []( const storage::PutRequest & ){} ) = 0;

  virtual void get( const std::vector<storage::GetRequest> & requests,
                    const GetCallback & success_callback = []( const storage::GetRequest & ){} ) = 0;

  virtual ~StorageBackend() {}
};

#endif /* STORAGE_BACKEND_HH */
