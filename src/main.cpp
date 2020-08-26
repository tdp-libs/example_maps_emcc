#include "example_maps_emcc/Globals.h"

#include "tp_maps_emcc/Map.h"
#include "tp_maps_emcc/MapManager.h"

#include "tp_maps_example/BuildExample.h"

#include "tp_utils/DebugUtils.h"

#include <set>

extern tp_maps_emcc::MapManager* manager;
tp_maps_emcc::MapManager* manager{nullptr};

//##################################################################################################
struct MapDetails : public tp_maps_emcc::MapDetails
{
  //################################################################################################
  MapDetails(tp_maps_emcc::Map* map_):
    tp_maps_emcc::MapDetails(map_)
  {

  }

  //################################################################################################
  ~MapDetails() override;
};

//##################################################################################################
MapDetails::~MapDetails()=default;


//##################################################################################################
int main()
{
  auto createMapCallback = [](tp_maps_emcc::Map* map)
  {
    MapDetails* details = new MapDetails(map);
    tp_maps_example::buildExample(details->map);
    return details;
  };

  manager = new tp_maps_emcc::MapManager(createMapCallback);

  manager->exec();
  delete manager;
  return 0;
}

extern "C"
{

//##################################################################################################
void* createMap(char* canvasID)
{
  return manager?manager->createMap(canvasID):nullptr;
}

//##################################################################################################
void destroyMap(void* handle)
{
  if(manager)
    manager->destroyMap(handle);
}

//##################################################################################################
void setConfig(void* handle, char* data)
{
  if(handle)
  {
    try
    {
      nlohmann::json j = nlohmann::json::parse(std::string(data));
    }
    catch(...)
    {
    }
  }
}

//##################################################################################################
void getConfig(void* handle, void(*callback)(const void*))
{
  if(handle)
  {
    MapDetails* details = reinterpret_cast<MapDetails*>(handle);

    nlohmann::json j;
    
    std::string data = j.dump();
    callback(data.data());
  }
}

//##################################################################################################
void resize(void* handle)
{
  if(handle)
  {
    MapDetails* details = reinterpret_cast<MapDetails*>(handle);
    details->map->resize();
  }
}

}
