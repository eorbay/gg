#include "storage_requests.hh"
#include "backend_uds.hh"

#include <vector>

int main() {
  
  std::vector<storage::PutRequest> reqs;
  storage::PutRequest req = {"test.txt", "deadc0de", ""};
  reqs.push_back(req);
  
  UDSStorageBackend backend;
  backend.put(reqs,  [] ( const storage::PutRequest & upload_request ) { std::cout << "put req: " << upload_request.filename.string() << std::endl; } );
  
  std::vector<storage::GetRequest> greqs;
  storage::GetRequest greq = {"deadc0de", "test2.txt"};
  greqs.push_back(greq);
  
  backend.get(greqs,  [] ( const storage::GetRequest & upload_request ) { std::cout << "put req: " << upload_request.filename.string() << std::endl; } );

  return 0;
}
