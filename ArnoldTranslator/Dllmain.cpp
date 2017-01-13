#include <extension//Extension.h>
#include "apiMeshShapeTranslator.h"
extern "C"
{
	DLLEXPORT void initializeExtension ( CExtension& extension )
	{
		MStatus status;

		extension.RegisterTranslator("apiMesh",
									"",
									CApiMeshShapeTranslator::creator);
		
	}

	DLLEXPORT void deinitializeExtension ( CExtension& extension )
	{
	}

}
