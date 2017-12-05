/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend_uds.hh"
#include <cajun/json/reader.h>
#include <cajun/json/writer.h>
#include <cajun/json/elements.h>

using namespace std;
using namespace storage;

UDSStorageBackend::UDSStorageBackend( const std::string & path)
  : pathname_(path), io_service_(), sock_(io_service_), end_(pathname_)
{
  sock_.connect(end_);
}

void UDSStorageBackend::put( const std::vector<PutRequest> & requests,
                            const PutCallback & success_callback )
{
  json::Array put_requests;
  for (auto& req : requests) {
    json::Object putreq;
    putreq["filename"] = json::String(req.filename.string());
    putreq["object-key"] = json::String(req.object_key);
    putreq["hash"] = json::String(req.content_hash.get_or(std::string("")));
    put_requests.Insert(putreq);
  }
  
  std::ostringstream oss;
  json::Writer::Write(put_requests, oss);

  const char* puts = oss.str().c_str();
  
  boost::asio::write(sock_, boost::asio::buffer(puts, std::strlen(puts)));
  
  char buffer[1];
  size_t bytesRead = boost::asio::read(sock_, boost::asio::buffer(buffer, 1));
  if (bytesRead > 0) {
    for (auto& req : requests) {
      success_callback(req);
    }
  }
}

void UDSStorageBackend::get( const std::vector<GetRequest> & requests,
                            const GetCallback & success_callback )
{
  json::Array get_requests;
  for (auto& req : requests) {
    json::Object get;
    get["object-key"] = json::String(req.object_key);
    get["filename"] = json::String(req.filename.string());
    get_requests.Insert(get);
  }
  std::ostringstream oss;
  json::Writer::Write(get_requests, oss);

  const char* gets = oss.str().c_str();

  boost::asio::write(sock_, boost::asio::buffer(gets, std::strlen(gets)));
 
  char buffer[1];
  size_t bytesRead = boost::asio::read(sock_, boost::asio::buffer(buffer, 1));
  if (bytesRead > 0) {
    for (auto& req : requests) {
      success_callback(req);
    }
  }
}
