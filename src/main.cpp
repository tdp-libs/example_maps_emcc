#include "example_maps_emcc/Globals.h"

#include "tp_maps_emcc/Map.h"
#include "tp_maps_emcc/MapManager.h"

#include "tp_maps/controllers/FlatController.h"
#include "tp_maps/layers/Geometry3DLayer.h"
#include "tp_maps/layers/LinesLayer.h"

#include "tp_math_utils/Sphere.h"

#include "tp_utils/DebugUtils.h"

#include "glm/gtc/matrix_transform.hpp"

#include <set>

extern tp_maps_emcc::MapManager* manager;
tp_maps_emcc::MapManager* manager{nullptr};

//##################################################################################################
struct MapDetails : public tp_maps_emcc::MapDetails
{
  tp_maps::Geometry3DLayer* geometryLayer {nullptr};
  tp_maps::LinesLayer*      linesLayer    {nullptr};

  int rotation{0};

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
    details->map->setBackgroundColor({0.5f, 0.5f, 0.5f});

    details->geometryLayer = new tp_maps::Geometry3DLayer();
    details->map->addLayer(details->geometryLayer);

    details->linesLayer = new tp_maps::LinesLayer();
    details->map->addLayer(details->linesLayer);

    std::vector<tp_maps::Geometry3D> geometry;
    auto& mesh = geometry.emplace_back().geometry;
    mesh = tp_math_utils::Sphere::octahedralClass1(6.0, 
                                                   4, 
                                                   GL_TRIANGLE_FAN,
                                                   GL_TRIANGLE_STRIP,
                                                   GL_TRIANGLES);
    mesh.calculateFaceNormals();

    details->geometryLayer->setGeometry(geometry);
    details->linesLayer->setLinesFromGeometry(geometry);

    auto controller = new tp_maps::FlatController(details->map);
    controller->setAllowRotation(false);
    controller->setVariableViewAngle(false);
    controller->setAllowTranslation(false);

    details->geometryLayer->animateCallbacks.addCallback([=](double t)
    {
      details->rotation+=1;
      details->rotation%=360;
      glm::mat4 m{1.0f};
      m = glm::rotate(m, glm::radians(float(details->rotation)), glm::vec3(0.0, 1.0, 0.0));
      details->geometryLayer->setObjectMatrix(m);
      details->linesLayer->setObjectMatrix(m);
    });

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
