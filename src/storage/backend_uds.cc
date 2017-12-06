/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "backend_uds.hh"
#include <cajun/json/reader.h>
#include <cajun/json/writer.h>
#include <cajun/json/elements.h>

using namespace std;
using namespace storage;

UDSStorageBackend::UDSStorageBackend( void )
{}

void UDSStorageBackend::put( const std::vector<PutRequest> & requests,
                            const PutCallback & success_callback )
{
  UnixDomainSocket uds;
  json::Object data;
  data["type"] = json::String(std::string("PUT"));
  json::Array put_requests;
  for (auto& req : requests) {
    json::Object putreq;
    putreq["filename"] = json::String(req.filename.string());
    putreq["object-key"] = json::String(req.object_key);
    putreq["hash"] = json::String(req.content_hash.get_or(std::string("")));
    put_requests.Insert(putreq);
  }
  data["requests"] = put_requests;

  std::ostringstream oss;
  json::Writer::Write(data, oss);
  std::string json_str = oss.str();
  unsigned int json_len = json_str.length();
  std::cout << "Len: " << json_len << " str: " << json_str << std::endl;
  std::ostringstream oss2;
  oss2.write(reinterpret_cast<const char*>(&json_len), sizeof(unsigned int));
  oss2 << json_str;
  
  uds.send(oss2.str());
  
  std::string recv_data = uds.recv();
  if (recv_data.size() > 0) {
    for (auto& req : requests) {
      success_callback(req);
    }
  }
}

void UDSStorageBackend::get( const std::vector<GetRequest> & requests,
                            const GetCallback & success_callback )
{ 
  UnixDomainSocket uds;
  json::Array get_requests;
  json::Object data;
  data["type"] = json::String(std::string("GET"));
  for (auto& req : requests) {
    json::Object get;
    get["object-key"] = json::String(req.object_key);
    get["filename"] = json::String(req.filename.string());
    get_requests.Insert(get);
  }

  data["requests"] = get_requests;
  
  std::ostringstream oss;
  json::Writer::Write(data, oss);
  std::string json_str = oss.str();
  unsigned int json_len = json_str.length();
  std::cout << "Len: " << json_len << " str: " << json_str << std::endl;
  std::ostringstream oss2;
  oss2.write(reinterpret_cast<const char*>(&json_len), sizeof(unsigned int));
  oss2 << json_str;
  
  uds.send(oss2.str());
  
  std::string recv_data = uds.recv();
  if (recv_data.size() > 0) {
    for (auto& req : requests) {
      success_callback(req);
    }
  }
}
