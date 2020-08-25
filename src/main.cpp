#include "example_maps_emcc/Globals.h"

#include "tp_maps_emcc/Map.h"
#include "tp_maps_emcc/MapManager.h"

#include "tp_maps/layers/ImageLayer.h"
#include "tp_maps/textures/DefaultSpritesTexture.h"
#include "tp_maps/SpriteTexture.h"
#include "tp_maps/controllers/AnimatedFlatController.h"
#include "tp_maps/layers/GeometryLayer.h"
#include "tp_maps/layers/LinesLayer.h"
#include "tp_maps/MouseEvent.h"
#include "tp_maps/picking_results/GeometryPickingResult.h"

#include "tp_math_utils/Polygon.h"

#include "tp_utils/DebugUtils.h"

#include <set>

extern example_maps_emcc::MapManager* manager;
example_maps_emcc::MapManager* manager{nullptr};

//##################################################################################################
struct MapDetails : public tp_maps_emcc::MapDetails
{
  tp_maps::GeometryLayer*          geometryLayer {nullptr};
  tp_maps::LinesLayer*             linesLayer    {nullptr};

  tp_maps::AnimatedFlatController* controller    {nullptr};

  std::vector<tp_maps::Geometry> existingSections;
  int selectedSection{-1};
  std::string selectedSegment;

  std::string clickablePolygonSet{"Extracted"};
  std::unordered_map<int, int> geometryIndexLookup;

  bool showAvailable{true};
  bool showSelected{true};
  bool showMerged{true};
  float minVolume{100.0f};

  //################################################################################################
  MapDetails(tp_maps_emcc::Map* map_):
    tp_maps_emcc::MapDetails(map_)
  {
    roiRect.geometry.reserve(4);
    roiRect.geometry.emplace_back(0.0f, 0.0f, 0.0f);
    roiRect.geometry.emplace_back(10.0f, 0.0f, 0.0f);
    roiRect.geometry.emplace_back(10.0f, 10.0f, 0.0f);
    roiRect.geometry.emplace_back(0.0f, 10.0f, 0.0f);

    roiRect.material.ambient  = glm::vec3(0.9f, 0.9f, 0.2f);
    roiRect.material.diffuse  = glm::vec3(1.0f, 1.0f, 0.0f);
    roiRect.material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    roiRect.material.shininess = 32.0f;
    roiRect.material.alpha = 0.2f;
  }

  //################################################################################################
  ~MapDetails() override;

  //################################################################################################
  void updateGeometry()
  {
    geometryLayer->setGeometry({roiRect});

    tp_maps::Lines roiLines;
    roiLines.lines = roiRect.geometry;
    roiLines.color = {1.0f, 0.0f, 0.0f, 1.0f};
    linesLayer->setLines({roiLines});
  }
};

//##################################################################################################
MapDetails::~MapDetails()=default;


//##################################################################################################
int main()
{
//#warning fix
//  static int ii = tp_maps::staticInit();

  auto createMapCallback = [](tp_maps_emcc::Map* map)
  {
    MapDetails* details = new MapDetails(map);
    details->map->setBackgroundColor({0.5f, 0.5f, 0.5f});

    details->geometryLayer = new tp_maps::GeometryLayer();
    details->geometryLayer->setDefaultRenderPass(tp_maps::RenderPass::GUI);
    details->map->addLayer(details->geometryLayer);

    details->linesLayer = new tp_maps::LinesLayer();
    details->linesLayer->setDefaultRenderPass(tp_maps::RenderPass::GUI);
    details->map->addLayer(details->linesLayer);

    details->updateGeometry();

    details->controller = new tp_maps::AnimatedFlatController(details->map);
    details->controller->setVariableViewAngle(false);


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
    {
      j["type"] = "Feature";

      auto& g = j["geometry"];
      g["type"] = "polygon";

      nlohmann::json geometry = nlohmann::json::array();
      const std::vector<tp_maps::HandleDetails*>& handles = details->handleLayer->handles();
      for(const tp_maps::HandleDetails* point : handles)
        geometry.push_back(nlohmann::json::array({int(point->position.x),
                                                  int(point->position.y)}));

      g["coordinates"] = nlohmann::json::array({geometry});
    }

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
