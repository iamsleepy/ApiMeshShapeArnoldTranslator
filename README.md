# ApiMeshShapeArnoldTranslator

A basic Arnold translator for the apiMeshShape in the Maya devkit.

# Usage

Copy ArnoldTranslator.dll into mtoadeploy/2017/extensions folder. Reload mtoa in Maya and the apiMeshShape can be rendered in Arnold now.

# Note

You'll need to modify apiMeshShape and expose apiMeshGeom and link with it. E.g.

```
    //Modify apiMeshGeom* meshGeom(); in the apiMeshShape.h
    __declspec(dllexport) apiMeshGeom*     __cdecl meshGeom();
```

For more details, please check 

https://around-the-corner.typepad.com/adn/2017/05/how-to-write-an-arnold-translator-for-mpxsurfaceshapes.html
