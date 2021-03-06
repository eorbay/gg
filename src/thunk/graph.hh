/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef GRAPH_HH
#define GRAPH_HH

#include <string>
#include <memory>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

#include "thunk.hh"

class DependencyGraph
{
private:
  std::unordered_map<std::string, gg::thunk::Thunk> thunks_ {};
  std::unordered_map<std::string, std::set<std::string>> referenced_thunks_ {};

  std::unordered_map<std::string, std::string> updated_hashes_{};
  std::unordered_map<std::string, std::string> original_hashes_{};

  std::unordered_set<std::string> order_zero_dependencies_ {};
  std::unordered_set<std::string> executable_dependencies_ {};

  void update_thunk_hash( const std::string & old_hash,
                          const std::string & new_hash );

public:
  DependencyGraph();

  void add_thunk( const std::string & hash );

  std::unordered_set<std::string> force_thunk( const std::string & old_hash,
                                               const std::string & new_hash );

  const std::unordered_set<std::string> &
  order_zero_dependencies() const { return order_zero_dependencies_; }

  const std::unordered_set<std::string> &
  executable_dependencies() const { return executable_dependencies_; }

  std::unordered_set<std::string>
  order_one_dependencies( const std::string & thunk_hash ) const;

  const gg::thunk::Thunk &
  get_thunk( const std::string & hash ) const { return thunks_.at( hash ); }

  std::string updated_hash( const std::string & original_hash ) const;

  size_t size() const { return thunks_.size(); }
};

#endif /* GRAPH_HH */
