#include <translators/shape/ShapeTranslator.h>

#include <translators/NodeTranslator.h>

class CApiMeshShapeTranslator:public CShapeTranslator
{
public:
	virtual AtNode* CreateArnoldNodes();
	virtual void Export(AtNode* polymesh);
	virtual void Update(AtNode* polymesh);

	void Export(AtNode* polymesh, bool isUpdate);	
	void ExportMeshData(AtNode* polymesh);
	void ExportMeshShaders(AtNode* polymesh);

	static void* creator()
	{
		cout << "ApiMeshShapeTranlator loaded." <<endl;		
		return new CApiMeshShapeTranslator();
	}
};