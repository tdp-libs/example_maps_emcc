MapWidget = 
{
  //! Called to create the OpenGL context and prepare the canvas for use.
  init: function(canvasID) 
  {
    var onRuntimeInitialized = Module.onRuntimeInitialized;
    Module.onRuntimeInitialized = function(){setTimeout(function()
    {
      var canvas = document.getElementById(canvasID);
      var ptr = allocate(intArrayFromString(canvasID), 'i8', 0);
      canvas.mapDetails = _createMap(ptr);
      _free(ptr);
  
      if(onRuntimeInitialized instanceof Function)
        onRuntimeInitialized();
      else
        Module.onRuntimeInitialized = null;
    })};
  },

  //! Pass the config JSON into the widget.
  setConfig: function(canvasID, config)
  {
    if(typeof config!=='string')
      config = JSON.stringify(config);
    var canvas = document.getElementById(canvasID);
    var ptr = allocate(intArrayFromString(config), 'i8', 0);
    _setConfig(canvas.mapDetails, ptr);
    _free(ptr);
  },

  getConfigJSON: "",
  getConfigCallback: Module.addFunction(function(ptr)
  {
    getConfigJSON = UTF8ToString(ptr);
  }, 'vi'),

  //! Get config JSON from the widget.
  getConfig: function(canvasID)
  {
    var canvas = document.getElementById(canvasID);
    _getConfig(canvas.mapDetails, this.getConfigCallback);
    let getConfigJSON_ = getConfigJSON;
    getConfigJSON="";
    return getConfigJSON_;
  },

  resize: function(canvasID)
  {
    var canvas = document.getElementById(canvasID);
    _resize(canvas.mapDetails);
  }
}
