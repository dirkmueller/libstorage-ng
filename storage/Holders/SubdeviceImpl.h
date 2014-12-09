#ifndef SUBDEVICE_IMPL_H
#define SUBDEVICE_IMPL_H


#include "storage/Holders/Subdevice.h"
#include "storage/Holders/HolderImpl.h"


namespace storage_bgl
{

    class Subdevice::Impl : public Holder::Impl
    {
    public:

	Impl()
	    : Holder::Impl() {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

    };

}

#endif
