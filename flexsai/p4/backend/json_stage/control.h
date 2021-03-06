/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _BACKENDS_SAI_CONTROL_H_
#define _BACKENDS_SAI_CONTROL_H_

#include "ir/ir.h"
#include "lib/json.h"
#include "analyzer.h"
#include "frontends/p4/coreLibrary.h"
#include "frontends/p4/typeMap.h"
#include "frontends/p4/typeChecking/typeChecker.h"
#include "frontends/common/resolveReferences/referenceMap.h"
#include "midend/convertEnums.h"
#include "expression.h"
#include "helpers.h"
#include "sharedActionSelectorCheck.h"
#include "saiSwitch.h"

namespace SAI {

class ControlConverter : public Inspector {
    Backend*               backend;
    P4::ReferenceMap*      refMap;
    P4::TypeMap*           typeMap;
    ExpressionConverter*   conv;
    SAI::JsonObjects*     json;

 protected:
    Util::IJson* convertTable(const CFG::TableNode* node,
                              Util::JsonArray* action_profiles,
                              SAI::SharedActionSelectorCheck& selector_check);
    void convertTableEntries(const IR::P4Table *table, Util::JsonObject *jsonTable);
    cstring getKeyMatchType(const IR::KeyElement *ke);
    /// Return 'true' if the table is 'simple'
    bool handleTableImplementation(const IR::Property* implementation, const IR::Key* key,
                                   Util::JsonObject* table, Util::JsonArray* action_profiles,
                                   SAI::SharedActionSelectorCheck& selector_check);
    Util::IJson* convertIf(const CFG::IfNode* node, cstring prefix);
    Util::IJson* convertControl(const IR::ControlBlock* block, cstring name,
                                Util::JsonArray *counters, Util::JsonArray* meters,
                                Util::JsonArray* registers);
    Util::JsonObject* correlateHeader(Util::IJson *jo);

 public:
    bool preorder(const IR::PackageBlock* b) override;
    bool preorder(const IR::ControlBlock* b) override;

    explicit ControlConverter(Backend *backend) : backend(backend),
        refMap(backend->getRefMap()), typeMap(backend->getTypeMap()),
        conv(backend->getExpressionConverter()), json(backend->json)
    { setName("Control"); }
};

class ChecksumConverter : public Inspector {
    Backend* backend;
 public:
    bool preorder(const IR::PackageBlock* b) override;
    bool preorder(const IR::ControlBlock* b) override;
    explicit ChecksumConverter(Backend *backend) : backend(backend)
    { setName("UpdateChecksum"); }
};

class ConvertControl final : public PassManager {
 public:
    explicit ConvertControl(Backend *backend) {
        passes.push_back(new ControlConverter(backend));
        setName("ConvertControl");
    }
};

}  // namespace SAI

#endif  /* _BACKENDS_SAI_CONTROL_H_ */
