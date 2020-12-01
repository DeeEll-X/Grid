#include "args.hpp"
#include <stdexcept>

namespace Grid {
std::string StatusToString(Status stat){
  switch(stat){
    case CREATING:
      return "creating";
    case CREATED:
      return "created";
    case RUNNING:
      return "running";
    case STOPPED:
      return "stopped";
  }
}

Status StringToStatus(const std::string& stat){
  if(stat == "creating"){
    return CREATING;
  } else if(stat == "created"){
    return CREATED;
  } else if(stat == "running"){
    return RUNNING;
  } else if(stat == "stopped"){
    return STOPPED;
  }
  throw std::runtime_error("unknown status: "+ stat );
}
}