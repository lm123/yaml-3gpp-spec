# yaml-3gpp-spec
Generate nlohmann::json file based on Yaml file

Build procedure:
1. build libyaml-cpp.a 
   cd yaml-cpp-master
   mkdir build
   cd build
   cmake ..
   make

2. build DiffYaml
   make

How to run?
   DiffYaml 29540 h60 f40

Example output:

29540 h60<-->f40 ---- TS29540_Nsmsf_SMService.yaml
/ue-contexts/{supi}  put  responses Added:
        307 308
/ue-contexts/{supi}  delete  responses Added:
        307 308
/ue-contexts/{supi}/sendsms  post  responses Added:
        307 308
paths Added:
        /ue-contexts/{supi}/send-mt-sms
UeSmsContextData  properties Added:
        additionalAccessType hNwPubKeyId ratType additionalRatType supportedFeatures
------ TS29540_Nsmsf_SMService.yaml ------
struct UeSmsContextData {
        std::shared_ptr<std::string> additionalAccessType;
        std::shared_ptr<int> hNwPubKeyId;
        std::shared_ptr<std::string> ratType;
        std::shared_ptr<std::string> additionalRatType;
        std::shared_ptr<std::string> supportedFeatures;
}
inline void from_json(const json & j, LS_SBI::UeSmsContextData& x) {
        x.additionalAccessType = LS_SBI::get_optional<std::string>(j, "additionalAccessType");
        x.hNwPubKeyId = LS_SBI::get_optional<int>(j, "hNwPubKeyId");
        x.ratType = LS_SBI::get_optional<std::string>(j, "ratType");
        x.additionalRatType = LS_SBI::get_optional<std::string>(j, "additionalRatType");
        x.supportedFeatures = LS_SBI::get_optional<std::string>(j, "supportedFeatures");
}
inline void to_json(const json & j, LS_SBI::UeSmsContextData& x) {
        j = json::object();
        if (x.additionalAccessType) j["additionalAccessType"] = x.additionalAccessType;
        if (x.hNwPubKeyId) j["hNwPubKeyId"] = x.hNwPubKeyId;
        if (x.ratType) j["ratType"] = x.ratType;
        if (x.additionalRatType) j["additionalRatType"] = x.additionalRatType;
        if (x.supportedFeatures) j["supportedFeatures"] = x.supportedFeatures;
}
