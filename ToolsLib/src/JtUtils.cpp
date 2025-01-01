#include "JtUtils.h"

#include <assert.h>
#include  <cctype>

#include <JtElement_ProxyMetaData.hxx>
#include <JtProperty_LateLoaded.hxx>

static Handle(Standard_Type) TypeOf_JtElement_ProxyMetaData = STANDARD_TYPE(JtElement_ProxyMetaData);


using namespace std;



const std::vector<char>& HandleLateLoadsMeta(const JtData_Object::VectorOfLateLoads& aLateLoaded)
{
    static std::vector<char> EmptyVector;

    for (int i = 0; i < aLateLoaded.Count(); i++) {
        if (aLateLoaded[i]->getSegmentType() == SegmentType::Meta_Data) {
            if (aLateLoaded[i]->DefferedObject().IsNull())
                aLateLoaded[i]->Load();

            Handle(JtData_Object)  prop = aLateLoaded[i]->DefferedObject();
            if (!prop.IsNull() && prop->IsKind(TypeOf_JtElement_ProxyMetaData)) {
                Handle(JtElement_ProxyMetaData) aProxyMetaDataElement =
                    Handle(JtElement_ProxyMetaData)::DownCast(prop);

                return aProxyMetaDataElement->getKeyValueStream();

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

    return EmptyVector;
}
