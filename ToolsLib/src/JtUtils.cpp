#include "JtUtils.h"

#include <assert.h>
#include  <cctype>

#include <JtElement_ProxyMetaData.hxx>
#include <JtElement_MetaData_PMIManager.hxx>
#include <JtProperty_LateLoaded.hxx>


using namespace std;



const std::vector<char> HandleLateLoadsMeta(const JtData_Object::VectorOfLateLoads& aLateLoaded)
{
    std::vector<char> ResultStream;

    for (int i = 0; i < aLateLoaded.Count(); i++) {
        if (aLateLoaded[i]->getSegmentType() == SegmentType::Meta_Data) {
            if (!aLateLoaded[i]->DefferedObject())
                aLateLoaded[i]->Load();
            //aLateLoaded[i]->LoadDump();

            std::shared_ptr<JtData_Object>  prop = aLateLoaded[i]->DefferedObject();
            if (prop && (dynamic_cast<const JtElement_ProxyMetaData*>(prop.get()) != nullptr)) {
                std::shared_ptr<JtElement_ProxyMetaData> aProxyMetaDataElement =
                    std::dynamic_pointer_cast<JtElement_ProxyMetaData>(prop);

                auto values = aProxyMetaDataElement->getKeyValueStream();
                ResultStream.insert(ResultStream.end(), values.begin(), values.end());
                //ResultStream. aProxyMetaDataElement->getKeyValueStream();

                //writeKeyValueStream(stream, outStream, indention);

               // aLateLoaded[i]->Unload();

            }
        }
        else if (aLateLoaded[i]->getSegmentType() == SegmentType::PMI_Data) {
            if (!aLateLoaded[i]->DefferedObject())
                aLateLoaded[i]->Load();

            std::shared_ptr<JtData_Object>  prop = aLateLoaded[i]->DefferedObject();
            if (prop && (dynamic_cast<const JtElement_MetaData_PMIManager*>(prop.get()) != nullptr)) {
                std::shared_ptr<JtElement_MetaData_PMIManager> aPMIManager =
                    std::dynamic_pointer_cast<JtElement_MetaData_PMIManager>(prop);

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
