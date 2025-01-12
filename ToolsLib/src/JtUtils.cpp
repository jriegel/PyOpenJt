#include "JtUtils.h"

#include <assert.h>
#include  <cctype>

#include <JtElement_ProxyMetaData.hxx>
#include <JtElement_MetaData_PMIManager.hxx>
#include <JtProperty_LateLoaded.hxx>

static Handle(Standard_Type) TypeOf_JtElement_ProxyMetaData = STANDARD_TYPE(JtElement_ProxyMetaData);
static Handle(Standard_Type) TypeOf_JtElement_MetaData_PMIManager = STANDARD_TYPE(JtElement_MetaData_PMIManager);

using namespace std;



const std::vector<char> HandleLateLoadsMeta(const JtData_Object::VectorOfLateLoads& aLateLoaded)
{
    std::vector<char> ResultStream;

    for (int i = 0; i < aLateLoaded.Count(); i++) {
        if (aLateLoaded[i]->getSegmentType() == SegmentType::Meta_Data) {
            if (aLateLoaded[i]->DefferedObject().IsNull())
                aLateLoaded[i]->Load();
            //aLateLoaded[i]->LoadDump();

            Handle(JtData_Object)  prop = aLateLoaded[i]->DefferedObject();
            if (!prop.IsNull() && prop->IsKind(TypeOf_JtElement_ProxyMetaData)) {
                Handle(JtElement_ProxyMetaData) aProxyMetaDataElement =
                    Handle(JtElement_ProxyMetaData)::DownCast(prop);

                auto values = aProxyMetaDataElement->getKeyValueStream();
                ResultStream.insert(ResultStream.end(), values.begin(), values.end());
                //ResultStream. aProxyMetaDataElement->getKeyValueStream();

                //writeKeyValueStream(stream, outStream, indention);

               // aLateLoaded[i]->Unload();

            }
        }
        else if (aLateLoaded[i]->getSegmentType() == SegmentType::PMI_Data) {
            if (aLateLoaded[i]->DefferedObject().IsNull())
                aLateLoaded[i]->Load();

            Handle(JtData_Object)  prop = aLateLoaded[i]->DefferedObject();
            if (!prop.IsNull() && prop->IsKind(TypeOf_JtElement_MetaData_PMIManager)) {
                Handle(JtElement_MetaData_PMIManager) aPMIManager =
                    Handle(JtElement_MetaData_PMIManager)::DownCast(prop);

                //aPMIManager->Dump(std::cout);
                //return aProxyMetaDataElement->getKeyValueStream();

                //writeKeyValueStream(stream, outStream, indention);

               // aLateLoaded[i]->Unload();

            }
        }
        else {
            int type = aLateLoaded[i]->getSegmentType();
            if (type != 17 && type != 18)
                cerr << "Unknown Late load: " << aLateLoaded[i]->getSegmentType() << '\n';
        }
    }

    return ResultStream;
}
