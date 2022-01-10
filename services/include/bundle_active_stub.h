#ifndef BUNDLE_ACTIVE_STUB_H
#define BUNDLE_ACTIVE_STUB_H

#include "bundle_active_iservice.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveStub : public IRemoteStub<IBundleActiveService> {
public:
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel &reply, MessageOption &option) override;

public:
    BundleActiveStub() {};
    ~BundleActiveStub() {};
};

}
}
#endif
